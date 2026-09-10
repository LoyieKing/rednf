// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CUserCheckLevel/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// CUser::_check_level_up — 独占还原 TU（未登记 CMake，由宿主统一登记）
//
// ORIG: _ZN5CUser15_check_level_upEiRiS0_13eExpAddReasoni @ 0x08662aea
//       [0x08662aea, 0x08663119)
// 报告: docs/class_func_reports/CUser/_check_level_up.md（## 汇编 + ## 反编译 C
//       已完整核对；本文件逐段映射见 /tmp/df_batches/CheckLevelUp_only.md）
//
// 依赖符号均经 ORIG nm 实证存在：
//   _ZNK15CUserCharacInfo16get_charac_levelEv(080da2b8) getCurCharacR(08120432)
//   _ZNK12CDataManager13get_level_expEi(08360442) 14GetSpAtLevelUpEi(08360cb8)
//   _ZN15CUserCharacInfo15setCurCharacExpEi(0819a87c) addCurCharacExp(086967be)
//   incCurCharacLevel(08696762) getCurCharacInvenW(080da28e)
//   _ZNK15CUserCharacInfo15getCurCharacNameEv(08101028) getCurCharacExp(08151abe)
//   _ZN5CUser19GetTutorialSkipableEv(084ecad4) UpdateTutorialSkipable(08697318)
//   _ZN10HistoryLog12WriteLevelUpEP8_IO_FILEi(084b9e5e)
//   _ZNK8WongWork12CUserPremium12GetGoldBonusEi(08694a64)
//   RecalcAdditionalInfoEPK5CUser(086ae8c6)
//   _ZN10CInventory10gain_moneyEi15eMoneyAddReasonbi(084ff29c)
//   SendUpdateItemListENS_11eSendTargetE14ENUM_ITEMSPACEi(0867c65a)
//   _ZN8XNuclear6CHades5ExpUpEj(084b953e) CUser8getHadesEv(08230800)
//   _ZN15cUserHistoryLog7LevelUpEi14eLevelUpReason(086845b2)
//   DimensionInoutUpdateEbb(08656c12) CGMAccounts4isGMEj(08109346)
//   Packet_Monitor_Max_Level_BroadCastC1Ev(08694560)
//   CMonitorServerProxy10SendPacketEPci(08470df4)
//   GameWorld30getDungeonMinimumRequiredLevelEi(086c9076)
//   send_user_dungeon_inout_messageEP5CUserib(086c8fc8)
//   ServerParameterScript13isDungeonOpenEi(082687fc)
//   makeGuildLevelUpMessageEi(08679754) _update_charac_statEb(086646c8)
//   _ZNK5CUser14GetServerGroupEv(080cbc90) get_acc_id(080da36e)
//   G_CDataManager(080cc19b) G_GameWorld(080da3a7)
// ============================================================================
#include "CUser.h"
#include "CDataManager.h"
#include "GlobalData.h"                          // s_pGMAccounts_ / s_monitor_proxy_mgr
#include "CGuildServerProxy.h"                   // WongWork::CGMAccounts::isGM 权威声明
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"                     // GetServerProxy 模板完整定义
#include "CInventory.h"                         // gain_money（getCurCharacInvenW 完整类型）
#include "GameWorld.h"
#include "HistoryLog.h"                          // HistoryLog::WriteLevelUp(FILE*, int)

#include <stdio.h>
#include "ReverseEngineerLib.h"                  // TEST_CLASS_SIZE 宏
#include "Packet_Monitor_Max_Level_BroadCast.h"  // 0x177e/0x28 包（ORIG W ctor 0x8694560）

#include <string.h>

// bool CUser::_check_level_up(int exp, int& sp, int& sfp, eExpAddReason, int)
//
// ORIG 参数语义（报告反编译 param_2..param_5 + 汇编实证）：
//   expAdd    — 本次待结算经验增量；
//   sp/sfp    — 升级时按 GetSpAtLevelUp(next)/>49 返还的输出引用；
//   reason    — eExpAddReason（第 4 参，0x18(%ebp)）；ORIG 两处
//               `cmpl $0x0,0x18(%ebp)`（+0x3e6 升级路径 / +0x5d9 未升级路径）
//               ⇒ 非 0 时向 XNuclear::CHades::ExpUp 上报经验（原码即用它作标志）；
//   hadesFlag — 第 5 参（0x1c(%ebp)）；ORIG 全函数 0 次引用，未读取。
// [SweepCUserCheckLevel 2026-09-09] 逐条对齐结论（check_tu_routed fresh .o 实证）：
//   ① 真 bug-1：循环为 while 前置条件（ORIG +0x087 je 落点 +0x600 条件块）；
//      原 for(;;)+尾部 break 至少多执行一次循环体（可观察差异）。已改 while。
//   ② 真 bug-2：Hades 上报标志用第 4 参 eExpAddReason（0x18(%ebp)，ORIG 两处
//      cmpl $0x0,0x18(%ebp)），第 5 参 0x1c(%ebp) ORIG 全函数 0 次引用。已改。
//   ③ P0 具名局部双向修正：补 goldBonus（ORIG -0x1c 槽往返）；删 inven/proxy/
//      open 三个多余具名局部（ORIG 结果寄存器直传，无槽）。
//   ④ P221 扩展：4 个 cMyTrace 改全表达式临时链 ⇒ 槽 -0x70/-0x60/-0x50/-0x40
//      与 ORIG 逐槽一致（具名形 4.4 恒反向浅→深）。
//   ⑤ bottom/top 声明提至函数头 ⇒ 槽 -0x2c/-0x28 与 ORIG 一致。
//   ⑥ if/else 正极性翻转（达线{升级}else{未升级}）⇒ 复现 ORIG 尾部未升级块
//      布局 + setge 条件码。
//   ⑦ 结果：帧 push edi/esi/ebx + sub $0xac、全部具名局部槽号、442 条指令与
//      ORIG 全等；check_tu_routed 判本函数 = AE（strict 差仅数据符号地址，N132）。
//   ⑧ 调用点 73=73；p167 仅 C1/C2 同址别名伪差（nm 实证同址，P93 豁免）⇒ CLEAR。
//   ⑨ 虚槽核对：ORIG 区间 `call *` = 0 处（全直接调用），无虚槽可错。
//   ⑩ jcc 逐位交叉验证 32 处：条件码/落点语义全等。
//   剩余真缺口 = 0。
#include "CUserCheckLevel/_check_level_up.inc"
}
