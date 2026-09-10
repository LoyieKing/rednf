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

// ==== UNLINKED_CALLEE 前向声明：符号均 ORIG nm -S --defined-only 实证（T/W），
// 定义由后续批次排期；链接缺口汇总见 /tmp/df_batches/TimerBatch.md ====
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


// GI 移位：dispatch_sig 原位于 Timer_DailySchedule 族内（3656 行区），纯移位上送
// 至本 TU 首个强实体 TimerDispatcher::C2 之前，使 _GLOBAL__I_ 后缀对齐 ORIG 的
// _GLOBAL__I__ZN19Timer_DailySchedule12dispatch_sigEiij。语义零变化。
#include "TimerDispatcher/dispatch_sig.inc"
{
    // ORIG 0x812c920: CDailyScheduleManager::CheckAndSendToAllUserSchedule(s_DailyScheduleManager); RegistNextTimer(); return 1
    GlobalData::s_DailyScheduleManager->CheckAndSendToAllUserSchedule();
    RegistNextTimer();
    return 1;
}

// ORIG 0x863120a：清零槽 0..0xae 后逐项 new 并按 ORIG 顺序写入槽位。
// §6.2 超大函数（1661 insns）验收：调用点差集 MISSING=0；EXTRA 仅 +2 operator
// delete / +2 _Unwind_Resume（new 表达式 EH 清理路径，N 类『new-expression
// landing-pad』编译器生成差，源码侧无合法解豁免）；gap 1661→1679(-18，全部为
// 上述 EH 清理块) ⇒ 判语义完成、byte 层 N 封顶（N83）。
// [b28d-30] 复核 DIFF：ctor 大件（N82/N83 族，ORIG 0x08635xxx 见 report）。
// [b28d-solo-TimerDispatcher] 判词 N83/N82 封顶：1661 insns、gap 1661→1679（+18 全为
// new-expression EH cleanup 块），调用点差集 MISSING=0；无合法源形。
// [SweepTimerDispatcher2 2026-09-09] **旧 N 判词作废**：根因是 146 个 Timer* 类 ctor
// 缺 nothrow 语义（ORIG 为头内 inline/W，4.1 判 nothrow）。修法 = 本 .cpp 定义加
// `__attribute__((weak))`（判据㉓ 735 符号 T→W）+ 声明/定义同加 `throw()`（消 weak 引入
// 的 landing），**唯一例外** `TimerGeneratePowerUp`（ctor 含 2 个 vector 成员，ORIG
// 0x086321d6 处确有 landing ⇒ 该 ctor 不加 throw()）。结果 **1661/1661、gap=0、ndiff=0
// （AE）**，帧 0x2c+push edi/esi 与 ORIG 全等。
#include "TimerDispatcher/TimerDispatcher.inc"
{
    for (int i = 0; i <= 0xae; ++i)
    {
        m_dispatchers[i] = 0;
    }
    m_dispatchers[0] = new TimerCheckConn();  // slot +0x000
    m_dispatchers[1] = new TimerDBUpdate();  // slot +0x004
    m_dispatchers[2] = new TimerStamina();  // slot +0x008
    m_dispatchers[11] = new TimerMapLoading();  // slot +0x02c
    m_dispatchers[12] = new TimerSetResult();  // slot +0x030
    m_dispatchers[13] = new TimerDungeonClear();  // slot +0x034
    m_dispatchers[14] = new TimerDungeonFail();  // slot +0x038
    m_dispatchers[15] = new TimerPartyWalkOutLackUser();  // slot +0x03c
    m_dispatchers[49] = new TimerPVPWalkOutLackUser();  // slot +0x0c4
    m_dispatchers[80] = new TimerFatigueReset();  // slot +0x140
    m_dispatchers[81] = new TimerAvatarExpire();  // slot +0x144
    m_dispatchers[82] = new TimerStatisticsLog();  // slot +0x148
    m_dispatchers[16] = new TimerCardSelect();  // slot +0x040
    m_dispatchers[17] = new TimerClearDungeonReward();  // slot +0x044
    m_dispatchers[18] = new TimerClearDungeonScoreScoll();  // slot +0x048
    m_dispatchers[19] = new TimerClearDungeonCardSelectRight();  // slot +0x04c
    m_dispatchers[4] = new TimerPremiumServiceOver();  // slot +0x010
    m_dispatchers[5] = new TimerPremiumServiceUpdate();  // slot +0x014
    m_dispatchers[6] = new TimerPremiumServiceStart();  // slot +0x018
    m_dispatchers[7] = new TimerCheckPaidPCRoomTerm();  // slot +0x01c
    m_dispatchers[20] = new TimerEPLPReturnVillage();  // slot +0x050
    m_dispatchers[21] = new TimerEPLPCommandWait();  // slot +0x054
    m_dispatchers[65] = new TimerWarRoomWaitCount();  // slot +0x104
    m_dispatchers[66] = new TimerWarRoomReadyLockCount();  // slot +0x108
    m_dispatchers[67] = new TimerWarRoomBattlePauseCount();  // slot +0x10c
    m_dispatchers[68] = new TimerWarRoomResultCount();  // slot +0x110
    m_dispatchers[69] = new TImerWarRoomTimeBomb();  // slot +0x114
    m_dispatchers[70] = new TimerWarRoomCheckFail();  // slot +0x118
    m_dispatchers[71] = new TimerWarRoomChangeHost();  // slot +0x11c
    m_dispatchers[22] = new TimerBloodSmallRoundRegister();  // slot +0x058
    m_dispatchers[23] = new TimerBloodPhaseRegister0();  // slot +0x05c
    m_dispatchers[24] = new TimerBloodPhaseRegister1();  // slot +0x060
    m_dispatchers[25] = new TimerBloodPhaseRegister2();  // slot +0x064
    m_dispatchers[26] = new TimerBloodPhaseRegister3();  // slot +0x068
    m_dispatchers[27] = new TimerBloodPhaseRegister4();  // slot +0x06c
    m_dispatchers[28] = new TimerBloodPhaseRegister5();  // slot +0x070
    m_dispatchers[29] = new TimerBloodPhaseRegister6();  // slot +0x074
    m_dispatchers[30] = new TimerBloodPhaseRegister7();  // slot +0x078
    m_dispatchers[31] = new TimerBloodPhaseRegister8();  // slot +0x07c
    m_dispatchers[32] = new TimerBloodPhaseRegister9();  // slot +0x080
    m_dispatchers[33] = new TimerBloodCrazyMonster();  // slot +0x084
    m_dispatchers[34] = new TimerBloodDungeonClearReward();  // slot +0x088
    m_dispatchers[35] = new TimerBloodDungeonClearReward();  // slot +0x08c
    m_dispatchers[36] = new TimerBloodDungeonClearReward();  // slot +0x090
    m_dispatchers[37] = new TimerBloodDungeonClearReward();  // slot +0x094
    m_dispatchers[38] = new TimerUltimateNextRoundStart();  // slot +0x098
    m_dispatchers[83] = new TimerEventChristmas();  // slot +0x14c
    m_dispatchers[84] = new TimerStatisticsDetailChannelInfo();  // slot +0x150
    m_dispatchers[85] = new TimerStatisticLevel();  // slot +0x154
    m_dispatchers[86] = new TimerScheduleOneHour();  // slot +0x158
    m_dispatchers[88] = new TimerSaveLogGamechannel();  // slot +0x160
    m_dispatchers[89] = new TimerQueryCounter();  // slot +0x164
    m_dispatchers[90] = new TimerNatType();  // slot +0x168
    m_dispatchers[91] = new TimerNatTypeUser();  // slot +0x16c
    m_dispatchers[8] = new TimerCheckMailTimeOut();  // slot +0x020
    m_dispatchers[92] = new TimerCreature();  // slot +0x170
    m_dispatchers[93] = new TimerForbidChat();  // slot +0x174
    m_dispatchers[94] = new TimerForbidMove();  // slot +0x178
    m_dispatchers[95] = new TimerScheduleFiveMinPerUser();  // slot +0x17c
    m_dispatchers[96] = new TimerScheduleTenMinPerUser();  // slot +0x180
    m_dispatchers[97] = new TimerQueueSizeLog();  // slot +0x184
    m_dispatchers[98] = new TimerScheduleGuildWarEnd();  // slot +0x188
    m_dispatchers[41] = new TimerSetPlayResult();  // slot +0x0a4
    m_dispatchers[99] = new TimerStartRelayBattle();  // slot +0x18c
    m_dispatchers[100] = new TimerBossDieTimeout();  // slot +0x190
    m_dispatchers[101] = new TimerEventCoinRefill();  // slot +0x194
    m_dispatchers[51] = new TimerPvPDieUserRevive0();  // slot +0x0cc
    m_dispatchers[52] = new TimerPvPDieUserRevive1();  // slot +0x0d0
    m_dispatchers[53] = new TimerPvPDieUserRevive2();  // slot +0x0d4
    m_dispatchers[54] = new TimerPvPDieUserRevive3();  // slot +0x0d8
    m_dispatchers[55] = new TimerPvPDieUserRevive4();  // slot +0x0dc
    m_dispatchers[56] = new TimerPvPDieUserRevive5();  // slot +0x0e0
    m_dispatchers[57] = new TimerPvPDieUserRevive6();  // slot +0x0e4
    m_dispatchers[58] = new TimerPvPDieUserRevive7();  // slot +0x0e8
    m_dispatchers[102] = new TimerDungeonDataStatistic();  // slot +0x198
    m_dispatchers[103] = new TimerNotResponseUserTimeout();  // slot +0x19c
    m_dispatchers[104] = new TimerLightServerNotice();  // slot +0x1a0
    m_dispatchers[105] = new TimerInsertUserPlayTime();  // slot +0x1a4
    m_dispatchers[106] = new TimerCheckForcedDisconnect();  // slot +0x1a8
    m_dispatchers[60] = new TimerCheckPvPRoomUser();  // slot +0x0f0
    m_dispatchers[61] = new TimerCheckRecvPvpEndResult();  // slot +0x0f4
    m_dispatchers[9] = new TimerCheckHumanCertifyTimeout();  // slot +0x024
    m_dispatchers[107] = new TimerUpdateItemGenerateStatistics();  // slot +0x1ac
    m_dispatchers[108] = new TimerUpdatePvPGrade();  // slot +0x1b0
    m_dispatchers[109] = new TimerDetectDisconnect();  // slot +0x1b4
    m_dispatchers[110] = new TimerCompleteLoadAssaultTimeout();  // slot +0x1b8
    m_dispatchers[111] = new TimerConnectP2PAssaultTimeout();  // slot +0x1bc
    m_dispatchers[112] = new TimerEndAssault();  // slot +0x1c0
    m_dispatchers[113] = new TimerCloseAssault();  // slot +0x1c4
    m_dispatchers[42] = new TimerCompleteLoadAfterAssaultTimeout();  // slot +0x0a8
    m_dispatchers[114] = new TimerConnectP2PAfterAssaultTimeout();  // slot +0x1c8
    m_dispatchers[73] = new TimerDeathTower();  // slot +0x124
    m_dispatchers[75] = new TimerDeathTower();  // slot +0x12c
    m_dispatchers[76] = new TimerDeathTower();  // slot +0x130
    m_dispatchers[77] = new TimerDeathTower();  // slot +0x134
    m_dispatchers[78] = new TimerDeathTower();  // slot +0x138
    m_dispatchers[74] = new TimerDeathTower();  // slot +0x128
    m_dispatchers[115] = new TimerDeathTowerTopRankerReload();  // slot +0x1cc
    m_dispatchers[87] = new TimerScheduleTenSecond();  // slot +0x15c
    m_dispatchers[118] = new TimerFinishItemEffect();  // slot +0x1d8
    m_dispatchers[121] = new TimerStartMatching();  // slot +0x1e4
    m_dispatchers[119] = new TimerGlobalEffectItemBegin();  // slot +0x1dc
    m_dispatchers[120] = new TimerGlobalEffectItemTimeout();  // slot +0x1e0
    m_dispatchers[10] = new TimerVendingMachineItemNotify();  // slot +0x028
    m_dispatchers[122] = new TimerConnectMonitorServer();  // slot +0x1e8
    m_dispatchers[126] = new TimerAssaultSuperPowerState();  // slot +0x1f8
    m_dispatchers[127] = new TimerAssaultSuperPowerState();  // slot +0x1fc
    m_dispatchers[116] = new TimerNPCMoodChange();  // slot +0x1d0
    m_dispatchers[40] = new TimerElevatorDungeon();  // slot +0x0a0
    m_dispatchers[123] = new TimerRefreshPowerWarProcessInfo();  // slot +0x1ec
    m_dispatchers[124] = new TimerGeneratePowerUp();  // slot +0x1f0（ORIG ctor 0x086321d6 实证）
    m_dispatchers[125] = new TimerGeneratePowerUpNotiTime();  // slot +0x1f4
    m_dispatchers[128] = new TimerCheckPowerWarAbuser();  // slot +0x200
    m_dispatchers[129] = new TimerSetPunishUser();  // slot +0x204
    m_dispatchers[151] = new TimerSetTradePunishUser();  // slot +0x25c
    m_dispatchers[130] = new TimerRevivalCharac();  // slot +0x208
    m_dispatchers[132] = new TimerRequestExit();  // slot +0x210
    m_dispatchers[133] = new TimerVillageMonsterUpdateRate();  // slot +0x214
    m_dispatchers[134] = new TimerVillageMonsterRegen();  // slot +0x218
    m_dispatchers[135] = new TimerStartVillageAttacked();  // slot +0x21c
    m_dispatchers[39] = new TimerVillageAttackedPlayTimeout();  // slot +0x09c
    m_dispatchers[136] = new TimerItemUnlockWaiting();  // slot +0x220
    m_dispatchers[137] = new TimerUpdateCompoundEmblemStatics();  // slot +0x224
    m_dispatchers[138] = new TimerStartExpEventAfterVillageAttack();  // slot +0x228
    m_dispatchers[140] = new Timer_Security_NonClient_CheckResponse();  // slot +0x230
    m_dispatchers[141] = new Timer_Midnight();  // slot +0x234
    m_dispatchers[139] = new TimerEndPowerWarVictoriousReward();  // slot +0x22c
    m_dispatchers[142] = new Timer_TenMinuteSchedule();  // slot +0x238
    m_dispatchers[143] = new Timer_UpdateOnlinePreliminaryTeam();  // slot +0x23c
    m_dispatchers[144] = new Timer_SecretShopStatistic();  // slot +0x240
    m_dispatchers[145] = new Timer_DailySchedule();  // slot +0x244
    m_dispatchers[59] = new Timer_ConnectP2PPvPTimeout();  // slot +0x0ec
    m_dispatchers[43] = new Timer_RoutingItemWait();  // slot +0x0ac
    m_dispatchers[146] = new Timer_PcRoomAuthRetry();  // slot +0x248
    m_dispatchers[147] = new Timer_DungeonInoutOpenTime();  // slot +0x24c
    m_dispatchers[148] = new Timer_DungeonInoutCloseTime();  // slot +0x250
    m_dispatchers[62] = new Timer_FairPvPRematch();  // slot +0x0f8
    m_dispatchers[149] = new Timer_ReturnUserTimeout();  // slot +0x254
    m_dispatchers[150] = new Timer_HadesHeartBeat();  // slot +0x258
    m_dispatchers[44] = new Timer_TOD_Send_ClearReward();  // slot +0x0b0
    m_dispatchers[153] = new Timer_OneADayItemShop();  // slot +0x264
    m_dispatchers[154] = new Timer_Schedule_MidNight();  // slot +0x268
    m_dispatchers[155] = new Timer_Send_Ontime_Reward();  // slot +0x26c
    m_dispatchers[156] = new Timer_EventManagement();  // slot +0x270
    m_dispatchers[158] = new Timer_QuickPartyWaitingPartyMatching();  // slot +0x278
    m_dispatchers[45] = new Timer_QuickPartyWaitingStartGame();  // slot +0x0b4
    m_dispatchers[160] = new Timer_AssaultGhostTimer();  // slot +0x280
    m_dispatchers[161] = new Timer_GenerateCaptchaData();  // slot +0x284
    m_dispatchers[46] = new Timer_TournamentDugeonClearStateNotiReward();  // slot +0x0b8
    m_dispatchers[47] = new Timer_TournametDungeonClearStateSelect();  // slot +0x0bc
    m_dispatchers[163] = new Timer_TowerOnTime();  // slot +0x28c
    m_dispatchers[164] = new Timer_TowerOnTime();  // slot +0x290
    m_dispatchers[165] = new Timer_TowerOnTime();  // slot +0x294
    m_dispatchers[162] = new Timer_NoticeEmergencyPatchMessage();  // slot +0x288
    m_dispatchers[166] = new advancealtar::Timer_MainTick();  // slot +0x298
    m_dispatchers[167] = new advancealtar::Timer_StageControl();  // slot +0x29c
    m_dispatchers[168] = new advancealtar::Timer_StageControl();  // slot +0x2a0
    m_dispatchers[169] = new advancealtar::Timer_StageControl();  // slot +0x2a4
    m_dispatchers[170] = new advancealtar::Timer_StageControl();  // slot +0x2a8
    m_dispatchers[171] = new Timer_DungeonStatisticEntrance_Hour();  // slot +0x2ac
    m_dispatchers[172] = new Timer_RefreshServerState();  // slot +0x2b0
    m_dispatchers[173] = new Timer_StayTimeEvent();  // slot +0x2b4
    m_dispatchers[174] = new Timer_CreateDnfDbLoad();  // slot +0x2b8
}

#include "TimerDispatcher/dtor_TimerDispatcher.inc"
{
    // [b28c-L18] N7 判词：虚删除 `call *%ecx`（ORIG vtable 载入 %ecx、槽读 %edx、
    // 元素基址 %eax）在 4.4 恒把间接 call 目标放 %eax（元素/槽角色轮换）——
    // 36=36 助记符全等仅寄存器名差，语义同，勿再耗轮次。
    for (int i = 0; i <= 0xae; ++i)
    {
        if (m_dispatchers[i] != 0)
        {
            delete m_dispatchers[i];
        }
        m_dispatchers[i] = 0;
    }
}

// ORIG 0x86329f6：空槽返回 0。
#include "TimerDispatcher/get_dispatcher.inc"
{
    // ORIG 0x086329f6：非空分支为 fall-through（先 reload 再 jmp），空走 mov $0。
    if (m_dispatchers[msg] != 0)
    {
        return m_dispatchers[msg];
    }
    return 0;
}

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
// [b28d-30] 复核 DIFF：callee-saved/帧继承跨版本差（N7/N82/P210 族），ORIG 0x0863xxx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82 封顶：全帧槽平移（std::string 槽 -0x4c↔-0x24、
// user -0x24↔-0x1c）+ logFormat varargs 装载差；助记符族同。
// [TimerDispatcher-2 2026-09-09 复核] gap=1 = 1 条 ORIG-only jmp（EH 尾块相对序 N104）；
// 另 esi↔ebx 角色互换（ORIG GetUID→esi / GetCheckConnInterval→ebx vs 4.4 反向）
// ⇒ N7（N109 角色互换）⇒ 封顶。
// [TimerDispatcher-3 2026-09-10 判据①] 14 条 jcc 目标逐条对应；位移差全部来自 ORIG
// `jmp <跳过 catch 的共享 D1Ev 块>`（N104 EH 尾块相对序）与 ss 析构块位置，语义等价。
#include "TimerDispatcher/dispatch_sig_1.inc"
{
    // ORIG 0x8632bca：getUser(p1, 2)（increId 形参被 getUser 忽略，常量 2 为占位）。
    CUser* user = G_CGameManager()->getUser(p1, 2);
    // ORIG 0x8632be4 `cmpl $0x0,-0x24; jne` + 0x8632c05 `setne %al; test; je`
    // ⇒ 两条独立守卫（P133）；旧码 `||` 合并产 mov$1/mov$0 物化岛（多 3 条）。
    // ORIG 0x8632bff：movzwl %ax 后与 p2 作 int 比较（unique_id 低 16 位）。
    if (user == 0)
    {
        return 0;
    }
    if (static_cast<unsigned short>(user->get_unique_id()) != p2)
    {
        return 0;
    }
    std::string accName(user->get_acc_name());
    char ip[0x10] = {0};
    user->m_network.GetPeerIP2(ip, 0x10);
    // ORIG 0x8632cc1 `mov %al,-0x1d(%ebp); cmpb $0x0,-0x1d(%ebp); je`
    // ⇒ 原码有具名 bool 局部（旧码直判 test %al 少 2 条）。
    bool noGameGuard = G_CGameManager()->IsNoGameGuardApplingUser(accName, ip);
    if (noGameGuard)
    {
        // P227 全表达式临时链：ORIG 08632cca 先求 c_str 结果驻 %ebx，再 ctor，最后压
        // 变参（两语句形把求值推到 ctor 之后 ⇒ 少 mov %eax,%ebx）。
        cMyTrace(__PRETTY_FUNCTION__, 0x3a4, 0)(
            "Try To Login By No Game Guard : %s", accName.c_str());
        return 1;
    }
    if (user->get_state() != 0)
    {
        if (user->get_connect_count() <= 2)
        {
            if (user->get_connect_count() > 1)
            {
                // P227：ORIG 08632d72 movswl→%esi（count）、08632d80 getCurCharacName→%ebx
                // 均在 ctor 之前求值保活。
                cMyTrace(__PRETTY_FUNCTION__, 0x3bc, 0)(
                    // [LeaTimer28Run 2026-09-11 · §6.5 串盲区] ORIG .rodata 实字节无句点
                    // （`User %s - no response %dth ping\0`，objdump -s 实证）；
                    // 旧源码多一个 '.' ⇒ 运行期日志文本与 ORIG 不同。
                    "User %s - no response %dth ping", user->getCurCharacName(),
                    static_cast<int>(user->get_connect_count()));
                // P0：ORIG 08632ddc `mov %eax,-0x1c(%ebp); mov -0x1c(%ebp),%eax` 槽往返
                // ⇒ 原码有具名局部（旧码内联实参少 2 条）。
                int resumeTime = GlobalData::s_systemTime_.getCurSec() +
                                 user->GetCheckConnInterval();
                user->SetResumeChecksumTime(resumeTime);
            }
            user->incre_check_count();
            // ORIG 无 SendCheckConnPacket 符号：内联构造空包头（put_header(0,0)+finalize）发送。
            PacketGuard packet;
            packet.put_header(0, 0);
            packet.finalize(true);
            user->Send(packet);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                        (TIMER_MESSAGE)0, user->GetCheckConnInterval(),
                                        user->get_unique_id(), 0);
        }
        else
        {
            // ORIG 0x8632ec4：连续 3 次无响应 → 日志 + 断线。
            LogManager::logFormat(1, __PRETTY_FUNCTION__, "TimerDispatcher.cpp", 0x40a,
                                  "User %s - no response during 3 times ping",
                                  user->getCurCharacName());
            user->DisConnSig(static_cast<DISCONN_SIG>(8), false, 0);
        }
    }
    return 1;
}

__attribute__((weak)) TimerDBUpdate::TimerDBUpdate() throw() {}
__attribute__((weak)) TimerDBUpdate::~TimerDBUpdate() throw() {}
// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x8631f0c 见 report。
// [b28d-solo-TimerDispatcher] 判词 N7/N82附 封顶：callee-saved 角色轮换（ORIG
// edi=characNo/esi=loginTick/ebx=UID；4.4 恒 esi/ebx/edi）+ 槽 -0x28↔-0x20。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；残差 = callee-saved 角色
// edi/esi/ebx 轮换（N7）⇒ 无语义缺口。
#include "TimerDispatcher/dispatch_sig_2.inc"
{
    // ORIG 0x8631f0c：getUser(p1, 3)（第二实参为常量 3，increId 占位）。
    CUser* user = G_CGameManager()->getUser(p1, 3);
    if (user == 0)
    {
        return 0;
    }
    // UpdateData/GetUID 为 CUser 成员（CUser 继承 CUserCharacInfo），保持 CUser*。
    // P0：ORIG 三个具名槽 -0x24(loginTick)/-0x20(curSec)/-0x1c(elapsedHours)。
    const int loginTick = user->GetLoginTick();
    const int curSec = static_cast<int>(GlobalData::s_systemTime_.getCurSec());
    const int elapsedHours = (curSec - loginTick) / 0xe10;
    // ORIG 08632fd8-0x8633013：范围判定 `> 0 && <= 1` / `> 0xb && <= 0xc`
    // （cmpl $0;jle / cmpl $1;jg），非 `== 1` 单比较形。
    if (elapsedHours > 0 && elapsedHours <= 1)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)0x23, 0, ENUM_PACKETCLASS_0);
    }
    else if (elapsedHours > 0xb && elapsedHours <= 0xc)
    {
        APSystem::CUserProc::ClearActionAndSendtoUser(
            user, (APSystem::_ActionGroupIndex)0x24, 0, ENUM_PACKETCLASS_0);
    }
    // ORIG 08633043：`cmp; setne %al; test %al,%al; je` ⇒ P225 while+body 形。
    while (user->GetLoginTick() != p2)
    {
        return 0;
    }
    if (p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3)
    {
        return 0;
    }
    user->UpdateData();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                (TIMER_MESSAGE)1, get_rand_int(300) + 600,
                                user->GetLoginTick(),
                                static_cast<unsigned int>(user->getCurCharacNo()));
    return 1;
}

__attribute__((weak)) TimerStamina::TimerStamina() throw() {}
__attribute__((weak)) TimerStamina::~TimerStamina() throw() {}
// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x08633c90 见 report。
// [b28d-solo-TimerDispatcher] 判词 N7 封顶：DIFF→NEAR（95/95 助记符全等）。
// 修法：①P0 删具名 `updateCount`（ORIG 无槽，08633d6c `mov %eax,%ebx` 后 cmp 直用）
// ——内联后 user 槽回 -0x1c（原 -0x20 全帧平移）；②P63 合并短路拆三条独立守卫
// （ORIG 08633cea/08633d09/08633d41 共享同一 leave 出口）；③求值序左操作数写
// GetCurCharacLuckPointUpdateTimeCount（ORIG 先求该 getter 入 %ebx）。
// 残差 = 尾部三 getter callee-saved 角色轮换（ORIG edi/esi/ebx vs 4.4 esi/ebx/edi），
// 实参压栈序相同 ⇒ N7。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；残差 = 尾部三 getter 结果
// edi/esi/ebx 保活序互换（N7）⇒ 无语义缺口。
#include "TimerDispatcher/dispatch_sig_3.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 4);   // ORIG 0x08633c90 第2参固定 4（p2 仅用于 loginTick 比对）
    // P63：ORIG 三段独立早退共享同一 leave 出口（08633cea/08633d09/08633d41 均
    // `mov $0,%eax; jmp +0x150`），故拆三条独立守卫而非合并短路。
    if (user == 0)
        return 0;
    if (user->GetLoginTick() != p2)
        return 0;
    if (p3 != 0 && static_cast<unsigned int>(user->get_charac_no(-1)) != p3)
        return 0;
    user->recover_stamina();
    user->IncCurCharacLuckPointUpdateTimeCount();
    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（m_84 / m_80 共 2 次）。
    // P0：ORIG updateCount 无栈槽（0x0b0 `mov %eax,%ebx` 后 cmp 直用）⇒ 原码无具名
    // 局部，表达式内联（旧具名形多占 -0x1c 槽使 user 槽 -0x1c→-0x20 全帧平移）。
    // P243 同族（求值序）：ORIG 先求 GetCurCharacLuckPointUpdateTimeCount（%ebx）后
    // 求 G_CDataManager ⇒ 左操作数写成该 getter（4.4 先求左）。
    if (user->GetCurCharacLuckPointUpdateTimeCount() >=
        G_CDataManager()->m_serverParameter.m_84)
    {
        user->IncCurCharacLuckPoint(G_CDataManager()->m_serverParameter.m_80);
        user->SetCurCharacLuckPointUpdateTimeCount(0);
    }
    // [TD4 2026-09-10 P231/P307 试验·失败] 三 getter 结果改 register 具名局部
    // （按 ORIG 求值序）后四数不变（60/700/4/13）⇒ ORIG 0x08633d6c..0x08633db3 的
    // %edi/%esi/%ebx 角色属 4.1 callee-saved 分配产物（N7），源码侧不可控 ⇒ 回滚内联实参形。
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, user->GetUID(),
                                (TIMER_MESSAGE)2, 0x3c, user->GetLoginTick(),
                                static_cast<unsigned int>(user->getCurCharacNo()));
    return 1;
}

#include "TimerDispatcher/dispatch_sig_4.inc"
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, 0, 0x32);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x32) != p2)
    {
        return 0;
    }
    room->pvp_reward();
    return 1;
}

__attribute__((weak)) TimerMapLoading::TimerMapLoading() throw() {}
__attribute__((weak)) TimerMapLoading::~TimerMapLoading() throw() {}
#include "TimerDispatcher/dispatch_sig_5.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xb, p2))
    {
        return 0;
    }
    party->map_load_forced_finish();
    return 1;
}

