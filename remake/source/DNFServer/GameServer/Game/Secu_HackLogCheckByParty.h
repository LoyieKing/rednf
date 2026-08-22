// df_game_r Secu_HackLogCheckByParty —— 唯一声明点（Secu_HackLogCheckByParty.cpp 实现；CParty 子对象 +0x1abc，总尺寸 0x14）。
// 布局：+0x00 Secu_DungeonAverageCheck 基类（0x10：CParty* + vector 占位）
//   +0x10 CParty* m_party2
#ifndef GAME_SECU_HACKLOGCHECKBYPARTY_H_
#define GAME_SECU_HACKLOGCHECKBYPARTY_H_

class CParty;
class CUser;
class CMap;
class map_monster;

struct MSG_MONSTER_DIE
{
    char m_pad[0xf];
    short m_fieldF;   // +0x0f
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
    void setKillMonsterInfo(const MSG_MONSTER_DIE* msg, map_monster* mob);

    CParty* m_party;    // +0x00
    char m_pad[0xc];    // +0x04..0x0f（vector<SecKillMonsterInfo> 占位）
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