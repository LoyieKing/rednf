#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CGameManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "GameRand.h"
// [A12-2026-09-10 · 本轮 15 件复核（判据⑩⑪⑫ + 报告兜底取证）] 结论：13 业务件全 N 封顶，
// 零净改动（TU 四数 identical=729/ae=1894/near=0/diff=16 改前=改后）。
//  · 判据⑪（/tmp/shift.py，必读 .o）：returnUserPool / GetPvp(int,CUser*,int) 为
//    PURE SHIFT c=0（帧 96=96 / 76=76，无槽平移，残差纯指令级）；createUser（帧
//    108 vs 96）、Send_userinfos_to_upper_server（5968=5968，槽 [-56,-50,-48] vs
//    [-54,-52,-44]）、WarRoomCountAdjustByChannelInfo（60 vs 64）、CheckOutPvp（MIXED）
//    ⇒ N82/N115 族（分区规律差，非恒定平移）。
//  · 判据⑫（/tmp/widthscan.py）：无 clr_l/cmp_l 单向差命中（不涉类型宽度）。
//  · ① GetPvp(int,CUser*,int) **报告缺失**：docs/class_func_reports/CGameManager/
//    GetPvp.md 是无参重载 `_ZN12CGameManager6GetPvpEv`(0x0829507c)，非目标 ⇒ §3.2
//    兜底：nm 定址 `_ZN12CGameManager6GetPvpEiP5CUseri` @0x08295206 size 0x11c，
//    objdump 区间转写对比（81=81 insns）。NDIFF=24 绝大多数为 .o 未解析重定位
//    （call 标注）伪差；实质残差 = `user==0 ? "" : getCurCharacName()/get_acc_name()`
//    两处三目臂落点与 CSwitchLog ctor 实参装载序（P231/P277 寄存器-块序族）。
//  · ② CheckOutQuickParty（26/26）：唯一残差 = ORIG `xor$1; test%al; jmp; nop` 的
//    dead test（P225 4.1 bool 归一）缺席 + 我方 epilogue 前 `movzbl -0xc(%ebp),%eax`
//    （`return flag;` 归一）。ORIG 的 party==0 路径复用入口 `mov 0x10(%ebp),%eax`
//    残值（4.1 spill 保留寄存器），4.4 恒从槽零扩展 ⇒ 跨版本存活差，N 封顶。
//  · ③ WarRoomCountManageTest @+0x43..0x46：ORIG `mov -0x8c(%ebp,%eax,4),%edx;
//    add $1,%edx; mov %edx,-0x8c(%ebp,%eax,4)`（scaled 读改写）vs 我方多一条
//    `mov -0x48(%ebp),%edx` 中转 ⇒ N82/N86 族（4.4 槽分区）。
//  · ④ FindQuickJoinablePvPRoom @+0x27：ORIG `cmpb $0,-0x2c(%ebp); je`（内存直比）
//    vs 我方 `movzbl; xor $1; test; jne`（N122：4.4 bool 局部取反恒 xor）⇒ 封顶。
//  · ⑤ send_userinfos_to_cutoff_server @+0x60..：`% 10` 展开 ORIG 商留寄存器
//    （`shr $3,%edx; mov %edx,%eax; shl; add; add; sub`）vs 我方商落 -0x18 槽 + 两条
//    重载（+4 条）—— 4.4 除法/取模 lowering 中间值落槽，源形不可择 ⇒ N86b 族。
//  · ⑥ StaticPool<CTradeSpace,300>::~StaticPool **报告缺失**（game_func_report report=NO）
//    ⇒ §3.2 兜底：nm 定址 `_ZN10StaticPoolI11CTradeSpaceLi300EED1Ev` @0x082ae79c
//    **size 0x65（101B/37 insns）**，objdump 逐条转写 —— ORIG 体 = m_pNode 判空
//    (`mov this,%eax; mov (%eax),%eax; test; je +0x51`) → 逆序 `CNodeD1` 循环
//    (`imul $0xf78`步长, `je` 出口) → `_ZdaPv` → **fallthrough** `queue D1`
//    (`add $4,%eax; call ~queue`)，**全函数无 _Unwind_Resume / 无 EH 段**。
//    我方 50 insns/87B：主体逐条同构，多 = EH landing 块（`mov %edx,%ebx; mov %eax,%esi;
//    call ~queue; _Unwind_Resume`）+ 体尾 `jmp <epilogue>` + `push %esi`（共 13 条）
//    ⇒ **N91 封顶**（4.4 对 delete[] 元素析构循环恒布 cleanup；P216 边界明文：throw()
//    对 D1 侧无效、元素 dtro 加 throw() 亦无效；P284 全特化 throw() 仅适用于析构平凡 T
//    如 _IO_FILE）。
// ============================================================================
// [PoolA 2026-09-11 fresh 复核 · 清单 13 件（routed 11:56，本 TU 四数 729/1896/1/13，
//   missing=1 为 static_init 口径 N123）] 小 6 件逐件三态：
//   · WarRoomAlloc 0x08298a30 / user_exit 0x082985a8：**AE**（77/79、222/224，
//     ndiff 0；gap −2 为 4.4 outgoing 预留 N115）——清单 NEAR 系陈旧口径。
//   · CheckOutQuickParty 0x082983fc：**DIFF** 26/27 ndiff2。残差 = ORIG
//     `xor $1,%eax; test %al,%al; jmp <epilogue>` 的 dead test（P225 4.1 bool 归一
//     无消费者）+ party==0 沿入口 `mov 0x10(%ebp),%eax` 残值直落 epilogue；4.4 不发
//     dead test 且补 `movzbl -0xc(%ebp),%eax`（按槽零扩展）⇒ N7/P52 跨版本 spill 差
//     （`!call()`/`(bool)(call()^1)` 两形实测逐条相同，在册）⇒ 维持。
//   · GetPvp(int,CUser*,int) 0x08295206（报告缺失，§3.2 nm 兜底）：**DIFF** 81/81
//     ndiff4。残差 = 两处空串三目臂 `mov $<空串>,%esi/%ebx`（imm 直装 callee-saved）
//     vs 4.4 `mov $imm,%eax; mov %eax,%esi/%ebx` ⇒ N18a/N7（P244 具名局部向会加槽，
//     实测无用）⇒ 维持。
//   · returnUserPool 0x08294b2c：**DIFF** 148/149 ndiff2。残差 = err 跨 cMyTrace ctor
//     的 %esi 缓存（ORIG 0x08294cdf `mov -0xc(%ebp),%esi`）vs 4.4 槽重读（0x08294d17
//     `mov %esi,0x8(%esp)` ↔ 我方 `mov -0xc(%ebp),%eax; mov %eax,0x8(%esp)`）。
//     本波试 P226 裸语句表达式 `({ err; })`：**无效**（148/149 ndiff2 逐值不变，已回滚）
//     ⇒ 维持 N7/N82 封顶。
//   · WarRoomCountManage 0x082a271a：**NEAR** 311/312 ndiff16。残差 = 索引/room 槽
//     整体 +4（ORIG -0x48/-0x44/-0x40 vs 我方 -0x44/-0x40/-0x3c）+ 循环体调度；
//     P212 提级实验（在册）实测反劣 52→58 ⇒ N82 维持。
//   大 7 件（fresh，同日）：
//   · FindQuickJoinablePvPRoom 0x082a3202：**DIFF** 70/72 ndiff1，唯一残差 = `quick==0`
//     段 `cmpb $0x0,-0x2c(%ebp); je` vs 4.4 `movzbl; xor $1; test; jne` ⇒ N122
//     （跨版本 bool 取反 lowering；(int)quick==0 与 quick==0 两形实测逐条相同在册）。
//   · send_userinfos_to_cutoff_server 0x08295e84：**DIFF** 178/182 ndiff2，残差 = `%10`
//     展开：ORIG 0x08295f5f `shr $0x3,%edx` 商留寄存器 vs 4.4 商落 -0x18 槽再回读
//     （`mov %edx,%eax; shr $0x3,%eax; mov %eax,-0x18(%ebp); mov -0x18(%ebp),%edx` + 尾
//     `mov %edx,%eax; mov %eax,-0x18(%ebp)`，+4 条）⇒ N86b（4.4 除法中间值落槽）。
//   · CheckOutPvp 0x08297eaa：**DIFF** 212/211 ndiff36（N82 槽组分区 + N91/P285 岛）。
//   · Send_userinfos_to_upper_server 0x082961be：**DIFF** 304/302 ndiff34（N82/N115
//     帧 0x1764↔0x1750）。
//   · createUser 0x08294296：**DIFF** 126/124 ndiff28（N82：缺 push %edi + Guard 槽
//     −0x54↔−0x44）。
//   · insert_game_world 0x08296d30：**DIFF** 890/892 ndiff86（N82 大帧槽组 + N89；
//     p167 193/193 CLEAR 在册）。
//   · StaticPool<CTradeSpace,300>::~StaticPool 0x082ae79c：**DIFF** 37/50 ndiff5
//     （N91：delete[] 元素析构循环 EH landing 多 1×_Unwind_Resume；P216 边界）。
#include "CShutdowManager.h"   // GetInstanceShutdowManager 完整类型（SendLastMsgDBQueue 调用）
// [b28d-solo-CGameManager 本波判词（2026-09-09）] 清单 23 件收口（TU 编译绿、
// p167 逐符号核对：语义件全 CLEAR；N91 族 D1Ev/MoreAlloc 的 delete[] EH EXTRA 与
// CNodeC1 的 memset/C1Ev 见各自函数头判词）：
//   · 真语义 bug 2 处，均在 user_disconnect：①SendLastMsgDBQueue 是 if/else **共享
//     尾部**（ORIG 0x08298d54/0x08298da1 两路同落 0x08298da6；原形只在 state<3 分支
//     内 ⇒ state>=3 路径漏发）；②长链 CheckOutParty 第二实参 ORIG 为 true
//     （0x08298c25 `movl $0x1,0x8(%esp)`，原写 false）。
//   · 翻正 3 处：①send_userinfos_to_cutoff_server 写目标改
//     `buckets[bucketIdx].m_ids[bucket->m_count]` 重算基址（复现 ORIG 0x08295f87
//     `imul $0x259,%esi,%edx; add %ebx,%edx; mov %eax,-0x6904(%ebp,%edx,4)`）；
//     ②FindQuickJoinablePvPRoom 的 IsExistPassword 已由 Main 落 bool（PvP_Room.h:110，
//     T3 直证 0x082a3263）⇒ 裸 `!` 形复现 `call; xor $1; test; je`；
//     ③SendPartyList 局部 matching char→unsigned char（T1 直证 0x08296b88 movzbl）。
//   · 余件残差为 N 族：N82（槽组分区）/N91（delete[] EH cleanup）/N115（帧）/
//     N122（bool 取反 xor）/N7（三目 join 点）/P321-T5（lea-vs-add），已逐件在函数头
//     记 ORIG 地址与实测两形数据。
// [CGM4 2026-09-10 · L12 复核（判据⑩槽序 + ①jcc + ②视图 + p167 三件套）] 本波 15 件全核：
//   · p167 逐符号核对：**全部 CLEAR**。差异仅 C1↔C2 / D1↔D2 同址别名（P93 伪差集）；
//     `insert_game_world` 完整核对 193/193：`vector<pair<int,int>>::C1Ev/D1Ev ×3/×6`
//     ↔ ORIG 的 C2Ev/D2Ev 同址别名，**无真缺口**（旧判「EXTRA vector」已证伪）。
//   · 实测反劣并回滚 1 项：WarRoomCountAdjustByChannelInfo 的 dmW/b/a 三处纯 register
//     提示（P231/P307）⇒ 74→79 insns、帧 0x3c→0x4c（register 局部仍各占槽 + 多 mov
//     中转）⇒ 维持表达式形。
//   · 残余 N 族（ORIG 地址见各函数体判词）：N82 槽组分区 ×9（createUser 缺 push %edi +
//     Guard 槽 -0x54↔-0x44 @0x08294296；Send_userinfos_to_upper_server @0x082961be；
//     insert_game_world @0x08296d30；CheckOutPvp @0x08297eaa；WarRoomCountManage
//     @0x082a271a(+Test @0x082a2c08)；send_userinfos_to_cutoff_server @0x08295e84
//     （商 `shr $0x3,%edx` 留寄存器 vs 4.4 落槽 +4 条）；returnUserPool @0x08294b2c；
//     GetPvp @0x08295206（空串立即数落 %esi vs 4.4 经 %eax 中转））；
//     N115 帧 ×2（Send_userinfos_to_upper_server 帧 0x1764↔0x1750、N82/N115 同族）；
//     N7 寄存器角色 ×3（WarRoomAlloc @0x08298a30 / user_exit @0x082985a8 /
//     FindQuickJoinablePvPRoom @0x082a3202 的 `quick==0` cmpb↔xor，S1 实测两形逐条相同
//     = N122）；N107/P321-T5（WarRoomCountAdjustByChannelInfo flds+x87 槽往返）；
//     N91（StaticPool<CTradeSpace,300>::~StaticPool @0x082ae79c：delete[] 元素析构循环
//     的 EH landing，我方多 1×_Unwind_Resume；P216 边界= D1 侧对元素 ctor `throw()`
//     无效，等 L21CtorExchange 试验结论后复核）。
//   · 本波零头文件改动（无 R2 广播需求）。
// df_game_r CGameManager（G2 单例对象池/房间管理器）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/CGameManager.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 依赖的池化对象（CTradeSpace/PvP_Room/WarRoom/塔/祭坛/快速队伍/蓝弹珠）与
// 管理器类权威头属后续批次：本 TU 以尺寸垫 + 桩 ctor/dtor，跨类方法以
// asm-label extern 对齐 ORIG 符号（链接桩由主 agent / 后续批次提供）。
#include <string.h>
#include <time.h>

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "CAchievement.h"   // 先于 CGameManager.h：Quest.h 先定义 ENUM_CHARACTERJOB（STEquipmentScript.h 有 #ifndef GAME_QUEST_H_ 守卫）
#include "CTitleBook.h"
#include "CGameManager.h"
#include "CStreamGuard.h"
#include "CPrivateStoreMgr.h"  // StaticPool<private_store::CPrivateStore,300> 显式实例化需要完整类型
#include "CSystemTime.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "EpollHandler.h"
#include "StreamPool.h"
#include "MoreTypes.h"
#include "SigTypes.h"
#include "APSystemTypes.h"
#include "PacketGuard.h"
#include "CPrivateStoreMgr.h"

