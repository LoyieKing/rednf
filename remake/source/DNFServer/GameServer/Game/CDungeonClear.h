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
    int toString(char* buf, unsigned int bufSize) const;
    // b28d-60：void→int。ORIG 0x084846d4 尾 `mov -0x20(%ebp),%eax`（返回 written）
    // + empty 分支 `mov $0x0,%eax`；返回类型不入 mangled。全仓调用点 0（仅本 TU）。
    int toString(std::string& out, int nUnknown) const;

private:
    std::map<short, char> m_map;  // +0x00
};

}

#endif // __CDungeonClear_h__
