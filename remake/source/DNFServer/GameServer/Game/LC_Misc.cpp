// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LC_Misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// ---------------------------------------------------------------------------
// ORIG 0x081468aa（报告 Global__CBossTowerFunc/DieMob.md）：
// b28c-L8 判词 [N(N7)]：fresh 复验 35=35、p167 调用点差集 0/0、sub $0x44 +
// push/pop %ebx 全等。唯一残差 = ORIG 0x081468db `mov (%eax),%ebx`（虚槽 +0x24
// handleKillMonster 目标留 callee-saved %ebx）、实参装载走 %edx/%eax/%ecx；4.4 对间接
// call 目标恒留 %eax 且装载序整体前移一格（%ecx/%edx/%ebx）。P226 语句表达式升格只对
// 「call 结果值」有效、对 call 目标寄存器无效；-march=i486 对 N7 亦无效（Main 口径）
// ⇒ N7 封顶（与 lane12 Secu_MonsterCheck 族同判）。
// getBossTower() 非空 → 虚槽 +0x24（handleKillMonster）(user,mobSeq,param,msg,0)；
// 恒返回 1。
#include "LC_Misc/DieMob.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0827a81c（报告 Secu_HackLogCheck/DieMob.md）：
// msg->m_fieldF(+0x0f short) != 0xffff 时：SetMonsterInfo(msg)；m_flag223/224/225
// 非 0 者分别以 Check223/224/225 结果刷新。
// [b28c-27] N18a：ORIG 8429xx `call Check223; mov 0x8(%ebp),%edx; mov %al,0x38(%edx)`
// 为 call 返回 char 单字节直存；4.4 恒经 `mov %eax,%edx` 截断中转（+2 条/处）。
// 删 cast、字段 unsigned 化、bool 化均实测不可翻（P168a③b 边界），语义等价。
#include "LC_Misc/DieMob_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08636555（报告缺失；objdump 0x08636555..0x086365d8 逐指令核实）：
//   key = p4 + 0x33
//   room = G_CGameManager()->GetPvp(p1, (CUser*)0, key)
//   room == 0 → return 0
//   if (!room->CheckTimerKey((TIMER_MESSAGE)p4, p2)) return 0
//   room->HandleTimerUserRevive(p4)
//   return 1
// （p3 在 ORIG 中未被读取。）
#include "LC_Misc/PvPDieUserRevive.inc"
}

// ---------------------------------------------------------------------------
// ORIG W 0x084e91a4（报告缺失；objdump 逐指令核实）：
// for i in 0..count-1: Inven_Item& it = items[i]（stride 0x3d）；
//   it.m_addInfo2(+0x07) == addInfo 且 != 0 → return it.m_addInfo(+0x02)；
// 未命中 return 0。（mangled 参数为非 const 指针 _Z18GetItemIdByAddInfoP10Inven_Itemii。）
#include "LC_Misc/GetItemIdByAddInfo.inc"
}

// ---------------------------------------------------------------------------
// ORIG W 0x0844d54c / 0x0844d55a（objdump：mov 0x134/0x138(%eax); ret）。
// ORIG W 0x082652d8（objdump：+0x38 自增返回新值）。
#include "LC_Misc/incFailCnt.inc"
}

// ORIG W 0x082652f2：写 +0x38 = 0。
#include "LC_Misc/resetFailCnt.inc"
}

// ORIG W 0x08265326：返回 m_pad（密文首址，非 const）。
#include "LC_Misc/getPassword.inc"
}

// ORIG T 0x08599712（报告 Sanicova__CPad/checkAnswer.md）：
// 清零 36 字节缓冲 ×2 → strncpy(answer, 8) → encryptAES_ → 与 m_pad 0x24 字节
// strncmp（ORIG call 0x0807e8c0 = strncmp@plt；密文二进制可能含 NUL，
// strncmp 遇 NUL 提前停止——ORIG 语义如此，不得改为 memcmp）。
#include "LC_Misc/checkAnswer.inc"
}

// encryptAES_ —— ORIG T 0x0846c398（MouseRegisterEJ.cpp 定义）。
char* encryptAES_(CRijndael* method, char* src, char* dst);

