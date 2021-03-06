#pragma once
#include <vector>
#include <iostream>

using std::vector;
using std::cout;
using std::endl;

#include "bnEntity.h"
#include "bnCharacterDeletePublisher.h"

class Character;
class Spell;
class Obstacle;
class Artifact;

namespace Battle {
  class Tile;
}

class Field : public CharacterDeletePublisher {
public:
  
  /**
   * @brief Creates a field _wdith x _height tiles. Sets isBattleActive to false
   */
  Field(int _width, int _height);
  
  /**
   * @brief Deletes tiles
   */
  ~Field();

  /**
   * @brief Get the columns
   * @return columns
   */
  int GetWidth() const;
  
  /**
   * @brief Get the rows
   * @return rows
   */
  int GetHeight() const;

  /**
   * @brief Query for tiles that pass the input function
   * @param query input function, returns true or false based on conditions
   * @return vector of Tile* passing the input
   */
  std::vector<Battle::Tile*> FindTiles(std::function<bool(Battle::Tile* t)> query);

  /**
   * @brief Adds a character using the character's AdoptTile() routine
   * @param character
   * @param x col
   * @param y row
   */
  void AddEntity(Character& character, int x, int y);
  void AddEntity(Character& character, Battle::Tile& dest);

  /**
   * @brief Adds a spell using the spell's AdoptTile() routine
   * @param spell
   * @param x col
   * @param y row
   */
  void AddEntity(Spell& spell, int x, int y);
  void AddEntity(Spell& spell, Battle::Tile& dest);

  /**
   * @brief Adds an obstacle using the obstacle's AdoptTile() routine
   * @param obst
   * @param x col
   * @param y row
   */
  void AddEntity(Obstacle& obst, int x, int y);
  void AddEntity(Obstacle& obst, Battle::Tile& dest);

  /**
   * @brief Adds an artifact using the artifact's AdoptTile() routine
   * @param art
   * @param x col
   * @param y row
   */
  void AddEntity(Artifact& art, int x, int y);
  void AddEntity(Artifact& art, Battle::Tile& dest);

  /**
   * @brief Query for entities on the entire field
   * @param e the query input function
   * @return list of Entity* that passed the input function's conditions
   */
  std::vector<Entity*> FindEntities(std::function<bool(Entity* e)> query);

  /**
   * @brief Set the tile at (x,y) team to _team
   * @param _x
   * @param _y
   * @param _team
   */
  void SetAt(int _x, int _y, Team _team);
  
  /**
   * @brief Get the tile at (x,y)
   * @param _x col
   * @param _y row
   * @return null if x < 1 or x > 6 or y < 1 or y > 3, otherwise returns Tile*
   */
  Battle::Tile* GetAt(int _x, int _y) const;

  /**
   * @brief Updates all tiles
   * @param _elapsed in seconds
   */
  void Update(float _elapsed);
  
  /**
   * @brief Propagates the state to all tiles for specific behavior
   * @param state whether or not the battle is ongoing
   */
  void SetBattleActive(bool state);

  /**
  * @brief Removes any pending entities that have not been added back to the field 
  * @param pointer to the tile 
  * @param ID long ID references the entity
  */
  void TileRequestsRemovalOfQueued(Battle::Tile*, long ID);

private:

  bool isBattleActive; /*!< State flag if battle is over */
  int width; /*!< col */
  int height; /*!< rows */
  bool isUpdating; /*!< enqueue entities if added in the update loop */

  struct queueBucket {
    int x;
    int y;
    long ID;

    enum class type : int {
      character,
      spell,
      obstacle,
      artifact
    } entity_type;

    union type_data {
      Character* character;
      Spell* spell;
      Obstacle* obstacle;
      Artifact* artifact;
    } data;

    queueBucket(int x, int y, Character& d);

    queueBucket(int x, int y, Obstacle& d);

    queueBucket(int x, int y, Artifact& d);

    queueBucket(int x, int y, Spell& d);

    queueBucket(const queueBucket& rhs) = default;
  };

  vector<queueBucket> pending;

  vector<vector<Battle::Tile*>> tiles; /*!< Nested vector to make calls via tiles[x][y] */
};