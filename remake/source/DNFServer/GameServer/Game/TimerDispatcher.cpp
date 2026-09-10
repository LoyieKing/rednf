// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TimerDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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
}

#include "TimerDispatcher/dtor_TimerDispatcher.inc"
}

// ORIG 0x86329f6：空槽返回 0。
#include "TimerDispatcher/get_dispatcher.inc"
}

#include "TimerDispatcher/dispatch.inc"
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
}

__attribute__((weak)) TimerDBUpdate::TimerDBUpdate() throw() {}
__attribute__((weak)) TimerDBUpdate::~TimerDBUpdate() throw() {}
// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x8631f0c 见 report。
// [b28d-solo-TimerDispatcher] 判词 N7/N82附 封顶：callee-saved 角色轮换（ORIG
// edi=characNo/esi=loginTick/ebx=UID；4.4 恒 esi/ebx/edi）+ 槽 -0x28↔-0x20。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；残差 = callee-saved 角色
// edi/esi/ebx 轮换（N7）⇒ 无语义缺口。
#include "TimerDispatcher/dispatch_sig_2.inc"
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
}

#include "TimerDispatcher/dispatch_sig_4.inc"
}

__attribute__((weak)) TimerMapLoading::TimerMapLoading() throw() {}
__attribute__((weak)) TimerMapLoading::~TimerMapLoading() throw() {}
#include "TimerDispatcher/dispatch_sig_5.inc"
}

__attribute__((weak)) TimerSetResult::TimerSetResult() throw() {}
__attribute__((weak)) TimerSetResult::~TimerSetResult() throw() {}
#include "TimerDispatcher/dispatch_sig_6.inc"
}

__attribute__((weak)) TimerDungeonClear::TimerDungeonClear() throw() {}
__attribute__((weak)) TimerDungeonClear::~TimerDungeonClear() throw() {}
#include "TimerDispatcher/dispatch_sig_7.inc"
}

__attribute__((weak)) TimerDungeonFail::TimerDungeonFail() throw() {}
__attribute__((weak)) TimerDungeonFail::~TimerDungeonFail() throw() {}
#include "TimerDispatcher/dispatch_sig_8.inc"
}

__attribute__((weak)) TimerPartyWalkOutLackUser::TimerPartyWalkOutLackUser() throw() {}
__attribute__((weak)) TimerPartyWalkOutLackUser::~TimerPartyWalkOutLackUser() throw() {}
#include "TimerDispatcher/dispatch_sig_9.inc"
}

__attribute__((weak)) TimerPVPWalkOutLackUser::TimerPVPWalkOutLackUser() throw() {}
__attribute__((weak)) TimerPVPWalkOutLackUser::~TimerPVPWalkOutLackUser() throw() {}
#include "TimerDispatcher/dispatch_sig_10.inc"
}

__attribute__((weak)) TimerFatigueReset::TimerFatigueReset() throw() {}
__attribute__((weak)) TimerFatigueReset::~TimerFatigueReset() throw() {}
// ---- TimerFatigueReset ----
// 报告：docs/class_func_reports/TimerFatigueReset/{dispatch_sig,RegistNextTimer,updateAbnormalClientCount}.md
// 头已有声明：dispatch_sig / ~ / static int RegistNextTimer()（静态与非静态 mangled 相同 _ZN17..15RegistNextTimerEv，无需改头）。
// 类对象仅 vptr。
// ORIG T 0x086339fa（0xa 字节）：mov $0x1,%eax 直返 1。
#include "TimerDispatcher/updateAbnormalClientCount.inc"
}

// ORIG T 0x08633750 (_ZN17TimerFatigueReset12dispatch_sigEiij)
// [b28d-30] 复核 DIFF：callee-saved 寄存器缓存/帧差（N7/N82/P231 族），ORIG 0x08633750。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：callee-saved 缓存组 + 帧差（167/169）。
#include "TimerDispatcher/dispatch_sig_11.inc"
}

// ORIG T 0x08633a04 (_ZN17TimerFatigueReset15RegistNextTimerEv)
// ORIG 非静态成员但不使用 this（调用点 0x086339d4 传 this）；返回 bool。
// [b28d-30] 复核 DIFF：push edi/esi/ebx 缓存组跨版本差（N7/N82 族），ORIG 0x086339d4。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：ORIG -0x4c(%ebp)→%ebx 缓存 vs 4.4 槽化
// + cMyTrace 槽 -0x38↔-0x48 + 帧 0x8c↔leave（95/94）。
#include "TimerDispatcher/RegistNextTimer.inc"
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
}

// TimerAvatarExpire ctor W 0x0863bb3e / ~D1 W 0x0863bb8a / ~D0 W 0x0863bb5a
// （ORIG 仅 vptr：TimerSig 基类 vptr 后覆写本类 vtable，与其它 Timer_* 同构）。
__attribute__((weak)) TimerAvatarExpire::TimerAvatarExpire() throw() {}
__attribute__((weak)) TimerAvatarExpire::~TimerAvatarExpire() throw() {}
// ORIG T 0x08632a78 (_ZN17TimerAvatarExpire12dispatch_sigEiij)：恒返回 1（mov $1,%eax; ret）。
#include "TimerDispatcher/dispatch_sig_12.inc"
}

// ORIG T 0x08632a82 (_ZN17TimerAvatarExpire18CheckExpiredAvatarEP5CUser)
// PacketGuard 局部 + get_state==0 → 1；getCurCharacR==0 → 0；
// 有到期头像时经 getCurCharacInvenW 发包；恒 1。
#include "TimerDispatcher/CheckExpiredAvatar.inc"
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
}

__attribute__((weak)) TimerCardSelect::TimerCardSelect() throw() {}
__attribute__((weak)) TimerCardSelect::~TimerCardSelect() throw() {}
#include "TimerDispatcher/dispatch_sig_14.inc"
}

__attribute__((weak)) TimerClearDungeonReward::TimerClearDungeonReward() throw() {}
__attribute__((weak)) TimerClearDungeonReward::~TimerClearDungeonReward() throw() {}
#include "TimerDispatcher/dispatch_sig_15.inc"
}

__attribute__((weak)) TimerClearDungeonScoreScoll::TimerClearDungeonScoreScoll() throw() {}
__attribute__((weak)) TimerClearDungeonScoreScoll::~TimerClearDungeonScoreScoll() throw() {}
#include "TimerDispatcher/dispatch_sig_16.inc"
}

__attribute__((weak)) TimerClearDungeonCardSelectRight::TimerClearDungeonCardSelectRight() throw() {}
__attribute__((weak)) TimerClearDungeonCardSelectRight::~TimerClearDungeonCardSelectRight() throw() {}
#include "TimerDispatcher/dispatch_sig_17.inc"
}