__attribute__((weak)) TimerSetResult::TimerSetResult() throw() {}
__attribute__((weak)) TimerSetResult::~TimerSetResult() throw() {}
#include "TimerDispatcher/dispatch_sig_6.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xc, p2))
    {
        return 0;
    }
    party->forced_set_play_result();
    return 1;
}

__attribute__((weak)) TimerDungeonClear::TimerDungeonClear() throw() {}
__attribute__((weak)) TimerDungeonClear::~TimerDungeonClear() throw() {}
#include "TimerDispatcher/dispatch_sig_7.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xd, p2))
    {
        return 0;
    }
    party->dungeon_clear();
    return 1;
}

__attribute__((weak)) TimerDungeonFail::TimerDungeonFail() throw() {}
__attribute__((weak)) TimerDungeonFail::~TimerDungeonFail() throw() {}
#include "TimerDispatcher/dispatch_sig_8.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xe, p2))
    {
        return 0;
    }
    party->party_dungeon_fail();
    return 1;
}

__attribute__((weak)) TimerPartyWalkOutLackUser::TimerPartyWalkOutLackUser() throw() {}
__attribute__((weak)) TimerPartyWalkOutLackUser::~TimerPartyWalkOutLackUser() throw() {}
#include "TimerDispatcher/dispatch_sig_9.inc"
{
    // ORIG 0x08633e7a：GetParty(p1) 按定时器 party index 取队伍（非无参重载）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0xf, p2))
    {
        return 0;
    }
    if (!party->OnTimerWalkOutLackUser())
    {
        return 0;
    }
    return 1;
}

__attribute__((weak)) TimerPVPWalkOutLackUser::TimerPVPWalkOutLackUser() throw() {}
__attribute__((weak)) TimerPVPWalkOutLackUser::~TimerPVPWalkOutLackUser() throw() {}
#include "TimerDispatcher/dispatch_sig_10.inc"
{
    // ORIG 0x08633e86：GetPvp(p1, 0, 0x31)；timer_key 与 p2 比较（非 p3）。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, 0, 0x31);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x31) != p2)
    {
        return 0;
    }
    if (!room->OnTimerWalkOutLackUser())
    {
        room->reset();
        G_CGameManager()->PutPvp(room);
    }
    return 1;
}

__attribute__((weak)) TimerFatigueReset::TimerFatigueReset() throw() {}
__attribute__((weak)) TimerFatigueReset::~TimerFatigueReset() throw() {}
// ---- TimerFatigueReset ----
// 报告：docs/class_func_reports/TimerFatigueReset/{dispatch_sig,RegistNextTimer,updateAbnormalClientCount}.md
// 头已有声明：dispatch_sig / ~ / static int RegistNextTimer()（静态与非静态 mangled 相同 _ZN17..15RegistNextTimerEv，无需改头）。
// 类对象仅 vptr。
// ORIG T 0x086339fa（0xa 字节）：mov $0x1,%eax 直返 1。
#include "TimerDispatcher/updateAbnormalClientCount.inc"
{
    return 1;
}

// ORIG T 0x08633750 (_ZN17TimerFatigueReset12dispatch_sigEiij)
// [b28d-30] 复核 DIFF：callee-saved 寄存器缓存/帧差（N7/N82/P231 族），ORIG 0x08633750。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：callee-saved 缓存组 + 帧差（167/169）。
#include "TimerDispatcher/dispatch_sig_11.inc"
{
    // ORIG：G_CEnvironment()+0x37c（每日重置小时）即 m_serverEnvir.m_fatigueTime；
    // ORIG 每处均重新取 env。
    // ORIG 0x0863375d `mov 0x37c(%eax),%ebx` 单段直落 callee-saved 跨 update 调用，
    // 无栈槽 ⇒ P0 表达式内联（旧具名局部多 -0x10 槽并使 guard 槽 -0x14→-0x18 全帧平移）。
    static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
        ->update_time_values(G_CEnvironment()->m_serverEnvir.m_fatigueTime);

    // ORIG 每个 GameWorld 成员调用前都重新取 G_GameWorld()，逐条保留。
    G_GameWorld()->ResetAllFatigue();
    G_GameWorld()->ResetAllCoins();
    G_GameWorld()->DailyEventModify();
    G_GameWorld()->ResetAllEventData();
    G_GameWorld()->ReselectDailyQuest();
    G_GameWorld()->DeleteDailyItem();
    G_GameWorld()->ResetAllCleadpadPoint();
    G_GameWorld()->ResetAllPassPadFailCount();
    G_GameWorld()->AddDailyItem();
    G_GameWorld()->resetDailyData();
    G_GameWorld()->ResetEntireReliablePerson();   // ORIG 0x8633806：resetDailyData 之后立即调用
    GetInstanceDungeonEntranceLog()->InsertDungeonEntrance();
    GetInstanceCommonStatisticsMgr()->InsertDatabase();

    // ORIG：三个统计域上报（CLevelDungeonPlayStatistic@+0x0、CDungeonExpStatistic@+0x18、
    // CExpertJobStatistic 单例），与上面两个 InsertDatabase 同批落库。
    ((CLevelDungeonPlayStatistic*)GlobalData::s_statisticMgr)->InsertDatabase();
    ((CDungeonExpStatistic*)((char*)GlobalData::s_statisticMgr + 0x18))->InsertDatabase();
    GetInstanceExpertJobStatistic()->UpdateExpertJob();

    advancealtar::HistoryLog::updateStatistics();   // 已声明，定义未链接 → UNLINKED_CALLEE

    // ORIG 0x0863386c-0x086338fd：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)
    // （技巧36）；guard1 槽 -0x14、guard2 槽 -0x1c 紧邻、dtor+catch 各随块尾
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x512), true);
        // ORIG 0x08633888-0x086338b9：每写经显式 operator*() 调用（跨版本代理链
        // 形，同 CUserOnlyOrig LoadMileage 模板；guard<<x 直调会丢 op* 段）
        guard.operator*()->operator<<((int)0x4e);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q /*2*/, guard);
    }

    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x51c), true);
        guard.operator*()->operator<<((int)0x50);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q /*4*/, guard);
    }

    DB_LoadAutoMarketConditionsControl::makeRequest(false);   // 定义 W9_EventSmall.cpp:30
    DB_CreateCharac::makeRequestResetLimitCreateMid();        // 定义 DB_CreateCharac.cpp
    G_GameWorld()->ResetOneADayItemShop();

    // ORIG 0x086339d9-0x086339e7：bool 返回 byte 槽 -0x9 物化 + `movzbl; xor $0x1;
    // test; je TRUE岛(mov$1)`——false fall-through（技巧1/22 同族）
    bool ok = RegistNextTimer();
    if (ok != 1)
        return 0;
    return 1;
}

// ORIG T 0x08633a04 (_ZN17TimerFatigueReset15RegistNextTimerEv)
// ORIG 非静态成员但不使用 this（调用点 0x086339d4 传 this）；返回 bool。
// [b28d-30] 复核 DIFF：push edi/esi/ebx 缓存组跨版本差（N7/N82 族），ORIG 0x086339d4。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：ORIG -0x4c(%ebp)→%ebx 缓存 vs 4.4 槽化
// + cMyTrace 槽 -0x38↔-0x48 + 帧 0x8c↔leave（95/94）。
#include "TimerDispatcher/RegistNextTimer.inc"
{
    // ORIG 0x08633a17-0x08633a1c：`call getCurSec; mov %eax,-0x4c` 单段直存，
    // curSec 槽与 tmv(-0x78..-0x49) 高端相邻——试先声明取得更外槽位
    time_t curSec = GlobalData::s_systemTime_.getCurSec();   // -0x4c
    tm timeInfo;                                             // local_7c -0x78

    tm* pt = localtime_r(&curSec, &timeInfo);                // ORIG sret 形 pt 槽 -0x28

    const int hour = pt->tm_hour;                            // -0x24（08633a33 直读）
    // ORIG 0x08633a40/0x45/0x47：三字段各 `mov -0x28,%eax` 重读 + 单段直落 callee-saved
    // （无栈槽）⇒ P231/P307 纯 register（非 asm 绑定）。
    register int sec = pt->tm_sec;
    register int min = pt->tm_min;
    register int hr = pt->tm_hour;

    // ORIG 0x08633a71-0x08633a79：参数装载 edi→0x10/min→0xc/hr→0x8（右→左）
    cMyTrace(__PRETTY_FUNCTION__, 0x572, 0)(
        "Reset all user's fatigue!(%02d:%02d:%02d)", hr, min, sec);

    // env+0x37c 写回 tm_hour；min/sec 清零（ORIG 0x8(%eax)/0x4(%eax)/(%eax) 槽序）。
    pt->tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;   // env+0x37c
    pt->tm_min = 0;
    pt->tm_sec = 0;

    int nextMidnight = static_cast<int>(mktime(pt));         // -0x20
    if (G_CEnvironment()->m_serverEnvir.m_fatigueTime <= hour)
        nextMidnight += 0x15180;                             // ORIG addl 原地 -0x20

    const int delta = nextMidnight - static_cast<int>(curSec);  // -0x1c

    cMyTrace(__PRETTY_FUNCTION__, 0x587, 0)(
        "DELTA %d , CHECK_T %d , CUR_T %d\n", delta, nextMidnight, static_cast<int>(curSec));

    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2,
                                0,
                                (TIMER_MESSAGE)0x50,
                                delta,
                                0,
                                0);
    return 1;
}

// ORIG T 0x08632b32 (_ZN17TimerAvatarExpire11RegistTimerEP5CUser)
// 求值序（右→左）：getCurCharacNo → get_unique_id → env+0x380*0x3c → GetSlotIDX →
// G_TimerQueue；InsertTimer(0, slot, 0x51, env380*60, unique_id, characNo)；返回 1（void 化）。
// [b28c-27] N82附：ORIG callee-saved 分配 ebx=getCurCharacNo / edi=get_unique_id /
// esi=GetSlotIDX，4.4 循环移位为 edi/esi/ebx（多实参装载序跨版本对调，声明序不可控）。
// [b28d-30] 复核 NEAR：ORIG callee-saved 分配 ebx/edi/esi vs 4.4 循环移位（N7/N82附 族），
// ORIG 0x08632b32；声明序不可控，封顶。
// [b28d-solo-TimerDispatcher] 已 AE：Main 代落 getCurCharacNo→unsigned int 后
// callee-saved 分配（ebx/edi/esi）与 ORIG 一致（43/43）。
#include "TimerDispatcher/RegistTimer.inc"
{
	// env+0x380（每日过期时长分钟数）即 m_serverEnvir.m_avatarTime；
    // 内联实参保持 ORIG 求值序（getCurCharacNo → get_unique_id → env → GetSlotIDX → G_TimerQueue）。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0,
        // ORIG movswl %ax：GetSlotIDX 返回值按 signed short 符号扩展（0x8632b32）。
        static_cast<int>(static_cast<short>(user->GetSlotIDX())),
        (TIMER_MESSAGE)0x51,
        G_CEnvironment()->m_serverEnvir.m_avatarTime * 0x3c,
        static_cast<unsigned short>(user->get_unique_id()),   // ORIG movzwl %ax
        static_cast<unsigned int>(user->getCurCharacNo()));
    return true;
}

// TimerAvatarExpire ctor W 0x0863bb3e / ~D1 W 0x0863bb8a / ~D0 W 0x0863bb5a
// （ORIG 仅 vptr：TimerSig 基类 vptr 后覆写本类 vtable，与其它 Timer_* 同构）。
__attribute__((weak)) TimerAvatarExpire::TimerAvatarExpire() throw() {}
__attribute__((weak)) TimerAvatarExpire::~TimerAvatarExpire() throw() {}
// ORIG T 0x08632a78 (_ZN17TimerAvatarExpire12dispatch_sigEiij)：恒返回 1（mov $1,%eax; ret）。
#include "TimerDispatcher/dispatch_sig_12.inc"
{
    return 1;
}

// ORIG T 0x08632a82 (_ZN17TimerAvatarExpire18CheckExpiredAvatarEP5CUser)
// PacketGuard 局部 + get_state==0 → 1；getCurCharacR==0 → 0；
// 有到期头像时经 getCurCharacInvenW 发包；恒 1。
#include "TimerDispatcher/CheckExpiredAvatar.inc"
{
    PacketGuard guard;
    if (user->get_state() == 0)
    {
        return 1;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->getCurCharacInvenR()->isAnyExpiredAvatarItem())
    {
        user->getCurCharacInvenW()->ExpiredAvatarItem(&guard);
    }
    return 1;
}

__attribute__((weak)) TimerStatisticsLog::TimerStatisticsLog() throw() {}
__attribute__((weak)) TimerStatisticsLog::~TimerStatisticsLog() throw() {}
// [b28d-30] 复核 DIFF：帧/callee-saved 跨版本差（N7/N82 族），ORIG 0x08633f3d 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82 封顶：DIFF→NEAR（107/107）。
// 修法：①P231/P307 后三 getter 结果加纯 register（非 asm 绑定）⇒ 复现 ORIG
// %edi/%esi/%ebx 跨 cMyTrace ctor 保活（删槽）；②P212 前三 getter 局部提函数头。
// 残差 = 前三槽 -0x44/-0x40/-0x3c vs -0x28..（4.1 声明序深槽 vs 4.4 首次使用序）
// + cMyTrace varargs 求值序 ⇒ N80/N82。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；残差 = guard/trace 槽
// (-0x34/-0x2c) 与三 getter 槽 (-0x44..-0x3c) 的区块排布互换（类对象 vs 标量混合区，
// N82/N93）⇒ 无语义缺口。
#include "TimerDispatcher/dispatch_sig_13.inc"
{
    // P212 全头部声明：ORIG 三 getter 结果落深槽 -0x44/-0x40/-0x3c、trace -0x2c、
    // guard -0x34；4.4 块内声明按首次使用序落浅槽 ⇒ 提至函数头复现块级槽序。
    // [A12-2026-09-10 实验·失败] 三 getter 结果改 POD 聚合（一个对象）试探
    // 4.4「对象优先占深槽」分区规律：实测槽位不变（仍 -0x28/-0x24/-0x20，
    // ORIG -0x44/-0x40/-0x3c）⇒ POD 聚合不改分区，已回滚。
    int cacheLoadFailOverflowCount;
    int cacheLoadFailCount;
    int cacheLoadOverflowCount;
    // ORIG 0x08633f3d-0x08633f54：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)
    // （技巧36 删 s 中转槽）
    // ORIG 0x08633f25-0x08633fce：guard 生命周期止于 put 后（dtor+catch 块夹在
    // getter 段之前，ORIG 0x08633fad catch）⇒ 显式 {} 作用域块（技巧12）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x62f), true);
        // ORIG 0x08633f59-0x08633f6f：显式 operator*() 代理链（同 FatigueReset 模板）
        guard.operator*()->operator<<((int)0xcd);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    // ORIG 0x08633fe0/0x08633ff0/0x086334000：前三 getter 结果槽 -0x44/-0x40/-0x3c
    //（4.1 声明序布局；4.4 按生命周期落 -0x28..近端，槽序不可翻=N 族残差）
    cacheLoadFailOverflowCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadFailOverflowCount();
    cacheLoadFailCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadFailCount();
    cacheLoadOverflowCount = exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadOverflowCount();
    // ORIG 0x08634010/0x0863401f/0x0863402e：后三 getter 结果直落
    // %edi/%esi/%ebx 跨 ctor 存活；绑定版残差=装载时多一条重排 mov（probe 实测
    // N96/P0：绑定形已废（三 callee 结果寄存器缓存残差记 N76/N82附）。
    // P231/P307：ORIG 后三 getter 结果直落 %edi/%esi/%ebx 跨 cMyTrace ctor 保活
    // （无栈槽）⇒ 纯 register 存储类提示（非 asm 绑定，P0 合规）。
    register int cacheLoadCount =
        exchange_server::GetInstanceExchangeServerHitRate()->GetCacheLoadCount();
    register int dbLoadOverflowCount =
        exchange_server::GetInstanceExchangeServerHitRate()->GetDBLoadOverflowCount();
    register int dbLoadCount =
        exchange_server::GetInstanceExchangeServerHitRate()->GetDBLoadCount();

    cMyTrace trace(__PRETTY_FUNCTION__, 0x637, 0);
    trace("EXCHANGE_SERVER_HIT_RATE : db count(%u), overflow db count(%u), cache count(%u), overflow cache count(%u), cache fail count(%u), overflow cache fail count(%u)",
          dbLoadCount, dbLoadOverflowCount, cacheLoadCount, cacheLoadOverflowCount,
          cacheLoadFailCount, cacheLoadFailOverflowCount);

    // ORIG 0x08634092-0x0863409b：`mov %al,-0x19; movzbl -0x19,%eax; xor $0x1`——
    // 具名 bool 局部 byte 槽 + 重读后 ==1 判定（技巧22 邻域）
    bool ok = TimerStatisticsLog::RegistNextTimer();
    // ORIG 0x08634099 `xor $0x1; test; je TRUE岛` ——false 为 fall-through 体
    if (ok != 1)
        return 0;
    return 1;
}

__attribute__((weak)) TimerCardSelect::TimerCardSelect() throw() {}
__attribute__((weak)) TimerCardSelect::~TimerCardSelect() throw() {}
#include "TimerDispatcher/dispatch_sig_14.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x10, p2))
    {
        return 0;
    }
    // ORIG 0x08634412..+0x84：|| 合并条件 —— gcc4.4 对 || 生成短路跳转 +
    // 汇合点 mov$1/jmp/mov$0 物化（探针 x4 p1 实证）；独立两 if 各自 sete，不合 ORIG。
    if (party->get_dungeon_clear_state() == 0 ||
        party->get_dungeon_clear_state() == 5)
    {
        return 1;
    }
    if (!party->IsCompleteCardSelectOfMembers(eClearRewardCardType_t_0))
    {
        party->CompleteCardSelect();
        party->SendEachCardInfo();
    }
    party->set_dungeon_clear_state(5);
    party->dungeon_clear();
    return 1;
}

__attribute__((weak)) TimerClearDungeonReward::TimerClearDungeonReward() throw() {}
__attribute__((weak)) TimerClearDungeonReward::~TimerClearDungeonReward() throw() {}
#include "TimerDispatcher/dispatch_sig_15.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x11, p2))
    {
        return 0;
    }
    if (party->get_dungeon_clear_state() == 2)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x48);
        packet.put_byte(1);
        packet.finalize(true);
        party->send_to_party(packet);
        party->set_dungeon_clear_state(3);
        party->insert_time_dungeon_score_scroll_msg(0x1e);
    }
    return 1;
}

__attribute__((weak)) TimerClearDungeonScoreScoll::TimerClearDungeonScoreScoll() throw() {}
__attribute__((weak)) TimerClearDungeonScoreScoll::~TimerClearDungeonScoreScoll() throw() {}
#include "TimerDispatcher/dispatch_sig_16.inc"
{
    // [b23S3C] P48 去缓存：get_user 每调用点重取×3；fatigue/max 结果寄存器直比；
    // 判定链拆早返岛（fresh .o 134/134 零差）。
    // ORIG 0x086345f2：GetParty(p1) + check_timer_key(0x12, p2)（用 p2 非 p3）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x12, p2))
        return 0;
    if (party->get_dungeon_clear_state() == 0x03)
    {
        PacketGuard packet;
        packet.put_header(1, 0x49);
        packet.put_byte(1);
        for (int i = 0; i <= 3; ++i)
        {
            // ORIG 0x08634696：get_user ×3（每次调用点重取，P183 去缓存）。
            if (party->get_user(i) == 0)
                packet.put_short(-1);
            else
            {
                if (party->get_user(i)->getCurCharacTotalFatigue() ==
                    party->get_user(i)->getCurCharacTotalMaxFatigue())
                    packet.put_short(0);
                else
                    packet.put_short(1);
            }
        }
        packet.finalize(true);
        party->send_to_party(packet);
        party->set_dungeon_clear_state(0x04);
        party->insert_time_dungeon_score_scroll_msg(0x1e);
        party->insert_time_card_select_msg(0x04);
    }
    return 1;
}

__attribute__((weak)) TimerClearDungeonCardSelectRight::TimerClearDungeonCardSelectRight() throw() {}
__attribute__((weak)) TimerClearDungeonCardSelectRight::~TimerClearDungeonCardSelectRight() throw() {}
#include "TimerDispatcher/dispatch_sig_17.inc"
{
    // ORIG 0x086347e2：GetParty(p1) + check_timer_key(0x13, p2)。
    CParty* party = G_CGameManager()->GetParty(p1);
    // 拆分独立早退（ORIG 0x086347e3：null 段 cmpl;jne 7f0+mov0 岛；
    // check 段 xor$1;test;je 818 前跳 + 811 mov0 岛）。
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x13, p2))
        return 0;
    if (party->get_dungeon_clear_state() == 4)
    {
        party->set_dungeon_clear_state(5);
        party->insert_time_dungeon_clear_msg(5);
    }
    return 1;
}

__attribute__((weak)) TimerPremiumServiceOver::TimerPremiumServiceOver() throw() {}
__attribute__((weak)) TimerPremiumServiceOver::~TimerPremiumServiceOver() throw() {}
#include "TimerDispatcher/dispatch_sig_18.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 5);   // ORIG 第二实参为常量 5
    // ORIG 0x0863485a：三段独立早退（jne 岛 + setne 物化 + xor$1），非 || 链。
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->IsPremiumUser())
        return 0;
    WongWork::CHandlePremium::handlePremiumTimeout(user);   // ORIG 0x863485a
    return 1;
}

__attribute__((weak)) TimerPremiumServiceUpdate::TimerPremiumServiceUpdate() throw() {}
__attribute__((weak)) TimerPremiumServiceUpdate::~TimerPremiumServiceUpdate() throw() {}
#include "TimerDispatcher/dispatch_sig_19.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
    // ORIG 0x086348d4：三段独立早退。
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->IsPremiumUser())
        return 0;
    user->GetPremiumInfoW()->ReCalcAdvantage();
    user->GetPremiumInfoW()->RecalcAdditionalInfo(user);
    if (user->get_state() > 3)
    {
        user->SendFatigue();
    }
    const time_t now = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm midnight;
    localtime_r(&now, &midnight);   // ORIG 0x086349ae：localtime_r（线程安全）
    midnight.tm_hour = 0;
    midnight.tm_min = 0;
    midnight.tm_sec = 0;
    int nextAt = static_cast<int>(mktime(&midnight));   // ORIG mov %eax,-0xc(%ebp)
    if (midnight.tm_hour >= 0)                          // ORIG mov -0x38(%ebp) test js
        nextAt += 0x15180;
    register int delay = nextAt - static_cast<int>(now);   // ORIG %ebx 直落 0x10(%esp)
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, p1, (TIMER_MESSAGE)5,
                                delay, p2, p3);
    return 1;
}

__attribute__((weak)) TimerPremiumServiceStart::TimerPremiumServiceStart() throw() {}
__attribute__((weak)) TimerPremiumServiceStart::~TimerPremiumServiceStart() throw() {}
#include "TimerDispatcher/dispatch_sig_20.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->GetPremiumInfoW()->startPremium(p2, false))
    {
        return 0;
    }
    if (user->get_state() > 2)
    {
        // ORIG 通过 CUser::GetPremiumInfo()（const）取 CUserPremium*，再查指定类型信息。
        WongWork::SUserPremiumInfo* info =
            user->GetPremiumInfo()->GetPremiumInfo(p2);
        // ORIG 0x08634af4/0x08634b3b：两处正向早退 return 1（mov $1 岛内联）。
        if (info->m_type == 0)
            return 1;
        WongWork::CHandlePremium::handleNotifyPremiumInfo(
            user, info->m_type, info->m_end - info->m_start);
        if (g_SPremiumInfo[info->m_type].m_kind == 2)
            return 1;
        WongWork::CHandlePremium::handlePremiumEffect(
            user, static_cast<ENUM_PREMIUM_TYPE>(info->m_type));
    }
    return 1;
}

__attribute__((weak)) TimerCheckPaidPCRoomTerm::TimerCheckPaidPCRoomTerm() throw() {}
__attribute__((weak)) TimerCheckPaidPCRoomTerm::~TimerCheckPaidPCRoomTerm() throw() {}
#include "TimerDispatcher/dispatch_sig_21.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 6);   // ORIG 第二实参为常量 6
    // ORIG 报告 TimerCheckPaidPCRoomTerm/dispatch_sig.md：两段独立早退；
    // !isAffectedPremium()（bool 声明 ⇒ xor $0x1）先走 notify/return 0，
    // logout/login/return 1 位于尾部 je 目标块。
    if (user == 0)
        return 0;
    if (static_cast<unsigned int>(user->get_acc_id()) != p3)
        return 0;
    if (!user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(p2)))
    {
        WongWork::CHandlePremium::handleNotifyPremiumInfo(user, p2, 0);
        return 0;
    }
    user->SendLogoutToPCRoomServer();
    user->SendLoginToPCRoomServer();
    return 1;
}

__attribute__((weak)) TimerEPLPReturnVillage::TimerEPLPReturnVillage() throw() {}
__attribute__((weak)) TimerEPLPReturnVillage::~TimerEPLPReturnVillage() throw() {}
#include "TimerDispatcher/dispatch_sig_22.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x14, p2))
    {
        return 0;
    }
    party->ReturnToVillage();
    return 1;
}

__attribute__((weak)) TimerEPLPCommandWait::TimerEPLPCommandWait() throw() {}
__attribute__((weak)) TimerEPLPCommandWait::~TimerEPLPCommandWait() throw() {}
#include "TimerDispatcher/dispatch_sig_23.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x15, p2))
    {
        return 0;
    }
    if (party->GetSelectedEPLPCmd() == 3)
    {
        return 0;
    }
    if (party->GetEPLPState() == 2)
    {
        party->ReturnToVillage();
    }
    return 1;
}

__attribute__((weak)) TimerWarRoomWaitCount::TimerWarRoomWaitCount() throw() {}
__attribute__((weak)) TimerWarRoomWaitCount::~TimerWarRoomWaitCount() throw() {}
#include "TimerDispatcher/dispatch_sig_24.inc"
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x41, p2))
    {
        return 0;
    }
    room->ChangeState(WARROOM_STATE_2);
    return 1;
}

__attribute__((weak)) TimerWarRoomReadyLockCount::TimerWarRoomReadyLockCount() throw() {}
__attribute__((weak)) TimerWarRoomReadyLockCount::~TimerWarRoomReadyLockCount() throw() {}
#include "TimerDispatcher/dispatch_sig_25.inc"
{
    // ORIG 0x08634d86：GetWarRoom(p1) + CheckTimerKey(0x42, p2)。
    // 拆分早退（ORIG 0x08634da5：cmpl;jne 前跳+mov0 岛；0x08634dc7 xor$1;test;je
    // 前跳 STATE + mov0 岛；ChangeState 后 mov$1 直落 EXIT）。
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
        return 0;
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x42, p2))
        return 0;
    room->ChangeState(WARROOM_STATE_3);
    return 1;
}

__attribute__((weak)) TimerWarRoomBattlePauseCount::TimerWarRoomBattlePauseCount() throw() {}
__attribute__((weak)) TimerWarRoomBattlePauseCount::~TimerWarRoomBattlePauseCount() throw() {}
#include "TimerDispatcher/dispatch_sig_26.inc"
{
    // ORIG 0x08634df6：GetWarRoom(p1) + CheckTimerKey(0x43, p2)。
    // 拆分早退（同 ReadyLockCount，ORIG 0x08634e15/0x08634e37）。
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
        return 0;
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x43, p2))
        return 0;
    room->ChangeState(WARROOM_STATE_3);
    return 1;
}

__attribute__((weak)) TimerWarRoomResultCount::TimerWarRoomResultCount() throw() {}
__attribute__((weak)) TimerWarRoomResultCount::~TimerWarRoomResultCount() throw() {}
#include "TimerDispatcher/dispatch_sig_27.inc"
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x44, p2))
    {
        return 0;
    }
    room->HandleTimerResultCount();
    return 1;
}

__attribute__((weak)) TImerWarRoomTimeBomb::TImerWarRoomTimeBomb() throw() {}
__attribute__((weak)) TImerWarRoomTimeBomb::~TImerWarRoomTimeBomb() throw() {}
#include "TimerDispatcher/dispatch_sig_28.inc"
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x45, p2))
    {
        return 0;
    }
    room->HandleTimerTimeBomb();
    return 1;
}

__attribute__((weak)) TimerWarRoomCheckFail::TimerWarRoomCheckFail() throw() {}
__attribute__((weak)) TimerWarRoomCheckFail::~TimerWarRoomCheckFail() throw() {}
#include "TimerDispatcher/dispatch_sig_29.inc"
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x46, p2))
    {
        return 0;
    }
    room->WarRoomFail();
    return 1;
}

__attribute__((weak)) TimerWarRoomChangeHost::TimerWarRoomChangeHost() throw() {}
__attribute__((weak)) TimerWarRoomChangeHost::~TimerWarRoomChangeHost() throw() {}
#include "TimerDispatcher/dispatch_sig_30.inc"
{
    WarRoom* room = G_CGameManager()->GetWarRoom(p1);
    if (room == 0)
    {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x47, p2))
    {
        return 0;
    }
    room->CheckHostChange();
    return 1;
}

