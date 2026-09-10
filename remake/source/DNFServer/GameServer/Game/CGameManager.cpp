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
namespace GlobalData
{
extern MsgQueueMgr* s_msgq_mgr;
extern StreamPool* s_stream_pool;
extern CServerProxyMgr<CMonitorServerProxy>* s_monitor_proxy_mgr;
}
namespace GlobalData
{
extern CServerProxyMgr<CGuildServerProxy>* s_guild_proxy_mgr;
extern CDoubleConnCheckServerProxy* s_double_check_proxy;
extern WongWork::CLogGameChannel* s_pLogGameChannel;
TimerQueue* s_timerQueue_;  // ORIG 全局（GlobalData::s_timerQueue_），原 GameStubs.cpp 定义迁移
}
// [SweepLeagueMatch4·判据㉔] g_schoolMgr / g_guildAgitMgr 定义已按 ORIG 归属迁出本 TU
// → CLeagueMatch.cpp（ORIG MatchingSystem.cpp static_init @0x085685a0 的构造点
// 0x085685e2 / 0x08568612 实证；本文件下方 extern 声明与各头内 extern 继续生效）。
namespace GlobalData
{
extern TimerQueue* s_timerQueue_;
}


char* NumberToString(unsigned int value, int radix);
CUserGlobalInfoHandle* CUserGlobalInfoHandleInstance();
CEventCharacterHandler* CEventCharacterHandlerInstance();
CShutdowManager* GetInstanceShutdowManager();
EpollHandler* G_EpollHandler();
TimerQueue* G_TimerQueue();   // ORIG W 0x080f647c（P167：insert_game_world 四处定时器注册用它）
extern CSchoolMgr g_schoolMgr;
extern CGuildAgitManager g_guildAgitMgr;
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime);

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
        cMyTrace(__FUNCTION__, 0x6c, 5)("[StaticPool] %s(%d) new fail", "StaticPool.cpp", 0x6c);
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
        cMyTrace(__FUNCTION__, 0x89, 0)("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 3;
    }
    if (pNode->bUse != 0)
    {
        register int use = pNode->bUse;
        cMyTrace(__FUNCTION__, 0x8f, 0)("[StaticPool] bUse(%d) node(%p)", use, pNode);
        m_nErrorCode = 4;
    }
    pNode->bUse++;
    return pNode;
}

void StaticPool<_IO_FILE, 50>::Free(_IO_FILE* p)
{
    if (p == 0)
        cMyTrace(__FUNCTION__, 0xa0, 5)("[StaticPool] %s(%d) free null", "StaticPool.cpp", 0xa0);
    CNode* pNode = (CNode*)p;
    m_Queue.push(pNode);
    if (pNode->magic != 0x8f21)
    {
        cMyTrace(__FUNCTION__, 0xaa, 0)("[StaticPool] magic(%d) node(%p)", pNode->magic, pNode);
        m_nErrorCode = 5;
    }
    if (pNode->bUse != 1)
    {
        cMyTrace(__FUNCTION__, 0xb0, 0)("[StaticPool] bUse(%d) node(%p)", pNode->bUse, pNode);
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



WongWork::CMailBox::CMailBox()
{
    // ORIG 0x085579b2：按布局顺序构造 7 个子对象成员；现均为真实成员，由
    // 编译器按声明序（= ORIG 构造序 pool→map→list→iter→keep→vector→map）生成。
}

WongWork::CMailBox::~CMailBox()
{
    // ORIG 0x08557aac：逆序析构 7 个子对象成员；现由编译器生成（逆声明序）。
}

void WongWork::CMailBox::Init()
{
	// ORIG 0x08551d98：m_bInit==0 早退；否则遍历 m_mailMap 重置并释放邮件、
	// 清空各容器与计数。迭代器形态：it 默认构造 + end 直构缓存 + begin 经临时
	// 拷贝（P28）；循环为 while（前置 jmp）。
	if (!m_bInit)
		return;
	CMail* mail;   // ORIG 槽 -0xc（先于迭代器声明；P28 声明序即栈槽序）
	std::map<unsigned int, CMail*>::iterator it;
	std::map<unsigned int, CMail*>::iterator endIt = m_mailMap.end();
	it = m_mailMap.begin();
	while (it != endIt)
	{
		mail = it->second;
		mail->ResetPackage();   // ORIG call CMail::ResetPackage（0x085519fe）
		m_mailPool.Free(mail);
		++it;
	}
	m_mailMap.clear();
	m_bInit = false;
	m_lastLoadIdx = 0;
	m_notLoadedMailCount = 0;
	if (!m_streamList.empty())
	{
		std::list<Stream*>::iterator sit = m_streamList.begin();
		std::list<Stream*>::iterator send = m_streamList.end();
		while (sit != send)
		{
			// ORIG 无 Stream* 局部、无判空：*it 直取即 Free（mov (%eax),%edx）。
			GlobalData::s_stream_pool->Free(*sit);
			++sit;
		}
	}
	m_streamList.clear();
	m_letterKeepCount.clear();
	m_lastLoadLetterIdx = 0;
	m_loadedLetterCount = 0;
	m_loadLackList.clear();
		m_letterInfo.clear();
}
void WongWork::CMailBox::SetLastLoadLetterIdx(unsigned int idx)
{
	// ORIG 0x08557858：m_lastLoadLetterIdx(+0x7c) = idx 直写。
	m_lastLoadLetterIdx = idx;
}

void WongWork::CMailBox::SetPackageLoadLack(const unsigned int* arr, int count,
                                            std::set<unsigned int>& oldLack)
{
	// ORIG 0x08557676：先把旧列表（m_loadLackList，+0x84）全部并入 oldLack，
	// 清空列表；随后把新数组逐项 push_back，并把在 oldLack 中出现者从中移除。
	// 迭代器形态：vector iterator 直构 begin，end() 在循环条件内每轮调用（非缓存）。
	std::vector<unsigned int>::iterator vit = m_loadLackList.begin();
	while (vit != m_loadLackList.end())
	{
		oldLack.insert(*vit);
		++vit;
	}
	m_loadLackList.clear();
	for (int i = 0; i < count; ++i)
	{
		m_loadLackList.push_back(arr[i]);
		std::set<unsigned int>::iterator sit = oldLack.find(arr[i]);
		if (sit != oldLack.end())
			oldLack.erase(sit);
	}
}
void WongWork::CMailBox::SetLoadedLetterCount(int count)
{
	// ORIG 0x085789c：m_loadedLetterCount(+0x80) = count 直写。
	m_loadedLetterCount = count;
}
void WongWork::CMailBox::SetNotLoadedMailCount(int count)
{
	// ORIG 0x08557914：m_notLoadedMailCount(+0x08) = count 直写。
	m_notLoadedMailCount = count;
}
WongWork::CMailBox::CMail* WongWork::CMailBox::getNextMail()
{
    // ORIG 0x0855236a：迭代器指向 m_mailMap；未到 end 则返回 second 并前移，
    // 否则返回 0。
    if (m_mailIterator == m_mailMap.end())
        return 0;
    CMail* mail = m_mailIterator->second;
    ++m_mailIterator;
    return mail;
}
void WongWork::CMailBox::incMailLoadCount()
{
	// ORIG 0x085575dc：m_mailLoadCount(+0x0c) 自增，超过 0x7ffffffe 归零
	//（cmp $0x7ffffffe; jbe 无符号判定）。
	m_mailLoadCount = m_mailLoadCount + 1;
	if ((unsigned int)m_mailLoadCount > 0x7ffffffeU)
		m_mailLoadCount = 0;
}
void WongWork::CMailBox::reset()
{
	// ORIG 0x08551f4a：仅清 m_mailLoadCount(+0x0c) 一个 dword。
	m_mailLoadCount = 0;
}
void WongWork::CMailBox::setMailIterator()
{
    // ORIG 0x0855232e：迭代器 = m_mailMap.begin()
    m_mailIterator = m_mailMap.begin();
}
// void WongWork::CDungeonClear::clear() 已由 CDungeonClear.cpp 提供（合并移除）


// ============================================================================
// DynamicPool<CEgg/CCreature>::CNode —— 多态 T 的 CNode vptr 重写（显式特化）
// ============================================================================
// ORIG CNodeC1/D1（df_game_r）：基类 T() 之后先把 vptr 改写为 CNode 自身虚表
// （CEgg CNode `_ZTVN11DynamicPoolIN13user_creature4CEggEE5CNodeE@0x08c2f470`+8
//  =0x08c2f478；CCreature CNode `@0x08c2f488`+8=0x08c2f490），再写 magic/bUse。
// 真虚建模不可行（实证：-O0 true-virtual 的 vptr 写为 `movl $0x8`+R_386_32 重定位，
// 恒为小偏移而非硬编 0x8c2f478；AE 口径不归一立即数）→ 手动常量按 ORIG 槽序写回。
// 基类 dtor 外联定义必须出现在 CNode 特化之前（P210 帧继承实证：callee out-of-line
// 定义先可见 ⇒ caller sub $0x4=ORIG；仅声明 ⇒ 4.4 保守 sub $0x18；探针 /tmp/bm3/p1-p10）。
user_creature::CEgg::~CEgg()
{
    // [b28d-Main·检查点14] vptr 复位由编译器发射（ORIG 0x08338f74 同形，探针 vt_probe.cpp）。
}
user_creature::CCreature::~CCreature()
{
    // [b28d-Main·检查点14] 同上（ORIG 0x83370a8）。
}
template <> DynamicPool<user_creature::CEgg>::CNode::CNode()
{
    m_type = 0x8f21;
    m_inUse = 0;
}
template <> DynamicPool<user_creature::CEgg>::CNode::~CNode()
{
}
template <> DynamicPool<user_creature::CCreature>::CNode::CNode()
{
    m_type = 0x8f21;
    m_inUse = 0;
}
template <> DynamicPool<user_creature::CCreature>::CNode::~CNode()
{
}

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

bool CGameManager::init()
{
    m_pCraneMinigameMgr = new CraneMinigameManager;
    //（ORIG 0x08294231..：`call init; xor $0x1,%eax; test %al,%al; je mov1 /
    // 非零→mov 0` 即 return (r==1)（真值归一，非取反——原实现 !r 语义写反，
    // init 失败/成功与调用方判定相反，真实 BUG）。X1 形复现 xor 直落+0/1 物化。
    // N96/P0：禁 register 绑定。无槽折叠形（probe /tmp/l7_h.cpp h2）：
    // 4.4 产 `cmp$1,%al; setne; test; je` 直比寄存器，残差 xor vs cmp+setne
    // 2 行（setcc-fold 负例在册）；槽化 X2 会污染 frame（f1/f3 实测更差）。
    if ((((unsigned char)GetCraneMinigameManager()->init()) ^ 1) != 0)
        return 0;
    return 1;
}

short CGameManager::getNextUID()
{
    m_uid++;  // ORIG movzwl+lea 0x1(%eax),%edx+mov %dx 单装载形态（探针 t5）
    unsigned short v = m_uid;
    if (m_uid == 0x7fff || m_uid < 0)
        m_uid = 0;
    return (short)v;
}

int CGameManager::GetIdx(CUser* user)
{
    int idx = m_userPool.GetIndex(user);
    return idx;
}

CUser* CGameManager::GetUserByAccId(unsigned int accId)
{
    // b28d-solo-CGameManager: ORIG 0x082948c6 早返链（P16/P222 正极性）：
    //   it==end → `mov $0,%eax; jmp epilogue`；get_acc_id()==accId → 直返 user；
    //   不等 → cMyTrace 临时链后 `mov $0,%eax` 落 epilogue。原 `user=0` 槽形
    //   会产 `movl $0,-0xc` 中转（P167 已 CLEAR，本修为控制流/槽形）。
    std::map<unsigned int, CUser*>::iterator it;
    it = m_userByAccId.find(accId);
    if (it == m_userByAccId.end())
        return 0;
    CUser* user = it->second;
    if (user->get_acc_id() == accId)
        return user;
    // P227 全表达式临时链（N80 突破口）——ORIG 0x0829499e 在 cMyTrace ctor 前
    // 右起求 varargs（mapId 先入 %esi、inId 后入 %ebx）并跨 ctor 保活。
    cMyTrace(__PRETTY_FUNCTION__, 0x9ab, 5)(
        "[%s][%d][IN m_id: %s][Map m_id: %s]",
        __PRETTY_FUNCTION__, 0x9ab,
        NumberToString(accId, 0), NumberToString(user->get_acc_id(), 1));
    return 0;
}

CGameManager::FindUserByUnique::FindUserByUnique(short uniqueId)
{
    m_uniqueId = uniqueId;
}

bool CGameManager::FindUserByUnique::operator()(
    std::pair<const unsigned int, CUser*>& p)
{
    if (p.second != 0 &&
        (unsigned short)p.second->get_unique_id() == (short)m_uniqueId)
        return true;
    return false;
}

CUser* CGameManager::getUserByUnique(short uniqueId)
{
    std::map<unsigned int, CUser*>::iterator it;
    it = std::find_if(m_userByAccId.begin(), m_userByAccId.end(),
                      FindUserByUnique(uniqueId));
    if (it != m_userByAccId.end())
        return it->second;
    return 0;
}

CUser* CGameManager::getUser(int slotId, int increId)
{
    // b28d-lane12: ORIG 0x082947a4 实证——Guard 作用域为**块**（0x082947ee 析构
    // 先于 0x082947f3 `cmpl $0,-0x10` 判空）；else 臂为早返回链（incre==0→0、
    // incre==id→user、冗余 incre==0→user（P171 照抄）、id==0→user、否则 0）。
    // b28d-solo-CGameManager 判词：尾链已改三目（P47）；gap 0（74=74），余差为
    // 三处 je 落点 +0xec/+0xec/+0xf1 vs ORIG 同落 +0xf3（4.4 对末项恒反转 jne）→ N82附。
    short slot = (short)(slotId >> 0x10);
    short id = (short)slotId;
    CUser* user;
    {
        Guard<Mutex> guard(&m_mutex);
        user = m_userPool.Get((int)slot);
    }
    if (user == 0)
    {
        // P37/P227：ORIG 0x08294816 `movswl -0x14(%ebp),%ebx` 在 ctor 前物化
        // slot 并跨 ctor 保活 ⇒ 全表达式临时链形（具名 tr 对象在实参处才读）。
        cMyTrace(__PRETTY_FUNCTION__, 0x95c, 5)(
            " No way! getUser , slot_id(%d) is abnormal!", (int)slot);
        return 0;
    }
    short incre = user->GetIncreID();
    if (incre == 0)
        return 0;
    // b28d-solo-CGameManager 判词：AE（P47 镜像翻正）。ORIG 0x0829487c 三处
    // `je <ret_user>` **同一落点**（+0xf3 `mov -0x10(%ebp),%eax`），false 路径
    // 直落 `mov $0,%eax; jmp` ⇒ 源码是**否定守卫 + 尾 return**（`if(!(...)) return 0;
    // return user;`），4.4 逐条复现（探针 /tmp/solo_probe1.cpp f1/f2，4.4.6）。
    // 三目形 `? user : 0` 块序相反（ret_user 前置，三处 je 落 +0xec/+0xec、末项
    // 反转 jne）——S154 判 N82附 系源形未穷举，本波翻正。
    if (!(incre == id || incre == 0 || id == 0))
        return 0;
    return user;
}

void CGameManager::insertUserByAccID(CUser* user)
{
    unsigned int accId = user->get_acc_id();
    m_userByAccId[accId] = user;
    unsigned int accId2 = user->get_acc_id();
    if (accId2 == 0)
    {
        // b28d-lane12: 试 P227 全表达式临时链（N80 突破口）——ORIG 0x082945xx 在
        // cMyTrace ctor call 前取 get_acc_name 值并跨 ctor 保活 %ebx。
        cMyTrace(__PRETTY_FUNCTION__, 0x91d, 5)(
            "[QQID_ERROR]CGameManager::insertUserByAccID  qq_id:%s", user->get_acc_name());
    }
    else
    {
        m_userByAccId2[accId2] = user;
        GlobalData::s_pLogGameChannel->IncInUser();
    }
}

bool CGameManager::isConnectUserByAccId(unsigned int accId)
{
    for (int i = 0; i < 600; ++i)
    {
        CUser* user = m_userPool.Get(i);
        if (user->get_acc_id() == accId)
            return 1;
    }
    return 0;
}

CUser* CGameManager::createUser()
{
    // ORIG 0x08294296：Guard<Mutex> 仅护住 Acquire（析构紧随其后），随后才判
    // user==0 → 空则 cMyTrace(0x8e3,level5)"createUser Failed! need to restart!"
    // 并返 0；非空走 else 正常路径（原实现误删 null 分支且把 guard 放整函数作用域）。
    // b28d-solo-CGameManager 判词：N82附——ORIG `push edi/esi/ebx; sub $0x6c`（Guard
    // 对象 -0x54）vs 4.4 `push esi/ebx; sub $0x70`（Guard -0x2c）：ORIG 多 edi 且
    // Guard 槽深 0x28，系 4.4 具名标量/类临时分区分配（P112/N82）不可控。
    CUser* user;
    {
        Guard<Mutex> guard(&m_mutex);
        user = m_userPool.Acquire();
    }
    if (user == 0)
    {
        // b28d-laneCGM（P227 全表达式临时链）：ORIG 该处 op() 实参在 ctor 前求值/直落
        //   寄存器（具名 tr 形多一次对象槽 + ctor 后读）。
        cMyTrace(__PRETTY_FUNCTION__, 0x8e3, 5)("createUser Failed! need to restart!");
        return 0;
    }
    unsigned int uid = user->GetUID();
    unsigned int accId = user->get_acc_id();
    check_user_var(user);
    int idx = GetIdx(user);
    user->SetSlotIDX((short)idx);
    // b28d-laneCGM S1 实测（2026-09-10）：`register int err` 反劣（ndiff 36→48、gap +2→+1）
    //   ⇒ 回滚裸形；ORIG 的 `mov -0x20(%ebp),%edi` 属 4.1/4.4 寄存器分配差（N7），
    //   不满足 P248 裸 register 四条件，封顶。
    int err = m_userPool.GetLastErrorCode();
    if (err != 0)
    {
        // ORIG 0x082943a8/0x082943bd：GetUID/NumberToString 结果入 %esi/%ebx 跨 ctor
        // 保活（无具名槽）⇒ 实参内联（P329/P227）。
        cMyTrace(__PRETTY_FUNCTION__, 0x8f6, 0)(
            "STATIC MEMORY POOL createUser error(%d), user(%x), last user m_acc_id(%s), idx(%d), new user idx(%d)",
            err, user, NumberToString(accId, 0), uid, user->GetUID());
    }
    short nxt = getNextUID();
    user->SetIncreID(nxt);
    // ORIG 0x08294441：movzwl %ax,%eax 直传（无具名 short 槽）。
    user->set_unique_id(CUserGlobalInfoHandleInstance()->get_uniqueid());
    return user;
}
void CGameManager::check_user_var(CUser* user)
{
    // ORIG：直接 && 条件（无中间 bool 变量）。
    if (user->get_charac_guildkey() != 0 && user->get_guild_exp() != 0)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x92c,
                              "[CHECK_USER_VAR] RESET GUILD EXP ERROR!");
    }
    if (user->get_charac_level_before_dungeon() != 0)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x92f,
                              "[CHECK_USER_VAR] RESET CHARACTER LEVEL BEFORE DUNGEON ERROR!");
    }
    if (user->CheckInParty())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x932,
                              "[CHECK_USER_VAR] RESET PARTY ERROR!");
    }
    if (user->CheckInTrade())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x935,
                              "[CHECK_USER_VAR] RESET TRADE INDEX ERROR!");
    }
    if (user->CheckInPvp())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x938,
                              "[CHECK_USER_VAR] RESET PVP INDEX ERROR!");
    }
    if (user->CheckInWarRoom())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x93b,
                              "[CHECK_USER_VAR] RESET WARROOM INDEX ERROR!");
    }
    if (user->getPlayingDungeonIndex() != -1)
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x93e,
                              "[CHECK_USER_VAR] RESET DUNGEON ERROR!");
    }
    if (user->IsProgLogout())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x941,
                              "[CHECK_USER_VAR] RESET PROG LOGOUT ERROR!");
    }
    if (user->m_characterView.isSaveCharacView())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x944,
                              "[CHECK_USER_VAR] RESET CHARACTER VIEW ERROR!");
    }
    if (user->checkInBlueMarble())
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x948,
                              "[CHECK_USER_VAR] RESET BLUEMARBLE INDEX ERROR!");
    }
}