__attribute__((weak)) TimerPremiumServiceOver::TimerPremiumServiceOver() throw() {}
__attribute__((weak)) TimerPremiumServiceOver::~TimerPremiumServiceOver() throw() {}
#include "TimerDispatcher/dispatch_sig_18.inc"
}

__attribute__((weak)) TimerPremiumServiceUpdate::TimerPremiumServiceUpdate() throw() {}
__attribute__((weak)) TimerPremiumServiceUpdate::~TimerPremiumServiceUpdate() throw() {}
#include "TimerDispatcher/dispatch_sig_19.inc"
}

__attribute__((weak)) TimerPremiumServiceStart::TimerPremiumServiceStart() throw() {}
__attribute__((weak)) TimerPremiumServiceStart::~TimerPremiumServiceStart() throw() {}
#include "TimerDispatcher/dispatch_sig_20.inc"
}

__attribute__((weak)) TimerCheckPaidPCRoomTerm::TimerCheckPaidPCRoomTerm() throw() {}
__attribute__((weak)) TimerCheckPaidPCRoomTerm::~TimerCheckPaidPCRoomTerm() throw() {}
#include "TimerDispatcher/dispatch_sig_21.inc"
}

__attribute__((weak)) TimerEPLPReturnVillage::TimerEPLPReturnVillage() throw() {}
__attribute__((weak)) TimerEPLPReturnVillage::~TimerEPLPReturnVillage() throw() {}
#include "TimerDispatcher/dispatch_sig_22.inc"
}

__attribute__((weak)) TimerEPLPCommandWait::TimerEPLPCommandWait() throw() {}
__attribute__((weak)) TimerEPLPCommandWait::~TimerEPLPCommandWait() throw() {}
#include "TimerDispatcher/dispatch_sig_23.inc"
}

__attribute__((weak)) TimerWarRoomWaitCount::TimerWarRoomWaitCount() throw() {}
__attribute__((weak)) TimerWarRoomWaitCount::~TimerWarRoomWaitCount() throw() {}
#include "TimerDispatcher/dispatch_sig_24.inc"
}

__attribute__((weak)) TimerWarRoomReadyLockCount::TimerWarRoomReadyLockCount() throw() {}
__attribute__((weak)) TimerWarRoomReadyLockCount::~TimerWarRoomReadyLockCount() throw() {}
#include "TimerDispatcher/dispatch_sig_25.inc"
}

__attribute__((weak)) TimerWarRoomBattlePauseCount::TimerWarRoomBattlePauseCount() throw() {}
__attribute__((weak)) TimerWarRoomBattlePauseCount::~TimerWarRoomBattlePauseCount() throw() {}
#include "TimerDispatcher/dispatch_sig_26.inc"
}

__attribute__((weak)) TimerWarRoomResultCount::TimerWarRoomResultCount() throw() {}
__attribute__((weak)) TimerWarRoomResultCount::~TimerWarRoomResultCount() throw() {}
#include "TimerDispatcher/dispatch_sig_27.inc"
}

__attribute__((weak)) TImerWarRoomTimeBomb::TImerWarRoomTimeBomb() throw() {}
__attribute__((weak)) TImerWarRoomTimeBomb::~TImerWarRoomTimeBomb() throw() {}
#include "TimerDispatcher/dispatch_sig_28.inc"
}

__attribute__((weak)) TimerWarRoomCheckFail::TimerWarRoomCheckFail() throw() {}
__attribute__((weak)) TimerWarRoomCheckFail::~TimerWarRoomCheckFail() throw() {}
#include "TimerDispatcher/dispatch_sig_29.inc"
}

__attribute__((weak)) TimerWarRoomChangeHost::TimerWarRoomChangeHost() throw() {}
__attribute__((weak)) TimerWarRoomChangeHost::~TimerWarRoomChangeHost() throw() {}
#include "TimerDispatcher/dispatch_sig_30.inc"
}

__attribute__((weak)) TimerBloodSmallRoundRegister::TimerBloodSmallRoundRegister() throw() {}
__attribute__((weak)) TimerBloodSmallRoundRegister::~TimerBloodSmallRoundRegister() throw() {}
#include "TimerDispatcher/dispatch_sig_31.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister0::TimerBloodPhaseRegister0() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister0::~TimerBloodPhaseRegister0() throw() {}
#include "TimerDispatcher/dispatch_sig_32.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister1::TimerBloodPhaseRegister1() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister1::~TimerBloodPhaseRegister1() throw() {}
#include "TimerDispatcher/dispatch_sig_33.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister2::TimerBloodPhaseRegister2() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister2::~TimerBloodPhaseRegister2() throw() {}
#include "TimerDispatcher/dispatch_sig_34.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister3::TimerBloodPhaseRegister3() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister3::~TimerBloodPhaseRegister3() throw() {}
#include "TimerDispatcher/dispatch_sig_35.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister4::TimerBloodPhaseRegister4() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister4::~TimerBloodPhaseRegister4() throw() {}
#include "TimerDispatcher/dispatch_sig_36.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister5::TimerBloodPhaseRegister5() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister5::~TimerBloodPhaseRegister5() throw() {}
#include "TimerDispatcher/dispatch_sig_37.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister6::TimerBloodPhaseRegister6() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister6::~TimerBloodPhaseRegister6() throw() {}
#include "TimerDispatcher/dispatch_sig_38.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister7::TimerBloodPhaseRegister7() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister7::~TimerBloodPhaseRegister7() throw() {}
#include "TimerDispatcher/dispatch_sig_39.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister8::TimerBloodPhaseRegister8() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister8::~TimerBloodPhaseRegister8() throw() {}
#include "TimerDispatcher/dispatch_sig_40.inc"
}

__attribute__((weak)) TimerBloodPhaseRegister9::TimerBloodPhaseRegister9() throw() {}
__attribute__((weak)) TimerBloodPhaseRegister9::~TimerBloodPhaseRegister9() throw() {}
#include "TimerDispatcher/dispatch_sig_41.inc"
}

__attribute__((weak)) TimerBloodCrazyMonster::TimerBloodCrazyMonster() throw() {}
__attribute__((weak)) TimerBloodCrazyMonster::~TimerBloodCrazyMonster() throw() {}
#include "TimerDispatcher/dispatch_sig_42.inc"
}

__attribute__((weak)) TimerBloodDungeonClearReward::TimerBloodDungeonClearReward() throw() {}
__attribute__((weak)) TimerBloodDungeonClearReward::~TimerBloodDungeonClearReward() throw() {}
#include "TimerDispatcher/dispatch_sig_43.inc"
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
}
__attribute__((weak)) TimerEventChristmas::TimerEventChristmas() throw() {}
__attribute__((weak)) TimerEventChristmas::~TimerEventChristmas() throw() {}
#include "TimerDispatcher/dispatch_sig_45.inc"
}

