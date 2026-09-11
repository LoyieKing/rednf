#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_E/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCmds_E.cpp — game_master GM 命令批次 E（11 类）。
// 对照 docs/class_func_reports/game_master__<Class>/：
//   CMobileRestrictCommand / CMonsterDropTest / NpcLimitBuyItem /
//   COneADayItemShop / CPcroomPremiumCmd / CPcroomRewardReset / CPickup /
//   CPowerWarStraightVictory / CRandomObjectTest / CRecipeSuccess / CRefund
//
// b28d-solo-GmCmds_E（2026-09-09 单文件泳道收口）：清单 9 件最终
//   ①AE/identical：makeGoldItemDropList、parse、getVendingMachineInfo（原状）
//     + CMonsterDropTest::execute（本次 DIFF→AE：fname 255 + 表头/循环 switch）；
//   ②NEAR：makeGeneralItemDropList（key 槽提到函数头，ratio 0.9108→0.9682）；
//   ③N 封顶 DIFF：processKillMonster（T1/T2 直证 unsigned base + (int)(long long)
//     截断，282=280；余 N85 x87 比较 + N82/N115 帧）、makeUpgradeDropList
//     （N98/N82 槽序反向）、passedMapMonsterKill（N7 后端寄存器角色）、
//     CRandomObjectTest::execute（P75 memset 实参装载序，手册「待解」项）。
//   TU 级：identical=123 ae=251 near=1 diff=6（基线 123/250/1/7，diff −1、ae +1）；
//   无 .h 改动；无子子代理。
//
// b28d-solo-GmCmds_E-2（2026-09-09 第二轮：dual_rank 3 件 + 全 TU jcc 交叉验证）
//   ①processKillMonster gap 2→0（280→282=ORIG 282）：
//     - 删 `float baseF` 具名局部（P0：ORIG getDropRate 返回值直接 fnstcw/
//       fistpll 转 int64，无 fstps 槽往返）⇒ 消 2 条 fstps/flds；
//     - 两处 `if(checkValidUser(i)){body}` → `if(!checkValidUser(i)) continue;`
//       （P168a：ORIG 0x084adf66 `xor $1;test;jne <循环尾>`）⇒ 全函数 jcc
//       助记符序列与 ORIG 逐位一致（MNE-DIFF 1→0）。余 N85 x87 比较 lowering
//       （ORIG fucompp/fnstsw/test$0x45/sete vs 4.4 fucomip/fstp/seta，4 条）
//       + N82/N115 帧（ORIG push edi/esi/ebx+sub 0x9c vs sub 0x98）。
//   ②makeGeneralItemDropList NEAR→**AE**（ae 252→253、near 1→0）：
//     删具名 `int key`，改 `out.find(it->m_item.m_addInfo)` 内联实参
//     （P236：ORIG find 实参临时槽 -0x50 比 it(-0x58) 浅；具名 key 被 4.4
//     排到 -0x54 且 end 临时 -0x50，与 ORIG 互换）。
//   ③makeUpgradeDropList ndiff 555→427（grade/up/k2 内联进 find 实参，P236
//     同族）：仍 DIFF，余 N86（ORIG find 返回经 sret 临时再拷贝 -2 mov，
//     4.4 copy-elision 无源码解）+ N98 槽序 + EH 析构路径（p167 EXTRA）。
//   ④全 TU jcc 逐位交叉验证：MNE-DIFF=0（条件码序列全对齐）；TGT-DIFF 7 件
//     均为指令数/编码差导致的相对偏移差（5 件游戏域 + N123 static_init +
//     N97 _M_incr）。逐 call * 虚槽核对：本 TU 两侧虚调用站点 = 0（无 call *）。
//   ⑤CRandomObjectTest::execute / passedMapMonsterKill / _M_incr 复验：
//     - CRandomObjectTest::execute gap 2 = ORIG 中部对齐 nop(§6.1) + memset
//       首参 lea 先行/size 经 edx 中转（P75 跨版本，手册「待解」项，三形复测）；
//     - passedMapMonsterKill gap -3 = N7 寄存器角色（ORIG sret=%edx/实参=%eax
//       vs 4.4 先 `mov %eax,%edx`），路由已 4.4.6 仍不翻，禁改路由 ⇒ 封顶；
//     - _M_incr = N97（4.1 mov/sar/shr/lea vs 4.4 lea/test/cmovs）。
//   TU 级（fresh check_tu_routed）：identical=126 ae=253 near=0 diff=6
//   （本轮起点 126/252/1/6 ⇒ ae +1、near −1、零回退）；无 .h 改动；无子子代理。
//
// b28d-loopaudit（Main 高优先级系统性排查：脚本循环「goto 退出标签」假 AE 模式）
//   本 TU 无 `goto`（仅 for/while/switch + break/continue）；按「jcc/jmp 目标
//   **指令下标**」逐位核对全部 469 符号（非字节偏移，避免 AE 归一化掩盖）：
//   - 7 件目标下标不一致：5 件游戏域 + N123 static_init + N97 _M_incr；
//   - 5 件游戏域逐条展开核对，落点差异全部语义等价，无「循环退出 vs 回边」错位：
//     · processKillMonster #12/#16：ORIG 落点含 callee-saved 载入
//       （`movzbl -0x35(%ebp),%ebx` / `mov -0x34(%ebp),%edi`）vs 我方栈直读
//       ⇒ N82/P332 装载时机差；
//     · CMonsterDropTest::parse #0/#4：je/jb 落点相差一个对齐 nop（§6.1）；
//     · CRandomObjectTest::execute #8：memset 实参装载序（P75，已知 gap2）；
//     · makeUpgradeDropList / passedMapMonsterKill：落点全对应（仅指令数平移）。
//   - 回边完整性：processKillMonster(2)/passedMapMonsterKill(3)/
//     CRandomObjectTest::execute(1)/singleMapMonsterKill(2)/CMonsterDropTest::
//     execute(4) 的 `jne <循环条件块>` 回边数量与落点均与 ORIG 一一对应，
//     无「continue/break 落到函数出口」形态；
//   - AE/IDENTICAL 循环密集件落点 DIFF=0：singleMapMonsterKill/
//     makeGoldItemDropList/makeFileGoldContents/CMonsterDropTest::execute/
//     getVendingMachineInfo/getVendingMachineItem/processVendingMachineItemLottery。
//   ⇒ 本 TU 无该 bug 模式，无需修复。
//
// 头文件 GameMasterCmd.h 本批禁止编辑：+0x08 通用槽在头里为占位
// std::string（gcc4.4 旧 ABI sizeof=4）。本批多数类的 ORIG 真实语义是
// int（atoi/sscanf 解析结果），经 str_as_int / str_as_int2 别名访问；
// CPowerWarStraightVictory 的 +0x08 真实语义是 short（仅写 2 字节），
// 经 str_as_short 访问；CMobileRestrictCommand 的 +0x0c 为 uint（无占位名），
// 从 &m_str0+4 别名访问（sizeof=0x10 与 vptr+m_parser+8 一致）。
//
// 辅助函数带参者（头内占位声明为无参）以 G 批次同款 asm 标签自由函数承载
// ORIG 成员符号（i386 成员 ABI = cdecl + this 首参压栈，extern 兼容）。
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <list>
#include "GameWorld.h"
#include "LimitNpcBuyItemRequestInfo.h"
#include <vector>
#include <string>
#include <algorithm>

