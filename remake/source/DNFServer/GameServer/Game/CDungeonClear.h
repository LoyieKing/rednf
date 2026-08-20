#ifndef __CDungeonClear_h__
#define __CDungeonClear_h__

#include <map>
#include <string>

class CUser;

namespace WongWork
{

class CDungeonClear
{
public:
    CDungeonClear();
    ~CDungeonClear();

    void clear();
    char getClearedDungeonDiff(short dungeonIdx) const;
    void addClearedDungeon(short dungeonIdx, char diff);
    unsigned int getDungeonClearSize() const;

    // Binary methods
    void AradfromString(CUser* pUser, const char* str);
    void enumDungeonClear(CUser* pUser, bool (*fn)(short, char, char*), ...) const;
    void fromString(CUser* pUser, char* str);
    void toString(char* buf, unsigned int bufSize) const;
    void toString(std::string& out, int nUnknown) const;

private:
    std::map<short, char> m_map;  // +0x00
};

}

#endif // __CDungeonClear_h__