void CGameManager::returnUserPool(CUser* user)
{
    // b28d-lane12: N82 槽平移封顶（orig=147 our=147 gap=0；ndiff 102 全为槽号——
    // Guard/cMyTrace/map 迭代器槽组 4.4 分区 vs 4.1 生命周期穿插）。ORIG 0x08294b2c。
    // b28d-solo-CGameManager 判词：N82 维持（P167 CLEAR，gap 0；残差全为槽号：
    // find sret -0x28 vs -0x34、accId -0x24 vs -0x30）。
    // [SweepCGameManager2 本轮] lastId 具名删 → P227 全表达式临时链，复现 ORIG
    //   0x08294cd8 NumberToString 结果入 %ebx 跨 cMyTrace ctor 保活；残余 =
    //   err 跨 ctor 的 %esi 缓存（ORIG `mov -0xc,%esi`）4.4 恒槽重读（裸 register
    //   实测反偏离 ORIG，已回滚）+ 尾部 `jmp epilogue` 岛（P285）⇒ N82/P52。
    // ORIG 0x08294b38：`cmpl $0x0,0xc(%ebp); je 08294d30`（08294d30 是 nop 空岛，
    // 08294d31 才是 epilogue）⇒ 裸早返形复现尾 nop 岛（P223 变体）。
    if (user == 0)
        return;
    {
        unsigned int accId = user->get_acc_id();
        std::map<unsigned int, CUser*>::iterator it = m_userByAccId.find(accId);
        if (it != m_userByAccId.end())
        {
            m_userByAccId.erase(it);
            GlobalData::s_pLogGameChannel->IncOutUser();
        }
        unsigned int accId2 = user->get_acc_id();
        std::map<unsigned int, CUser*>::iterator it2 = m_userByAccId2.find(accId2);
        if (it2 != m_userByAccId2.end())
        {
            m_userByAccId2.erase(it2);
        }
        unsigned int uid = user->GetUID();
        unsigned int accId3 = user->get_acc_id();
        user->log_out();
        // ORIG 0x08294c71..0x08294ca8：Guard 在独立作用域，Free 后立即 ~Guard
        // （0x08294ca8），随后才 GetLastErrorCode ⇒ 不能与 err 同作用域。
        {
            Guard<Mutex> guard(&m_mutex);
            m_userPool.Free(user);
        }
        // ORIG 0x08294cc1..0x08294cd2：err 入槽后 `mov -0xc(%ebp),%esi` 跨
        // cMyTrace ctor 保活；4.4 恒从槽重读（裸 register 实测 146→145 反偏离
        // ORIG，已回滚）⇒ N82/P52 封顶。
        int err = m_userPool.GetLastErrorCode();
        if (err != 0)
        {
            // ORIG 0x08294cd8..0x08294d00：NumberToString 结果入 %ebx 跨 cMyTrace
            // ctor 保活（无具名槽）⇒ P227 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0xa0e, 0)(
                "STATIC MEMORY_POOL returnUserPool error(%d), user(%x), last user m_acc_id(%s), idx(%d)",
                err, user, NumberToString(accId3, 0), uid);
        }
    }
}

// ============================================================================
// 队伍
// ============================================================================
CParty* CGameManager::GetParty()
{
    CParty* party = m_partyPool.Acquire();
    if (party == 0)
        return 0;
    party->SetIDX(m_partyPool.GetIndex(party));
    int partyIndex = party->GetPartyIndex();
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.find(partyIndex);
    if (it == m_partyMap.end())
    {
        m_partyMap[party->GetPartyIndex()] = party;
        return party;
    }
    else
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0xa5f,
                              "party index(%d) already alloc", party->GetPartyIndex());
    }
    return party;
}

CParty* CGameManager::GetParty(int index)
{
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.find(index);
    if (it == m_partyMap.end())
        return 0;
    return it->second;
}

int CGameManager::GetPartyIdx(CParty* party)
{
    int idx = m_partyPool.GetIndex(party);
    return idx;
}

void CGameManager::PutParty(CParty* party)
{
    // N82 封顶（hash_map 迭代器槽平移）：ORIG 槽组 key@-0x18/iter@-0x20/
    // eraseKey@-0x14/erase-arg@-0xc；4.4 对 __gnu_cxx::hash_map find/erase 链
    // 槽组整体 -4 平移（key@-0x14/iter@-0x1c/eraseKey@-0x10/arg@-0x20），
    // 归一后余差仅槽号（fn_diff_tu 实证），槽分配策略跨版本差无源码解。
    // b28d-lane12: 声明序 eraseKey,key 使 key@-0x18 与 ORIG 对齐（ndiff 12→8）；
    // 残余 eraseKey@-0x10 vs ORIG -0xc（end sret 占 -0x14..-0xd）→ N82附。ORIG 0x08294fc8。
    // b28d-solo-CGameManager: AE 达成（compare_tu clean）——P0 内联右值形：ORIG 槽
    // -0x18/-0x14/-0xc 全为 find/erase 的 const& 实参临时与 end() sret 临时（无具名
    // key/eraseKey）；具名标量形把 sret 顶到最浅 -0x10（三形实测 N82附，本波翻正）。
    if (party == 0)
        return;
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it =
            m_partyMap.find(party->GetPartyIndex());
    if (it != m_partyMap.end())
    {
        m_partyMap.erase(party->GetPartyIndex());
        m_partyPool.Free(party);
    }
}

void CGameManager::CheckOutParty(CUser* user, bool flag)
{
    // 探针 /tmp/l4_cop_probe.cpp：f1 裸 (bool)(x^1) goto 形=xor;test;jne；
    // f2c (char)字段>0=movzbl;test;jle（P196 有符号）；f4 get_state()==2 裸形
    // =cmp;sete;test;je。三空块 4.1 [nop;jmp] 尾不可复现（N，同 deathTower 族）。
    if ((bool)(user->CheckInParty() ^ 1)) goto end;
    {
        CParty* party = GetParty(user->GetPartyIndex());
        if (party == 0) goto end;
        {
            bool bVar1 = false;
            CDungeon* dungeon = party->m_battleField.m_pDungeon;  // ORIG *(party+0xcac)（CBattle_Field 区）
            if (dungeon != 0)
            {
                if ((char)dungeon->m_bloodDungeonType > 0)
                    bVar1 = true;
                if (((CDungeon*)dungeon)->isTowerOfDespairDungeon())
                    bVar1 = true;
            }
            if (dungeon != 0)
            {
                if ((char)((CDungeon*)dungeon)->get_dimension_possible() > 0 ||
                    dungeon->m_89c != 0 ||
                    ((CDungeon*)dungeon)->isTournamentDungeon())
                    bVar1 = true;
            }
            party->m_battleField.m_dungeonClearTracer.Trace(
                __PRETTY_FUNCTION__);
            if (party->m_battleField.m_bloodState != 1)
            {
                if (party->get_state() == 2)
                {
                    bool bVar2 = false;
                    if (dungeon != 0 && dungeon->m_entryRestrictFlag != 0)
                        bVar2 = true;
                    bool bVar3 = false;
                    if (dungeon != 0 && dungeon->m_villageAttackEventFlag != 0)
                        bVar3 = true;
                    if (!bVar1 && bVar2 && bVar3)
                        user->giveup_panalty();
                }
            }
        }
        party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
    }
end:
    ;
}

bool CGameManager::CheckOutQuickParty(CParty* party, bool flag)
{
    // ORIG 0x082983fc：then 分支 xor$1,%eax 直返（探针 f1 形同构）；
    // party==0 沿 eax 残值（flag spill）返回且无 movzbl（4.1 spill 策略，残差 N）。
    // b28d-solo-CGameManager 判词：N 封顶（ours 25 / orig 25，残差 1 行）——ORIG 入口
    // `mov 0x10(%ebp),%eax; mov %al,-0xc` 后 eax 恒持 flag，party==0 直落 epilogue
    // 复用 eax；4.4 恒补 `movzbl -0xc(%ebp),%eax`。ORIG `xor$1;test;jmp;nop` 的 test
    // 为 4.1 bool 归一 dead test（P225），4.4 不发。本波实测 `!call()` 与
    // `(bool)(call()^1)` 两形产物逐条相同（4.4.7），故保留 `!` 形。
    // b28d-laneCGM: P223 空 then 占位岛形——ORIG 0x0829840d `cmpl $0,0xc(%ebp); je
    //   +0x46(nop 岛)`；体尾 0x08298440 `jmp +0x47` 跨过 nop 落 epilogue。裸
    //   if/return 形被 4.4 jump-thread（无 jmp+nop，ndiff 2）。
    if (party == 0)
        goto L_quick_party_flag;
    return !G_CGameManager()->GetQuickPartySystemManager()->cancel_quick_party(party, flag, 0);
L_quick_party_flag:;
    return flag;
}

// ============================================================================
// 交易
// ============================================================================
CTradeSpace* CGameManager::GetTradeSpace()
{
    CTradeSpace* tradeSpace = m_tradeSpacePool.Acquire();
    if (tradeSpace == 0)
        return 0;
    tradeSpace->SetIDX(m_tradeSpacePool.GetIndex(tradeSpace));
    return tradeSpace;
}

CTradeSpace* CGameManager::GetTradeSpace(int index)
{
    CTradeSpace* tradeSpace = m_tradeSpacePool.Get(index);
    if (tradeSpace == 0)
        return 0;
    return tradeSpace;
}

int CGameManager::GetTradeIdx(CTradeSpace* tradeSpace)
{
    int idx = m_tradeSpacePool.GetIndex(tradeSpace);
    return idx;
}

void CGameManager::PutTradeSpace(CTradeSpace* tradeSpace)
{
    if (tradeSpace == 0)
        return;
    tradeSpace->reset();
    m_tradeSpacePool.Free(tradeSpace);
}

void CGameManager::CheckOutTrade(CUser* user)
{
    // ORIG 0x08297c25（Ghidra: cVar1 == '\x01'）：空 then + else 结构命中
    // xor $0x1,%eax + test %al,%al + jne skip（body fall-through，探针 wb 实证）
    if ((bool)(user->CheckInTrade() ^ 1))
    {
    }
    else
    {
        CTradeSpace* tradeSpace = m_tradeSpacePool.Get(user->GetTradeSpace());
        tradeSpace->cancel_trade_by_dis(user);
        PutTradeSpace(tradeSpace);
        return;
    }
    // ORIG +0x5c 尾岛：else 块尾 return + 函数尾显式 return → call;jmp;merge;leave
    return;
}