__attribute__((weak)) TimerBloodSmallRoundRegister::TimerBloodSmallRoundRegister() throw() {}
__attribute__((weak)) TimerBloodSmallRoundRegister::~TimerBloodSmallRoundRegister() throw() {}
#include "TimerDispatcher/dispatch_sig_31.inc"
{
    // ORIG 0x08634fee：GetParty(p1) + check_timer_key(0x16, p2)。
    CParty* party = G_CGameManager()->GetParty(p1);
    // 拆分独立早退（ORIG 0x08634ffx：null 段 jne 前跳岛 + check 段 xor$1;test;je 岛）。
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x16, p2))
        return 0;
    party->m_battleField.BloodSpawnTimerManager(
        false, true);   // ORIG：party+0xb24 即 CBattle_Field 子对象
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister0::TimerBloodPhaseRegister0() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister0::~TimerBloodPhaseRegister0() throw() {}
#include "TimerDispatcher/dispatch_sig_32.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x17, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[0];
    short* spawnTime =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime;
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x17, 0);
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister1::TimerBloodPhaseRegister1() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister1::~TimerBloodPhaseRegister1() throw() {}
#include "TimerDispatcher/dispatch_sig_33.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x18, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[1];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[1];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x18, 1);
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister2::TimerBloodPhaseRegister2() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister2::~TimerBloodPhaseRegister2() throw() {}
#include "TimerDispatcher/dispatch_sig_34.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x19, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[2];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[2];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x19, 2);
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister3::TimerBloodPhaseRegister3() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister3::~TimerBloodPhaseRegister3() throw() {}
#include "TimerDispatcher/dispatch_sig_35.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1a, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[3];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[3];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1a, 3);
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister4::TimerBloodPhaseRegister4() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister4::~TimerBloodPhaseRegister4() throw() {}
#include "TimerDispatcher/dispatch_sig_36.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1b, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[4];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[4];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1b, 4);
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister5::TimerBloodPhaseRegister5() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister5::~TimerBloodPhaseRegister5() throw() {}
#include "TimerDispatcher/dispatch_sig_37.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1c, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[5];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[5];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1c, 5);
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister6::TimerBloodPhaseRegister6() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister6::~TimerBloodPhaseRegister6() throw() {}
#include "TimerDispatcher/dispatch_sig_38.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1d, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[6];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[6];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1d, 6);
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister7::TimerBloodPhaseRegister7() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister7::~TimerBloodPhaseRegister7() throw() {}
#include "TimerDispatcher/dispatch_sig_39.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1e, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[7];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[7];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1e, 7);
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister8::TimerBloodPhaseRegister8() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister8::~TimerBloodPhaseRegister8() throw() {}
#include "TimerDispatcher/dispatch_sig_40.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x1f, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[8];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[8];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x1f, 8);
    return 1;
}

__attribute__((weak)) TimerBloodPhaseRegister9::TimerBloodPhaseRegister9() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister9::~TimerBloodPhaseRegister9() throw() {}
#include "TimerDispatcher/dispatch_sig_41.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x20, p2))
        return 0;
    short nextRound =
        reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnCnt[9];
    short* spawnTime =
        &reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.m_bloodSpawnTime[9];
    reinterpret_cast<CPartyBattleFieldView*>(party)->m_battleField.BloodPhaseSpawn(
        nextRound, *spawnTime, 0x20, 9);
    return 1;
}

__attribute__((weak)) TimerBloodCrazyMonster::TimerBloodCrazyMonster() throw() {}
__attribute__((weak)) TimerBloodCrazyMonster::~TimerBloodCrazyMonster() throw() {}
#include "TimerDispatcher/dispatch_sig_42.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x21, p2))
    {
        return 0;
    }
    party->m_battleField.BloodCrazyMonsterSpawnPacket(
        );   // ORIG：party+0xb24 子对象
    return 1;
}

__attribute__((weak)) TimerBloodDungeonClearReward::TimerBloodDungeonClearReward() throw() {}
__attribute__((weak)) TimerBloodDungeonClearReward::~TimerBloodDungeonClearReward() throw() {}
#include "TimerDispatcher/dispatch_sig_43.inc"
{
    // ORIG 0x08638cd0：GetParty(p1)；check_timer_key 以 p3 为 msg、p2 为 param。
    CParty* party = G_CGameManager()->GetParty(p1);
    // 拆分三段独立早退（ORIG 0x08638cd5 jne 岛 / 0x08638cf7 xor$1;test;je /
    // 0x08638d17 同形，je→8d25 mov$1 尾岛）。
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2))
        return 0;
    if (!party->onBloodRewardTimer((TIMER_MESSAGE)p3))
        return 0;
    return 1;
}

__attribute__((weak)) TimerUltimateNextRoundStart::TimerUltimateNextRoundStart() throw() {}
__attribute__((weak)) TimerUltimateNextRoundStart::~TimerUltimateNextRoundStart() throw() {}
// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x08635xxx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N4/N7 封顶：94/97（原 94/100）。
// 修法：①P183 删 `CBattle_Field* field = &party->m_battleField` 缓存（ORIG 每站点
// 重发 `mov -0x10(%ebp),%eax; add $0xb24,%eax`）；②get_blood_dungeon_type() 外联
// call 改直读 m_bloodDungeonType（ORIG `movzbl 0x89f(%eax),%eax`）。
// 残差 = sendUltimateRandomDifficulty 的 `add $0xb24` vs 4.4 `lea 0xb24` + 实参
// 装载序（P30/N4）+ 条件物化 ⇒ N4/N7。
#include "TimerDispatcher/dispatch_sig_44.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2))
    {
        return 0;
    }
    // 真语义 bug 修正（Ghidra C：body 条件 = m_pDungeon==0 || type==2）⇒ 早退条件
    // 必须写成 De Morgan 正形 `m_pDungeon != 0 && type != 2`（旧码 `==0 || !=2` 极性错）。
    if (party->m_battleField.m_pDungeon != 0 &&
        party->m_battleField.m_pDungeon->m_bloodDungeonType != 2)
    {
        return 0;
    }
    // P183 地址重物化铁律：ORIG 每个站点重发 `mov -0x10(%ebp),%eax; add $0xb24,%eax`
    // （08638dfb/08638e2f/08638e5c 等），禁缓存 `CBattle_Field* field = &party->m_battleField`。
    if (party->m_battleField.getUltimateDifficylty() != 0)
    {
        return 0;
    }
    const int diff = party->m_battleField.getNextRoundRandomDifficulty();
    party->m_battleField.setUltimateDifficulty(static_cast<char>(diff));
    // P48 no-CSE：ORIG 两次 fresh getBloodRoundOrder()（比较用 + send 实参用）
    if (static_cast<short>(party->m_battleField.m_bloodRound.getBloodRoundOrder()) !=
        party->m_battleField.getBloodMaxRound() - 1)
    {
        party->m_battleField.sendUltimateRandomDifficulty(
            diff, static_cast<short>(party->m_battleField.m_bloodRound.getBloodRoundOrder()));
    }
    party->m_battleField.startUltimateRound();
    return 1;
}
__attribute__((weak)) TimerEventChristmas::TimerEventChristmas() throw() {}
__attribute__((weak)) TimerEventChristmas::~TimerEventChristmas() throw() {}
#include "TimerDispatcher/dispatch_sig_45.inc"
{
    // ORIG 三方法为静态语义（调用点无 this 压栈，直接操作全局 0x943c138），
    // 静态成员直调（ORIG 8635710/863571b/8635726 无 temp 对象槽，帧 0x18）。
    CServerEvent::SubExpRate(p2);
    CServerEvent::TurnOffEvent(p3);
    // ORIG：结果不落实名槽，直接 movswl %ax,%edx 寄存器扩展后入参。
    Inter_MonitorServerEventStart::SendEventNotiPacket(false,
                                                        static_cast<short>(p3),
                                                        static_cast<short>(CServerEvent::GetEventInterval(p3)), 0);
    return 1;
}

__attribute__((weak)) TimerStatisticsDetailChannelInfo::TimerStatisticsDetailChannelInfo() throw() {}
__attribute__((weak)) TimerStatisticsDetailChannelInfo::~TimerStatisticsDetailChannelInfo() throw() {}
// ORIG T 0x0863575c (_ZN32TimerStatisticsDetailChannelInfo12dispatch_sigEiij)：
// G_GameWorld()->updateDetailChannelServiceInfo()；RegistNextTimer()；返回 1。
#include "TimerDispatcher/dispatch_sig_46.inc"
{
    G_GameWorld()->updateDetailChannelServiceInfo();
    RegistNextTimer();
    return 1;
}
// ORIG T 0x08635782 (_ZN32TimerStatisticsDetailChannelInfo15RegistNextTimerEv)
// cur=getCurSec()；localtime_r → next=(tm_min%0x1e)*(-0x3c)+(0x708-tm_sec)（对齐半小时）；
// cMyTrace("content min:%d sec:%d next sec:%d  (channel no:%d)")；
// InsertTimer(2, 0, 0x54, next, 0, 0)；返回 1。
// SIGFIX：ORIG __PRETTY_FUNCTION__ 串为 "bool TimerStatisticsDetailChannelInfo::RegistNextTimer()"。
// [b28d-30] 复核 DIFF：push edi/esi 缓存组/帧差（N7/N82 族），ORIG 0x086355xx。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：push edi/esi 缓存组 + 帧差（85/80）。
// [TimerDispatcher-2 2026-09-09 复核] 残差 = `%0x1e` 强度归约模板（ORIG
// `lea (%eax,%eax,1),%edx; mov %edx,%eax; shl $4,%eax; sub %edx,%eax` vs 4.4
// `add %eax,%eax; mov %eax,%edx; shl $4,%edx; sub %eax,%edx`，N86b/N1）⇒ 封顶。
// [TimerDispatcher-3 2026-09-10 判据①] 同族：唯一 jg 目标差 1 = 该 lea↔add 单条
// （探针 r1..r6 六形恒 add），分支语义等价。
#include "TimerDispatcher/RegistNextTimer_1.inc"
{
    // [TimerDispatcher-4 2026-09-11] 同 DDStat/UCE：唯一真残差 = `%0x1e` 余数段
    // ORIG `lea (%eax,%eax,1),%edx; mov %edx,%eax; shl $4,%eax; sub %edx,%eax`（4 条）
    // vs 4.4 `add %eax,%eax; mov %eax,%edx; shl $4,%edx; sub %eax,%edx`（4 条）
    // ⇒ N86b/N1（22 形探针无一命中，见 DDStat 判词）。p167：8=8，MISSING/EXTRA
    // 仅 `cMyTrace C2Ev↔C1Ev` 同址别名（P93）⇒ 无缺口。
    const int curSec = GlobalData::s_systemTime_.getCurSec();
    tm timeInfo;
    // [b28c-27] ORIG 0x08635782：localtime_r 第一实参是 curSec 槽本身的地址
    // （`lea -0x34(%ebp),%eax`），无 time_t now 中间槽；4.4 对具名 now 恒多一槽
    // 与两条 mov。按 ORIG 直取 curSec 地址（int/time_t 同 4 字节）。
    tm* pt = localtime_r(reinterpret_cast<const time_t*>(&curSec), &timeInfo);
    // ORIG 086358xx：`imul $-0x3c; lea 0x6cc(%eax),%ecx; mov $0x3c; sub sec; lea`
    // ⇒ 常量按 ORIG 拆成 0x6cc + (0x3c - sec)（0x6cc=0x708-0x3c，等价）。
    const int nextSec = (pt->tm_min % 0x1e) * -0x3c + 0x6cc + (0x3c - pt->tm_sec);
    // P0：ORIG channelNo 无槽（086358xx `mov %eax,%ebx` 直入 callee-saved）⇒ 实参内联；
    // P221/P227 全表达式临时链令 tm_min/tm_sec/channelNo 在 ctor 前求值驻 edi/esi/ebx。
    cMyTrace(__PRETTY_FUNCTION__, 0x92c, 0)(
        "content min:%d sec:%d next sec:%d  (channel no:%d)\n",
        pt->tm_min, pt->tm_sec, nextSec, G_CEnvironment()->get_channel_no());
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x54, nextSec, 0, 0);
    return true;
}

__attribute__((weak)) TimerStatisticLevel::TimerStatisticLevel() throw() {}
__attribute__((weak)) TimerStatisticLevel::~TimerStatisticLevel() throw() {}
#include "TimerDispatcher/dispatch_sig_47.inc"
{
    G_GameWorld()->UpdateStatistic4ChannelUserLev();
    // b28c-28（P0 清绑 + P52 内联实参）：ORIG 0x086358ac 为
    // `push %ebx; sub $0x24` + `call CalcNextTime; mov %eax,%ebx; …;
    // call getCurSec; sub %eax,%ebx; …; mov %ebx,0x10(%esp)`——
    // delay 无栈槽、经 callee-saved 跨 getCurSec/G_TimerQueue 两 call 保活。
    // 具名局部（含旧 register 绑定）一律落槽 ⇒ 改实参位内联求值。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x55,
        CalcNextTime() - static_cast<int>(GlobalData::s_systemTime_.getCurSec()),
        0, 0);
    return 1;
}

__attribute__((weak)) TimerScheduleOneHour::TimerScheduleOneHour() throw() {}
__attribute__((weak)) TimerScheduleOneHour::~TimerScheduleOneHour() throw() {}
#include "TimerDispatcher/dispatch_sig_48.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x56, 0xe10, 0, 0);
    return 1;
}

__attribute__((weak)) TimerSaveLogGamechannel::TimerSaveLogGamechannel() throw() {}
__attribute__((weak)) TimerSaveLogGamechannel::~TimerSaveLogGamechannel() throw() {}
#include "TimerDispatcher/dispatch_sig_49.inc"
{
    // CLogGameChannel 三方法为无参实例调用（ORIG 0x08550456/0x085506c6/0x085503fe）。
    GlobalData::s_pLogGameChannel->ReqSaveLog();
    GlobalData::s_pLogGameChannel->ResetInOutUser();
    GlobalData::s_pLogGameChannel->SetNextLogTimer();
    return 1;
}

__attribute__((weak)) TimerQueryCounter::TimerQueryCounter() throw() {}
__attribute__((weak)) TimerQueryCounter::~TimerQueryCounter() throw() {}
// ORIG TimerQueryCounter::dispatch_sig @ 0x8635a60 (0x17b)
#include "TimerDispatcher/dispatch_sig_50.inc"
{
    // 第一段：LOGDB_Q(4)，值 0x7b / -1 —— ORIG 0x08635a60 段：Acquire 免槽直落
    // guard ctor、显式 operator*() 代理链、guard1 槽 -0x10 + {} 块界 dtor/catch
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x97b), true);
        guard.operator*()->operator<<((int)0x7b);
        guard.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    // 第二段：DB_Q(2)，值 0x7c / -1（ORIG guard2 槽 -0x18 与 guard1 相邻）
    {
        CStreamGuard guard2(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0x981), true);
        guard2.operator*()->operator<<((int)0x7c);
        guard2.operator*()->operator<<((int)-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard2);
    }

    RegistNextTimer();
    return 1;
}

// ORIG T 0x8635bdc（RegistNextTimer.md 全文转写；声明已在 TimerDispatcher.h，
// 签名 static void；ORIG 恒返回 eax=1，void 化由现有头口径决定）
// ORIG T 0x08635bdc：InsertTimer(2,0,0x59,0x708,0,0)；返回 1。
#include "TimerDispatcher/RegistNextTimer_2.inc"
{
    // ORIG：delay 先物化到局部槽 -0xc(%ebp)（movl $0x708,-0xc），再两步入栈。
    int delay = 0x708;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x59,
                                delay, 0, 0);
    return 1;
}

__attribute__((weak)) TimerNatType::TimerNatType() throw() {}
__attribute__((weak)) TimerNatType::~TimerNatType() throw() {}
// ORIG T 0x08635c2c (_ZN12TimerNatType12dispatch_sigEiij)：
// G_GameWorld()->GetStatisticsNatType()->UpdateNatType()；RegistNextTimer()；返回 1。
#include "TimerDispatcher/dispatch_sig_51.inc"
{
    G_GameWorld()->GetStatisticsNatType()->UpdateNatType();
    RegistNextTimer();
    return 1;
}
// ORIG T 0x08635c5a (_ZN12TimerNatType15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x5a, 0x15180, 0, 0)；返回 1（void 化）。
#include "TimerDispatcher/RegistNextTimer_3.inc"
{
    // ORIG 先把 delay 常量暂存 -0xc(%ebp) 再装入实参槽（gcc4.1 形态），
    // 用局部变量复现该栈槽暂存（同 TimerFatigueReset 先例）。
    int delay = 0x15180;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x5a, delay, 0, 0);
    return 1;
}

__attribute__((weak)) TimerNatTypeUser::TimerNatTypeUser() throw() {}
__attribute__((weak)) TimerNatTypeUser::~TimerNatTypeUser() throw() {}
#include "TimerDispatcher/dispatch_sig_52.inc"
{
    G_GameWorld()->UpdateNatTypeUser();
    RegistNextTimer();
    return 1;
}

// ORIG T 0x08635cd0 (_ZN16TimerNatTypeUser15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x5b, 0xe10, 0, 0)；返回 1。
#include "TimerDispatcher/RegistNextTimer_4.inc"
{
    // ORIG 先把 delay 常量暂存 -0xc(%ebp) 再装入实参槽（gcc4.1 形态），
    // 用非 const 局部变量复现该栈槽暂存（const 会被 4.4 折叠为立即数）。
    int delay = 0xe10;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x5b, delay, 0, 0);
    return 1;
}

__attribute__((weak)) TimerCheckMailTimeOut::TimerCheckMailTimeOut() throw() {}
__attribute__((weak)) TimerCheckMailTimeOut::~TimerCheckMailTimeOut() throw() {}
// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x08635xxx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82/N93 封顶：帧/槽位跨版本差（106/109）。
#include "TimerDispatcher/dispatch_sig_53.inc"
{
    // ORIG 0x08635d0f：单出口 result(%ebx) + 四段判错，各 setXX %al 物化
    // （0x08635d9x `cmp $0x1,%eax; setle` / `cmp 0x10,%eax; setne` / `cmp 0x14,%eax; setne`）
    // → `test %al,%al; je` 前跳；result=1 在 PacketGuard dtor(0x08635e64) 之前。
    // [b28d-solo-TimerDispatcher] P231 纯 register：ORIG result 驻 %ebx 跨全部 call
    // 无栈槽；P0：ORIG 无 bad 槽 ⇒ 条件内联（旧具名 bool 生 mov %al,-0x11; cmpb）。
    register int result;
    CUser* user = G_CGameManager()->getUser(p1, 7);   // ORIG 第二实参为常量 7
    if (user == 0) {
        result = 0;
    } else if (user->get_state() <= 1) {
        result = 0;
    } else if (user->GetLoginTick() != p2) {
        result = 0;
    } else if (static_cast<unsigned int>(user->getCurCharacNo()) != p3) {
        result = 0;
    } else {
        PacketGuard packet;
        packet.put_header(0, 0x62);
        int countIndex = packet.get_index();
        packet.put_int(0);
        int expired = static_cast<int>(WongWork::CMailBoxHelper::CheckMailTimeout(user, &packet));
        if (expired != 0)
        {
            packet.put_int(countIndex, expired);
            packet.finalize(true);
            user->Send(packet);
        }
        WongWork::CMailBoxHelper::SetPeriodCheckTimer(user);
        result = 1;
    }
    return result;
}

__attribute__((weak)) TimerCreature::TimerCreature() throw() {}
__attribute__((weak)) TimerCreature::~TimerCreature() throw() {}
// [b28d-30] 复核 NEAR：died 槽 -0x11 vs -0xa（1 字节槽隙，N82/P190 族），ORIG 0x08635e96。
// [b28d-solo-TimerDispatcher] NEAR 封顶 N98（4.4 按首次使用序分配槽）：ORIG died 槽
// -0x11（两 byte 槽间 8B 隙）vs 4.4 -0xa；long long/volatile/double pad 三形实测反劣。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；唯一差 = died 槽 -0x11↔-0xa
// （N98/N82 槽隙，无语义缺口）。
#include "TimerDispatcher/dispatch_sig_54.inc"
{
    // ORIG 0x08635e96：getUser 第二实参为常量 1（movl $0x1,0x8(%esp)）；
    // 三段独立早退（jne 岛 + 两条 cmp/setne 物化，ORIG +0x29/+0x46/+0x62）。
    // [TD4 2026-09-10 判据⑩] ORIG 槽位浅→深 = saveInventory(-0x9) → user(-0x10) →
    // died(-0x11)；按「声明序=槽由深到浅」⇒ 原码 died 声明在 user **之前**。
    // 旧形把 died 声明在 saveInventory 之后 ⇒ died 落 -0xa（紧贴 save）。
    bool died;
    CUser* user = G_CGameManager()->getUser(p1, 1);
    if (user == 0)
        return 0;
    if (user->get_unique_id() != p2)
        return 0;
    if (user->getCurCharacNo() != p3)
        return 0;
    // CheckDiedCreature 是 user_creature::CCreatureMgr 成员（ORIG 0x08339acc）：
    // user->getCurCharacInvenW()->GetCreatureMgrW() 取管理器实例。
    // ORIG +0x99：CheckDiedCreature 结果物化 -0x11(%ebp)（4.1 无死码消除，具名复现）。
    // 技巧21：died 先声明占远槽 -0x11、saveInventory 后声明占近槽 -0x9（ORIG
    // 8635f11/8635f2f 槽号）；`x ^ 1` 具名 char 槽条件出 movzbl+xor$1+test+je
    // （ORIG 8635f32，cmpb 融合形被 ^ 表达式阻断）。
    // [b28c-L26 判词·N82/P190 族] ORIG died 落 -0x11（与 saveInventory -0x9 间留
    // 8B 隙），4.4 对 1 字节局部紧邻分配 -0xa ⇒ 本件唯一残差（2 行）。实测把 died
    // 改 long long 借 8 字节对齐推深槽基 → 55→58 反劣（store/load 变 8B 两段），
    // 已回退 bool；volatile/double framePad（P190）不适用（本函数无 double 实体）。
    // ORIG 0x08635eeb/0x08635f08：isSaveInven 先调（byte 槽 -0x9），
    // CheckDiedCreature 后调（-0x11，4.1 两 byte 槽间留隙）
    const bool saveInventory = user->isSaveInven();   // b28c-L26 档①：callee bool 化
    died = user->getCurCharacInvenW()->GetCreatureMgrW()->CheckDiedCreature();
    (void)died;
    // ORIG 8635f32 `movzbl -0x9,%eax; xor $1,%eax; test %al; je`：
    // N96/P0：绑定形全域禁用后无合法源形可阻 fold-const（探针 /tmp/l7_m.cpp
    // m1/m3 恒折 `cmpb $1,-0x9; je`；register-hint n1 形溢入 %ebx 改帧）——
    // setcc-fold 负例在册，取最短 gap 折叠形，残差 3 行封顶。
    // P168a 负例（setcc-fold）：ORIG 8635f32 `movzbl -0x9,%eax; xor $1,%eax;
    // test %al; je` 寄存器形不可造——表达式形折 cmpb $1（53 条），
    // X2 `v ^= 1` 两语句形生 xorb 栈槽物化（54 条，反劣），取折叠形封顶。
    if ((unsigned char)(saveInventory ^ 1))
    {
        user->disableSaveInven();
    }
    return 1;
}

__attribute__((weak)) TimerForbidChat::TimerForbidChat() throw() {}
__attribute__((weak)) TimerForbidChat::~TimerForbidChat() throw() {}
#include "TimerDispatcher/dispatch_sig_55.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 0xa);   // ORIG 第二实参常量 0xa
    if (user == 0)
        return 0;
    // ORIG 0x08635f50：name 临时跨调用驻留 %ebx（register），且每个使用点
    // 各自重新调用 get_charac_name_by_charac_no（真/假分支+首 trace 共 3 次）。
    register const char* name = user->get_charac_name_by_charac_no(p3);
    cMyTrace(__PRETTY_FUNCTION__, 0xa52, 0)("TimerForbidChat %s", name);
    name = user->get_charac_name_by_charac_no(p3);
    // ORIG：`!EnableChat` 前置（xor$1/test），Failed 块 fall-through、Enabled 后置。
    if (!G_GameWorld()->EnableChat(name))
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa54, 0)("Failed to enable to chat for: %s", name);
    }
    else
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa56, 0)("Enabled to chat for: %s", name);
    }
    return 1;
}

__attribute__((weak)) TimerForbidMove::TimerForbidMove() throw() {}
__attribute__((weak)) TimerForbidMove::~TimerForbidMove() throw() {}
#include "TimerDispatcher/dispatch_sig_56.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 0xb);   // ORIG 第二实参常量 0xb
    if (user == 0)
        return 0;
    // 与 TimerForbidChat 同构（ORIG 0x086360a8）：register name→%ebx、
    // 各点重新调用、!EnableMove 前置、Failed fall-through。
    register const char* name = user->get_charac_name_by_charac_no(p3);
    cMyTrace(__PRETTY_FUNCTION__, 0xa64, 0)("TimerForbidMove %s", name);
    name = user->get_charac_name_by_charac_no(p3);
    if (!G_GameWorld()->EnableMove(name))
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa66, 0)("Failed to enable to move for: %s", name);
    }
    else
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa68, 0)("Enabled to move for: %s", name);
    }
    return 1;
}

__attribute__((weak)) TimerScheduleFiveMinPerUser::TimerScheduleFiveMinPerUser() throw() {}
__attribute__((weak)) TimerScheduleFiveMinPerUser::~TimerScheduleFiveMinPerUser() throw() {}
#include "TimerDispatcher/dispatch_sig_57.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 3);   // ORIG 第二实参为常量 3
    // ORIG 0x086341d2：三段独立早退；第三段 && 经 (bool) 物化（+0x57..+0x7b 岛形）。
    if (user == 0)
        return 0;
    if (user->GetLoginTick() != p2)
        return 0;
    if ((bool)(p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3))
        return 0;
    user->WorkPerFiveMin();
    // ORIG 0x0863425f..：右到左求值，RCR 依次落 %edi(no)/%esi(tick)/%ebx(uid)
    // （4.4 自由分配先选 %ebx，asm 绑名复现）。
    register unsigned int characNo = user->getCurCharacNo();
    register unsigned int tick = user->GetLoginTick();
    register int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x5f, 300, tick, characNo);
    return 1;
}

__attribute__((weak)) TimerScheduleTenMinPerUser::TimerScheduleTenMinPerUser() throw() {}
__attribute__((weak)) TimerScheduleTenMinPerUser::~TimerScheduleTenMinPerUser() throw() {}
#include "TimerDispatcher/dispatch_sig_58.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 3);   // ORIG 第二实参为常量 3
    // ORIG 0x08634106（报告 TimerScheduleTenMinPerUser/dispatch_sig.md）：同 Twin 形。
    if (user == 0)
        return 0;
    if (user->GetLoginTick() != p2)
        return 0;
    if ((bool)(p3 != 0 && static_cast<unsigned int>(user->getCurCharacNo()) != p3))
        return 0;
    user->WorkPerTenMin();
    // 同 FiveMin 孪生（ORIG 0x08634106）。
    register unsigned int characNo = user->getCurCharacNo();
    register unsigned int tick = user->GetLoginTick();
    register int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x60, 600, tick, characNo);
    return 1;
}

__attribute__((weak)) TimerQueueSizeLog::TimerQueueSizeLog() throw() {}
__attribute__((weak)) TimerQueueSizeLog::~TimerQueueSizeLog() throw() {}
#include "TimerDispatcher/dispatch_sig_59.inc"
{
    // remain/put 为 MsgQueueMgr 成员（ORIG this=s_msgq_mgr）；2=DB_Q、4=LOGDB_Q。
    const int remaining = static_cast<int>(GlobalData::s_msgq_mgr->remain(MsgQueueMgr::DB_Q));
    if (remaining > 10)
    {
        // ORIG 0x08636246：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)（无 stream 槽）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp",
                                                              0xa7f), true);
        // ORIG 0x08636262..：每项 `*guard`（operator* deEv 0x080c8c46）后 lsEi
        // （0x0863626d/288/29e/2c7 各一次 lea+call deEv），非链式折叠。
        (*guard)->operator<<(0x9b);
        (*guard)->operator<<(-1);
        (*guard)->operator<<(remaining);
        (*guard)->operator<<(G_CEnvironment()->get_channel_no());
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }
    // ORIG 0x0863631c：`if(!RegistNextTimer()) return 0; return 1;`
    // bool 结果落槽 -0x9(%ebp)（0x08636327 mov %al,-0x9），movzbl+xor$1 尾岛。
    const bool ok = RegistNextTimer();
    if (!ok)
        return 0;
    return 1;
}

// ORIG T 0x08636348 (_ZN17TimerQueueSizeLog15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x61, 0x3c, 0, 0)；返回 1。
#include "TimerDispatcher/RegistNextTimer_5.inc"
{
    // ORIG：delay 物化到局部槽 -0xc(%ebp) 再两步入栈。
    int delay = 0x3c;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x61, delay, 0, 0);
    return 1;
}