// ---- 跨类真实类型头（替代 extern "C" asm 桥；符号与 ORIG mangled 一致）----
#include "GameWorld.h"
#include "CDataManager.h"
#include "PvPTypes.h"   // BlueMarbleScriptManager/channel_script_t/channel_info_t 完整类型
#include "CMissionList_Charac.h"
#include "CEventCharacterHandler.h"
#include "CDungeon.h"
#include "CDungeonClearTracer.h"
#include "CLogGameChannel.h"
#include "AvatarItem.h"   // WongWork::Avatar_Item 权威 packed 定义（DynamicPool 显式实例化需完整类型）
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "CServerProxyMgr.h"
#include "QuickParty.h"
#include "CBossStage.h"
#include "CExpandEquipslot.h"
#include "CUserGlobalInfoHandle.h"
#include "HistoryLog.h"
#include "CSchoolMgr.h"
#include "CGuildAgitManager.h"
#include "CDoubleConnCheckServerProxy.h"
#include "Inter_MonitorGuildPointDel.h"
#include "DB_AccountCargoSync.h"
#include "Packet_ChannelType.h"
#include "Packet_Monitor_Char_Info.h"
#include "Packet_CutOff_UDP_Reply_UserInfo.h"
#include "Packet_Monitor_UDP_Reply_UserInfo.h"
#include "CraneMinigameManager.h"
#include "CQuickPartyRewardManager.h"
#include "CPremiumLetheManager.h"
#include "CConditionEventManager.h"
#include "CSpecialItemRoutingManager.h"
#include "CAuctionAveragePrice.h"
#include "CSharedServerMessageManager.h"

