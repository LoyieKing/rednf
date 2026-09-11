#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_B/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCmds_B.cpp — game_master 命令体系还原·批次 B（11 类）。
//
// 对照 docs/class_func_reports/game_master__{CChangeCharacName,CChangeExpertJobCmd,
//   CChangeExpertJobExpCmd,CChangeNPCFavorCmd,CChangeNPCMaxDayCmd,CCharacStatCmd,
//   CChoiceItem,CCirculationStatisticCmd,CClearAvatarInvenCmd,CClearInventoryCmd,
//   CClearRewardTest}/{parse,execute,printClient}.md。
//
// 占位字段说明（本批次禁止编辑 GameMasterCmd.h）：
//   头文件对这些类统一给出占位成员 std::string m_str0（+0x08）。ORIG 布局中
//   CChangeExpertJobCmd/CChangeExpertJobExpCmd/CChangeNPCFavorCmd/
//   CChangeNPCMaxDayCmd/CChoiceItem 的 +0x08（及 NPC 双参数类的 +0x0c）实为
//   int 槽（ctor 不构造、parse 按 atoi 写入），故此处按存储位置 reinterpret
//   访问；对象大小与 ORIG 一致（32 位 COW std::string 为 4 字节）。
//   CChangeCharacName 的 +0x08 是真 std::string（ORIG ctor 调 _ZNSsC1Ev）。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "CDataManager.h"
#include "CItem.h"
#include "SkillSlot.h"
#include "STExpertJobScript.h"
#include "CExpertJob.h"
#include "Inven_Item.h"
#include "CInventory.h"
#include "LogManager.h"
#include "CBattle_Field.h"
#include "CDungeon.h"
#include "CParty.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <utility>

// ---------------------------------------------------------------------------
// ORIG T 0x08426590 _ZN22DB_ReqChangeCharacName11makeRequestEiiPKci
// （静态成员；仓库尚无该类声明，按 objdump 符号补 extern 声明）
// ---------------------------------------------------------------------------
class DB_ReqChangeCharacName
{
public:
    static void makeRequest(int uid, int characNo, char const* name, int flag);
};

// ---------------------------------------------------------------------------
// ORIG 0x084a905e：getCurCharacAddInfoR() 的附加信息缓冲在 +0x42/+0x46/+0x48
// 处存放 move/attack/cast speed，ORIG 对三处一律单条 disp 访问
//（`mov 0x42(%eax),%eax` / `movzwl 0x46(%eax),%eax` / `movzwl 0x48(%eax),%eax`）。
// 用 `unsigned char*` + 常量偏移在 4.4 会退化成 `add $0xNN,%eax; mov (%eax),%eax`
// 两步（N107 族）⇒ 按 P328② 用具名 pack(1) 视图恢复 disp 折叠。视图仅表达布局，
// 不发符号、不改任何权威头。
// ---------------------------------------------------------------------------
namespace
{
struct CharacAddInfoStatView
{
    unsigned char m_pad[0x42];
    int           m_moveSpeed;    // +0x42
    unsigned short m_attackSpeed; // +0x46
    unsigned short m_castSpeed;   // +0x48
} __attribute__((packed));
}

namespace game_master
{

// ===========================================================================
// CChangeCharacName — ORIG W ctor 0x082a8418：基类 vptr → 自身 vptr(0x8c81930)，
// string@+0x08 默认构造（_ZNSsC1Ev）。
// ===========================================================================

#include "GmCmds_B/parse.inc"


#include "GmCmds_B/execute.inc"


// ===========================================================================
// CChangeExpertJobCmd — ORIG W ctor 0x082a816a：仅基类 vptr + 自身 vptr
// (0x8c81b70)；+0x08 为 int 槽【专家职业类型】，不构造。
// ===========================================================================

#include "GmCmds_B/parse_1.inc"


#include "GmCmds_B/execute_1.inc"


// ===========================================================================
// CChangeExpertJobExpCmd — ORIG W ctor 0x082a8186：仅 vptr 设置；
// +0x08 为 int 槽【专家职业经验增量】。
// ===========================================================================

#include "GmCmds_B/parse_2.inc"


#include "GmCmds_B/execute_2.inc"


// ===========================================================================
// CChangeNPCFavorCmd — ORIG W ctor 0x082a831c：仅 vptr；
// +0x08/+0x0c 为双 int 槽【npcId / favor】。
// ===========================================================================

#include "GmCmds_B/parse_3.inc"


#include "GmCmds_B/execute_3.inc"


// ===========================================================================
// CChangeNPCMaxDayCmd — ORIG W ctor 0x082a8300：仅 vptr；
// +0x08/+0x0c 为双 int 槽【npcId / maxDay】。
// ===========================================================================

#include "GmCmds_B/parse_4.inc"


#include "GmCmds_B/execute_4.inc"


// ===========================================================================
// CCharacStatCmd — ORIG W ctor 0x082a8098：仅 vptr（无成员）。
// ===========================================================================

#include "GmCmds_B/execute_5.inc"


// ===========================================================================
// CChoiceItem — ORIG W ctor 0x082a87c2：仅 vptr；+0x08 为 int 槽【物品索引】。
// ===========================================================================

#include "GmCmds_B/parse_5.inc"


#include "GmCmds_B/execute_6.inc"


// ===========================================================================
// CCirculationStatisticCmd — ORIG W ctor 0x082a8044：仅 vptr（无成员）。
// ===========================================================================

#include "GmCmds_B/execute_7.inc"


// ===========================================================================
// CClearInventoryCmd — ORIG W ctor 0x082a8338：仅 vptr（无成员）。
// ===========================================================================

#include "GmCmds_B/execute_8.inc"


// ===========================================================================
// CClearAvatarInvenCmd — ORIG W ctor 0x082a8354：仅 vptr（无成员）。
// ===========================================================================

#include "GmCmds_B/execute_9.inc"


// ===========================================================================
// CClearRewardTest — ORIG W ctor 0x082a8902：仅 vptr（无成员）。
// ===========================================================================

#include "GmCmds_B/parse_6.inc"


#include "GmCmds_B/execute_10.inc"


#include "GmCmds_B/printClient.inc"


} // namespace game_master