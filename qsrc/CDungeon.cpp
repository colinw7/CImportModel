#include <CDungeon.h>
#include <CDungeonXML.h>

CDungeonRoom *
CDungeonFactory::
createRoom(CDungeon *dungeon, const CIPoint2D &point)
{
  auto *room = new CDungeonRoom(dungeon, point);

  return room;
}

CDungeonWall *
CDungeonFactory::
createWall(CDungeonRoom *room, CCompassType pos)
{
  auto *wall = new CDungeonWall(room, pos);

  return wall;
}

CDungeonFloor *
CDungeonFactory::
createFloor(CDungeonRoom *room)
{
  auto *floor = new CDungeonFloor(room);

  return floor;
}

CDungeonCeiling *
CDungeonFactory::
createCeiling(CDungeonRoom *room)
{
  auto *ceiling = new CDungeonCeiling(room);

  return ceiling;
}

CDungeonPlayer *
CDungeonFactory::
createPlayer(CDungeon *dungeon)
{
  auto *player = new CDungeonPlayer(dungeon);

  return player;
}

//--------

CDungeon::
CDungeon()
{
  factory_ = new CDungeonFactory;
}

void
CDungeon::
init()
{
  player_ = createPlayer(this);

  updateRooms();
}

CDungeonRoom *
CDungeon::
createRoom(CDungeon *dungeon, const CIPoint2D &pos)
{
  return factory_->createRoom(dungeon, pos);
}

CDungeonWall *
CDungeon::
createWall(CDungeonRoom *room, CCompassType type)
{
  return factory_->createWall(room, type);
}

CDungeonFloor *
CDungeon::
createFloor(CDungeonRoom *room)
{
  return factory_->createFloor(room);
}

CDungeonCeiling *
CDungeon::
createCeiling(CDungeonRoom *room)
{
  return factory_->createCeiling(room);
}

CDungeonPlayer *
CDungeon::
createPlayer(CDungeon *dungeon)
{
  return factory_->createPlayer(dungeon);
}

CDungeonRoom *
CDungeon::
getRoomAtPoint(const CIPoint2D &world_pos)
{
  CIPoint2D pos, room_pos;

  if (! fromWorldPos(world_pos, pos, room_pos))
    return NULL;

  return getRoom(pos);
}

CDungeonRoom *
CDungeon::
getRoom(const CIPoint2D &point)
{
  int ind = point.y*int(cols_) + point.x;

  if (ind < 0 || ind >= int(rooms_.size()))
    return NULL;

  return rooms_[uint(ind)];
}

void
CDungeon::
updateRooms()
{
  uint num = rows_*cols_;

  for_each(rooms_.begin(), rooms_.end(), CDeletePointer());

  rooms_.resize(num);

  for (uint i = 0, y = 0; y < rows_; ++y) {
    for (uint x = 0; x < cols_; ++x, ++i) {
      rooms_[i] = createRoom(this, CIPoint2D(int(x), int(y)));
    }
  }
}

void
CDungeon::
save(const std::string &fileName)
{
  CDungeonXML xml;

  xml.save(fileName, this);
}

void
CDungeon::
load(const std::string &fileName)
{
  CDungeonXML xml;

  xml.load(fileName, this);
}

//--------

CDungeonRoom::
CDungeonRoom(CDungeon *dungeon, const CIPoint2D &pos) :
 dungeon_(dungeon), pos_(pos)
{
  n_wall_ = WallP(dungeon->createWall(this, CCompassType::NORTH));
  s_wall_ = WallP(dungeon->createWall(this, CCompassType::SOUTH));
  w_wall_ = WallP(dungeon->createWall(this, CCompassType::WEST ));
  e_wall_ = WallP(dungeon->createWall(this, CCompassType::EAST ));

  floor_ = FloorP(dungeon->createFloor(this));

  ceiling_ = CeilingP(dungeon->createCeiling(this));
}

const CIBBox2D &
CDungeonRoom::
getBBox() const
{
  bbox_ = dungeon_->getRoomBBox(pos_);

  return bbox_;
}

CDungeonRoom *
CDungeonRoom::
getNRoom() const
{
  if (pos_.y < int(dungeon_->getNumRows()) - 1)
    return dungeon_->getRoom(CIPoint2D(pos_.x, pos_.y + 1));
  else
    return NULL;
}

