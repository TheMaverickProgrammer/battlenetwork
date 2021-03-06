#include "bnStarfishMob.h"
#include "bnMettaur.h"
#include "bnField.h"
#include "bnSpawnPolicy.h"
#include "bnChipsSpawnPolicy.h"

StarfishMob::StarfishMob(Field* field) : MobFactory(field)
{
}


StarfishMob::~StarfishMob()
{
}

Mob* StarfishMob::Build() {
  Mob* mob = new Mob(field);
  mob->RegisterRankedReward(1, BattleItem(Chip(75, 147, 'R', 30, Element::NONE, "Recov30", "Recover 30HP", "", 1)));
  mob->RegisterRankedReward(11, BattleItem(Chip(81, 153, 'R', 300, Element::NONE, "Recov300", "Recover 300HP", "", 5)));

  mob->Spawn<Rank1<Starfish>>(4 + (rand() % 3), 1);
  mob->Spawn<Rank1<Starfish>>(4 + (rand() % 3), 3);

  bool allIce = !(rand() % 10);

  for (auto t : field->FindTiles([](Battle::Tile* t) { return true; })) {
    if (allIce) {
      t->SetState(TileState::ICE);
    }
    else {
      if (t->GetX() == 3) {
        if (t->GetY() == 1 || t->GetY() == 3) {
          t->SetState(TileState::DIRECTION_LEFT);
        }
      }
    }
  }

  return mob;
}