__attribute__((weak)) TimerStatisticsDetailChannelInfo::TimerStatisticsDetailChannelInfo() throw() {}
__attribute__((weak)) TimerStatisticsDetailChannelInfo::~TimerStatisticsDetailChannelInfo() throw() {}
// ORIG T 0x0863575c (_ZN32TimerStatisticsDetailChannelInfo12dispatch_sigEiij)：
// G_GameWorld()->updateDetailChannelServiceInfo()；RegistNextTimer()；返回 1。
#include "TimerDispatcher/dispatch_sig_46.inc"
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
}

__attribute__((weak)) TimerStatisticLevel::TimerStatisticLevel() throw() {}
__attribute__((weak)) TimerStatisticLevel::~TimerStatisticLevel() throw() {}
#include "TimerDispatcher/dispatch_sig_47.inc"
}

__attribute__((weak)) TimerScheduleOneHour::TimerScheduleOneHour() throw() {}
__attribute__((weak)) TimerScheduleOneHour::~TimerScheduleOneHour() throw() {}
#include "TimerDispatcher/dispatch_sig_48.inc"
}

__attribute__((weak)) TimerSaveLogGamechannel::TimerSaveLogGamechannel() throw() {}
__attribute__((weak)) TimerSaveLogGamechannel::~TimerSaveLogGamechannel() throw() {}
#include "TimerDispatcher/dispatch_sig_49.inc"
}

__attribute__((weak)) TimerQueryCounter::TimerQueryCounter() throw() {}
__attribute__((weak)) TimerQueryCounter::~TimerQueryCounter() throw() {}
// ORIG TimerQueryCounter::dispatch_sig @ 0x8635a60 (0x17b)
#include "TimerDispatcher/dispatch_sig_50.inc"
}

// ORIG T 0x8635bdc（RegistNextTimer.md 全文转写；声明已在 TimerDispatcher.h，
// 签名 static void；ORIG 恒返回 eax=1，void 化由现有头口径决定）
// ORIG T 0x08635bdc：InsertTimer(2,0,0x59,0x708,0,0)；返回 1。
#include "TimerDispatcher/RegistNextTimer_2.inc"
}

__attribute__((weak)) TimerNatType::TimerNatType() throw() {}
__attribute__((weak)) TimerNatType::~TimerNatType() throw() {}
// ORIG T 0x08635c2c (_ZN12TimerNatType12dispatch_sigEiij)：
// G_GameWorld()->GetStatisticsNatType()->UpdateNatType()；RegistNextTimer()；返回 1。
#include "TimerDispatcher/dispatch_sig_51.inc"
}
// ORIG T 0x08635c5a (_ZN12TimerNatType15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x5a, 0x15180, 0, 0)；返回 1（void 化）。
#include "TimerDispatcher/RegistNextTimer_3.inc"
}

__attribute__((weak)) TimerNatTypeUser::TimerNatTypeUser() throw() {}
__attribute__((weak)) TimerNatTypeUser::~TimerNatTypeUser() throw() {}
#include "TimerDispatcher/dispatch_sig_52.inc"
}

// ORIG T 0x08635cd0 (_ZN16TimerNatTypeUser15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x5b, 0xe10, 0, 0)；返回 1。
#include "TimerDispatcher/RegistNextTimer_4.inc"
}

__attribute__((weak)) TimerCheckMailTimeOut::TimerCheckMailTimeOut() throw() {}
__attribute__((weak)) TimerCheckMailTimeOut::~TimerCheckMailTimeOut() throw() {}
// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x08635xxx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82/N93 封顶：帧/槽位跨版本差（106/109）。
#include "TimerDispatcher/dispatch_sig_53.inc"
}

__attribute__((weak)) TimerCreature::TimerCreature() throw() {}
__attribute__((weak)) TimerCreature::~TimerCreature() throw() {}
// [b28d-30] 复核 NEAR：died 槽 -0x11 vs -0xa（1 字节槽隙，N82/P190 族），ORIG 0x08635e96。
// [b28d-solo-TimerDispatcher] NEAR 封顶 N98（4.4 按首次使用序分配槽）：ORIG died 槽
// -0x11（两 byte 槽间 8B 隙）vs 4.4 -0xa；long long/volatile/double pad 三形实测反劣。
// [TimerDispatcher-3 2026-09-10 判据①] 分支目标序列全等；唯一差 = died 槽 -0x11↔-0xa
// （N98/N82 槽隙，无语义缺口）。
#include "TimerDispatcher/dispatch_sig_54.inc"
}

__attribute__((weak)) TimerForbidChat::TimerForbidChat() throw() {}
__attribute__((weak)) TimerForbidChat::~TimerForbidChat() throw() {}
#include "TimerDispatcher/dispatch_sig_55.inc"
}

__attribute__((weak)) TimerForbidMove::TimerForbidMove() throw() {}
__attribute__((weak)) TimerForbidMove::~TimerForbidMove() throw() {}
#include "TimerDispatcher/dispatch_sig_56.inc"
}

__attribute__((weak)) TimerScheduleFiveMinPerUser::TimerScheduleFiveMinPerUser() throw() {}
__attribute__((weak)) TimerScheduleFiveMinPerUser::~TimerScheduleFiveMinPerUser() throw() {}
#include "TimerDispatcher/dispatch_sig_57.inc"
}

__attribute__((weak)) TimerScheduleTenMinPerUser::TimerScheduleTenMinPerUser() throw() {}
__attribute__((weak)) TimerScheduleTenMinPerUser::~TimerScheduleTenMinPerUser() throw() {}
#include "TimerDispatcher/dispatch_sig_58.inc"
}

__attribute__((weak)) TimerQueueSizeLog::TimerQueueSizeLog() throw() {}
__attribute__((weak)) TimerQueueSizeLog::~TimerQueueSizeLog() throw() {}
#include "TimerDispatcher/dispatch_sig_59.inc"
}

// ORIG T 0x08636348 (_ZN17TimerQueueSizeLog15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x61, 0x3c, 0, 0)；返回 1。
#include "TimerDispatcher/RegistNextTimer_5.inc"
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
}

__attribute__((weak)) TimerSetPlayResult::TimerSetPlayResult() throw() {}
__attribute__((weak)) TimerSetPlayResult::~TimerSetPlayResult() throw() {}
#include "TimerDispatcher/dispatch_sig_61.inc"
}

__attribute__((weak)) TimerStartRelayBattle::TimerStartRelayBattle() throw() {}
__attribute__((weak)) TimerStartRelayBattle::~TimerStartRelayBattle() throw() {}
#include "TimerDispatcher/dispatch_sig_62.inc"
}
#include "TimerDispatcher/dispatch_sig_63.inc"
}

__attribute__((weak)) TimerEventCoinRefill::TimerEventCoinRefill() throw() {}
__attribute__((weak)) TimerEventCoinRefill::~TimerEventCoinRefill() throw() {}
#include "TimerDispatcher/dispatch_sig_64.inc"
}