// ============================================================================
// PvP
// ============================================================================
PvP_Room* CGameManager::GetPvp()
{
    // b28d-lane12: CSwitchLog 改临时链（P221）无净降（62）；残余 Guard/CSwitchLog/
    // find sret 槽组平移 → N82附。ORIG 0x0829507c。
    // b28d-solo-CGameManager 判词：N82附（帧 0x64+push ebx vs 0x68；room -0xc vs -0x10）。
    PvP_Room* room = m_pvpRoomPool.Acquire();
    if (room == 0)
        return 0;
    room->SetIDX(m_pvpRoomPool.GetIndex(room));
    int roomIndex = room->get_index();
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(roomIndex);
    if (it == m_pvpRoomMap.end())
    {
        // b28d-laneCGM（P87/P227）：ORIG 0x08295134 `call get_index; mov %eax,%ebx` 在
        //   CSwitchLog ctor **前**预取（op() 实参用 %ebx）；具名 roomIdx 恒落槽。
        CSwitchLog(__PRETTY_FUNCTION__, 0xaa9, 0, 0)(
            "pvp@log room(%d) new alloc", room->get_index());
        m_pvpRoomMap[room->get_index()] = room;
        return room;
    }
    else
    {
        CSwitchLog(__PRETTY_FUNCTION__, 0xab2, 0, 0)(
            "pvp@log room(%d) already alloc", room->get_index());
        return 0;
    }
}

PvP_Room* CGameManager::GetPvp(int index, CUser* user, int param)
{
    // b28d-lane12: CSwitchLog 改临时链（P221）60→42；残余 CSwitchLog/find sret/
    // name/accName 槽平移 → N82附。ORIG 0x08295206。
    // b28d-solo-CGameManager 修法（ORIG 0x08295206）：早返 `it==end → return 0`；
    // name/accName 用三元 `user==0 ? "" : ...` 内联（ORIG 0x08295284 段 cmpl/je 双分支 +
    // `mov $0x8c161de,%esi` 空串）；末尾 `return it->second` 重新 ptEv。
    // b28d-solo-CGameManager: P231/P307 纯 register（非绑定，P0 合规）使 roomIdx/
    // name/accName 升格 callee-saved：帧/指令数 76→81 与 ORIG 全等（ndiff 101→22），
    // 余差仅 ORIG 空串立即数落 %esi/%ebx vs 4.4 落 %eax → N7 寄存器名族封顶。
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(index);
    if (it == m_pvpRoomMap.end())
        return 0;
    // b28d-solo 实验（P231/P307/P87）：ORIG 用 edi/esi/ebx 跨 CSwitchLog ctor 保活。
    register int roomIdx = it->second->get_index();
    register const char* name = user == 0 ? "" : user->getCurCharacName();
    register const char* accName = user == 0 ? "" : user->get_acc_name();
    CSwitchLog(__PRETTY_FUNCTION__, 0xacc, 0, 0)(
        "pvp@log %s,%s, room(%d), %d", accName, name, roomIdx, param);
    return it->second;
}

int CGameManager::GetPvpIdx(PvP_Room* room)
{
    int idx = m_pvpRoomPool.GetIndex(room);
    return idx;
}

void CGameManager::PutPvp(PvP_Room* room)
{
    // b28d-solo-CGameManager 判词：AE（P223 变体翻正——S154「N121 尾岛不可择」判词作废）。
    // ORIG 0x08295348 形状 = `je <nop 岛 0x101>; …; test %al; je <epilogue 0x102>;
    // …; jmp 0x102; 0x101: nop; 0x102:`——唯一复现形是**第一守卫裸早返**
    // `if (room == 0) return;` + 第二守卫正向包裹（探针 /tmp/solo_probe3.cpp g8
    // 在 4.4.6 下逐条同构；g2/g10 双早返会多出第二个 nop 岛、g1/g3 正向包裹无岛）。
    if (room == 0)
        return;
    // ORIG：roomIndex 经 ebx callee-saved 跨 CSwitchLog 构造缓存（P87）；
    // bare `register`（无 asm 绑定，P0 合规——仅槽+ebx 装载形）。
    register int roomIndex = room->get_index();
    CSwitchLog(__PRETTY_FUNCTION__, 0xae4, 0, 0)("pvp@log room(%d)", roomIndex);
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it =
            m_pvpRoomMap.find(room->get_index());
    if (it != m_pvpRoomMap.end())
    {
        m_pvpRoomMap.erase(room->get_index());
        m_pvpRoomPool.Free(room);
    }
}

void CGameManager::CheckOutPvp(CUser* user, bool flag)
{
    // b28d-lane12: N82 槽平移封顶（orig=210 our=210 gap=0；ndiff 270 全为槽号——
    // PacketGuard/CSwitchLog/leave_room 局部槽组）。ORIG 0x08297eaa。
    // b28d-laneCGM 真语义修正（判据①·条件极性）：ORIG 0x08297ec9 `jne 08298197`
    // 落点 = +0x2ed（尾部 nop→epilogue），且报告 Ghidra C 为 `if (cVar2 == '\x01')`
    // ⇒ ORIG 是 `CheckInPvp()` 为**真**时执行 body。旧形 `!user->CheckInPvp()`
    // （我方 1fe0 `je 22be` 同样落尾部 ⇒ 假值进 body）方向相反——真 bug。
    // ORIG 的 `xor $1; test; jne tail` 是 4.1 对 bool 取反的跳转形（保留 xor 段）。
    // b28d-laneCGM：空 then + else 结构复现 ORIG 的 `xor $1,%eax; test %al,%al;
    //   jne <tail>`（P223 变体 / 同 CheckOutTrade 形）；`if(f()){body}` 反向。
    if (!user->CheckInPvp())
    {
    }
    else
    {
        PvP_Room* room = GetPvp((int)user->GetPvpIndex(), user, 0);
        if (room != 0)
        {
            PacketGuard guard;
            bool outFlag = false;
            int ret = room->leave_room(user, outFlag);
            int outFlagInt = outFlag;
            // b28d-laneCGM（P227 全表达式临时链）：ORIG 0x08297f49..0x08297fc3 在
            //   CSwitchLog ctor **之前**右→左求值 battleMode/roomIdx/characName/accName
            //   并以 %edi/%esi/%ebx 跨 ctor 保活；具名局部恒落槽（我方旧形 -0x14/-0x10/-0xc）。
            CSwitchLog(__PRETTY_FUNCTION__, 0x11a1, 0, 0)(
                "pvp@log %s,%s, room(%d),%d,%d",
                user->get_acc_name(), user->getCurCharacName(),
                room->get_index(), room->get_pvp_battle_mode(), outFlagInt);
            // b28d-laneCGM: 反转 if/else 使 4.4 物理块序与 ORIG 一致——ORIG
            //   0x08297fcc `js +0x209` 且 >=0 块直落（+0x128），logFormat 块置后
            //   （+0x209，尾直落 join）；4.4 对 `if(ret<0){A} else {B}` 恒产
            //   `jns B; A; jmp join; B:`（A 前置）⇒ 改写为 `if(ret>=0){B} else {A}`。
            if (ret >= 0)
            {
                guard.clear();
                room->make_seat_info((char*)&guard, ret);
                G_GameWorld()->send_all(guard);
                if (outFlag)
                {
                    guard.clear();
                    room->make_state_info((char*)&guard);
                    G_GameWorld()->send_all(guard);
                }
                if (room->get_waiter_count() == 0)
                {
                    room->destroy_room(0);
                    PutPvp(room);
                    guard.clear();
                    room->make_state_info((char*)&guard);
                    G_GameWorld()->send_all(guard);
                }
            }
            else
            {
                LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x11bd,
                                      "pvp@log room(%d) user(%s) leave fail",
                                      room->get_index(),
                                      NumberToString(user->get_acc_id(), 0));
            }
            if (!flag && room->get_recv_pvp_rank_count() > 0)
            {
                if (room->IsInsertTimerRecvPvpRank() != 1 &&
                    room->IsEndPvpBattle() != 1)
                {
                    room->send_pvp_end();
                }
            }
        }
    }
}

PvP_Room* CGameManager::FindQuickJoinablePvPRoom(bool quick, CUser* user)
{
    // b28d-solo-CGameManager 判词：控制流已按 ORIG 重写（mode==6 正向守卫 sete 形、
    // (quick==0||!IsExistPassword())&&IsQuickJoinablePVPRoom 短路物化；PvP_Room.h
    // IsQuickJoinablePVPRoom int→bool 由 S09 落定并经 R2 广播）；余 70 vs 69 为
    // `quick==0` 的 xor 物化（N122：4.4 对 bool 局部取反恒 xor，4.1 为 cmpb 内存
    // 直比）——S1 实测 `(int)quick==0`→`quick==0` 两形产物逐条相同，按 N122 封顶；
    // IsExistPassword 已由 Main 落 bool（PvP_Room.h:110，T3 直证 0x082a3263），
    // 裸 `!` 形复现 ORIG 的 `xor $1; test; je` 段。ORIG 0x082a3202。
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.begin();
    // b28d-solo-CGameManager: while 形复现 ORIG 入口 jmp 底部条件并消 goto 形多出的
    // jmp 后 nop（探针 /tmp/solo_probe5.cpp h_while；ORIG 增量段调 ppEv = 前置 ++it）。
    while (it != m_pvpRoomMap.end())
    {
        PvP_Room* room = it->second;
        if (room != 0)
        {
            // b28d-solo-CGameManager: ORIG 0x082a324d `cmp $6; sete; test; jne`
            // ⇒ 正向守卫 `== 6` 跳过（原 `!= 6` 包裹产生反向 je 布局）。
            if (room->get_pvp_battle_mode() == 6)
                goto it_inc;
            // ORIG 0x082a3257..0x082a3293（Ghidra C 逐字）：
            //   false = (quick && IsExistPassword()) || !IsQuickJoinablePVPRoom(user)
            // 本形与原德摩根形 `(quick==0||!pw)&&joinable` 在 4.4 产物**逐条相同**
            // （`movzbl -0x2c; xor $1; test; jne`，quick 进 && 首项即被物化）；
            // ORIG 的 `cmpb $0,-0x2c; je` 内存直比属 4.1 bool 形参消费形
            // ⇒ N122 封顶（两形实测同，保留与 ORIG Ghidra 一致的写法）。
            if (!((quick && room->IsExistPassword()) ||
                  !room->IsQuickJoinablePVPRoom(user)))
                return room;
        }
    it_inc:
        ++it;
    }
    return 0;
}

// ============================================================================
// 快速队伍
// ============================================================================
QuickParty::CQuickParty* CGameManager::GetQuickParty()
{
    QuickParty::CQuickParty* qp = m_quickPartyPool.Acquire();
    if (qp == 0)
        return 0;
    qp->set_quick_party_index(m_quickPartyPool.GetIndex(qp));
    int qpIndex = qp->get_quick_party_index();
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(qpIndex);
    if (it == m_quickPartyMap.end())
    {
        m_quickPartyMap[qp->get_quick_party_index()] = qp;
        return qp;
    }
    else
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xb64,
                              "quick party index(%d) already alloc",
                              qp->get_quick_party_index());
    }
    return qp;
}

QuickParty::CQuickParty* CGameManager::GetQuickParty(int index)
{
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(index);
    if (it == m_quickPartyMap.end())
        return 0;
    return it->second;
}

int CGameManager::GetQuickPartyIdx(QuickParty::CQuickParty* qp)
{
    int idx = m_quickPartyPool.GetIndex(qp);
    return idx;
}

void CGameManager::PutQuickParty(QuickParty::CQuickParty* qp)
{
    // P212 头部声明使 find sret 缓冲对齐 ORIG -0x20；余差 key/eraseKey/erase-arg
    // 三槽号（4.4 标量分区 vs 4.1 生命周期穿插，N82附封顶，v1/v4(double pad)/v5/v7 探针穷举）。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    // ORIG 0x08295608。
    if (qp == 0)
        return;
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(qp->get_quick_party_index());
    if (it != m_quickPartyMap.end())
    {
        m_quickPartyMap.erase(qp->get_quick_party_index());
        m_quickPartyPool.Free(qp);
        qp->init();
    }
}

// ============================================================================
// 公会战房间
// ============================================================================
WarRoom* CGameManager::GetWarRoom()
{
    WarRoom* room = m_warRoomPool.Acquire();
    if (room == 0)
        return 0;
    room->SetIndex(m_warRoomPool.GetIndex(room));
    int roomIndex = room->GetIndex();
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(roomIndex);
    if (it == m_warRoomMap.end())
    {
        m_warRoomMap[room->GetIndex()] = room;
        return room;
    }
    else
    {
        return 0;
    }
}

WarRoom* CGameManager::GetWarRoom(int index)
{
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(index);
    if (it == m_warRoomMap.end())
        return 0;
    return it->second;
}

int CGameManager::GetWarRoomIdx(WarRoom* room)
{
    int idx = m_warRoomPool.GetIndex(room);
    return idx;
}

void CGameManager::PutWarRoom(WarRoom* room)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x082a2666。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (room == 0)
        return;
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it =
            m_warRoomMap.find(room->GetIndex());
    if (it != m_warRoomMap.end())
    {
        m_warRoomMap.erase(room->GetIndex());
        m_warRoomPool.Free(room);
    }
}

void CGameManager::CheckOutWarRoom(CUser* user)
{
    // ORIG 族形态：空 then + else（xor$1;test;jne + 尾岛）
    if ((bool)(user->CheckInWarRoom() ^ 1))
    {
    }
    else
    {
        WarRoom* room = GetWarRoom(user->GetWarRoomIndex());
        if (room != 0)
        {
            // ORIG cmp$0x4;sete%al;test;je：具名 ret 槽比较生成 cmpl+jne（无 sete），
            // (bool)(call()==4) 复现物化形态
            if ((bool)(room->WalkOutUser(user) == 4))
            {
                LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x117b,
                                      "CGameManager::CheckOutWarRoom Can't find user charno(%d)",
                                      user->get_charac_no(-1));
            }
            room->CheckState();
        }
        return;
    }
    return;
}

WarRoom* CGameManager::FindJoinableWarRoom(CUser* user)
{
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.begin();
    // b28d-solo-CGameManager 判词：AE（while 形翻正——S154「N82附 槽互换」判词作废；
    // 探针 /tmp/solo_probe5.cpp h_while：入口 jmp 底部条件 + it++ 临时浅/end 深）。
    while (it != m_warRoomMap.end())
    {
        WarRoom* room = it->second;
        // P225 推广：ORIG 0x082a3316 两段独立守卫（cmpl/je + call/test/sete/test/je），
        // `&&` 合并会被 4.4 折成 mov$1/mov$0 物化 ⇒ 拆嵌套 if。
        if (room != 0)
        {
            if (room->IsJoinable(user) == 0)
                return room;
        }
        it++;
    }
    return 0;
}