// ---------------------------------------------------------------------------
// ORIG 0x08a9e868（报告 StrikerScript/checkSkillIndex.md）：
// pass0 → getStrikerSkillVector(first,second)；pass1 → (first,0)；
// 向量为空指针返回 0；命中 m_field00 == skillIndex 返回 1。
#include "LC_Misc/checkSkillIndex.inc"
}

// ---------------------------------------------------------------------------
// ORIG W 0x08234516（objdump：mov 0x1ac(%eax),%eax; ret）：
// m_serverEnvir(+0x198) + 0x14 = m_processSequence。
#include "LC_Misc/getProcessSequence.inc"
}

// ---------------------------------------------------------------------------
// ORIG T 0x08a9e7e0 sz 0x88（getStrikerSkillVector.md 全文已读）：
// m_skillMap(+0x78).find((int)(unsigned short)first)；命中返回 it->second
// （vector<StrikerSkill>*），未命中返回 0。ORIG 汇编中 second 形参仅被零扩展
// 存放（-0xe 槽），从未参与 find key——忠实保留未用语义。
#include "LC_Misc/getStrikerSkillVector.inc"
}

// ===========================================================================
// Secu_MonsterCheck 9 函数族剩余 7 个（SetUserPtr/Reset/Secu_HackLogCheck::Init
// 已在 UserCoreDepsB2.cpp；DieMob 在本文件）。报告 Secu_MonsterCheck/
// {SetMonsterInfo,init223,init224,init225,Check223,Check224,Check225}.md 全文已读。
// ===========================================================================

// ORIG T 0x0827abfa sz 0x80（SetMonsterInfo.md）：
#include "LC_Misc/SetMonsterInfo.inc"
}

// ORIG T 0x0827b288 sz 0x5c（init223.md）：active 未置 1 时记录首杀快照。
#include "LC_Misc/init223.inc"
}

// ORIG T 0x0827b2e4 sz 0x50（init224.md）
#include "LC_Misc/init224.inc"
}

// ORIG T 0x0827b334 sz 0x50（init225.md）
#include "LC_Misc/init225.inc"
}

// ORIG T 0x0827ad98 sz 0x1e0（Check223.md）：击杀数快照一致 → 1；
// 坐标/击杀差值阈值判定 → 计数超限上报 0xdf 并清状态返 0，否则重置快照返 1。
// [b28c-lane12 判词·DIFF(N)，Main 分派件] 接手后三处修正 + 残差定性：
//  ① P196 符号性鉴别：ORIG @0x0827ae60/0x0827aec6 的 `cmp $0xa,%ax; jbe` /
//     `cmp $0x4,%ax; jbe` 为 **16 位无符号**域，4.4 把 ushort 提升 int 后有符号
//     ⇒ 比较改写 unsigned 域后 jbe 族回归。值域 0..N 不越 0x7fff ⇒ 非真语义 bug。
//  ② P222/P47 块序翻正：ORIG @0x0827ae09 `jg +0xa1`（dx>0 跳远处 setne 块）⇒
//     外层改否定比较 `if (dx <= 0)`，内层 `-dx>0xc7`/`dy<=0` 同为 jg 跳走。
//  ③ 三出口嵌套三目汇合（无具名 bool）：令 4.4 三出口 setle/setne 留 %al 并统一
//     test/je，消除具名 `bool moveOk` 的 -0x9 槽物化与 4 字节帧漂（P226 应用例）。
//  残差 2 行（136=136）：`if (m_field0e > 4) m_field10 = 1;` 的 ORIG `jbe 尾`
//  vs 4.4 镜像 `ja body`；三形互试（>4 body / `<=4 goto 尾空标签` / BM8 外层
//  标签）4.4 恒折 ja ⇒ P222 镜像边界 + P223 空块岛被 jump-thread 吞，封顶。
#include "LC_Misc/Check223.inc"
}

// ORIG T 0x0827af78 sz 0x187（Check224.md）：击杀间隔 1000ms 判定，上报 0xe0。
#include "LC_Misc/Check224.inc"
}

// ORIG T 0x0827b100 sz 0x187（Check225.md）：击杀间隔 2000ms 判定，上报 0xe1。
#include "LC_Misc/Check225.inc"
}
