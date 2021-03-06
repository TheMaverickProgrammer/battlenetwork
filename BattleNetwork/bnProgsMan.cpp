#include "bnProgsMan.h"
#include "bnTile.h"
#include "bnField.h"
#include "bnWave.h"
#include "bnProgBomb.h"
#include "bnTextureResourceManager.h"
#include "bnAudioResourceManager.h"
#include "bnShaderResourceManager.h"
#include "bnEngine.h"
#include "bnNaviExplodeState.h"

#define RESOURCE_PATH "resources/mobs/progsman/progsman.animation"

ProgsMan::ProgsMan(Rank _rank)
  :
    AI<ProgsMan>(this), Character(_rank) {
  name = "ProgsMan";
  this->team = Team::BLUE;
  SetHealth(1200);

  if (_rank == ProgsMan::Rank::EX) {
    SetHealth(2500);
  }

  setTexture(*TEXTURES.GetTexture(TextureType::MOB_PROGSMAN_ATLAS));
  setScale(2.f, 2.f);

  this->SetHealth(health);

  //Components setup and load

  animationComponent = new AnimationComponent(this);
  this->RegisterComponent(animationComponent);
  animationComponent->Setup(RESOURCE_PATH);
  animationComponent->Reload();
  animationComponent->SetAnimation(MOB_IDLE);

  animationComponent->OnUpdate(0);
}

ProgsMan::~ProgsMan() {
}

void ProgsMan::OnFrameCallback(int frame, std::function<void()> onEnter, std::function<void()> onLeave, bool doOnce) {
  animationComponent->AddCallback(frame, onEnter, onLeave, doOnce);
}

void ProgsMan::OnUpdate(float _elapsed) {
  setPosition(tile->getPosition().x + this->tileOffset.x, tile->getPosition().y + this->tileOffset.y);

  this->AI<ProgsMan>::Update(_elapsed);
}

void ProgsMan::OnDelete() {
  this->SetAnimation(MOB_HIT);
  this->ChangeState<NaviExplodeState<ProgsMan>>(); // freezes animation
}

const bool ProgsMan::OnHit(const Hit::Properties props) {
  if ((props.flags & Hit::recoil) == Hit::recoil) {
    FinishMove();
    this->ChangeState<ProgsManHitState>();
  }

  return true;
}

const float ProgsMan::GetHeight() const {
  return 64.0f;
}

void ProgsMan::SetCounterFrame(int frame)
{
  auto onFinish = [&]() { this->ToggleCounter(); };
  auto onNext = [&]() { this->ToggleCounter(false); };
  animationComponent->AddCallback(frame, onFinish, onNext);
}

void ProgsMan::SetAnimation(string _state, std::function<void()> onFinish) {
  animationComponent->SetAnimation(_state, onFinish);
  animationComponent->OnUpdate(0);
}