// ---- GlobalData / 全局单例（CUserCharacInfo.h 已有 namespace GlobalData，
//      与 GlobalData.h 的 class 声明互斥，本 TU 以 namespace extern 对齐符号）----
class MsgQueueMgr;
class StreamPool;
class CSystemTime;
class CMonitorServerProxy;
class CGuildServerProxy;
class CDoubleConnCheckServerProxy;
class CSchoolMgr;
class CGuildAgitManager;
class TimerQueue;
namespace WongWork { class CLogGameChannel; }
class CUserGlobalInfoHandle;
class CEventCharacterHandler;
class CShutdowManager;
template <class T> class CServerProxyMgr;
// [R18gd 2026-09-13] GlobalData 已按 ORIG 形态 class 化（namespace→class 静态成员）；
// 原此处 `namespace GlobalData { extern ... }` 最小声明重开点合并到权威头（唯一声明点）
// ——符号名/mangled 与 `GlobalData::x` 访问语法零变化。
#include "GlobalData.h"


// [SweepLeagueMatch4·判据㉔] g_schoolMgr / g_guildAgitMgr 定义已按 ORIG 归属迁出本 TU
// → CLeagueMatch.cpp（ORIG MatchingSystem.cpp static_init @0x085685a0 的构造点
// 0x085685e2 / 0x08568612 实证；本文件下方 extern 声明与各头内 extern 继续生效）。
// [A12-2026-09-10 · 铁律1 消 only-NEW「KEY_ABSENT」] 原发明符号
// `GlobalData::s_timerQueue_`（**ORIG nm 0 命中**）及其本文件冗余 extern 已删。
// ORIG 真形 = `GlobalInstance<TimerQueue>::m_p`（V 4B @0x93facc4）+
// inst/inst_ptr/create/destroy（W），由 TimerQueue.cpp 的 `G_TimerQueue()`
// （ORIG W 0x080f647c → `_ZN14GlobalInstanceI10TimerQueueE8inst_ptrEv`）实例化发射；
// 我方二进体现已含同名 `_ZN14GlobalInstanceI10TimerQueueE3m_pE`（4B, unique）
// ⇒ 无需任何本地替身。全仓 grep 实测该符号**仅本文件定义 + 本块 extern、零消费点**
// ⇒ 删除零语义影响（同时消去派生的 `_GLOBAL__I__ZN10GlobalData13s_timerQueue_E`
// 键名，GI 键名回退到本 TU 首个强符号=与 ORIG 同规则的候选）。