__attribute__((weak)) TimerPvPDieUserRevive0::TimerPvPDieUserRevive0() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive0::~TimerPvPDieUserRevive0() throw() {}
#include "TimerDispatcher/dispatch_sig_65.inc"
}

__attribute__((weak)) TimerPvPDieUserRevive1::TimerPvPDieUserRevive1() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive1::~TimerPvPDieUserRevive1() throw() {}
#include "TimerDispatcher/dispatch_sig_66.inc"
}

__attribute__((weak)) TimerPvPDieUserRevive2::TimerPvPDieUserRevive2() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive2::~TimerPvPDieUserRevive2() throw() {}
#include "TimerDispatcher/dispatch_sig_67.inc"
}

__attribute__((weak)) TimerPvPDieUserRevive3::TimerPvPDieUserRevive3() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive3::~TimerPvPDieUserRevive3() throw() {}
#include "TimerDispatcher/dispatch_sig_68.inc"
}

__attribute__((weak)) TimerPvPDieUserRevive4::TimerPvPDieUserRevive4() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive4::~TimerPvPDieUserRevive4() throw() {}
#include "TimerDispatcher/dispatch_sig_69.inc"
}

__attribute__((weak)) TimerPvPDieUserRevive5::TimerPvPDieUserRevive5() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive5::~TimerPvPDieUserRevive5() throw() {}
#include "TimerDispatcher/dispatch_sig_70.inc"
}

__attribute__((weak)) TimerPvPDieUserRevive6::TimerPvPDieUserRevive6() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive6::~TimerPvPDieUserRevive6() throw() {}
#include "TimerDispatcher/dispatch_sig_71.inc"
}

__attribute__((weak)) TimerPvPDieUserRevive7::TimerPvPDieUserRevive7() throw() {}
__attribute__((weak)) TimerPvPDieUserRevive7::~TimerPvPDieUserRevive7() throw() {}
#include "TimerDispatcher/dispatch_sig_72.inc"
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
}

// ORIG T 0x08636c14（_ZN22TimerLightServerNotice15RegistNextTimerEi）
#include "TimerDispatcher/RegistNextTimer_7.inc"
}

__attribute__((weak)) TimerInsertUserPlayTime::TimerInsertUserPlayTime() throw() {}
__attribute__((weak)) TimerInsertUserPlayTime::~TimerInsertUserPlayTime() throw() {}
#include "TimerDispatcher/dispatch_sig_76.inc"
}

// ORIG T 0x08636d44（_ZN23TimerInsertUserPlayTime15RegistNextTimerEv）
#include "TimerDispatcher/RegistNextTimer_8.inc"
}

__attribute__((weak)) TimerCheckForcedDisconnect::TimerCheckForcedDisconnect() throw() {}
__attribute__((weak)) TimerCheckForcedDisconnect::~TimerCheckForcedDisconnect() throw() {}
// ---- TimerCheckForcedDisconnect ----
// 报告：docs/class_func_reports/TimerCheckForcedDisconnect/{dispatch_sig,RegistNextTimer}.md

// ORIG T 0x08636e06 (_ZN26TimerCheckForcedDisconnect12dispatch_sigEiij)
// 参数语义（实测 2026-08-30）：getUser(p1, 0x2)，比较 unique_id!=p2；
// NEW 曾误用 p2/p3，已修正。
#include "TimerDispatcher/dispatch_sig_77.inc"
}

// ORIG T 0x08636f26 (_ZN26TimerCheckForcedDisconnect15RegistNextTimerEP5CUser)
// 头声明为 static bool；ORIG eax 恒为 1，语义等价。
// TimerDispatcher.h 现签名 `static bool RegistNextTimer(CUser*)` 即可，无需改动。
#include "TimerDispatcher/RegistNextTimer_9.inc"
}

__attribute__((weak)) TimerCheckPvPRoomUser::TimerCheckPvPRoomUser() throw() {}
__attribute__((weak)) TimerCheckPvPRoomUser::~TimerCheckPvPRoomUser() throw() {}
// ===== TimerCheckPvPRoomUser =====
// dispatch_sig ORIG _ZN21TimerCheckPvPRoomUser12dispatch_sigEiij @0x08636f90 (0xa9B)

#include "TimerDispatcher/dispatch_sig_78.inc"
}

__attribute__((weak)) TimerCheckRecvPvpEndResult::TimerCheckRecvPvpEndResult() throw() {}
__attribute__((weak)) TimerCheckRecvPvpEndResult::~TimerCheckRecvPvpEndResult() throw() {}
// ---- TimerCheckRecvPvpEndResult ----
// 报告：docs/class_func_reports/TimerCheckRecvPvpEndResult/dispatch_sig.md

// ORIG T 0x0863703a (_ZN26TimerCheckRecvPvpEndResult12dispatch_sigEiij)
#include "TimerDispatcher/dispatch_sig_79.inc"
}

__attribute__((weak)) TimerCheckHumanCertifyTimeout::TimerCheckHumanCertifyTimeout() throw() {}
__attribute__((weak)) TimerCheckHumanCertifyTimeout::~TimerCheckHumanCertifyTimeout() throw() {}
// [b28d-30] 复核 DIFF：槽位差 8（N82/N93 族），ORIG 0x086369xx 见 report。
// [b28d-solo-TimerDispatcher] 判词 N82+P292 封顶：槽平移 -0xc↔-0x14 + 块序差；
// isSendMailCertified 需 bool（CUser.h 冻结，已上报 Main）（105/113）。
// [TimerDispatcher-3 2026-09-10 判据①] 15 条 jcc 目标逐条对应（整体 +1 位移由一条 mov
// 中转引起）；唯一差 = call 结果 eax↔edx 中转（N22/N7；反形实测 116 insns 已回滚）。
#include "TimerDispatcher/dispatch_sig_80.inc"
}

__attribute__((weak)) TimerUpdateItemGenerateStatistics::TimerUpdateItemGenerateStatistics() throw() {}
__attribute__((weak)) TimerUpdateItemGenerateStatistics::~TimerUpdateItemGenerateStatistics() throw() {}
#include "TimerDispatcher/dispatch_sig_81.inc"
}

// ORIG T 0x08637318 _ZN33TimerUpdateItemGenerateStatistics15registNextTimerEl
#include "TimerDispatcher/registNextTimer.inc"
}

__attribute__((weak)) TimerUpdatePvPGrade::TimerUpdatePvPGrade() throw() {}
__attribute__((weak)) TimerUpdatePvPGrade::~TimerUpdatePvPGrade() throw() {}
#include "TimerDispatcher/dispatch_sig_82.inc"
}

