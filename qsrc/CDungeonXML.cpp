#include <CDungeon.h>
#include <CDungeonXML.h>

#include <CXML.h>
#include <CFile.h>
#include <CStrUtil.h>
#include <CStrParse.h>

class CXMLDungeonTag;
class CXMLDungeonRoomTag;
class CXMLDungeonWallTag;

class CXMLDungeonFactory : public CXMLFactory {
 public:
  CXMLDungeonFactory(CDungeon *dungeon) :
   CXMLFactory(), dungeon_(dungeon) {
  }

  CDungeon *getDungeon() const { return dungeon_; }

  CXMLTag *createTag(const CXML *xml, CXMLTag *parent, const std::string &name,
                     CXMLTag::OptionArray &options) override;

 private:
  CDungeon *dungeon_;
};

class CXMLDungeonTag : public CXMLTag {
 public:
  CXMLDungeonTag(const CXML *xml, CXMLTag *parent, CDungeon *dungeon, const std::string &name,
                 CXMLTag::OptionArray &options);

  CDungeon *getDungeon() const { return dungeon_; }

 private:
  CDungeon *dungeon_;
};

class CXMLRoomTag : public CXMLTag {
 public:
  CXMLRoomTag(const CXML *xml, CXMLTag *parent, const std::string &name,
              CXMLTag::OptionArray &options);

  CXMLDungeonTag *getDungeon() const { return dungeon_; }

  CDungeonRoom *getRoom() const { return room_; }

 private:
  CXMLDungeonTag *dungeon_;
  CDungeonRoom   *room_;
};

class CXMLWallTag : public CXMLTag {
 public:
  CXMLWallTag(const CXML *xml, CXMLTag *parent, const std::string &name,
              CXMLTag::OptionArray &options);

  CXMLRoomTag *getRoom() const { return room_; }

  CDungeonWall *getWall() const { return wall_; }

 private:
  CXMLRoomTag  *room_;
  CDungeonWall *wall_;
};

class CXMLPlayerTag : public CXMLTag {
 private:
  CXMLDungeonTag *dungeon_;

 public:
  CXMLPlayerTag(const CXML *xml, CXMLTag *parent, const std::string &name,
                CXMLTag::OptionArray &options);

  CXMLDungeonTag *getDungeon() const { return dungeon_; }
};

//------

bool
CDungeonXML::
load(const std::string &fileName, CDungeon *dungeon)
{
  CXML xml;

  CXMLDungeonFactory *factory = new CXMLDungeonFactory(dungeon);

  xml.setFactory(factory);

  CXMLTag *tag;

  if (! xml.read(fileName, &tag))
    return false;

  return true;
}

bool
CDungeonXML::
save(const std::string &fileName, CDungeon *dungeon)
{
  CFile file(fileName);

  file.printf("<dungeon size=\"%d,%d\" room_size=\"%d,%d\">\n",
              dungeon->getNumCols(), dungeon->getNumRows(),
              dungeon->getRoomCols(), dungeon->getRoomRows());

  const CDungeon::RoomList &rooms = dungeon->getRooms();

  CDungeon::RoomList::const_iterator r1 = rooms.begin();
  CDungeon::RoomList::const_iterator r2 = rooms.end  ();

  for ( ; r1 != r2; ++r1) {
    CDungeonRoom *room = *r1;

    const CIPoint2D &pos = room->getPos();

    file.printf("<room pos=\"%d,%d\">\n", pos.x, pos.y);

    CDungeonWall *nwall = room->getWall(CCompassType::NORTH);
    CDungeonWall *swall = room->getWall(CCompassType::SOUTH);
    CDungeonWall *wwall = room->getWall(CCompassType::WEST );
    CDungeonWall *ewall = room->getWall(CCompassType::EAST );

    file.printf("<wall type=\"n\" visible=\"%s\"/>\n",
                CStrUtil::toString(nwall->getVisible()).c_str());
    file.printf("<wall type=\"s\" visible=\"%s\"/>\n",
                CStrUtil::toString(swall->getVisible()).c_str());
    file.printf("<wall type=\"w\" visible=\"%s\"/>\n",
                CStrUtil::toString(wwall->getVisible()).c_str());
    file.printf("<wall type=\"e\" visible=\"%s\"/>\n",
                CStrUtil::toString(ewall->getVisible()).c_str());

    //CDungeonFloor   *floor   = room->getFloor();
    //CDungeonCeiling *ceiling = room->getCeiling();

    file.printf("</room>>\n");
  }

  const CIPoint2D &pos      = dungeon->getPlayer()->getPos    ();
  const CIPoint2D &room_pos = dungeon->getPlayer()->getRoomPos();

  file.printf("<player pos=\"%d,%d\" room_pos=\"%d,%d\"/>\n",
              pos.x, pos.y, room_pos.x, room_pos.y);

  file.printf("</dungeon>\n");

  return true;
}

//-------