char* NumberToString(unsigned int value, int radix);
CUserGlobalInfoHandle* CUserGlobalInfoHandleInstance();
CEventCharacterHandler* CEventCharacterHandlerInstance();
CShutdowManager* GetInstanceShutdowManager();
EpollHandler* G_EpollHandler();
TimerQueue* G_TimerQueue();   // ORIG W 0x080f647c（P167：insert_game_world 四处定时器注册用它）
extern CSchoolMgr g_schoolMgr;
extern CGuildAgitManager g_guildAgitMgr;
#include "CGameManager/CheckDailyScheduleTime.inc"

// ============================================================================
// SIG_UPDATE_LINK_CHARAC_CONNECT_STATE（CStreamGuard::GetInBuffer 模板参数，
// ORIG mangled 名 36 字符；真实 struct 属后续批次）
// ============================================================================
struct SIG_UPDATE_LINK_CHARAC_CONNECT_STATE
{
    unsigned int m_accId;     // +0x00（ORIG *(info)=param_1=accId）
    unsigned int m_characNo;  // +0x04
    int m_action;             // +0x08（ENUM_CHARAC_LINK_ACTION_TYPE）
};

// PacketGuard 权威定义见 PacketGuard.h（InterfacePacketBuf 子类）。

// ============================================================================
// 外部依赖已全部换为真实头 include + 直接调用（约束12：不允许 extern C asm 桥）。
// ============================================================================