// ORIG T 0x086373f2（objdump 转写，尾界 next symbol 0x86374b5）：
// channel_no!=1 直接返回；否则 localtime_r(&sec) 对齐当日 02:00:00，
// delay=max(target-curSec,1)，InsertTimer(OBJ_TYPE_2,0,0x6c,delay,0,0)。
#include "TimerDispatcher/registNextTimer_1.inc"
}

__attribute__((weak)) TimerDetectDisconnect::TimerDetectDisconnect() throw() {}
__attribute__((weak)) TimerDetectDisconnect::~TimerDetectDisconnect() throw() {}
#include "TimerDispatcher/dispatch_sig_83.inc"
}

__attribute__((weak)) TimerCompleteLoadAssaultTimeout::TimerCompleteLoadAssaultTimeout() throw() {}
__attribute__((weak)) TimerCompleteLoadAssaultTimeout::~TimerCompleteLoadAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_84.inc"
}

__attribute__((weak)) TimerConnectP2PAssaultTimeout::TimerConnectP2PAssaultTimeout() throw() {}
__attribute__((weak)) TimerConnectP2PAssaultTimeout::~TimerConnectP2PAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_85.inc"
}

__attribute__((weak)) TimerEndAssault::TimerEndAssault() throw() {}
__attribute__((weak)) TimerEndAssault::~TimerEndAssault() throw() {}
#include "TimerDispatcher/dispatch_sig_86.inc"
}

__attribute__((weak)) TimerCloseAssault::TimerCloseAssault() throw() {}
__attribute__((weak)) TimerCloseAssault::~TimerCloseAssault() throw() {}
#include "TimerDispatcher/dispatch_sig_87.inc"
}

// ---- pvp_assault 超时定时器注册（ORIG T，四函数同构，仅 TIMER_MESSAGE 不同）----
// objdump 0x863755a/0x86375ca/0x863763a/0x86376aa：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE_2, p1, msg, (int)sec, p2, 0)
#include "TimerDispatcher/registNextTimer_2.inc"
}

#include "TimerDispatcher/registNextTimer_3.inc"
}

#include "TimerDispatcher/registNextTimer_4.inc"
}

#include "TimerDispatcher/registNextTimer_5.inc"
}

__attribute__((weak)) TimerCompleteLoadAfterAssaultTimeout::TimerCompleteLoadAfterAssaultTimeout() throw() {}
__attribute__((weak)) TimerCompleteLoadAfterAssaultTimeout::~TimerCompleteLoadAfterAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_88.inc"
}

__attribute__((weak)) TimerConnectP2PAfterAssaultTimeout::TimerConnectP2PAfterAssaultTimeout() throw() {}
__attribute__((weak)) TimerConnectP2PAfterAssaultTimeout::~TimerConnectP2PAfterAssaultTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_89.inc"
}

// ORIG T 0x0863778a size 0x42：
//   G_TimerQueue()->InsertTimer(OBJ_TYPE 1, partyIdx, TIMER_MESSAGE 0x72,
//                               sec, key, 0)
// —— CloseAssault 后按队安排 ConnectP2PAfterAssault 超时重载。
#include "TimerDispatcher/registNextTimer_6.inc"
}

__attribute__((weak)) TimerDeathTower::TimerDeathTower() throw() {}
__attribute__((weak)) TimerDeathTower::~TimerDeathTower() throw() {}
#include "TimerDispatcher/dispatch_sig_90.inc"
}

__attribute__((weak)) TimerDeathTowerTopRankerReload::TimerDeathTowerTopRankerReload() throw() {}
__attribute__((weak)) TimerDeathTowerTopRankerReload::~TimerDeathTowerTopRankerReload() throw() {}
#include "TimerDispatcher/dispatch_sig_91.inc"
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
}

// ORIG T 0x08637d64 (_ZN21TimerFinishItemEffect36_processSepecialEffectContinuousItemEP5CUserj)
// ⚠ 私有方法，TimerDispatcher.h 该类内需补声明
//   `void _processSepecialEffectContinuousItem(CUser*, unsigned int);`（NEEDS_LOCAL_STATIC）
// [b28d-30] ORIG `mov 0x10(%ebp),%eax; cmp $0x1cd1,%eax; jne` 载入+寄存器比较：
// 原码是单 case switch（P208/P251 应用）。探针 /tmp/probe30_b.cpp f6 实证
// 4.4 对 `switch(a){case K:}` 逐条复现该形；if 形恒折 mem 直比（旧 N18b 判词作废）。
#include "TimerDispatcher/_processSepecialEffectContinuousItem.inc"
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
}

// ORIG T 0x08637e14
#include "TimerDispatcher/registNextTimer_7.inc"
}

__attribute__((weak)) TimerGlobalEffectItemBegin::TimerGlobalEffectItemBegin() throw() {}
__attribute__((weak)) TimerGlobalEffectItemBegin::~TimerGlobalEffectItemBegin() throw() {}
#include "TimerDispatcher/dispatch_sig_95.inc"
}

__attribute__((weak)) TimerGlobalEffectItemTimeout::TimerGlobalEffectItemTimeout() throw() {}
__attribute__((weak)) TimerGlobalEffectItemTimeout::~TimerGlobalEffectItemTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_96.inc"
}

// ORIG T 0x08637ef6 _ZN28TimerGlobalEffectItemTimeout11registTimerEjj
#include "TimerDispatcher/registTimer.inc"
}

__attribute__((weak)) TimerVendingMachineItemNotify::TimerVendingMachineItemNotify() throw() {}
__attribute__((weak)) TimerVendingMachineItemNotify::~TimerVendingMachineItemNotify() throw() {}
// ORIG TimerVendingMachineItemNotify::dispatch_sig @ 0x8637f40 (0x134)
#include "TimerDispatcher/dispatch_sig_97.inc"
}

__attribute__((weak)) TimerConnectMonitorServer::TimerConnectMonitorServer() throw() {}
__attribute__((weak)) TimerConnectMonitorServer::~TimerConnectMonitorServer() throw() {}
// ===== TimerConnectMonitorServer =====
// dispatch_sig ORIG _ZN25TimerConnectMonitorServer12dispatch_sigEiij @0x086380bc (0xe3B)
// registNextTimer ORIG _ZN25TimerConnectMonitorServer15registNextTimerElii @0x086381a0 (0x41B)

#include "TimerDispatcher/dispatch_sig_98.inc"
}

// ORIG T 0x086381a0：static 注册链，与其它 registNextTimer 族同构
#include "TimerDispatcher/registNextTimer_8.inc"
}

__attribute__((weak)) TimerAssaultSuperPowerState::TimerAssaultSuperPowerState() throw() {}
__attribute__((weak)) TimerAssaultSuperPowerState::~TimerAssaultSuperPowerState() throw() {}
#include "TimerDispatcher/dispatch_sig_99.inc"
}

