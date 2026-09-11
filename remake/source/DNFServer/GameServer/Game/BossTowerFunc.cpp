// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BossTowerFunc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "BossTowerFunc.h"

#include "CGameManager.h"   // G_CGameManager / getBossTower() / getBossTower(int)
#include "CBossTower.h"     // WongWork::CBossTower（handleStartGame/_destroy/handleUseItem）
#include "CParty.h"         // CParty::useItem / get_member_count（完整类型）
#include "Inven_Item.h"     // Inven_Item 完整布局（m_field1/m_addInfo 字段化读取）
#include "GameTypes.h"      // cMyTrace

#include "CUser.h"          // CUser 完整类型（get_state/GetParty/getBossTower）

#include "BossTowerFunc/Get_State_Check.inc"


#include "BossTowerFunc/UseItem.inc"


#include "BossTowerFunc/GetItem.inc"


#include "BossTowerFunc/SelectDungeon.inc"
                                           ENUM_DUNGEON_TYPE type)
{
    if (party == 0)
    {
        return false;
    }

    // ORIG +0x1d/+0x21：两个具名字节局部（result 深于 memberCount）；
    // 决策树按 P163 补 case 1（与未列值同体）撑宽 range。
    // [P211/P300] handleStartGame 返回 char（ORIG `mov %al,-0x12` 直存，无
    // setne）；bool 目标会让 4.4 插 `test/setne` 归一化（多 2 条）⇒ 同址
    // union 双视角：写入走 char 视角（直存），读取走 bool 视角（xor 直测）。
    union ResByte
    {
        char c;
        bool b;
    };
    ResByte result;
    result.b = false;
    char memberCount = 0;

    WongWork::CBossTower* tower = G_CGameManager()->getBossTower();
    if (tower == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x34, 5);
        trace("StaticPool BossTower NULL");
    }
    else
    {
        const int member = party->get_member_count();
        switch (member)
        {
            case 1: memberCount = 0; break;
            case 2: memberCount = 1; break;
            case 3: memberCount = 2; break;
            case 4: memberCount = 3; break;
            default: break;  // ORIG: 越界值保持 0
        }

        result.c = tower->handleStartGame(party, static_cast<int>(dungeonIdx),
                                          memberCount, type);
        if (!result.b)
        {
            tower->_destroy();
        }
    }
    return result.b;
}

#include "BossTowerFunc/getBossTower.inc"


#include "BossTowerFunc/getAdvanceAltar.inc"


#include "BossTowerFunc/DieCharacInDungeon.inc"


#include "BossTowerFunc/FinishLoad.inc"


#include "BossTowerFunc/GiveUpGame.inc"


#include "BossTowerFunc/MoveMap.inc"

