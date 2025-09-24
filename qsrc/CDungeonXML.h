#ifndef CDUNGEON_XML
#define CDUNGEON_XML

class CDungeon;

class CDungeonXML {
 public:
  bool load(const std::string &fileName, CDungeon *dungeon);
  bool save(const std::string &fileName, CDungeon *dungeon);
};

#endif