__attribute__((weak)) TimerScheduleGuildWarEnd::TimerScheduleGuildWarEnd() throw() {}
__attribute__((weak)) TimerScheduleGuildWarEnd::~TimerScheduleGuildWarEnd() throw() {}
// TimerScheduleGuildWarEnd::dispatch_sig ORIG T 0x86331d0 (0x57f)
// [b28d-30] 复核 DIFF：巨件残差（N82/N89 族），ORIG 0x086331d0。
// [b28d-solo-TimerDispatcher] 判词 N82/N89 封顶：巨件 317 insns，帧槽 + STL 内联残差。
// [TimerDispatcher-2 2026-09-09 复核] gap=6 全为 6 条 ORIG-only `jmp`（每 case 一条
// 跳正常析构的中继；ORIG 正常路径 D1 落 catch 块之后、4.4 就地内联）⇒ N104 EH 尾块
// 相对序；两侧指令多重集其余全等（ORIG-only={'jmp':6}，OURS-only=∅）⇒ 无语义缺口。
// [TimerDispatcher-3 2026-09-10 判据①] 全 TU jcc/jmp 目标下标逐位核对：ORIG 23 条 vs
// 我方 17 条；6 条缺口全是每 case 尾 `jmp <该 case 共享 D1Ev 中继>`（目标落在 catch
// 之后），jne/je 目标逐条对应下一 case 比较与前置守卫 ⇒ 无极性反写、无语义缺口。
// [fix] ORIG 全部 6 处 case 比较读 0x10(%ebp)=**p2**（Ghidra 标 param_3 不准，
// 与本 TU TimerGlobalEffectItemBegin/Timeout 等同一模式）⇒ 旧码用 p3(0x14) 是参数用错。
#include "TimerDispatcher/dispatch_sig_60.inc"
{
    if (G_GameWorld()->GetChannelType() != 6)
    {
        return 1;
    }
    // ORIG 08633209：CEventManager::GetRepeatEvent(0x21)；vptr+0x34 虚调用判活
    // P0：ORIG 无 event 具名槽（0863320e `mov (%eax),%edx` 直用返回值）⇒ 内联表达式。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x21)->IsEventing((CUser*)0))
    {
        return 1;
    }
    if (p2 == 10)
    {
        PacketGuard packet;  // ORIG local @ -0x14(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(10);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086332d5：InsertTimer(OBJ_TYPE_2,0,0x62,300,5,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 300, 5, 0);
    }
    else if (p2 == 5)
    {
        PacketGuard packet;  // ORIG local @ -0x20(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(5);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086333a9：InsertTimer(OBJ_TYPE_2,0,0x62,60,4,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 4, 0);
    }
    else if (p2 == 4)
    {
        PacketGuard packet;  // ORIG local @ -0x2c(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(4);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 0863347d：InsertTimer(OBJ_TYPE_2,0,0x62,60,3,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 3, 0);
    }
    else if (p2 == 3)
    {
        PacketGuard packet;  // ORIG local @ -0x38(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(3);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 08633551：InsertTimer(OBJ_TYPE_2,0,0x62,60,2,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 2, 0);
    }
    else if (p2 == 2)
    {
        PacketGuard packet;  // ORIG local @ -0x44(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(2);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 08633625：InsertTimer(OBJ_TYPE_2,0,0x62,60,1,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 1, 0);
    }
    else if (p2 == 1)
    {
        PacketGuard packet;  // ORIG local @ -0x50(%ebp)
        packet.put_header(0, 0x6d);
        packet.put_byte(1);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        // ORIG 086336f9：InsertTimer(OBJ_TYPE_2,0,0x62,60,0,0)
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                    (TIMER_MESSAGE)0x62, 60, 0, 0);
    }
    else if (p2 == 0)
    {
        // ORIG 0863373e：TriggerEventEnd(0x21)
        GlobalData::s_event_manager->TriggerEventEnd(0x21);
    }
    return 1;
}

__attribute__((weak)) TimerSetPlayResult::TimerSetPlayResult() throw() {}
__attribute__((weak)) TimerSetPlayResult::~TimerSetPlayResult() throw() {}
#include "TimerDispatcher/dispatch_sig_61.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x29, p2))
    {
        return 0;
    }
    party->SetPlayResult(party->getManager());
    return 1;
}

__attribute__((weak)) TimerStartRelayBattle::TimerStartRelayBattle() throw() {}
__attribute__((weak)) TimerStartRelayBattle::~TimerStartRelayBattle() throw() {}
#include "TimerDispatcher/dispatch_sig_62.inc"
{
    // ORIG 0x0863640c：GetPvp(p1, 0, 99) → relay(=room+0x620) 计时键比对(p2) → OnFight。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, 0, 99);
    if (room == 0)
    {
        return 0;
    }
    if (room->m_relayMgr.GetTimerKey() != p2)   // ORIG room+0x620
    {
        return 0;
    }
    room->m_relayMgr.OnFight();
    return 1;
}
#include "TimerDispatcher/dispatch_sig_63.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
    {
        return 0;
    }
    // ORIG 0x08636426：直接透传 OnTimeoutBossDie 返回值（CParty.h 声明 int）。
    return party->OnTimeoutBossDie(p2, p3);
}

__attribute__((weak)) TimerEventCoinRefill::TimerEventCoinRefill() throw() {}
__attribute__((weak)) TimerEventCoinRefill::~TimerEventCoinRefill() throw() {}
#include "TimerDispatcher/dispatch_sig_64.inc"
{
    // ORIG 0x086364c4：GetRepeatEvent(0xd) → receiver->IsEventing(0)（vtbl+0x34）
    // → GameWorld::ResetAllCoins() → trace → registNextRefillTimer()。
    CCoinRefillEvent* event =
        static_cast<CCoinRefillEvent*>(GlobalData::s_event_manager->GetRepeatEvent(0xd));
    if (event->IsEventing(0))   // ORIG 0x086364c4：无 event!=0 检查，直接解引用
    {
        G_GameWorld()->ResetAllCoins();
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0xadc, 0);
        trace("Coin refilled by event!");
        event->registNextRefillTimer();
    }
    return 1;
}

__attribute__((weak)) TimerPvPDieUserRevive0::TimerPvPDieUserRevive0() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive0::~TimerPvPDieUserRevive0() throw() {}
#include "TimerDispatcher/dispatch_sig_65.inc"
{
    PvPDieUserRevive(p1, p2, p3, 0);
}

__attribute__((weak)) TimerPvPDieUserRevive1::TimerPvPDieUserRevive1() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive1::~TimerPvPDieUserRevive1() throw() {}
#include "TimerDispatcher/dispatch_sig_66.inc"
{
    PvPDieUserRevive(p1, p2, p3, 1);
}

__attribute__((weak)) TimerPvPDieUserRevive2::TimerPvPDieUserRevive2() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive2::~TimerPvPDieUserRevive2() throw() {}
#include "TimerDispatcher/dispatch_sig_67.inc"
{
    PvPDieUserRevive(p1, p2, p3, 2);
}

__attribute__((weak)) TimerPvPDieUserRevive3::TimerPvPDieUserRevive3() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive3::~TimerPvPDieUserRevive3() throw() {}
#include "TimerDispatcher/dispatch_sig_68.inc"
{
    PvPDieUserRevive(p1, p2, p3, 3);
}

__attribute__((weak)) TimerPvPDieUserRevive4::TimerPvPDieUserRevive4() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive4::~TimerPvPDieUserRevive4() throw() {}
#include "TimerDispatcher/dispatch_sig_69.inc"
{
    PvPDieUserRevive(p1, p2, p3, 4);
}

__attribute__((weak)) TimerPvPDieUserRevive5::TimerPvPDieUserRevive5() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive5::~TimerPvPDieUserRevive5() throw() {}
#include "TimerDispatcher/dispatch_sig_70.inc"
{
    PvPDieUserRevive(p1, p2, p3, 5);
}

__attribute__((weak)) TimerPvPDieUserRevive6::TimerPvPDieUserRevive6() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive6::~TimerPvPDieUserRevive6() throw() {}
#include "TimerDispatcher/dispatch_sig_71.inc"
{
    PvPDieUserRevive(p1, p2, p3, 6);
}

__attribute__((weak)) TimerPvPDieUserRevive7::TimerPvPDieUserRevive7() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive7::~TimerPvPDieUserRevive7() throw() {}
#include "TimerDispatcher/dispatch_sig_72.inc"
{
    PvPDieUserRevive(p1, p2, p3, 7);
}

__attribute__((weak)) TimerDungeonDataStatistic::TimerDungeonDataStatistic() throw() {}
__attribute__((weak)) TimerDungeonDataStatistic::~TimerDungeonDataStatistic() throw() {}
// ---- TimerDungeonDataStatistic ----
// 报告：docs/class_func_reports/TimerDungeonDataStatistic/{dispatch_sig,RegistNextTimer}.md

// ORIG T 0x0863672a (_ZN25TimerDungeonDataStatistic12dispatch_sigEiij)
// [b28d-30] 复核 DIFF：N104 EH 尾块相对序（unwind 块与正常尾块互换、内容全等），ORIG 0x0863672a。
// [b28d-solo-TimerDispatcher] 判词 N104 封顶：ORIG unwind 块居中、正常 D1 尾块最后；
// 内容逐条全等，仅两尾块互换（4.4 恒把 epilogue 就地内联）。
#include "TimerDispatcher/dispatch_sig_73.inc"
{
    // ORIG 0x0863674a-0x08636757：Acquire 免槽直落 ctor；guard 全程作用域
    // （dtor 在 updateDataIntoDB/Regist 之后，catch @0fc）；写流走显式
    // operator*() 代理链；GetInBuffer 经 operator->()（ORIG 0x086367a2 ptEv call）
    register int ret;   // [b28d-solo] 共享尾 %ebx（ORIG 单出口）
    {
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp", 0xb1a), true);
    guard.operator*()->operator<<((int)0xaf);
    guard.operator*()->operator<<((int)-1);
    SIG_DUNGEON_DATA_STATISTIC* stat =
        guard.operator->()->GetInBuffer<SIG_DUNGEON_DATA_STATISTIC>();
    memset(stat, 0, 0x80);
    *(int*)stat = 3;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    GetInstanceVendingMachineStatistic()->updateDataIntoDB();
    // ORIG 0x0863680a-0x08636824：Regist byte 槽 -0x9 物化 + `xor $0x1; test; je`
    // 双岛直存 %ebx 跨 dtor（绑定消 bool byte 槽），单出口 return %ebx
    bool ok = RegistNextTimer();
    if (ok != 1)
        ret = false;
    else
        ret = true;
    }
    return ret;
}

// ORIG T 0x08636856 (_ZN25TimerDungeonDataStatistic15RegistNextTimerEv)
// ORIG 非 static（调用点传 this）返回 bool（ORIG T 0x08636856）
// [b28d-30] 复核 DIFF：帧差 0x14（0x64 vs 0x78，P232/P210 族），ORIG 0x08636856。
// [b28d-solo-TimerDispatcher] 判词 N1 封顶：`%0x1e` 常量除法展开算术折叠（ORIG
// lea+shl+sub 链 vs 4.4 add+shl+sub），源形不可择（74/74）。
// [TimerDispatcher-2 2026-09-09 复核] 同上 N86b/N1（ORIG-only lea/mov vs OURS-only
// add 各 1，余全等）⇒ 封顶，无语义缺口。
// [TimerDispatcher-3 2026-09-10 判据①] 唯一 jg 目标差 1 = `%0x1e` 强度归约 2x 的
// lea↔add（探针 /tmp/td_probe5.cpp r1..r6 六形恒 add）⇒ N86b/N1；分支语义等价。
#include "TimerDispatcher/RegistNextTimer_6.inc"
{
    // [TimerDispatcher-4 2026-09-11] N86b/N1 复证（22 形探针 /tmp/mod_probe{,2,3}.cpp）：
    // 4.4 对 `x*0x1e` 独立展开同为 add 形（f17 `add %eax,%eax; mov %eax,%edx;
    // shl $4,%edx; sub %eax,%edx`），与 `%0x1e` 余数段逐条同构 ⇒ ORIG 的
    // `lea (%eax,%eax,1),%edx`（4.1 expand_mult 目标寄存器选择）不可由源码形诱导；
    // 探针含显式 `X-(X/30)*30`(f3/f9/f15/imul 形)、手写分解 (2q<<4)-2q(f4/f8/f19)、
    // 具名 q(f16/f20 reload)、语句表达式——唯一命中 lea 的 f8 需具名 q 落槽且整体多
    // reload+add 反劣。p167：ORIG 6 calls = OURS 6 CLEAR ⇒ 无语义缺口，封底。
    // ORIG 0x086368xx：curSec 单 time_t 槽（免 int→long 拷贝）；localtime_r 返回
    // pt 落槽 -0x18，各字段经 `mov -0x18,%eax` 重读 + 指针 disp 读写（技巧37 反用：
    // 具名指针槽正是 ORIG 形）
    time_t curSec = GlobalData::s_systemTime_.getCurSec();
    tm t;
    tm* pt = localtime_r(&curSec, &t);
    const int curHour = pt->tm_hour;
    pt->tm_hour = 5;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    // ORIG 0x086368a7-0x086368e1：chn 全程 %ecx、x=chn/30（0x88888889+sar4 魔数）
    // 存 %ebx（sign 修正后）、2x=%edx；`shl $4; sub %edx` =30x；
    // tm_min = chn-30x = chn%30（P196：旧 /15*45 形在 chn≥15 分叉系误判）。
    // P0：ORIG chn/x/x2 全程寄存器无槽（086368a7 `mov 0x1b0(%eax),%ecx` 直入
    // 魔数除法）⇒ 表达式内联为 `% 0x1e`（旧三个具名局部多 3 槽 + 全帧平移）。
    pt->tm_min = G_CEnvironment()->m_serverEnvir.m_channelNo % 0x1e;  // +0x1b0
    int target = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= curHour)
        target += 0x15180;
    const int delay = target - static_cast<int>(curSec);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x66,
                                delay, 0, 0);
    return 1;
}

__attribute__((weak)) TimerNotResponseUserTimeout::TimerNotResponseUserTimeout() throw() {}
__attribute__((weak)) TimerNotResponseUserTimeout::~TimerNotResponseUserTimeout() throw() {}
// ---- TimerNotResponseUserTimeout ----
// 报告：docs/class_func_reports/TimerNotResponseUserTimeout/dispatch_sig.md

// ORIG T 0x0863695a (_ZN27TimerNotResponseUserTimeout12dispatch_sigEiij)
// [b28d-30] 复核 DIFF：帧差 4 + 槽位差（N7/N82 族），ORIG 0x0863695a。
// [b28d-solo-TimerDispatcher2] 本轮改善：P231 纯 register 使 GetPeerIP2 结果回 %ebx、
// push ebx/帧 0x44/user 槽 -0xc 全对齐、68/68。残差 = ipBuf 块级槽序（ORIG -0x30..-0x20
// vs 4.4 -0x20..-0x10，N82/P212）+ 成员偏移取址 add-vs-lea（N107）；IsRecvEvent 需 bool
//（CUser.h 冻结，已上报 Main）。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等（无语义缺口）；唯一残留 =
// `user+0xe0` 成员取址 lea↔add（N107 六形封顶）。
#include "TimerDispatcher/dispatch_sig_74.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 0xc);
    if (user == NULL)
        return 0;
    if ((unsigned short)user->get_unique_id() != p2)
        return 0;
    // N96/P0：uchar callee 无槽折叠形（probe /tmp/l7_m.cpp：4.4 恒折
    // cmpb$1 直判，xor 寄存器物化属 setcc-fold 负例）。
    if ((unsigned char)(user->IsRecvEvent() ^ 1))
    {
        // ORIG 0x086369e6：DisConnSig(0x1d, false, 0) 断线
        user->DisConnSig((DISCONN_SIG)0x1d, false, 0);
        // [b28d-solo-TimerDispatcher] ORIG 0x08636a1d..0x08636a34：0x14 字节缓冲
        // 以 5×`movl $0x0,-0x30..-0x20(%ebp)` 清零（4.1 对 20B 块按 dword 展开）
        // ⇒ 源形用 int[5] 具名缓冲（逐字节 movb 反劣），语义同为全零 IP 缓冲。
        int ipBuf[5] = {0};
        // [b28d-solo-TimerDispatcher2] ORIG `mov %eax,%ebx`（结果驻 %ebx 跨 cMyTrace
        // ctor，无栈槽）⇒ P231/P307 纯 register 存储类提示（非 asm 绑定，P0 合规）。
        // [G9WorldTimerLeague 2026-09-10 复核] N107 封顶确认：ORIG 0x08636a11
        // `lea 0xe0(%eax),%edx` 取 m_network 成员地址；我方 4.4 恒 `add $0xe0,%edx`
        // （成员常量偏移取址 add-vs-lea，N107 六形探针全产 add）⇒ 语义等价，勿再试。
        register const char* ipStr =
            user->m_network.GetPeerIP2((char*)ipBuf, 0x14);  // +0xe0
        cMyTrace(__PRETTY_FUNCTION__,
                 0xb73, 0)("Not response user timeout: idx(%d), IP(%s)", p1, ipStr);
    }
    return 1;
}

__attribute__((weak)) TimerLightServerNotice::TimerLightServerNotice() throw() {}
__attribute__((weak)) TimerLightServerNotice::~TimerLightServerNotice() throw() {}
// [b28d-30] 复核 DIFF：越界检查形/帧差（N82 族），ORIG 0x08636a7c。
// [b28d-solo-TimerDispatcher] 判词 N104 封顶：P183 去 notices/packet 缓存 + 真语义
// bug（第二形参）修正后 112/112 助记符全等；唯一残差 = EH 尾块相对序（ORIG 正常路径
// D1 在 +0x183 最末、unwind 块居中 +0x168；4.4 就地内联 D1、unwind 压最末）⇒ N104。
// [b28d-solo-TimerDispatcher] 真语义 bug 修正：ORIG 全程读 **0x10(%ebp) = 第二形参**
// （越界检查 08636a84、size 08636a9f、operator[] 08636b1c、RegistNextTimer 08636bb6），
// 旧码误用第三形参 p3（0x14(%ebp)）。
#include "TimerDispatcher/dispatch_sig_75.inc"
{
    // ORIG 0x08636a7c：越界检查（idx 负或 >= size 直接 false）→ 构造 0xc 包
    // {byte 0, short 0, byte 0, int len, str} 广播 → RegistNextTimer(idx)。
    // P183 地址重物化铁律：ORIG 每个站点重发 `call G_CDataManager; add $0xa4,%eax`
    // （08636a8d/08636b1f/08636b58），禁 `std::vector<...>& notices = ...` 引用缓存。
    register int ret;
    if (p2 < 0 ||
        static_cast<unsigned int>(p2) >=
            G_CDataManager()->m_serverParameter.m_light3c.size())
    {
        ret = 0;
    }
    else
    {
    PacketGuard guard;
    // P89/P183：ORIG 每站点 `lea -0x1c(%ebp),%eax; mov %eax,(%esp)`（guard 地址直传），
    // 禁具名 `InterfacePacketBuf& packet`（多占 -0x18 槽）。
    static_cast<InterfacePacketBuf&>(guard).put_header(0, 0xc);
    static_cast<InterfacePacketBuf&>(guard).put_byte(0);
    static_cast<InterfacePacketBuf&>(guard).put_short(0);
    static_cast<InterfacePacketBuf&>(guard).put_byte(0);
    int len = G_CDataManager()->m_serverParameter.m_light3c[p2].m_message.size();
    // ORIG 0x08636b50：先 put_int(len) 再 put_str
    static_cast<InterfacePacketBuf&>(guard).put_int(len);
    static_cast<InterfacePacketBuf&>(guard).put_str(
        G_CDataManager()->m_serverParameter.m_light3c[p2].m_message.c_str(), len);
    static_cast<InterfacePacketBuf&>(guard).finalize(true);
    G_GameWorld()->send_all(guard);
    const bool done = RegistNextTimer(p2);
    // P231/P307：ORIG 汇聚变量驻 %ebx（08636bd6 `mov $0x0,%ebx` / 08636bdd
    // `mov $0x1,%ebx`）且无栈槽 ⇒ 纯 register 存储类提示（非 asm 绑定，P0 合规）。
    if (!done)
        ret = 0;
    else
        ret = 1;
    }
    return ret;
}

// ORIG T 0x08636c14（_ZN22TimerLightServerNotice15RegistNextTimerEi）
#include "TimerDispatcher/RegistNextTimer_7.inc"
{
    // ORIG：localtime_r 返回值 pt 具名存槽 -0x18，字段读写全经 pt 指针
    // （`mov -0x18,%eax; mov %edx,0x8(%eax)` 形），非 cur 折叠；curSec 槽 -0x1c。
    // G_CDataManager()+0xa4 的 vector operator[] 按 P48 每用点重取（tm_hour/tm_min
    // 各自独立 operator[] + G_CDataManager 调用），禁缓存 notice 引用。
    time_t curSec = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm cur;
    tm* pt = localtime_r(&curSec, &cur);
    int prevHour = pt->tm_hour;
    // P48：两处各自主调用 G_CDataManager()+0xa4 operator[]（ORIG 双调用）。
    pt->tm_hour = G_CDataManager()->m_serverParameter.m_light3c[idx].m_hour;
    pt->tm_min = G_CDataManager()->m_serverParameter.m_light3c[idx].m_min;
    pt->tm_sec = 0;
    int next = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= prevHour)
    {
        next += 0x15180;
    }
    int delay = static_cast<int>(next - curSec);
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                static_cast<TIMER_MESSAGE>(0x68), delay, idx, 0);
    return 1;
}

__attribute__((weak)) TimerInsertUserPlayTime::TimerInsertUserPlayTime() throw() {}
__attribute__((weak)) TimerInsertUserPlayTime::~TimerInsertUserPlayTime() throw() {}
#include "TimerDispatcher/dispatch_sig_76.inc"
{
    // ORIG 0x08636d0a：统计单例落盘 → 重排当日 04:30 定时器。
    GetInstanceUserPlayTimeStatisticMgr()->InsertUserPlayTime();
    // ORIG +0x1e..：bool 结果落 -0x9 槽，reload+xor$1（Midnight 同形，探针 t5）。
    const bool registered = RegistNextTimer();
    if (!registered)
        return 0;
    return 1;
}

// ORIG T 0x08636d44（_ZN23TimerInsertUserPlayTime15RegistNextTimerEv）
#include "TimerDispatcher/RegistNextTimer_8.inc"
{
    // ORIG 0x08636：curSec 落槽 -0x1c（time_t 直传 &）；localtime_r 返回值具名 pt
    // 槽 -0x18；prevHour -0x14 / next -0x10；全部字段读写经 pt 指针
    // （`mov -0x18,%eax; movl $0x4,0x8(%eax)` 形，ORIG 0x08636xxx），非 cur 折叠。
    time_t curSec = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm cur;
    tm* pt = localtime_r(&curSec, &cur);
    int prevHour = pt->tm_hour;
    pt->tm_hour = 4;
    pt->tm_min = 0x1e;
    pt->tm_sec = 0;
    int next = static_cast<int>(mktime(pt));
    if (pt->tm_hour <= prevHour)
    {
        next += 0x15180;
    }
    int delay = static_cast<int>(next - curSec);
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2), 0,
                                static_cast<TIMER_MESSAGE>(0x69), delay, 0, 0);
    return 1;
}

__attribute__((weak)) TimerCheckForcedDisconnect::TimerCheckForcedDisconnect() throw() {}
__attribute__((weak)) TimerCheckForcedDisconnect::~TimerCheckForcedDisconnect() throw() {}
// ---- TimerCheckForcedDisconnect ----
// 报告：docs/class_func_reports/TimerCheckForcedDisconnect/{dispatch_sig,RegistNextTimer}.md

// ORIG T 0x08636e06 (_ZN26TimerCheckForcedDisconnect12dispatch_sigEiij)
// 参数语义（实测 2026-08-30）：getUser(p1, 0x2)，比较 unique_id!=p2；
// NEW 曾误用 p2/p3，已修正。
#include "TimerDispatcher/dispatch_sig_77.inc"
{
    // ORIG 0x08636e06：结果驻留 %ebx；四段 else-if 链（jne 岛 mov$0/%1）；
    // RegistNextTimer 为成员调用（+0xca mov 0x8(%ebp)=this；+0xd5 bool 落 -0x9）。
    register int result;
    CUser* user = G_CGameManager()->getUser(p1, 0x2);
    if (user == 0)
    {
        result = 0;
    }
    else if (user->get_unique_id() != p2)
    {
        result = 0;
    }
    else if (user->get_state() != 7)
    {
        result = 1;
    }
    else
    {
        PacketGuard packet;
        packet.put_header(0, 0x7b);
        packet.finalize(true);
        user->Send(packet);
        const bool registered = RegistNextTimer(user);
        if (!registered)
            result = 0;
        else
            result = 1;
    }
    return result;
}

// ORIG T 0x08636f26 (_ZN26TimerCheckForcedDisconnect15RegistNextTimerEP5CUser)
// 头声明为 static bool；ORIG eax 恒为 1，语义等价。
// TimerDispatcher.h 现签名 `static bool RegistNextTimer(CUser*)` 即可，无需改动。
#include "TimerDispatcher/RegistNextTimer_9.inc"
{
    // ORIG 0x08636f26：delay 先落 -0xc 槽（movl $0xa,-0xc）；两次 call 结果
    // 直落 %esi/%ebx（register 形态，探针 x10 r1）。
    int delay = 10;
    register int uid16 = static_cast<unsigned short>(user->get_unique_id());   // ORIG movzwl %ax,%esi 定义处零扩展
    register unsigned int uid = user->GetUID();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, uid,
                                (TIMER_MESSAGE)0x6a, delay, uid16, 0);
    return 1;
}

__attribute__((weak)) TimerCheckPvPRoomUser::TimerCheckPvPRoomUser() throw() {}
__attribute__((weak)) TimerCheckPvPRoomUser::~TimerCheckPvPRoomUser() throw() {}
// ===== TimerCheckPvPRoomUser =====
// dispatch_sig ORIG _ZN21TimerCheckPvPRoomUser12dispatch_sigEiij @0x08636f90 (0xa9B)

#include "TimerDispatcher/dispatch_sig_78.inc"
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, (int)0x3c);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x3c) != p2)
    {
        return 0;
    }
    if (room->get_state() != 2)
    {
        return 0;
    }
    // ORIG +0x80: check_recv_pvp_rank()
    room->check_recv_pvp_rank();
    // ORIG +0x90: !IsEndPvpBattle()（bool 声明 ⇒ xor $0x1,%eax；PvPR 同步 .h/.cpp）
    if (!room->IsEndPvpBattle())
    {
        room->send_pvp_end();
    }
    return 1;
}

__attribute__((weak)) TimerCheckRecvPvpEndResult::TimerCheckRecvPvpEndResult() throw() {}
__attribute__((weak)) TimerCheckRecvPvpEndResult::~TimerCheckRecvPvpEndResult() throw() {}
// ---- TimerCheckRecvPvpEndResult ----
// 报告：docs/class_func_reports/TimerCheckRecvPvpEndResult/dispatch_sig.md

// ORIG T 0x0863703a (_ZN26TimerCheckRecvPvpEndResult12dispatch_sigEiij)
#include "TimerDispatcher/dispatch_sig_79.inc"
{
    // ORIG 0x0863703a：GetPvp 首参=0xc(%ebp)=p1；timer key 比较=0x10(%ebp)=p2
    // （旧版误用 p2/p3，Ghidra 参数标注偏移不可信，以 asm 为准）。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, NULL, 0x3d);
    if (room == NULL)
        return 0;
    if (room->get_timer_key((TIMER_MESSAGE)0x3d) != p2)
        return 0;
    if (room->get_recv_pvp_end_result_count() != 0)
    {
        room->end_pvp_result();
        PacketGuard packet;
        room->make_state_info((char*)&packet);
        G_GameWorld()->send_all(packet);
        room->end_pvp();
        room->battle_reset();
    }
    return 1;
}