// ---- CSwitchLog（PvP 日志；实现见 CSwitchLog.cpp）----
class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int a, int b);
    void operator()(const char* fmt, ...);
    char m_pad[0x10];
};

// [b28d-Main·检查点17] StaticPool<T,N> 主模板成员定义已移入 StaticPool.h（模板定义必须
// 在实例化点可见；原「定义在此 + 19 条显式实例化」会为 19 个类型发射全部成员，
// 造成 66 个 ORIG 没有的 only-NEW 符号）。
// ============================================================================
// StaticPool<_IO_FILE, 50> 全特化成员定义（见 StaticPool.h 特化说明，Main 批准方案①）
// 体与主模板逐字相同，仅 N→50；差异仅在 CNode ctor 保留 throw()（_IO_FILE 析构平凡，
// ORIG 0x084ba31a 无 new[] EH cleanup）。
// ============================================================================
StaticPool<_IO_FILE, 50>::StaticPool()
{
    m_pNode = new CNode[50];
    if (m_pNode == 0)
    {
        // [K9GameRestA-S322 · §6.5 串盲区] ORIG 0x084ba3bf/0x084ba3e2：格式串
        // 0x8c86396 = "[%s][%d]"、vararg 第一参 = 0x8c869d0 = "StaticPool"（= __FUNCTION__）；
        // 原 "…new fail"/"StaticPool.cpp" 是发明文案（全库 `[StaticPool]` 0 次命中）。
        cMyTrace(__FUNCTION__, 0x6c, 5)("[%s][%d]", __FUNCTION__, 0x6c);
    }
    for (int i = 0; i < 50; ++i)
        m_Queue.push(&m_pNode[i]);
    m_nErrorCode = 0;
}

StaticPool<_IO_FILE, 50>::~StaticPool()
{
    delete[] m_pNode;
}

