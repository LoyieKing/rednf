// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TimerDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "TimerDispatcher.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <new>
#include <string>
#include <vector>

#include "GlobalData.h"
#include "GameWorld.h"
#include "CGameManager.h"
#include "CDataManager.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CParty.h"
#include "PvP_Room.h"
#include "WarRoom.h"
#include "CBattle_Field.h"
#include "CPowerManager.h"
#include "PacketGuard.h"
#include "CEnvironment.h"
#include "CEventScriptMng.h"
#include "CEventTable.h"
#include "CServerEvent.h"
#include "InterDispatcher.h"
#include "CLogGameChannel.h"
#include "CCommonStatisticsMgr.h"
#include "CDungeonEntranceLog.h"
#include "CLevelDungeonPlayStatistic.h"
#include "CCirculationStatistic.h"
#include "CDungeonExpStatistic.h"
#include "CExpertJob.h"      // UpdateExpertJob / GetInstanceExpertJobStatistic
#include "CInventory.h"
#include "CUserProc.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "CGuildServerProxy.h"   // CEventManager 完整定义（GlobalData.h 仅前向）
#include "CMailBoxHelper.h"
#include "CHandlePremium.h"
#include "SPremiumInfo.h"
#include "GameServerDemonManager.h"  // GameserverDemonManagerInstance（LinkClosureB-2 声明头）
#include "CShutdowManager.h"
#include "CHackAnalyzer.h"     // ENUM_HACKTYPE/addServerHackCnt（_NS_NonClient_CheckResponse）         // GetInstanceShutdowManager/SendShutdownSignal
#include "ExchangeServer.h"
#include "VendingMachine.h"
#include "CDailyScheduleManager.h"
#include "DB_CreateCharac.h"
#include "W9_EventSmall.h"
#include "Arad_ServerStateManager.h"
#include "CSpecialItemRoutingManager.h"  // 完整类型（CGameManager.h 仅前向）
#include "CCoinRefillEvent.h"           // 金币补充 repeat 事件（本批新建模）
#include "CUserPlayTimeStatisticMgr.h"  // 在线游玩时长统计（本批新建模）
#include "CEventCreateDnf.h"            // CEventCreateDnf（CreateDnf 事件，Timer_CreateDnfDbLoad）
#include "TaiwanInternal.h"             // Taiwan::internal_stream（DBSelectCreateDnfEventInfo）

#include "TimerDispatcher/PvPDieUserRevive.inc"
#include "TimerDispatcher/_NS_NonClient_CheckResponse.inc"
class DB_UpdatePvPGrade
{
public:
    static void makeRequest();   // ORIG T 0x084293a0（ORIG 无 this 直调）
};
class DB_DeathTowerLoadTopRanker
{
public:
    static void makeRequest(bool reload);   // ORIG T 0x0842a910（定义 W8LT_DBTask.cpp，本批落地）
};
#include "DBSelectCreateDnfEventInfo.h"  // 权威声明（OrigTaiwanDisp 新建）；原局部类按 §4.4 删除
#include "CEventStayTime.h"
// ORIG T _ZN30DB_UpdateCompoundEmblemStatics11makeRequestEijjjjjjj @0x08439dec：
// 定义缺失（无 TU 产出），此处按 ORIG mangled 提供声明，链接由集成者排期补 producer。
class DB_UpdateCompoundEmblemStatics
{
public:
    static void makeRequest(int channelId, unsigned int grade0, unsigned int grade1,
                            unsigned int grade2, unsigned int grade3, unsigned int grade4,
                            unsigned int grade5, unsigned int grade6);
};
#include "CSecretShop.h"
#include "OnlinePreliminary.h"
#include "COnTimeEvent.h"
#include "CHadesServerProxy.h"
#include "CDisconnectDetecter.h"
#include "CompoundEmblemDeps.h"
#include "CGlobalEffectManager.h"
// ===========================================================================
// [b28d-solo-TimerDispatcher 收口 2026-09-09] 本 TU 30 件清单最终判词
// （verdict 口径 = check_tu_routed.sh + p167；ndiff = show_func_diff.py）：
// AE(ndiff=0) ×9：TimerLightServerNotice::dispatch_sig / TimerDungeonDataStatistic::
//   dispatch_sig / TimerUltimateNextRoundStart::dispatch_sig（真 bug×2：守卫极性 +
//   实参序反）/ TimerCheckMailTimeOut::dispatch_sig / TimerFatigueReset::dispatch_sig /
//   TimerFatigueReset::RegistNextTimer / TimerGeneratePowerUp::generatePowerUp /
//   Timer_Midnight::RegistNextTimer / Timer_NoticeEmergencyPatchMessage::dispatch_sig。
// 真语义 bug 另 1：Timer_DungeonInoutCloseTime::dispatch_sig `ptm->tm_min` 应为
//   pair+0xc（second+8 = m_startMin，ORIG 0863a29c + Ghidra 双证），旧码 m_endMin(+0x10)。
// N 封顶 ×21：N98 TimerCreature(2)；N7/P75 DBSelectCreateDnfEventInfo(2)；
//   N93附/P223 Timer_Send_Ontime_Reward(4)；N22 CheckHumanCertifyTimeout(5)；
//   N107 NotResponseUserTimeout(6)；N1 DDS::RegistNextTimer(11)/UCE::registNextTimer(11)/
//   SDCI::RegistNextTimer(20)；N7 TimerDBUpdate(12,NEAR)/TimerStamina(14,NEAR)/
//   GeneratePowerUpNotiTime(17)；N82 StatisticsLog(28)/DungeonInoutCloseTime(60)/
//   UpdateCompoundEmblemStatics::dispatch_sig(81)/DungeonInoutOpenTime(200)/
//   CheckConn(219)/ScheduleGuildWarEnd(271)；N82/N86 ScheduleTenSecond(65, p167 CLEAR)；
//   N85 GeneratePowerUp::dispatch_sig(71, x87)；N82/N93 FinishItemEffect(135)；
//   N83/N82 TimerDispatcher::C1(1016)。逐件明细见 /tmp/b28d_solo_TimerDispatcher_result.tsv。
// 新范式（已实测）：guard+register 汇聚变量改「if/else 包裹 + 作用域止于 return 前」
//   ⇒ dtor/EH 尾块落共享尾（LightServerNotice 6→0、DDS 20→0）；
//   P225 while+return 复现 setne 物化；P231 纯 register 复现 edi/esi/ebx；
//   P221 全表达式临时链翻聚合/标量槽序；P0 内联消 slot round-trip。
// .h 改动（已 R2 广播）：CGameManager.h +`friend class TimerScheduleTenSecond;`（零布局/
//   sizeof/mangled 影响，消费点仅本 TU）。P0：全文件无 `register asm` 绑定（清绑数 0）。
// ---------------------------------------------------------------------------
// [SweepTimerDispatcher2 2026-09-09 续修] 本轮 2 项硬修复 + 2 项符号级修复：
// ① 判据㉓ 符号绑定：147 个 Timer* 类的 C1/C2/D0/D1/D2 共 **735 符号** ORIG=W 我方=T
//    ⇒ 294 个 ctor/dtor 定义加 `__attribute__((weak))`（nm 双侧 0 mismatch）。
// ② weak 引入 EH landing（TimerDispatcher::C1 163×_ZdlPv/_Unwind_Resume、3119 insns）
//    ⇒ TimerDispatcher.h 147 类声明 + 本 .cpp 294 定义同加 `throw()`（探针证
//    weak+`nothrow` 无效、仅异常规格可消；ORIG 该 ctor 0 landing）⇒ C1 2000 insns、
//    gap -1458→-9、ndiff 1458→929。
// ③ TimerUpdateCompoundEmblemStatics::dispatch_sig：ORIG 14 个 getGradeCount 无具名局部
//    + 全表达式临时链（P227/P329）⇒ 130/130、ndiff 95→**0**、DIFF→AE。
// ④ TimerFinishItemEffect::dispatch_sig：uid/characNo 内联实参（P329/P0）⇒ ndiff 135→25
//    （残余 = sete/setne 物化方向 N18b + Send 返回类型 movzbl + edi/esi 角色 N7）。
// ⑤ 全 TU jcc 助记符序列核对 777 函数：仅 4 件差异（FIE / GeneratePowerUp x87 /
//    GeneratePowerUpNotiTime jl↔jg / Send_Ontime_Reward 镜像），无隐藏极性反写；
//    GI 键名 `_GLOBAL__I__ZN19Timer_DailySchedule12dispatch_sigEiij` 保持对齐。
// ===========================================================================
// [TimerDispatcher-3 2026-09-10] 判据①（jcc/jmp 目标「函数内指令下标」逐位比对）：
// 执行器 /tmp/td_jcc.py（AE 归一后两侧目标地址→下标，777 函数全扫）。11 件不匹配：
// ① 真修复 1 件：TimerGeneratePowerUp::dispatch_sig —— ORIG clamp 段是三目形
//    （`cmp;jg <保持臂>;[二次 size()];jmp <汇合>;保持臂: mov x→%eax;汇合: mov %eax→x`），
//    旧 if-赋值形在 4.4 恒产 `setl;test;je` 物化（探针 /tmp/td_probe4.cpp g6/g7 逐条
//    同构）⇒ 改三目后 DIFF→AE（TU 四数 ae 698→699、diff 14→13）。
// ② N104 EH 尾块相对序 3 件：TimerScheduleGuildWarEnd（6×`jmp <case 共享 D1Ev 中继>`）/
//    TimerCheckConn（1 条）/ TimerFinishItemEffect（i147 jmp 目标差 9）；目标逐条对应
//    无极性反写 ⇒ 无语义缺口。Send_Ontime_Reward 双 goto 复现形实测 4.4 反极性
//    `sete;je`+nop（差异 2 条 > 裸 if 的 1 条）⇒ 已回滚。
// ③ N86b/N1 3 件：DDS/SDCI/UCE 的 `%0x1e` 强度归约 lea↔add（探针 /tmp/td_probe5.cpp
//    r1..r6 六形恒 add）；④ N18b/N22 2 件：CloseTime `mov;test`↔`cmpl`（探针
//    td_probe_cmp/td_probe2 十四形恒 cmpl）、CHCT call 结果 eax↔edx 中转；
// ⑤ P196 折痕 1 件：GeneratePowerUpNotiTime jl↔jg 同目标；
// ⑥ 静态初始化噪音 1 件：_Z41__static_initialization_and_destruction_0ii（N123 豁免）。
// 剩余 18 件（diff 13，含 N123 豁免 1 件 + near 5）全部落 N 判词，每件判词区已附
// 判据①读数（分支目标逐条对应/无语义缺口）。
// ---------------------------------------------------------------------------
// [TD4 2026-09-10 · L12 复核（判据⑩槽序 + ①jcc + ②视图 + p167 三件套）] 本波 17 件全核：
//  · 翻正 1 件：**TimerCreature::dispatch_sig DIFF/NEAR → AE**。判据⑩（声明序=槽由深到浅）
//    把 `bool died;` 移到 user 之前 ⇒ died 落 -0x11、saveInventory -0x9、user -0x10 与
//    ORIG 0x08635f2f/0x08635f11/0x08635eb8 逐槽全等（ae 699→700、near 5→4）。
//  · 实测反劣并回滚 2 项试验：①CHCT `(unsigned)p2 < key` 交换操作数 ⇒ push %ebx +
//    `cmp %eax,%ebx; setb`（106→107 insns）②TimerStamina 三 getter 改 register 具名局部
//    ⇒ 四数不变（60/700/4/13）。
//  · p167：16 件逐符号核对**全 CLEAR** —— 残差仅 C1↔C2 / D1↔D2 同址别名（P93 伪差集），
//    无任何真 EXTRA/MISSING ⇒ 无语义缺口。
//  · 残余 N 族（每件函数体内判词均附 ORIG 地址 + 指令形证据）：
//    N86b/N1 ×3（DDS/SDCI/UCE 的 `%0x1e` 余数 lea↔add；本波探针 /tmp/probe_mod.cpp
//      f1..f6 六形含显式 `x - q*30` 恒产 add ⇒ 源形不可择）；
//    N7 ×3（TimerStamina/TimerDBUpdate 尾部三 getter edi/esi/ebx 角色轮换、
//      DBSelectCreateDnfEventInfo 立即数实参 vs this 装载序）；
//    N98 ×1（TimerStatisticsLog：4.1 声明序 vs 4.4 首次使用序，int 组无法推深）；
//    N104 ×3（ScheduleGuildWarEnd/CheckConn/FinishItemEffect：EH 尾块相对序）；
//    N115/N82（CloseTime/DDIO：outgoing 16B 预留 + tm/map 槽分区）；
//    N85/P196（GeneratePowerUpNotiTime）、N107（NotResponseUserTimeout 成员取址
//      add-vs-lea）、N93附（Send_Ontime_Reward 尾块镜像）、N22/N7（CHCT call 结果
//      eax↔edx 中转）。
//  · P227 族复核：16 件 cMyTrace 均已为**全表达式临时链**且 push 数与 ORIG 全等
//    （push 计数脚本 /tmp 比对：仅 StaticPool/createUser 类非本 TU cMyTrace 件不等）
//    ⇒ 非 P227 缺口。
// [A12-2026-09-10 · 本轮 16 件复核（判据①⑩⑪⑫ + 5 项实测）] 结论：16 件全 N 封顶，
// 零净改动（TU 四数 identical=60/ae=700/near=4/diff=13 改前=改后）。
//  · 判据⑪（/tmp/shift.py，必读 .o）：8 件 PURE SHIFT c=0x0（帧两侧全等、无槽平移）
//    —— ScheduleGuildWarEnd/CheckConn/FinishItemEffect/DDS.RegistNextTimer/
//    SDCI.RegistNextTimer/UCE.registNextTimer/TimerStamina/TimerDBUpdate；
//    TimerStatisticsLog 亦无平移（差异纯在标量 vs 对象分区的槽深，见③）。
//  · 判据⑫（/tmp/widthscan.py）：仅 DDICloseTime 报 cmp_l our=2/orig=1 —— 回 ORIG
//    同下标核形 = `mov slot,%eax; test %eax,%eax`（两段寄存器比较）vs 我方
//    `cmpl $0,slot` 单段 ⇒ 判据⑫明示的 N18b 反误判族，非类型宽度问题（不改类型）。
//  · ① D/E/F 三件（`%0x1e` 强度归约 lea↔add）**新增探针实证**：/tmp/a12/probe1.cpp
//    f0..f8（九形：单表达式/显式两乘积/移位拆形/unsigned/`*3*10`/裸 2x）在 4.4.7 下
//    恒产 `add %eax,%eax` 或 `imull $-30`，**无任何源形可出 4.1 的
//    `lea (%eax,%eax,1),%edx; mov %edx,%eax`** ⇒ N86b/N1 封顶（旧六形探针结论加固）。
//  · ② Timer_Send_Ontime_Reward（N93附）**实测翻案失败**：正极性双 goto + 标签后置
//    （`if(!=0) goto L_regist; goto L_join;` + L_regist/L_join 置 else 块之后）实测
//    54 条 —— 4.4 仍反转成 `je <join>` 且 call 后插 `jmp <join>; nop`（把标签块划为
//    独立块），比裸 if 的 52 条多 2 ⇒ 已回滚。ORIG 假路径 `jmp +0xb5(join)` 为 4.1
//    共享出口 mov 合并产物，4.4 恒发 per-return mov ⇒ P325 族不可达。
//  · ③ TimerStatisticsLog（N82附）**实测翻案失败**：三 getter 结果改 POD 聚合
//    （struct{int a,b,c;}）试探分区，实测仍落 -0x28/-0x24/-0x20（ORIG -0x44/-0x40/
//    -0x3c）、guard/trace 槽不变 ⇒ POD 聚合不改 4.4 分区（聚合对象优先低槽规律仅对
//    非平凡对象成立）⇒ 已回滚，维持 N82附。
//  · ④ TimerCheckHumanCertifyTimeout（N22/N7）**实测证明 cast 语义必需**：删调用点
//    `(unsigned int)` cast 后实测落**有符号** `setg`（p2 形参实为 int，非 unsigned；
//    106→104 insns，语义改变）⇒ 已回滚。ORIG `cmp %edx,%eax; seta` 的 call 结果留
//    %eax 属寄存器角色差，一次性旋钮已用尽（P248 赌输封顶）。
//  · ⑤ 语义核对：本 TU 全 777 函数 jcc/jmp 目标下标逐位比对（/tmp/td_jcc.py）后续
//    轮无新增可修项；p167 调用点差集 16 件全 CLEAR（残差仅 C1↔C2/D1↔D2 同址别名 P93）
//    ⇒ 无语义缺口、无真 bug。
// ===========================================================================
// ===========================================================================
// ======== 基类 ========


