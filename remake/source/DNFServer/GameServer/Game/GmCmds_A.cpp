#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCmds_A.cpp — game_master 命令体系批次 A（df_game_r ORIG T 符号族还原）。
// 负责类（11）：
//   CAutoMarketConditionsControlCmd ctor 0x082a8060 / execute 0x084a8ffc（空）
//   CAssaultAbuseUserCmd         ctor 0x082a8212 / execute 0x084a9daa（空）
//   CBloodMonsterListCmd         ctor 0x082a824a / execute 0x084a9e34
//   CAddItemPollutionCmd         ctor 0x082a82e4 / parse 0x084aa9c8 / execute 0x084aaa00
//   CAddItemByMail               ctor 0x082a853e / parse 0x084ac2aa / execute 0x084ac2e2
//   CAdjustCharacStat            ctor 0x082a855a / execute 0x084ac58c
//   CBoosterTest                 ctor 0x082a863a / parse 0x084aeb5e / execute 0x084aed04
//                                + 辅助 14 函数（getRarityStr/SendChatMsg/printServer/
//                                  printClient/getBoosterResult/getCompoundEmblemResult/
//                                  getJarResult/getRandomboxResult/get_gold_lottery_result/
//                                  is_gold_lottery_item/_selectJewelTypeIndex/
//                                  makeFileContents/make_file_gold_contents/clear）
//   CBindCubeTest                ctor 0x082a8672 / parse 0x084acbe4 / execute 0x084acc1e
//   BlueMarbleSet                ctor 0x082a88ae / parse 0x084b22ca / getParam 0x084b235c /
//                                execute 0x084b25a4
//   CBingoGmTest                 ctor 0x082a893a / parse 0x084b35c4 / execute 0x084b35fc /
//                                resetData 0x084b363e / reqValue 0x084b368a
// 对照 docs/class_func_reports/game_master__<Class>/{README,parse,execute,...}.md；
// 报告缺失的 CBoosterTest::clear 由 objdump 0x084b40b0 直转。
//
// 头文件占位字段失配说明（GameMasterCmd.h 本批禁改）：占位 m_str0(string@+0x08)
// 与下列真实标量布局同尺寸（gcc4.4 32 位 std::string 仅 4 字节），本 TU 经偏移访问：
//   CAddItemByMail/CAddItemPollutionCmd/CBingoGmTest  +0x08 int
//   CBoosterTest                                      +0x08 int +0x0c int
//   CBindCubeTest                                     +0x08 byte（signed 使用）
//   BlueMarbleSet                                     +0x08 byte +0x09 byte +0x0a byte
//                                                     +0x0c int（超出占位的部分同样偏移访问）
//   CBillingRechargeCommand                           m_str0(+0x08) 真为 string +
//                                                     +0x0c int（金额）
//
// 头文件辅助函数占位签名失配说明：GameMasterCmd.h 对各辅助统一给出无参 `void f();`
// 占位声明，但 ORIG 真实签名带参（见报告）。本批不能改头，故带参辅助一律以
// 自由函数 + asm 标签对齐 ORIG mangled 实现（32 位 O0 下非虚 thiscall 与自由函数
// ABI 一致：this 即首栈参；静态成员本就不含 this）。头中无参占位声明保持
// 未定义、未被调用，不产生代码与符号。
//   注：CBingoGmTest 的 +0x08 在头中为占位 string，ORIG 真实为 int 子命令码
//   （ctor 不初始化；本 TU 因头声明 string 成员而随编译器生成其构造，差异仅限
//   构造期清零，parse 后行为一致）。reqValue/resetData 按头中无参声明实现。

#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "Inven_Item.h"
#include "CItemAmplifier.h"
#include "CMailBoxHelper.h"
#include "CHandlePremium.h"
#include "RDARScriptStringManager.h"
#include "CSystemTime.h"
#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CInventory.h"
#include "CItemList.h"
#include "CStackableItem.h"
#include "CompoundEmblemDeps.h"
#include "BlueMarble.h"
#include "CCompoundAvatar.h"      // WongWork::CCompoundAvatar::ProcTest（桥清零）
#include "CGuildServerProxy.h"    // CEventManager::GetRepeatEvent（桥清零，权威头）
#include "CGameManager.h"         // CGameManager::GetParty / G_CGameManager（桥清零）
#include "TaiwanCash.h"           // WongWork::IPG::CIPGHelper 虚接口
                                  // （CBillingRechargeCommand::execute 真虚调用）

