#ifndef CDUNGEON_H
#define CDUNGEON_H

#include <CIPoint2D.h>
#include <CIBBox2D.h>
#include <CFuncs.h>
#include <CCompassType.h>
#include <memory>

class CDungeon;
class CDungeonRoom;
class CDungeonWall;
class CDungeonFloor;
class CDungeonCeiling;
class CDungeonPlayer;

class CDungeonFactory {
 public:
  CDungeonFactory() { }

  virtual ~CDungeonFactory() { }

  virtual CDungeonRoom    *createRoom   (CDungeon *dungeon, const CIPoint2D &pos);
  virtual CDungeonWall    *createWall   (CDungeonRoom *room, CCompassType type);
  virtual CDungeonFloor   *createFloor  (CDungeonRoom *room);
  virtual CDungeonCeiling *createCeiling(CDungeonRoom *room);
  virtual CDungeonPlayer  *createPlayer (CDungeon *dungeon);
};

//---

class CDungeonTexture {
 public:
  CDungeonTexture() { }

  virtual ~CDungeonTexture() { }

 protected:
  char *dummy { nullptr };
};

//---

class CDungeonWall {
 public:
  CDungeonWall(CDungeonRoom *room, CCompassType type) :
   room_(room), type_(type) {
  }

  virtual ~CDungeonWall() { }

  //! get set wall is visible
  bool getVisible() { return visible_; }
  void setVisible(bool visible) { visible_ = visible; }

 protected:
  CDungeonRoom*   room_ { nullptr };
  CCompassType    type_;
  bool            visible_ { true };
  CDungeonTexture texture_;
};

//---

class CDungeonFloor {
 public:
  CDungeonFloor(CDungeonRoom *room) :
   room_(room) {
  }

  virtual ~CDungeonFloor() { }

 protected:
  CDungeonRoom*   room_ { nullptr };
  CDungeonTexture texture_;
};

//---

class CDungeonCeiling {
 public:
  CDungeonCeiling(CDungeonRoom *room) :
   room_(room) {
  }

  virtual ~CDungeonCeiling() { }

 protected:
  CDungeonRoom*   room_ { nullptr };
  CDungeonTexture texture_;
};

//---

class CDungeonRoom {
 public:
  CDungeonRoom(CDungeon *dungeon, const CIPoint2D &pos);

  virtual ~CDungeonRoom() { }

  const CIPoint2D &getPos() const { return pos_; }

  const CIBBox2D &getBBox() const;

  CDungeonWall *getNWall() const { return n_wall_.get(); }
  CDungeonWall *getSWall() const { return s_wall_.get(); }
  CDungeonWall *getWWall() const { return w_wall_.get(); }
  CDungeonWall *getEWall() const { return e_wall_.get(); }

  CDungeonWall *getWall(CCompassType type) const {
    switch (type) {
      case CCompassType::NORTH: return getNWall();
      case CCompassType::SOUTH: return getSWall();
      case CCompassType::WEST : return getWWall();
      case CCompassType::EAST : return getEWall();
      default                 : return nullptr;
    }
  }

  CDungeonWall *getOppositeWall(CCompassType type) const {
    switch (type) {
      case CCompassType::NORTH: return getSWall();
      case CCompassType::SOUTH: return getNWall();
      case CCompassType::WEST : return getEWall();
      case CCompassType::EAST : return getWWall();
      default                 : return nullptr;
    }
  }

  CDungeonRoom *getNRoom() const;
  CDungeonRoom *getSRoom() const;
  CDungeonRoom *getWRoom() const;
  CDungeonRoom *getERoom() const;

 protected:
  using WallP    = std::unique_ptr<CDungeonWall>;
  using FloorP   = std::unique_ptr<CDungeonFloor>;
  using CeilingP = std::unique_ptr<CDungeonCeiling>;

  CDungeon* dungeon_ { nullptr };
  CIPoint2D pos_;
  WallP     n_wall_;
  WallP     s_wall_;
  WallP     w_wall_;
  WallP     e_wall_;
  FloorP    floor_;
  CeilingP  ceiling_;

  mutable CIBBox2D bbox_;
};

//---

class CDungeonPlayer {
 public:
  CDungeonPlayer(CDungeon *dungeon);

  virtual ~CDungeonPlayer() { }

  const CIPoint2D &getPos() const { return pos_; }

  const CIPoint2D &getRoomPos() const { return room_pos_; }

  void setPos(const CIPoint2D &pos, const CIPoint2D &room_pos) {
    pos_      = pos;
    room_pos_ = room_pos;
  }

  CIPoint2D getWorldPos() const;

  CDungeonRoom *getRoom();

  CCompassType getDirection() const { return dir_; }
  void setDirection(CCompassType dir) { dir_ = dir; }