#include "TimerDispatcher/dispatch_sig.inc"


#include "TimerDispatcher/TimerDispatcher.inc"


#include "TimerDispatcher/dtor_TimerDispatcher.inc"


#include "TimerDispatcher/get_dispatcher.inc"


#include "TimerDispatcher/dispatch.inc"
{
    // [b28c-L18] N7 判词：ORIG `mov (%eax),%ebx` vptr 跨三条 entry 成员装载保活、
    // `call *%ebx`；4.4 恒间接 call 目标=%eax（成员装载改用 edx/ebx/ecx 轮换）。
    // 34=34 助记符序列全等、esp 槽值集全等，仅寄存器分配差，封顶。
    TimerSig* d = get_dispatcher(entry.msg);
    if (d == 0)
    {
        return 0;
    }
    return d->dispatch_sig(entry.param1, entry.param2, entry.param3);
}

// ======== 具体分发器生命周期 + dispatch_sig stub ========
// CParty +0xb24 内嵌 CBattle_Field 子对象局部视图（与 GmCmds_D.cpp/
// DisPatcherCore.cpp 同款；CParty.h 以 pad 占位，Main 裁定 TU 局部视图）。
struct CPartyBattleFieldView
{
    char m_pad[0xb24];
    CBattle_Field m_battleField;   // +0xb24
};
// stub 体待后续批次按 docs/class_func_reports 还原；地址=该函数 ORIG 起点。

__attribute__((weak)) TimerCheckConn::TimerCheckConn() throw() {}
__attribute__((weak)) TimerCheckConn::~TimerCheckConn() throw() {}
#include "TimerDispatcher/dispatch_sig_1.inc"


__attribute__((weak)) TimerDBUpdate::TimerDBUpdate() throw() {}
__attribute__((weak)) TimerDBUpdate::~TimerDBUpdate() throw() {}
#include "TimerDispatcher/dispatch_sig_2.inc"


__attribute__((weak)) TimerStamina::TimerStamina() throw() {}
__attribute__((weak)) TimerStamina::~TimerStamina() throw() {}
#include "TimerDispatcher/dispatch_sig_3.inc"


#include "TimerDispatcher/dispatch_sig_4.inc"


__attribute__((weak)) TimerMapLoading::TimerMapLoading() throw() {}
__attribute__((weak)) TimerMapLoading::~TimerMapLoading() throw() {}
#include "TimerDispatcher/dispatch_sig_5.inc"


__attribute__((weak)) TimerSetResult::TimerSetResult() throw() {}
__attribute__((weak)) TimerSetResult::~TimerSetResult() throw() {}
#include "TimerDispatcher/dispatch_sig_6.inc"