#include "DNFFunctionLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
// ORIG 数据符号（ELF .rodata 指针表，仓库尚无声明）。
extern char* CHARACTER_JOB_NAMES[];
extern char* BIND_NAMES[];
extern char* RECIPE_NAMES[];
extern char* EQUIP_NAMES[];

// ---------------------------------------------------------------------------
// [b23S3C] P201 桥转真：BingoEvent/CBingo 改 include 权威头
// （BingoEvent.h：getBingoSystem/sendOneMatchLineReward；CBingoTest.h：
//  CBingo::calNewValue(unsigned int&, int)），reqValue 调用点
//  boost::shared_ptr<CBingo>::operator-> 真调用（ORIG +0x8f call 080ce766）。
// ---------------------------------------------------------------------------
#include "BingoEvent.h"
#include "CBingoTest.h"
#include <boost/shared_ptr.hpp>

namespace game_master
{

// ---- GM 命令对象参数区一律具名字段访问（§4.3 裸偏移清零，2026-09-06）：
// GameMasterCmd.h 已按 ORIG POD 落真实字段（CAddItemPollutionCmd/CAddItemByMail
// ::m_itemIdx、CBoosterTest::m_mode(+0x08)/m_count(+0x0c)、CBindCubeTest::m_flag、
// BlueMarbleSet::m_flag0/1/2(+0x08/09/0a)+m_param(+0x0c)、CBingoGmTest::m_subCmd），
// 原 cmd_int0/cmd_int1/cmd_byte0 三宏（内部为 char*+0x08/0x0c 裸偏移）已删除。


// ===========================================================================
// CAutoMarketConditionsControlCmd — ORIG W ctor 0x082a8060：基类 vptr → 自身
// vptr(0x8c81c58)，无字段（sizeof=0x8）。
// ===========================================================================

#include "GmCmds_A/execute.inc"


// ===========================================================================
// CAssaultAbuseUserCmd — ORIG W ctor 0x082a8212：基类 vptr → 自身 vptr(0x8c81ae0)。
// ===========================================================================

#include "GmCmds_A/execute_1.inc"


// ===========================================================================
// CBloodMonsterListCmd — ORIG W ctor 0x082a824a：基类 vptr → 自身 vptr(0x8c81ab0)。
// ===========================================================================

#include "GmCmds_A/execute_2.inc"


// ===========================================================================
// CAddItemPollutionCmd — ORIG W ctor 0x082a82e4：基类 vptr → 自身 vptr(0x8c81a20)，
// string@+0x08 默认构造（占位；真实 +0x08 为 int 物品索引）。
// ===========================================================================

#include "GmCmds_A/parse.inc"


#include "GmCmds_A/execute_3.inc"


// ===========================================================================
// CAddItemByMail — ORIG W ctor 0x082a853e：基类 vptr → 自身 vptr(0x8c81840)，
// string@+0x08 默认构造（占位；真实 +0x08 为 int 物品索引）。
// ===========================================================================

#include "GmCmds_A/parse_1.inc"


#include "GmCmds_A/execute_4.inc"


// ===========================================================================
// CAdjustCharacStat — ORIG W ctor 0x082a855a：基类 vptr → 自身 vptr(0x8c81828)。
// ===========================================================================

#include "GmCmds_A/execute_5.inc"


// ===========================================================================
// CBoosterTest — ORIG W ctor 0x082a863a：基类 vptr → 自身 vptr(0x8c81750)。
// 布局：+0x08 int 物品索引（clear 置 -1）、+0x0c int 统计次数。
// ===========================================================================

#include "GmCmds_A/parse_2.inc"


#include "GmCmds_A/clear.inc"


#include "GmCmds_A/execute_6.inc"


// ---- CBoosterTest 辅助函数（ORIG 逐符号转写）------------------------------

#include "GmCmds_A/getRarityStr.inc"


#include "GmCmds_A/SendChatMsg.inc"


#include "GmCmds_A/printServer.inc"


#include "GmCmds_A/printClient.inc"


#include "GmCmds_A/is_gold_lottery_item.inc"


#include "GmCmds_A/getJarResult.inc"


#include "GmCmds_A/get_gold_lottery_result.inc"


#include "GmCmds_A/getBoosterResult.inc"


#include "GmCmds_A/getRandomboxResult.inc"


// ORIG getCompoundEmblemResult 0x084af0d6：合成徽章统计；目标不是 16 型物品返回 false。
// [b28d-47] verdict=DIFF(routed) [N82] ORIG 0x084af0d6 orig/ours=218/200
//   残差：槽/寄存器分配跨版本差 + STL 内联实例（N89）。
// [b28d-solo-GmCmds_A] verdict=DIFF ndiff=61 [N82/N4] p167 CLEAR(31/31)；sockets 槽差
// 0x10 + ORIG 单条 scaled disp `mov 0xc(%ebx,%eax,4)` vs 4.4 shl+lea 折叠 + sete 形。
// （S298 将 stStackableBoosterItemInfo_t::m_itemIdx 改 unsigned int 后本件 ndiff 76→61。）
// [b28d-solo-2] verdict=DIFF：修 sete 物化（ORIG +0x05d `cmpl $-1; sete; test; je`
// ⇒ `while (jewelType == -1) { return false; }`，P295/P225）后 orig/ours=219/219。
// 残余：全帧槽序差 8（sockets -0x48 vs -0x40，4.1 先声明深 vs 4.4 分区分配，N82）
// + 跳表 `mov 0xc(%ebx,%eax,4)` vs shl+lea（N4）+ 虚调用 receiver 寄存器角色（N7）。
//
// [round5-GmCmdsA] P291/P311 具名数组成员视图：ORIG 0x084af148 先 `call G_CDataManager`
// 入 %ebx（无槽），0x084af155 才 `call get_grade`，随后
// `add $0x208,%eax; mov 0xc(%ebx,%eax,4),%eax` —— 即「%ebx = 管理器基址、disp=0xc、
// 索引寄存器已含 +0x208」的 scaled 索引形。旧源码 `int grade = item->get_grade();`
// 具名槽 + `(char*)mgr + (grade*6+jewelType)*4 + 0x82c` 裸算术 ⇒ 4.4 折成
// `shl $2; lea (%ecx,%eax,1)` 且 get_grade 先于 G_CDataManager 求值（差 2 条）。
// 改用视图后：基址先求值、索引常量入寄存器、scaled disp 折叠，与 ORIG 逐条一致。
struct CBoosterEmblemIdTableView
{
    char m_pad00[0x0c];       // +0x00（ORIG 索引基址 = G_CDataManager() + 0xc）
    int  m_ids[0x1000];       // +0x0c（ORIG 恒 `mov 0xc(%ebx,%eax,4)`）
};

#include "GmCmds_A/getCompoundEmblemResult.inc"


#include "GmCmds_A/_selectJewelTypeIndex.inc"


#include "GmCmds_A/makeFileContents.inc"


#include "GmCmds_A/make_file_gold_contents.inc"


// ===========================================================================
// CBindCubeTest — ORIG W ctor 0x082a8672：基类 vptr → 自身 vptr(0x8c81780)，
// string@+0x08 默认构造（占位；真实 +0x08 为 byte 职业）。
// ===========================================================================

#include "GmCmds_A/parse_3.inc"


#include "GmCmds_A/execute_7.inc"


// ===========================================================================
// BlueMarbleSet — ORIG W ctor 0x082a88ae：基类 vptr → 自身 vptr(0x8c81588)。
// 布局：+0x08 byte 步数、+0x09 byte move 标记、+0x0a byte enter 重置标记、
//       +0x0c int token 计数（getParam 工作区）。
// ===========================================================================

#include "GmCmds_A/parse_4.inc"


#include "GmCmds_A/getParam.inc"


#include "GmCmds_A/execute_8.inc"


// ===========================================================================
// CBingoGmTest — ORIG W ctor 0x082a893a：基类 vptr → 自身 vptr(0x8c81510)，
// 无成员初始化（+0x08 int 未初始化，ORIG 如此）。类声明见 GameMasterCmd.h
// （占位 std::string m_str0@+0x08，真实为 int 子命令码，本 TU 偏移访问）。
// ===========================================================================

#include "GmCmds_A/parse_5.inc"


#include "GmCmds_A/execute_9.inc"


#include "GmCmds_A/resetData.inc"


#include "GmCmds_A/reqValue.inc"


// ===========================================================================
// CBillingRechargeCommand — ORIG W ctor 0x082a898e：基类 vptr → 自身
// vptr(0x8c814c8)，string@+0x08 默认构造（货币名）；+0x0c int 数额。
// ===========================================================================

#include "GmCmds_A/parse_6.inc"


#include "GmCmds_A/execute_10.inc"


} // namespace game_master