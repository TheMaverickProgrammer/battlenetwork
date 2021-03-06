#include "bnMetalManPunchState.h"
#include "bnMetalMan.h"
#include "bnHitbox.h"
#include "bnObstacle.h"
#include "bnAudioResourceManager.h"

MetalManPunchState::MetalManPunchState() : AIState<MetalMan>()
{
}


MetalManPunchState::~MetalManPunchState()
{
}

void MetalManPunchState::OnEnter(MetalMan& metal) {
  auto tile = metal.GetTarget()? metal.GetTarget()->GetTile() : nullptr;
  if (!tile) {
    metal.GoToNextState();
    return;
  }

  auto nextTile = metal.GetField()->GetAt(tile->GetX() + 1, tile->GetY());

  if (nextTile) {
    auto lastTile = metal.GetTile();
    nextTile->ReserveEntityByID(metal.GetID());
    lastTile->ReserveEntityByID(metal.GetID());


    auto onFinish = [metal = &metal, nextTile, lastTile, this]() {
      Logger::Log("metalman move on finish called");

      metal->Teleport(nextTile->GetX(), nextTile->GetY());
      metal->AdoptNextTile();
      metal->FinishMove();

      auto onFinishPunch = [m = metal, lastTile]() { 
        Logger::Log("finish punch called");
        m->Teleport(lastTile->GetX(), lastTile->GetY());
        m->AdoptNextTile();
        m->FinishMove();
        m->GoToNextState(); 
      };
      auto onGroundHit = [this, m = metal]() {       
        Logger::Log("on ground hit called"); 
        this->Attack(*m); 
      };

      metal->SetAnimation("PUNCH", onFinishPunch); // TODO: this is not firing
      metal->SetCounterFrame(1);
      metal->SetCounterFrame(2);
      metal->SetCounterFrame(3);
      metal->OnFrameCallback(4, onGroundHit, Animator::NoCallback, true);

    };

    metal.SetAnimation(MOB_MOVING, onFinish);
  }
  else {
    metal.GoToNextState();
  }

}

void MetalManPunchState::OnLeave(MetalMan& metal) {
  metal.SetAnimation(MOB_IDLE);

}

void MetalManPunchState::OnUpdate(float _elapsed, MetalMan& metal) {

}

void MetalManPunchState::Attack(MetalMan& metal) {
  Battle::Tile* tile = metal.GetField()->GetAt(metal.GetTile()->GetX()-1, metal.GetTile()->GetY());

  if (tile) {
    Entity* next = nullptr;

    Hitbox* hitbox = new Hitbox(metal.field, metal.GetTeam(), 100);
    auto props = hitbox->GetHitboxProperties();
    props.flags |= Hit::flinch;
    props.aggressor = &metal;
    hitbox->SetHitboxProperties(props);

    metal.field->AddEntity(*hitbox, tile->GetX(), tile->GetY());

    if (tile->GetState() != TileState::EMPTY && tile->GetState() != TileState::BROKEN) {
      ENGINE.GetCamera()->ShakeCamera(5.0, sf::seconds(0.5));
      AUDIO.Play(AudioType::PANEL_CRACK);

      if (tile->GetState() == TileState::CRACKED) {
        tile->SetState(TileState::BROKEN);
      }
      else {
        tile->SetState(TileState::CRACKED);
      }
    }
  }
}