// ============================================================================
// 死亡塔 / 领主塔 / 前置祭坛 / 蓝弹珠
// ============================================================================
WongWork::CDeathTower* CGameManager::getDeathTower()
{
    WongWork::CDeathTower* tower = m_deathTowerPool.Acquire();
    if (tower == 0)
        return 0;
    tower->setIdx(m_deathTowerPool.GetIndex(tower));
    int towerIdx = tower->getIdx();
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(towerIdx);
    if (it == m_deathTowerMap.end())
    {
        m_deathTowerMap[tower->getIdx()] = tower;
        tower->reset();
        return tower;
    }
    else
    {
        return 0;
    }
}

WongWork::CDeathTower* CGameManager::getDeathTower(int index)
{
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(index);
    if (it == m_deathTowerMap.end())
        return 0;
    return it->second;
}

int CGameManager::getDeathTowerIdx(WongWork::CDeathTower* tower)
{
    int idx = m_deathTowerPool.GetIndex(tower);
    return idx;
}

void CGameManager::returnDeathTower(WongWork::CDeathTower* tower)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x08295852。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (tower == 0)
        return;
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(tower->getIdx());
    if (it != m_deathTowerMap.end())
    {
        m_deathTowerMap.erase(tower->getIdx());
        m_deathTowerPool.Free(tower);
    }
}

// [B24D判词 N] 语义正确：checkInDeathTower 以 (bool)(x^1) 判断复现 xor$1；
//   checkIn==1 跳过；else getDeathTower(idx)→onLeaveUser(x^1)→logFormat(0x123a)。
//   diff=句尾合并跳转链我方多一 `jmp;nop` 对（4.1 vs 4.4 对齐），57/55 纯
//   lowering/对齐表示差异，语义等价。
void CGameManager::checkOutDeathTower(CUser* user)
{
    // P63 goto 形：checkIn!=1 直跳出口。4.4 对空 then 块穿透（消 nop;jmp），
    // ORIG 4.1 空块 [nop;jmp 出口] 不可复现（跨版本块排布 N，B24D 判词在册）；
    // goto 形消多余尾部中继，为本源码形态最优近似（53/55）。
    WongWork::CDeathTower* tower;
    if ((bool)(user->checkInDeathTower() ^ 1)) goto end;
    tower = getDeathTower(user->getDeathTowerIndex());
    if (tower == 0) goto end;
    // onLeaveUser 返回类型 bool 化后裸 !call() 直产 call;xor$1,%eax;test%al;je（P168a 技巧1）。
    if (!tower->onLeaveUser(user))
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x123a,
                              "DeathTower checkOutDeathTower ID[%d] ACCID[%s]",
                              tower->getIdx(),
                              NumberToString(user->get_acc_id(), 0));
    }
end:
    ;
}

WongWork::CBossTower* CGameManager::getBossTower()
{
    WongWork::CBossTower* tower = m_bossTowerPool.Acquire();
    if (tower == 0)
        return 0;
    tower->setIdx(m_bossTowerPool.GetIndex(tower));
    int towerIdx = tower->getIdx();
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(towerIdx);
    if (it == m_bossTowerMap.end())
    {
        m_bossTowerMap[tower->getIdx()] = tower;
        ((WongWork::CBossStage*)tower)->reset();
        return tower;
    }
    else
    {
        return 0;
    }
}

WongWork::CBossTower* CGameManager::getBossTower(int index)
{
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(index);
    if (it == m_bossTowerMap.end())
        return 0;
    return it->second;
}

int CGameManager::getBossTowerIdx(WongWork::CBossTower* tower)
{
    int idx = m_bossTowerPool.GetIndex(tower);
    return idx;
}

void CGameManager::returnBossTower(WongWork::CBossTower* tower)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x08295a90。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (tower == 0)
        return;
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(tower->getIdx());
    if (it != m_bossTowerMap.end())
    {
        m_bossTowerMap.erase(tower->getIdx());
        m_bossTowerPool.Free(tower);  // ORIG 0x082b2420 = StaticPool<CBossTower,600>::Free（call 目标核对纠错）
    }
}

void CGameManager::checkOutBossTower(CUser* user)
{
    // P63 goto 形（同 checkOutDeathTower）：空 then 块 4.4 穿透，N 封顶；
    WongWork::CBossTower* tower;
    if ((bool)(user->checkInBossTower() ^ 1)) goto end;
    tower = getBossTower(user->getBossTowerIndex());
    if (tower == 0) goto end;
    {
            // **虚槽仲裁（多点联立，非单点）**：ORIG 此处是
            // `mov (%eax),%eax; add $0x14,%eax; mov (%eax),%edx; call *%edx`。
            // %eax 取到的是 vptr（指向 _ZTV+0x08），故 +0x14 ⇒ _ZTV+0x1c。
            // dump `_ZTVN8WongWork10CBossTowerE`@0x08b70260 全槽实测：
            //   +0x00 generateRewardItem  +0x04 generateRewardExp
            //   +0x08 _checkMemberDie     +0x0c checkAllMemberReady(0x081493fe)
            //   +0x10 handleDieUser       **+0x14 handleLeaveUser(0x08143ae2)**
            //   +0x18 handleMoveMap       +0x1c CBossStage::handlePickupItem
            // ⇒ 原注释「+0x14 = checkAllMemberReady」把 **vtable 数组下标**当成
            // **vptr 偏移**，实际被调的是 **CBossTower::handleLeaveUser(CUser*)**。
            // 头文件虚声明顺序本身与 ORIG 完全一致，无需改布局（零 ABI 风险）。
            // X1 形（probe /tmp/xg2.cpp 形 B）：裸 (bool)(x^1) 产 cmp$1;setne，
            // N96/P0：bool callee 裸形（P168a①）直产 call;xor$1;test;je；
            // 残差 call *%eax vs ORIG *%edx 属 N7 寄存器名族。
            if (!tower->handleLeaveUser(user))
            {
                LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x11d9,
                                      "BossTower checkOutBossTower ID[%d] ACCID[%s]",
                                      tower->getIdx(),
                                      NumberToString(user->get_acc_id(), 0));
            }
        }
end:
    ;
}

advancealtar::StageControl* CGameManager::getAdvanceAltar()
{
    advancealtar::StageControl* control = m_stagePool.Acquire();
    if (control == 0)
        return 0;
    control->setIndex(m_stagePool.GetIndex(control));
    int ctrlIdx = control->getIndex();
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(ctrlIdx);
    if (it == m_stageMap.end())
    {
        m_stageMap[control->getIndex()] = control;
        control->reset();
        return control;
    }
    else
    {
        return 0;
    }
}

advancealtar::StageControl* CGameManager::getAdvanceAltar(int index)
{
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(index);
    if (it == m_stageMap.end())
        return 0;
    return it->second;
}

int CGameManager::getAdvanceAltarIndex(advancealtar::StageControl* control)
{
    int idx = m_stagePool.GetIndex(control);
    return idx;
}

void CGameManager::returnAdvanceAltar(advancealtar::StageControl* control)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x08295cce。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (control == 0)
        return;
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(control->getIndex());
    if (it != m_stageMap.end())
    {
        m_stageMap.erase(control->getIndex());
        m_stagePool.Free(control);
    }
}

void CGameManager::checkOutAdvanceAltar(CUser* user)
{
    // P133 独立守卫 + X1 自然形（xor 物化）+ P63 goto 尾（空块 4.4 穿透，N 残差）。
    advancealtar::StageControl* control;
    if (user == 0) goto end;
    {
        // N96/P0：bool callee 裸形。
        if (!user->checkInAdvanceAltar()) goto end;
    }
    control = getAdvanceAltar(user->getAdvanceAltarIndex());
    if (control == 0) goto end;
    {
        // leaveUser 返回类型 bool 化后裸 !call() 直产 call;xor$1,%eax;test%al;je（P168a 技巧1）。
        if (!control->leaveUser())
        {
            LogManager::logFormat(1, "App.cpp",
                                  __PRETTY_FUNCTION__, 0x11ef,
                                  "invalid user : advance altar logout ID[%d] ACCID[%s]",
                                  control->getIndex(),
                                  NumberToString(user->get_acc_id(), 0));
        }
    }
end:
    ;
}

void CGameManager::onTimeAdvanceAltar()
{
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.begin();
    while (it != m_stageMap.end())
    {
        advancealtar::StageControl* control = it->second;
        // ORIG 0x08295dXX：`call onTimerStageTick; xor $0x1,%eax; test %al,%al;
        // je <it++ 块>` —— `!tick` 为真落 erase、为假跳 it++（P9/P22 物化形，
        // 非 cmp$1/sete；语义与 tick==1→keep 等价）。
        if (!control->onTimerStageTick())
        {
            // ORIG 0x08295eXX：无具名 eraseIt 拷贝对（省 mov 双 qword 4 条）——
            // 后自增返回临时迭代器直接喂 erase（P135 pair 临时槽形）。
            m_stageMap.erase(it++);
            m_stagePool.Free(control);
        }
        else
        {
            it++;
        }
    }
}

BlueMarble* CGameManager::getBlueMarble()
{
    BlueMarble* marble = m_blueMarblePool.Acquire();
    if (marble == 0)
        return 0;
    marble->setMemoryPoolIndex(m_blueMarblePool.GetIndex(marble));
    int marbleIdx = marble->getMemoryPoolIndex();
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(marbleIdx);
    if (it == m_blueMarbleMap.end())
    {
        m_blueMarbleMap[marble->getMemoryPoolIndex()] = marble;
        return marble;
    }
    else
    {
        return 0;
    }
}

BlueMarble* CGameManager::getBlueMarble(int index)
{
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(index);
    if (it == m_blueMarbleMap.end())
        return 0;
    return it->second;
}

int CGameManager::getBlueMarbleIdx(BlueMarble* marble)
{
    int idx = m_blueMarblePool.GetIndex(marble);
    return idx;
}

void CGameManager::putBlueMarble(BlueMarble* marble)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x082a2434。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (marble == 0)
        return;
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(marble->getMemoryPoolIndex());
    if (it != m_blueMarbleMap.end())
    {
        m_blueMarbleMap.erase(marble->getMemoryPoolIndex());
        m_blueMarblePool.Free(marble);
    }
}

void CGameManager::checkOutBlueMarble(CUser* user)
{
    // ORIG 同 CheckOutTrade 族：空 then + else → xor$1;test;jne + 双 jmp 尾岛
    if ((bool)(user->checkInBlueMarble() ^ 1))
    {
    }
    else
    {
        BlueMarble* marble = getBlueMarble(user->getBlueMarbleIndex());
        if (marble != 0)
        {
            marble->leaveUser(user);
        }
        return;
    }
    return;
}

void CGameManager::allocBlueMarble()
{
    // P0/P48：ORIG 0x082a339c 将 G_CDataManager() 结果 `mov %eax,(%esp)` 直传
    // getBlueMarbleScript（无 dm 槽）；仅 getBlueMarbleScript 结果存 -0x18 槽
    // （script 具名局部）。故勿命名 dm，内联 cast 进调用点。
    BlueMarbleScriptManager* script =
        (BlueMarbleScriptManager*)
        ((CDataManager*)G_CDataManager())->getBlueMarbleScript();
    for (int i = 0; i < 300; ++i)
    {
        BlueMarble* marble = getBlueMarble();
        marble->getMemoryPoolIndex();
        for (int j = 0; j < script->m_info.m_playerMax; ++j)  // ORIG *(script+0x30)
        {
            marble->setUserInfo(script);
        }
        marble->setScript(script);
    }
}

BlueMarble* CGameManager::findJoinableBlueMarble()
{
    // b28d-solo-CGameManager: ORIG 0x082a3424 段 `call G_CDataManager; mov %eax,(%esp);
    // call getBlueMarbleScript` 无 dm 具名槽（P0 表达式直传）。
    BlueMarbleScriptManager* script =
        (BlueMarbleScriptManager*)((CDataManager*)G_CDataManager())->getBlueMarbleScript();
    if (script == 0)
        return 0;
    int count = script->m_info.m_playerMax;  // ORIG *(script+0x30)
    // b28d-lane12: 真语义修正——ORIG 0x082a3424 外层为**倒计数** `for(;count>0;count--)`
    // （0x082a352a `subl $1,-0x10`），内层条件用**当前** count-1（0x082a34c4
    // `mov -0x10,%edx; sub $1; cmp`），而非恒定 m_playerMax-1。内层为 4.1 for 布局
    // （入口 jmp 底部条件 0x082a3491→0x082a34f5）；marble==0 时不 ++it（0x082a34a8
    // je 内层条件，P171 照抄）；迭代器先默认构造后赋值（P50 复活，0x082a3460 C1）。
    BlueMarble* marble = 0;
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it;
    for (; count > 0; count--)
    {
        it = m_blueMarbleMap.begin();
        // b28d-solo-CGameManager 判词：AE（while 形翻正——S154「N82附 槽互换」判词作废；
        // 探针 /tmp/solo_probe5.cpp h_while，同 SendWarRoomList/FindJoinableWarRoom）。
        while (it != m_blueMarbleMap.end())
        {
            marble = it->second;
            if (marble != 0)
            {
                if (marble->checkJoin())
                {
                    if (marble->getWaitUserCount() == count - 1)
                        return marble;
                }
                it++;
            }
        }
    }
    return 0;
}