__attribute__((weak)) TimerDungeonClear::TimerDungeonClear() throw() {}
__attribute__((weak)) TimerDungeonClear::~TimerDungeonClear() throw() {}
#include "TimerDispatcher/dispatch_sig_7.inc"


__attribute__((weak)) TimerDungeonFail::TimerDungeonFail() throw() {}
__attribute__((weak)) TimerDungeonFail::~TimerDungeonFail() throw() {}
#include "TimerDispatcher/dispatch_sig_8.inc"


__attribute__((weak)) TimerPartyWalkOutLackUser::TimerPartyWalkOutLackUser() throw() {}
__attribute__((weak)) TimerPartyWalkOutLackUser::~TimerPartyWalkOutLackUser() throw() {}
#include "TimerDispatcher/dispatch_sig_9.inc"


__attribute__((weak)) TimerPVPWalkOutLackUser::TimerPVPWalkOutLackUser() throw() {}
__attribute__((weak)) TimerPVPWalkOutLackUser::~TimerPVPWalkOutLackUser() throw() {}
#include "TimerDispatcher/dispatch_sig_10.inc"


__attribute__((weak)) TimerFatigueReset::TimerFatigueReset() throw() {}
__attribute__((weak)) TimerFatigueReset::~TimerFatigueReset() throw() {}
#include "TimerDispatcher/updateAbnormalClientCount.inc"


#include "TimerDispatcher/dispatch_sig_11.inc"


#include "TimerDispatcher/RegistNextTimer.inc"


#include "TimerDispatcher/RegistTimer.inc"


// TimerAvatarExpire ctor W 0x0863bb3e / ~D1 W 0x0863bb8a / ~D0 W 0x0863bb5a
// （ORIG 仅 vptr：TimerSig 基类 vptr 后覆写本类 vtable，与其它 Timer_* 同构）。
__attribute__((weak)) TimerAvatarExpire::TimerAvatarExpire() throw() {}
__attribute__((weak)) TimerAvatarExpire::~TimerAvatarExpire() throw() {}
#include "TimerDispatcher/dispatch_sig_12.inc"


#include "TimerDispatcher/CheckExpiredAvatar.inc"


__attribute__((weak)) TimerStatisticsLog::TimerStatisticsLog() throw() {}
__attribute__((weak)) TimerStatisticsLog::~TimerStatisticsLog() throw() {}
#include "TimerDispatcher/dispatch_sig_13.inc"


__attribute__((weak)) TimerCardSelect::TimerCardSelect() throw() {}
__attribute__((weak)) TimerCardSelect::~TimerCardSelect() throw() {}
#include "TimerDispatcher/dispatch_sig_14.inc"


__attribute__((weak)) TimerClearDungeonReward::TimerClearDungeonReward() throw() {}
__attribute__((weak)) TimerClearDungeonReward::~TimerClearDungeonReward() throw() {}
#include "TimerDispatcher/dispatch_sig_15.inc"


__attribute__((weak)) TimerClearDungeonScoreScoll::TimerClearDungeonScoreScoll() throw() {}
__attribute__((weak)) TimerClearDungeonScoreScoll::~TimerClearDungeonScoreScoll() throw() {}
#include "TimerDispatcher/dispatch_sig_16.inc"


__attribute__((weak)) TimerClearDungeonCardSelectRight::TimerClearDungeonCardSelectRight() throw() {}
__attribute__((weak)) TimerClearDungeonCardSelectRight::~TimerClearDungeonCardSelectRight() throw() {}
#include "TimerDispatcher/dispatch_sig_17.inc"


__attribute__((weak)) TimerPremiumServiceOver::TimerPremiumServiceOver() throw() {}
__attribute__((weak)) TimerPremiumServiceOver::~TimerPremiumServiceOver() throw() {}
#include "TimerDispatcher/dispatch_sig_18.inc"


__attribute__((weak)) TimerPremiumServiceUpdate::TimerPremiumServiceUpdate() throw() {}
__attribute__((weak)) TimerPremiumServiceUpdate::~TimerPremiumServiceUpdate() throw() {}
#include "TimerDispatcher/dispatch_sig_19.inc"


__attribute__((weak)) TimerPremiumServiceStart::TimerPremiumServiceStart() throw() {}
__attribute__((weak)) TimerPremiumServiceStart::~TimerPremiumServiceStart() throw() {}
#include "TimerDispatcher/dispatch_sig_20.inc"


__attribute__((weak)) TimerCheckPaidPCRoomTerm::TimerCheckPaidPCRoomTerm() throw() {}
__attribute__((weak)) TimerCheckPaidPCRoomTerm::~TimerCheckPaidPCRoomTerm() throw() {}
#include "TimerDispatcher/dispatch_sig_21.inc"


__attribute__((weak)) TimerEPLPReturnVillage::TimerEPLPReturnVillage() throw() {}
__attribute__((weak)) TimerEPLPReturnVillage::~TimerEPLPReturnVillage() throw() {}
#include "TimerDispatcher/dispatch_sig_22.inc"


__attribute__((weak)) TimerEPLPCommandWait::TimerEPLPCommandWait() throw() {}
__attribute__((weak)) TimerEPLPCommandWait::~TimerEPLPCommandWait() throw() {}
#include "TimerDispatcher/dispatch_sig_23.inc"


__attribute__((weak)) TimerWarRoomWaitCount::TimerWarRoomWaitCount() throw() {}
__attribute__((weak)) TimerWarRoomWaitCount::~TimerWarRoomWaitCount() throw() {}
#include "TimerDispatcher/dispatch_sig_24.inc"


__attribute__((weak)) TimerWarRoomReadyLockCount::TimerWarRoomReadyLockCount() throw() {}
__attribute__((weak)) TimerWarRoomReadyLockCount::~TimerWarRoomReadyLockCount() throw() {}
#include "TimerDispatcher/dispatch_sig_25.inc"


__attribute__((weak)) TimerWarRoomBattlePauseCount::TimerWarRoomBattlePauseCount() throw() {}
__attribute__((weak)) TimerWarRoomBattlePauseCount::~TimerWarRoomBattlePauseCount() throw() {}
#include "TimerDispatcher/dispatch_sig_26.inc"


__attribute__((weak)) TimerWarRoomResultCount::TimerWarRoomResultCount() throw() {}
__attribute__((weak)) TimerWarRoomResultCount::~TimerWarRoomResultCount() throw() {}
#include "TimerDispatcher/dispatch_sig_27.inc"


__attribute__((weak)) TImerWarRoomTimeBomb::TImerWarRoomTimeBomb() throw() {}
__attribute__((weak)) TImerWarRoomTimeBomb::~TImerWarRoomTimeBomb() throw() {}
#include "TimerDispatcher/dispatch_sig_28.inc"


__attribute__((weak)) TimerWarRoomCheckFail::TimerWarRoomCheckFail() throw() {}
__attribute__((weak)) TimerWarRoomCheckFail::~TimerWarRoomCheckFail() throw() {}
#include "TimerDispatcher/dispatch_sig_29.inc"


__attribute__((weak)) TimerWarRoomChangeHost::TimerWarRoomChangeHost() throw() {}
__attribute__((weak)) TimerWarRoomChangeHost::~TimerWarRoomChangeHost() throw() {}
#include "TimerDispatcher/dispatch_sig_30.inc"


__attribute__((weak)) TimerBloodSmallRoundRegister::TimerBloodSmallRoundRegister() throw() {}
__attribute__((weak)) TimerBloodSmallRoundRegister::~TimerBloodSmallRoundRegister() throw() {}
#include "TimerDispatcher/dispatch_sig_31.inc"


__attribute__((weak)) TimerBloodPhaseRegister0::TimerBloodPhaseRegister0() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister0::~TimerBloodPhaseRegister0() throw() {}
#include "TimerDispatcher/dispatch_sig_32.inc"


__attribute__((weak)) TimerBloodPhaseRegister1::TimerBloodPhaseRegister1() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister1::~TimerBloodPhaseRegister1() throw() {}
#include "TimerDispatcher/dispatch_sig_33.inc"


__attribute__((weak)) TimerBloodPhaseRegister2::TimerBloodPhaseRegister2() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister2::~TimerBloodPhaseRegister2() throw() {}
#include "TimerDispatcher/dispatch_sig_34.inc"


__attribute__((weak)) TimerBloodPhaseRegister3::TimerBloodPhaseRegister3() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister3::~TimerBloodPhaseRegister3() throw() {}
#include "TimerDispatcher/dispatch_sig_35.inc"


__attribute__((weak)) TimerBloodPhaseRegister4::TimerBloodPhaseRegister4() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister4::~TimerBloodPhaseRegister4() throw() {}
#include "TimerDispatcher/dispatch_sig_36.inc"


__attribute__((weak)) TimerBloodPhaseRegister5::TimerBloodPhaseRegister5() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister5::~TimerBloodPhaseRegister5() throw() {}
#include "TimerDispatcher/dispatch_sig_37.inc"


__attribute__((weak)) TimerBloodPhaseRegister6::TimerBloodPhaseRegister6() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister6::~TimerBloodPhaseRegister6() throw() {}
#include "TimerDispatcher/dispatch_sig_38.inc"


__attribute__((weak)) TimerBloodPhaseRegister7::TimerBloodPhaseRegister7() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister7::~TimerBloodPhaseRegister7() throw() {}
#include "TimerDispatcher/dispatch_sig_39.inc"


__attribute__((weak)) TimerBloodPhaseRegister8::TimerBloodPhaseRegister8() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister8::~TimerBloodPhaseRegister8() throw() {}
#include "TimerDispatcher/dispatch_sig_40.inc"