_IO_FILE* StaticPool<_IO_FILE, 50>::Acquire()
{
    if (m_Queue.empty())
        return 0;
    CNode* pNode = m_Queue.front();
    m_Queue.pop();
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        // [K9GameRestA-S322 · §6.5] ORIG 0x084ba4f1：0x8c863a0 =
        // "STATIC MEMORY_POOL Acquire error dummy(%x), node(%x)"（实参序 magic, pNode 不变）。
        cMyTrace(__FUNCTION__, 0x89, 0)("STATIC MEMORY_POOL Acquire error dummy(%x), node(%x)", magic, pNode);
        m_nErrorCode = 3;
    }
    if (pNode->bUse != 0)
    {
        register int use = pNode->bUse;
        // [K9GameRestA-S322 · §6.5] ORIG 0x084ba557：0x8c863d8 =
        // "STATIC MEMORY_POOL Acquire error reference count(%d), node(%x)"。
        cMyTrace(__FUNCTION__, 0x8f, 0)("STATIC MEMORY_POOL Acquire error reference count(%d), node(%x)", use, pNode);
        m_nErrorCode = 4;
    }
    pNode->bUse++;
    return pNode;
}

void StaticPool<_IO_FILE, 50>::Free(_IO_FILE* p)
{
    if (p == 0)
        // [K9GameRestA-S322 · §6.5] ORIG 0x084ba5d5：0x8c86396 = "[%s][%d]"（与 ctor 同串）。
        cMyTrace(__FUNCTION__, 0xa0, 5)("[%s][%d]", __FUNCTION__, 0xa0);
    CNode* pNode = (CNode*)p;
    m_Queue.push(pNode);
    if (pNode->magic != 0x8f21)
    {
        // [K9GameRestA-S322 · §6.5] ORIG 0x084ba64a：0x8c86418 =
        // "STATIC MEMORY_POOL Free error dummy(%x), node(%x)"。
        cMyTrace(__FUNCTION__, 0xaa, 0)("STATIC MEMORY_POOL Free error dummy(%x), node(%x)", pNode->magic, pNode);
        m_nErrorCode = 5;
    }
    if (pNode->bUse != 1)
    {
        // [K9GameRestA-S322 · §6.5] ORIG 0x084ba6b0：0x8c8644c =
        // "STATIC MEMORY_POOL Free error reference count(%d), node(%x)"。
        cMyTrace(__FUNCTION__, 0xb0, 0)("STATIC MEMORY_POOL Free error reference count(%d), node(%x)", pNode->bUse, pNode);
        m_nErrorCode = 6;
        PrintBackTrace();
    }
    pNode->bUse--;
}

// ============================================================================
// WongWork::CMailBox —— ORIG 布局（反汇编 0x8551d98/0x85579b2 核对）已于
// 2026-08-26 提升为 CMailBoxHelper.h 真实成员（m_bInit@+0x00 / m_lastLoadIdx@+0x04 /
// m_notLoadedMailCount@+0x08 / m_mailPool@+0x10 / m_mailMap@+0x40 / m_streamList@+0x58 /
// m_mailIterator@+0x60 / m_letterKeepCount@+0x64 / m_lastLoadLetterIdx@+0x7c /
// m_loadedLetterCount@+0x80 / m_loadLackList@+0x84 / m_letterInfo@+0x90，
// sizeof 0x100 断言守 ABI）；本 TU 各方法直接以具名成员访问。
// ---- StaticPool 元素类型完整布局（ORIG CACHE_CHARACTER_TYPE::C1 0x8492262）----
// 子对象顺序与偏移：
//   SIG_LOAD_CHARAC@+0x00000 / SIG_LOAD_INVENTORY@+0x00703 / SIG_LOAD_QUEST@+0x15b4f /
//   APSystem::_SIG_LOAD_ACTION_POINT@+0x1eb13 / _CharacAdvanceAltarDbData@+0x1f804 /
//   SIG_LOAD_ETC@+0x1fd20 / SIG_LOAD_KILL_MONSTER_INFO@+0x34884。
// ORIG 总尺寸 0x3f85c（AcquireCharacter memset 0x3f85c + CacheCharacterMemory
// memcpy(local_10+0x3f850, this+0x8d0fd, 0xc) 证据）。+0x34880 为缓存时间戳
// （AcquireCharacter 写 time(0)）。
// 注：本类为 ORIG 权威成员布局（GetMemberPosition@0x848fd7c 逐 ENUM 返回 offset/size；
//   C1@0x8492262 按子对象顺序构造；AcquireCharacter memset 0x3f85c + CacheCharacterMemory
//   memcpy(+0x3f850, this+0x8d0fd, 0xc) 定总尺寸 0x3f85c）。各成员区以具名 char 数组承载
//   ORIG 精确尺寸，避免与权威 SIG_LOAD_* 类型尺寸（并发改型中）耦合；CacheCharacterMemory
//   仍以 char* 偏移算术把子对象指针传给 GetCharacInfo/GetInventoryInfo/GetEtcInfo 等。
#include "CacheCharacterType.h"   // CACHE_CHARACTER_TYPE 权威布局（2026-08-27 提取为独立头）