__attribute__((weak)) TimerCheckHumanCertifyTimeout::TimerCheckHumanCertifyTimeout() throw() {}
__attribute__((weak)) TimerCheckHumanCertifyTimeout::~TimerCheckHumanCertifyTimeout() throw() {}
// [b28d-30] 复核 DIFF：槽位差 8（N82/N93 族），ORIG 0x086369xx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82+P292 封顶：槽平移 -0xc↔-0x14 + 块序差；
// isSendMailCertified 需 bool（CUser.h 冻结，已上报 Main）（105/113）。
// [TimerDispatcher-3 2026-09-10 判据①] 15 条 jcc 目标逐条对应（整体 +1 位移由一条 mov
// 中转引起）；唯一差 = call 结果 eax↔edx 中转（N22/N7；反形实测 116 insns 已回滚）。
#include "TimerDispatcher/dispatch_sig_80.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 2);

    if (user == 0 || user->get_state() < 2)
    {
        return 0;
    }

    if (user->isHumanCertified() && user->isSendMailCertified())
    {
        return 1;
    }

    // [b28e-TimerDispatcher] 比较反形实测 116 insns（原 106）⇒ 回退；ORIG `cmp %edx,%eax`
    // 的 call 结果留 %eax 属 N22/N7 寄存器角色差（P248 一次性旋钮，赌输即回滚）。
    // [TD4 2026-09-10 判据①/P248 档③实测] 交换操作数书写序为 `(unsigned)p2 < key`
    // ⇒ 4.4 反劣（push %ebx + `cmp %eax,%ebx; setb`，帧 0x38→0x34、insns 106→107）；
    // ORIG `cmp %edx,%eax; seta`（call 结果留 %eax）属 N22/N7 寄存器角色差，回滚原形。
    // [TD-2026-09-11] H3CUser 已按 Main 核准把 `CUser::getHumanCertifyTimerKey()` 返回类型
    // int→unsigned（CUser.h:3405 + CUser/getHumanCertifyTimerKey.inc:1）⇒ 删左侧冗余 cast，
    // 落成探针 /tmp/td_p2.cpp f7 精确形（ORIG 086371cd `mov 0x10(%ebp),%edx; cmp %edx,%eax;
    // seta` = 2 条形）。p2 形参为 int ⇒ 右侧 cast 必须保留（A12 实验：裸 `key() > p2`
    // 落有符号 setg，语义改变 106→104 insns）。左侧 cast 变冗余也是 P41 变体诱因。
    if (user->getHumanCertifyTimerKey() > (unsigned int)p2)
    {
        return 1;
    }

    // P0：ORIG point 无槽（getPoint 结果 %eax 直入 0x4(%esp)）⇒ 实参内联。
    user->m_historyLog.RequestCleanPad(user->m_mcap.getPoint(), '\x03');

    // ORIG 08636xxx：5×`movl $0x0` 展开（20B 数组 = {0} 初始化，落在 RequestCleanPad
    // 之后）；旧循环形生 movw/addl/setle 循环体。
    unsigned short local_24[10] = {0};

    // P168a①：bool callee 一律裸形（技巧1：`!boolcall()` 直产 xor$1;test;je）
    if (!user->checkHumanCertify(1, local_24))
    {
        // ORIG 08636xxx：`call isSendMailCertified; xor $1; test; je <else>` ⇒
        // 原码为否定式 `if (!isSendMailCertified())`，正分支（reset 组）直落、else 后置。
        if (!user->isSendMailCertified())
        {
            user->setHumanCertified(true);
            user->SendMail(false);
            user->resetCleanpadFailCnt();
            user->resetHumanCertifyData();
        }
        else
        {
            user->DisConnSig((DISCONN_SIG)0x34, true, 0);
        }
    }

    return 1;
}

__attribute__((weak)) TimerUpdateItemGenerateStatistics::TimerUpdateItemGenerateStatistics() throw() {}
__attribute__((weak)) TimerUpdateItemGenerateStatistics::~TimerUpdateItemGenerateStatistics() throw() {}
#include "TimerDispatcher/dispatch_sig_81.inc"
{
    // ORIG 0x08637170：两级链式（%eax 直落 (%esp)，无具名槽），帧 0x18；
    // getCurSec 结果寄存器内 add $0x15180 后直传 registNextTimer。
    G_CDataManager()->getItemGenerator()->onTimerEvent(0x6b);
    registNextTimer(GlobalData::s_systemTime_.getCurSec() + 0x15180);
    return 1;
}

// ORIG T 0x08637318 _ZN33TimerUpdateItemGenerateStatistics15registNextTimerEl
#include "TimerDispatcher/registNextTimer.inc"
{
    std::tm timePoint;
    localtime_r(&pointSec, &timePoint);
    timePoint.tm_hour = 0x17;
    timePoint.tm_min = 0;
    timePoint.tm_sec = 0;
    // [b28d-30] ORIG 0x086373xx：mktime 结果写回形参槽 0x8(%ebp)（`mov %eax,0x8(%ebp)`）
    // ⇒ 原码给形参 pointSec 赋值（值传递副本），4.4 直写形参槽复现（旧具名 base 局部
    // 落 -0x10 槽使 candidates 整体上移 8）。
    pointSec = mktime(&timePoint);
    // [b28d-30] ORIG 086373xx：两值落独立槽 -0x10/-0xc 且实参取址为单 lea（无 add）
    // ⇒ 原码是两个独立 int 局部（数组形 &arr[1] 4.4 恒折 lea+add，P30/N4）；
    // max 结果留 %ebx 跨 G_TimerQueue call（P231 纯 register）。
    // N93 例外（标量可翻）+ P197③：声明序定槽（candMin 先声明占 -0xc、candBase
    // 后声明占 -0x10），赋值语句序独立（ORIG 先算 candBase 再 candMin=1）⇒ 声明与
    // 赋值拆两句。
    int candMin;
    int candBase;
    candBase = static_cast<int>(pointSec - GlobalData::s_systemTime_.getCurSec());
    candMin = 1;
    register const int delay = std::max(candMin, candBase);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x6b, delay, 0, 0);
}

__attribute__((weak)) TimerUpdatePvPGrade::TimerUpdatePvPGrade() throw() {}
__attribute__((weak)) TimerUpdatePvPGrade::~TimerUpdatePvPGrade() throw() {}
#include "TimerDispatcher/dispatch_sig_82.inc"
{
    // ORIG 0x86373c6：makeRequest() → getCurSec() → registNextTimer(sec+0x15180)。
    DB_UpdatePvPGrade::makeRequest();
    registNextTimer(GlobalData::s_systemTime_.getCurSec() + 0x15180);
    return 1;
}

// ORIG T 0x086373f2（objdump 转写，尾界 next symbol 0x86374b5）：
// channel_no!=1 直接返回；否则 localtime_r(&sec) 对齐当日 02:00:00，
// delay=max(target-curSec,1)，InsertTimer(OBJ_TYPE_2,0,0x6c,delay,0,0)。
#include "TimerDispatcher/registNextTimer_1.inc"
{
    if (G_CEnvironment()->get_channel_no() != 1)
        return;
    tm timeInfo;
    localtime_r(&sec, &timeInfo);
    timeInfo.tm_hour = 2;
    timeInfo.tm_min = 0;
    timeInfo.tm_sec = 0;
    sec = static_cast<long>(mktime(&timeInfo));   // ORIG mov %eax,0x8(%ebp) 回写参数槽
    // b28c-28（P0 清绑 + P52 内联实参）：ORIG 0x08637449 段 =
    // `mov 0x8(%ebp),%ebx; call getCurSec; mov %ebx,%edx; sub %eax,%edx;
    // mov %edx,%eax; test; jle` → then 臂 `mov 0x8(%ebp),%ebx; call getCurSec;
    // sub %eax,%ebx` / else 臂 `mov $0x1,%ebx`，delay 无槽、经 %ebx 跨
    // G_TimerQueue call ⇒ 三目整体内联到 InsertTimer 实参位（两臂各自重调
    // getCurSec，P48 无 CSE）。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x6c,
        sec - static_cast<long>(GlobalData::s_systemTime_.getCurSec()) > 0
            ? static_cast<int>(sec -
                               static_cast<long>(GlobalData::s_systemTime_.getCurSec()))
            : 1,
        0, 0);
}

__attribute__((weak)) TimerDetectDisconnect::TimerDetectDisconnect() throw() {}
__attribute__((weak)) TimerDetectDisconnect::~TimerDetectDisconnect() throw() {}
#include "TimerDispatcher/dispatch_sig_83.inc"
{
    // ORIG 0x86374bc: GetInstanceDisconnectDetecter()->DoDetectingDisconnect() 为真时 registNextTimer(); return 1
    if (disconnect_detecter::GetInstanceDisconnectDetecter()->DoDetectingDisconnect()) {
        registNextTimer();
    }
    return 1;
}

__attribute__((weak)) TimerCompleteLoadAssaultTimeout::TimerCompleteLoadAssaultTimeout() throw() {}
__attribute__((weak)) TimerCompleteLoadAssaultTimeout::~TimerCompleteLoadAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_84.inc"
{
    // ORIG 0x863752c: GetInstanceAssaultMgr() -> OnTimeoutCompleteLoad(p1,p2), return 1
    // ORIG：p1 先物化到 callee-saved %ebx（跨 GetInstance 调用存活），mgr 不落槽。
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutCompleteLoad(partyIdx, p2);
    return 1;
}

__attribute__((weak)) TimerConnectP2PAssaultTimeout::TimerConnectP2PAssaultTimeout() throw() {}
__attribute__((weak)) TimerConnectP2PAssaultTimeout::~TimerConnectP2PAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_85.inc"
{
    // ORIG 0x863759c: GetInstanceAssaultMgr() -> OnTimeoutConnectP2P(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutConnectP2P(partyIdx, p2);
    return 1;
}

__attribute__((weak)) TimerEndAssault::TimerEndAssault() throw() {}
__attribute__((weak)) TimerEndAssault::~TimerEndAssault() throw() {}
#include "TimerDispatcher/dispatch_sig_86.inc"
{
    // ORIG 0x863760c: GetInstanceAssaultMgr() -> OnTimeoutEndAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutEndAssault(partyIdx, p2);
    return 1;
}

__attribute__((weak)) TimerCloseAssault::TimerCloseAssault() throw() {}
__attribute__((weak)) TimerCloseAssault::~TimerCloseAssault() throw() {}
#include "TimerDispatcher/dispatch_sig_87.inc"
{
    // ORIG 0x863767c: GetInstanceAssaultMgr() -> OnTimeoutCloseAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutCloseAssault(partyIdx, p2);
    return 1;
}

// ---- pvp_assault 超时定时器注册（ORIG T，四函数同构，仅 TIMER_MESSAGE 不同）----
// objdump 0x863755a/0x86375ca/0x863763a/0x86376aa：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE_2, p1, msg, (int)sec, p2, 0)
#include "TimerDispatcher/registNextTimer_2.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x6e, static_cast<int>(sec),
                                p2, 0);
}

#include "TimerDispatcher/registNextTimer_3.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x6f, static_cast<int>(sec),
                                p2, 0);
}

#include "TimerDispatcher/registNextTimer_4.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x70, static_cast<int>(sec),
                                p2, 0);
}

#include "TimerDispatcher/registNextTimer_5.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x71, static_cast<int>(sec),
                                p2, 0);
}

__attribute__((weak)) TimerCompleteLoadAfterAssaultTimeout::TimerCompleteLoadAfterAssaultTimeout() throw() {}
__attribute__((weak)) TimerCompleteLoadAfterAssaultTimeout::~TimerCompleteLoadAfterAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_88.inc"
{
    // ORIG 0x86376ec: GetInstanceAssaultMgr() -> OnTimeoutCompleteLoadAfterAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutCompleteLoadAfterAssault(partyIdx, p2);
    return 1;
}

__attribute__((weak)) TimerConnectP2PAfterAssaultTimeout::TimerConnectP2PAfterAssaultTimeout() throw() {}
__attribute__((weak)) TimerConnectP2PAfterAssaultTimeout::~TimerConnectP2PAfterAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_89.inc"
{
    // ORIG 0x863775c: GetInstanceAssaultMgr() -> OnTimeoutConnectP2PAfterAssault(p1,p2), return 1
    register unsigned int partyIdx = (unsigned int)p1;
    ((pvp_assault::CAssaultMgr*)pvp_assault::GetInstanceAssaultMgr())
        ->OnTimeoutConnectP2PAfterAssault(partyIdx, p2);
    return 1;
}

// ORIG T 0x0863778a size 0x42：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE 1, partyIdx, TIMER_MESSAGE 0x72,
//                               sec, key, 0)
// —— CloseAssault 后按队安排 ConnectP2PAfterAssault 超时重载。
#include "TimerDispatcher/registNextTimer_6.inc"
                                                         int p1, int p2)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)1, p1,
                                (TIMER_MESSAGE)0x72, (int)sec,
                                (unsigned int)p2, 0);
}

__attribute__((weak)) TimerDeathTower::TimerDeathTower() throw() {}
__attribute__((weak)) TimerDeathTower::~TimerDeathTower() throw() {}
#include "TimerDispatcher/dispatch_sig_90.inc"
{
    WongWork::CDeathTower* tower = G_CGameManager()->getDeathTower(p1);
    if (tower == NULL) {
        return 0;
    }
    tower->onTimer(static_cast<TIMER_MESSAGE>(p3), p2);
    return 1;
}

__attribute__((weak)) TimerDeathTowerTopRankerReload::TimerDeathTowerTopRankerReload() throw() {}
__attribute__((weak)) TimerDeathTowerTopRankerReload::~TimerDeathTowerTopRankerReload() throw() {}
#include "TimerDispatcher/dispatch_sig_91.inc"
{
    // ORIG 0x8637816：两阶段加载 TopRanker（false 增量 → true 全量），
    // 再把下轮触发排到次日 04:00 + rand(0..299) 分钟。
    DB_DeathTowerLoadTopRanker::makeRequest(false);
    DB_DeathTowerLoadTopRanker::makeRequest(true);
    registNextTimer(static_cast<long>(GlobalData::s_systemTime_.getCurSec()) + 0x15180);
    return 1;
}

__attribute__((weak)) TimerScheduleTenSecond::TimerScheduleTenSecond() throw() {}
__attribute__((weak)) TimerScheduleTenSecond::~TimerScheduleTenSecond() throw() {}
// [b28d-30] 复核 DIFF：迭代器槽/帧差（N82/N86 族），ORIG 0x08637936 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82/N86 封顶：迭代器槽 + 帧差（86/79）。
// 附：p167 显示 ORIG 用 const_iterator 族（P96/P270），但改 const 形后本 TU 不再
// 发射该族条件 ⇒ identical/ae -5、missing +5（符号发射恶化）⇒ 已回退，按非 const 封顶。
// [b28e-TimerDispatcher] AE（86/86，逐条同构，仅 .o 重定位符号名差）：本轮三处真修
// ①真 bug：antibot 调用旧码 `*(CheckFn*)((*(void***)checker)[5])` **多一次解引用**
//   （调用目标错，我方多 1 条 `mov (%eax),%eax`）⇒ 改单层
//   `*reinterpret_cast<CheckFn*>(reinterpret_cast<char*>(*reinterpret_cast<void**>(
//   s_antibotChecker_)) + 0x14)` 内联进调用实参（ORIG `mov (%eax),%esi; call *%esi`
//   驻 %esi 形逐条复现，push esi/sub 0x30 全对齐）；
// ②P13：end 迭代器具名缓存到循环外（ORIG 08637982/08637997 一次求值，旧码每轮重发
//   endEv+C1ERKS_ 多 6 insn/轮）；③P211/P248 档③：CheckFn 第二形参 unsigned→int
//   （getProcessSequence 返回 int，异型多 1 条 `mov %eax,%edx` 中转）。
// 历史判词（N7+N82 封顶、P326 回退）作废。
#include "TimerDispatcher/dispatch_sig_92.inc"
{
    // ORIG 0x8637936：遍历 +0x22c 用户 map；get_state()>=2 且 getAntibotKey()>=0
    // 时经 s_antibotChecker_ 虚槽 +0x14（antibot 检查，签名 (void*, seq, 0, accId)）
    // 发起防机器人校验；循环完 registNextTimer(10) 返回 1。
    // [b28d-solo-TimerDispatcher2] P167 差集 MISSING = const_iterator 族
    // （C1ERKS_ ×2 = begin()/end() 的 iterator→const_iterator 转换 + const 版
    // ptEv/ppEv/ne）、EXTRA = 非 const 版 ⇒ 原码是**非 const map 引用 + const_iterator
    // 循环变量**（若 map 引用本身 const 则 begin/end 直返 const_iterator、C1ERKS_ 不再
    // 发射，故旧「改 const 引用」形计数恶化）。ORIG 0x08637936。
    // P0/P183：ORIG 无 users 引用局部（08637936 `call G_CGameManager; lea 0x22c(%eax),%edx`
    // 每站点重发）⇒ 全表达式内联（旧具名引用多 -0x18 槽 + 2 insn）。
    // [b28e-TimerDispatcher] P13：ORIG 在循环初始化区**一次**求值 end（08637982 endEv +
    // 08637997 C1ERKS_，槽 -0x10/-0x1c），cond 只用缓存值（08637a3a ne 直比）⇒ 源码
    // 必须把 end 迭代器具名缓存；旧码 cond 内每轮重发 endEv+C1ERKS_（多 6 insn/轮）。
    std::map<unsigned int, CUser*>::const_iterator it =
        G_CGameManager()->m_userByAccId.begin();
    std::map<unsigned int, CUser*>::const_iterator itEnd =
        G_CGameManager()->m_userByAccId.end();
    for (; it != itEnd; ++it)
    {
        CUser* user = it->second;
        if (user->get_state() >= 2 && user->getAntibotKey() >= 0)
        {
            // 第二形参 int：与 CEnvironment::getProcessSequence() 返回类型同型
            // （ORIG 08637a11 `mov %eax,0x4(%esp)` 无转换中转；unsigned 形多 1 条
            // `mov %eax,%edx`，P211/P248 档③）。
            typedef void (*CheckFn)(void*, int, int, unsigned int);
            // [b28e-TimerDispatcher] 真 bug 修正：ORIG 086379e8 `add $0x14,%eax;
            // mov (%eax),%esi` = vtable[5] 本体即函数指针（单层解引用）；旧码
            // `*(CheckFn*)((*(void***)checker)[0x14/4])` 多一次解引用（我方多 1 条
            // `mov (%eax),%eax`，调用目标错）。按 PacketDispatcher.cpp 同族单层形 +
            // 去 checker 具名槽（ORIG 调用点重发 `mov 0x941f7e4,%edx`）。
            (*reinterpret_cast<CheckFn*>(
                reinterpret_cast<char*>(
                    *reinterpret_cast<void**>(GlobalData::s_antibotChecker_)) + 0x14))(
                GlobalData::s_antibotChecker_,
                G_CEnvironment()->getProcessSequence(), 0, user->get_acc_id());
        }
    }
    registNextTimer(10);
    return 1;
}

__attribute__((weak)) TimerFinishItemEffect::TimerFinishItemEffect() throw() {}
__attribute__((weak)) TimerFinishItemEffect::~TimerFinishItemEffect() throw() {}
// ---- TimerFinishItemEffect ----
// 报告：docs/class_func_reports/TimerFinishItemEffect/{dispatch_sig,_processSepecialEffectContinuousItem}.md
// 类对象仅 vptr；头声明区现缺私有 helper（见 NEEDS_LOCAL_STATIC）。

// ORIG T 0x08637aac (_ZN21TimerFinishItemEffect12dispatch_sigEiij)
// [b28d-30] 复核 DIFF：槽位差 0xc + 帧差（N82/N93 族），ORIG 0x08637aac。
// [b28d-solo-TimerDispatcher] 判词 N82/N93 封顶：P157 去 entry 缓存已修（const
// operator[] ×2 补齐，195/195）；残差 = 槽位差 0xc + 帧差。
// [SweepTimerDispatcher2 2026-09-09] P329/P0 修复：InsertTimer 的 uid/characNo 具名局部
// 改内联实参（ORIG characNo→%edi、uid→%esi 无槽）⇒ 全帧槽号回 ORIG 形、ndiff 135→25；
// 残余 = ①`!=` 物化 setne/jne vs ORIG sete/je（P196/N18b，已试否定式回滚）
// ②`return user->Send(packet)` 的 movzbl（Send 返回 bool 的 4.4 扩展）
// ③characNo/uid 的 edi/esi 角色互换（N7）。
// [TimerDispatcher-3 2026-09-10 判据①] 22 条 jcc 目标核对：i63 `je/sete`↔`jne/setne`
// 同目标（N18b 折痕，语义等价）；i147 `jmp` 目标差 9 = ORIG `mov $0x1,%esi` 中继块位置
// 差（N104 EH 尾块序）；我方多出 `jmp <next>;nop` 属 4.4 epilogue-threading 岛（N121）。
// [Main P164 回扫 2026-09-10] /tmp/td_entryjmp.py 全 TU 扫描（`jmp L`+nop 填充+L 恰为
// 填充之后）：本件 OURS hits=1 / ORIG hits=0（其余 2 件两侧一致）⇒ 确有我方多余跳板。
// 按 P164 修法试「continue 移至循环体末」形（反条件 if/else 保块序）实测跳板仍在
// （仅位移 i177→i178）、ndiff 25 不变、四数不变 ⇒ **非 P164 族**（本件无后跳 goto，
// 系 PacketGuard 析构 esi 分流后的 latch 跳板）⇒ 回滚，维持 N121 判词。
#include "TimerDispatcher/dispatch_sig_93.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 0x3);
    if (user == NULL)
        return 0;
    if (user->get_state() <= 2)          // ORIG setle: state<=2 → return 0
        return 0;
    if (p3 != 0 &&
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
        return 0;

    const unsigned int curSec =
        static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
    const std::vector<ContinuousItemInfo>* items =
        user->GetCurCharacEffectItemListR();
    if (items == NULL)
        return 0;

    // [LeaTimer28Run 2026-09-11] P248 档②·局部 unsigned 镜像视图：ORIG 0x08637b7e 段
    // `mov (%eax),%edx; mov 0x10(%ebp),%eax; cmp %eax,%edx; sete %al` 要求字段侧
    // **unsigned**（4.4 对 int 字段 + int 形参恒折单段内存直比 `cmp 0x10(%ebp),%eax`）。
    // 探针 /tmp/lea_probe2.cpp f_m 实证：`reinterpret_cast<const ContU*>(&v[i])->m_itemIdx
    // == (unsigned)p2` 逐条复现 ORIG 形、零额外指令；m_itemIdx 偏移 +0x00（头定义不变）。
    struct ContinuousItemIdxView
    {
        unsigned int m_itemIdx;     // +0x00（ContinuousItemInfo::m_itemIdx 的 unsigned 视角）
        unsigned int m_expireTime;  // +0x04
    };

    // 循环条件每次迭代重新取 size()（ORIG +0x290 seta 无符号比较）。
    // [b28d-solo-TimerDispatcher] P157：p167 差集 MISSING = const operator[] ×2
    // （ORIG 对 m_itemIdx/m_expireTime 每次重发 `(*items)[i]`，无 entry 引用缓存）。
    for (unsigned int i = 0; i < static_cast<unsigned int>(items->size()); ++i)
    {
        // [TD-2026-09-11 判据/探针] ORIG 08637b7e `mov (%eax),%edx; mov 0x10(%ebp),%eax;
        // cmp %eax,%edx; sete %al; test; je <i++ 尾部>` = `if (idx == p2) { body }`
        // **正向包裹**原形（Ghidra C 同形：`if (*piVar4 == param_2) {…}`）。旧码
        // `if (idx != p2) continue;` 产 `setne; jne` + 单段内存直比（ndiff 5 行）。
        // 探针 /tmp/td_p4.cpp：a1(`!=` 早返)⇒setne/jne；a2(`==` 正向包裹)⇒sete/je。
        if (reinterpret_cast<const ContinuousItemIdxView*>(&(*items)[i])->m_itemIdx ==
            static_cast<unsigned int>(p2))
        {
            PacketGuard packet;              // ORIG try 区 0x8637bb1..0x8637cfd
            packet.put_header(0, 0xae);
            packet.put_byte(1);
            packet.put_int(p2);
            packet.put_int(0);
            packet.finalize(true);

            if ((*items)[i].m_expireTime <= curSec)
            {
                // 已到期：立即结算并返回 Send 结果（ORIG esi=0 直通尾部返回 ebx）。
                _processSepecialEffectContinuousItem(user, p2);
                return user->Send(packet);
            }

            const unsigned int remain = (*items)[i].m_expireTime - curSec;
            if (!((remain < 6u) && (0x3cu < remain)))
            {
                // ORIG +0x1d2 重排程：OBJ_TYPE 0 / key=GetUID / msg 0x76 / delay=remain / p2 / characNo
                // [SweepTimerDispatcher2] P329/P0：ORIG 无 uid/characNo 具名槽——characNo 先求值
                // 驻 %edi、uid 驻 %esi（实参右→左求值），我方具名局部使全帧槽深 +8（-0x2c→-0x34）。
                // 改内联实参后槽序应回 ORIG 形。
                G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0,
                                            static_cast<int>(user->GetUID()),
                                            (TIMER_MESSAGE)0x76,
                                            static_cast<int>(remain),
                                            p2,
                                            static_cast<unsigned int>(user->getCurCharacNo()));
                continue;                    // esi=1 → 回到 i++ 尾部继续遍历
            }

            // ORIG +0x227 死分支（remain<6 且 >60 同时成立，恒不可达，按汇编原样保留）。
            _processSepecialEffectContinuousItem(user, p2);
            return user->Send(packet);
        }
    }
    return 1;                            // ORIG 遍历结束 ebx=1
}

// ORIG T 0x08637d64 (_ZN21TimerFinishItemEffect36_processSepecialEffectContinuousItemEP5CUserj)
// ⚠ 私有方法，TimerDispatcher.h 该类内需补声明
//   `void _processSepecialEffectContinuousItem(CUser*, unsigned int);`（NEEDS_LOCAL_STATIC）
// [b28d-30] ORIG `mov 0x10(%ebp),%eax; cmp $0x1cd1,%eax; jne` 载入+寄存器比较：
// 原码是单 case switch（P208/P251 应用）。探针 /tmp/probe30_b.cpp f6 实证
// 4.4 对 `switch(a){case K:}` 逐条复现该形；if 形恒折 mem 直比（旧 N18b 判词作废）。
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
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, 0x79);
    if (room == NULL) {
        return 0;
    }
    // ORIG 0x08638xxx：room 单槽 -0xc，matcher 每次从 room+0x6e4 直读（三次
    // `mov -0xc,%eax; mov 0x6e4(%eax),%eax`，无具名槽）；虚表槽 +0x38，
    // 实参为 p2（0x10(%ebp)）非 p3。
    if (((TimerStartMatchingMatcherSlot*)room)->m_matcher == 0)
        return 0;
    ((TimerStartMatchingMatcherSlot*)room)->m_matcher->OnMatchRoom(room, p2);
    return 1;
}

// ORIG T 0x08637e14
#include "TimerDispatcher/registNextTimer_7.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x79, (int)sec, p2, 0);
}

__attribute__((weak)) TimerGlobalEffectItemBegin::TimerGlobalEffectItemBegin() throw() {}
__attribute__((weak)) TimerGlobalEffectItemBegin::~TimerGlobalEffectItemBegin() throw() {}
#include "TimerDispatcher/dispatch_sig_95.inc"
{
    // ORIG 0x08637e56：实参为 0x10(%ebp)=p2（先前误用 p3，已修正）。
    GlobalData::s_globalEffectManager_->beginReservedEffect(p2);
    return 1;
}

__attribute__((weak)) TimerGlobalEffectItemTimeout::TimerGlobalEffectItemTimeout() throw() {}
__attribute__((weak)) TimerGlobalEffectItemTimeout::~TimerGlobalEffectItemTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_96.inc"
{
    // ORIG 0x08637ec2：getCurSec 结果直落 %ecx 跨后续求值（无名变量槽）。
    // 且第一实参为 0x10(%ebp)=p2（Ghidra 标 param_3 不准，asm 实证）。
    GlobalData::s_globalEffectManager_->checkTimeout(
        p2, GlobalData::s_systemTime_.getCurSec());
    return 1;
}

// ORIG T 0x08637ef6 _ZN28TimerGlobalEffectItemTimeout11registTimerEjj
#include "TimerDispatcher/registTimer.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x78, static_cast<int>(timeoutSec),
                                static_cast<int>(type), 0);
}

__attribute__((weak)) TimerVendingMachineItemNotify::TimerVendingMachineItemNotify() throw() {}
__attribute__((weak)) TimerVendingMachineItemNotify::~TimerVendingMachineItemNotify() throw() {}
// ORIG TimerVendingMachineItemNotify::dispatch_sig @ 0x8637f40 (0x134)
#include "TimerDispatcher/dispatch_sig_97.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    // ORIG 0x08637f40：结果驻留 %ebx；true 块 `result=1` 先于 dtor，
    // false 块 mov $0 紧跟 cmp（无提前初始化）。
    // 4.4 布局：`if (user == 0)` 写守卫（jne 跳过 inline mov$0 岛）复现 ORIG；
    // `if (user != 0)` 则 je 跳后置岛。
    register int result;
    if (user == 0)
    {
        result = 0;
    }
    else
    {
        PacketGuard packet;
        packet.put_header(0, 0x56);
        packet.put_byte(3);
        packet.put_byte(1);
        packet.put_short(user->get_unique_id() & 0xffff);   // movzwl 零扩展
        packet.put_item_idx(p2);
        packet.put_byte(0);
        packet.put_int(p3);
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        result = 1;
    }
    return result;
}

__attribute__((weak)) TimerConnectMonitorServer::TimerConnectMonitorServer() throw() {}
__attribute__((weak)) TimerConnectMonitorServer::~TimerConnectMonitorServer() throw() {}
// ===== TimerConnectMonitorServer =====
// dispatch_sig ORIG _ZN25TimerConnectMonitorServer12dispatch_sigEiij @0x086380bc (0xe3B)
// registNextTimer ORIG _ZN25TimerConnectMonitorServer15registNextTimerElii @0x086381a0 (0x41B)