__attribute__((weak)) TimerBloodPhaseRegister9::TimerBloodPhaseRegister9() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister9::~TimerBloodPhaseRegister9() throw() {}
#include "TimerDispatcher/dispatch_sig_41.inc"


__attribute__((weak)) TimerBloodCrazyMonster::TimerBloodCrazyMonster() throw() {}
__attribute__((weak)) TimerBloodCrazyMonster::~TimerBloodCrazyMonster() throw() {}
#include "TimerDispatcher/dispatch_sig_42.inc"


__attribute__((weak)) TimerBloodDungeonClearReward::TimerBloodDungeonClearReward() throw() {}
__attribute__((weak)) TimerBloodDungeonClearReward::~TimerBloodDungeonClearReward() throw() {}
#include "TimerDispatcher/dispatch_sig_43.inc"


__attribute__((weak)) TimerUltimateNextRoundStart::TimerUltimateNextRoundStart() throw() {}
__attribute__((weak)) TimerUltimateNextRoundStart::~TimerUltimateNextRoundStart() throw() {}
#include "TimerDispatcher/dispatch_sig_44.inc"

__attribute__((weak)) TimerEventChristmas::TimerEventChristmas() throw() {}
__attribute__((weak)) TimerEventChristmas::~TimerEventChristmas() throw() {}
#include "TimerDispatcher/dispatch_sig_45.inc"


__attribute__((weak)) TimerStatisticsDetailChannelInfo::TimerStatisticsDetailChannelInfo() throw() {}
__attribute__((weak)) TimerStatisticsDetailChannelInfo::~TimerStatisticsDetailChannelInfo() throw() {}
#include "TimerDispatcher/dispatch_sig_46.inc"

#include "TimerDispatcher/RegistNextTimer_1.inc"


__attribute__((weak)) TimerStatisticLevel::TimerStatisticLevel() throw() {}
__attribute__((weak)) TimerStatisticLevel::~TimerStatisticLevel() throw() {}
#include "TimerDispatcher/dispatch_sig_47.inc"


__attribute__((weak)) TimerScheduleOneHour::TimerScheduleOneHour() throw() {}
__attribute__((weak)) TimerScheduleOneHour::~TimerScheduleOneHour() throw() {}
#include "TimerDispatcher/dispatch_sig_48.inc"


__attribute__((weak)) TimerSaveLogGamechannel::TimerSaveLogGamechannel() throw() {}
__attribute__((weak)) TimerSaveLogGamechannel::~TimerSaveLogGamechannel() throw() {}
#include "TimerDispatcher/dispatch_sig_49.inc"


__attribute__((weak)) TimerQueryCounter::TimerQueryCounter() throw() {}
__attribute__((weak)) TimerQueryCounter::~TimerQueryCounter() throw() {}
#include "TimerDispatcher/dispatch_sig_50.inc"


#include "TimerDispatcher/RegistNextTimer_2.inc"


__attribute__((weak)) TimerNatType::TimerNatType() throw() {}
__attribute__((weak)) TimerNatType::~TimerNatType() throw() {}
#include "TimerDispatcher/dispatch_sig_51.inc"

#include "TimerDispatcher/RegistNextTimer_3.inc"


__attribute__((weak)) TimerNatTypeUser::TimerNatTypeUser() throw() {}
__attribute__((weak)) TimerNatTypeUser::~TimerNatTypeUser() throw() {}
#include "TimerDispatcher/dispatch_sig_52.inc"


#include "TimerDispatcher/RegistNextTimer_4.inc"


__attribute__((weak)) TimerCheckMailTimeOut::TimerCheckMailTimeOut() throw() {}
__attribute__((weak)) TimerCheckMailTimeOut::~TimerCheckMailTimeOut() throw() {}
#include "TimerDispatcher/dispatch_sig_53.inc"


__attribute__((weak)) TimerCreature::TimerCreature() throw() {}
__attribute__((weak)) TimerCreature::~TimerCreature() throw() {}
#include "TimerDispatcher/dispatch_sig_54.inc"


__attribute__((weak)) TimerForbidChat::TimerForbidChat() throw() {}
__attribute__((weak)) TimerForbidChat::~TimerForbidChat() throw() {}
#include "TimerDispatcher/dispatch_sig_55.inc"


__attribute__((weak)) TimerForbidMove::TimerForbidMove() throw() {}
__attribute__((weak)) TimerForbidMove::~TimerForbidMove() throw() {}
#include "TimerDispatcher/dispatch_sig_56.inc"


__attribute__((weak)) TimerScheduleFiveMinPerUser::TimerScheduleFiveMinPerUser() throw() {}
__attribute__((weak)) TimerScheduleFiveMinPerUser::~TimerScheduleFiveMinPerUser() throw() {}
#include "TimerDispatcher/dispatch_sig_57.inc"


__attribute__((weak)) TimerScheduleTenMinPerUser::TimerScheduleTenMinPerUser() throw() {}
__attribute__((weak)) TimerScheduleTenMinPerUser::~TimerScheduleTenMinPerUser() throw() {}
#include "TimerDispatcher/dispatch_sig_58.inc"


__attribute__((weak)) TimerQueueSizeLog::TimerQueueSizeLog() throw() {}
__attribute__((weak)) TimerQueueSizeLog::~TimerQueueSizeLog() throw() {}
#include "TimerDispatcher/dispatch_sig_59.inc"


#include "TimerDispatcher/RegistNextTimer_5.inc"


__attribute__((weak)) TimerScheduleGuildWarEnd::TimerScheduleGuildWarEnd() throw() {}
__attribute__((weak)) TimerScheduleGuildWarEnd::~TimerScheduleGuildWarEnd() throw() {}
#include "TimerDispatcher/dispatch_sig_60.inc"


__attribute__((weak)) TimerSetPlayResult::TimerSetPlayResult() throw() {}
__attribute__((weak)) TimerSetPlayResult::~TimerSetPlayResult() throw() {}
#include "TimerDispatcher/dispatch_sig_61.inc"


__attribute__((weak)) TimerStartRelayBattle::TimerStartRelayBattle() throw() {}
__attribute__((weak)) TimerStartRelayBattle::~TimerStartRelayBattle() throw() {}
#include "TimerDispatcher/dispatch_sig_62.inc"

#include "TimerDispatcher/dispatch_sig_63.inc"


__attribute__((weak)) TimerEventCoinRefill::TimerEventCoinRefill() throw() {}
__attribute__((weak)) TimerEventCoinRefill::~TimerEventCoinRefill() throw() {}
#include "TimerDispatcher/dispatch_sig_64.inc"


__attribute__((weak)) TimerPvPDieUserRevive0::TimerPvPDieUserRevive0() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive0::~TimerPvPDieUserRevive0() throw() {}
#include "TimerDispatcher/dispatch_sig_65.inc"


__attribute__((weak)) TimerPvPDieUserRevive1::TimerPvPDieUserRevive1() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive1::~TimerPvPDieUserRevive1() throw() {}
#include "TimerDispatcher/dispatch_sig_66.inc"


__attribute__((weak)) TimerPvPDieUserRevive2::TimerPvPDieUserRevive2() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive2::~TimerPvPDieUserRevive2() throw() {}
#include "TimerDispatcher/dispatch_sig_67.inc"


__attribute__((weak)) TimerPvPDieUserRevive3::TimerPvPDieUserRevive3() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive3::~TimerPvPDieUserRevive3() throw() {}
#include "TimerDispatcher/dispatch_sig_68.inc"


__attribute__((weak)) TimerPvPDieUserRevive4::TimerPvPDieUserRevive4() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive4::~TimerPvPDieUserRevive4() throw() {}
#include "TimerDispatcher/dispatch_sig_69.inc"


__attribute__((weak)) TimerPvPDieUserRevive5::TimerPvPDieUserRevive5() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive5::~TimerPvPDieUserRevive5() throw() {}
#include "TimerDispatcher/dispatch_sig_70.inc"


__attribute__((weak)) TimerPvPDieUserRevive6::TimerPvPDieUserRevive6() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive6::~TimerPvPDieUserRevive6() throw() {}
#include "TimerDispatcher/dispatch_sig_71.inc"


__attribute__((weak)) TimerPvPDieUserRevive7::TimerPvPDieUserRevive7() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive7::~TimerPvPDieUserRevive7() throw() {}
#include "TimerDispatcher/dispatch_sig_72.inc"


__attribute__((weak)) TimerDungeonDataStatistic::TimerDungeonDataStatistic() throw() {}
__attribute__((weak)) TimerDungeonDataStatistic::~TimerDungeonDataStatistic() throw() {}
// ---- TimerDungeonDataStatistic ----
// 报告：docs/class_func_reports/TimerDungeonDataStatistic/{dispatch_sig,RegistNextTimer}.md

#include "TimerDispatcher/dispatch_sig_73.inc"


#include "TimerDispatcher/RegistNextTimer_6.inc"


__attribute__((weak)) TimerNotResponseUserTimeout::TimerNotResponseUserTimeout() throw() {}
__attribute__((weak)) TimerNotResponseUserTimeout::~TimerNotResponseUserTimeout() throw() {}
// ---- TimerNotResponseUserTimeout ----
// 报告：docs/class_func_reports/TimerNotResponseUserTimeout/dispatch_sig.md

#include "TimerDispatcher/dispatch_sig_74.inc"


__attribute__((weak)) TimerLightServerNotice::TimerLightServerNotice() throw() {}
__attribute__((weak)) TimerLightServerNotice::~TimerLightServerNotice() throw() {}
#include "TimerDispatcher/dispatch_sig_75.inc"


#include "TimerDispatcher/RegistNextTimer_7.inc"


