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
    // b28d-60 绑定判据㉓（W vs T）：ORIG 下列 6 个符号全为 W 弱符号，地址分散
    // 在不同 TU ⇒ 头内定义、使用点发射 W（P233）；我方原为 .cpp 强定义 T。
    // ORIG W 0x08374ff2（sz 0x13）
    CDungeonClear() {}
    // ORIG W 0x08375006（sz 0x13）
    ~CDungeonClear() {}

    // ORIG W 0x08695972（sz 0x13）
    void clear() { m_map.clear(); }
    // ORIG W 0x085bf96c（sz 0x6c）
    char getClearedDungeonDiff(short dungeonIdx) const
    {
        std::map<short, char>::const_iterator it = m_map.find(dungeonIdx);
        if (it == m_map.end())
            return 0;
        return it->second;
    }
    // ORIG W 0x08374f24（sz 0xba）
    void addClearedDungeon(short dungeonIdx, char diff)
    {
        std::map<short, char>::iterator it = m_map.find(dungeonIdx);
        if (it == m_map.end())
            m_map.insert(std::make_pair(dungeonIdx, diff));
        else
            it->second = diff;
    }
    // ORIG W 0x0869595e（sz 0x13）
    unsigned int getDungeonClearSize() const { return m_map.size(); }

    // Binary methods
    // b28d-60：void→int。ORIG 0x08483fae 正常退出路径 08484351 `mov $0x1,%ebx`
    // + 尾 `mov %ebx,%eax`（Ghidra C `return 1;`）；返回类型不入 mangled。
    int AradfromString(CUser* pUser, const char* str);
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
