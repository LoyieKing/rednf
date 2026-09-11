// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_F/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCmds_F.cpp — game_master GM 命令批次 F（11 类）。
// 对照 docs/class_func_reports/game_master__<Class>/：
//   CRequestDisjointCmd / CReselectDailyQuestCmd / CResetTrainingQuestCmd /
//   CSecretShopCheckLottery / CSecretShopNpc / CSetGuildFund /
//   CSetMultiboxFailCnt / CSetTutorialFlagCmd / CStartEvent(+getParam) /
//   CStartMacroCmd / CStopMacroCmd
//
// 头文件 GameMasterCmd.h：本批各类 +0x08 槽均已按 ORIG 真实语义建模
// （int/string 按类），CStartEvent +0x08..0x13 = [int count][int eventId]
// [short p1][short p2]（总大小 0x14）。
// [b28d-solo-GmCmds_F 补] CSetTutorialFlagCmd::m_flag unsigned int→int
// （+0x08/sizeof 0xc/偏移/mangled 不变，Main 已批准；P211 双旋钮闭合 parse
// 0x084a921a 无中转 + execute 0x084a9252 两段，见下方 parse 判词）。
//
// [b28d-solo-GmCmds_F2 复核] TU 级符号差集闭合（唯一残余）：.o 相对 ORIG 多 1 个
// `_GLOBAL__I__ZN11game_master19CRequestDisjointCmd5parseEv`（compare_tu
// missing_in_game=1）。根因（编译器 -H 实测 include 链，非推断）：
//   GmCmds_F.cpp → TaiwanQueries.h → TaiwanCash.h → ThreadInterface.h:16
//   `#include <iostream>` ⇒ 本 TU 发射 std::ios_base::Init __ioinit
//   （.o 的 _Z41__static_initialization_and_destruction_0ii 体仅
//    ios_base::InitC1 + __cxa_atexit，逐条核过）。
// ORIG 全二进制无任何 GmCmds/GmCmds_F 相关 _GLOBAL__I_（nm 实查）⇒ ORIG 该 TU
// 无 file-scope 动态初始化。唯一修法 = 删 ThreadInterface.h 的该 include（共享头：
// 7 处直接 include + TaiwanCash.h 12 处 + MySQL.h 56 处间接消费）⇒
// Main 2026-09-10 裁决「不删」（收益仅 1 个 GI 键、风险面大；P327/N132 已把此类
// 键名归属列为结构性噪音）。判 N/P327 封顶，本 TU 保持原状。
//
// [b28d-solo-GmCmds_F2 复核·Main「jmp/jcc 目标下标」新判据] 本 TU 唯一循环 =
// CStartEvent::getParam 的 while(find(' '))（本文件 362 行；全 TU 源码无
// goto/break/continue）。全 18 函数 jmp+jcc 的**函数内落点 +0xNN** 逐位比对
// ORIG vs NEW = 47/47 全等、0 mismatch（getParam 17 个跳转，含循环回边
// `jne +0x6f` 与条件头 `jmp +0x145`，与 ORIG 逐位同）⇒ 无
// EventScriptsCompleteB2 型「goto 退出标签 ⇒ 每文件只解析首 token 即 return」
// bug（AE 归一化只掩盖目标文本、未掩盖落点，本例落点亦全等）。
// 核对脚本：/tmp/jmp_check_F.py（jmp+jcc，目标解析优先 `sym+0xNN`）。
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "DisPatcherCore.h"   // DisPatcher_DisJointItem::_disjoint（ORIG T 0x081f92ca）
#include "CServerProxyMgr.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "CSecretShop.h"
#include "CParty.h"
#include "CDungeon.h"
#include "GameTypes.h"
#include "PacketHeader.h"