__attribute__((weak)) TimerInsertUserPlayTime::TimerInsertUserPlayTime() throw() {}
__attribute__((weak)) TimerInsertUserPlayTime::~TimerInsertUserPlayTime() throw() {}
#include "TimerDispatcher/dispatch_sig_76.inc"


#include "TimerDispatcher/RegistNextTimer_8.inc"


__attribute__((weak)) TimerCheckForcedDisconnect::TimerCheckForcedDisconnect() throw() {}
__attribute__((weak)) TimerCheckForcedDisconnect::~TimerCheckForcedDisconnect() throw() {}
// ---- TimerCheckForcedDisconnect ----
// 报告：docs/class_func_reports/TimerCheckForcedDisconnect/{dispatch_sig,RegistNextTimer}.md

#include "TimerDispatcher/dispatch_sig_77.inc"


#include "TimerDispatcher/RegistNextTimer_9.inc"


__attribute__((weak)) TimerCheckPvPRoomUser::TimerCheckPvPRoomUser() throw() {}
__attribute__((weak)) TimerCheckPvPRoomUser::~TimerCheckPvPRoomUser() throw() {}
// ===== TimerCheckPvPRoomUser =====
// dispatch_sig ORIG _ZN21TimerCheckPvPRoomUser12dispatch_sigEiij @0x08636f90 (0xa9B)

#include "TimerDispatcher/dispatch_sig_78.inc"


__attribute__((weak)) TimerCheckRecvPvpEndResult::TimerCheckRecvPvpEndResult() throw() {}
__attribute__((weak)) TimerCheckRecvPvpEndResult::~TimerCheckRecvPvpEndResult() throw() {}
// ---- TimerCheckRecvPvpEndResult ----
// 报告：docs/class_func_reports/TimerCheckRecvPvpEndResult/dispatch_sig.md

#include "TimerDispatcher/dispatch_sig_79.inc"


__attribute__((weak)) TimerCheckHumanCertifyTimeout::TimerCheckHumanCertifyTimeout() throw() {}
__attribute__((weak)) TimerCheckHumanCertifyTimeout::~TimerCheckHumanCertifyTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_80.inc"


__attribute__((weak)) TimerUpdateItemGenerateStatistics::TimerUpdateItemGenerateStatistics() throw() {}
__attribute__((weak)) TimerUpdateItemGenerateStatistics::~TimerUpdateItemGenerateStatistics() throw() {}
#include "TimerDispatcher/dispatch_sig_81.inc"


#include "TimerDispatcher/registNextTimer.inc"


__attribute__((weak)) TimerUpdatePvPGrade::TimerUpdatePvPGrade() throw() {}
__attribute__((weak)) TimerUpdatePvPGrade::~TimerUpdatePvPGrade() throw() {}
#include "TimerDispatcher/dispatch_sig_82.inc"


#include "TimerDispatcher/registNextTimer_1.inc"


__attribute__((weak)) TimerDetectDisconnect::TimerDetectDisconnect() throw() {}
__attribute__((weak)) TimerDetectDisconnect::~TimerDetectDisconnect() throw() {}
#include "TimerDispatcher/dispatch_sig_83.inc"


__attribute__((weak)) TimerCompleteLoadAssaultTimeout::TimerCompleteLoadAssaultTimeout() throw() {}
__attribute__((weak)) TimerCompleteLoadAssaultTimeout::~TimerCompleteLoadAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_84.inc"


__attribute__((weak)) TimerConnectP2PAssaultTimeout::TimerConnectP2PAssaultTimeout() throw() {}
__attribute__((weak)) TimerConnectP2PAssaultTimeout::~TimerConnectP2PAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_85.inc"


__attribute__((weak)) TimerEndAssault::TimerEndAssault() throw() {}
__attribute__((weak)) TimerEndAssault::~TimerEndAssault() throw() {}
#include "TimerDispatcher/dispatch_sig_86.inc"


__attribute__((weak)) TimerCloseAssault::TimerCloseAssault() throw() {}
__attribute__((weak)) TimerCloseAssault::~TimerCloseAssault() throw() {}
#include "TimerDispatcher/dispatch_sig_87.inc"


#include "TimerDispatcher/registNextTimer_2.inc"


#include "TimerDispatcher/registNextTimer_3.inc"


#include "TimerDispatcher/registNextTimer_4.inc"


#include "TimerDispatcher/registNextTimer_5.inc"


__attribute__((weak)) TimerCompleteLoadAfterAssaultTimeout::TimerCompleteLoadAfterAssaultTimeout() throw() {}
__attribute__((weak)) TimerCompleteLoadAfterAssaultTimeout::~TimerCompleteLoadAfterAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_88.inc"


__attribute__((weak)) TimerConnectP2PAfterAssaultTimeout::TimerConnectP2PAfterAssaultTimeout() throw() {}
__attribute__((weak)) TimerConnectP2PAfterAssaultTimeout::~TimerConnectP2PAfterAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_89.inc"


#include "TimerDispatcher/registNextTimer_6.inc"


__attribute__((weak)) TimerDeathTower::TimerDeathTower() throw() {}
__attribute__((weak)) TimerDeathTower::~TimerDeathTower() throw() {}
#include "TimerDispatcher/dispatch_sig_90.inc"


__attribute__((weak)) TimerDeathTowerTopRankerReload::TimerDeathTowerTopRankerReload() throw() {}
__attribute__((weak)) TimerDeathTowerTopRankerReload::~TimerDeathTowerTopRankerReload() throw() {}
#include "TimerDispatcher/dispatch_sig_91.inc"


__attribute__((weak)) TimerScheduleTenSecond::TimerScheduleTenSecond() throw() {}
__attribute__((weak)) TimerScheduleTenSecond::~TimerScheduleTenSecond() throw() {}
#include "TimerDispatcher/dispatch_sig_92.inc"


__attribute__((weak)) TimerFinishItemEffect::TimerFinishItemEffect() throw() {}
__attribute__((weak)) TimerFinishItemEffect::~TimerFinishItemEffect() throw() {}
// ---- TimerFinishItemEffect ----
// 报告：docs/class_func_reports/TimerFinishItemEffect/{dispatch_sig,_processSepecialEffectContinuousItem}.md
// 类对象仅 vptr；头声明区现缺私有 helper（见 NEEDS_LOCAL_STATIC）。

#include "TimerDispatcher/dispatch_sig_93.inc"


#include "TimerDispatcher/_processSepecialEffectContinuousItem.inc"
                                                                 unsigned int itemIdx)
{
    // ORIG 0x08637d6b：mov 0x10(%ebp),%eax; cmp $0x1cd1,%eax; jne 尾。
    switch (itemIdx)
    {
    case 0x1cd1:
        user->SetExpAffectItemEffect(false, 0.0f);
        break;
    default:
        break;
    }
}

__attribute__((weak)) TimerStartMatching::TimerStartMatching() throw() {}
__attribute__((weak)) TimerStartMatching::~TimerStartMatching() throw() {}
// ORIG：PvP_Room+0x6e4 视图（PvP_Room.h 该槽现标 int；ORIG 实为 create_room
// 时 CMatchingSystem::GetMatchingSystem 登记的 IMatch 匹配器指针），
// 与 GmCmds_M.cpp CPvPStartCmd 的 PvpRoomMatcherSlot 同款模式。
struct TimerStartMatchingMatcherSlot
{
    char m_pad[0x6e4];
    IMatch* m_matcher;
};

#include "TimerDispatcher/dispatch_sig_94.inc"


#include "TimerDispatcher/registNextTimer_7.inc"


__attribute__((weak)) TimerGlobalEffectItemBegin::TimerGlobalEffectItemBegin() throw() {}
__attribute__((weak)) TimerGlobalEffectItemBegin::~TimerGlobalEffectItemBegin() throw() {}
#include "TimerDispatcher/dispatch_sig_95.inc"


__attribute__((weak)) TimerGlobalEffectItemTimeout::TimerGlobalEffectItemTimeout() throw() {}
__attribute__((weak)) TimerGlobalEffectItemTimeout::~TimerGlobalEffectItemTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_96.inc"


#include "TimerDispatcher/registTimer.inc"


__attribute__((weak)) TimerVendingMachineItemNotify::TimerVendingMachineItemNotify() throw() {}
__attribute__((weak)) TimerVendingMachineItemNotify::~TimerVendingMachineItemNotify() throw() {}
#include "TimerDispatcher/dispatch_sig_97.inc"


__attribute__((weak)) TimerConnectMonitorServer::TimerConnectMonitorServer() throw() {}
__attribute__((weak)) TimerConnectMonitorServer::~TimerConnectMonitorServer() throw() {}
// ===== TimerConnectMonitorServer =====
// dispatch_sig ORIG _ZN25TimerConnectMonitorServer12dispatch_sigEiij @0x086380bc (0xe3B)
// registNextTimer ORIG _ZN25TimerConnectMonitorServer15registNextTimerElii @0x086381a0 (0x41B)

#include "TimerDispatcher/dispatch_sig_98.inc"


#include "TimerDispatcher/registNextTimer_8.inc"


__attribute__((weak)) TimerAssaultSuperPowerState::TimerAssaultSuperPowerState() throw() {}
__attribute__((weak)) TimerAssaultSuperPowerState::~TimerAssaultSuperPowerState() throw() {}
#include "TimerDispatcher/dispatch_sig_99.inc"


__attribute__((weak)) TimerNPCMoodChange::TimerNPCMoodChange() throw() {}
__attribute__((weak)) TimerNPCMoodChange::~TimerNPCMoodChange() throw() {}
#include "TimerDispatcher/dispatch_sig_100.inc"


// ORIG T 0x086381ec (_ZN18TimerNPCMoodChange15registNextTimerEl)：