#include "TimerDispatcher/dispatch_sig_98.inc"
{
    // ORIG 0x086380dc：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)（无 stream 槽）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("TimerDispatcher.cpp",
                                                          0xeaa), true);

    // ORIG 0x086380f8/0x08638113：`*guard`（deEv 0x080c8c46）+ lsEi 两级。
    (*guard)->operator<<(0x121);
    (*guard)->operator<<(-1);

    // ORIG 0x0863812e/0x08638139：`guard->`（ptcEv 0x080c8c4e）+ GetInBuffer。
    Packet_Tcp_Server_Connect* pkt = guard->GetInBuffer<Packet_Tcp_Server_Connect>();
    // ORIG 0x08638144：值源为 p2（0x10(%ebp)），具名 char 字段恢复
    // `mov %dl,0xa(%eax)` disp 直存（0x0863814c）。
    pkt->m_serverFlag = static_cast<char>(p2);

    // ORIG +0xaa: MsgQueueMgr::put(s_msgq_mgr, 1, guard)
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    // 析构由作用域自然产生（ORIG +0xba ~CStreamGuard）
    return 1;
}

// ORIG T 0x086381a0：static 注册链，与其它 registNextTimer 族同构
#include "TimerDispatcher/registNextTimer_8.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x7a, static_cast<int>(sec),
                                p2, 0);
}

__attribute__((weak)) TimerAssaultSuperPowerState::TimerAssaultSuperPowerState() throw() {}
__attribute__((weak)) TimerAssaultSuperPowerState::~TimerAssaultSuperPowerState() throw() {}
#include "TimerDispatcher/dispatch_sig_99.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 3);
    if (user == NULL) {
        return 0;
    }
    // ORIG 0x08634180：短路 && 物化 %al 双岛 merge（mov$1/mov$0 + test%al）。
    // N96/P0：绑定形已废；短路直判为 4.4 常态（4.1 merge 岛形属跨版本 bool
    // ABI 负例在册）。
    if (p3 != 0 &&
        static_cast<unsigned int>(user->getCurCharacNo()) != p3)
        return 0;
    user->SetCurCharacSuperState(false);
    pvp_assault::CAssaultPlace::SendUserState(user);
    return 1;
}

__attribute__((weak)) TimerNPCMoodChange::TimerNPCMoodChange() throw() {}
__attribute__((weak)) TimerNPCMoodChange::~TimerNPCMoodChange() throw() {}
#include "TimerDispatcher/dispatch_sig_100.inc"
{
    // ORIG 0x86381e2：空函数（纯 ret），恒返 1
    return 1;
}

// ORIG T 0x086381ec (_ZN18TimerNPCMoodChange15registNextTimerEl)：

__attribute__((weak)) TimerElevatorDungeon::TimerElevatorDungeon() throw() {}
__attribute__((weak)) TimerElevatorDungeon::~TimerElevatorDungeon() throw() {}
#include "TimerDispatcher/dispatch_sig_101.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == NULL) {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x28, p2)) {
        return 0;
    }
    // ORIG：CBattle_Field 嵌于 party+0xb24。
    reinterpret_cast<CBattle_Field*>(&party->m_battleField)
        ->check_elevator_timer();
    return 1;
}

__attribute__((weak)) TimerRefreshPowerWarProcessInfo::TimerRefreshPowerWarProcessInfo() throw() {}
__attribute__((weak)) TimerRefreshPowerWarProcessInfo::~TimerRefreshPowerWarProcessInfo() throw() {}
#include "TimerDispatcher/dispatch_sig_102.inc"
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        // ORIG 0x086382xx：G_GameWorld 结果 %eax 直落 (%esp) 实参（无槽）。
        G_GameWorld()->OnRefreshPowerWarProcessInfo();
        // dm+0xa658 = m_powerParameter.m_refreshProcessInfo(+0x30)；ORIG 按 8 字节
        // long 读取（覆盖 +0x30..+0x37 连续两 int），保持块读语义。
        // ORIG 0x086382f0：`mov 0xa658(%eax),%eax` 单级字段读（int 值语义，
        // 4B 等宽；reinterpret long* 取址形会反产 add+mov 两级）。
        registNextTimer(G_CDataManager()->m_powerParameter.m_refreshProcessInfo);
    }
    return 1;
}

// ORIG T 0x08638316 _ZN31TimerRefreshPowerWarProcessInfo15registNextTimerEl
#include "TimerDispatcher/registNextTimer_9.inc"
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, 0,
                                (TIMER_MESSAGE)0x7b, static_cast<int>(sec), 0, 0);
}

// ---- TimerGeneratePowerUp ----
// 报告：docs/class_func_reports/TimerGeneratePowerUp/{dispatch_sig,generatePowerUp,registNextTimer,sendPowerUpInfo}.md
// ctor/dtor 反汇编：ORIG 0x0863cc5a（TimerSig + 2×vector + vtable 0x8ce89e8）/
//   0x0863f156（D1）/ 0x0863f1e0（D0）。dispatcher 槽 +0x1f0（ORIG ctor 0x086321d6）。
// 数据字段（CDataManager.m_powerParameter 相对 +0xa628）：
//   m_powerUpGeneratePeriod(+0x48→dm+0xa670)、m_powerUpRatio(+0x1c→dm+0xa644)、
//   m_powerUpPointMod(+0xa8→dm+0xa6d0)。

__attribute__((weak)) TimerGeneratePowerUp::TimerGeneratePowerUp() {}
__attribute__((weak)) TimerGeneratePowerUp::~TimerGeneratePowerUp() throw() {}

// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82 族），ORIG 0x086388xx 见 report。
// [b28d-solo-TimerDispatcher] 判词 setcc-fold/N82 封顶：vector::size() 比较 ORIG
// 直跳 `cmp;jg` vs 4.4 `setl;test;je` 物化（P197④ 族）（280/279）。
// [TimerDispatcher-2 2026-09-09] 修 x87 因子结合序（ORIG 0863883f..08638856 /
// 0863886e..08638885 实证：(count*ratio)*(CP2/CP1)*(ratio+1.0)）⇒ ndiff 64→36；
// 残余 = x87 比较 lowering（fucompp/fnstsw/sahf vs fucomip/fstp，N85/P230，
// 需 -march=i486 路由，已报 Main）+ clamp 段 setl 物化（P197④；S1 已试 `<=` 形回滚）。
#include "TimerDispatcher/dispatch_sig_103.inc"
{
    // ORIG 08638860：`call IsPowerWarEventOn; test %al,%al; je +0x3f4(共享尾 mov $1)`
    // ⇒ 正向 if 包裹 + 共享尾 return 1（P222/P214①）；旧码 `if(!f()) return 1;` 产
    // xor$1 + 局部 mov$1/jmp 岛（多 3 条）。
    if (GlobalData::s_power_manager->IsPowerWarEventOn())
    {
        GlobalData::s_power_manager->SetPowerUpRemainTime(
            G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
        GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
        G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7d, 5, 0, 0);
        registNextTimer(G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
        G_GameWorld()->CancelPowerUp();
        m_team1Users.clear();
        m_team2Users.clear();

        const double ratio =
            static_cast<double>(G_CDataManager()->m_powerParameter.m_powerUpRatio) / 100.0;  // /_DAT_08ce8590(=100.0)
        const int team1Count = GlobalData::s_power_manager->GetPowerUserCount(1);
        const int team2Count = GlobalData::s_power_manager->GetPowerUserCount(2);
        const int team1ChannelPoint = GlobalData::s_power_manager->GetPowerWarPointInChannel(1);
        const int team2ChannelPoint = GlobalData::s_power_manager->GetPowerWarPointInChannel(2);
        const int team1Point = GlobalData::s_power_manager->GetPowerWarPoint((ENUM_POWER_SIDE_TYPE)1);
        const int team2Point = GlobalData::s_power_manager->GetPowerWarPoint((ENUM_POWER_SIDE_TYPE)2);
        const int mod = G_CDataManager()->m_powerParameter.m_powerUpPointMod;  // dm+0xa6d0
        if (mod == 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__,
                           0xff4, 0);
            trace("[PowerWar] PowerUpMod is 0");
            sendPowerUpInfo(0, 0);
            return 1;
        }

        double team1Ratio = static_cast<double>(team2Point - team1Point) / mod;  // local_24
        double team2Ratio = static_cast<double>(team1Point - team2Point) / mod;  // local_1c
        if (0.9 < team1Ratio)
            team1Ratio = 0.9;
        if (team1Ratio < -0.9)
            team1Ratio = -0.9;
        if (0.9 < team2Ratio)
            team2Ratio = 0.9;
        if (team2Ratio < -0.9)
            team2Ratio = -0.9;
        if (team1ChannelPoint == 0 || team2ChannelPoint == 0)
        {
            sendPowerUpInfo(0, 0);
            return 1;
        }

        // [fix] ORIG 0863883f..08638856 / 0863886e..08638885 因子序实证：
        //   fildl team2Count(-0x38) → fmull ratio(-0x48) → fildl team2CP(-0x30)
        //   → fildl team1CP(-0x34) → fdivrp → fmulp → fldl team1Ratio(-0x20) → fld1
        //   → faddp → fmulp ⇒ 原码 = (count*ratio)*(CP2/CP1)*(ratio+1.0)。
        // 浮点乘不可结合，结合序改变可观察结果 ⇒ 旧码 (ratio+1.0) 起首（Ghidra
        // 槽号错位误抄）是真语义差，按 ORIG 因子序还原。
        int team1PowerUp = static_cast<int>(team2Count * ratio *
                                            (static_cast<double>(team2ChannelPoint) / team1ChannelPoint) *
                                            (team1Ratio + 1.0));
        int team2PowerUp = static_cast<int>(team1Count * ratio *
                                            (static_cast<double>(team1ChannelPoint) / team2ChannelPoint) *
                                            (team2Ratio + 1.0));
        G_GameWorld()->GetPowerUpDomain(m_team1Users, m_team2Users);
        // [TimerDispatcher-3 判据①] 旧 if-赋值形在 4.4 恒产 `setl;test;je` 物化
        // （3 条），ORIG 是 `cmp; jg <保持臂>; [二次 size()]; jmp <汇合>;
        // 保持臂: mov x→%eax; 汇合: mov %eax→x`（clamp 直跳 + 中继 jmp + 自赋值对，
        // 探针 /tmp/td_probe4.cpp g6/g7 逐条同构）⇒ 原码为**三目赋值**。
        // 语义等价（size>x 保持 x，否则取 size()），二次 size() 调用位置亦吻合。
        team1PowerUp = (static_cast<int>(m_team1Users.size()) > team1PowerUp)
                           ? team1PowerUp
                           : static_cast<int>(m_team1Users.size());
        team2PowerUp = (static_cast<int>(m_team2Users.size()) > team2PowerUp)
                           ? team2PowerUp
                           : static_cast<int>(m_team2Users.size());
        generatePowerUp(m_team1Users, team1PowerUp);
        generatePowerUp(m_team2Users, team2PowerUp);
        sendPowerUpInfo(team1PowerUp, team2PowerUp);
    }
    return 1;
}

// ORIG T 0x08638960 (_ZN20TimerGeneratePowerUp15generatePowerUpERSt6vectorIP5CUserSaIS2_EEi)
// [b28d-30] 复核 DIFF：缺 -0x18/-0x30 槽与 0x2710 常量槽（N82/N93 族），ORIG 0x08638960。
// [b28d-solo-TimerDispatcher] 判词 N82/N93 封顶：缺 -0x18/-0x30 槽与 0x2710 常量槽
// + 帧 0x50↔0x40（89/89）。
#include "TimerDispatcher/generatePowerUp.inc"
{
    const int total = static_cast<int>(users.size());
    int selected = 0;
    int idx = 0;                 // ORIG -0x14：rand()%total 具名槽（P0 槽往返）
    CUser* picked = 0;           // ORIG -0x30：预清零（P198①）
    int maxLoop = 10000;         // ORIG -0x10：死写入照抄（P171；条件仍用立即数）
    int loopCnt = 0;
    std::vector<CUser*> buf;
    // ORIG 0x08638a0b-0x08638a4e：循环体尾 `++loopCnt; cmp $0x2710; jle <cond>`，
    // 超限落 trace 块 + break ⇒ 源码形为 while(count>selected){…;++loopCnt;
    // if(loopCnt>10000){trace;break;}}（旧 && 合并形产 mov$1/$0/test 物化 + jle/jg）。
    while (count > selected)
    {
        idx = rand() % total;
        picked = users[idx];
        // N96/P0：uchar callee 无槽折叠形（xor 寄存器物化属 setcc-fold 负例）。
        if ((unsigned char)(picked->IsPowerUp() ^ 1))
        {
            picked->SetPowerUp(true);
            buf.push_back(picked);
            ++selected;
        }
        ++loopCnt;
        if (loopCnt > 10000)
        {
            cMyTrace(__PRETTY_FUNCTION__,
                     0x104e, 5)("Generate Power Up Party Failure");
            break;
        }
    }
    G_GameWorld()->SendPowerUpList(ENUM_POWER_UP_ACTIVATE_TYPE_ACTIVATE, buf);
}

// ORIG T 0x08638aac (_ZN20TimerGeneratePowerUp15sendPowerUpInfoEii)
#include "TimerDispatcher/sendPowerUpInfo.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0xdd);
    packet.put_int(team1Count);
    packet.put_int(team2Count);
    packet.finalize(true);
    G_GameWorld()->send_to_vill(7, packet);
}

// ORIG T 0x08638b5c (_ZN20TimerGeneratePowerUp15registNextTimerEl)
#include "TimerDispatcher/registNextTimer_10.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x7c, static_cast<int>(sec), 0, 0);
}

	// ---- TimerGeneratePowerUpNotiTime ----
	// 报告：docs/class_func_reports/TimerGeneratePowerUpNotiTime/{dispatch_sig,registNextTimer,send_noti_time}.md

__attribute__((weak)) TimerGeneratePowerUpNotiTime::TimerGeneratePowerUpNotiTime() throw() {}
__attribute__((weak)) TimerGeneratePowerUpNotiTime::~TimerGeneratePowerUpNotiTime() throw() {}


// CDataManager+0xa648 为 powerup 通知时间表 std::vector<int>，即
// m_powerParameter.m_vec20（+0xa628+0x20）。
// [b28d-30] 复核 DIFF：period 槽+%ebx 双态（P248 条件②，4.4 不自行升格）⇒ N7/N82 封顶。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：比较方向/++period/period=0 已按
// P218/P171 修（79/76 不变）；残差 = period 槽 + ORIG 自行升格 %ebx 跨
// G_CDataManager() call（P248 条件②：4.4 不自行升格）。
// [TimerDispatcher-2 2026-09-09] 修：period `unsigned int`→`int`（P290 方向）⇒ 下标
// 实参升格 %ebx 跨 G_CDataManager() call，gap 3→0、ndiff 17→6（79/79）；残余
// jl↔jg 比较方向 fold（ORIG `mov period;cmp cnt;jl` vs 4.4 `mov cnt;cmp period;jg`，
// P196 折痕，非真反）记 N18b。
#include "TimerDispatcher/dispatch_sig_104.inc"
{
    // ORIG 0x0863835a：size/GetPowerUpRemainTime/GetPowerUpTimerPeriod 均由 fresh
    // G_CDataManager()/s_power_manager 直取（P48 去缓存 pm/table）。两 bool 不可合并：
    // bVar1 门内块（IsPowerWarEventOn && cnt!=0，register 态即用），bVar2（槽 -0xd）
    // 门尾 registNextTimer，仅内块 cnt<=period+1 清 0——不受事件门影响（原实现误用
    // 单 active 使事件关时漏 registNextTimer，语义 bug）。
    const int cnt =
        static_cast<int>(G_CDataManager()->m_powerParameter.m_vec20.size());
    int remain = GlobalData::s_power_manager->GetPowerUpRemainTime();
    // [b28d-30] ORIG 086383e9 `mov -0x14(%ebp),%ebx` = 具名槽 -0x14 **且** 4.1 自行
    // 升格 %ebx 跨 G_CDataManager() call；实测纯 register 删槽致 cnt/remain 整体上移
    // （P248 条件②：4.4 不自行升格）⇒ 保槽形，寄存器缓存差记 N7/N82 封顶。
    // [S1 实验] ORIG 086383e9 `mov -0x14(%ebp),%ebx` 在 G_CDataManager() call 之前
    // 装载（实参跨 receiver call 保活）；P290 方向：局部改 int 令下标处发生
    // int→size_t 转换而升格 callee-saved。改前读数 79/76 gap3 ndiff17。
    int period = GlobalData::s_power_manager->GetPowerUpTimerPeriod();
    bool bVar2 = true;
    // [b28d-30] ORIG 086383a2..086383cb：门条件只有 IsPowerWarEventOn && cnt!=0
    // （test;je + cmpl;je 双 je 同落 +0x107），`period < cnt` 是门内**第一层嵌套**
    // （+0x6b cmp;jge 跳过整块）——旧码并入 && 使门少一条 test、嵌套层丢失。
    if (GlobalData::s_power_manager->IsPowerWarEventOn() && cnt != 0)
    {
        // [S1 已试并回滚] 否定式 `!(period >= cnt)` 与正写产物全等（4.4 恒折
        // `cnt > period` 的 jg）⇒ 残余 jl↔jg 属比较方向 fold（P196），记 N18b。
        if (period < cnt)
        {
            remain -= 5;
            GlobalData::s_power_manager->SetPowerUpRemainTime(remain);
            const int notiSec =
                G_CDataManager()->m_powerParameter.m_vec20[period];
            // [b28d-solo-TimerDispatcher] ORIG 08638446 `mov -0xc(%ebp),%eax;
            // cmp -0x18(%ebp),%eax; jl` ⇒ 源形左操作数 notiSec（`notiSec >= remain`）。
            if (notiSec >= remain)
            {
                send_noti_time(notiSec);
                // ORIG 08638455 `addl $0x1,-0x14(%ebp)`（内存自增）⇒ ++period 语句。
                ++period;
                GlobalData::s_power_manager->SetPowerUpTimerPeriod(period);
                // [TD-2026-09-11] ORIG 08638439 `mov -0x14(%ebp),%eax; cmp -0x1c(%ebp),%eax;
                // jl` ⇒ 原码书写序 = `period >= cnt`（左操作数 period 留 %eax、右 cnt 内存直比，
                // 取反跳走 jl）；旧码 `cnt <= period` 产 `mov cnt; cmp period; jg`（3 行差）。
                // 探针 /tmp/td_p1.cpp loc/loc2 实证两形各有唯一产物。
                if (period >= cnt)
                {
                    // ORIG 0863846a `movl $0x0,-0x14(%ebp)`（period=0 写后未读照抄，P171）。
                    period = 0;
                    bVar2 = false;
                    GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
                }
            }
        }
    }
    if (bVar2)
        registNextTimer(5);
    return 1;
}

// ORIG T 0x0863847e (_ZN28TimerGeneratePowerUpNotiTime15registNextTimerEl)
#include "TimerDispatcher/registNextTimer_11.inc"
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x7d,
                                sec, 0, 0);
}

// ORIG T 0x086384c2 (_ZN28TimerGeneratePowerUpNotiTime14send_noti_timeEi)
#include "TimerDispatcher/send_noti_time.inc"
{
    PacketGuard packet;
    packet.put_header(0, 0xdc);
    packet.put_int(sec);
    packet.finalize(true);
    G_GameWorld()->send_to_vill(7, packet);
}

__attribute__((weak)) TimerCheckPowerWarAbuser::TimerCheckPowerWarAbuser() throw() {}
__attribute__((weak)) TimerCheckPowerWarAbuser::~TimerCheckPowerWarAbuser() throw() {}
#include "TimerDispatcher/dispatch_sig_105.inc"
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        G_GameWorld()->OnBanAbuserFromPowerWar();
        registNextTimer(0x3c);
    }
    return 1;
}

// ORIG T 0x08638294 _ZN24TimerCheckPowerWarAbuser15registNextTimerEl
#include "TimerDispatcher/registNextTimer_12.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x80, static_cast<int>(sec), 0, 0);
}

__attribute__((weak)) TimerSetPunishUser::TimerSetPunishUser() throw() {}
__attribute__((weak)) TimerSetPunishUser::~TimerSetPunishUser() throw() {}
#include "TimerDispatcher/dispatch_sig_106.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    // ORIG 0x08638ba0：独立早退（jne 岛）；比较对象 p2（0x10(%ebp)）；
    // get_unique_id unsigned short ⇒ movzwl %ax 直比，无 &0xffff。
    if (user == 0)
        return 0;
    if (user->get_unique_id() != p2)
        return 0;
    user->setHackUserFlag();
    user->SetTradePunishType(0x40);   // ORIG T _ZN5CUser17SetTradePunishTypeEi
    if (user->isARSUserKick())
    {
        user->DisConnSig(static_cast<DISCONN_SIG>(0x2e), 1, 0);   // ORIG 0x8638ba0
    }
    return 1;
}

__attribute__((weak)) TimerSetTradePunishUser::TimerSetTradePunishUser() throw() {}
__attribute__((weak)) TimerSetTradePunishUser::~TimerSetTradePunishUser() throw() {}
#include "TimerDispatcher/dispatch_sig_107.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    // ORIG 0x08638c48：两段独立早退；比较对象是 p2（0x10(%ebp)）非 p3；
    // get_unique_id 返回 unsigned short ⇒ movzwl %ax 后直比，无 &0xffff。
    if (user == 0)
        return 0;
    if (user->get_unique_id() != p2)
        return 0;
    user->SetTradePunishType(4);
    return 1;
}

__attribute__((weak)) TimerRevivalCharac::TimerRevivalCharac() throw() {}
__attribute__((weak)) TimerRevivalCharac::~TimerRevivalCharac() throw() {}
#include "TimerDispatcher/dispatch_sig_108.inc"
{
    // ORIG 0x08638e78：单出口 result(%ebx) 链，三段 if/else-if；
    // 0x08638ed8 GetRevivalTime 比较对象为 p2（0x10(%ebp)），非 p3；
    // PacketGuard 于 else 块内构造+dtor（0x08638f39 call D1）后 result=1。
    // 技巧2：result 驻 %ebx（ORIG 0x08638ea5 `mov $0x0,%ebx`×3 / 0x08638f61
    // `mov %ebx,%eax` 单出口），PacketGuard 回 -0x18、帧 0x20。
    register int result;   // [b28d-30] 纯 register 存储建议（P231/P248：值跨 call 保活、4.4 不自升格、ORIG 目标 %ebx）
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    if (user == 0) {
        result = 0;
    } else if (user->getCurCharacR() == 0) {
        result = 0;
    } else if (user->GetRevivalTime() != p2) {
        result = 0;
    } else {
        // ORIG 0x08638f34：result=1 在 PacketGuard dtor(0x08638f39) 之前（else 块内）。
        PacketGuard packet;
        packet.put_header(0, 0xe4);
        packet.finalize(true);
        user->Send(packet);
        result = 1;
    }
    return result;
}

__attribute__((weak)) TimerRequestExit::TimerRequestExit() throw() {}
__attribute__((weak)) TimerRequestExit::~TimerRequestExit() throw() {}
// [b28d-30] 复核 DIFF：同 _NS_NonClient_CheckResponse——N18b 同型折 mem；异型化旋钮跨泳道。
// [b28d-solo-TimerDispatcher] 已 AE：Main 代落 getCurCharacNo→unsigned int 后两段
// 物化对齐（42/42）。
#include "TimerDispatcher/dispatch_sig_109.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    // [b28d-30] 非成员比较对象核对：本函数为成员，this@0x8 ⇒ 0x10(%ebp)=p2
    // （旧注释正确，b28d-30 一度误改 p3/0x14 已回滚）。ORIG `mov 0x10(%ebp),%edx;
    // cmp %edx,%eax` 两段物化 = callee 返回 unsigned 与 int 形参异型（P248 档①，
    // 探针 /tmp/probe30_e.cpp d5/d6 实证）。
    // ORIG 0x08638f6a：三段独立早退；characR 判空 sete 物化（c1 探针形）；
    // characNo 比较对象 p2（0x10(%ebp)），混型走 mov edx;cmp 形态（c3 探针形）。
    if (user == 0)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;
    // ORIG 0x08638f3b：`mov 0x10(%ebp),%edx; cmp %edx,%eax`（call 结果 vs arg 两步形）。
    if (user->getCurCharacNo() != p2)
        return 0;
    user->DisConnSig(static_cast<DISCONN_SIG>(0x2f), 1, 0);   // ORIG 0x8638f6a
    return 1;
}

__attribute__((weak)) TimerVillageMonsterUpdateRate::TimerVillageMonsterUpdateRate() throw() {}
__attribute__((weak)) TimerVillageMonsterUpdateRate::~TimerVillageMonsterUpdateRate() throw() {}
#include "TimerDispatcher/dispatch_sig_110.inc"
{
    // OnRun/OnRegen/OnStart 为非静态成员（CVillageMonsterMgr.h），经全局管理器实例调用。
    GlobalData::s_villageMonsterMgr->OnRunVillageMonster();
    return 1;
}

__attribute__((weak)) TimerVillageMonsterRegen::TimerVillageMonsterRegen() throw() {}
__attribute__((weak)) TimerVillageMonsterRegen::~TimerVillageMonsterRegen() throw() {}
#include "TimerDispatcher/dispatch_sig_111.inc"
{
    GlobalData::s_villageMonsterMgr->OnRegenVillageMonster(p2,
                                                           static_cast<int>(p3));
    return 1;
}

// ORIG 0x8639042：村战开启定时器（报告 TimerStartVillageAttacked/dispatch_sig.md）。
#include "TimerDispatcher/dispatch_sig_112.inc"
                                            unsigned int /*p3*/)
{
    GlobalData::s_villageMonsterMgr->OnStartVillageAttacked();
    return 1;
}

__attribute__((weak)) TimerVillageAttackedPlayTimeout::TimerVillageAttackedPlayTimeout() throw() {}
__attribute__((weak)) TimerVillageAttackedPlayTimeout::~TimerVillageAttackedPlayTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_113.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == NULL) {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)0x27, p2)) {
        return 0;
    }
    // ORIG 0x086391a3：`mov -0xc,%eax; movzwl 0xc4c(%eax),%eax; movzwl %ax,%eax;
    // cmp 0x14(%ebp),%eax` 单级折叠（view 嵌套成员链式，无 field 具名槽）。
    if (static_cast<unsigned short>(
            reinterpret_cast<CPartyBattleFieldView*>(party)
                ->m_battleField.m_villageMonsterId) != p3) {
        return 0;
    }
    party->OnFinishVillageMonsterFighting((CUser*)0, true);
    return 1;
}

__attribute__((weak)) TimerItemUnlockWaiting::TimerItemUnlockWaiting() throw() {}
__attribute__((weak)) TimerItemUnlockWaiting::~TimerItemUnlockWaiting() throw() {}
#include "TimerDispatcher/dispatch_sig_114.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 2);
    if (user == NULL) {
        return 0;
    }
    if (user->getCurCharacR() == NULL) {
        return 0;
    }
    if ((unsigned int)user->getCurCharacNo() != p3) {
        return 0;
    }
    user->OnItemUnlockWaitTimeout(p2, p3);
    return 1;
}

