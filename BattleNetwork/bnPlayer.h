/*! \brief Player controlled net navi prefab 
 * 
 * This class is the foundation for all player controlled characters
 * To write a new netnavi inherit from this class
 * And simply load new sprites and animation file
 * And change some properties
 */

#pragma once

#include "bnCharacter.h"
#include "bnPlayerState.h"
#include "bnTextureType.h"
#include "bnChargeEffectSceneNode.h"
#include "bnAnimationComponent.h"
#include "bnAI.h"
#include "bnPlayerControlledState.h"
#include "bnPlayerIdleState.h"
#include "bnPlayerHitState.h"
#include "bnPlayerChangeFormState.h"
#include "bnPlayerForm.h"

#include <array>

using sf::IntRect;

class CardAction;

class Player : public Character, public AI<Player> {
  friend class PlayerControlledState;
  friend class PlayerNetworkState;
  friend class PlayerIdleState;
  friend class PlayerHitState;
  friend class PlayerChangeFormState;
  friend class PlayerCardUseListener;

protected:
  bool RegisterForm(PlayerFormMeta* info);

  template<typename T>
  PlayerFormMeta* AddForm();
public:
  using DefaultState = PlayerControlledState;
  static constexpr int MAX_FORM_SIZE = 5;

    /**
   * @brief Loads graphics and adds a charge component
   */
  Player();
  
  /**
   * @brief deconstructor
   */
  virtual ~Player();

  /**
   * @brief Polls for interrupted states and fires delete state when deleted
   * @param _elapsed for secs
   */
  virtual void OnUpdate(float _elapsed);
  
  /**
   * @brief Fires a buster
   */
  void Attack();

  void UseSpecial();

  /**
   * @brief Describe what happens when a player gets hit. Can be overridden.
   */
  virtual void OnHit();

  /**
   * @brief when player is deleted, changes state to delete state and hide charge component
   */
  virtual void OnDelete();

  virtual const float GetHeight() const;

  /**
   * @brief Get how many times the player has moved across the grid
   * @return int
   */
  int GetMoveCount() const;
  
  /**
   * @brief Get how many times the player has been hit
   * @return int
   */
  int GetHitCount() const;

  /**
   * @brief Toggles the charge component
   * @param state
   */
  void SetCharging(bool state);

  /**
   * @brief Set the animation and on finish callback
   * @param _state name of the animation
   */
  void SetAnimation(string _state, std::function<void()> onFinish = nullptr);

  // TODO: these two are hacks and shouldn't belong
  void EnablePlayerControllerSlideMovementBehavior(bool enable = true);
  const bool PlayerControllerSlideEnabled() const;

  virtual CardAction* OnExecuteBusterAction() = 0;
  virtual CardAction* OnExecuteChargedBusterAction() = 0;
  virtual CardAction* OnExecuteSpecialAction() = 0;

  CardAction* ExecuteBuster();
  CardAction* ExecuteChargedBuster();
  CardAction* ExecuteSpecial();

  void ActivateFormAt(int index); 
  void DeactivateForm();
  const bool IsInForm() const;

  const std::vector<PlayerFormMeta*> GetForms();
protected:
  int hitCount; /*!< How many times the player has been hit. Used by score board. */
  string state; /*!< Animation state name */
  bool playerControllerSlide;
  AnimationComponent* animationComponent;
  ChargeEffectSceneNode chargeEffect; /*!< Handles charge effect */

  std::vector<PlayerFormMeta*> forms;
  PlayerForm* activeForm{ nullptr };
};

template<typename T>
PlayerFormMeta* Player::AddForm() {
  PlayerFormMeta* info = new PlayerFormMeta(forms.size()+1);
  info->SetFormClass<T>();
  
  if (!RegisterForm(info)) {
    delete info;
    info = nullptr;
  }

  return info;
}