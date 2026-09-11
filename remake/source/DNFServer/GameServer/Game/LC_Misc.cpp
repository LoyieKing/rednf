// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LC_Misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkClosureC 批次散件 producer（杂项组）。
// 全部函数逐报告/逐指令转写；报告缺失处已用 ORIG objdump 逐指令核实并标注。
// 覆盖符号：
//   Global::CBossTowerFunc::DieMob         T 0x081468aa
//   Secu_HackLogCheck::DieMob              T 0x0827a81c
//   PvPDieUserRevive(int,int,uint,int)     T 0x08636555
//   GetItemIdByAddInfo(Inven_Item*,int,int) W 0x084e91a4
//   Sanicova::CPad::{incFailCnt,resetFailCnt,getPassword,checkAnswer}
//                                          W 0x082652d8/0x082652f2/0x08265326 / T 0x08599712
//   StrikerScript::checkSkillIndex         T 0x08a9e868
//   CEnvironment::getProcessSequence       W 0x08234516
#include "BossTowerFunc.h"

 
 #include <cstring>
 #include <cstdio>
 

#include "CEnvironment.h"       // getProcessSequence 声明
 #include "CUser.h"              // CPad/Secu_HackLogCheck 声明（权威）
#include "CGameManager.h"       // G_CGameManager / GetPvp
#include "Secu_HackLogCheckByParty.h"  // MSG_MONSTER_DIE 完整定义
 #include "GlobalData.h"         // s_pcryptRijndael_MousePassword_
#include "Inven_Item.h"
#include "PvP_Room.h"           // CheckTimerKey/HandleTimerUserRevive
#include "RefPvpGrade.h"
#include "StrikerScript.h"
#include "CSystemTime.h"        // getCurTickCount（SetMonsterInfo/init224/225）
#include "WarField.h"           // （CGameManager/TIMER_MESSAGE 依赖链）

// encryptAES_ —— ORIG T 0x0846c398（MouseRegisterEJ.cpp 定义）。
char* encryptAES_(CRijndael* method, char* src, char* dst);

#include "LC_Misc/DieMob.inc"


#include "LC_Misc/DieMob_1.inc"


#include "LC_Misc/PvPDieUserRevive.inc"


#include "LC_Misc/GetItemIdByAddInfo.inc"


#include "LC_Misc/incFailCnt.inc"


#include "LC_Misc/resetFailCnt.inc"


#include "LC_Misc/getPassword.inc"


#include "LC_Misc/checkAnswer.inc"


// encryptAES_ —— ORIG T 0x0846c398（MouseRegisterEJ.cpp 定义）。
char* encryptAES_(CRijndael* method, char* src, char* dst);

#include "LC_Misc/checkSkillIndex.inc"


#include "LC_Misc/getProcessSequence.inc"


#include "LC_Misc/getStrikerSkillVector.inc"
                                                                short second)
{
    // b28d-36（修法）：ORIG 0x08a9e7f4 段把 key 槽（int @-0x10）用**两次 16 位
    // store** 填成 `低半=first / 高半=second`：
    //   movzwl -0x1c,%eax; mov %ax,-0x10 ; movzwl -0x20,%eax; mov %ax,-0xe
    // 然后 find 传 &key（int）。旧形 `ushort uw + int key=(int)uw` 使 4.4 多出
    // uw 槽并 32 位直存 key（`mov %eax,-0x14`，+3 条、全帧浅 4）。
    // 用 union 的 short 视图逐半字写入即逐条复现（P197① union 拆半同族；
    // union 为函数局部 POD，零符号发射）。
    union StrikerSkillKey
    {
        int key;
        struct { unsigned short lo, hi; } parts;
    };
    StrikerSkillKey k;
    k.parts.lo = static_cast<unsigned short>(first);
    k.parts.hi = static_cast<unsigned short>(second);
    std::map<int, std::vector<StrikerSkill>*>::iterator it =
        m_skillMap.find(k.key);
    if (it != m_skillMap.end())
        return it->second;
    return 0;
}

// ===========================================================================
// Secu_MonsterCheck 9 函数族剩余 7 个（SetUserPtr/Reset/Secu_HackLogCheck::Init
// 已在 UserCoreDepsB2.cpp；DieMob 在本文件）。报告 Secu_MonsterCheck/
// {SetMonsterInfo,init223,init224,init225,Check223,Check224,Check225}.md 全文已读。
// ===========================================================================

#include "LC_Misc/SetMonsterInfo.inc"


#include "LC_Misc/init223.inc"


#include "LC_Misc/init224.inc"


#include "LC_Misc/init225.inc"


#include "LC_Misc/Check223.inc"


#include "LC_Misc/Check224.inc"


#include "LC_Misc/Check225.inc"