__attribute__((weak)) TimerElevatorDungeon::TimerElevatorDungeon() throw() {}
__attribute__((weak)) TimerElevatorDungeon::~TimerElevatorDungeon() throw() {}
#include "TimerDispatcher/dispatch_sig_101.inc"


__attribute__((weak)) TimerRefreshPowerWarProcessInfo::TimerRefreshPowerWarProcessInfo() throw() {}
__attribute__((weak)) TimerRefreshPowerWarProcessInfo::~TimerRefreshPowerWarProcessInfo() throw() {}
#include "TimerDispatcher/dispatch_sig_102.inc"


#include "TimerDispatcher/registNextTimer_9.inc"


// ---- TimerGeneratePowerUp ----
// 报告：docs/class_func_reports/TimerGeneratePowerUp/{dispatch_sig,generatePowerUp,registNextTimer,sendPowerUpInfo}.md
// ctor/dtor 反汇编：ORIG 0x0863cc5a（TimerSig + 2×vector + vtable 0x8ce89e8）/
//   0x0863f156（D1）/ 0x0863f1e0（D0）。dispatcher 槽 +0x1f0（ORIG ctor 0x086321d6）。
// 数据字段（CDataManager.m_powerParameter 相对 +0xa628）：
//   m_powerUpGeneratePeriod(+0x48→dm+0xa670)、m_powerUpRatio(+0x1c→dm+0xa644)、
//   m_powerUpPointMod(+0xa8→dm+0xa6d0)。

__attribute__((weak)) TimerGeneratePowerUp::TimerGeneratePowerUp() {}
__attribute__((weak)) TimerGeneratePowerUp::~TimerGeneratePowerUp() throw() {}

#include "TimerDispatcher/dispatch_sig_103.inc"


#include "TimerDispatcher/generatePowerUp.inc"


#include "TimerDispatcher/sendPowerUpInfo.inc"


#include "TimerDispatcher/registNextTimer_10.inc"


	// ---- TimerGeneratePowerUpNotiTime ----
	// 报告：docs/class_func_reports/TimerGeneratePowerUpNotiTime/{dispatch_sig,registNextTimer,send_noti_time}.md

__attribute__((weak)) TimerGeneratePowerUpNotiTime::TimerGeneratePowerUpNotiTime() throw() {}
__attribute__((weak)) TimerGeneratePowerUpNotiTime::~TimerGeneratePowerUpNotiTime() throw() {}


#include "TimerDispatcher/dispatch_sig_104.inc"


#include "TimerDispatcher/registNextTimer_11.inc"


#include "TimerDispatcher/send_noti_time.inc"


__attribute__((weak)) TimerCheckPowerWarAbuser::TimerCheckPowerWarAbuser() throw() {}
__attribute__((weak)) TimerCheckPowerWarAbuser::~TimerCheckPowerWarAbuser() throw() {}
#include "TimerDispatcher/dispatch_sig_105.inc"


#include "TimerDispatcher/registNextTimer_12.inc"


__attribute__((weak)) TimerSetPunishUser::TimerSetPunishUser() throw() {}
__attribute__((weak)) TimerSetPunishUser::~TimerSetPunishUser() throw() {}
#include "TimerDispatcher/dispatch_sig_106.inc"


__attribute__((weak)) TimerSetTradePunishUser::TimerSetTradePunishUser() throw() {}
__attribute__((weak)) TimerSetTradePunishUser::~TimerSetTradePunishUser() throw() {}
#include "TimerDispatcher/dispatch_sig_107.inc"


__attribute__((weak)) TimerRevivalCharac::TimerRevivalCharac() throw() {}
__attribute__((weak)) TimerRevivalCharac::~TimerRevivalCharac() throw() {}
#include "TimerDispatcher/dispatch_sig_108.inc"


__attribute__((weak)) TimerRequestExit::TimerRequestExit() throw() {}
__attribute__((weak)) TimerRequestExit::~TimerRequestExit() throw() {}
#include "TimerDispatcher/dispatch_sig_109.inc"


__attribute__((weak)) TimerVillageMonsterUpdateRate::TimerVillageMonsterUpdateRate() throw() {}
__attribute__((weak)) TimerVillageMonsterUpdateRate::~TimerVillageMonsterUpdateRate() throw() {}
#include "TimerDispatcher/dispatch_sig_110.inc"


__attribute__((weak)) TimerVillageMonsterRegen::TimerVillageMonsterRegen() throw() {}
__attribute__((weak)) TimerVillageMonsterRegen::~TimerVillageMonsterRegen() throw() {}
#include "TimerDispatcher/dispatch_sig_111.inc"


#include "TimerDispatcher/dispatch_sig_112.inc"


__attribute__((weak)) TimerVillageAttackedPlayTimeout::TimerVillageAttackedPlayTimeout() throw() {}
__attribute__((weak)) TimerVillageAttackedPlayTimeout::~TimerVillageAttackedPlayTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_113.inc"


__attribute__((weak)) TimerItemUnlockWaiting::TimerItemUnlockWaiting() throw() {}
__attribute__((weak)) TimerItemUnlockWaiting::~TimerItemUnlockWaiting() throw() {}
#include "TimerDispatcher/dispatch_sig_114.inc"


// ===== TimerUpdateCompoundEmblemStatics =====
__attribute__((weak)) TimerUpdateCompoundEmblemStatics::TimerUpdateCompoundEmblemStatics() throw() {}
__attribute__((weak)) TimerUpdateCompoundEmblemStatics::~TimerUpdateCompoundEmblemStatics() throw() {}
#include "TimerDispatcher/dispatch_sig_115.inc"


#include "TimerDispatcher/registNextTimer_13.inc"


__attribute__((weak)) TimerStartExpEventAfterVillageAttack::TimerStartExpEventAfterVillageAttack() throw() {}
__attribute__((weak)) TimerStartExpEventAfterVillageAttack::~TimerStartExpEventAfterVillageAttack() throw() {}
#include "TimerDispatcher/dispatch_sig_116.inc"


__attribute__((weak)) Timer_Security_NonClient_CheckResponse::Timer_Security_NonClient_CheckResponse() throw() {}
__attribute__((weak)) Timer_Security_NonClient_CheckResponse::~Timer_Security_NonClient_CheckResponse() throw() {}
#include "TimerDispatcher/dispatch_sig_117.inc"


__attribute__((weak)) Timer_Midnight::Timer_Midnight() throw() {}
__attribute__((weak)) Timer_Midnight::~Timer_Midnight() throw() {}
#include "TimerDispatcher/dispatch_sig_118.inc"


#include "TimerDispatcher/RegistNextTimer_10.inc"


__attribute__((weak)) TimerEndPowerWarVictoriousReward::TimerEndPowerWarVictoriousReward() throw() {}
__attribute__((weak)) TimerEndPowerWarVictoriousReward::~TimerEndPowerWarVictoriousReward() throw() {}
#include "TimerDispatcher/dispatch_sig_119.inc"


__attribute__((weak)) Timer_TenMinuteSchedule::Timer_TenMinuteSchedule() throw() {}
__attribute__((weak)) Timer_TenMinuteSchedule::~Timer_TenMinuteSchedule() throw() {}
#include "TimerDispatcher/dispatch_sig_120.inc"


__attribute__((weak)) Timer_UpdateOnlinePreliminaryTeam::Timer_UpdateOnlinePreliminaryTeam() throw() {}
__attribute__((weak)) Timer_UpdateOnlinePreliminaryTeam::~Timer_UpdateOnlinePreliminaryTeam() throw() {}
#include "TimerDispatcher/dispatch_sig_121.inc"


#include "TimerDispatcher/RegistNextTimer_11.inc"


__attribute__((weak)) Timer_SecretShopStatistic::Timer_SecretShopStatistic() throw() {}
__attribute__((weak)) Timer_SecretShopStatistic::~Timer_SecretShopStatistic() throw() {}
#include "TimerDispatcher/dispatch_sig_122.inc"


#include "TimerDispatcher/RegistNextTimer_12.inc"


__attribute__((weak)) Timer_DailySchedule::Timer_DailySchedule() throw() {}
__attribute__((weak)) Timer_DailySchedule::~Timer_DailySchedule() throw() {}

__attribute__((weak)) Timer_ConnectP2PPvPTimeout::Timer_ConnectP2PPvPTimeout() throw() {}
__attribute__((weak)) Timer_ConnectP2PPvPTimeout::~Timer_ConnectP2PPvPTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_123.inc"


__attribute__((weak)) Timer_RoutingItemWait::Timer_RoutingItemWait() throw() {}
__attribute__((weak)) Timer_RoutingItemWait::~Timer_RoutingItemWait() throw() {}
#include "TimerDispatcher/dispatch_sig_124.inc"


__attribute__((weak)) Timer_PcRoomAuthRetry::Timer_PcRoomAuthRetry() throw() {}
__attribute__((weak)) Timer_PcRoomAuthRetry::~Timer_PcRoomAuthRetry() throw() {}
// ===== Timer_PcRoomAuthRetry =====
// dispatch_sig ORIG _ZN21Timer_PcRoomAuthRetry12dispatch_sigEiij @0x08639924 (0xacB)
// registNextTimer ORIG _ZN21Timer_PcRoomAuthRetry15registNextTimerEliij @0x086399d0 (0x41B)

#include "TimerDispatcher/dispatch_sig_125.inc"


#include "TimerDispatcher/registNextTimer_14.inc"


__attribute__((weak)) Timer_DungeonInoutOpenTime::Timer_DungeonInoutOpenTime() throw() {}
__attribute__((weak)) Timer_DungeonInoutOpenTime::~Timer_DungeonInoutOpenTime() throw() {}