// ===== TimerUpdateCompoundEmblemStatics =====
__attribute__((weak)) TimerUpdateCompoundEmblemStatics::TimerUpdateCompoundEmblemStatics() throw() {}
__attribute__((weak)) TimerUpdateCompoundEmblemStatics::~TimerUpdateCompoundEmblemStatics() throw() {}
// [b28d-30] 复核 DIFF：push edi/esi/ebx 缓存组跨版本差（N7/N82 族），ORIG 0x08638xxx。
// [b28d-solo-TimerDispatcher] 判词 N7/N82附 封顶：getGradeCount 结果 ORIG 驻
// edi/ebx vs 4.4 槽化（-0x18/-0x14/-0x10/-0xc）+ 帧 0x6c↔leave（130/130）。
// [TimerDispatcher-2 2026-09-09 复核] gap=1 全为 callee-saved 角色：ORIG grade2/1/0
// 驻 edi/esi/ebx（push edi/esi/ebx + sub $0x6c）vs 4.4 全槽化（sub $0x88）⇒ N7/N82附；
// 裸 register 已试并回滚（130→133、帧 0x6c→0x7c）⇒ 封顶。
// [SweepTimerDispatcher2 2026-09-09] P329/P0 复核：ORIG 14 个 getGradeCount 结果
// **无任何具名局部**（前 4 个 spill 临时槽 -0x48/-0x44/-0x40/-0x3c，后 3 个驻
// edi/esi/ebx；makeRequest 侧 dbGrade6 驻 %esi、dbGrade1/0 驻 %edi/%ebx）⇒ 原码
// 全部内联到实参位（右→左求值序）。改为内联表达式后复验。
#include "TimerDispatcher/dispatch_sig_115.inc"
{
    if (getCompoundEmblemStatistic()->getTotalCount() != 0)
    {
        // [P227/P221] ORIG 在 cMyTrace ctor 之前右→左求值 7 个 getGradeCount
        // （前 4 个 spill -0x48/-0x44/-0x40/-0x3c，后 3 个驻 edi/esi/ebx）
        // ⇒ 必须写全表达式临时链（两语句形会把 ctor 提到求值之前）。
        cMyTrace(__PRETTY_FUNCTION__, 0x11aa, 0)(
            "DNF Compound Emblem Statistics (grade 0 : %d)(grade 1 : %d)(grade 2 : %d)(grade 3 : %d)(grade 4 : %d)(grade 5 : %d)(grade 6 : %d)",
            getCompoundEmblemStatistic()->getGradeCount(0),
            getCompoundEmblemStatistic()->getGradeCount(1),
            getCompoundEmblemStatistic()->getGradeCount(2),
            getCompoundEmblemStatistic()->getGradeCount(3),
            getCompoundEmblemStatistic()->getGradeCount(4),
            getCompoundEmblemStatistic()->getGradeCount(5),
            getCompoundEmblemStatistic()->getGradeCount(6));
        DB_UpdateCompoundEmblemStatics::makeRequest(
            // §4.3 具名化：+0x1b0 = m_serverEnvir(+0x198).m_channelNo(+0x18)
            G_CEnvironment()->m_serverEnvir.m_channelNo,
            getCompoundEmblemStatistic()->getGradeCount(0),
            getCompoundEmblemStatistic()->getGradeCount(1),
            getCompoundEmblemStatistic()->getGradeCount(2),
            getCompoundEmblemStatistic()->getGradeCount(3),
            getCompoundEmblemStatistic()->getGradeCount(4),
            getCompoundEmblemStatistic()->getGradeCount(5),
            getCompoundEmblemStatistic()->getGradeCount(6));
    }
    registNextTimer();
    getCompoundEmblemStatistic()->clear();
    return 1;
}

// ORIG T _ZN32TimerUpdateCompoundEmblemStatics15registNextTimerEv @0x0863939a
// [b28d-30] 复核 DIFF：localtime_r 返回指针视图/槽差（P43/N82 族），ORIG 0x0863939a。
// [b28d-solo-TimerDispatcher] 判词 N1 封顶：74/73。修法：①P43 指针访问已修；
// ②§4.3 具名化 `*(const int*)((const char*)G_CEnvironment() + 0x1b0)` →
// `G_CEnvironment()->m_serverEnvir.m_channelNo`（CEnvironment.h:132，ORIG
// `mov 0x1b0(%eax),%ecx` 单条 disp 已匹配）。残差 = `%0x1e` 常量除法展开
// lea+shl+sub vs add+shl+sub（源形不可择）⇒ N1。
// [TimerDispatcher-2 2026-09-09 复核] 同 TimerDungeonDataStatistic::RegistNextTimer：
// N86b/N1（ORIG-only lea/mov vs OURS-only add 各 1）⇒ 封顶，无语义缺口。
// [TimerDispatcher-3 2026-09-10 判据①] 同族：唯一 jg 目标差 1 = lea↔add 单条
// （探针 r1..r6 六形恒 add），分支语义等价。
#include "TimerDispatcher/registNextTimer_13.inc"
{
    // [TimerDispatcher-4 2026-09-11] 同 TimerDungeonDataStatistic：唯一真残差 =
    // `%0x1e` 余数段 lea↔add（N86b/N1，22 形探针封底）；p167：6=6 CLEAR。
    const time_t now = static_cast<time_t>(GlobalData::s_systemTime_.getCurSec());
    tm t;
    // [b28d-solo-TimerDispatcher] P43：ORIG 经 localtime_r 返回值指针访问
    // （`mov -0x18(%ebp),%eax; mov 0x8(%eax),%eax` / mktime(p)），非直接读栈上 tm。
    tm* p = localtime_r(&now, &t);
    const int curHour = p->tm_hour;
    p->tm_hour = 5;
    p->tm_min = 0;
    p->tm_sec = 0;
    // §4.3 具名化：+0x1b0 = CEnvironment::m_serverEnvir(+0x198).m_channelNo(+0x18)
    // （CEnvironment.h:132 权威声明；ORIG `mov 0x1b0(%eax),%ecx` 单条 disp）。
    p->tm_min = G_CEnvironment()->m_serverEnvir.m_channelNo % 0x1e;
    int target = static_cast<int>(mktime(p));
    if (p->tm_hour <= curHour)
        target += 0x15180;
    const int delay = target - static_cast<int>(now);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x89,
                                delay, 0, 0);
    return 1;
}

__attribute__((weak)) TimerStartExpEventAfterVillageAttack::TimerStartExpEventAfterVillageAttack() throw() {}
__attribute__((weak)) TimerStartExpEventAfterVillageAttack::~TimerStartExpEventAfterVillageAttack() throw() {}
#include "TimerDispatcher/dispatch_sig_116.inc"
{
    // TriggerEventStart 为 CEventManager 成员；Word_Param 低 ushort=100（ORIG movw $0x64）。
    Word_Param param;
    param.m_duration = 100;
    GlobalData::s_event_manager->TriggerEventStart(3, param);
    return 1;
}

__attribute__((weak)) Timer_Security_NonClient_CheckResponse::Timer_Security_NonClient_CheckResponse() throw() {}
__attribute__((weak)) Timer_Security_NonClient_CheckResponse::~Timer_Security_NonClient_CheckResponse() throw() {}
#include "TimerDispatcher/dispatch_sig_117.inc"
{
    // ORIG 0x86394f0：call _NS_NonClient_CheckResponse(p1,p2,p3) 后 leave/ret（eax 未定）。
    _NS_NonClient_CheckResponse(p1, p2, static_cast<int>(p3));
}

__attribute__((weak)) Timer_Midnight::Timer_Midnight() throw() {}
__attribute__((weak)) Timer_Midnight::~Timer_Midnight() throw() {}
#include "TimerDispatcher/dispatch_sig_118.inc"
{
    G_GameWorld()->ResetMidnight();
    GlobalData::s_onlinePreliminaryTeamMgr->ResetDailyMidnight();
    const bool registered = RegistNextTimer();
    if (!registered) {
        return 0;
    }
    return 1;
}

// ORIG T 0x08639554 _ZN14Timer_Midnight15RegistNextTimerEv
// [b28d-30] 复核 DIFF：push edi/esi/ebx 缓存组/帧差（N7/N82 族），ORIG 0x08639554。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：push edi/esi/ebx 缓存组 + 槽差
// （-0x3c↔-0x2c 等）（71/72）。
#include "TimerDispatcher/RegistNextTimer_10.inc"
{
    // ORIG 0x08639554：curSec time_t 单槽 -0x3c（免 int→long 拷贝）；pt 槽 -0x28
    // 各点 reload；hourNow 槽 -0x24；sec/min/hour 直落 edi/esi/ebx（绑定，N18a
    // 两段残差）跨 cMyTrace ctor 存活
    time_t nowSec = GlobalData::s_systemTime_.getCurSec();
    std::tm timePoint;
    std::tm* pt = localtime_r(&nowSec, &timePoint);
    const int hourNow = pt->tm_hour;
    // [b28d-solo] P231/P307 纯 register（非 asm 绑定）：ORIG 三值直落 edi/esi/ebx
    // 跨 cMyTrace ctor 保活无栈槽（086395xx mov (%eax),%edi / 0x4,%esi / 0x8,%ebx）。
    register int secNow = pt->tm_sec;
    register int minNow = pt->tm_min;
    register int hrNow = pt->tm_hour;
    cMyTrace(__PRETTY_FUNCTION__, 0x1293, 0)(
        "Reset all user's fatigue!(%02d:%02d:%02d)", hrNow, minNow, secNow);
    pt->tm_hour = 0;
    pt->tm_min = 0;
    pt->tm_sec = 0;
    // ORIG 0863964x：`call mktime; mov %eax,-0x20; addl $0x15180,-0x20`（单槽原地自增，
    // 无独立 midnight 局部）⇒ 具名 nextAt + 复合赋值（旧双局部多 1 槽 + 2 insn）。
    int nextAt = static_cast<int>(mktime(pt));
    nextAt += 0x15180;
    const int delay = nextAt - static_cast<int>(nowSec);
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x8d, delay, 0, 0);
    return true;
}

__attribute__((weak)) TimerEndPowerWarVictoriousReward::TimerEndPowerWarVictoriousReward() throw() {}
__attribute__((weak)) TimerEndPowerWarVictoriousReward::~TimerEndPowerWarVictoriousReward() throw() {}
#include "TimerDispatcher/dispatch_sig_119.inc"
{
    GlobalData::s_event_manager->TriggerEventEnd(0x35);
    return 1;
}

__attribute__((weak)) Timer_TenMinuteSchedule::Timer_TenMinuteSchedule() throw() {}
__attribute__((weak)) Timer_TenMinuteSchedule::~Timer_TenMinuteSchedule() throw() {}
#include "TimerDispatcher/dispatch_sig_120.inc"
{
    // ORIG 0x8639668：检查第二参数（0x10(%ebp)）：==0 → InsertValueStatistic +
    // RegistNextTimer(1)；==1 → InsertCirculationStatistic + RegistNextTimer(0)；
    // 其他值两分支都不执行。ORIG 形：bool ok=false（movb $0,-0x9）先行初始化，
    // 分支内 ok = RegistNextTimer(n)（mov %al,-0x9），尾部 reload+xor$1 物化返回。
    bool ok = false;
    if (p2 == 0)
    {
        GetInstanceValueStatistic()->InsertValueStatistic();
        ok = RegistNextTimer(1);
    }
    else if (p2 == 1)
    {
        GetInstanceCirculationStatistic()->InsertCirculationStatistic();
        ok = RegistNextTimer(0);
    }
    if (!ok)
        return 0;
    return 1;
}

__attribute__((weak)) Timer_UpdateOnlinePreliminaryTeam::Timer_UpdateOnlinePreliminaryTeam() throw() {}
__attribute__((weak)) Timer_UpdateOnlinePreliminaryTeam::~Timer_UpdateOnlinePreliminaryTeam() throw() {}
#include "TimerDispatcher/dispatch_sig_121.inc"
{
    // ORIG 0x08639700：FindTeam/RegistNextTimer 均用 p2（0x10(%ebp)），非 p3。
    (void)p1;
    (void)p3;
    online_preliminary::COnlinePreliminaryTeam* team =
        GlobalData::s_onlinePreliminaryTeamMgr->FindTeam(p2);
    // ORIG 0x08639722：team 判空 je +0x4e；registered 为 bool 局部（槽 -0x9）
    // 经 `movzbl; xor $1; test; je +0x55` 判定；两块 `mov $0x0,%eax` 岛 +
    // `mov $0x1,%eax` 尾岛。布局钥匙（P222 正向包裹）：正极性 if(team!=0) 包
    // body、return 0 归共享尾（je 跳过 body）；`if (registered) return 1;` 后
    // fall-through 到 return 0。goto 标签串形被 4.4 逐标签加 nop（+3 insn）。
    if (team != 0)
    {
        team->saveData();
        const bool registered = RegistNextTimer(p2);
        if (!registered)
            return 0;
    }
    else
    {
        return 0;
    }
    return 1;
}

// ORIG T 0x0863977e _ZN33Timer_UpdateOnlinePreliminaryTeam15RegistNextTimerEi
#include "TimerDispatcher/RegistNextTimer_11.inc"
{
    // ORIG 0x0863977e：movl $0x258,-0xc(%ebp) 具名槽（帧 0x38），0x10(%esp) 二级装载。
    int nSec = 0x258;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x8f, nSec, team, 0);
    return true;
}

__attribute__((weak)) Timer_SecretShopStatistic::Timer_SecretShopStatistic() throw() {}
__attribute__((weak)) Timer_SecretShopStatistic::~Timer_SecretShopStatistic() throw() {}
#include "TimerDispatcher/dispatch_sig_122.inc"
{
    // ORIG 0x86397ce: secretshop::CSecretShop::SendSecretShopStatistic(s_secret_shop); RegistNextTimer(); return 1
    GlobalData::s_secret_shop->SendSecretShopStatistic();
    RegistNextTimer();
    return 1;
}

// ORIG T 0x086397ee (_ZN25Timer_SecretShopStatistic15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x90, 0x3c, 0, 0)。
#include "TimerDispatcher/RegistNextTimer_12.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x90, 0x3c, 0, 0);
}

__attribute__((weak)) Timer_DailySchedule::Timer_DailySchedule() throw() {}
__attribute__((weak)) Timer_DailySchedule::~Timer_DailySchedule() throw() {}

__attribute__((weak)) Timer_ConnectP2PPvPTimeout::Timer_ConnectP2PPvPTimeout() throw() {}
__attribute__((weak)) Timer_ConnectP2PPvPTimeout::~Timer_ConnectP2PPvPTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_123.inc"
{
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, 0x3b);
    if (room == NULL) {
        return 0;
    }
    if (!room->CheckTimerKey((TIMER_MESSAGE)0x3b, p2)) {
        return 0;
    }
    room->OnConnectP2PPvPTimeout();
    return 1;
}

__attribute__((weak)) Timer_RoutingItemWait::Timer_RoutingItemWait() throw() {}
__attribute__((weak)) Timer_RoutingItemWait::~Timer_RoutingItemWait() throw() {}
#include "TimerDispatcher/dispatch_sig_124.inc"
{
    // ORIG 0x086398a8：null 早退（jne 岛）+ `!check` 早退（xor$1;test;je，
    // 0x086398c9）；mgr 具名槽 -0xc（party=-0x10 后使用，0x086398d8 mov %eax,-0xc）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2b, p2))
        return 0;
    CSpecialItemRoutingManager* mgr = G_CGameManager()->GetSpecialItemRoutingManager();
    mgr->ProcessTimeOut(party);
    // GetSpecialItemRoutingManager 返回值域：CGameManager.h 仅前向，见 TimerBatch.md
    return 1;
}

__attribute__((weak)) Timer_PcRoomAuthRetry::Timer_PcRoomAuthRetry() throw() {}
__attribute__((weak)) Timer_PcRoomAuthRetry::~Timer_PcRoomAuthRetry() throw() {}
// ===== Timer_PcRoomAuthRetry =====
// dispatch_sig ORIG _ZN21Timer_PcRoomAuthRetry12dispatch_sigEiij @0x08639924 (0xacB)
// registNextTimer ORIG _ZN21Timer_PcRoomAuthRetry15registNextTimerEliij @0x086399d0 (0x41B)

#include "TimerDispatcher/dispatch_sig_125.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 2);
    if (user == 0)
    {
        return 0;
    }
    // ORIG +0x38/+0x40: get_unique_id() 低 16 位与 p2 比较
    if ((unsigned short)user->get_unique_id() != p2)
    {
        return 0;
    }
    // ORIG +0x57: getCurCharacR() 判空
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    // ORIG +0x72: getCurCharacNo() 与 p3 比较
    if (user->getCurCharacNo() != p3)
    {
        return 0;
    }
    // ORIG +0x8e..+0xa5: !getPcRoomAuth()（bool 声明 ⇒ xor $0x1,%eax 形态）
    if (!user->getPcRoomAuth())
    {
        user->SendLoginToPCRoomServer();
    }
    return 1;
}

// ORIG T 0x086399d0：static，四参全转发（无 this 使用）
#include "TimerDispatcher/registNextTimer_14.inc"
                                            unsigned int p3)
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x92, static_cast<int>(sec),
                                p2, p3);
}

__attribute__((weak)) Timer_DungeonInoutOpenTime::Timer_DungeonInoutOpenTime() throw() {}
__attribute__((weak)) Timer_DungeonInoutOpenTime::~Timer_DungeonInoutOpenTime() throw() {}

// [b28d-solo-TimerDispatcher] P157：ORIG 每个字段都重新 `it->ptEv()`（无
// `it->second` 引用缓存），旧码 `const STDungeonInoutTime& w = it->second;`
// 引入指针槽 -0xc 且只调一次 ptEv ⇒ 改回逐字段 `it->second.m_xxx`。
// [b28d-solo-TimerDispatcher2] P167 差集（MISSING G_CDataManager/ne/ptEv、EXTRA eq）
// 判明原码形：①时间读取（getCurSec/localtime_r/hour/min）在 find **之前**；
// ②find 与 end 各自重发 `G_CDataManager()`+`lea 0x130`（无 map 引用局部缓存，P157/P183）；
// ③正极性 `if (it != end) { if (cond) return true; } return false;`（P220，ORIG ne）。
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

// Timer_DungeonInoutOpenTime::dispatch_sig ORIG T 0x8639e0a (0x3c5)
// [b28d-30] 复核 DIFF：全帧槽号平移（N82 族），ORIG 0x08639e0a。
// [b28d-solo-TimerDispatcher] 判词 N82 封顶：全帧槽号平移（233/231，调用点全等）。
// [TimerDispatcher-2 2026-09-09] 修 2 处：①nextOpenT 拆两句式（ORIG 08639f1a
// `addl $0x15180,-0x1c(%ebp)` 内存自增）②"DELTA..." cMyTrace 改全表达式临时链
// （P227 ⇒ curT 升格 %ebx 跨 ctor）⇒ ndiff 185→179；残余 = 全帧槽平移（ORIG 标量区
// -0x78..-0x14 vs 4.4 -0x30..-0x10，帧同为 0xd4，N82/N95）+ dungeonIdx 实参升格序 N7。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；残差为 tm/map 对象区与标量区
// 的槽排布（非恒定平移：tm -0xac↔-0xb0、map -0x78↔-0x3c）⇒ N82/N95，无语义缺口。
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
// ORIG Timer_DungeonInoutCloseTime::dispatch_sig @ 0x863a1d0 (0x25f)
// [b28d-30] 复核 DIFF：帧差 0x10（0xa4 vs 0xb4，N82/P210 族），ORIG 0x0863a1d0。
// [b28d-solo-TimerDispatcher2] P167 差集 MISSING G_CDataManager×1 ⇒ ORIG find/end
// **各自重发** `G_CDataManager()`+`lea 0x130`（无 dataMgr 局部缓存，P157/P183）；
// P0：ORIG 0863a213 `mov 0x14(%ebp),%eax; mov %eax,-0x58(%ebp)` 全程读 -0x58
// ⇒ 原码有 p3 的具名副本（dungeonIdx），旧码形参直读。
    // [TimerDispatcher-3 2026-09-10 判据①] 9 条 jcc 目标逐条对应（jns 位移 1 由单条指令差
    // 引起）；唯一多重集差 = ORIG `mov -0x58,%eax;test %eax,%eax` vs 我方
    // `cmpl $0x0,-0x58`（/tmp/td_probe_cmp.cpp 六形 + /tmp/td_probe2.cpp 八形恒 cmpl）
    // ⇒ 旧判词 N18b/N22 封顶。
    // [A12-2026-09-10 · C05 ② **命中**（旧 N18b 判词作废）] 该具名局部
    // `unsigned int dungeonIdx` → **int**（S1 一次性旋钮）后，4.4 即由 mem 直比
    // `cmpl $0x0,-0x58` 翻为 ORIG 的载寄存器两段形 `mov -0x58(%ebp),%eax; test`：
    // 本件 **DIFF→NEAR**，TU 四数 diff 13→12 / near 4→5（identical 60 / ae 700 不变）。
    // 边界留痕：此处比较是**零比较（test 形）**而非 `cmp $imm`，与 Main 汇总的
    // 「常量 vs 局部不适用」边界不同 ⇒ 供边界更新参考（cmp $imm 子型仍归 N18b）。
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
{
    // [b26N7] 尾部虚调用 callee 寄存器 ORIG vfn→%ecx/参数 eax,edx vs 4.4 恒
    // vfn→%eax（N7 探针实证），ndiff 12 封顶。
    PvP_Room* room = G_CGameManager()->GetPvp(p1, (CUser*)0, (int)0x3e);
    if (room == 0)
    {
        return 0;
    }
    if (room->get_timer_key((TIMER_MESSAGE)0x3e) != p2)
    {
        return 0;
    }
    // ORIG +0x61: matcher = *(void**)(room + 0x6e4)（匹配系统指针，ORIG 布局该槽为带虚表对象）
    // ORIG 0x0863cxxx：room 单槽 -0xc，matcher 三次从 +0x6e4 直读（view 链式，
    // 无具名槽）；虚表 +0x64 = IMatch::OnRematchTimeout(room)。
    // ORIG 0x0863a499：正向 `if (matcher != 0){call}`（je 863a4c0 跳 mov$1，
    // null 与 call 后都汇聚 return 1）。
    if (((TimerStartMatchingMatcherSlot*)room)->m_matcher != 0)
        ((TimerStartMatchingMatcherSlot*)room)->m_matcher->OnRematchTimeout(room);
    return 1;
}

__attribute__((weak)) Timer_ReturnUserTimeout::Timer_ReturnUserTimeout() throw() {}
__attribute__((weak)) Timer_ReturnUserTimeout::~Timer_ReturnUserTimeout() throw() {}
// ORIG Timer_ReturnUserTimeout::dispatch_sig @ 0x863a4c8 (0x23e)
#include "TimerDispatcher/dispatch_sig_129.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 2);
    if (user == 0)
        return 0;

    if ((unsigned short)user->get_unique_id() != p2)
        return 0;

    user->ResetReturnUser();

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x12d);
    packet.put_int(0);
    packet.finalize(true);
    user->Send(packet);

    packet.clear();
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
    packet.finalize(true);

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (user->get_state() == 5)
    {
        // ORIG 0x0863a631：party!=0 走 fall-through（send_to_party），==0 je 跳尾部 Send 岛。
        if (party != 0)
            party->send_to_party(packet);
        else
            user->Send(packet);
    }
    else
    {
        G_GameWorld()->send_all(packet, user);
    }

    if (party != 0)
    {
        if (!(party->IsReturnUserParty() && party->IsEventCharacParty()))
        {
            G_GameWorld()->send_party_info_to_all(party, 1);
        }
    }
    return 1;
}

// ORIG _ZN23Timer_ReturnUserTimeout15registNextTimerEiij @0x0863a706：
//   now = CSystemTime::getCurSec(&GlobalData::s_systemTime_);
//   G_TimerQueue()->InsertTimer(OBJ_TYPE_2, p1, (TIMER_MESSAGE)0x95,
//                               (int)(p3 - now), p2, 0)
#include "TimerDispatcher/registNextTimer_15.inc"
{
    // [b28d-30] ORIG 0x0863ae10：getCurSec 返回值无槽往返（call 后直接
    // `mov 0x10(%ebp),%edx; mov %edx,%ecx; sub %eax,%ecx` 三步链，delta 落具名槽）
    // ⇒ P0 表达式直用：删 cur 槽、无显式 cast（探针 /tmp/probe30_c.cpp r1
    // 逐条复现 ORIG；旧「表达式形反劣」判词源于带 cast 的 r3 形，作废）。
    int delta = p3 - GlobalData::s_systemTime_.getCurSec();
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, p1,
                                (TIMER_MESSAGE)0x95, delta,
                                p2, 0);
}

__attribute__((weak)) Timer_HadesHeartBeat::Timer_HadesHeartBeat() throw() {}
__attribute__((weak)) Timer_HadesHeartBeat::~Timer_HadesHeartBeat() throw() {}
#include "TimerDispatcher/dispatch_sig_130.inc"
{
    if (GlobalData::s_hades_proxy->GetSocketState() == 0) {
        // ORIG 0x0863a760：channelNo 直落 %ebx（push %ebx 帧）、udp_port 参数位
        // movzwl %ax 免槽（探针 x7 实证 register + 实参内联 cast 形态）。
        register int channelNo = G_CEnvironment()->get_channel_no();
        GlobalData::s_hades_proxy->SendHeartBeatPacket(
            (unsigned short)G_CEnvironment()->get_udp_port(), channelNo);
        registNextTimer();
    }
    return 1;
}

// ORIG T 0x0863a7c2 _ZN20Timer_HadesHeartBeat15registNextTimerEv
#include "TimerDispatcher/registNextTimer_16.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x96, 0x3c, 0, 0);
}

__attribute__((weak)) Timer_TOD_Send_ClearReward::Timer_TOD_Send_ClearReward() throw() {}
__attribute__((weak)) Timer_TOD_Send_ClearReward::~Timer_TOD_Send_ClearReward() throw() {}
#include "TimerDispatcher/dispatch_sig_131.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == NULL) {
        return 0;
    }
    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2)) {
        return 0;
    }
    if (!party->OnTODRewardTimer((TIMER_MESSAGE)p3)) {
        return 0;
    }
    return 1;
}

__attribute__((weak)) Timer_OneADayItemShop::Timer_OneADayItemShop() throw() {}
__attribute__((weak)) Timer_OneADayItemShop::~Timer_OneADayItemShop() throw() {}
#include "TimerDispatcher/dispatch_sig_132.inc"
{
    // ORIG 0x863a916: G_CDataManager()->m_itemShop->updateOneADayItemList(); RegistNextTimer(); return 1
    G_CDataManager()->m_itemShop->updateOneADayItemList();   // m_itemShop @+0x8cbc
    RegistNextTimer();
    return 1;
}

__attribute__((weak)) Timer_Schedule_MidNight::Timer_Schedule_MidNight() throw() {}
__attribute__((weak)) Timer_Schedule_MidNight::~Timer_Schedule_MidNight() throw() {}
#include "TimerDispatcher/dispatch_sig_133.inc"
{
    // ORIG 0x863a982: g_GoldControlDateFlag++; RegistNextTimer(); return 1
    ++GlobalData::g_GoldControlDateFlag;
    RegistNextTimer();
    return 1;
}

__attribute__((weak)) Timer_Send_Ontime_Reward::Timer_Send_Ontime_Reward() throw() {}
__attribute__((weak)) Timer_Send_Ontime_Reward::~Timer_Send_Ontime_Reward() throw() {}
// ===== Timer_Send_Ontime_Reward =====
// dispatch_sig ORIG _ZN24Timer_Send_Ontime_Reward12dispatch_sigEiij @0x0863aa90 (0xbcB)
// RegistNextTimer ORIG _ZN24Timer_Send_Ontime_Reward15RegistNextTimerEv @0x0863ab4c (0x45B)

// [b28d-30] 复核 DIFF：极性/短路已修（真 bug 修正）；残余 `jne then; jmp end` 块排布差
// 为 N93附/P223（4.4 恒产 je 跳过镜像），ORIG 0x0863aa90。
// [b28d-solo-TimerDispatcher] 判词 N93附/P223 封顶：`jne then; jmp skip` 块序（4.4 恒
// `je skip` 镜像）；P272 空 then+else 试形反产 sete（已回退）（52/51）。
#include "TimerDispatcher/dispatch_sig_134.inc"
{
    COnTimeEvent* ev =
        (COnTimeEvent*)GlobalData::s_event_manager->GetRepeatEvent(0x33);
    // [b28d-30] ORIG 0863aaae..0863ab03：正极性短路 `ev != 0 && IsEventing(0)`
    // （双 je 同落 +0x49 的 mov $0 物化岛）⇒ if/else 两臂形；旧码 `ev==0 ||
    // IsEventing==false` 负极性 + bool ok 槽物化（多 xor/槽段，指令 59 vs 52）。
    if (ev != 0 && ev->IsEventing(0))
    {
        // ORIG +0x58: process_send_reward_to_user()
        ev->process_send_reward_to_user();
        // ORIG +0x63: get_ontime_reward_user_count()
        // [TimerDispatcher-3 判据①] ORIG 0863ab15 `jne [尾 body]` + 0863ab17
        // `jmp [尾 return 1]`：真分支跳**函数尾部** body（else 块之后）、假分支 jmp 尾。
        // 双 goto 复现形实测：`if(==0)goto L_done; goto L_regist;` 结构对齐（je 目标
        // 同为 body）但 4.4 反极性成 `sete;je` + L_regist 前 nop，差异 2 条 > 裸 if 的
        // 1 条（je↔jne，多重集仅此一处）⇒ 回滚保留裸 if（N93附/P223 镜像封顶）。
        // [A12-2026-09-10 翻案实测·失败] 正极性 `!=0` 双 goto + 标签后置
        // （`if(!=0) goto L_regist; goto L_join;` + `L_regist: RegistNextTimer();`
        // `L_join: return 1;`）实测 4.4 产物 54 条：条件跳仍反转成 `je <join>`、
        // 且 call 后多插 `jmp <join>; nop`（4.4 把标签块划为独立块）⇒ 比裸 if 的
        // 52 条多 2 条，已回滚。4.4 的 return-合并/块布局与 4.1 不同（ORIG 假路径
        // `jmp <+0xb5 join>` 为 4.1 共享出口 mov 合并产物，4.4 恒发 per-return mov）
        // ⇒ 维持 N93附/P223 封顶。
        if (ev->get_ontime_reward_user_count() != 0)
        {
            RegistNextTimer();
        }
    }
    else
    {
        cMyTrace trace(__PRETTY_FUNCTION__,
                       0x1638, 5);
        trace("[ON_TIME_EVENT] Timer_Send_Ontime_Reward() error");
        return 0;
    }
    return 1;
}