// ============================================================================
// 列表发送
// ============================================================================
void CGameManager::SendPartyList(CUser* user)
{
    // b28d-lane12: N82 槽平移封顶（orig=384 our=380 gap=4；ndiff 614——迭代器/
    // PacketGuard/局部槽组分区差）。ORIG 0x08296792。
    // b28d-solo-CGameManager 判词：语义缺口已修（P167 70=70 CLEAR——4 处三元
    // put_byte 改 if/else 双调用点、getDemensionInoutValue 改 getCurCharacR()->
    // m_demensionInout[j] 直读、dim 去具名槽）；本波 P11 类型旋钮（type→unsigned
    // char、member!=0 正向包裹）ndiff 82→45；余 386 vs 384 为 if/else 臂序
    // （4.4 恒 then 后置）+ char/uchar 读扩展 + N82 槽组分区（guard -0x50 vs -0x4c）。
    PacketGuard guard;
    guard.put_header(0, 9);
    int count = 0;
    int index = guard.get_index();
    guard.put_short((int)m_partyMap.size());
    unsigned char type = 0;  // b28d-solo：ORIG 读 type 用 movzbl（P11 unsigned 化）
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.begin();
    while (it != m_partyMap.end())
    {
        CParty* party = it->second;
        if (party != 0 && !party->IsAutoCreated())
        {
            count = count + 1;
            guard.put_short(party->GetPartyIndex());
            guard.put_byte((int)type);
            if (type == 0 || type == 1)
            {
                // T1 直证：ORIG 0x082968a1 `call getTitleIndex; movzbl %al,%eax`
                // ⇒ 低字节按 unsigned 扩展（原 `& 0xff` 在 char 返回下产
                // movsbl+and，P321-T1 照抄扩展形）。
                guard.put_byte((unsigned char)party->getTitleIndex());
                if (party->getTitleIndex() == 0)
                {
                    guard.put_int((int)strlen(party->getTitle()));
                    guard.put_str(party->getTitle(), (int)strlen(party->getTitle()));
                }
                // b28d-solo-CGameManager: ORIG 0x08296927 段为 if/else 两独立
                // put_byte 调用点（+0x1a7/+0x1bc）；三元写法 4.4 汇合单点（P167 -1）。
                if (party->IsReturnUserParty())
                    guard.put_byte(1);
                else
                    guard.put_byte(0);
                guard.put_byte(party->getUserMax() & 0xff);
                guard.put_short(party->getDungIndex() & 0xffff);
                guard.put_byte(party->getDungDiffi() & 0xff);
                if (party->IsEventCharacParty())
                    guard.put_byte(1);
                else
                    guard.put_byte(0);
            }
            if (type == 0 || type == 2)
            {
                int managerIdx = 0;
                for (int i = 0; i < 4; ++i)
                {
                    CUser* member = party->get_user(i);
                    // b28d-solo：ORIG 为 member!=0 直落、null 段后置（P222 块序翻正）
                    if (member != 0)
                    {
                        guard.put_short(member->get_unique_id() & 0xffff);
                        if (party->getManager() == member)
                            managerIdx = i;
                        char sex = member->getSex();  // ORIG -0x16 用 movsbl/cmpb$0xff（P11 signed）
                        if (sex == -1)
                            sex = 1;
                        guard.put_byte((int)sex);
                    }
                    else
                    {
                        guard.put_short(0xffff);
                        guard.put_byte(0xff);
                    }
                }
                if (party->IsReturnUserParty())
                    guard.put_byte(1);
                else
                    guard.put_byte(0);
                guard.put_byte(managerIdx);
                // T1 直证：ORIG 0x08296b11 `movzbl %al,%eax`（无 and）⇒ (unsigned char) cast。
                guard.put_byte((unsigned char)party->is_quick_party());
                if (party->IsEventCharacParty())
                    guard.put_byte(1);
                else
                    guard.put_byte(0);
            }
            if (type == 0 || type == 1 || type == 2)
            {
                // T1 直证：ORIG 0x08296b88 读该局部用 `movzbl -0x15(%ebp),%eax`
                // ⇒ unsigned char（原 char 生 movsbl，P321-T1 照抄宽度/扩展）。
                unsigned char matching = G_GameWorld()->is_dungeon_tag_matching_channel("[impossible]");
                guard.put_byte((int)matching);
                if (matching != 0)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        CUser* member = party->get_user(i);
                        // ORIG 0x08296bca `je <put_byte(0) 块>`（then 后置、else 直落）
                        // ⇒ P272 块序交换：条件取反 + 两块互换。
                        if (member != 0)
                        {
                            guard.put_byte(6);
                            for (int j = 0; j < 6; ++j)
                            {
                                // ORIG +0x456..+0x477：dim 无具名槽（get_dimensionInout
                                // 结果 movsbl 后直压 put_byte 实参，P0 表达式直传）。
                                guard.put_byte((int)(char)((CDataManager*)G_CDataManager())->get_dimensionInout(j));
                                // ORIG +0x47c..+0x49f：getCurCharacR() 后直读
                                // +0xeb9+j 字节（movzbl+movsbl），非 getDemensionInoutValue
                                // 调用（P167 EXTRA/MISSING 实证）。
                                guard.put_byte((int)(signed char)member->getCurCharacR()->m_demensionInout[j]);
                            }
                        }
                        else
                        {
                            guard.put_byte(0);
                        }
                    }
                }
            }
        }
        it++;
    }
    guard.put_short(index, count);
    guard.finalize(true);
    user->Send(guard);
}

void CGameManager::SendPvpList(CUser* user)
{
    PacketGuard guard;
    guard.put_header(0, 0x29);
    guard.put_short((int)m_pvpRoomMap.size());
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.begin();
    // b28d-solo-CGameManager 判词：AE（while 形翻正——S154「N82附 槽互换」判词作废；
    // 同 SendWarRoomList，探针 /tmp/solo_probe5.cpp h_while）。
    while (it != m_pvpRoomMap.end())
    {
        PvP_Room* room = it->second;
        if (room != 0)
        {
            if (room->get_waiter_count() > 0)
            {
                room->make_room_info((char*)&guard);
            }
        }
        it++;
    }
    guard.finalize(true);
    user->Send(guard);
}

void CGameManager::SendWarRoomList(CUser* user)
{
    PacketGuard guard;
    guard.put_header(0, 0x57);
    int index = guard.get_index();
    guard.put_short(0);
    int count = 0;
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.begin();
    // b28d-solo-CGameManager 判词：AE（while 形翻正——S154「N82附 槽互换」判词作废）。
    // 探针 /tmp/solo_probe5.cpp：4.4.6 对 while/for 形产「入口 jmp 底部条件」+ it++
    // 临时浅槽/end 深槽（与 ORIG 同：it++ -0x18 < end -0x20），goto 尾标签形则相反
    // （it++ -0x20 > end -0x18）且多一条 jmp 后 nop。
    while (it != m_warRoomMap.end())
    {
        WarRoom* room = it->second;
        if (room != 0)
        {
            room->MakeRoomInfo((char*)&guard);
            count = count + 1;
        }
        it++;
    }
    guard.put_short(index, count);
    guard.finalize(true);
    user->Send(guard);
}

void CGameManager::SendWarRoomToAll(int index)
{
    PacketGuard guard;
    guard.put_header(0, 0x57);
    guard.put_short(1);
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(index);
    if (it == m_warRoomMap.end())
        return;
    WarRoom* room = it->second;
    room->MakeRoomInfo((char*)&guard);
    guard.finalize(true);
    G_GameWorld()->send_all(guard);
}

void CGameManager::PrintWarRoomList()
{
	// ORIG 0x8298b40：函数体仅 4 条指令（push/mov/sub $0x50/leave/ret），
	// 0x50 帧为未使用局部（P31 死局部帧差对齐）。
	char m_unused[0x50];
}

// ============================================================================
// WarRoom 分配 / 计数管理
// ============================================================================
void CGameManager::WarRoomAlloc()
{
    // b28d-solo-CGameManager: ORIG 0x08298a30（77 insn）真形 = Ghidra C 双计数：
    //   roomSeq(-0x14, char, 初 0, 写 room+1 后 +1) 与 i(-0x10, 用于循环条件与
    //   SetState 比较) 是两个独立变量；原重建误用单 count 兼顾二者（语义/槽双错）。
    //   内循环为 4.1 底部条件布局（+0x36 jmp +0xc4）⇒ P283 goto 尾标签复现；
    //   dm/hourTable 去具名缓存（P0/P183：ORIG 无 dm 槽，hourTable 直接 -0x1c）。
    // b28d-solo-CGameManager 判词：DIFF·N82（gap 1，76 vs 77）。ORIG 帧 0x34+push %ebx
    //   vs 我方 0x38——%ebx 是 4.1 对表索引加法 `add %ebx,%eax` 的 area 副本
    //   （0x08298aa7 `mov -0x18(%ebp),%ebx`），4.4 恒用内存操作数（N18a/P0 族）；
    //   本波 dm3 具名删（内联 G_CDataManager）后 diff 行 51、p167 CLEAR。
    int hourTable = ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetCurrenTimeTable();
    for (int area = 0; area < 10; ++area)
    {
        // ORIG `addl $0x1,-0x14` + `mov %dl,0x1(%eax)` ⇒ roomSeq 为 int（Ghidra
        // 显示 char 仅因只取低字节）；声明 char 会产 movb/movzbl 三段（实测）。
        int roomSeq = 0;
        int i = 0;
        goto inner_check;
    inner_body:
        {
            WarRoom* room = GetWarRoom();
            // ORIG 0x8298a81/+0x05b：直写 +0x00=area、+0x01=roomSeq（区内序号）。
            room->m_levelBand = (char)area;
            room->m_roomSeq = (char)roomSeq;
            roomSeq = roomSeq + 1;      // ORIG +0x05e 在 Prepare 之前
            room->Prepare();
            // ORIG 0x08298a9d..0x08298aba：G_CDataManager 结果**无具名槽**（`mov %eax,%ecx`
            // 直用），表寻址 (hourTable*10+area+0x223c)*4+8 —— P0/P183 内联表达式。
            // ORIG 0x08298aa4..0x08298aba：成员数组二维索引 ⇒ 4.4 与 4.1 同发
            // `add $0x223c,%eax; mov 0x8(%ecx,%eax,4),%eax`（探针 f1；
            // reinterpret_cast<int*>(arr)[...] 指针索引恒折 add+shl+lea，f2/f3）。
            if (i < ((CDataManager*)G_CDataManager())
                         ->m_warAreaCounter.m_warRoomCount[hourTable][area])
                room->SetState((WARROOM_STATE)0);
            else
                room->SetState((WARROOM_STATE)-1);
            i = i + 1;
        }
    inner_check:
        if (((CDataManager*)G_CDataManager())
                ->m_warAreaCounter.GetWarRoomCountAtPeekTime(area) > i)
            goto inner_body;
    }
    PrintWarRoomList();
}

void CGameManager::WarRoomCountAdjustByChannelInfo()
{
    channel_script_t* script = (channel_script_t*)G_CDataManager()->GetChannelScript();
    // ORIG get_channel_no 与 getChannelInfo 各独立 G_CEnvironment()（P183 勿缓存 env）。
    // b28d-solo-CGameManager 判词：N85①+N82 封顶——ORIG `flds 0x8(%eax,%edx,4);
    // fstps -0x34`（具名 float 经 x87 栈槽往返）vs 4.4 `mov mem,%eax; mov %eax,-0x10`
    // 直搬（P252/N85① 跨版本）；另 ORIG push edi/esi/ebx 缓存 a/b/dmW（P52），4.4 槽化。
    unsigned int channelNo = G_CEnvironment()->get_channel_no();
    channel_info_t* info = script->getChannelInfo(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup, channelNo);
    for (int a = 0; a < 10; ++a)
    {
        for (int b = 0; b < 0x18; ++b)
        {
            // ORIG 内层每轮 **两次** G_CDataManager：dmW(写表基, 先)→f→第二处**内联**
            // 在乘法表达式里（ORIG 无 dmR 槽：0x8298a0a `call G_CDataManager` 结果
            // 直接 `mov -0x1c(%ebp),%edx; add $0x21ec,%edx; mov 0x4(%eax,%edx,4),%eax`
            // ——P0/P183 表达式直传）。
            // [TD4 2026-09-10 复核] ORIG 0x082989a1..0x082989b7 内层头把 dmW/b/a 载入
            // %esi/%ebx/%edi 跨调用保活；本波实测三处纯 register 提示（P231/P307）
            // 反劣（74→79、帧 0x3c→0x4c：register 变量仍各占槽 + 多 mov 中转）⇒ 回滚，
            // 维持表达式形，按 N7/N82 记录（ORIG 的 callee-saved 独占属 4.1 分配产物）。
            CDataManager* dmW = (CDataManager*)G_CDataManager();
            float f = info->m_gridPercent[a];  // ORIG *(float*)(info+8+(a+4)*4)
            // 写表：dmW+8+(b*10+a+0x223c)*4 = m_warRoomCount[b][a]（ORIG 0x082989f2..
            // 0x08298a02 单条 scaled store；reinterpret_cast<int*>(arr)[...] 指针索引
            // 恒折 add+shl+lea —— 探针 f1 vs f2/f3）。
            dmW->m_warAreaCounter.m_warRoomCount[b][a] =
                (int)(((CDataManager*)G_CDataManager())
                          ->m_warAreaCounter.m_counterTable[b] * f);
        }
    }
}

void CGameManager::WarRoomCountManage(int hourIndex)
{
    // b28d-solo-CGameManager 判词：DIFF·N82（本波 N78 部分翻正）。ORIG 0x082a271e
    // 帧 0xec / 本波帧 0xfc：清零段已由 `int current[10]={0}` 逐条复现（P214③/P253，
    // 见下），余差为 4.4 为 rep stos 多发 push %edi + 槽组整体平移 0x10（ORIG 的
    // 4.1 内联 memset 用 %edi 却不保存——ABI 层跨版本差，无合法 C++ 表达）。
    // 本波 dm/dm2 具名删（P0/P183 内联 G_CDataManager）后 gap 0（311=311）、
    // diff 行 432→332；p167 CLEAR（42=42）。语义等价，字节层 N82 封顶。
    // b28d-laneCGM（P227）：判词「gap 0 / diff 432→332」阶段的具名 tr0-tr3 逐点改
    //   全表达式临时链（同 CheckOutPvp/insert_game_world 实测有效形）。
    cMyTrace(__PRETTY_FUNCTION__, 0x306f, 0)("WarRoomCountManage Start");
    // ORIG 0x082a2768 实证：`mov $0x0,%eax; mov $0xa,%edx; mov %ebx,%edi;
    // mov %edx,%ecx; rep stos` —— **零初始化声明** `int current[10] = {0};` 在
    // 4.4.6 下逐条复现该段（探针 /tmp/solo_probe4.cpp m4 与 ORIG 全等），
    // 而 `memset(current,0,sizeof)` 恒发 `call memset`（N78）。P214③/P253。
    // 语义：10 个 dword（40 字节）全清（旧注"仅清 10 字节"误，曾为真 bug）。
    int current[10] = {0};
    int total = 0;
    for (int area = 0; area < 10; ++area)
    {
        int n = 0;
        // ORIG 0x082a2818 COND：peek 返回值**不入槽**，直接 `cmp -0x44(n),%eax;
        // setg;test;jne`（我们旧形 int need=… 多两条槽 store/load）。
        while (((CDataManager*)G_CDataManager())
                   ->m_warAreaCounter.GetWarRoomCountAtPeekTime(area) > n)
        {
            WarRoom* room = GetWarRoom(total++);
            if (room == 0)
            {
                LogManager::logFormat(1, "App.cpp",
                                      __PRETTY_FUNCTION__, 0x307d,
                                      "WarRoomCountManage Error : WarRoom does not exist %d", total);
            }
            else
            {
                if (room->GetState() != -1)
                    current[area] = current[area] + 1;
            }
            n = n + 1;
        }
    }
    int hourTable;
    if (hourIndex == -1)
    {
        // P0/P183：ORIG 0x082a2870 `call G_CDataManager; add $0x87b4,%eax; mov %eax,(%esp)`
        // ——无 dm 具名槽，内联表达式。
        hourTable = ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetCurrenTimeTable();
    }
    else
    {
        hourTable = hourIndex;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x3088, 0)(
        "WarRoomCountManage currentHourIndex %d", hourTable);
    PacketGuard guard;
    guard.put_header(0, 0x58);
    int index = guard.get_index();
    guard.put_short(0);
    int totalDiff = 0;
    int area = 0;
    // ORIG 0x082a291d `jmp 082a2b28`（入口跳条件）+ 0x082a2b28 `cmpl $0x9,-0x3c;
    // setle; test; jne 082a2922`（回边）⇒ while 形（P25：前置 jmp + 尾部 setle 物化）。
    // [jcc 目标判据修正] 原 do-while 形缺 ORIG 的入口 jmp（jcc 33 vs 32），已改回
    // while；Ghidra C 的 `do{...}while(true)`+`if(9<area)` 是条件块序误读。
    while (area <= 9)
    {
        // ORIG 0x082a292f..0x082a293f `add $0x223c; mov 0x8(%ecx,%eax,4),%edx`：
        // m_warRoomCount[hour][area] 成员数组二维索引（WarAreaCounter+0x144）；
        // G_CDataManager 结果无具名槽。
        int diff = ((CDataManager*)G_CDataManager())
                       ->m_warAreaCounter.m_warRoomCount[hourTable][area] -
                   current[area];
        cMyTrace(__PRETTY_FUNCTION__, 0x3097, 0)(
            "WarRoomCountManage diff(%d) currentWarRoomCount(lev%d)", diff, current[area]);
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            int lastIdx =
                ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetWarRoomCountLastIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(lastIdx--);
                if (room == 0)
                    break;
                // ORIG 0x082a2a00..0x082a2a24：`room!=0 && state!=-1` 两 && 项各 je
                // 同一 mov$0 岛（探针 boolp.cpp h 形）；德摩根 or 形在 4.4 反产 jne。
                if (room != 0 && room->GetState() != -1)
                {
                    char shut = room->ShutDown();
                    if (shut != 0)
                        room->MakeStateInfo((char*)&guard);
                    need = need - 1;
                }
            }
        }
        else if (diff > 0)
        {
            unsigned int need = (unsigned int)diff;
            int firstIdx =
                ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetWarRoomCountFirstIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(firstIdx++);
                if (room == 0)
                    break;
                // ORIG 0x082a2ab7..0x082a2ad9：`room!=0 && state==-1`（je + jne 同岛）。
                if (room != 0 && room->GetState() == -1)
                {
                    room->Open();
                    room->MakeStateInfo((char*)&guard);
                    need = need - 1;
                }
            }
        }
        totalDiff = totalDiff + (diff < 0 ? -diff : diff);
        area = area + 1;
    }
    if (totalDiff != 0)
    {
        guard.put_short(index, totalDiff);
        guard.finalize(true);
        G_GameWorld()->send_all(guard);
        PrintWarRoomList();
        cMyTrace(__PRETTY_FUNCTION__, 0x30cf, 0)("WarRoomCountManage end");
    }
}

