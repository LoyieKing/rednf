#ifndef DNF_GAME_BOSS_TOWER_FUNC_H_
#define DNF_GAME_BOSS_TOWER_FUNC_H_

// ============================================================================
// BossTowerFunc.h —— Global::CBossTowerFunc 权威声明
//
// ORIG 8 方法（docs/class_func_reports/Global__CBossTowerFunc/README.md），
// 均为 namespace Global 内静态语义函数（asm 不使用 this）。本批落地
// UseItem / SelectDungeon（定义 BossTowerFunc.cpp）；其余声明先行、定义待批次。
// ============================================================================

class CUser;
class CParty;
class Inven_Item;

// ORIG 全局枚举（mangled 17ENUM_DUNGEON_TYPE；权威定义 CParty.h，同守卫幂等复制）
#ifndef DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
#define DNF_ENUM_ENUM_DUNGEON_TYPE_DEFINED
enum ENUM_DUNGEON_TYPE
{
    ENUM_DUNGEON_TYPE_0 = 0
};
#endif

namespace Global
{

class CBossTowerFunc
{
public:
    // ORIG T 0x08146c20：state==5 → party->useItem；state==12(0xc) →
    // getBossTower()->handleUseItem(item idx, ITEM_TYPE)；否则 0。
    static bool UseItem(CUser* user, const Inven_Item& item);

    // ORIG T 0x08146722：getBossTower() 空则 trace 后 0；按队伍人数映射难度槽
    // (1→0,2→1,3→2,4→3)；handleStartGame 失败时 _destroy。
    static bool SelectDungeon(CParty* party, short dungeonIdx, ENUM_DUNGEON_TYPE type);

    // ---- 声明先行（DisP_GroupA 调用；定义待批次）----
    static char FinishLoad(CUser* user);                        // ORIG T 0x08146916
    static char MoveMap(CUser* user, unsigned short* mapIndex); // ORIG T 0x08146ba2
    static char DieCharacInDungeon(CUser* user);                // ORIG T 0x08146824（DisP_GroupG 调用）
    static void GiveUpGame(CUser* user);                        // ORIG T 0x08146ad8（恒返回 1；DisP_GroupG 调用）
};

}  // namespace Global

#endif  // DNF_GAME_BOSS_TOWER_FUNC_H_