__attribute__((weak)) TimerNPCMoodChange::TimerNPCMoodChange() throw() {}
__attribute__((weak)) TimerNPCMoodChange::~TimerNPCMoodChange() throw() {}
#include "TimerDispatcher/dispatch_sig_100.inc"
}

// ORIG T 0x086381ec (_ZN18TimerNPCMoodChange15registNextTimerEl)：

__attribute__((weak)) TimerElevatorDungeon::TimerElevatorDungeon() throw() {}
__attribute__((weak)) TimerElevatorDungeon::~TimerElevatorDungeon() throw() {}
#include "TimerDispatcher/dispatch_sig_101.inc"
}

__attribute__((weak)) TimerRefreshPowerWarProcessInfo::TimerRefreshPowerWarProcessInfo() throw() {}
__attribute__((weak)) TimerRefreshPowerWarProcessInfo::~TimerRefreshPowerWarProcessInfo() throw() {}
#include "TimerDispatcher/dispatch_sig_102.inc"
}

// ORIG T 0x08638316 _ZN31TimerRefreshPowerWarProcessInfo15registNextTimerEl
#include "TimerDispatcher/registNextTimer_9.inc"
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
}

// ORIG T 0x08638960 (_ZN20TimerGeneratePowerUp15generatePowerUpERSt6vectorIP5CUserSaIS2_EEi)
// [b28d-30] 复核 DIFF：缺 -0x18/-0x30 槽与 0x2710 常量槽（N82/N93 族），ORIG 0x08638960。
// [b28d-solo-TimerDispatcher] 判词 N82/N93 封顶：缺 -0x18/-0x30 槽与 0x2710 常量槽
// + 帧 0x50↔0x40（89/89）。
#include "TimerDispatcher/generatePowerUp.inc"
}

// ORIG T 0x08638aac (_ZN20TimerGeneratePowerUp15sendPowerUpInfoEii)
#include "TimerDispatcher/sendPowerUpInfo.inc"
}

// ORIG T 0x08638b5c (_ZN20TimerGeneratePowerUp15registNextTimerEl)
#include "TimerDispatcher/registNextTimer_10.inc"
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
}

// ORIG T 0x0863847e (_ZN28TimerGeneratePowerUpNotiTime15registNextTimerEl)
#include "TimerDispatcher/registNextTimer_11.inc"
}

// ORIG T 0x086384c2 (_ZN28TimerGeneratePowerUpNotiTime14send_noti_timeEi)
#include "TimerDispatcher/send_noti_time.inc"
}

__attribute__((weak)) TimerCheckPowerWarAbuser::TimerCheckPowerWarAbuser() throw() {}
__attribute__((weak)) TimerCheckPowerWarAbuser::~TimerCheckPowerWarAbuser() throw() {}
#include "TimerDispatcher/dispatch_sig_105.inc"
}

// ORIG T 0x08638294 _ZN24TimerCheckPowerWarAbuser15registNextTimerEl
#include "TimerDispatcher/registNextTimer_12.inc"
}

__attribute__((weak)) TimerSetPunishUser::TimerSetPunishUser() throw() {}
__attribute__((weak)) TimerSetPunishUser::~TimerSetPunishUser() throw() {}
#include "TimerDispatcher/dispatch_sig_106.inc"
}

__attribute__((weak)) TimerSetTradePunishUser::TimerSetTradePunishUser() throw() {}
__attribute__((weak)) TimerSetTradePunishUser::~TimerSetTradePunishUser() throw() {}
#include "TimerDispatcher/dispatch_sig_107.inc"
}

__attribute__((weak)) TimerRevivalCharac::TimerRevivalCharac() throw() {}
__attribute__((weak)) TimerRevivalCharac::~TimerRevivalCharac() throw() {}
#include "TimerDispatcher/dispatch_sig_108.inc"
}

__attribute__((weak)) TimerRequestExit::TimerRequestExit() throw() {}
__attribute__((weak)) TimerRequestExit::~TimerRequestExit() throw() {}
// [b28d-30] 复核 DIFF：同 _NS_NonClient_CheckResponse——N18b 同型折 mem；异型化旋钮跨泳道。
// [b28d-solo-TimerDispatcher] 已 AE：Main 代落 getCurCharacNo→unsigned int 后两段
// 物化对齐（42/42）。
#include "TimerDispatcher/dispatch_sig_109.inc"
}

__attribute__((weak)) TimerVillageMonsterUpdateRate::TimerVillageMonsterUpdateRate() throw() {}
__attribute__((weak)) TimerVillageMonsterUpdateRate::~TimerVillageMonsterUpdateRate() throw() {}
#include "TimerDispatcher/dispatch_sig_110.inc"
}

__attribute__((weak)) TimerVillageMonsterRegen::TimerVillageMonsterRegen() throw() {}
__attribute__((weak)) TimerVillageMonsterRegen::~TimerVillageMonsterRegen() throw() {}
#include "TimerDispatcher/dispatch_sig_111.inc"
}

// ORIG 0x8639042：村战开启定时器（报告 TimerStartVillageAttacked/dispatch_sig.md）。
#include "TimerDispatcher/dispatch_sig_112.inc"
}

__attribute__((weak)) TimerVillageAttackedPlayTimeout::TimerVillageAttackedPlayTimeout() throw() {}
__attribute__((weak)) TimerVillageAttackedPlayTimeout::~TimerVillageAttackedPlayTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_113.inc"
}

__attribute__((weak)) TimerItemUnlockWaiting::TimerItemUnlockWaiting() throw() {}
__attribute__((weak)) TimerItemUnlockWaiting::~TimerItemUnlockWaiting() throw() {}
#include "TimerDispatcher/dispatch_sig_114.inc"
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
}

__attribute__((weak)) TimerStartExpEventAfterVillageAttack::TimerStartExpEventAfterVillageAttack() throw() {}
__attribute__((weak)) TimerStartExpEventAfterVillageAttack::~TimerStartExpEventAfterVillageAttack() throw() {}
#include "TimerDispatcher/dispatch_sig_116.inc"
}

__attribute__((weak)) Timer_Security_NonClient_CheckResponse::Timer_Security_NonClient_CheckResponse() throw() {}
__attribute__((weak)) Timer_Security_NonClient_CheckResponse::~Timer_Security_NonClient_CheckResponse() throw() {}
#include "TimerDispatcher/dispatch_sig_117.inc"
}

__attribute__((weak)) Timer_Midnight::Timer_Midnight() throw() {}
__attribute__((weak)) Timer_Midnight::~Timer_Midnight() throw() {}
#include "TimerDispatcher/dispatch_sig_118.inc"
}

// ORIG T 0x08639554 _ZN14Timer_Midnight15RegistNextTimerEv
// [b28d-30] 复核 DIFF：push edi/esi/ebx 缓存组/帧差（N7/N82 族），ORIG 0x08639554。
// [b28d-solo-TimerDispatcher] 判词 N7/N82 封顶：push edi/esi/ebx 缓存组 + 槽差
// （-0x3c↔-0x2c 等）（71/72）。
#include "TimerDispatcher/RegistNextTimer_10.inc"
}

