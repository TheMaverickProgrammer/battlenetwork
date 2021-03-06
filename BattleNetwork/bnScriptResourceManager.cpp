#include "bnScriptResourceManager.h"
#include "bnEntity.h"
#include "bnScriptedCharacter.h"
#include "bnElements.h"
#include "bnScriptedChipAction.h"

// Building the c lib on windows failed. 
// Including the c files directly into source avoids static linking
// until it can be resolved
#include <lapi.c>
#include <lauxlib.c>
#include <lbaselib.c>
#include <lbitlib.c>
#include <lcode.c>
#include <lcorolib.c>
#include <lctype.c>
#include <ldblib.c>
#include <ldebug.c>
#include <ldo.c>
#include <ldump.c>
#include <lfunc.c>
#include <lgc.c>
#include <linit.c>
#include <liolib.c>
#include <llex.c>
#include <lmathlib.c>
#include <lmem.c>
#include <loadlib.c>
#include <lobject.c>
#include <lopcodes.c>
#include <loslib.c>
#include <lparser.c>
#include <lstate.c>
#include <lstring.c>
#include <lstrlib.c>
#include <ltable.c>
#include <ltablib.c>
#include <ltm.c>
#include <lundump.c>
#include <lutf8lib.c>
#include <lvm.c>
#include <lzio.c>

void ScriptResourceManager::ConfigureEnvironment() {
  luaState.open_libraries(sol::lib::base);

  auto battle_namespace = luaState.create_table("Battle");

  // make usertype metatable
  auto character_record = battle_namespace.new_usertype<ScriptedCharacter>("Character",
    sol::constructors<ScriptedCharacter(Character::Rank)>(),
    sol::base_classes, sol::bases<Entity>(),
    "GetName", &Character::GetName,
    "GetID", &Entity::GetID,
    "GetHealth", &Character::GetHealth,
    "GetMaxHealth", &Character::GetMaxHealth,
    "SetHealth", &Character::SetHealth
    );

  // TODO: register animation callback methods
  auto chip_record = battle_namespace.new_usertype<ScriptedChipAction>("ChipAction",
    sol::constructors<ScriptedChipAction(Character*, int)>(),
    sol::base_classes, sol::bases<ChipAction>()
    );

  auto elements_table = battle_namespace.new_enum("Element");
  elements_table["FIRE"] = Element::FIRE;
  elements_table["AQUA"] = Element::AQUA;
  elements_table["ELEC"] = Element::ELEC;
  elements_table["WOOD"] = Element::WOOD;
  elements_table["SWORD"] = Element::SWORD;
  elements_table["WIND"] = Element::WIND;
  elements_table["CURSOR"] = Element::CURSOR;
  elements_table["SUMMON"] = Element::SUMMON;
  elements_table["PLUS"] = Element::PLUS;
  elements_table["BREAK"] = Element::BREAK;
  elements_table["NONE"] = Element::NONE;
  elements_table["ICE"] = Element::ICE;

  try {
    luaState.script(
      "--[[This system EXPECTS type Entity to have a GetID() function"
      "call to return a unique identifier"
      "--]]"

      "Handles = {}"
      "local memoizedFuncs = {}"

      "-- metatable which caches function calls"
      "local mt = {}"
      "mt.__index = function(handle, key)"
      "  if not handle.isValid then"
      "    print(debug.traceback())"
      "    error('Error: handle is not valid!', 2)"
      "  end"

      "  local f = memoizedFuncs[key]"
      "     if not f then"
      "       if handle.classType == 'Entity' then"
      "         f = function(handle, ...) return Battle.Entity[key](handle.cppRef, ...) end"
      "       elseif handle.classType == 'Character' then"
      "         f = function(handle, ...) return Battle.Character[key](handle.cppRef, ...) end"
      "       end"

      "       memoizedFuncs[key] = f"
      "     end"
      "  return f"
      "end"

      "-- exception catcher to avoid problems in C++"
      "function getWrappedSafeFunction(f)"
      "  return function(handle, ...)"
      "    if not handle.isValid then"
      "      print(debug.traceback())"
      "      error('Error: handle is not valid!', 2)"
      "    end"

      "    return f(handle.cppRef, ...)"
      "  end"
      "end"

      "-- To be called at the beginning of an Entity's lifetime"
      "  function createHandle(cppRef, classType)"
      "  local handle = {"
      "      cppRef = cppRef,"
      "      classType = classType,"
      "      isValid = true"
      "  }"

      "-- speedy access without __index call"
      "-- decrease call - time and wraps in an exception - catcher:"
      "-- handle.getName = getWrappedSafeFunction(Entity.getName)"

      "setmetatable(handle, mt)"
      "  Handles[cppRef:GetID()] = handle"
      "  return handle"
      "end"

      "-- To be called at the end of an Entity's lifetime"
      "function onEntityRemoved(cppRef)"
      "  local handle = Handles[cppRef:GetID()];"
      "  handle.cppRef = nil"
      "  handle.isValid = false"
      "  Handles[cppRef:GetID()] = nil"
      "end"
    );
  }
  catch (const sol::error& err) {
    Logger::Logf("[ShaderResourceManager] Something went wrong while configuring the environment: thrown error, %s", err.what());
  }
}

void ScriptResourceManager::AddToPaths(FileMeta pathInfo)
{
  paths.push_back(pathInfo);
}

void ScriptResourceManager::LoadAllSCripts(std::atomic<int>& status)
{
}
