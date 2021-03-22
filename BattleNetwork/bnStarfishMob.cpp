#include "bnStarfishMob.h"
#include "bnMettaur.h"
#include "bnField.h"
#include "bnWebClientMananger.h"
#include "bnCardUUIDs.h"
#include "bnFadeInState.h"

StarfishMob::StarfishMob(Field* field) : MobFactory(field)
{
}


StarfishMob::~StarfishMob()
{
}

Mob* StarfishMob::Build() {
  Mob* mob = new Mob(field);
  mob->RegisterRankedReward(1, BattleItem(WEBCLIENT.MakeBattleCardFromWebCardData(BuiltInCards::YoYo_M)));

  auto spawner = mob->CreateSpawner<Starfish>();
  spawner.SpawnAt<FadeInState>(4 + (rand() % 3), 1);
  spawner.SpawnAt<FadeInState>(4 + (rand() % 3), 3);

  bool allIce = !(rand() % 10);

  for (auto t : field->FindTiles([](Battle::Tile* t) { return true; })) {
    if (allIce) {
      t->SetState(TileState::ice);
    }
    else {
      if (t->GetX() <= 3 && t->GetX() != 1) {
        if (t->GetY() == 2) {
          t->SetState(TileState::directionRight);
        }
      }
    }
  }

  return mob;
}
