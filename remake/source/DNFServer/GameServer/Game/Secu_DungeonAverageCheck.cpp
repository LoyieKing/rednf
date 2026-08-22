// ============================================================================
// df_game_r 还原 —— Secu_DungeonAverageCheck（从 GameStubs.cpp 迁移，G2 安全域）
// 布局与 ORIG 一致（基类 sizeof 0x10：CParty* +0x00，后 0xc vector 占位）；
// Secu_HackLogCheckByParty : Secu_DungeonAverageCheck（CParty +0x1abc）。
// ORIG 符号（nm -C df_game_r）：
//   _ZN24Secu_DungeonAverageCheck11SetPartyPtrEP6CParty       SetPartyPtr(CParty*)
//   _ZN24Secu_DungeonAverageCheck12dungeonClearEv             dungeonClear()
//   _ZN24Secu_DungeonAverageCheck10dungeonFailEv              dungeonFail()
//   _ZN24Secu_DungeonAverageCheck12startDungeonEv             startDungeon()
//   _ZN24Secu_DungeonAverageCheck18setKillMonsterInfoEPK14MSG_MONSTER_DIEP10map_monster
//   ctor/dtor（0x82a4c4a / 0x82a4c6a，weak）
#include <string.h>
// 各方法当前为安全占位；后续按 Secu 报告精修。符号 T。
// ============================================================================
#include "CParty.h"

// MSG_MONSTER_DIE：内部填充结构（ORIG 0x30 字节）。
struct MSG_MONSTER_DIE
{
    char m_pad[0x30];
};

class Secu_DungeonAverageCheck
{
public:
    Secu_DungeonAverageCheck();
    ~Secu_DungeonAverageCheck();
    void SetPartyPtr(CParty* party);
    void dungeonClear();
    void dungeonFail();
    void startDungeon();
    void setKillMonsterInfo(const MSG_MONSTER_DIE* info, map_monster* monster);

    CParty* m_party;   // +0x00
    char m_pad[0xc];   // +0x04..0x0f（vector<SecKillMonsterInfo> 占位）
};

Secu_DungeonAverageCheck::Secu_DungeonAverageCheck()
    : m_party(0)
{
    // TODO(G2): ORIG 0x82a4c4a —— 清空击杀信息容器。
    memset(m_pad, 0, sizeof(m_pad));
}

Secu_DungeonAverageCheck::~Secu_DungeonAverageCheck()
{
    // TODO(G2): ORIG 0x82a4c6a。
}

void Secu_DungeonAverageCheck::SetPartyPtr(CParty* party)
{
    // TODO(G2): ORIG 0x82794a6 —— 记录队伍指针。
    m_party = party;
}

void Secu_DungeonAverageCheck::dungeonClear()
{
    // TODO(G2): ORIG 0x82799d6 —— 校验平均等级（clear）。
    (void)0;
}

void Secu_DungeonAverageCheck::dungeonFail()
{
    // TODO(G2): ORIG 0x82799f4 —— 校验平均等级（fail）。
    (void)0;
}

void Secu_DungeonAverageCheck::startDungeon()
{
    // TODO(G2): ORIG 0x82794c2 —— 开始副本时初始化。
    (void)0;
}

void Secu_DungeonAverageCheck::setKillMonsterInfo(const MSG_MONSTER_DIE* info,
                                                  map_monster* monster)
{
    // TODO(G2): ORIG 0x82794d6 —— 记录怪物死亡信息。
    (void)info;
    (void)monster;
}