CDungeonRoom *
CDungeonRoom::
getSRoom() const
{
  if (pos_.y > 0)
    return dungeon_->getRoom(CIPoint2D(pos_.x, pos_.y - 1));
  else
    return NULL;
}

CDungeonRoom *
CDungeonRoom::
getWRoom() const
{
  if (pos_.x > 0)
    return dungeon_->getRoom(CIPoint2D(pos_.x - 1, pos_.y));
  else
    return NULL;
}

CDungeonRoom *
CDungeonRoom::
getERoom() const
{
  if (pos_.x < int(dungeon_->getNumCols()) - 1)
    return dungeon_->getRoom(CIPoint2D(pos_.x + 1, pos_.y));
  else
    return NULL;
}

//--------

CDungeonPlayer::
CDungeonPlayer(CDungeon *dungeon) :
 dungeon_(dungeon), pos_(0,0), room_pos_(0,0), dir_(CCompassType::NORTH)
{
}

CIPoint2D
CDungeonPlayer::
getWorldPos() const
{
  CIPoint2D world_pos;

  dungeon_->toWorldPos(pos_, room_pos_, world_pos);

  return world_pos;
}


void
CDungeonPlayer::
moveForward()
{
  switch (dir_) {
    case CCompassType::NORTH: moveUp   (); break;
    case CCompassType::SOUTH: moveDown (); break;
    case CCompassType::WEST : moveLeft (); break;
    case CCompassType::EAST : moveRight(); break;
    default: break;
  }
}

void
CDungeonPlayer::
moveBack()
{
  switch (dir_) {
    case CCompassType::NORTH: moveDown (); break;
    case CCompassType::SOUTH: moveUp   (); break;
    case CCompassType::WEST : moveRight(); break;
    case CCompassType::EAST : moveLeft (); break;
    default: break;
  }
}

void
CDungeonPlayer::
moveUp()
{
  if (room_pos_.y < int(dungeon_->getRoomRows()) - 1)
    ++room_pos_.y;
  else {
    if (pos_.y < int(dungeon_->getNumRows()) - 1) {
      CDungeonRoom *room = getRoom();

      if (! room->getNWall()->getVisible()) {
        ++pos_.y;

        room_pos_.y = 0;
      }
    }
  }
}

void
CDungeonPlayer::
moveDown()
{
  if (room_pos_.y > 0)
    --room_pos_.y;
  else {
    if (pos_.y > 0) {
      CDungeonRoom *room = getRoom();

      if (! room->getSWall()->getVisible()) {
        --pos_.y;

        room_pos_.y = int(dungeon_->getRoomRows()) - 1;
      }
    }
  }
}

void
CDungeonPlayer::
moveLeft()
{
  if (room_pos_.x > 0)
    --room_pos_.x;
  else {
    if (pos_.x > 0) {
      CDungeonRoom *room = getRoom();

      if (! room->getWWall()->getVisible()) {
        --pos_.x;

        room_pos_.x = int(dungeon_->getRoomCols()) - 1;
      }
    }
  }
}

void
CDungeonPlayer::
moveRight()
{
  if (room_pos_.x < int(dungeon_->getRoomCols()) - 1)
    ++room_pos_.x;
  else {
    if (pos_.x < int(dungeon_->getNumCols()) - 1) {
      CDungeonRoom *room = getRoom();

      if (! room->getEWall()->getVisible()) {
        ++pos_.x;

        room_pos_.x = 0;
      }
    }
  }
}

void
CDungeonPlayer::
turnLeft()
{
  switch (dir_) {
    case CCompassType::NORTH: dir_ = CCompassType::WEST ; break;
    case CCompassType::WEST : dir_ = CCompassType::SOUTH; break;
    case CCompassType::SOUTH: dir_ = CCompassType::EAST ; break;
    case CCompassType::EAST : dir_ = CCompassType::NORTH; break;
    default: break;
  }
}

void
CDungeonPlayer::
turnRight()
{
  switch (dir_) {
    case CCompassType::NORTH: dir_ = CCompassType::EAST ; break;
    case CCompassType::EAST : dir_ = CCompassType::SOUTH; break;
    case CCompassType::SOUTH: dir_ = CCompassType::WEST ; break;
    case CCompassType::WEST : dir_ = CCompassType::NORTH; break;
    default: break;
  }
}

CDungeonRoom *
CDungeonPlayer::
getRoom()
{
  return dungeon_->getRoom(pos_);
}