#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "WarField.h"
#include "CBattle_Field.h"
#include "CDataManager.h"
#include "CGlobalEffectManager.h"
#include "DBUpdatePcroomDailyReward.h"  // DBUpdatePcroomDailyReward::MSG（MoreTypes.h 占位已迁入类）
#include "GlobalData.h"
#include "PvPTypes.h"
#include "Inven_Item.h"
#include "CItem.h"
#include "CHandlePremium.h"
#include "CSystemTime.h"
#include "GameTypes.h"
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "StreamPool.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "TaiwanInternal.h"
#include "MoreTypes.h"
#include "CInventory.h"
#include "InterSelectPcroomDailyReward.h"

// ---------------------------------------------------------------------------
// CBoosterTest 辅助静态成员（SendChatMsg/makeFileContents/printClient）已由
// GameMasterCmd.h 声明为 game_master::CBoosterTest 的 static 成员，调用点直接
// 真实成员调用（ORIG T 0x084aeac6/0x084af504/0x084aebf2；定义归 CBoosterTest 批）。
// ---------------------------------------------------------------------------
// ORIG T 0x081090fa _Z14StringToNumberPKc —— 全局工具：s ? strtoul(s,0,10) : 0。
// 项目尚无定义点；weak 定义避免与他批冲突（若他处强定义则以彼为准）。
__attribute__((weak)) int StringToNumber(const char* s);
__attribute__((weak)) int StringToNumber(const char* s)
{
    if (s == 0)
    {
        return 0;
    }
    return static_cast<int>(strtoul(s, 0, 10));
}