__attribute__((weak)) TimerEndPowerWarVictoriousReward::TimerEndPowerWarVictoriousReward() throw() {}
__attribute__((weak)) TimerEndPowerWarVictoriousReward::~TimerEndPowerWarVictoriousReward() throw() {}
#include "TimerDispatcher/dispatch_sig_119.inc"
}

__attribute__((weak)) Timer_TenMinuteSchedule::Timer_TenMinuteSchedule() throw() {}
__attribute__((weak)) Timer_TenMinuteSchedule::~Timer_TenMinuteSchedule() throw() {}
#include "TimerDispatcher/dispatch_sig_120.inc"
}

__attribute__((weak)) Timer_UpdateOnlinePreliminaryTeam::Timer_UpdateOnlinePreliminaryTeam() throw() {}
__attribute__((weak)) Timer_UpdateOnlinePreliminaryTeam::~Timer_UpdateOnlinePreliminaryTeam() throw() {}
#include "TimerDispatcher/dispatch_sig_121.inc"
}

// ORIG T 0x0863977e _ZN33Timer_UpdateOnlinePreliminaryTeam15RegistNextTimerEi
#include "TimerDispatcher/RegistNextTimer_11.inc"
}

__attribute__((weak)) Timer_SecretShopStatistic::Timer_SecretShopStatistic() throw() {}
__attribute__((weak)) Timer_SecretShopStatistic::~Timer_SecretShopStatistic() throw() {}
#include "TimerDispatcher/dispatch_sig_122.inc"
}

// ORIG T 0x086397ee (_ZN25Timer_SecretShopStatistic15RegistNextTimerEv)：
// G_TimerQueue()->InsertTimer(2, 0, 0x90, 0x3c, 0, 0)。
#include "TimerDispatcher/RegistNextTimer_12.inc"
}

__attribute__((weak)) Timer_DailySchedule::Timer_DailySchedule() throw() {}
__attribute__((weak)) Timer_DailySchedule::~Timer_DailySchedule() throw() {}

__attribute__((weak)) Timer_ConnectP2PPvPTimeout::Timer_ConnectP2PPvPTimeout() throw() {}
__attribute__((weak)) Timer_ConnectP2PPvPTimeout::~Timer_ConnectP2PPvPTimeout() throw() {}
#include "TimerDispatcher/dispatch_sig_123.inc"
}

__attribute__((weak)) Timer_RoutingItemWait::Timer_RoutingItemWait() throw() {}
__attribute__((weak)) Timer_RoutingItemWait::~Timer_RoutingItemWait() throw() {}
#include "TimerDispatcher/dispatch_sig_124.inc"
}

__attribute__((weak)) Timer_PcRoomAuthRetry::Timer_PcRoomAuthRetry() throw() {}
__attribute__((weak)) Timer_PcRoomAuthRetry::~Timer_PcRoomAuthRetry() throw() {}
// ===== Timer_PcRoomAuthRetry =====
// dispatch_sig ORIG _ZN21Timer_PcRoomAuthRetry12dispatch_sigEiij @0x08639924 (0xacB)
// registNextTimer ORIG _ZN21Timer_PcRoomAuthRetry15registNextTimerEliij @0x086399d0 (0x41B)

#include "TimerDispatcher/dispatch_sig_125.inc"
}

// ORIG T 0x086399d0：static，四参全转发（无 this 使用）
#include "TimerDispatcher/registNextTimer_14.inc"
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
}

__attribute__((weak)) Timer_FairPvPRematch::Timer_FairPvPRematch() throw() {}
__attribute__((weak)) Timer_FairPvPRematch::~Timer_FairPvPRematch() throw() {}
// ===== Timer_FairPvPRematch =====
// dispatch_sig ORIG _ZN20Timer_FairPvPRematch12dispatch_sigEiij @0x0863a430 (0x97B)

#include "TimerDispatcher/dispatch_sig_128.inc"
}

__attribute__((weak)) Timer_ReturnUserTimeout::Timer_ReturnUserTimeout() throw() {}
__attribute__((weak)) Timer_ReturnUserTimeout::~Timer_ReturnUserTimeout() throw() {}
// ORIG Timer_ReturnUserTimeout::dispatch_sig @ 0x863a4c8 (0x23e)
#include "TimerDispatcher/dispatch_sig_129.inc"
}

// ORIG _ZN23Timer_ReturnUserTimeout15registNextTimerEiij @0x0863a706：
//   now = CSystemTime::getCurSec(&GlobalData::s_systemTime_);
//   G_TimerQueue()->InsertTimer(OBJ_TYPE_2, p1, (TIMER_MESSAGE)0x95,
//                               (int)(p3 - now), p2, 0)
#include "TimerDispatcher/registNextTimer_15.inc"
}

__attribute__((weak)) Timer_HadesHeartBeat::Timer_HadesHeartBeat() throw() {}
__attribute__((weak)) Timer_HadesHeartBeat::~Timer_HadesHeartBeat() throw() {}
#include "TimerDispatcher/dispatch_sig_130.inc"
}

// ORIG T 0x0863a7c2 _ZN20Timer_HadesHeartBeat15registNextTimerEv
#include "TimerDispatcher/registNextTimer_16.inc"
}

__attribute__((weak)) Timer_TOD_Send_ClearReward::Timer_TOD_Send_ClearReward() throw() {}
__attribute__((weak)) Timer_TOD_Send_ClearReward::~Timer_TOD_Send_ClearReward() throw() {}
#include "TimerDispatcher/dispatch_sig_131.inc"
}

__attribute__((weak)) Timer_OneADayItemShop::Timer_OneADayItemShop() throw() {}
__attribute__((weak)) Timer_OneADayItemShop::~Timer_OneADayItemShop() throw() {}
#include "TimerDispatcher/dispatch_sig_132.inc"
}

__attribute__((weak)) Timer_Schedule_MidNight::Timer_Schedule_MidNight() throw() {}
__attribute__((weak)) Timer_Schedule_MidNight::~Timer_Schedule_MidNight() throw() {}
#include "TimerDispatcher/dispatch_sig_133.inc"
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
}

// ORIG T 0x0863ab4c：static，全部常量实参
#include "TimerDispatcher/RegistNextTimer_13.inc"
}

__attribute__((weak)) Timer_EventManagement::Timer_EventManagement() throw() {}
__attribute__((weak)) Timer_EventManagement::~Timer_EventManagement() throw() {}
#include "TimerDispatcher/dispatch_sig_135.inc"
}