#include "TimerDispatcher/checkDungeonInoutTime.inc"
{
    const time_t nowSec = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm timeBuf;
    struct tm* localTm = localtime_r(&nowSec, &timeBuf);
    const int hour = localTm->tm_hour;
    const int min = localTm->tm_min;
    std::map<int, STDungeonInoutTime>::iterator it =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.find(dungeonIdx);
    if (it != G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end())
    {
        // [b28d-solo-TimerDispatcher2] P171 死写入照抄：ORIG 08639ab0 `mov (%eax),%eax;
        // mov %eax,-0xc(%ebp)` 读 pair 首字段（key）后不再使用（第 5 次 ptEv 来源）。
        const int itKey = it->first;
        (void)itKey;
        // [b28d-solo-TimerDispatcher] P56：ORIG 尾段 `mov $0; test %al; je; mov $1;
        // jmp; mov $0` = `if (…) return true; return false;`（直 return 表达式
        // 4.4 恒折直跳，无 bool 归一岛）。
        if (hour >= it->second.m_startHour && hour <= it->second.m_endHour
            && min >= it->second.m_startMin && min < it->second.m_endMin)
            return true;
    }
    return false;
}

#include "TimerDispatcher/dispatch_sig_126.inc"
{
    (void)p1; (void)p2;
    // ORIG 08639e20：getCurSec 结果单槽 -0x78（int/time_t 同宽），localtime_r 直取
    // 该槽地址；旧码 curT+nowSec 双具名多一槽两条 mov。
    const int curT = static_cast<int>(GlobalData::s_systemTime_.getCurSec());  // ORIG -0x78(%ebp)
    struct tm timeBuf;                                 // ORIG -0xac(%ebp)
    struct tm* localTm = localtime_r(reinterpret_cast<const time_t*>(&curT), &timeBuf); // ORIG ptr → -0x20(%ebp)
    const int hour = localTm->tm_hour;                 // ORIG -0x18(%ebp)（tm+0x8）
    const int min = localTm->tm_min;                   // ORIG -0x14(%ebp)（tm+0x4）
    // P0：ORIG 08639e4d `mov 0x14(%ebp),%eax; mov %eax,-0x7c(%ebp)` 且后续 6 处
    // 均读 -0x7c（find key 亦 lea -0x7c）⇒ 原码有 p3 的具名副本。
    const int dungeonIdx = static_cast<int>(p3);       // ORIG -0x7c(%ebp)
    if (dungeonIdx == 0)
    {
        return 0;
    }

    // ORIG：find/end 各自写完整表达式（两次 G_CDataManagerv 调用，不缓存）
    std::map<int, STDungeonInoutTime>::iterator it =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.find(dungeonIdx);
    if (it != G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end())
    {
        // second 自节点数据 +0x04 起：endHour@+8 / startHour@+4 /
        // startMin@+c / endMin@+10（对照 asm 取值偏移）
        if (it->second.m_endHour < hour)
        {
            localTm->tm_hour = it->second.m_startHour;   // ORIG 写 tm+0x8
            localTm->tm_min = it->second.m_startMin;     // ORIG 写 tm+0x4
            localTm->tm_sec = 0;
            // [fix] ORIG 08639f1a `addl $0x15180,-0x1c(%ebp)`（内存自增，紧跟
            // `mov %eax,-0x1c(%ebp)` 的 mktime 返回值 store）⇒ 原码两句式；
            // 旧码单表达式生 `add $0x15180,%eax`（ORIG-only addl / OURS-only add 各 1）。
            int nextOpenT = mktime(localTm);
            nextOpenT += 0x15180;   // +86400 秒（次日同刻）
            int delta = nextOpenT - curT;                // ORIG -0xc(%ebp)
            if (delta < 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__,
                               0x1445, 0);
                trace("ULTIMATE_LOG : TIMER OFF!!");
                return 0;
            }
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                        (TIMER_MESSAGE)0x93, delta, 0, dungeonIdx);
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0x1454, 0);
            trace("ULTIMATE_LOG : NEXT DAY TIMER ON!!");
            return 1;
        }
        localTm->tm_hour = hour;                          // ORIG 写 tm+0x8
        localTm->tm_min = it->second.m_endMin;            // ORIG 写 tm+0x4（v+0x10）
        localTm->tm_sec = 0x3b;
        int checkT = mktime(localTm);                     // ORIG -0x1c(%ebp)
        int delta = checkT - curT;                        // ORIG -0x10(%ebp)
        if (delta < 0)
        {
            delta = 0;
        }
        // [S1 实验] ORIG 0863a0a6 前段 `mov -0x78(%ebp),%ebx`（curT 跨 cMyTrace ctor
        // 保活，实参位 0x10(%esp) 读 ebx）⇒ P227 全表达式临时链形；旧码具名两语句。
        cMyTrace(__PRETTY_FUNCTION__, 0x1462, 0)(
            // [LeaTimer28Run 2026-09-11 · §6.5 串盲区] ORIG .rodata 实字节以 0x0a 结尾
            // （objdump -s 实证 `...CUR_T %d\n\0`）；旧源码写字面 `\\n`（反斜杠+n 两字符）
            // ⇒ 运行期日志尾多打一个字面 `\n`。与 0x1462 同族 site 一致化。
            "DELTA %d , CHECK_T %d , CUR_T %d\n", delta, checkT, curT);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x94, delta, 0, dungeonIdx);
        cMyTrace traceClose(__PRETTY_FUNCTION__,
                            0x146e, 0);
        traceClose("ULTIMATE_LOG : ULTIMATE DUNGEON CLOSE TIMER ON!!");
        if (!GlobalData::s_power_manager->IsPowerWarEventOn() &&
            !GlobalData::s_villageMonsterMgr->OnEvent())
        {
            // ORIG 082687fc：dm+0x68（m_serverParameter）isDungeonOpen(0x2aff)；
            // ORIG 0826868c（0863a15c..0863a176）：setDungeonOpen(**dungeonIdx**,true)
            // —— `mov -0x7c(%ebp),%ebx; call G_CDataManager; add $0x68,%eax;
            // movl $0x1,0x8(%esp); mov %ebx,0x4(%esp); mov %eax,(%esp)` 实证
            // 第1实参 = -0x7c 具名副本（Ghidra C 同为 local_80），旧码 0x2aff 是
            // 与 isDungeonOpen 混淆的错常量（真 bug：开启的是错误 dungeon 开关）。
            if (!G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen(dungeonIdx, true);
                G_GameWorld()->send_all_dungeon_inout_message(dungeonIdx);
            }
            cMyTrace traceOpen(__PRETTY_FUNCTION__,
                               0x1478, 0);
            traceOpen("ULTIMATE_LOG : ULTIMATE DUNGEON OPEN!!");
        }
    }
    return 1;
}

__attribute__((weak)) Timer_DungeonInoutCloseTime::Timer_DungeonInoutCloseTime() throw() {}
__attribute__((weak)) Timer_DungeonInoutCloseTime::~Timer_DungeonInoutCloseTime() throw() {}
#include "TimerDispatcher/dispatch_sig_127.inc"
{
    (void)p1; (void)p2;
    int dungeonIdx;   // [C05② 命中] 原 unsigned int（ORIG -0x58：声明序最先）
    time_t now = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    struct tm tmBuf;
    struct tm* ptm = localtime_r(&now, &tmBuf);
    const int curHour = ptm->tm_hour;
    const int curMin = ptm->tm_min;
    dungeonIdx = p3;   // ORIG -0x58 具名副本（P0 槽往返）
    if (dungeonIdx == 0)
        return 0;

    std::map<int, STDungeonInoutTime>::iterator it =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.find(dungeonIdx);
    if (it != G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end())
    {
        ptm->tm_hour = curHour + 1;
        // ORIG 0863a29c `mov 0xc(%eax),%edx`（Ghidra C：*(int*)(it->+0xc)）⇒
        // pair+0xc = second+8 = m_startMin（旧码 m_endMin 产 +0x10，与 ORIG 不符）。
        ptm->tm_min = it->second.m_startMin;
        ptm->tm_sec = 0;
        const time_t closeSec = mktime(ptm);
        const int delay = static_cast<int>(closeSec - now);

        if (delay < 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x14a3, 0);
            trace("ULTIMATE_LOG : TIMER OFF!!");
            return 0;
        }
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x93,
                                    delay, 0, dungeonIdx);
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x14b1, 0);
            trace("ULTIMATE_LOG : ULTIMATE DUNGEON OPEN TIMER ON!!");
        }

        // !(IsPowerWarEventOn() || s_villageMonsterMgr->OnEvent())
        if (!GlobalData::s_power_manager->IsPowerWarEventOn() &&
            !GlobalData::s_villageMonsterMgr->OnEvent())
        {
            G_CDataManager()->m_serverParameter.setDungeonOpen(dungeonIdx, false);
            G_GameWorld()->send_all_dungeon_inout_message(dungeonIdx);
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x14b8, 0);
                trace("ULTIMATE_LOG : ULTIMATE DUNGEON CLOSE!!");
            }
        }
    }
    return 1;
}

__attribute__((weak)) Timer_FairPvPRematch::Timer_FairPvPRematch() throw() {}
__attribute__((weak)) Timer_FairPvPRematch::~Timer_FairPvPRematch() throw() {}
// ===== Timer_FairPvPRematch =====
// dispatch_sig ORIG _ZN20Timer_FairPvPRematch12dispatch_sigEiij @0x0863a430 (0x97B)

#include "TimerDispatcher/dispatch_sig_128.inc"


__attribute__((weak)) Timer_ReturnUserTimeout::Timer_ReturnUserTimeout() throw() {}
__attribute__((weak)) Timer_ReturnUserTimeout::~Timer_ReturnUserTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_129.inc"


#include "TimerDispatcher/registNextTimer_15.inc"


