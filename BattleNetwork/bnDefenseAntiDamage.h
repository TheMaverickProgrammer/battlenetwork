#pragma once
#include <functional>
#include "bnDefenseRule.h"

/**
 * @class DefenseAntiDamage
 * @author mav
 * @date 05/05/19
 * @brief AntiDamage is a rule that blocks certain attacks in the attack resolution step
 * @see NinjaAntiDamage.h
 * 
 * This is used by the NinjaAntiDamage component that adds the rule to the entity
 * and spawns a NinjaStar when the callback is triggered
 * 
 * You can create more anti damage chips using this rule
 */
class DefenseAntiDamage : public DefenseRule {
public:
  typedef std::function<void(Spell* in, Character* owner)> Callback;

private:
  Callback callback; /*!< Runs when the antidefense is triggered */

public:
  /**
   * @brief sets callback
   * @param callback
   */
  DefenseAntiDamage(Callback callback);

  virtual ~DefenseAntiDamage();

  /**
   * @brief If the attack does > 10 units of impact damage, triggers the callback
   * @param in attack spell
   * @param owner the character with antidamage defense (this) added 
   * @return true if triggered, false, if not
   */
  virtual const bool Check(Spell* in, Character* owner);
};
