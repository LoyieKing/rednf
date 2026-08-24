// df_game_r Secu_HackLogCheckByParty —— 唯一声明点（Secu_HackLogCheckByParty.cpp 实现；CParty 子对象 +0x1abc，总尺寸 0x14）。
// 布局：+0x00 Secu_DungeonAverageCheck 基类（0x10：CParty* + vector 占位）
//   +0x10 CParty* m_party2
#ifndef GAME_SECU_HACKLOGCHECKBYPARTY_H_
#define GAME_SECU_HACKLOGCHECKBYPARTY_H_

class CParty;
class CUser;
class CMap;
class map_monster;
#include <vector>

// MSG_MONSTER_DIE（网络填充结构，ORIG 访问至少到 +0xa21；
// setKillMonsterInfo 0x82794d6 使用 +0xa1e/+0xa1a 两个计数值——语义推断：
// +0xa1e 为耗时类、+0xa1a 为经验类累计）。
struct MSG_MONSTER_DIE
{
    char m_pad[0xf];
    short m_fieldF;   // +0x0f
    char m_pad11[0xa1a - 0x11];
    unsigned int m_killValue2;   // +0xa1a（经验类，推断）
    unsigned int m_killValue1;   // +0xa1e（耗时类，推断）

    // ---- 以下字段由 cUserHistoryLog::KillMonster（ORIG 0x086851ac）日志读取 ----
    unsigned char m_fieldA22;    // +0xa22
    char m_pad23[6];             // +0xa23
    short m_fieldA29;            // +0xa29
    short m_fieldA2b;            // +0xa2b
    int m_fieldA2d;              // +0xa2d
    int m_fieldA31;              // +0xa31
    short m_fieldA35;            // +0xa35
    short m_fieldA37;            // +0xa37
    short m_fieldA39;            // +0xa39
    short m_fieldA3b;            // +0xa3b
    short m_fieldA3d;            // +0xa3d
};

// SecKillMonsterInfo（0x18；ctor ORIG 随 push_back 内联，字段偏移实证）。
struct SecKillMonsterInfo
{
    SecKillMonsterInfo() : m_monsterId(0), m_count(0), m_sumValue1(0),
                           m_sumValue2(0), m_hackCntE6(0), m_hackCntE7(0) {}
    SecKillMonsterInfo(unsigned int monsterId, unsigned int count,
                       unsigned int v1, unsigned int v2)
        : m_monsterId(monsterId), m_count((int)count), m_sumValue1((int)v1),
          m_sumValue2((int)v2), m_hackCntE6(0), m_hackCntE7(0) {}

    int m_monsterId;     // +0x00
    int m_count;         // +0x04 击杀次数
    int m_sumValue1;     // +0x08 耗时类累计
    int m_sumValue2;     // +0x0c 经验类累计
    int m_hackCntE6;     // +0x10 已上报 0xe6 次数
    int m_hackCntE7;     // +0x14 已上报 0xe7 次数
};
// ---- Secu_DungeonAverageCheck（0x10；ctor/dtor/方法由 ORIG/桩提供）----
class Secu_DungeonAverageCheck
{
public:
    Secu_DungeonAverageCheck();
    ~Secu_DungeonAverageCheck();

    void SetPartyPtr(CParty* party);
    void reset();
    void startDungeon();
    void dungeonClear();
    void dungeonFail();
    std::vector<SecKillMonsterInfo>::iterator
    findKillMonsterInfo(unsigned int monsterId);   // ORIG 0x827993c
    void setKillMonsterInfo(const MSG_MONSTER_DIE* msg, map_monster* mob);

    CParty* m_party;    // +0x00
    std::vector<SecKillMonsterInfo> m_killInfos;   // +0x04..+0x0f（原 pad 占位）


};
// ---- Secu_HackLogCheckByParty（sizeof 0x14）----
class Secu_HackLogCheckByParty : public Secu_DungeonAverageCheck
{
public:
    Secu_HackLogCheckByParty();
    ~Secu_HackLogCheckByParty();

    void Init(CParty* party);
    void DieMob(const MSG_MONSTER_DIE* msg, map_monster* mob);
    void CheckMoveMapFitInPathHackCnt(CUser* user, CParty* party,
                                      unsigned int a, unsigned int b,
                                      int mapIdx);
    void startDungeon();
    void dungeonClear();
    void dungeonFail();

    CParty* m_party2;   // +0x10
};

#endif  // GAME_SECU_HACKLOGCHECKBYPARTY_H_