  void moveUp   ();
  void moveDown ();
  void moveLeft ();
  void moveRight();

  void moveForward();
  void moveBack   ();
  void turnLeft   ();
  void turnRight  ();

 private:
  CDungeon*    dungeon_ { nullptr };
  CIPoint2D    pos_;
  CIPoint2D    room_pos_;
  CCompassType dir_;
};

//------

class CDungeon {
 public:
  using RoomList = std::vector<CDungeonRoom *>;

 public:
  CDungeon();

  virtual ~CDungeon() { }

  void init();

  virtual CDungeonRoom*    createRoom(CDungeon *dungeon, const CIPoint2D &pos);
  virtual CDungeonWall*    createWall(CDungeonRoom *room, CCompassType type);
  virtual CDungeonFloor*   createFloor(CDungeonRoom *room);
  virtual CDungeonCeiling* createCeiling(CDungeonRoom *room);
  virtual CDungeonPlayer*  createPlayer(CDungeon *dungeon);

  uint getNumRows() const { return rows_; }

  void setNumRows(uint n) {
    if (n < 1 || n == rows_) return;

    rows_ = n;

    updateRooms();
  }

  uint getNumCols() const { return cols_; }

  void setNumCols(uint n) {
    if (n < 1 || n == cols_) return;

    cols_ = n;

    updateRooms();
  }

  void setSize(uint x, uint y) {
    if (x < 1 || y < 1) return;

    if (x == cols_ && y == rows_) return;

    cols_ = x; rows_ = y;

    updateRooms();
  }

  // number of columns per room
  uint getRoomCols() const { return room_x_; }
  void setRoomCols(uint n) { if (n > 1) room_x_ = n; }

  // number of rows per room
  uint getRoomRows() const { return room_y_; }
  void setRoomRows(uint n) { if (n > 1) room_y_ = n; }

  void setRoomSize(uint x, uint y) {
    if (x < 1 || y < 1) return;

    if (x == room_x_ && y == room_y_) return;

    room_x_ = x; room_y_ = y;
  }

  CDungeonPlayer *getPlayer() { return player_; }

  CDungeonRoom *getRoomAtPoint(const CIPoint2D &point);

  CDungeonRoom *getRoom(const CIPoint2D &point);

  const RoomList &getRooms() const { return rooms_; }

  CIBBox2D getBBox() const {
    int x1 = 0;
    int y1 = 0;
    int x2 = int(2*room_x_*cols_);
    int y2 = int(2*room_y_*rows_);

    return CIBBox2D(x1, y1, x2, y2);
  }

  CIBBox2D getRoomBBox(const CIPoint2D &pos) const {
    int x1 = int(2*room_x_)*pos.x;
    int y1 = int(2*room_y_)*pos.y;
    int x2 = x1 + int(2*room_x_);
    int y2 = y1 + int(2*room_y_);

    return CIBBox2D(x1, y1, x2, y2);
  }

  bool toWorldPos(const CIPoint2D &pos, const CIPoint2D &room_pos,
                  CIPoint2D &world_pos) {
    if (pos.x < 0 || pos.x >= int(cols_) || pos.y < 0 || pos.y >= int(rows_))
      return false;

    if (room_pos.x < 0 || room_pos.x >= int(room_x_) ||
        room_pos.y < 0 || room_pos.y >= int(room_y_))
      return false;

    world_pos.x = int(2*room_x_)*pos.x + 2*room_pos.x + 1;
    world_pos.y = int(2*room_y_)*pos.y + 2*room_pos.y + 1;

    return true;
  }

  bool fromWorldPos(const CIPoint2D &world_pos, CIPoint2D &pos,
                    CIPoint2D &room_pos) {
    pos.x = world_pos.x/int(2*room_x_);
    pos.y = world_pos.y/int(2*room_y_);

    room_pos.x = (world_pos.x - int(2*room_x_)*pos.x - 1)/2;
    room_pos.y = (world_pos.y - int(2*room_x_)*pos.y - 1)/2;

    if (pos.x < 0 || pos.x >= int(cols_) || pos.y < 0 || pos.y >= int(rows_))
      return false;

    if (room_pos.x < 0 || room_pos.x >= int(room_x_) ||
        room_pos.y < 0 || room_pos.y >= int(room_y_))
      return false;

    return true;
  }

  void load(const std::string &fileName);
  void save(const std::string &fileName);

 protected:
  void updateRooms();

 protected:
  CDungeonFactory* factory_ { nullptr };
  uint             rows_    { 0 }, cols_ { 0 }; // num rooms (x and y)
  uint             room_x_  { 5 }, room_y_ { 5 }; // num move points in a room (x and y)
  CDungeonPlayer*  player_  { nullptr };
  RoomList         rooms_;
};

#endif