namespace game_master
{

namespace
{

// 注：历史版本以 std::string 占位 +0x08 槽并按 ORIG int/short 语义别名访问
//（gcc4.4 旧 ABI string sizeof=4）；该批占位字段已全部具名化
//（GameMasterCmd.h：m_duration/m_param1/m_param2/m_result/m_straight/
// m_mode/m_count/m_restrictSwitch 等），本命名空间仅保留空壳。

} // namespace

// ===========================================================================
// CMobileRestrictCommand（sizeof=0x10）
//   +0x08 string m_str0（目标限制域："mobile"/"character"）
//   +0x0c uint 限制开关（StringToNumber(args[2])!=0；无头内占位名，别名访问）
// ===========================================================================

// ORIG ctor 0x082a89b8：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_E/parse.inc"


#include "GmCmds_E/execute.inc"


// ===========================================================================
// CMonsterDropTest（sizeof=0x10）
//   +0x08 int mode（0=single / 1=passed-map 保存 / 2=passed 统计 / 3=gold 统计）
//   +0x0c int count（统计次数，0 视作 10000；上限 mode2=10000 其余=100000）
// ===========================================================================

// ORIG ctor 0x082a861e：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_E/clear.inc"


#include "GmCmds_E/parse_1.inc"


// ===========================================================================
// CMonsterDropTest 辅助成员（真签名见 GameMasterCmd.h；i386 成员 ABI =
//   cdecl + this 首参压栈，与 ORIG mangled 逐字节同形，转换零 ABI 险）。
// ===========================================================================

#include "GmCmds_E/checkCommandExecuteError.inc"


#include "GmCmds_E/processDungeonDrop.inc"


#include "GmCmds_E/processKillMonster.inc"


#include "GmCmds_E/singleMapMonsterKill.inc"


#include "GmCmds_E/passedMapMonsterKill.inc"


#include "GmCmds_E/getDungeonDropResult.inc"


#include "GmCmds_E/makeGeneralItemDropList.inc"


#include "GmCmds_E/makeGoldItemDropList.inc"


#include "GmCmds_E/makeUpgradeDropList.inc"


#include "GmCmds_E/makeFileGoldContents.inc"


#include "GmCmds_E/execute_1.inc"


// ===========================================================================
// NpcLimitBuyItem（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a885a：基类 ctor → 自身 vptr，无数据成员。

#include "GmCmds_E/parse_2.inc"


#include "GmCmds_E/execute_2.inc"


// ===========================================================================
// COneADayItemShop（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a868e：基类 ctor → 自身 vptr，无数据成员。

#include "GmCmds_E/execute_3.inc"


// ===========================================================================
// CPcroomPremiumCmd（sizeof=0xc）
//   +0x08 int 时长（秒；atoi(args)，占位槽别名访问）
// ===========================================================================

// ORIG ctor 0x082a822e：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_E/parse_3.inc"


#include "GmCmds_E/execute_4.inc"


// ===========================================================================
// CPcroomRewardReset（sizeof=0x8）
// ===========================================================================

// ORIG ctor 0x082a89e2：基类 ctor → 自身 vptr，无数据成员。

#include "GmCmds_E/parse_4.inc"


#include "GmCmds_E/execute_5.inc"


// ===========================================================================
// CPickup（sizeof=0x10）
//   +0x08 int 参数一（atoi(args[0..space))）
//   +0x0c int 参数二（atoi(args[space+1..])；无头内占位名，别名访问）
//   注：ORIG execute 为空函数（0x084ac966 直接 ret）。
// ===========================================================================

// ORIG ctor 0x082a85e6：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_E/parse_5.inc"


#include "GmCmds_E/execute_6.inc"


// ===========================================================================
// CPowerWarStraightVictory（sizeof=0xc）
//   +0x08 short 连胜数（atoi(args) 截断为 16 位写入）
// ===========================================================================

// ORIG ctor 0x082a83c4：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_E/parse_6.inc"


#include "GmCmds_E/execute_7.inc"


// ===========================================================================
// CRandomObjectTest（sizeof=0x10）
//   +0x08 int mode（1=售货机统计，其余无动作）
//   +0x0c int count（统计次数，0 视作 10000；合法域 [0,100000]）
// ===========================================================================

// ORIG ctor 0x082a8656：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_E/clear_1.inc"


#include "GmCmds_E/parse_7.inc"

// ===========================================================================
// CRandomObjectTest 辅助成员（真签名见 GameMasterCmd.h；i386 成员 ABI =
//   cdecl + this 首参压栈，与 ORIG mangled 逐字节同形，转换零 ABI 险）。
// ===========================================================================

#include "GmCmds_E/checkCommandExecuteError_1.inc"


#include "GmCmds_E/getVendingMachineInfo.inc"


#include "GmCmds_E/getVendingMachineItem.inc"


#include "GmCmds_E/processVendingMachineItemLottery.inc"


#include "GmCmds_E/execute_8.inc"


// ===========================================================================
// CRecipeSuccess（sizeof=0xc）
//   +0x08 int 强制成功率档位（0=无 / 1=success / 2=fail）
// ===========================================================================

// ORIG ctor 0x082a8972：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_E/parse_8.inc"


#include "GmCmds_E/execute_9.inc"


// ===========================================================================
// CRefund（sizeof=0x10）
//   +0x08 int 参数一 / +0x0c int 参数二（ORIG execute 为空函数，参数仅解析）
// ===========================================================================

// ORIG ctor 0x082a8602：基类 ctor → 自身 vptr；string@+0x08 默认构造。

#include "GmCmds_E/parse_9.inc"


#include "GmCmds_E/execute_10.inc"


} // namespace game_master