__attribute__((weak)) Timer_QuickPartyWaitingPartyMatching::Timer_QuickPartyWaitingPartyMatching() throw() {}
__attribute__((weak)) Timer_QuickPartyWaitingPartyMatching::~Timer_QuickPartyWaitingPartyMatching() throw() {}
// ORIG Timer_QuickPartyWaitingPartyMatching::dispatch_sig @ 0x863ac18 (0x17e)
// [b28d-solo-TimerDispatcher2] 已 AE：P52 链式 receiver 内联（去 game/mgr 槽）+
// 短路条件内联去 bool cancelNow 槽 ⇒ 帧 0x38→0x28、全槽回正、113/113。
#include "TimerDispatcher/dispatch_sig_136.inc"
}

// ORIG T 0x863ad96（cancel_quick_party.md 全文转写；成员函数，需 TimerDispatcher.h 补声明）
// [b28d-30] 复核 DIFF：帧差 0x10（0x20 vs 0x30）+ 槽差（N82/P210 族），ORIG 0x0863ad96。
// [b28d-solo-TimerDispatcher2] P194/P222 + P52：ORIG `test %al,%al; je <mov $1,%ebx>`
// = **正极性包裹**（旧码 `!call` 多 xor 物化）；ORIG 无 mgr 具名槽（`call
// GetQuickPartySystemManager; mov %eax,(%esp)` 直传）⇒ 内联 receiver 链；party 槽
// 由 -0x10 回 -0xc（ORIG）。
#include "TimerDispatcher/cancel_quick_party.inc"
}

__attribute__((weak)) Timer_QuickPartyWaitingStartGame::Timer_QuickPartyWaitingStartGame() throw() {}
__attribute__((weak)) Timer_QuickPartyWaitingStartGame::~Timer_QuickPartyWaitingStartGame() throw() {}
// ORIG Timer_QuickPartyWaitingStartGame::dispatch_sig @ 0x863ae82 (0x238)
// [b28d-solo-TimerDispatcher2] 已 AE：P52 内联 getManager 链（去 managerInfo 槽）+
// P231 纯 register managerName ⇒ `mov %eax,%ebx` 与 ORIG 全等、帧 0x50→0x40；145/145。
#include "TimerDispatcher/dispatch_sig_137.inc"
}

__attribute__((weak)) Timer_AssaultGhostTimer::Timer_AssaultGhostTimer() throw() {}
__attribute__((weak)) Timer_AssaultGhostTimer::~Timer_AssaultGhostTimer() throw() {}
#include "TimerDispatcher/dispatch_sig_138.inc"
}

__attribute__((weak)) Timer_GenerateCaptchaData::Timer_GenerateCaptchaData() throw() {}
__attribute__((weak)) Timer_GenerateCaptchaData::~Timer_GenerateCaptchaData() throw() {}
#include "TimerDispatcher/dispatch_sig_139.inc"
}
#include "TimerDispatcher/RegistNextTimer_14.inc"
}

// ===== Timer_TournamentDugeonClearStateNotiReward =====
__attribute__((weak)) Timer_TournamentDugeonClearStateNotiReward::Timer_TournamentDugeonClearStateNotiReward() throw() {}
__attribute__((weak)) Timer_TournamentDugeonClearStateNotiReward::~Timer_TournamentDugeonClearStateNotiReward() throw() {}
#include "TimerDispatcher/dispatch_sig_140.inc"
}

// ===== Timer_TournametDungeonClearStateSelect =====
__attribute__((weak)) Timer_TournametDungeonClearStateSelect::Timer_TournametDungeonClearStateSelect() throw() {}
__attribute__((weak)) Timer_TournametDungeonClearStateSelect::~Timer_TournametDungeonClearStateSelect() throw() {}
#include "TimerDispatcher/dispatch_sig_141.inc"
}

__attribute__((weak)) Timer_TowerOnTime::Timer_TowerOnTime() throw() {}
__attribute__((weak)) Timer_TowerOnTime::~Timer_TowerOnTime() throw() {}
#include "TimerDispatcher/dispatch_sig_142.inc"
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
}
// ORIG T 0x0863b87e sz 0x45（_ZN33Timer_NoticeEmergencyPatchMessage15RegistNextTimerEv）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0xa2, 0x3c, 0, 0)。
#include "TimerDispatcher/RegistNextTimer_15.inc"
}

__attribute__((weak)) advancealtar::Timer_MainTick::Timer_MainTick() throw() {}
__attribute__((weak)) advancealtar::Timer_MainTick::~Timer_MainTick() throw() {}
#include "TimerDispatcher/dispatch_sig_144.inc"
}

__attribute__((weak)) advancealtar::Timer_StageControl::Timer_StageControl() throw() {}
__attribute__((weak)) advancealtar::Timer_StageControl::~Timer_StageControl() throw() {}
#include "TimerDispatcher/dispatch_sig_145.inc"
}

// ORIG T 0x0812f850 _ZN12advancealtar18Timer_StageControl21RegistTimerCheckFinshEi
#include "TimerDispatcher/RegistTimerCheckFinsh.inc"
}

// ORIG T 0x0812f89c _ZN12advancealtar18Timer_StageControl22RegistTimerClearRewardEiNS_12StageEndType1TE
#include "TimerDispatcher/RegistTimerClearReward.inc"
}

// ORIG T 0x0812f8ec _ZN12advancealtar18Timer_StageControl15RegistTimerEPLPEiNS_12StageEndType1TE
#include "TimerDispatcher/RegistTimerEPLP.inc"
}

// ORIG T 0x0812f956 _ZN12advancealtar18Timer_StageControl28RegistTimerEPLPReturnVillageEi
#include "TimerDispatcher/RegistTimerEPLPReturnVillage.inc"
}

__attribute__((weak)) Timer_DungeonStatisticEntrance_Hour::Timer_DungeonStatisticEntrance_Hour() throw() {}
__attribute__((weak)) Timer_DungeonStatisticEntrance_Hour::~Timer_DungeonStatisticEntrance_Hour() throw() {}
#include "TimerDispatcher/dispatch_sig_146.inc"
}

__attribute__((weak)) Timer_RefreshServerState::Timer_RefreshServerState() throw() {}
__attribute__((weak)) Timer_RefreshServerState::~Timer_RefreshServerState() throw() {}
#include "TimerDispatcher/dispatch_sig_147.inc"
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
}

__attribute__((weak)) Timer_CreateDnfDbLoad::Timer_CreateDnfDbLoad() throw() {}
__attribute__((weak)) Timer_CreateDnfDbLoad::~Timer_CreateDnfDbLoad() throw() {}
#include "TimerDispatcher/dispatch_sig_149.inc"
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
}

// ORIG T 0x086396d2 sz 0x4f（_ZN23Timer_TenMinuteSchedule15RegistNextTimerEi）：
// G_TimerQueue()->InsertTimer(OBJ_TYPE_2, 0, 0x8e, 600, param_1, 0)；返回 1。
#include "TimerDispatcher/RegistNextTimer_18.inc"
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
}