// Packet_Monitor_Event_Start —— ORIG 权威定义三处同构：
//   Monitor/DNFPacketTranslater.h:439（0x44f/0x12，eventCode + p1/p2 ushort）
//   Guild/DNFApplication.h:145 / COServer/DNFApplication.h:96。
// 因 COServer 版头会拉入与 Game/CUser.h 冲突的 COServer DNFUser（class CUser
// 重定义），本 TU 按权威布局最小化复刻对接（packed，sizeof=0x12；ctor 实现
// 位于 Monitor/DNFPacketTranslater.cpp:5674）。
class Packet_Monitor_Event_Start : public PacketHeader
{
public:
    Packet_Monitor_Event_Start();
    unsigned int m_field1;    // +0xa eventType/eventCode
    unsigned short m_field2;  // +0xe param1
    unsigned short m_field3;  // +0x10 param2
} __attribute__((packed));

// ---------------------------------------------------------------------------
// 外部符号（本批禁改共享头，按 ORIG mangled 直连 / 等价自由函数声明）：
//

// ORIG T 0x081f92ca _ZN23DisPatcher_DisJointItem9_disjointEP5CUserii14ENUM_CMDPACKETS1_t
// —— DisPatcher_DisJointItem 静态成员，权威声明在 DisPatcherCore.h（static int _disjoint），
// 由 DisPatcherCore.cpp 提供 ORIG 真实定义（LinkF2 已实现）。

namespace game_master
{

// 占位 std::string 槽（+0x08）按 ORIG 实际 int/short 语义别名访问。
// 用宏而非 inline 函数：-O0 下函数调用会引入 ORIG 没有的额外 call 形态。
#define GM_STR0_INT(obj) (*(reinterpret_cast<int*>(&(obj))))

// CStartEvent 真实布局（头文件占位 m_str0/m_f1 别名映射，总大小 0x14 一致）：
//   +0x08 int 参数计数 / +0x0c int eventId / +0x10 short p1 / +0x12 short p2

// ===========================================================================
// CRequestDisjointCmd（id 注册序见 Mgr ctor；sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a8124：基类 ctor → 自身 vptr(0x8c81ba0)；string@+0x08 默认构造。

#include "GmCmds_F/parse.inc"


#include "GmCmds_F/execute.inc"


// ===========================================================================
// CReselectDailyQuestCmd（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a8370：基类 ctor → 自身 vptr，无数据成员。

#include "GmCmds_F/execute_1.inc"


// ===========================================================================
// CResetTrainingQuestCmd（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a838c：基类 ctor → 自身 vptr，无数据成员。

#include "GmCmds_F/execute_2.inc"


// ===========================================================================
// CSecretShopCheckLottery（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a847a：基类 ctor → 自身 vptr，无数据成员。

#include "GmCmds_F/execute_3.inc"


// ===========================================================================
// CSecretShopNpc（sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a8496：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_F/parse_1.inc"


#include "GmCmds_F/execute_4.inc"


// ===========================================================================
// CSetGuildFund（sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a8576：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_F/parse_2.inc"


#include "GmCmds_F/execute_5.inc"


// ===========================================================================
// CSetMultiboxFailCnt（sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a891e：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_F/parse_3.inc"


#include "GmCmds_F/execute_6.inc"


// ===========================================================================
// CSetTutorialFlagCmd（sizeof=0xc）
// ===========================================================================

// ORIG ctor 0x082a80b4：基类 ctor → 自身 vptr；+0x08 标量未初始化。

#include "GmCmds_F/parse_4.inc"


#include "GmCmds_F/execute_7.inc"


// CStartEvent（sizeof=0x14）
//   ORIG 真实布局：+0x08 int 参数计数 / +0x0c int eventId /
//                 +0x10 short param1 / +0x12 short param2。
//   头文件使用真实字段布局，总大小 0x14。
// ===========================================================================

#include "GmCmds_F/parse_5.inc"


#include "GmCmds_F/getParam.inc"


#include "GmCmds_F/execute_8.inc"


// ===========================================================================
// CStartMacroCmd（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a81a2：基类 ctor → 自身 vptr，无数据成员。

#include "GmCmds_F/execute_9.inc"


// ===========================================================================
// CStopMacroCmd（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a81be：基类 ctor → 自身 vptr，无数据成员。

#include "GmCmds_F/execute_10.inc"


} // namespace game_master