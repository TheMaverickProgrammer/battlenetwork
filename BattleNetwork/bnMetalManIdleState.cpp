#include "bnMetalManMoveState.h"
#include "bnMetalMan.h"


MetalManIdleState::MetalManIdleState() : cooldown(0.8f), AIState<MetalMan>()
{
}


MetalManIdleState::~MetalManIdleState()
{
}

void MetalManIdleState::OnEnter(MetalMan& metal) {
  cooldown = 1.2f;
  metal.GetFirstComponent<AnimationComponent>()->SetAnimation(MOB_IDLE, Animator::Mode::Loop);
}

void MetalManIdleState::OnUpdate(float _elapsed, MetalMan& metal) {
  cooldown -= _elapsed;

  // printf("cooldown: %f", cooldown);

  if (cooldown < 0) {
    metal.GoToNextState();
  }
}

void MetalManIdleState::OnLeave(MetalMan& metal) {}