#include "CGameManager/CMailBox.inc"


#include "CGameManager/dtor_CMailBox.inc"


#include "CGameManager/Init.inc"

#include "CGameManager/SetLastLoadLetterIdx.inc"


#include "CGameManager/SetPackageLoadLack.inc"

#include "CGameManager/SetLoadedLetterCount.inc"

#include "CGameManager/SetNotLoadedMailCount.inc"

#include "CGameManager/getNextMail.inc"

#include "CGameManager/incMailLoadCount.inc"

#include "CGameManager/reset.inc"

#include "CGameManager/setMailIterator.inc"

// void WongWork::CDungeonClear::clear() 已由 CDungeonClear.cpp 提供（合并移除）


#include "CGameManager/dtor_CEgg.inc"

#include "CGameManager/dtor_CCreature.inc"

#include "CGameManager/CNode.inc"

#include "CGameManager/dtor_CNode.inc"

#include "CGameManager/CNode_1.inc"

#include "CGameManager/dtor_CNode_1.inc"


// ============================================================================
// 显式实例化（本 TU 池成员使用）
// ============================================================================
// [b28d-Main·检查点17] 原 19 条 `template class StaticPool<...>;` 显式实例化已删：
//   定义移入 StaticPool.h 后按使用点隐式实例化（ORIG 模型），否则会为 19 个类型
//   发射全部成员 ⇒ 66 个 ORIG 没有的 only-NEW 符号。
//   CACHE_CHARACTER_TYPE 的 CNodeC1 判词（N 封顶）保留于 StaticPool.h CNode 区。
//   StaticPool<_IO_FILE, 50> 为全特化（定义仍在本文件下方）。

// DynamicPool 显式实例化
// [b28d-Main·检查点18] 4 条 DynamicPool 显式实例化已删（与 StaticPool 同型问题）：
//   方法定义均在 GameTypes.h（实例化点可见）⇒ 按使用点隐式实例化即与 ORIG 一致；
//   显式实例化会为 4 个类型发射全部成员（IsValidPtr/SetPoolSize/双 ctor），
//   而 ORIG 只有 Stream 族用 IsValidPtr/SetPoolSize、CEgg/CCreature 仅用 C1Ei、
//   Avatar_Item 仅用 C1Eii ⇒ 多出 15 个 only-NEW 符号。
// b28d-solo-CGameManager 判词（DynamicPool<Avatar_Item>::MoreAlloc，ORIG 0x0843bxxx，
// 103 vs 129）：N91 封顶——ORIG `new CNode[n]` 无 EH landing（p167 差集 ORIG 7 calls，
// 我方 EXTRA `CNodeD1Ev`+`_ZdaPv`+`_Unwind_Resume` 三件，4.4 对元素 dtor 非平凡数组
// 恒布回滚段；P216 边界：D1 侧 throw() 无效，已实证）。

// ============================================================================
// CGameManager 实现
// ============================================================================
CGameManager* G_CGameManager()
{
    return GlobalInstance<CGameManager>::inst_ptr();
}
// 显式实例化补齐 ORIG W 弱符号 GlobalInstance<CGameManager>::inst()/destroy()
//（ORIG 0x82ab72e/0x82ab740；GameMain.cpp 的 CREATE/DESTROY_CGameManager 调用）。
template class GlobalInstance<CGameManager>;

#include "CGameManager/init.inc"


#include "CGameManager/getNextUID.inc"


#include "CGameManager/GetIdx.inc"


#include "CGameManager/GetUserByAccId.inc"


#include "CGameManager/FindUserByUnique.inc"


#include "CGameManager/operator.inc"


#include "CGameManager/getUserByUnique.inc"


#include "CGameManager/getUser.inc"


#include "CGameManager/insertUserByAccID.inc"


#include "CGameManager/isConnectUserByAccId.inc"


#include "CGameManager/createUser.inc"

#include "CGameManager/check_user_var.inc"


#include "CGameManager/returnUserPool.inc"


#include "CGameManager/GetParty.inc"


#include "CGameManager/GetParty_1.inc"


#include "CGameManager/GetPartyIdx.inc"


#include "CGameManager/PutParty.inc"