void CGameManager::WarRoomCountManageTest(int hourIndex)
{
    // b28d-lane12: N82/N93 槽平移封顶（orig=248 our=253 gap=-5；ndiff 307）。
    // ORIG 0x082a2c08。
    // b28d-solo-CGameManager 判词：N78（ORIG `mov $0xa,%edx; rep stos` 内联 vs 4.4
    // `call memset`）+ N82 槽平移。
    // 同 WarRoomCountManage：零初始化声明复现 ORIG `rep stos` 段（P214③/P253，
    // 探针 /tmp/solo_probe4.cpp m4），清 10 dword（40B）。
    int current[10] = {0};
    int total = 0;
    for (int area = 0; area < 10; ++area)
    {
        int n = 0;
        // 同 WarRoomCountManage：peek 直连 cmp，无 need 槽（ORIG 0x082a2dXX 孪生段）。
        while (((CDataManager*)G_CDataManager())
                   ->m_warAreaCounter.GetWarRoomCountAtPeekTime(area) > n)
        {
            WarRoom* room = GetWarRoom(total++);
            if (room == 0)
            {
                LogManager::logFormat(1, "App.cpp",
                                      __PRETTY_FUNCTION__, 0x30e2,
                                      "WarRoomCountManage Error : WarRoom does not exist %d", total);
            }
            else
            {
                if (room->GetState() != -1)
                    current[area] = current[area] + 1;
            }
            n = n + 1;
        }
    }
    int hourTable = hourIndex;
    PacketGuard guard;
    guard.put_header(0, 0x58);
    int index = guard.get_index();
    guard.put_short(0);
    int totalDiff = 0;
    for (int area = 0; area < 10; ++area)
    {
        // ORIG 0x082a2d65..0x082a2d82：G_CDataManager 结果无具名槽（`mov %eax,%ecx`
        // 直用），成员数组二维索引 ⇒ 单条 scaled 读。
        int diff = ((CDataManager*)G_CDataManager())
                       ->m_warAreaCounter.m_warRoomCount[hourTable][area] -
                   current[area];
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            int lastIdx =
                ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetWarRoomCountLastIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(lastIdx--);
                // ORIG 0x082a2dee..0x082a2e12：`room!=0 && state!=-1`（两 && 项 je 同岛）。
                if (room != 0 && room->GetState() != -1)
                {
                    char shut = room->ShutDown();
                    if (shut != 0)
                        room->MakeStateInfo((char*)&guard);
                    need = need - 1;
                }
            }
        }
        else if (diff > 0)
        {
            unsigned int need = (unsigned int)diff;
            int firstIdx =
                ((CDataManager*)G_CDataManager())->m_warAreaCounter.GetWarRoomCountFirstIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(firstIdx++);
                // ORIG 0x082a2e98..0x082a2eba：`room!=0 && state==-1`（je + jne 同岛）。
                if (room != 0 && room->GetState() == -1)
                {
                    room->Open();
                    room->MakeStateInfo((char*)&guard);
                    need = need - 1;
                }
            }
        }
        totalDiff = totalDiff + (diff < 0 ? -diff : diff);
    }
    if (totalDiff != 0)
    {
        guard.put_short(index, totalDiff);
        guard.finalize(true);
        G_GameWorld()->send_all(guard);
        PrintWarRoomList();
    }
}

// ============================================================================
// DB 连接状态同步 / 免 GameGuard 名单
// ============================================================================
void CGameManager::updateDBLinkCharacConnectState(
    unsigned int accId, unsigned int characNo,
    ENUM_CHARAC_LINK_ACTION_TYPE action)
{
    // ORIG：Acquire 结果直接作 CStreamGuard 第二实参（P52/P134 嵌套实参，
    // eax 不落栈槽）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("App.cpp", 0x125b), true);
    (**guard) << 0x1e5;
    (**guard) << -1;
    SIG_UPDATE_LINK_CHARAC_CONNECT_STATE* info =
        guard->GetInBuffer<SIG_UPDATE_LINK_CHARAC_CONNECT_STATE>();
    memset(info, 0, 0xc);
    info->m_accId = accId;
    info->m_characNo = characNo;
    info->m_action = (int)action;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

bool CGameManager::IsNoGameGuardApplingUser(std::string& name, char* addr)
{
    // ORIG 0x82a3544：单表达式比较 → 实参右先求值：end() 临时占深槽 -0x10、
    // find(name) 临时占 -0xc（具名两变量写法槽序相反）
    if (m_noGameGuard.find(name) == m_noGameGuard.end())
        return false;
    return true;
}

// ============================================================================
// 管理器
// ============================================================================
QuickParty::CQuickPartySystemManager* CGameManager::GetQuickPartySystemManager()
{
    if (m_pQuickPartySystemMgr == 0)
    {
        m_pQuickPartySystemMgr = new QuickParty::CQuickPartySystemManager;
    }
    return m_pQuickPartySystemMgr;
}

QuickParty::CQuickPartyRewardManager* CGameManager::GetQuickPartyRewardManager()
{
    if (m_pQuickPartyRewardMgr == 0)
    {
        m_pQuickPartyRewardMgr = new QuickParty::CQuickPartyRewardManager;
    }
    return m_pQuickPartyRewardMgr;
}

CPremiumLetheManager* CGameManager::GetPremiumLetheManager()
{
    if (m_pPremiumLetheMgr == 0)
    {
        m_pPremiumLetheMgr = new CPremiumLetheManager;
    }
    return m_pPremiumLetheMgr;
}

CSharedServerMessageManager* CGameManager::GetSharedServerMessageManager()
{
    if (m_pSharedServerMessageMgr == 0)
    {
        // ORIG lea 0x7d8(dm)：CDataManager 内嵌 stSharedServerMessageInfo。
        // 该结构即 ServerParameterScript::m_shared770（ServerParameterScript.h +0x770，
        // ORIG C1 0x836fed4 实证），m_serverParameter 位于 CDataManager+0x68，
        // 0x68+0x770=0x7d8 与 ORIG lea 偏移一致（不缓存 dmp，对齐 ORIG 寄存器形态）。
        m_pSharedServerMessageMgr = new CSharedServerMessageManager(
            &((CDataManager*)G_CDataManager())->m_serverParameter.m_shared770);
    }
    return m_pSharedServerMessageMgr;
}

CSpecialItemRoutingManager* CGameManager::GetSpecialItemRoutingManager()
{
    if (m_pSpecialItemRoutingMgr == 0)
    {
        m_pSpecialItemRoutingMgr = new CSpecialItemRoutingManager;
    }
    return m_pSpecialItemRoutingMgr;
}

CConditionEventManager* CGameManager::GetConditionEventManager()
{
    if (m_pConditionEventMgr == 0)
    {
        m_pConditionEventMgr = new CConditionEventManager;
    }
    return m_pConditionEventMgr;
}

CAuctionAveragePrice* CGameManager::GetAuctionAveragePriceManager()
{
    if (m_pAuctionAveragePriceMgr == 0)
    {
        m_pAuctionAveragePriceMgr = new (std::nothrow) CAuctionAveragePrice;
    }
    return m_pAuctionAveragePriceMgr;
}

CraneMinigameManager* CGameManager::GetCraneMinigameManager()
{
    return m_pCraneMinigameMgr;
}

// ============================================================================
// 用户信息上报
// ============================================================================

// ORIG 0x829611a _Z25SendUserInfoToUpperServerR33Packet_Monitor_UDP_Reply_UserInfohP5CUser17ENUM_SERVER_GROUP
// 按 type 选择上层代理发送（0xcb→公会代理 / 0xc9→监控代理），
// 发送后清空 pkt+0xc 起的 0x16da 字节负载。CUser* 形参为死参（ORIG 不读取）。
void SendUserInfoToUpperServer(Packet_Monitor_UDP_Reply_UserInfo& pkt,
                               unsigned char type, CUser*, 
                               ENUM_SERVER_GROUP group)
{
    if (type == 0xcb)
    {
        // b28d-lane12: P52——ORIG 0x0829611a 先把 pkt.packetSize/&pkt 物化到
        // %esi/%ebx 并跨 GetServerProxy 调用保活，返回值直传 SendTcpPacket 的 this
        // ⇒ 单条链式表达式（具名 proxy 局部会坍缩为栈槽形）。
        GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)(int)group)
            ->SendTcpPacket((char*)&pkt, pkt.packetSize);  // ORIG *(u16*)(pkt+2)
    }
    else if (type == 0xc9)
    {
        GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)(int)group)
            ->SendTcpPacket((char*)&pkt, pkt.packetSize);
    }
    memset(&pkt.m_items, 0, 0x16da);  // ORIG memset(pkt+0xc,0,0x16da)
}



void CGameManager::Send_userinfos_to_upper_server(unsigned char channelType)
{
    // b28d-lane12 旧 N82 判词（gap 32）已被本轮布局修正推翻。
    // b28d-solo-CGameManager 判词：DIFF·N115/N82（ours 302 / orig 303，gap 1）。
    //   本轮真布局修复：Packet_Monitor_UDP_Reply_UserInfo.h 的
    //   ST_MonitorReplyUserInfo 条目步长 0x52→0x4e（m_job 4B→1B；ORIG
    //   0x08296369..0x082964ca 逐字段 `imul $0x4e,%ebx,%edx` + 1B/2B/4B 写偏移
    //   实证）+ 去 item 引用缓存改逐字段 pkt.m_items[userCount]（P157/P183：
    //   ORIG 每字段重发 imul/lea/sub 全链地址重算）⇒ gap 37→1。
    //   残余 = 4.4 缺 push %esi（ORIG 用 %esi 跨循环缓存 group）+ 帧 0x1764 vs
    //   0x1750（N115：4.4 恒预留 16B 对齐 outgoing）+ 槽号整体平移 0x10（N82）；
    //   P167 CLEAR 43/43。
    int userCount = 0;
    Packet_Monitor_UDP_Reply_UserInfo pkt;
    pkt.m_channel = (unsigned char)((CEnvironment*)G_CEnvironment())->get_channel_no();  // ORIG p[11]
    // b28d-solo-CGameManager: ORIG 0x082961be 两独立守卫（`test %al;je` +
    // `cmpb $0xc9;jne` 同落点）⇒ 拆嵌套 if（P225 推广）；合并 && 会多
    // mov$1/mov$0+test 物化段（实测）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        if (channelType == 0xc9)
        {
            int idx = 0;
            int group = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
            while (idx < GlobalData::s_monitor_proxy_mgr->GetEndIndex())
            {
                Packet_ChannelType ctype;
                // ORIG 写 pkt+0xa/+0xe（12 字节头旧读法 +0xc/+0x10 为误对齐）。
                ctype.m_channel = (unsigned int)((CEnvironment*)G_CEnvironment())->get_channel_no();
                ctype.m_type = (int)G_GameWorld()->GetChannelType();
                // ORIG 0x08296260..0x08296289：packetSize→%esi、&ctype→%ebx 先物化并跨
                // GetServerProxy 调用保活（push %esi），随后才压 SendTcpPacket 实参
                // ⇒ 必须单语句链式 receiver（P52/P23），具名 proxy 会把实参装载推到
                // receiver 之后并丢掉 %esi 独占。
                GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)group)
                    ->SendTcpPacket((char*)&ctype, ctype.packetSize);
                group = GlobalData::s_monitor_proxy_mgr->GetNextIndex(idx);
            }
        }
    }
    // b28d-laneCGM: P223 空 then 占位岛复现——ORIG 0x082962c3 `call empty; test %al;
    //   jne +0x478(nop 岛)` + 循环退出段 `jmp +0x479`（跨过 nop 落 epilogue）。
    //   写 `if (!empty) { body }` 时 4.4 jump-thread 掉 jmp+nop（15 分支 vs ORIG 16）。
    if (m_userByAccId.empty())
        goto L_user_by_acc_id_done;
    {
        int groupIdx = 0;
        int group = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
        while (groupIdx < GlobalData::s_monitor_proxy_mgr->GetEndIndex())
        {
            // b28d-laneCGM 真语义修正（ORIG 0x08296324 `mov %eax,-0x14(%ebp)` 在循环头
            //   **无条件**执行 ⇒ 原码是该轮 `it->second` 的具名 user，不是"最后一个写入
            //   成功者"）：我方旧形 `CUser* lastUser = 0;`（仅 if 体内更新）在末轮
            //   user 不满足条件时把**上一轮**的 user 传给 SendUserInfoToUpperServer，
            //   与 ORIG 传"末轮 user"不同；且 `= 0` 多一条 `movl $0x0,SLOT`（ORIG 裸声明）。
            CUser* user;
            std::map<unsigned int, CUser*>::iterator it = m_userByAccId.begin();
            for (; it != m_userByAccId.end(); ++it)
            {
                user = it->second;
                if (user->get_state() >= 2 &&
                    (int)(size_t)user->GetServerGroup() == group)
                {
                    // b28d-solo-CGameManager 修法（P157/P183）：ORIG 每字段重发
                    // `mov -0x18(%ebp),%ebx; imul $0x4e,%ebx,%edx; lea; sub $K;
                    // mov disp(%edx)` 全链地址重算（0x08296369..0x082964ca 逐字段
                    // 实证），无 item 引用缓存 ⇒ 逐字段写 pkt.m_items[userCount]。
                    pkt.m_items[userCount].m_dbid = user->get_acc_id();
                    pkt.m_items[userCount].m_charNo = user->get_charac_no(-1);
                    pkt.m_items[userCount].m_channel = user->GetUID();  // 游戏端写 UID（见包头注释）
                    pkt.m_items[userCount].m_guildKey = user->get_charac_guildkey();
                    pkt.m_items[userCount].m_job = user->get_charac_job();
                    pkt.m_items[userCount].m_grade = user->getCurCharacGrowType();
                    pkt.m_items[userCount].m_level = user->get_charac_level();
                    pkt.m_items[userCount].m_memberKey = user->get_charac_memberkey();
                    strcpy(pkt.m_items[userCount].m_name, user->getCurCharacName());
                    // b28d-laneCGM 字段写序照 ORIG 0x08296497..0x0829651f：getSex →
                    //   GetSsnString/memcpy(6) → GetPeerIP2/strcpy(ip)（原形把 ip 提前，
                    //   fn_diff 归一化后仍差 20 条块序）。
                    pkt.m_items[userCount].m_sex = user->getSex();
                    memcpy(pkt.m_items[userCount].m_ssn, user->GetSsnString(), 6);
                    char ipBuf[16];
                    user->m_network.GetPeerIP2(ipBuf, 0x10);  // ORIG (CNetwork*)(user+0xe0)
                    strcpy(pkt.m_items[userCount].m_ip, ipBuf);
                    userCount = userCount + 1;
                    if (userCount > 0x4a)
                    {
                        pkt.packetSize = 0x16e6;
                        pkt.m_count = (unsigned char)userCount;  // ORIG p[10]
                        SendUserInfoToUpperServer(pkt, channelType, user, (ENUM_SERVER_GROUP)group);
                        userCount = 0;
                    }
                }
            }
            if (userCount != 0)
            {
                pkt.packetSize = (unsigned short)(userCount * 0x4e + 0xc);
                pkt.m_count = (unsigned char)userCount;
                SendUserInfoToUpperServer(pkt, channelType, user, (ENUM_SERVER_GROUP)group);
                userCount = 0;
            }
            group = GlobalData::s_monitor_proxy_mgr->GetNextIndex(groupIdx);
        }
    }
