#ifndef DNF_GAME_VILLAGE_H_
#define DNF_GAME_VILLAGE_H_

// ============================================================================
// df_game_r 还原 —— Village（G2-5 村庄对象，sizeof 0x34）
// 权威声明点：Village.cpp 实现，GameWorld.h/GameWorld.cpp 共用本头。
// 布局依据 ORIG ctor 086c32e0 / set_village 086c33b8（Village.md）：
//   +0x00 int    m_villageId      （set_village 写入 town script +0）
//   +0x04 pad（4）
//   +0x08 map<ushort,CUser*> m_users（0x18 字节，ctor 调 _ZNSt3map...C1Ev）
//   +0x20 bool   m_bEnabled
//   +0x24 int    m_areaCount
//   +0x28 Area*  m_pAreas         （new[] 数组，前 4 字节存元素个数）
//   +0x2c int    m_field2c        （set_village 写入 town script +0x3c）
//   +0x30 int    m_field30        （set_village 写入 town script +0x40）
// ============================================================================

#include <map>
#include <vector>

class CUser;
class TownScript;
class PacketGuard;
class Area;

class Village
{
public:
    Village();
    ~Village();
    void destroy();
    bool set_village(TownScript& script);
    int get_gate_area();
    Area* getArea(int area);
    void get_user_id_list(
        int area,
        std::vector<unsigned short, std::allocator<unsigned short> >& list);
    void send_to_area(int area, PacketGuard& pkt);
    int GetUserCount(int area) const;

    int m_villageId;      // +0x00
    char m_pad04[4];      // +0x04
    std::map<unsigned short, CUser*> m_users;  // +0x08（0x18）
    bool m_bEnabled;      // +0x20
    char m_pad21[3];      // +0x21
    int m_areaCount;      // +0x24
    Area* m_pAreas;       // +0x28
    int m_field2c;        // +0x2c
    int m_questIdx;       // +0x30（CheckMoveTown ORIG 读此作 isClearQuest 检查）
};

#endif  // DNF_GAME_VILLAGE_H_