CXMLTag *
CXMLDungeonFactory::
createTag(const CXML *xml, CXMLTag *parent, const std::string &name, CXMLTag::OptionArray &options)
{
  if      (name == "dungeon") {
    auto *dungeon_tag = new CXMLDungeonTag(xml, parent, dungeon_, name, options);

    return dungeon_tag;
  }
  else if (name == "room") {
    auto *room = new CXMLRoomTag(xml, parent, name, options);

    return room;
  }
  else if (name == "wall") {
    auto *wall = new CXMLWallTag(xml, parent, name, options);

    return wall;
  }
  else if (name == "player") {
    auto *player = new CXMLPlayerTag(xml, parent, name, options);

    return player;
  }
  else
    return CXMLFactory::createTag(xml, parent, name, options);
}

CXMLDungeonTag::
CXMLDungeonTag(const CXML *xml, CXMLTag *parent, CDungeon *dungeon, const std::string &name,
               CXMLTag::OptionArray &options) :
 CXMLTag(xml, parent, name, options), dungeon_(dungeon)
{
  CXMLTag::OptionArray::const_iterator o1, o2;

  for (o1 = options.begin(), o2 = options.end(); o1 != o2; ++o1) {
    const CXMLTagOption *option = *o1;

    if      (option->getName() == "size") {
      CStrParse pattern(option->getValue());

      int w, h;

      if (pattern.readInteger(&w) && pattern.skipChar(',') && pattern.readInteger(&h))
        dungeon_->setSize(uint(w), uint(h));
    }
    else if (option->getName() == "room_size") {
      CStrParse pattern(option->getValue());

      int w, h;

      if (pattern.readInteger(&w) && pattern.skipChar(',') && pattern.readInteger(&h))
        dungeon_->setRoomSize(uint(w), uint(h));
    }
    else
      std::cerr << "Unhandle option: " << option->getValue() << "\n";
  }
}

CXMLRoomTag::
CXMLRoomTag(const CXML *xml, CXMLTag *parent, const std::string &name,
            CXMLTag::OptionArray &options) :
 CXMLTag(xml, parent, name, options), dungeon_(NULL)
{
  dungeon_ = dynamic_cast<CXMLDungeonTag *>(parent);

  auto *dungeon = dungeon_->getDungeon();

  CXMLTag::OptionArray::const_iterator o1, o2;

  for (o1 = options.begin(), o2 = options.end(); o1 != o2; ++o1) {
    const CXMLTagOption *option = *o1;

    if      (option->getName() == "pos") {
      CStrParse pattern(option->getValue());

      double x, y;

      if (pattern.readReal(&x) && pattern.skipChar(',') && pattern.readReal(&y))
        room_ = dungeon->getRoom(CIPoint2D(int(x), int(y)));
    }
    else
      std::cerr << "Unhandle option: " << option->getValue() << "\n";
  }
}

CXMLWallTag::
CXMLWallTag(const CXML *xml, CXMLTag *parent, const std::string &name,
            CXMLTag::OptionArray &options) :
 CXMLTag(xml, parent, name, options), room_(NULL), wall_(NULL)
{
  room_ = dynamic_cast<CXMLRoomTag *>(parent);

  CDungeonRoom *room = room_->getRoom();

  CXMLTag::OptionArray::const_iterator o1, o2;

  for (o1 = options.begin(), o2 = options.end(); o1 != o2; ++o1) {
    const CXMLTagOption *option = *o1;

    if      (option->getName() == "type") {
      std::string value = CStrUtil::toLower(option->getValue());

      CCompassType type;

      if      (value == "n" || value == "north")
        type = CCompassType::NORTH;
      else if (value == "s" || value == "south")
        type = CCompassType::SOUTH;
      else if (value == "w" || value == "west" )
        type = CCompassType::WEST;
      else if (value == "e" || value == "east" )
        type = CCompassType::EAST;
      else
        continue;

      if (room)
        wall_ = room->getWall(type);
    }
    else if (option->getName() == "visible") {
      bool visible = CStrUtil::toBool(option->getValue());

      if (wall_)
        wall_->setVisible(visible);
    }
    else
      std::cerr << "Unhandle option: " << option->getValue() << "\n";
  }
}

CXMLPlayerTag::
CXMLPlayerTag(const CXML *xml, CXMLTag *parent, const std::string &name,
              CXMLTag::OptionArray &options) :
 CXMLTag(xml, parent, name, options), dungeon_(NULL)
{
  dungeon_ = dynamic_cast<CXMLDungeonTag *>(parent);

  auto *player = dungeon_->getDungeon()->getPlayer();
  if (! player) return;

  for (auto o1 = options.begin(), o2 = options.end(); o1 != o2; ++o1) {
    const CXMLTagOption *option = *o1;

    if      (option->getName() == "pos") {
      CStrParse pattern(option->getValue());

      double x, y;

      if (pattern.readReal(&x) && pattern.skipChar(',') && pattern.readReal(&y))
        player->setPos(CIPoint2D(int(x), int(y)), player->getRoomPos());
    }
    else if (option->getName() == "room_pos") {
      CStrParse pattern(option->getValue());

      double x, y;

      if (pattern.readReal(&x) && pattern.skipChar(',') && pattern.readReal(&y))
        player->setPos(player->getPos(), CIPoint2D(int(x), int(y)));
    }
    else
      std::cerr << "Unhandle option: " << option->getValue() << "\n";
  }
}
