#include "LogManager.h"
// ============================================================================
// df_game_r Secu_HackLogCheckByParty 还原（G2-5 批次，自包含 TU）
// 逐函数对照 docs/class_func_reports/Secu_HackLogCheckByParty.md 与 ORIG 反汇编。
// 布局（ORIG ctor/dtor/Init 推导，sizeof 0x14，CParty +0x1abc）：
//   +0x00 Secu_DungeonAverageCheck 基类（0x10：CParty* + vector 占位）
//   +0x10 CParty* m_party2
// 跨类/基类调用一律 extern "C" asm("_ZN...")，mangled 名与 ORIG 完全一致。
// ============================================================================

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

// ============================================================================
// 跨类调用（asm-label extern，mangled 名与 ORIG 一致）
// ============================================================================
extern "C" char sub_CParty_IsFirstMapClear(void* self)
    asm("_ZN6CParty15IsFirstMapClearEv");
extern "C" void sub_CParty_SetFirstMapClear(void* self, bool b)
    asm("_ZN6CParty16SetFirstMapClearEb");
extern "C" char sub_CParty_IsAutoCreated(void* self)
    asm("_ZN6CParty13IsAutoCreatedEv");
extern "C" int sub_CParty_get_member_count(void* self)
    asm("_ZN6CParty16get_member_countEv");

extern "C" unsigned int sub_CUser_get_acc_id(const void* self)
    asm("_ZNK5CUser10get_acc_idEv");
extern "C" void* sub_CUser_getHackAnalyzer(void* self)
    asm("_ZN5CUser15getHackAnalyzerEv");

extern "C" void* sub_G_CDataManager()
    asm("_Z14G_CDataManagerv");
extern "C" void* sub_CDataManager_find_map(void* self, int idx)
    asm("_ZNK12CDataManager8find_mapEi");
extern "C" char sub_CMap_checkFitInPathObject(void* self, int x, int y)
    asm("_ZNK4CMap20checkFitInPathObjectEii");

extern "C" char* sub_NumberToString(unsigned int value, int radix)
    asm("_Z14NumberToStringji");
extern "C" void sub_CHackAnalyzer_addServerHackCnt(void* self, void* user,
                                                   int hackType,
                                                   unsigned int a, unsigned int b,
                                                   unsigned int c)
    asm("_ZN8WongWork13CHackAnalyzer16addServerHackCntEP5CUserNS_13ENUM_HACKTYPEEjjj");

// ============================================================================
// Secu_HackLogCheckByParty（sizeof 0x14）
// ============================================================================
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

// ============================================================================
// 实现
// ============================================================================

Secu_HackLogCheckByParty::Secu_HackLogCheckByParty()
{
    m_party2 = 0;
}

Secu_HackLogCheckByParty::~Secu_HackLogCheckByParty()
{
}

void Secu_HackLogCheckByParty::Init(CParty* party)
{
    m_party2 = party;
    Secu_DungeonAverageCheck::SetPartyPtr(party);
}

void Secu_HackLogCheckByParty::DieMob(const MSG_MONSTER_DIE* msg,
                                      map_monster* mob)
{
    if (msg->m_fieldF != -1)
    {
        Secu_DungeonAverageCheck::setKillMonsterInfo(msg, mob);
    }
}

void Secu_HackLogCheckByParty::CheckMoveMapFitInPathHackCnt(
    CUser* user, CParty* party, unsigned int a, unsigned int b, int mapIdx)
{
    if (a == 0)
    {
        return;
    }
    if (party == 0)
    {
        return;
    }

    if (sub_CParty_IsFirstMapClear(party) != 1)
    {
        sub_CParty_SetFirstMapClear(party, true);
        return;
    }

    void* map = sub_CDataManager_find_map(sub_G_CDataManager(), mapIdx);
    bool isHack;
    if ((((sub_CParty_IsAutoCreated(party) == 0) &&
          (sub_CParty_get_member_count(party) != 1)) ||
         (map == 0)) ||
        (sub_CMap_checkFitInPathObject(map, (int)a, (int)b) == 1))
    {
        isHack = false;
    }
    else
    {
        isHack = true;
    }

    if (isHack)
    {
        unsigned int acc = sub_CUser_get_acc_id(user);
        char* accStr = sub_NumberToString(acc, 0);
        LogManager::logFormat(
            1, "Secu_HackLogCheckByParty.cpp",
            "void Secu_HackLogCheckByParty::CheckMoveMapFitInPathHackCnt(CUser*, CParty*, unsigned int, unsigned int, int)",
            0x2c, "Path gate position error. (User: %s %d,%d)", accStr, a, b);
        void* analyzer = sub_CUser_getHackAnalyzer(user);
        sub_CHackAnalyzer_addServerHackCnt(analyzer, user, 0x1f7, 1, a, b);
    }
}

void Secu_HackLogCheckByParty::startDungeon()
{
    Secu_DungeonAverageCheck::startDungeon();
}

void Secu_HackLogCheckByParty::dungeonClear()
{
    Secu_DungeonAverageCheck::dungeonClear();
}

void Secu_HackLogCheckByParty::dungeonFail()
{
    Secu_DungeonAverageCheck::dungeonFail();
}