__attribute__((weak)) Timer_HadesHeartBeat::Timer_HadesHeartBeat() throw() {}
__attribute__((weak)) Timer_HadesHeartBeat::~Timer_HadesHeartBeat() throw() {}
#include "TimerDispatcher/dispatch_sig_130.inc"


#include "TimerDispatcher/registNextTimer_16.inc"


__attribute__((weak)) Timer_TOD_Send_ClearReward::Timer_TOD_Send_ClearReward() throw() {}
__attribute__((weak)) Timer_TOD_Send_ClearReward::~Timer_TOD_Send_ClearReward() throw() {}
#include "TimerDispatcher/dispatch_sig_131.inc"


__attribute__((weak)) Timer_OneADayItemShop::Timer_OneADayItemShop() throw() {}
__attribute__((weak)) Timer_OneADayItemShop::~Timer_OneADayItemShop() throw() {}
#include "TimerDispatcher/dispatch_sig_132.inc"


__attribute__((weak)) Timer_Schedule_MidNight::Timer_Schedule_MidNight() throw() {}
__attribute__((weak)) Timer_Schedule_MidNight::~Timer_Schedule_MidNight() throw() {}
#include "TimerDispatcher/dispatch_sig_133.inc"


__attribute__((weak)) Timer_Send_Ontime_Reward::Timer_Send_Ontime_Reward() throw() {}
__attribute__((weak)) Timer_Send_Ontime_Reward::~Timer_Send_Ontime_Reward() throw() {}
// ===== Timer_Send_Ontime_Reward =====
// dispatch_sig ORIG _ZN24Timer_Send_Ontime_Reward12dispatch_sigEiij @0x0863aa90 (0xbcB)
// RegistNextTimer ORIG _ZN24Timer_Send_Ontime_Reward15RegistNextTimerEv @0x0863ab4c (0x45B)

#include "TimerDispatcher/dispatch_sig_134.inc"


#include "TimerDispatcher/RegistNextTimer_13.inc"


__attribute__((weak)) Timer_EventManagement::Timer_EventManagement() throw() {}
__attribute__((weak)) Timer_EventManagement::~Timer_EventManagement() throw() {}
#include "TimerDispatcher/dispatch_sig_135.inc"


__attribute__((weak)) Timer_QuickPartyWaitingPartyMatching::Timer_QuickPartyWaitingPartyMatching() throw() {}
__attribute__((weak)) Timer_QuickPartyWaitingPartyMatching::~Timer_QuickPartyWaitingPartyMatching() throw() {}
#include "TimerDispatcher/dispatch_sig_136.inc"


#include "TimerDispatcher/cancel_quick_party.inc"


__attribute__((weak)) Timer_QuickPartyWaitingStartGame::Timer_QuickPartyWaitingStartGame() throw() {}
__attribute__((weak)) Timer_QuickPartyWaitingStartGame::~Timer_QuickPartyWaitingStartGame() throw() {}
#include "TimerDispatcher/dispatch_sig_137.inc"


__attribute__((weak)) Timer_AssaultGhostTimer::Timer_AssaultGhostTimer() throw() {}
__attribute__((weak)) Timer_AssaultGhostTimer::~Timer_AssaultGhostTimer() throw() {}
#include "TimerDispatcher/dispatch_sig_138.inc"


__attribute__((weak)) Timer_GenerateCaptchaData::Timer_GenerateCaptchaData() throw() {}
__attribute__((weak)) Timer_GenerateCaptchaData::~Timer_GenerateCaptchaData() throw() {}
#include "TimerDispatcher/dispatch_sig_139.inc"

#include "TimerDispatcher/RegistNextTimer_14.inc"


// ===== Timer_TournamentDugeonClearStateNotiReward =====
__attribute__((weak)) Timer_TournamentDugeonClearStateNotiReward::Timer_TournamentDugeonClearStateNotiReward() throw() {}
__attribute__((weak)) Timer_TournamentDugeonClearStateNotiReward::~Timer_TournamentDugeonClearStateNotiReward() throw() {}
#include "TimerDispatcher/dispatch_sig_140.inc"


// ===== Timer_TournametDungeonClearStateSelect =====
__attribute__((weak)) Timer_TournametDungeonClearStateSelect::Timer_TournametDungeonClearStateSelect() throw() {}
__attribute__((weak)) Timer_TournametDungeonClearStateSelect::~Timer_TournametDungeonClearStateSelect() throw() {}
#include "TimerDispatcher/dispatch_sig_141.inc"


__attribute__((weak)) Timer_TowerOnTime::Timer_TowerOnTime() throw() {}
__attribute__((weak)) Timer_TowerOnTime::~Timer_TowerOnTime() throw() {}
#include "TimerDispatcher/dispatch_sig_142.inc"


__attribute__((weak)) Timer_NoticeEmergencyPatchMessage::Timer_NoticeEmergencyPatchMessage() throw()
{
    m_remainingNotiCount = 0xa;   // ORIG W 0x0863bc2e：movl $0xa,0x4(%eax)
}
__attribute__((weak)) Timer_NoticeEmergencyPatchMessage::~Timer_NoticeEmergencyPatchMessage() throw() {}
#include "TimerDispatcher/dispatch_sig_143.inc"

#include "TimerDispatcher/RegistNextTimer_15.inc"


__attribute__((weak)) advancealtar::Timer_MainTick::Timer_MainTick() throw() {}
__attribute__((weak)) advancealtar::Timer_MainTick::~Timer_MainTick() throw() {}
#include "TimerDispatcher/dispatch_sig_144.inc"


__attribute__((weak)) advancealtar::Timer_StageControl::Timer_StageControl() throw() {}
__attribute__((weak)) advancealtar::Timer_StageControl::~Timer_StageControl() throw() {}
#include "TimerDispatcher/dispatch_sig_145.inc"


#include "TimerDispatcher/RegistTimerCheckFinsh.inc"


#include "TimerDispatcher/RegistTimerClearReward.inc"


#include "TimerDispatcher/RegistTimerEPLP.inc"


#include "TimerDispatcher/RegistTimerEPLPReturnVillage.inc"


__attribute__((weak)) Timer_DungeonStatisticEntrance_Hour::Timer_DungeonStatisticEntrance_Hour() throw() {}
__attribute__((weak)) Timer_DungeonStatisticEntrance_Hour::~Timer_DungeonStatisticEntrance_Hour() throw() {}
#include "TimerDispatcher/dispatch_sig_146.inc"


__attribute__((weak)) Timer_RefreshServerState::Timer_RefreshServerState() throw() {}
__attribute__((weak)) Timer_RefreshServerState::~Timer_RefreshServerState() throw() {}
#include "TimerDispatcher/dispatch_sig_147.inc"


__attribute__((weak)) Timer_StayTimeEvent::Timer_StayTimeEvent() throw() {}
__attribute__((weak)) Timer_StayTimeEvent::~Timer_StayTimeEvent() throw() {}
// ===== Timer_StayTimeEvent =====
// dispatch_sig ORIG _ZN19Timer_StayTimeEvent12dispatch_sigEiij @0x0863b928 (0xb2B)

#include "TimerDispatcher/dispatch_sig_148.inc"


__attribute__((weak)) Timer_CreateDnfDbLoad::Timer_CreateDnfDbLoad() throw() {}
__attribute__((weak)) Timer_CreateDnfDbLoad::~Timer_CreateDnfDbLoad() throw() {}
#include "TimerDispatcher/dispatch_sig_149.inc"


#include "TimerDispatcher/makeRequest.inc"



// 类内已初始化（TimerDispatcher.h）；此处仅保留类外定义以维持 ORIG R 符号。
const unsigned int advancealtar::Timer_MainTick::kMainTickMs = 1000;  // ORIG R 0x8b6c5e4（头文件无类内初始化）
const unsigned int advancealtar::Timer_StageControl::kFailEPLPMs;   // ORIG R 0x8b6c5f4
const unsigned int advancealtar::Timer_StageControl::kClearEPLPMs;  // ORIG R 0x8b6c5f0
const unsigned int advancealtar::Timer_StageControl::kCheckFinishMs;// ORIG R 0x8b6c5e8
const unsigned int advancealtar::Timer_StageControl::kClearRewardMs;// ORIG R 0x8b6c5ec
const unsigned int advancealtar::Timer_StageControl::kEPLPReturnVillageMs; // ORIG R 0x8b6c5f8

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：三个 RegistNextTimer（报告全文已读，逐指令转写）
// ---------------------------------------------------------------------------

#include "TimerDispatcher/RegistNextTimer_16.inc"


#include "TimerDispatcher/RegistNextTimer_17.inc"
{
    // ORIG 0x0863a9b4：getCurSec 结果寄存器内 add 后单落槽 -0x28(%ebp)。
    time_t sec = (time_t)(GlobalData::s_systemTime_.getCurSec() + 0x15180);
    struct tm v;                                 // ORIG v@-0x24（比 sec 近端）
    struct tm* pt = localtime_r(&sec, &v);
    pt->tm_hour = 0;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    int next = (int)mktime(pt);
    int delta = next - GlobalData::s_systemTime_.getCurSec();
    // [b28c-31] ORIG 0x0863aa1c：sec=-0x28 / cMyTrace=-0x24（标量深、聚合浅）；
    // 具名 trace 4.4 反置（trace=-0x28/sec=-0x18，N82附）⇒ 试全表达式临时链（P221）。
    cMyTrace(__PRETTY_FUNCTION__, 0x1626, 0)(
        "RegistNextTimer Schedule_MidNight delta %d\n", delta);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x9a,
                                delta, 0, 0);
}

#include "TimerDispatcher/RegistNextTimer_18.inc"


#include "TimerDispatcher/_NS_NonClient_CheckResponse_1.inc"