L_user_by_acc_id_done:;
}

void CGameManager::send_userinfos_to_cutoff_server()
{
    // [SweepCGameManager2 本轮] 槽序翻正：bucketIdx 提至函数头声明（P212），
    //   user(-0x14 浅)/bucketIdx(-0x18 深) 与 ORIG 逐槽一致；残余仅 `% 10`
    //   无符号取模展开——ORIG `shr $0x3,%edx` 商留寄存器 vs 4.4 商落槽
    //   （`mov %edx,%eax; shr; mov %eax,-0x18; mov -0x18,%edx`，+4 条）⇒ N。
    // b28d-solo-CGameManager 判词：DIFF·N82（ours 180 / orig 177，gap 3）。
    //   语义缺口已清零（p167 CLEAR 16/16）：删发明 placement-new 循环、accId 去具名、
    //   去 bucket 指针缓存（P183）、empty() 早返（P16）、字段写序照 ORIG。
    //   残余 = STTempUsers buckets[10] 对象组槽基差 + 第二循环表寻址 shl/lea 折叠
    //   （P311 族）——4.4 槽分区不可控。
    //   [b28d-solo 本波翻正] 写目标改 `buckets[bucketIdx].m_ids[bucket->m_count]`
    //   复现 ORIG 0x08295f87..0x08295f93 重算基址形
    //   （`imul $0x259,%esi,%edx; add %ebx,%edx; mov %eax,-0x6904(%ebp,%edx,4)`；
    //   原 `bucket->m_ids[...]` 产 `mov %edx,0x4(%eax,%ebx,4)`）；余差仅
    //   bucketIdx/bucket 槽号互换与 %esi 装载（N82）。
    // b28d-solo-CGameManager: ORIG 0x08295e84（178 insn）。修法（P167+P0+P183）：
    //   ①删发明 placement-new 循环（ORIG 无 _ZnwjPv/重复 ctor，P167 EXTRA）；
    //   ②accId 去具名 → get_acc_id() 两次表达式直读（ORIG 两处 call，P0）；
    //      m_count 索引不去具名 n（ORIG 经 %ebx 无槽）；
    //   ③第二循环去 bucket 指针缓存（ORIG 每访问重发 imul/lea/sub，P183）；
    //   ④empty() 改早返（ORIG call;test;jne <epilogue>，P16）；
    //   ⑤字段写序照 ORIG：packetSize→m_count→memcpy→m_connNo→SendPacket(,pkt.packetSize)。
    struct STTempUsers
    {
        unsigned short m_count;
        int m_ids[600];
        STTempUsers() { m_count = 0; }
    };
    STTempUsers buckets[10];
    // P212：ORIG user(-0x14 浅) / bucketIdx(-0x18 深) ⇒ 全头部先声明 bucketIdx。
    unsigned int bucketIdx;
    Packet_CutOff_UDP_Reply_UserInfo pkt;
    pkt.m_channel = (unsigned char)((CEnvironment*)G_CEnvironment())->get_channel_no();
    pkt.m_group = (unsigned char)((CEnvironment*)G_CEnvironment())->m_serverEnvir.m_serverGroup;
    if (m_userByAccId.empty())
        return;
    std::map<unsigned int, CUser*>::iterator it = m_userByAccId.begin();
    for (; it != m_userByAccId.end(); ++it)
    {
        CUser* user = it->second;
        if (user->get_state() > 1)
        {
            bucketIdx = user->get_acc_id() % 10;
            STTempUsers* bucket = &buckets[bucketIdx];
            // ORIG 0x08295f87..0x08295f93 写目标用重算基址
            // （`imul $0x259,%esi,%edx; add %ebx,%edx; mov %eax,-0x6904(%ebp,%edx,4)`），
            // 仅 m_count 读走 bucket 指针（-0x10）⇒ 目标写成 buckets[bucketIdx]。
            buckets[bucketIdx].m_ids[bucket->m_count] = (int)user->get_acc_id();
            bucket->m_count = bucket->m_count + 1;
        }
    }
    for (int i = 0; i < 10; ++i)
    {
        if (buckets[i].m_count != 0)
        {
            pkt.packetSize = (unsigned short)(buckets[i].m_count * 4 + 0xe);
            pkt.m_count = buckets[i].m_count;
            memcpy(pkt.m_accIds, buckets[i].m_ids, (unsigned int)buckets[i].m_count * 4);
            pkt.m_connNo = buckets[i].m_ids[0];
            GlobalData::s_double_check_proxy->SendPacket((char*)&pkt, pkt.packetSize);
        }
    }
}

// ============================================================================
// 进入游戏世界
// ============================================================================
enum ENUM_HARDCODE_ITEM
{
    ENUM_HARDCODE_ITEM_0 = 0
};

unsigned int CGameManager::insert_game_world(CUser* user)
{
    // b28d-lane12: N82 槽平移封顶（orig=890 our=905 gap=-15；ndiff 831——大帧
    // 局部槽组 + 迭代器/Guard 分区差）。ORIG 0x08296d30。
    // b28d-solo-CGameManager 判词：DIFF·N82（ours 911 / orig 890，gap -21；帧 0x12c
    // vs ORIG 0x10c——大帧槽组分区差）。本轮修法（P0+P214①）：
    //   ①getCurCharacR() 判空去具名局部 ⇒ 复现 ORIG `call; test %eax,%eax;
    //     sete %al; test %al,%al; je`（原 `mov %eax,-0x54; cmpl; jne` 槽化）；
    //   ②bGuildPointDel 三态 if/else-if/else 改短路汇聚形（P214① w2 形）⇒
    //     复现 ORIG `jg L_false; …; jge L_false; mov $1; jmp; L_false: mov $0;
    //     test %al; je`（原 movb 槽 + cmpb 直比）。
    // **P167 CLEAR 193/193**
    // （旧判词记的 MISSING G_TimerQueue×4/getCurCharacR/make_pair<int,int>/
    // GetChannelType/IsEquipSlotSwitchChannel/getPunishTradeAlert/getCurCharacName 与
    // EXTRA getCurCharacLastPlayTick/make_pair<int&,int&>/std::move 已被后续修复清零，
    // 本波复验 0 missing / 0 extra）⇒ 语义闭合、仅剩 byte 层槽形差。
    if (user == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf20,
                              "insert_waiting_room pUser parameter is null.");
        return 0;
    }
    // b28d-solo-CGameManager 修法（P0）：ORIG 0x08296dXX `call getCurCharacR;
    // test %eax,%eax; sete %al; test %al,%al; je` —— 调用结果寄存器直比、无具名
    // 局部槽往返 ⇒ 表达式内联判空（原 `const void* characR = ...` 多 -0x54 槽 +
    // cmpl/jne 两段）。
    if (user->getCurCharacR() == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf27,
                              "Not selected character(a reason is fast returning to character screen on client)");
        return 0;
    }
    int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<std::pair<int, int> > vecA;
    std::vector<std::pair<int, int> > vecB;
    // b28d-solo-CGameManager 修法（P214① 汇聚块 w2 形）：ORIG 0x08296dXX =
    // `call getLastDeleteTime; cmp curSec; jg L_false; call getCurCharacLastPlayTick;
    // mov %eax,%ebx; call getLastDeleteTime; cmp %eax,%ebx; jge L_false; mov $1;
    // jmp; L_false: mov $0; test %al,%al; je` —— 短路链 bool 汇聚（无槽物化）；
    // 原 if/else-if/else 三态赋值恒产 `movb` 槽 + `cmpb $0` 直比。
    // b28d-laneCGM（P0）：ORIG 0x08296dXX 该 bool 无槽往返（`mov $1; jmp; mov $0;
    //   test %al,%al; je` 全在寄存器）⇒ 原码把短路链直接写进 if 条件，不是具名
    //   局部（具名形多 `mov %al,SLOT; cmpb $0x0,SLOT`）。
    if (curSec >= Inter_MonitorGuildPointDel::getLastDeleteTime() &&
        user->getCurCharacLastPlayTick() <
            Inter_MonitorGuildPointDel::getLastDeleteTime())
    {
        // P167：ORIG 调 make_pair<int,int>(int&&,int&&)（0x081b8d41）⇒ 实参为字面量
        // 右值；具名 a/b 左值会实例化 make_pair<int&,int&>（我们旧形 EXTRA）。
        vecA.push_back(std::make_pair(0, 0xc84));
    }
    if (user->getCurCharacGuildSecede() != 0)
    {
        // P167：ORIG 为 `make_pair<ENUM_HARDCODE_ITEM,int>` + 隐式转换构造
        // `pair<int,int>::pair<ENUM,int>(pair<ENUM,int>&&)` + push_back(pair&&)
        // ——匿名右值直传，无具名 p、无 std::move call（我们旧形 EXTRA 两项）。
        vecA.push_back(std::make_pair((ENUM_HARDCODE_ITEM)0, 0x4b0));
        user->setCurCharacGuildSecede(0);
        DB_GuildExpBookDelete::makeRequest((unsigned int)user->get_charac_no(-1), 0, (ENUM_SERVER_GROUP)(size_t)user->GetServerGroup());
    }
    user->ResetItemByScript(vecA);
    int curSec2 = GlobalData::s_systemTime_.getCurSec();
    // P167：ORIG 调 getCurCharacR()（0x08120432）后直读 +0x7b = m_lastPlayTick，
    // 不调 getCurCharacLastPlayTick（旧形 EXTRA +1）。
    // b28d-laneCGM（P0/P183）：lastPlay/env 均只用一次且 ORIG 无槽往返
    //   ⇒ 原码把 getter 表达式直接内联进实参位（具名形多 `mov REG,SLOT` 往返）。
    if (CheckDailyScheduleTime(
            G_CEnvironment()->m_serverEnvir.m_fatigueTime,  // ORIG env+0x37c
            (long)user->getCurCharacR()->m_field7b, curSec2))  // _Charac_info +0x7b（CharLastPlayTick）
    {
        std::vector<std::pair<int, int> > vecC;
        user->deleteDailyStackableItem(vecC, 3, 8);
        user->deleteDailyStackableItem(vecC, 0x39, 0x68);
    }
    user->deleteSpecificItem(vecA, vecB);
    if (!user->send_itemspace(0xc))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf58,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_ACCOUNT_CARGO ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (!user->send_itemspace(0))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf5f,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (!user->send_itemspace(1))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf64,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_AVATAR ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (!user->send_itemspace(2))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf69,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CARGO ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (!user->send_itemspace(7))
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf7a,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CREATURE ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        user->set_charac_guildkey(0);
    }
    if (G_GameWorld()->reach_game_world(user) != 1)
    {
        // b28d-laneCGM（P227）：ORIG 两处 cMyTrace 均为全表达式临时链（实参在 ctor 前
        //   求值），具名 tr/tr2 形使 op() 实参装载落在 ctor 之后（fn_diff 归一化后
        //   仍差 6 条块序）。
        cMyTrace(__PRETTY_FUNCTION__, 0xfa2, 0)(
            "CGameManager::insert_game_world , !G_GameWorld()->reach_game_world( pUser ) , USER : %d(%s)",
            user->get_unique_id() & 0xffff, NumberToString(user->get_acc_id(), 0));
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 4);
        cMyTrace(__PRETTY_FUNCTION__, 0xfab, 0)(
            "from (%d) m_id[%s] uid[%d]", 1,
            NumberToString(user->get_acc_id(), 0), user->get_unique_id() & 0xffff);
        return 0;
    }
    // P321-T1 直证：ORIG 0x08297371 `movsbl %al,%eax`（signed 扩展）；头声明为
    // P321-T1 直证：ORIG 0x08297371 `movsbl %al,%eax`（signed 扩展）；getter 已按
    // ORIG 改为返回 char（CUserCharacInfo.h 广播），此处直用即产 movsbl。
    int eventGrow = user->getCurCharacEventCharacterGrowtype();
    if (eventGrow > 0)
    {
        CEventCharacterHandlerInstance()->_makeEventCharacter(user, 0x28);
    }
    CExpandEquipslot* expandSlot = user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
    // P167 真语义修正：ORIG 0x082973b7 外层条件是 `G_GameWorld()->IsEquipSlotSwitchChannel()`
    // （旧形误用 channelSwitch==1/==0 双分支，且完全不调 IsEquipSlotSwitchChannel）；
    // then 块判 `== 0`（0x082973cd sete 形）、else 块判 `== 1`（0x08297425 cmp$1;sete）。
    if (G_GameWorld()->IsEquipSlotSwitchChannel())
    {
        if (user->getCurChannelEquipslotSwitch() == 0)
        {
            // b28d-laneCGM（P0）：ORIG 该段两次 `movsbl REG,REG` 无槽往返 ⇒ 原码把
            //   getter 直接内联进实参位（具名 char c/d 多 `mov %al,SLOT` 往返）。
            expandSlot->EquipslotSwitch(
                user, 0, user->getCurChannelEquipslotSwitch(),
                user->getCurExpandEquipslotSwitch());
        }
    }
    else
    {
        if (user->getCurChannelEquipslotSwitch() == 1)
        {
            expandSlot->EquipslotSwitch(
                user, 0, user->getCurChannelEquipslotSwitch(),
                user->getCurExpandEquipslotSwitch());
        }
    }
    // P167：ORIG 0x08297472/0x08297484 两处独立 GetChannelType 调用（无具名 channelType）。
    if (G_GameWorld()->GetChannelType() == 0xf ||
        G_GameWorld()->GetChannelType() == 0x10)
    {
        void* mission = user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        ((CMissionList_Charac*)mission)->Update_EnterWorld_event(*user);
    }
    PacketGuard guard;
    guard.put_header(0, 2);
    guard.put_byte(0);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 0);
    guard.finalize(true);
    G_GameWorld()->send_all(guard, user);
    user->send_skill_info();
    guard.clear();
    guard.put_header(0, 2);
    guard.put_byte(1);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 1);
    guard.finalize(true);
    user->Send(guard);
    expandSlot->Send_Expand_Equip_Info(user);
    user->processReturnUserQuestAutoClear();
    user->send_clear_quest_list();
    guard.clear();
    user->getCurCharacQuestR()->get_quest_info((char*)&guard);
    user->Send(guard);
    ((CTitleBook*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14))->sendList();
    ((CAchievement*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)15))->sendList();
    ((item_lock::CItemLock*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)2))->SendItemLockList(user);
    if (G_GameWorld()->GetChannelType() != 0xf)
    {
        SendPvpList(user);
    }
    SendWarRoomList(user);
    user->send_pvp_record();
    SendPartyList(user);
    // P167：ORIG 0x08297738/0x08297749/0x08297796 三处独立 getPunishTradeAlert
    // （条件两处 + put_int 实参一处），无具名 punish 局部。
    if (user->getPunishTradeAlert() == 0xe7 ||
        user->getPunishTradeAlert() == 0xe8)
    {
        guard.clear();
        guard.put_header(0, 0xff);
        guard.put_int((int)user->getPunishTradeAlert());
        guard.finalize(true);
        user->Send(guard);
    }
    guard.clear();
    guard.put_header(0, 0x7c);
    guard.finalize(true);
    user->Send(guard);
    std::map<std::pair<char, char>, int>* pics =
        (std::map<std::pair<char, char>, int>*)user->GetPICSMap();
    pics->clear();
    user->ReCalcChattingEmoticon();
    user->SendChattingEmoticon();
    Packet_Monitor_Char_Info monitor;  // ORIG ctor 0x82a3ffa
    monitor.m_dbid = user->get_acc_id();
    monitor.m_channel = (unsigned char)((CEnvironment*)G_CEnvironment())->get_channel_no();
    monitor.m_channelType = (char)G_GameWorld()->GetChannelType();
    monitor.m_charNo = user->get_charac_no(-1);
    monitor.m_guildKey = user->get_charac_guildkey();
    monitor.m_job = user->get_charac_job();
    monitor.m_grade = user->getCurCharacGrowType();
    monitor.m_level = user->get_charac_level();
    monitor.m_memberKey = user->get_charac_memberkey();
    // P167：ORIG 0x08297913/0x08297928 两处独立 getCurCharacName（strlen 先求值入 %ebx，
    // 再第二次调用作 memcpy 第二实参），无具名 name/nameLen。
    memcpy(monitor.m_name, user->getCurCharacName(),
           (int)strlen(user->getCurCharacName()));
    monitor.m_returnUser = user->IsReturnUserFirstLogin();
    int serverGroup = (int)(size_t)user->GetServerGroup();
    CMonitorServerProxy* monitorProxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)serverGroup);
    monitorProxy->SendTcpPacket((char*)&monitor, 0x3f);
    int guildGroup = (int)(size_t)user->GetServerGroup();
    CGuildServerProxy* guildProxy = GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)guildGroup);
    guildProxy->SendTcpPacket((char*)&monitor, 0x3f);
    g_schoolMgr.AddUser(user);
    if (user->get_charac_guildkey() != 0)
    {
        g_guildAgitMgr.AllocGuildAgitArea(user->get_charac_guildkey());
    }
    // P167 真参数序修正：ORIG 0x08297a0d/0x08297a2e 段实证实参 =
    //   (OBJ_TYPE_0, GetUID(), msg, sec, GetLoginTick(), getCurCharacNo())
    // （旧形第2参写 0、第5/6参颠倒 —— 定时器 idx/key 与 uid/characNo 错位）。
    // 实参右→左求值序：getCurCharacNo→%edi、GetLoginTick→%esi、GetUID→%ebx
    // （三者无栈槽往返 ⇒ 原码内联实参，P0/P24）；sec 为 0x3c/300/600 常量，
    // 第二段为 `get_rand_int(300) + 600`（ORIG 0x08297a85 add $0x258）。
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, user->GetUID(), (TIMER_MESSAGE)2, 0x3c,
        user->GetLoginTick(), user->getCurCharacNo());
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, user->GetUID(), (TIMER_MESSAGE)1,
        get_rand_int(300) + 600, user->GetLoginTick(), user->getCurCharacNo());
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, user->GetUID(), (TIMER_MESSAGE)0x5f, 300,
        user->GetLoginTick(), user->getCurCharacNo());
    G_TimerQueue()->InsertTimer(
        TimerEntry::OBJ_TYPE_0, user->GetUID(), (TIMER_MESSAGE)0x60, 600,
        user->GetLoginTick(), user->getCurCharacNo());
    user->send_ontime_reward_start_notify();
    return 1;
}