#include "CGameManager/CheckOutParty.inc"


#include "CGameManager/CheckOutQuickParty.inc"


#include "CGameManager/GetTradeSpace.inc"


#include "CGameManager/GetTradeSpace_1.inc"


#include "CGameManager/GetTradeIdx.inc"


#include "CGameManager/PutTradeSpace.inc"


#include "CGameManager/CheckOutTrade.inc"


#include "CGameManager/GetPvp.inc"


#include "CGameManager/GetPvp_1.inc"


#include "CGameManager/GetPvpIdx.inc"


#include "CGameManager/PutPvp.inc"


#include "CGameManager/CheckOutPvp.inc"


#include "CGameManager/FindQuickJoinablePvPRoom.inc"


#include "CGameManager/GetQuickParty.inc"


#include "CGameManager/GetQuickParty_1.inc"


#include "CGameManager/GetQuickPartyIdx.inc"


#include "CGameManager/PutQuickParty.inc"


#include "CGameManager/GetWarRoom.inc"


#include "CGameManager/GetWarRoom_1.inc"


#include "CGameManager/GetWarRoomIdx.inc"


#include "CGameManager/PutWarRoom.inc"


#include "CGameManager/CheckOutWarRoom.inc"


#include "CGameManager/FindJoinableWarRoom.inc"


#include "CGameManager/getDeathTower.inc"


#include "CGameManager/getDeathTower_1.inc"


#include "CGameManager/getDeathTowerIdx.inc"


#include "CGameManager/returnDeathTower.inc"


#include "CGameManager/checkOutDeathTower.inc"


#include "CGameManager/getBossTower.inc"


#include "CGameManager/getBossTower_1.inc"


#include "CGameManager/getBossTowerIdx.inc"


#include "CGameManager/returnBossTower.inc"


#include "CGameManager/checkOutBossTower.inc"


#include "CGameManager/getAdvanceAltar.inc"


#include "CGameManager/getAdvanceAltar_1.inc"


#include "CGameManager/getAdvanceAltarIndex.inc"


#include "CGameManager/returnAdvanceAltar.inc"


#include "CGameManager/checkOutAdvanceAltar.inc"


#include "CGameManager/onTimeAdvanceAltar.inc"


#include "CGameManager/getBlueMarble.inc"


#include "CGameManager/getBlueMarble_1.inc"


#include "CGameManager/getBlueMarbleIdx.inc"


#include "CGameManager/putBlueMarble.inc"


#include "CGameManager/checkOutBlueMarble.inc"


#include "CGameManager/allocBlueMarble.inc"


#include "CGameManager/findJoinableBlueMarble.inc"


#include "CGameManager/SendPartyList.inc"


#include "CGameManager/SendPvpList.inc"


#include "CGameManager/SendWarRoomList.inc"


#include "CGameManager/SendWarRoomToAll.inc"


#include "CGameManager/PrintWarRoomList.inc"


#include "CGameManager/WarRoomAlloc.inc"


#include "CGameManager/WarRoomCountAdjustByChannelInfo.inc"


#include "CGameManager/WarRoomCountManage.inc"


#include "CGameManager/WarRoomCountManageTest.inc"


#include "CGameManager/updateDBLinkCharacConnectState.inc"


#include "CGameManager/IsNoGameGuardApplingUser.inc"


#include "CGameManager/GetQuickPartySystemManager.inc"


#include "CGameManager/GetQuickPartyRewardManager.inc"


#include "CGameManager/GetPremiumLetheManager.inc"


#include "CGameManager/GetSharedServerMessageManager.inc"


#include "CGameManager/GetSpecialItemRoutingManager.inc"


#include "CGameManager/GetConditionEventManager.inc"


#include "CGameManager/GetAuctionAveragePriceManager.inc"


#include "CGameManager/GetCraneMinigameManager.inc"


// ============================================================================
// 用户信息上报
// ============================================================================

#include "CGameManager/SendUserInfoToUpperServer.inc"




#include "CGameManager/Send_userinfos_to_upper_server.inc"


#include "CGameManager/send_userinfos_to_cutoff_server.inc"


// ============================================================================
// 进入游戏世界
// ============================================================================
enum ENUM_HARDCODE_ITEM
{
    ENUM_HARDCODE_ITEM_0 = 0
};

#include "CGameManager/insert_game_world.inc"


#include "CGameManager/user_disconnect.inc"


#include "CGameManager/user_exit.inc"


#include "CGameManager/CGameManager.inc"

#include "CGameManager/dtor_CGameManager.inc"