// ORIG T 0x0863ab4c：static，全部常量实参
#include "TimerDispatcher/RegistNextTimer_13.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                (TIMER_MESSAGE)0x9b, 10, 0, 0);
}

__attribute__((weak)) Timer_EventManagement::Timer_EventManagement() throw() {}
__attribute__((weak)) Timer_EventManagement::~Timer_EventManagement() throw() {}
#include "TimerDispatcher/dispatch_sig_135.inc"
{
    // ORIG 读 CEnvironment +0x37c int（字段未具名，仓库既有 reinterpret_cast 风格同 APSystemOnlyOrig.cpp:237）
    // ORIG 0x0863ab92：mov 0x37c(%eax),%ebx（register 直落 %ebx）；
    // mng 无具名槽（链式）。
    register int dayValue = G_CEnvironment()->m_serverEnvir.m_fatigueTime;
    static_cast<EventClassify::CEventScriptMng*>(G_CDataManager()->get_event_script_mng())
        ->update_time_values(dayValue);
    RegistNextTimer(0x3c);
    return 1;
}

__attribute__((weak)) Timer_QuickPartyWaitingPartyMatching::Timer_QuickPartyWaitingPartyMatching() throw() {}
__attribute__((weak)) Timer_QuickPartyWaitingPartyMatching::~Timer_QuickPartyWaitingPartyMatching() throw() {}
// ORIG Timer_QuickPartyWaitingPartyMatching::dispatch_sig @ 0x863ac18 (0x17e)
// [b28d-solo-TimerDispatcher2] 已 AE：P52 链式 receiver 内联（去 game/mgr 槽）+
// 短路条件内联去 bool cancelNow 槽 ⇒ 帧 0x38→0x28、全槽回正、113/113。
#include "TimerDispatcher/dispatch_sig_136.inc"
{
    QuickParty::CQuickParty* quickParty =
        G_CGameManager()->GetQuickParty(p1);
    if (quickParty == 0)
        return 0;

    if (!quickParty->check_timer_key((TIMER_MESSAGE)p3, p2))
        return 0;

    const int quickPartyCount = quickParty->get_quick_party_count();
    // ORIG：count < 2 && first_matching_cnt == count（movsbl 符号扩展后比较）
    if ((quickPartyCount < 2) &&
        (static_cast<int>(quickParty->get_first_matching_cnt()) == quickPartyCount))
    {
        return 0;
    }

    if (quickParty->get_timer_key((TIMER_MESSAGE)p3) == 1)
    {
        if (!cancel_quick_party(quickParty))
            return 0;
    }

    // ORIG：!(cnt < count) && !(is_change_quick_party_count()) 时取消
    // [b28d-solo-TimerDispatcher2] ORIG 无 firstCnt/cancelNow 槽（`movsbl %al,%eax;
    // cmp -0xc(%ebp),%eax; jl skip` + `xor $1; test; je skip` 两直跳）⇒ 短路条件
    // 内联（旧具名 bool 槽多 4 条物化 + 帧 +0x10）。
    if (static_cast<int>(quickParty->get_first_matching_cnt()) >= quickPartyCount &&
        !quickParty->is_change_quick_party_count())
    {
        if (!cancel_quick_party(quickParty))
            return 0;
    }

    QuickParty::STQuickPartyPoolMap_Key poolKey;   // ORIG ctor W @0x082344ca
    poolKey.m_dungeonIndex = quickParty->get_dungeon_index();
    poolKey.m_dungeonDiff  = static_cast<unsigned char>(quickParty->get_dungeon_diff());

    // [b28d-solo-TimerDispatcher2] P52/P183：ORIG 无 game/mgr 具名槽（`call
    // G_CGameManager; mov %eax,(%esp); call GetQuickPartySystemManager` 链式直传）
    // ⇒ 内联 receiver 链，帧 0x38→0x28、全槽回正。
    G_CGameManager()->GetQuickPartySystemManager()->start_game(quickParty, poolKey);
    return 1;
}

// ORIG T 0x863ad96（cancel_quick_party.md 全文转写；成员函数，需 TimerDispatcher.h 补声明）
// [b28d-30] 复核 DIFF：帧差 0x10（0x20 vs 0x30）+ 槽差（N82/P210 族），ORIG 0x0863ad96。
// [b28d-solo-TimerDispatcher2] P194/P222 + P52：ORIG `test %al,%al; je <mov $1,%ebx>`
// = **正极性包裹**（旧码 `!call` 多 xor 物化）；ORIG 无 mgr 具名槽（`call
// GetQuickPartySystemManager; mov %eax,(%esp)` 直传）⇒ 内联 receiver 链；party 槽
// 由 -0x10 回 -0xc（ORIG）。
#include "TimerDispatcher/cancel_quick_party.inc"
    QuickParty::CQuickParty* quickParty)
{
    CParty* party = quickParty->get_party_object();
    if (party == 0)
    {
        return false;
    }
    if (G_CGameManager()->GetQuickPartySystemManager()->cancel_quick_party(
            party, true, static_cast<CUser*>(0)))
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x17f);
        packet.finalize(true);
        party->send_to_party(packet);
        return false;
    }
    return true;
}

__attribute__((weak)) Timer_QuickPartyWaitingStartGame::Timer_QuickPartyWaitingStartGame() throw() {}
__attribute__((weak)) Timer_QuickPartyWaitingStartGame::~Timer_QuickPartyWaitingStartGame() throw() {}
// ORIG Timer_QuickPartyWaitingStartGame::dispatch_sig @ 0x863ae82 (0x238)
// [b28d-solo-TimerDispatcher2] 已 AE：P52 内联 getManager 链（去 managerInfo 槽）+
// P231 纯 register managerName ⇒ `mov %eax,%ebx` 与 ORIG 全等、帧 0x50→0x40；145/145。
#include "TimerDispatcher/dispatch_sig_137.inc"
{
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;

    if (!party->check_timer_key((TIMER_MESSAGE)p3, p2))
        return 0;

    party->send_quick_party_matching_result(false, (CUser*)0);
    party->set_quick_party(false);

    // [b28d-solo-TimerDispatcher2] ORIG `call getManager; mov %eax,(%esp); call
    // getCurCharacName; mov %eax,%ebx`（无 managerInfo 槽、结果落 %ebx 跨 cMyTrace
    // ctor）⇒ P52 链式内联 + P231 纯 register 提示。
    register const char* managerName = party->getManager()->getCurCharacName();
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x16a8, 0);
        trace("QUICK_PARTY_LOG : PARTY DESTROY AND LEAVE DUNGEON !! : MANAGER (%s)", managerName);
    }

    for (int i = 0; i < 4; ++i)   // asm: cmp $3 / setle（i <= 3）
    {
        if (!party->checkValidUser(i))
            continue;

        CUser* user = party->get_user(i);
        const int ret = party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
        if (ret != 1)
        {
            G_GameWorld()->out_from_dungeon(user);
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 2);
            packet.put_byte(0);
            packet.put_short(1);
            user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
            packet.finalize(true);
            G_GameWorld()->send_all(packet, user);
        }
    }
    return 1;
}

__attribute__((weak)) Timer_AssaultGhostTimer::Timer_AssaultGhostTimer() throw() {}
__attribute__((weak)) Timer_AssaultGhostTimer::~Timer_AssaultGhostTimer() throw() {}
#include "TimerDispatcher/dispatch_sig_138.inc"
{
    CUser* user = G_CGameManager()->getUser(p1, 2);

    if (user == 0)
    {
        return 0;
    }

    // ORIG 0x0863a??（报告 Timer_AssaultGhostTimer/dispatch_sig.md）：
    // getCurCharacNo(unsigned) != p3 同型 ⇒ cmp 0x14(%ebp),%eax 直比。
    if (user->getCurCharacNo() != p3)
    {
        return 0;
    }

    if (user->IsCurCharacGhost())
    {
        user->SetCurCharacGhost(false);
        user->SetCurCharacChaosDieTime(0);
        user->SetCurCharacChaosResponTime(0);
        user->m_characterView.enableSaveCharacView();

        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 3);
        packet.put_byte(1);
        packet.put_short(user->get_unique_id() & 0xffff);
        packet.put_byte(0);
        packet.finalize(true);

        G_GameWorld()->send_all(packet, user);
    }

    return 1;
}

__attribute__((weak)) Timer_GenerateCaptchaData::Timer_GenerateCaptchaData() throw() {}
__attribute__((weak)) Timer_GenerateCaptchaData::~Timer_GenerateCaptchaData() throw() {}
#include "TimerDispatcher/dispatch_sig_139.inc"
{
    // ORIG 0x863b29c: GlobalData::GenerateCaptchaData(); RegistNextTimer(3); return 1
    GlobalData::GenerateCaptchaData();
    RegistNextTimer(3);
    return 1;
}
#include "TimerDispatcher/RegistNextTimer_14.inc"
{
    // ORIG 0x863b2ba：G_TimerQueue()->InsertTimer((OBJ_TYPE)2, 0, (TIMER_MESSAGE)0xa1,
    // sec, 0, 0)。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0xa1,
                                sec, 0, 0);
}

// ===== Timer_TournamentDugeonClearStateNotiReward =====
__attribute__((weak)) Timer_TournamentDugeonClearStateNotiReward::Timer_TournamentDugeonClearStateNotiReward() throw() {}
__attribute__((weak)) Timer_TournamentDugeonClearStateNotiReward::~Timer_TournamentDugeonClearStateNotiReward() throw() {}
#include "TimerDispatcher/dispatch_sig_140.inc"
{
    // [b23S3C] check_timer_key 第二实参为 p2（非 p3，ORIG 0x0863b318）；P48 去缓存
    // get_user 每调用点重取×3（fresh .o 133/133 零差）。
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2e, p2))  // ORIG 0x0863b318：用 p2（0x10(%ebp)），非 p3
        return 0;
    if (party->GetTournamentDungeonClearState() == 0x01)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1ca);
        packet.put_byte(1);
        for (int i = 0; i <= 3; ++i)
        {
            if (party->get_user(i) == 0)
            {
                packet.put_byte(-1);
            }
            else
            {
                // ORIG 0x0863b40a：get_user ×3（每次调用点重取，P183 去缓存）；
                // 结果寄存器直比（sete）。
                if (party->get_user(i)->getCurCharacTotalFatigue() ==
                    party->get_user(i)->getCurCharacTotalMaxFatigue())
                    packet.put_byte(0);
                else
                    packet.put_byte(1);
            }
        }
        packet.finalize(true);
        party->send_to_party(packet);
        party->SetTournamentDungeonClearState(0x02);
        party->InsertTournamentDungeonTimer((TIMER_MESSAGE)0x2f);
    }
    return 1;
}

// ===== Timer_TournametDungeonClearStateSelect =====
__attribute__((weak)) Timer_TournametDungeonClearStateSelect::Timer_TournametDungeonClearStateSelect() throw() {}
__attribute__((weak)) Timer_TournametDungeonClearStateSelect::~Timer_TournametDungeonClearStateSelect() throw() {}
#include "TimerDispatcher/dispatch_sig_141.inc"
{
    (void)p3;   // ORIG 0x0863b68c：check_timer_key 用 p2（0x10(%ebp)），非 p3
    CParty* party = G_CGameManager()->GetParty(p1);
    if (party == 0)
        return 0;
    if (!party->check_timer_key((TIMER_MESSAGE)0x2f, p2))
        return 0;
    if (party->GetTournamentDungeonClearState() == 2)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1cb);
        packet.put_byte(1);
        CTournamentDungeonReward& reward =
            *static_cast<CTournamentDungeonReward*>(   // ORIG 0x0863b71b：mov party; add $0x17bc,%eax 单级
                (void*)((unsigned long)party + 0x17bcUL));
        std::vector<int> members;
        party->GetValidPartyMember(members);
        const bool victory = party->isTournamentVictory();
        if (!reward.isCardSelectComplete((ENUM_TOURNAMENT_REWARD_CARD_TYPE)2, members))
            reward.ForceSelectCard(members, victory);
        reward.MakeSelectPacket(&packet);
        packet.finalize(true);
        party->send_to_party(packet);
        party->SetTournamentDungeonClearState(3);
        party->TournamentDungeonClear(victory);
    }
    return 1;
}

__attribute__((weak)) Timer_TowerOnTime::Timer_TowerOnTime() throw() {}
__attribute__((weak)) Timer_TowerOnTime::~Timer_TowerOnTime() throw() {}
#include "TimerDispatcher/dispatch_sig_142.inc"
{
    WongWork::CBossTower* tower = G_CGameManager()->getBossTower(p1);
    if (tower == NULL) {
        return 0;
    }
    tower->_onTimerProcess(static_cast<TIMER_MESSAGE>(p3), p2);
    return 1;
}

__attribute__((weak)) Timer_NoticeEmergencyPatchMessage::Timer_NoticeEmergencyPatchMessage() throw()
{
    m_remainingNotiCount = 0xa;   // ORIG W 0x0863bc2e：movl $0xa,0x4(%eax)
}
__attribute__((weak)) Timer_NoticeEmergencyPatchMessage::~Timer_NoticeEmergencyPatchMessage() throw() {}
// ORIG T 0x0863b6c4 sz 0x1ba（_ZN33Timer_NoticeEmergencyPatchMessage12dispatch_sigEiij）
// 报告 docs/class_func_reports/Timer_NoticeEmergencyPatchMessage/dispatch_sig.md 全文已读：
// PacketGuard；put_header(0,0xc)；put_byte(0)；put_short(0)；put_byte(0)；
// local_43c[0x100] rep stos 清零；sprintf("%d분뒤 서버점검이 있습니다. %s",
//   this+0x04, GameserverDemonManagerInstance()->GetNoticeMessage())；
// len=strlen；put_int(len)；put_str(local_43c,len)；finalize(true)；
// G_GameWorld()->send_all；this+0x04 -= 1；
// ==0 → GetInstanceShutdowManager()->SendShutdownSignal() +
//   cMyTrace(PRETTY,0x1849,0)("Emergency Patch Message Received")；
// !=0 → RegistNextTimer()；return 1。
// [b28d-30] 复核 DIFF：push edi 缺失 + 帧差 4（0x43c vs 0x440，N82/P210 族），ORIG 0x0863bxxx。
// [b28d-solo-TimerDispatcher2] P167 差集 EXTRA memset×1 ⇒ ORIG 的 0x400 字节清零是
// **内联 rep stos**（N78/P214③/P312 突破口：`char buf[N] = {0}` 复现 4.1 rep stos，
// 显式 memset 调用在 4.4 恒为 call）⇒ 改数组零初始化形。残差（push edi/帧差/槽平移）
// 为 N82/P210 族。
#include "TimerDispatcher/dispatch_sig_143.inc"
                                                    unsigned int /*p3*/)
{
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    // ORIG 0863bxxx：guard ctor + 4 次 put 之后才 rep stos 清 0x400 字节 ⇒
    // 数组声明落在 put 段之后（旧形前置致缓冲/guard 槽序互换 + 帧差）。
    char local_43c[0x400] = {0};        // ORIG rep stos ecx=0x100（dword）→ 清 0x400 字节
    // P0：ORIG 无 notice 槽（`call GetNoticeMessage; mov %eax,0xc(%esp)` 直传）⇒ 内联。
    // [Main 授权 2026-09-11 · §6.5 盲区] ORIG .rodata 该串实字节 = EUC-KR(cp949)
    // （`%d` BA D0 B5 DA 20 BC AD B9 F6 C1 A1 B0 CB C0 CC 20 C0 D6 BD C0 B4 CF B4 D9 2E 20 `%s`），
    // 原 UTF-8 字面量发射 3 字节/字 ⇒ 运行期乱码。改字节转义发射（源码文件仍 UTF-8）。
    sprintf(local_43c, "\x25\x64\xba\xd0\xb5\xda\x20\xbc\xad\xb9\xf6\xc1\xa1\xb0\xcb\xc0\xcc\x20\xc0\xd6\xbd\xc0\xb4\xcf\xb4\xd9\x2e\x20\x25\x73", m_remainingNotiCount,
            GameserverDemonManagerInstance()->GetNoticeMessage());
    unsigned int len = strlen(local_43c);
    packet.put_int((int)len);
    packet.put_str(local_43c, (int)len);
    packet.finalize(true);
    G_GameWorld()->send_all(packet);
    m_remainingNotiCount -= 1;
    if (m_remainingNotiCount == 0)
    {
        GetInstanceShutdowManager()->SendShutdownSignal();
        cMyTrace(__PRETTY_FUNCTION__,
                 0x1849, 0)("Emergency Patch Message Received");
        // ORIG 0863bxxx：两分支各发 `mov $1,%ebx; jmp <共享尾>`（独立 return 1）。
        return 1;
    }
    RegistNextTimer();
    return 1;
}
// ORIG T 0x0863b87e sz 0x45（_ZN33Timer_NoticeEmergencyPatchMessage15RegistNextTimerEv）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0xa2, 0x3c, 0, 0)。
#include "TimerDispatcher/RegistNextTimer_15.inc"
{
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0xa2,
                                0x3c, 0, 0);
}

__attribute__((weak)) advancealtar::Timer_MainTick::Timer_MainTick() throw() {}
__attribute__((weak)) advancealtar::Timer_MainTick::~Timer_MainTick() throw() {}
#include "TimerDispatcher/dispatch_sig_144.inc"
{
    // ORIG 0x812f798: G_CGameManager()->onTimeAdvanceAltar(); RegistNextTimerGage(); return 1
    G_CGameManager()->onTimeAdvanceAltar();
    RegistNextTimerGage();
    return 1;
}

__attribute__((weak)) advancealtar::Timer_StageControl::Timer_StageControl() throw() {}
__attribute__((weak)) advancealtar::Timer_StageControl::~Timer_StageControl() throw() {}
#include "TimerDispatcher/dispatch_sig_145.inc"
{
    advancealtar::StageControl* stage = G_CGameManager()->getAdvanceAltar(p1);
    if (stage == NULL) {
        return 0;
    }
    stage->onTimer(static_cast<TIMER_MESSAGE>(p2),
                   static_cast<advancealtar::StageEndType::T>(p3));
    return 1;
}

// ORIG T 0x0812f850 _ZN12advancealtar18Timer_StageControl21RegistTimerCheckFinshEi
#include "TimerDispatcher/RegistTimerCheckFinsh.inc"
{
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xa7, kCheckFinishMs, 0xa7, 0);
}

// ORIG T 0x0812f89c _ZN12advancealtar18Timer_StageControl22RegistTimerClearRewardEiNS_12StageEndType1TE
#include "TimerDispatcher/RegistTimerClearReward.inc"
                                                             advancealtar::StageEndType::T endType)
{
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xa8, kClearRewardMs, 0xa8, endType);
}

// ORIG T 0x0812f8ec _ZN12advancealtar18Timer_StageControl15RegistTimerEPLPEiNS_12StageEndType1TE
#include "TimerDispatcher/RegistTimerEPLP.inc"
                                                       advancealtar::StageEndType::T endType)
{
    long ms = kFailEPLPMs;      // ORIG 先置 0x3e8；endType==3 分支同值再赋 0x3e8（语义无差异）
    if (endType == static_cast<advancealtar::StageEndType::T>(3)) {
        ms = kClearEPLPMs;
    }
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xa9, ms, 0xa9, endType);
}

// ORIG T 0x0812f956 _ZN12advancealtar18Timer_StageControl28RegistTimerEPLPReturnVillageEi
#include "TimerDispatcher/RegistTimerEPLPReturnVillage.inc"
{
    G_TimerQueue()->InsertTimerInMilisecond(
        (TimerEntry::OBJ_TYPE)8, stageIdx, (TIMER_MESSAGE)0xaa, kEPLPReturnVillageMs, 0xaa, 0);
}

__attribute__((weak)) Timer_DungeonStatisticEntrance_Hour::Timer_DungeonStatisticEntrance_Hour() throw() {}
__attribute__((weak)) Timer_DungeonStatisticEntrance_Hour::~Timer_DungeonStatisticEntrance_Hour() throw() {}
#include "TimerDispatcher/dispatch_sig_146.inc"
{
    // ORIG 0x863b9da: GetInstanceDungeonEntranceLog()->InsertDungeonEntrance_Hour(); RegistNextTimer(); return 1
    ((CDungeonEntranceLog*)GetInstanceDungeonEntranceLog())->InsertDungeonEntrance_Hour();
    RegistNextTimer();
    return 1;
}

__attribute__((weak)) Timer_RefreshServerState::Timer_RefreshServerState() throw() {}
__attribute__((weak)) Timer_RefreshServerState::~Timer_RefreshServerState() throw() {}
#include "TimerDispatcher/dispatch_sig_147.inc"
{
    // ORIG 0x863b90e: ARAD::Arad_ServerStateManager::checkContinus(s_serverStateManager_); return 1
    GlobalData::s_serverStateManager_->checkContinus();
    return 1;
}

__attribute__((weak)) Timer_StayTimeEvent::Timer_StayTimeEvent() throw() {}
__attribute__((weak)) Timer_StayTimeEvent::~Timer_StayTimeEvent() throw() {}
// ===== Timer_StayTimeEvent =====
// dispatch_sig ORIG _ZN19Timer_StayTimeEvent12dispatch_sigEiij @0x0863b928 (0xb2B)

// [b28d-solo-TimerDispatcher] P214①/P225 正极性形：ORIG 是
// `bVar1 = (ev != 0 && IsEventing(0))` 物化岛（je L0 / test;je L0 / mov$1;jmp / L0:mov$0）
// ⇒ 源码必须写正极性 `if (ev != 0 && ev->IsEventing(0)) { body } return 1;`
// （旧码 `ev==0 || !IsEventing` 使 4.4 多 `xor $1` 且 body 块错位）。
// 配套：rgf 实参内联（ORIG time() 结果驻 %ebx 跨 getRewardStayTime），ORIG 0x0863b928。
// [b28d-solo-TimerDispatcher] 已 AE：P214①/P225 正极性 `if (ev != 0 && IsEventing(0))`
// + rgf 实参内联（time()→%ebx），53/53 全等。
#include "TimerDispatcher/dispatch_sig_148.inc"
{
    CEventStayTime* ev =
        (CEventStayTime*)GlobalData::s_event_manager->GetRepeatEvent(0x9f);
    if (ev != 0 && ev->IsEventing(0))
    {
        // ORIG +0x5a/+0x67/+0x7a：右起求值 time() → getRewardStayTime()，
        // ctor 形参序 (rewardStayTime, now)（ORIG +0x6c/%ebx、+0x70/%eax 压栈实证）。
        CEventStayTime::RewardGoGoFighter visitor(ev->getRewardStayTime(),
                                                  time((time_t*)0));
        // ORIG +0x97: GameWorld::ProcessAllUsers<CEventStayTime::RewardGoGoFighter>(visitor)
        G_GameWorld()->ProcessAllUsers(visitor);
        // ORIG +0xa2: CEventStayTime::RegistNextTimer()
        ev->RegistNextTimer();
    }
    return 1;
}

__attribute__((weak)) Timer_CreateDnfDbLoad::Timer_CreateDnfDbLoad() throw() {}
__attribute__((weak)) Timer_CreateDnfDbLoad::~Timer_CreateDnfDbLoad() throw() {}
#include "TimerDispatcher/dispatch_sig_149.inc"
{
    // ORIG 0x863ba46：GetRepeatEvent(0xa0)（CEventCreateDnf）判空 + IsEventing(0)
    // 为真时 RegistNextTimer()；随后无条件 DBSelectCreateDnfEventInfo::makeRequest()。
    CEventCreateDnf* event =
        static_cast<CEventCreateDnf*>(GlobalData::s_event_manager->GetRepeatEvent(0xa0));
    if (event != 0 && event->IsEventing(0))
    {
        event->RegistNextTimer();
    }
    DBSelectCreateDnfEventInfo::makeRequest();
    return 1;
}

// ORIG T 0x0815ba94（_ZN26DBSelectCreateDnfEventInfo11makeRequestEv）
// 报告：docs/class_func_reports/DBSelectCreateDnfEventInfo/makeRequest.md：
// 事件未在进行中（或 receiver 空）时发 Taiwan DbThread 0x9 选择请求。
// [b28d-solo-TimerDispatcher2] P277 边界复核：ORIG 虚调用段 `movl $0x0,0x4(%esp)`
// （**立即数**实参）先于 `mov -0xc(%ebp),%eax`（this）。P277 探针的实参是复杂
// 表达式（引用解引用）才成立；具名派生指针形与点号形 `(*event).IsEventing(0)`
// 实测均 this-first（本件实参为立即数 0）⇒ 装载序不可翻，判 N7/P75 封顶（69/69）。
// [TimerDispatcher-3 2026-09-10 判据①] 全函数分支目标序列全等（无语义缺口）。
#include "TimerDispatcher/makeRequest.inc"
{
    CEventCreateDnf* event =
        static_cast<CEventCreateDnf*>(GlobalData::s_event_manager->GetRepeatEvent(0xa0));
    // [b28d-30] 真语义 bug 修正：ORIG 0815bab4..0815bae9 语义 = body 执行当且仅当
    // `event != 0 && IsEventing(0)==1`（Ghidra C：cVar2==1 → bVar1=false → !bVar1 进 body）。
    // 旧码 `event==0 || IsEventing(0)` 极性相反（IsEventing==0 才进 body）。修法 =
    // 早返形 `if (event==0 || !IsEventing(0)) return;`——探针 /tmp/probe30_f.cpp m1
    // 逐条复现 ORIG（je L_true / xor $1 / je L_false / test;jne 跳过 body 岛）。
    if (event == 0 || !event->IsEventing(0))
        return;
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(
            "localtaiwan/Dispatcher/Taiwan_DbThread.cpp", 0x1e4),
        true);
    Taiwan::internal_stream(guard, static_cast<TaiwanInternalPack::T>(9), -1);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}


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

// ORIG T 0x086340b4 sz 0x4f（_ZN18TimerStatisticsLog15RegistNextTimerEv）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0x52, 0x708, 0, 0)；返回 1。
#include "TimerDispatcher/RegistNextTimer_16.inc"
{
    // ORIG：delay 物化到局部槽 -0xc(%ebp) 再两步入栈。
    int delay = 0x708;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x52,
                                delay, 0, 0);
    return 1;
}

// ORIG T 0x0863a9a2 sz 0xed（_ZN23Timer_Schedule_MidNight15RegistNextTimerEv）：
// cur=getCurSec()；localtime_r(&cur+0x15180,&tm) → tm_hour/min/sec=0；
// next=mktime(&tm)；delta=next-getCurSec()；
// cMyTrace(__PRETTY_FUNCTION__,0x1626,0)
//   ("RegistNextTimer Schedule_MidNight delta %d\n", delta)；
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0x9a, delta, 0, 0)。
// [b28c-31] AE：cMyTrace 全表达式临时链（P221）复现 ORIG 0x0863aa1c 槽序
// （sec=-0x28 / trace=-0x24），54=54。
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

// ORIG T 0x086396d2 sz 0x4f（_ZN23Timer_TenMinuteSchedule15RegistNextTimerEi）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0x8e, 600, param_1, 0)；返回 1。
#include "TimerDispatcher/RegistNextTimer_18.inc"
{
    // ORIG 0x086396db：movl $0x258,-0xc(%ebp) 具名槽（帧 0x38），非 const 防常量传播，
    // 0x10(%esp)=mov -0xc(%ebp),%edx 装载。
    int nSec = 0x258;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x8e,
                                nSec, type, 0);
    return 1;
}

// ---------------------------------------------------------------------------
// LinkClosureB-2 追加：_NS_NonClient_CheckResponse（无独立报告，ORIG objdump
// 0x08274e4e sz 0xe3 逐指令转写，标注【推断】仅限语义命名）
// ---------------------------------------------------------------------------
// ORIG _Z27_NS_NonClient_CheckResponseiii：
//   user=G_CGameManager()->getUser(slot,2)；user==0 → 0；
//   (uint16)user->get_unique_id() != param_2 → 0（ORIG movzwl 16 位比较，
//     值域内与 int 比较等价）；
//   getCurCharacR()==0 → 0；getCurCharacNo()!=param_3 → 0；
//   !GetNonClientFlag()（ORIG xor $1 bool 取反）→
//     getHackAnalyzer()->addServerHackCnt(user, ENUM_HACKTYPE(0x33a), 1, 0, 0)；
//   return 1。
// [b28d-30] 复核 DIFF：N18b——同型 int callee 折 mem 直比；异型化旋钮需改
// CUserCharacInfo::getCurCharacNo 返回类型（跨泳道共享头，上报 Main 裁决）。
// [b28d-solo-TimerDispatcher] 已 AE：Main 代落 getCurCharacNo→unsigned int 后两段
// 物化对齐（63/63）。
#include "TimerDispatcher/_NS_NonClient_CheckResponse_1.inc"
{
    CUser* user = G_CGameManager()->getUser(slot, 2);
    if (user == 0)
    {
        return 0;
    }
    if (user->get_unique_id() != uniqueId)
    {
        return 0;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->getCurCharacNo() != characNo)
    {
        return 0;
    }
    if (!user->GetNonClientFlag())
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x33a, 1, 0, 0);
    }
    return 1;
}