// ============================================================================
// 断开 / 退出
// ============================================================================
void CGameManager::user_disconnect(CUser* user)
{
    // b28d-lane12: N82 槽平移封顶（orig=155 our=154 gap=1；ndiff 55；P227 临时链
    // 已上无改善——主体为分支内局部槽组差）。ORIG 0x08298b48。
    // b28d-solo-CGameManager 判词：N82 维持（P167 CLEAR；残差为分支内 cMyTrace/
    // 临时对象槽组分区差）。
    // b28d-solo-CGameManager 真语义 bug 修正（ORIG 0x08298d54/0x08298da1→0x08298da6）：
    //   SendLastMsgDBQueue 是**两分支共享尾部**——state<3 短链末尾 returnUserPool 后
    //   fall-through 到 0x08298da6，state>=3 长链末尾 `jmp 0x08298da6`；原重建把它
    //   写在短链内 ⇒ state>=3 路径漏发。同时改 `state > 2` 正向守卫复现 ORIG
    //   `cmp $2; setg; je <短链>`（原 `< 3` 产 setle + 反向块序）。
    // b28d-solo-CGameManager 第二处真语义 bug（ORIG 0x08298c25 `movl $0x1,0x8(%esp)`）：
    //   长链 CheckOutParty 第二实参为 **true**（原重建写 false；user_exit 同位置已是 true）。
    if (user == 0)
    {
        // b28d-lane12: P227 全表达式临时链（消 N80 具名 ctor 前求值差）。
        cMyTrace(__PRETTY_FUNCTION__, 0x138b, 5)(
            "USER DISCONNECT [%s][%d]", __PRETTY_FUNCTION__, 0x138b);
        return;
    }
    HistoryLog::LogClose((FILE*)user->m_field796f8);
    // ORIG 0x08298bbb：`test %eax,%eax; sete %al; test %al,%al; jne 08298dbc`（08298dbc
    // 为 nop 空岛）——4.1 对 != 的 sete 物化形；4.4 对 if(!=0) 恒产 setne+je。
    // 用 while(x==0){return;} 复现 sete+jne 物化与尾 nop 岛（P225）。
    while (user->get_state() == 0)
    {
        return;
    }
    {
        user->prepareDisconnect();
        G_GameWorld()->EraseLoginUser(user);
        user->SetSaveRentalInfoToExchange(false);
        if (user->get_state() > 2)
        {
            CheckOutTrade(user);
            CheckOutParty(user, true);
            CheckOutPvp(user, false);
            CheckOutWarRoom(user);
            checkOutDeathTower(user);
            checkOutBossTower(user);
            checkOutAdvanceAltar(user);
            checkOutBlueMarble(user);
            G_GameWorld()->leave_game_world(user);
            user->SetGameMasterMode(false);
            user->UpdateData();
            user->LogoutCachedCharacter(0);
            user->UpdateLogout(true);
            user->DeleteRentalItemInfo(1);
            user->DeleteRentalItemInfo(2);
            user->ResetCurCharac();
            user->doLinkCharacDisconnect();
            returnUserPool(user);
        }
        else
        {
            user->SetSaveRentalInfoToExchange(false);
            user->LogoutCachedCharacter(0);
            user->UpdateLogout(true);
            returnUserPool(user);
        }
        // ORIG 0x08298da6：两分支共享尾部（短链 fall-through / 长链 jmp 同址）
        GetInstanceShutdowManager()->SendLastMsgDBQueue(user);
}
}

// ORIG 0x82985a8 _ZN12CGameManager9user_exitEP5CUser：主动登出路径。
// 与 user_disconnect 的差异：state==0 视为异常直接拒绝；state<3 分支
// UpdateLogout(false)（disconnect 为 true）；无 DeleteRentalItemInfo；
// state>=3 分支末尾 ResetEpoll + checkLogOutCorrectly。
bool CGameManager::user_exit(CUser* user)
{
    // b28d-lane12: N82 槽平移封顶（orig=222 our=219 gap=3；ndiff 81；P227 临时链
    // 已上无改善）。ORIG 0x082985a8。
    // b28d-solo-CGameManager 判词：N82 维持（P167 CLEAR；trace 槽 -0x20 vs ORIG -0x3c
    // 等分支内槽组分区差）。
    if (user == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1271, 5);
        tr("USER_EXIT [%s][%d]", __PRETTY_FUNCTION__, 0x1271);
        return false;
    }
    if (user->get_state() == 0)
    {
        // ORIG 0x08298625：get_acc_id 结果先入 %ebx 跨 ctor 保活（无具名槽）⇒ P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x127a, 5)(
            "USER_EXIT STATE NONE [%s][%d]mid(%d)",
            __PRETTY_FUNCTION__, 0x127a, user->get_acc_id());
        return false;
    }
    HistoryLog::LogClose((FILE*)user->m_field796f8);   // ORIG *(user+0x796f8)
    user->prepareDisconnect();
    G_GameWorld()->EraseLoginUser(user);
    // ORIG 0x082986b6：`cmp $0x2,%eax; setg %al; test %al; je <尾部else块>` —— 真条件
    // 是 state>2（CheckOut 族 fall-through，SetSaveRental 块落函数尾），Ghidra C 的
    // `iVar2 < 3` 块序误读；必须按 ORIG 极性写 >2 才复现（P59/P222）。
    if (user->get_state() > 2)
    {
        CheckOutTrade(user);
        CheckOutParty(user, true);
        CheckOutPvp(user, false);
        CheckOutWarRoom(user);
        checkOutDeathTower(user);
        checkOutBossTower(user);
        checkOutAdvanceAltar(user);
        checkOutBlueMarble(user);
        G_GameWorld()->leave_game_world(user);
        user->SetGameMasterMode(false);
        user->UpdateData();
        user->LogoutCachedCharacter(0);
        user->UpdateLogout(false);
        // ORIG 0x082987c2/0x082987d0：get_unique_id→%esi、getCurCharacName→%ebx
        // 在 ctor 前求值保活 ⇒ P227 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0x12dc, 0)(
            "ID: %s - UID: %d from Logout",
            user->getCurCharacName(), user->get_unique_id() & 0xffff);
        // ORIG 0x08298815：err 先预清零（movl $0x0,-0xc）再独立赋值（P198①/P197③ 两句式）。
        int err = 0;
        err = G_EpollHandler()->ResetEpoll(user->m_network.get_socket());
        // ORIG 0x08298842：`cmpl $0x0,-0xc(%ebp); setne %al; test %al,%al; je` ⇒ 4.1 的
        // bool 物化四段形，用 while+break 复现（P225）。
        while (err != 0)
        {
            LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x12e3,
                                  "(m_id: %s) G_EpollHandler()->ResetEpoll(pUser->get_socket(%d)) %d(%s)",
                                  NumberToString(user->get_acc_id(), 0),
                                  user->m_network.get_socket(), err, strerror(err));
            break;
        }
        user->ResetCurCharac();
        user->doLinkCharacDisconnect();
        returnUserPool(user);
        user->checkLogOutCorrectly();
    }
    else
    {
        user->SetSaveRentalInfoToExchange(false);
        user->LogoutCachedCharacter(0);
        user->UpdateLogout(false);
    }
    return true;
}

CGameManager::CGameManager()
    : m_mutex(), m_noGameGuard(), m_gameGuard2(),
      m_userPool(), m_tradeSpacePool(), m_partyPool(), m_pvpRoomPool(),
      m_warRoomPool(), m_deathTowerPool(), m_bossTowerPool(), m_stagePool(),
      m_quickPartyPool(), m_blueMarblePool(), m_userByAccId(), m_userByAccId2(),
      m_pvpRoomMap(), m_partyMap(), m_warRoomMap(), m_deathTowerMap(), m_bossTowerMap(),
      m_stageMap(), m_quickPartyMap(), m_blueMarbleMap()
{
	// ORIG 0x82930c2：标量初始化全部在函数体内（初始化列表只含成员对象）：
	// 成员构造后先 movw $0,0x18（m_uid），随后向 m_noGameGuard（+0x1c）逐条
	// insert 14 个固定账号/IP 字符串（0x8c167b4..0x8c16833 实证，各插一条独立
	// std::string 临时），最后 8 个管理器指针（+0x2fc..+0x318）清零。
	m_uid = 0;
	m_noGameGuard.insert(std::string("testman820"));
	m_noGameGuard.insert(std::string("tilil"));
	m_noGameGuard.insert(std::string("azzuman"));
	m_noGameGuard.insert(std::string("cuwaki"));
	m_noGameGuard.insert(std::string("slainer"));
	m_noGameGuard.insert(std::string("mrsbscom"));
	m_noGameGuard.insert(std::string("arandra"));
	m_noGameGuard.insert(std::string("krucef"));
	m_noGameGuard.insert(std::string("dnftest126"));
	m_noGameGuard.insert(std::string("dnftest127"));
	m_noGameGuard.insert(std::string("dnftest128"));
	m_gameGuard2.insert(std::string("59.10.138.244"));
	m_gameGuard2.insert(std::string("218.145.141.85"));
	m_gameGuard2.insert(std::string("121.134.32.42"));
	m_pQuickPartySystemMgr = 0;
	m_pQuickPartyRewardMgr = 0;
	m_pPremiumLetheMgr = 0;
	m_pSharedServerMessageMgr = 0;
	m_pConditionEventMgr = 0;
	m_pSpecialItemRoutingMgr = 0;
	m_pAuctionAveragePriceMgr = 0;
	m_pCraneMinigameMgr = 0;
}
CGameManager::~CGameManager()
{
    // b28d-solo-CGameManager 判词：AE（compare_tu 无 DIFF/NEAR；清单 ndiff 1 已消，
    // 余量为 nop 对齐豁免 §6.1）。ORIG 0x08293d1a；无改码。
    //（crane +0x318 不置空），其余容器/池成员（hash_map、StaticPool、
    // std::map/set、Mutex）按声明逆序自动析构。
    if (m_pQuickPartySystemMgr != 0)
        delete m_pQuickPartySystemMgr;
    m_pQuickPartySystemMgr = 0;
    if (m_pQuickPartyRewardMgr != 0)
        delete m_pQuickPartyRewardMgr;
    m_pQuickPartyRewardMgr = 0;
    // ORIG 0x08293d9b `je 08293dc7`（跳过 0x08293dba 的置零）⇒ 本字段的置零在 if
    // 块**内**（与 0x2fc/0x300 的块外置零不同）；jcc 目标下标判据实证。
    if (m_pAuctionAveragePriceMgr != 0)
    {
        delete m_pAuctionAveragePriceMgr;
        m_pAuctionAveragePriceMgr = 0;
    }
    if (m_pCraneMinigameMgr != 0)
        delete m_pCraneMinigameMgr;
    // 其余容器/池成员（hash_map、StaticPool、std::map/set、Mutex）按声明逆序自动析构。
}
