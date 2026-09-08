#include "GameRand.h"
#include "CShutdowManager.h"   // GetInstanceShutdowManager 完整类型（SendLastMsgDBQueue 调用）
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
CSchoolMgr g_schoolMgr;           // ORIG 全局对象（nm -S BSS size 0x58），原 GameStubs.cpp 定义迁移
CGuildAgitManager* g_guildAgitMgr;  // ORIG 全局，原 GameStubs.cpp 定义迁移
namespace GlobalData
{
extern TimerQueue* s_timerQueue_;
}


char* NumberToString(unsigned int value, int radix);
CUserGlobalInfoHandle* CUserGlobalInfoHandleInstance();
CEventCharacterHandler* CEventCharacterHandlerInstance();
CShutdowManager* GetInstanceShutdowManager();
EpollHandler* G_EpollHandler();
extern CSchoolMgr g_schoolMgr;
extern CGuildAgitManager* g_guildAgitMgr;
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

// ============================================================================
// StaticPool<T,N> —— ORIG 真实实现（objdump 0x82ae390 起逐函数核对）
// ============================================================================
template <class T, int N>
StaticPool<T, N>::StaticPool()
{
    m_pNode = new CNode[N];
    if (m_pNode == 0)
    {
        // ORIG C1 实证：trace 落浅槽 -0x30（push 临时对象占 -0x58），临时对象形态
        cMyTrace(__FUNCTION__, 0x6c, 5)("[StaticPool] %s(%d) new fail", "StaticPool.cpp", 0x6c);
    }
    for (int i = 0; i < N; ++i)
        m_Queue.push(&m_pNode[i]);
    m_nErrorCode = 0;
}

template <class T, int N>
StaticPool<T, N>::~StaticPool()
{
    // b28c-L5 判词（N91 族·delete[] cleanup 侧）：ORIG 三池 D1（CTradeSpace<300>
    //   0x082ae79c / pvp_assault::CAssaultPlace<128> 0x082f141c /
    //   exchange_server::CSession<300> 0x0849584c，各 37 insns）**均无** EH landing
    //   pad；4.4-O0 对本 TU 发射的弱 `CNodeD1Ev` 数组析构循环恒追加
    //   「异常回滚 → m_Queue.~queue() → _Unwind_Resume」残段（+13 insns，
    //   含 push %esi），与函数体正常路径逐条同构、仅尾段 EH 多寡不同。
    //   已穷举两条无副作用翻法并实测否证（探针 /tmp/l5_probe_b.cpp 同 TU 三态）：
    //     ① 元素类 T 的 dtor 加 throw()（SP<B>）→ landing **不消**（4.4 的 cleanup
    //        判据取数组元素析构调用的可抛性，不受 T 声明影响）；
    //     ② CNode ctor 加 throw()（SP2<C>，本波已落 StaticPool.h，用于消 C1 侧
    //        new[] 的 operator delete[] 回滚）→ 对 D1 侧 landing **无效**。
    //   第三条路（CNode 用户定义 `~CNode() throw() {}`）会连带让 POD 元素池
    //   （StaticPool<_IO_FILE,50>，ORIG nm 实证无 CNodeD1Ev、其 D1 仅 0x2c 无循环）
    //   凭空多出发明的 CNodeD1Ev 符号 + 逐元素析构循环 ⇒ 违 §4.1（禁发明符号）
    //   且把 1 件 DIFF 换成 2 处结构性回归，否决。
    //   语义侧 delete[] 的循环/cookie/释放三段与 ORIG 逐条一致，判 N91 封顶。
    delete[] m_pNode;
}

template <class T, int N>
T* StaticPool<T, N>::Acquire()
{
    if (m_Queue.empty())
        return 0;
    CNode* pNode = m_Queue.front();
    m_Queue.pop();
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        // ORIG 0x082b0351 实证：临时对象形态（非具名局部）→ magic trace 槽 -0x2c、
        // bUse trace 槽 -0x1c（具名写法槽序相反）
        cMyTrace(__FUNCTION__, 0x89, 0)("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 3;
    }
    if (pNode->bUse != 0)
    {
        register int use = pNode->bUse;
        cMyTrace(__FUNCTION__, 0x8f, 0)("[StaticPool] bUse(%d) node(%p)", use, pNode);
        m_nErrorCode = 4;
    }
    // ORIG `lea 0x1(%eax),%edx` = 后缀 ++（读改写单表达式，探针 t5 实证）
    pNode->bUse++;
    return pNode;
}

template <class T, int N>
T* StaticPool<T, N>::Get(int index)
{
    if (index < 0 || index > N - 1)
        return 0;
    CNode* pNode = &m_pNode[index];
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        // ORIG 0x82b0xxx 实证：临时对象形态使 magic trace 槽落 -0x2c、bUse 落 -0x1c
        cMyTrace(__FUNCTION__, 0xc3, 0)("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 7;
    }
    if (pNode->bUse != 1)
    {
        if (pNode->bUse != 0)
        {
            register int use = pNode->bUse;
            cMyTrace(__FUNCTION__, 0xca, 0)("[StaticPool] bUse(%d) node(%p)", use, pNode);
            m_nErrorCode = 8;
        }
    }
    return pNode;
}

template <class T, int N>
void StaticPool<T, N>::Free(T* p)
{
    // ORIG 0x082b082a 实证（探针 t8 全等）：null 检查块先于 pNode 声明、
    // trace 全临时对象且实参直接写表达式（无具名中间量）→
    // 槽序 trnull-0x38/pNode-0x3c/trm-0x28/trb-0x18 + %esi 前置装载 + -- 单装载尾
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

template <class T, int N>
int StaticPool<T, N>::GetIndex(T* p)
{
    if ((unsigned int)p < (unsigned int)m_pNode)
        return -1;
    int diff = (int)((char*)p - (char*)m_pNode);
    int index = 0;
    if (diff != 0)
        index = (unsigned int)diff / sizeof(CNode);
    CNode* pNode = (CNode*)p;
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        // ORIG 实证：临时对象形态 → magic trace 槽 -0x34、bUse 槽 -0x24（具名相反）
        cMyTrace(__FUNCTION__, 0x50, 0)("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 1;
    }
    if (pNode->bUse != 1)
    {
        register int use = pNode->bUse;
        cMyTrace(__FUNCTION__, 0x56, 0)("[StaticPool] bUse(%d) node(%p)", use, pNode);
        m_nErrorCode = 2;
    }
    if (index > N - 1)
        return -1;
    return index;
}

template <class T, int N>
int StaticPool<T, N>::GetLastErrorCode()
{
    return m_nErrorCode;
}

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
    m_vptr = 0x08c2f4a8;  // ORIG 0x08338f74：movl $0x8c2f4a8,(%eax)
}
user_creature::CCreature::~CCreature()
{
    m_vptr = 0x08c2f4c0;  // ORIG 0x83370a8：movl $0x8c2f4c0,(%eax)
}
template <> DynamicPool<user_creature::CEgg>::CNode::CNode()
{
    this->m_vptr = 0x08c2f478;
    m_type = 0x8f21;
    m_inUse = 0;
}
template <> DynamicPool<user_creature::CEgg>::CNode::~CNode()
{
    this->m_vptr = 0x08c2f478;
}
template <> DynamicPool<user_creature::CCreature>::CNode::CNode()
{
    this->m_vptr = 0x08c2f490;
    m_type = 0x8f21;
    m_inUse = 0;
}
template <> DynamicPool<user_creature::CCreature>::CNode::~CNode()
{
    this->m_vptr = 0x08c2f490;
}

// ============================================================================
// 显式实例化（本 TU 池成员使用）
// ============================================================================
template class StaticPool<CUser, 600>;
template class StaticPool<CTradeSpace, 300>;
template class StaticPool<CParty, 600>;
template class StaticPool<PvP_Room, 600>;
template class StaticPool<WarRoom, 40>;
template class StaticPool<WongWork::CDeathTower, 600>;

template class StaticPool<WongWork::CBossTower, 600>;
template class StaticPool<advancealtar::StageControl, 600>;
template class StaticPool<QuickParty::CQuickParty, 300>;
template class StaticPool<BlueMarble, 300>;
template class StaticPool<CACHE_CHARACTER_TYPE, 1000>;
// StaticPool<_IO_FILE, 50> 为全特化（见 StaticPool.h），此处不可显式实例化。
template class StaticPool<expert_job::CEnchanter, 300>;
template class StaticPool<expert_job::CDisjointer, 600>;
template class StaticPool<pvp_assault::CAssaultPlace, 128>;
template class StaticPool<private_store::CPrivateStore, 300>;
template class StaticPool<exchange_server::CSession, 300>;
template class StaticPool<online_preliminary::COnlinePreliminaryTeam, 600>;
template class StaticPool<WongWork::CMailBox, 600>;
template class StaticPool<WongWork::CMailBox::CMail, 20>;

// DynamicPool 显式实例化
template class DynamicPool<PacketBuf>;
template class DynamicPool<user_creature::CEgg>;
template class DynamicPool<user_creature::CCreature>;
template class DynamicPool<WongWork::Avatar_Item>;

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
    // b28d-lane12: P227 全表达式临时链 55→9（ORIG 0x082948c6）；残余 2 处 -0xc 槽
    // （NumberToString 结果缓存槽，4.4 分区差）→ N82附。
    std::map<unsigned int, CUser*>::iterator it;
    it = m_userByAccId.find(accId);
    CUser* user;
    if (it == m_userByAccId.end())
    {
        user = 0;
    }
    else
    {
        user = it->second;
        if (user->get_acc_id() != accId)
        {
            // b28d-lane12: P227 全表达式临时链（N80 突破口）——ORIG 0x08294xxx
            // 在 cMyTrace ctor 前右起求 varargs（mapId 先、inId 后）并跨 ctor 保活。
            cMyTrace(__PRETTY_FUNCTION__, 0x9ab, 5)(
                "[%s][%d][IN m_id: %s][Map m_id: %s]",
                __PRETTY_FUNCTION__, 0x9ab,
                NumberToString(accId, 0), NumberToString(user->get_acc_id(), 1));
            user = 0;
        }
    }
    return user;
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
    // ORIG 0x0829487c 三处 `je <ret_user>` 同落点（P133 独立守卫）；goto 尾标签形
    // 残余 2 条 4.4 goto 中继 jmp（P223 单源 goto 恒合并，N 封顶）。
    if (incre == id)
        goto ret_user;
    if (incre == 0)
        goto ret_user;
    if (id == 0)
        goto ret_user;
    return 0;
ret_user:
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
    CUser* user;
    {
        Guard<Mutex> guard(&m_mutex);
        user = m_userPool.Acquire();
    }
    if (user == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x8e3, 5);
        tr("createUser Failed! need to restart!");
        return 0;
    }
    unsigned int uid = user->GetUID();
    unsigned int accId = user->get_acc_id();
    check_user_var(user);
    int idx = GetIdx(user);
    user->SetSlotIDX((short)idx);
    int err = m_userPool.GetLastErrorCode();
    if (err != 0)
    {
        unsigned int newUid = user->GetUID();
        const char* lastId = NumberToString(accId, 0);
        cMyTrace tr(__PRETTY_FUNCTION__, 0x8f6, 0);
        tr("STATIC MEMORY_POOL createUser error(%d), user(%x), last user m_acc_id(%s), idx(%d), new user idx(%d)",
           err, user, lastId, uid, newUid);
    }
    short nxt = getNextUID();
    user->SetIncreID(nxt);
    unsigned short uniqueId =
        CUserGlobalInfoHandleInstance()->get_uniqueid();
    user->set_unique_id(uniqueId);
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
    if (user != 0)
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
        Guard<Mutex> guard(&m_mutex);
        m_userPool.Free(user);
        int err = m_userPool.GetLastErrorCode();
        if (err != 0)
        {
            const char* lastId = NumberToString(accId3, 0);
            cMyTrace tr(__PRETTY_FUNCTION__, 0xa0e, 0);
            tr("STATIC MEMORY_POOL returnUserPool error(%d), user(%x), last user m_acc_id(%s), idx(%d)",
               err, user, lastId, uid);
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
    int eraseKey, key;
    if (party == 0)
        return;
    key = party->GetPartyIndex();
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.find(key);
    if (it != m_partyMap.end())
    {
        eraseKey = party->GetPartyIndex();
        m_partyMap.erase(eraseKey);
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
    if (party != 0)
        return (bool)(G_CGameManager()->GetQuickPartySystemManager()->cancel_quick_party(party, flag, 0) ^ 1);
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
    PvP_Room* room = m_pvpRoomPool.Acquire();
    int roomIdx;
    if (room == 0)
        return 0;
    room->SetIDX(m_pvpRoomPool.GetIndex(room));
    int roomIndex = room->get_index();
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(roomIndex);
    if (it == m_pvpRoomMap.end())
    {
        roomIdx = room->get_index();
        CSwitchLog(__PRETTY_FUNCTION__, 0xaa9, 0, 0)("pvp@log room(%d) new alloc", roomIdx);
        m_pvpRoomMap[room->get_index()] = room;
        return room;
    }
    else
    {
        roomIdx = room->get_index();
        CSwitchLog(__PRETTY_FUNCTION__, 0xab2, 0, 0)("pvp@log room(%d) already alloc", roomIdx);
        return 0;
    }
}

PvP_Room* CGameManager::GetPvp(int index, CUser* user, int param)
{
    // b28d-lane12: CSwitchLog 改临时链（P221）60→42；残余 CSwitchLog/find sret/
    // name/accName 槽平移 → N82附。ORIG 0x08295206。
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(index);
    PvP_Room* room;
    if (it == m_pvpRoomMap.end())
    {
        room = 0;
    }
    else
    {
        int roomIdx = it->second->get_index();
        const char* name;
        const char* accName;
        if (user == 0)
        {
            name = "";
            accName = "";
        }
        else
        {
            name = user->getCurCharacName();
            accName = user->get_acc_name();
        }
        CSwitchLog(__PRETTY_FUNCTION__, 0xacc, 0, 0)(
            "pvp@log %s,%s, room(%d), %d", accName, name, roomIdx, param);
        room = it->second;
    }
    return room;
}

int CGameManager::GetPvpIdx(PvP_Room* room)
{
    int idx = m_pvpRoomPool.GetIndex(room);
    return idx;
}

void CGameManager::PutPvp(PvP_Room* room)
{
    // b28d-lane12: CSwitchLog 改临时链 21→17；残余 CSwitchLog/key/it sret 槽平移
    // → N82附。ORIG 0x08295348。
    int key, eraseKey;   // P212 头部声明对齐 find sret 缓冲槽（同 PutParty）
    if (room != 0)
    {
        // ORIG：roomIndex 经 ebx callee-saved 跨 CSwitchLog 构造缓存（P87）；
        // bare `register`（无 asm 绑定，P0 合规——仅槽+ebx 装载形）。
        register int roomIndex = room->get_index();
        CSwitchLog(__PRETTY_FUNCTION__, 0xae4, 0, 0)("pvp@log room(%d)", roomIndex);
        key = room->get_index();
        __gnu_cxx::hash_map<int, PvP_Room*,
            __gnu_cxx::hash<int>, std::equal_to<int>,
            std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(key);
        if (it != m_pvpRoomMap.end())
        {
            eraseKey = room->get_index();
            m_pvpRoomMap.erase(eraseKey);
            m_pvpRoomPool.Free(room);
        }
    }
}

void CGameManager::CheckOutPvp(CUser* user, bool flag)
{
    // b28d-lane12: N82 槽平移封顶（orig=210 our=210 gap=0；ndiff 270 全为槽号——
    // PacketGuard/CSwitchLog/leave_room 局部槽组）。ORIG 0x08297eaa。
    if (user->CheckInPvp() == 1)
    {
        PvP_Room* room = GetPvp((int)user->GetPvpIndex(), user, 0);
        if (room != 0)
        {
            PacketGuard guard;
            bool outFlag = false;
            int ret = room->leave_room(user, outFlag);
            int outFlagInt = outFlag;
            int battleMode = room->get_pvp_battle_mode();
            int roomIdx = room->get_index();
            const char* characName = user->getCurCharacName();
            const char* accName = user->get_acc_name();
            {
                CSwitchLog log(__PRETTY_FUNCTION__, 0x11a1, 0, 0);
                log("pvp@log %s,%s, room(%d),%d,%d",
                    accName, characName, roomIdx, battleMode, outFlagInt);
            }
            if (ret < 0)
            {
                LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x11bd,
                                      "pvp@log room(%d) user(%s) leave fail",
                                      room->get_index(),
                                      NumberToString(user->get_acc_id(), 0));
            }
            else
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
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.begin();
    // b28d-lane12: ORIG 0x082a31xx 族为 4.1 for 布局（入口 jmp 底部条件）——
    // 与 FindJoinableWarRoom 同形，goto 尾标签复现。
    goto check;
loop_body:
    {
        PvP_Room* room = it->second;
        if (room != 0 && room->get_pvp_battle_mode() != 6)
        {
            // ORIG 0x082a3257-082a3291 极性已核：(quick && pwd) || !joinable →
            // 拒绝；等价 De Morgan：两检查都通过才 return。
            if (!(quick && room->IsExistPassword() == 1) &&
                room->IsQuickJoinablePVPRoom(user) != 0)
                return room;
        }
        ++it;
    }
check:
    if (it != m_pvpRoomMap.end())
        goto loop_body;
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
    // b28d-lane12: 声明序改 eraseKey,key 后 ndiff 12→8（残余同族 -0x10 vs -0xc）。ORIG 0x08295608。
    int eraseKey, key;
    if (qp == 0)
        return;
    key = qp->get_quick_party_index();
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(key);
    if (it != m_quickPartyMap.end())
    {
        eraseKey = qp->get_quick_party_index();
        m_quickPartyMap.erase(eraseKey);
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
    int eraseKey, key;
    if (room == 0)
        return;
    key = room->GetIndex();
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(key);
    if (it != m_warRoomMap.end())
    {
        eraseKey = room->GetIndex();
        m_warRoomMap.erase(eraseKey);
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
    // b28d-lane12: ORIG 0x082a32e2 为 4.1 for 布局（入口 jmp 底部条件；命中
    // 即 `return room`，循环后 `return 0`）——goto 尾标签形复现。
    goto check;
loop_body:
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
check:
    if (it != m_warRoomMap.end())
        goto loop_body;
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
    int eraseKey, key;
    if (tower == 0)
        return;
    key = tower->getIdx();
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(key);
    if (it != m_deathTowerMap.end())
    {
        eraseKey = tower->getIdx();
        m_deathTowerMap.erase(eraseKey);
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
    int eraseKey, key;
    if (tower == 0)
        return;
    key = tower->getIdx();
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(key);
    if (it != m_bossTowerMap.end())
    {
        eraseKey = tower->getIdx();
        m_bossTowerMap.erase(eraseKey);
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
    int eraseKey, key;
    if (control == 0)
        return;
    key = control->getIndex();
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(key);
    if (it != m_stageMap.end())
    {
        eraseKey = control->getIndex();
        m_stageMap.erase(eraseKey);
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
    int eraseKey, key;
    if (marble == 0)
        return;
    key = marble->getMemoryPoolIndex();
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(key);
    if (it != m_blueMarbleMap.end())
    {
        eraseKey = marble->getMemoryPoolIndex();
        m_blueMarbleMap.erase(eraseKey);
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
    CDataManager* dm = (CDataManager*)G_CDataManager();
    BlueMarbleScriptManager* script = (BlueMarbleScriptManager*)dm->getBlueMarbleScript();
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
        goto inner_check;
    inner_body:
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
    inner_check:
        if (it != m_blueMarbleMap.end())
            goto inner_body;
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
    PacketGuard guard;
    guard.put_header(0, 9);
    int count = 0;
    int index = guard.get_index();
    guard.put_short((int)m_partyMap.size());
    char type = 0;
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
                guard.put_byte(party->getTitleIndex() & 0xff);
                if (party->getTitleIndex() == 0)
                {
                    guard.put_int((int)strlen(party->getTitle()));
                    guard.put_str(party->getTitle(), (int)strlen(party->getTitle()));
                }
                guard.put_byte(party->IsReturnUserParty() ? 1 : 0);
                guard.put_byte(party->getUserMax() & 0xff);
                guard.put_short(party->getDungIndex() & 0xffff);
                guard.put_byte(party->getDungDiffi() & 0xff);
                guard.put_byte(party->IsEventCharacParty() ? 1 : 0);
            }
            if (type == 0 || type == 2)
            {
                int managerIdx = 0;
                for (int i = 0; i < 4; ++i)
                {
                    CUser* member = party->get_user(i);
                    if (member == 0)
                    {
                        guard.put_short(0xffff);
                        guard.put_byte(0xff);
                    }
                    else
                    {
                        guard.put_short(member->get_unique_id() & 0xffff);
                        if (party->getManager() == member)
                            managerIdx = i;
                        char sex = member->getSex();
                        if (sex == -1)
                            sex = 1;
                        guard.put_byte((int)sex);
                    }
                }
                guard.put_byte(party->IsReturnUserParty() ? 1 : 0);
                guard.put_byte(managerIdx);
                guard.put_byte(party->is_quick_party() & 0xff);
                guard.put_byte(party->IsEventCharacParty() ? 1 : 0);
            }
            if (type == 0 || type == 1 || type == 2)
            {
                char matching = G_GameWorld()->is_dungeon_tag_matching_channel("[impossible]");
                guard.put_byte((int)matching);
                if (matching != 0)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        CUser* member = party->get_user(i);
                        if (member == 0)
                        {
                            guard.put_byte(0);
                        }
                        else
                        {
                            guard.put_byte(6);
                            for (int j = 0; j < 6; ++j)
                            {
                                char dim = ((CDataManager*)G_CDataManager())->get_dimensionInout(j);
                                guard.put_byte((int)dim);
                                // ORIG *(char*)(characR+0xeb9+j) = _Charac_info
                                // m_demensionInout[j]（CUserCharacInfo::getDemensionInoutValue）。
                                guard.put_byte((int)member->getDemensionInoutValue((char)j));
                            }
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
    // b28d-lane12: ORIG 0x082a2f94 族布局——begin 后无条件 jmp 到底部条件块
    // （4.1 for 循环形态）；4.4 对 while(true)+break 恒条件置顶，故用 goto 尾标签。
    goto check;
loop_body:
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
check:
    if (it != m_pvpRoomMap.end())
        goto loop_body;
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
    // b28d-lane12: 同 SendPvpList——ORIG 4.1 for 布局（入口 jmp 底部条件）。
    goto check;
loop_body:
    {
        WarRoom* room = it->second;
        if (room != 0)
        {
            room->MakeRoomInfo((char*)&guard);
            count = count + 1;
        }
        it++;
    }
check:
    if (it != m_warRoomMap.end())
        goto loop_body;
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
    // b28d-lane12: N82附 槽平移封顶（orig=77 our=77 gap=0；ndiff 82——循环内
    // need/count 标量分区差）。ORIG 0x08298a30。
    void* dm = G_CDataManager();
    int hourTable = ((CDataManager*)dm)->m_warAreaCounter.GetCurrenTimeTable();
    for (int area = 0; area < 10; ++area)
    {
        int count = 0;
        for (;;)
        {
            void* dm2 = G_CDataManager();
            int need = ((CDataManager*)dm2)->m_warAreaCounter.GetWarRoomCountAtPeekTime(area);
            if (need <= count)
                break;
            WarRoom* room = GetWarRoom();
            // ORIG 0x8298a81/+0x05b：直写 +0x00=area、+0x01=count。
            // +0x00 即 WarRoom::m_levelBand 槽（IsJoinable 按 2..6 等级段解释），
            // +0x01 即 m_roomSeq（区内分配序号，MakeRoomInfo 广播）。
            room->m_levelBand = (char)area;
            room->m_roomSeq = (char)count;
            room->Prepare();
            CDataManager* dm3 = (CDataManager*)G_CDataManager();
            // ORIG *(dm+8+(hourTable*10+area+0x223c)*4)：m_warRoomCount 扁平表
            // （WarAreaCounter+0x144，240 项；头文件按 [10][10]+pad 建模）。
            if (count < reinterpret_cast<int*>(
                            dm3->m_warAreaCounter.m_warRoomCount)[hourTable * 10 + area])
                room->SetState((WARROOM_STATE)0);
            else
                room->SetState((WARROOM_STATE)-1);
            count = count + 1;
        }
    }
    PrintWarRoomList();
}

void CGameManager::WarRoomCountAdjustByChannelInfo()
{
    channel_script_t* script = (channel_script_t*)G_CDataManager()->GetChannelScript();
    // ORIG get_channel_no 与 getChannelInfo 各独立 G_CEnvironment()（P183 勿缓存 env）。
    unsigned int channelNo = G_CEnvironment()->get_channel_no();
    channel_info_t* info = script->getChannelInfo(
        (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup, channelNo);
    for (int a = 0; a < 10; ++a)
    {
        for (int b = 0; b < 0x18; ++b)
        {
            // ORIG 内层每轮 **两次** G_CDataManager：iVar4(写表基, 先)→f→iVar5(读乘表)。
            CDataManager* dmW = (CDataManager*)G_CDataManager();
            float f = info->m_gridPercent[a];  // ORIG *(float*)(info+8+(a+4)*4)
            CDataManager* dmR = (CDataManager*)G_CDataManager();
            // 写表：dmW+8+(b*10+a+0x223c)*4 = m_warRoomCount 扁平 [b*10+a]。
            reinterpret_cast<int*>(dmW->m_warAreaCounter.m_warRoomCount)[b * 10 + a] =
                (int)(dmR->m_warAreaCounter.m_counterTable[b] * f);
        }
    }
}

void CGameManager::WarRoomCountManage(int hourIndex)
{
    cMyTrace tr0(__PRETTY_FUNCTION__, 0x306f, 0);
    tr0("WarRoomCountManage Start");
    int current[10];
    // ORIG 0x082a2776：`mov $0xa,%edx; rep stos` = **10 个 dword（40 字节）全清**
    // （旧注"仅清 10 字节"误，current[2..9] 曾留未初始化——真 bug）。
    memset(current, 0, sizeof(current));
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
        void* dm = G_CDataManager();
        hourTable = ((CDataManager*)dm)->m_warAreaCounter.GetCurrenTimeTable();
    }
    else
    {
        hourTable = hourIndex;
    }
    cMyTrace tr1(__PRETTY_FUNCTION__, 0x3088, 0);
    tr1("WarRoomCountManage currentHourIndex %d", hourTable);
    PacketGuard guard;
    guard.put_header(0, 0x58);
    int index = guard.get_index();
    guard.put_short(0);
    int totalDiff = 0;
    int area = 0;
    for (;;)
    {
        if (area > 9)
        {
            if (totalDiff != 0)
            {
                guard.put_short(index, totalDiff);
                guard.finalize(true);
                G_GameWorld()->send_all(guard);
                PrintWarRoomList();
                cMyTrace tr3(__PRETTY_FUNCTION__, 0x30cf, 0);
                tr3("WarRoomCountManage end");
            }
            return;
        }
        CDataManager* dm = (CDataManager*)G_CDataManager();
        // ORIG *(dm+8+(hourTable*10+area+0x223c)*4)：m_warAreaCounter.m_warRoomCount
        // 扁平视图（WarAreaCounter+0x144，[hour][area]）。
        int diff = reinterpret_cast<int*>(
                       dm->m_warAreaCounter.m_warRoomCount)[hourTable * 10 + area] -
                   current[area];
        cMyTrace tr2(__PRETTY_FUNCTION__, 0x3097, 0);
        tr2("WarRoomCountManage diff(%d) currentWarRoomCount(lev%d)", diff, current[area]);
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            void* dm2 = G_CDataManager();
            int lastIdx = ((CDataManager*)dm2)->m_warAreaCounter.GetWarRoomCountLastIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(lastIdx--);
                if (room == 0)
                    break;
                if (room == 0 || room->GetState() == -1)
                ;
                else
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
            void* dm2 = G_CDataManager();
            int firstIdx = ((CDataManager*)dm2)->m_warAreaCounter.GetWarRoomCountFirstIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(firstIdx++);
                if (room == 0)
                    break;
                if (room == 0 || room->GetState() != -1)
                ;
                else
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
}

void CGameManager::WarRoomCountManageTest(int hourIndex)
{
    // b28d-lane12: N82/N93 槽平移封顶（orig=248 our=253 gap=-5；ndiff 307）。
    // ORIG 0x082a2c08。
    int current[10];
    // 同 WarRoomCountManage：ORIG rep stos 清 10 dword（40B），旧 10 字节为真 bug。
    memset(current, 0, sizeof(current));
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
        CDataManager* dm = (CDataManager*)G_CDataManager();
        int diff = reinterpret_cast<int*>(
                       dm->m_warAreaCounter.m_warRoomCount)[hourTable * 10 + area] -
                   current[area];
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            void* dm2 = G_CDataManager();
            int lastIdx = ((CDataManager*)dm2)->m_warAreaCounter.GetWarRoomCountLastIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(lastIdx--);
                if (room == 0 || room->GetState() == -1)
                ;
                else
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
            void* dm2 = G_CDataManager();
            int firstIdx = ((CDataManager*)dm2)->m_warAreaCounter.GetWarRoomCountFirstIndex(area);
            while (need != 0)
            {
                WarRoom* room = G_CGameManager()->GetWarRoom(firstIdx++);
                if (room == 0 || room->GetState() != -1)
                ;
                else
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
    // b28d-lane12: N82 槽平移封顶（orig=303 our=271 gap=32；ndiff 436——大帧 pkt
    // 对象槽基差 + 局部槽组）。ORIG 0x082961be。
    int userCount = 0;
    Packet_Monitor_UDP_Reply_UserInfo pkt;
    pkt.m_channel = (unsigned char)((CEnvironment*)G_CEnvironment())->get_channel_no();  // ORIG p[11]
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() && channelType == 0xc9)
    {
        int idx = 0;
        int group = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
        while (idx < GlobalData::s_monitor_proxy_mgr->GetEndIndex())
        {
            Packet_ChannelType ctype;
            // ORIG 写 pkt+0xa/+0xe（12 字节头旧读法 +0xc/+0x10 为误对齐）。
            ctype.m_channel = (unsigned int)((CEnvironment*)G_CEnvironment())->get_channel_no();
            ctype.m_type = (int)G_GameWorld()->GetChannelType();
            CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)group);
            proxy->SendTcpPacket((char*)&ctype, ctype.packetSize);
            group = GlobalData::s_monitor_proxy_mgr->GetNextIndex(idx);
        }
    }
    if (!m_userByAccId.empty())
    {
        int groupIdx = 0;
        int group = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
        while (groupIdx < GlobalData::s_monitor_proxy_mgr->GetEndIndex())
        {
            CUser* lastUser = 0;
            std::map<unsigned int, CUser*>::iterator it = m_userByAccId.begin();
            for (; it != m_userByAccId.end(); ++it)
            {
                CUser* user = it->second;
                if (user->get_state() >= 2 &&
                    (int)(size_t)user->GetServerGroup() == group)
                {
                    int n = userCount;
                    ST_MonitorReplyUserInfo& item = pkt.m_items[n];  // ORIG p+12+n*0x4e
                    item.m_dbid = user->get_acc_id();
                    item.m_charNo = user->get_charac_no(-1);
                    item.m_channel = user->GetUID();  // 游戏端写 UID（见包头注释）
                    item.m_guildKey = user->get_charac_guildkey();
                    item.m_job = user->get_charac_job();
                    item.m_grade = user->getCurCharacGrowType();
                    item.m_level = user->get_charac_level();
                    item.m_memberKey = user->get_charac_memberkey();
                    strcpy(item.m_name, user->getCurCharacName());
                    char ipBuf[16];
                    user->m_network.GetPeerIP2(ipBuf, 0x10);  // ORIG (CNetwork*)(user+0xe0)
                    strcpy(item.m_ip, ipBuf);
                    item.m_sex = user->getSex();
                    memcpy(item.m_ssn, user->GetSsnString(), 6);
                    lastUser = user;
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
                SendUserInfoToUpperServer(pkt, channelType, lastUser, (ENUM_SERVER_GROUP)group);
                userCount = 0;
            }
            group = GlobalData::s_monitor_proxy_mgr->GetNextIndex(groupIdx);
        }
    }
}

void CGameManager::send_userinfos_to_cutoff_server()
{
    // b28d-lane12: N82 槽平移封顶（orig=177 our=180 gap=-3；ndiff 135）。
    // ORIG 0x08295e84。
    struct STTempUsers
    {
        unsigned short m_count;
        int m_ids[600];
        STTempUsers() { m_count = 0; }
    };
    STTempUsers buckets[10];
    for (int i = 9; i != -1; --i)
        new (&buckets[i]) STTempUsers();
    Packet_CutOff_UDP_Reply_UserInfo pkt;
    pkt.m_channel = (unsigned char)((CEnvironment*)G_CEnvironment())->get_channel_no();
    pkt.m_group = (unsigned char)((CEnvironment*)G_CEnvironment())->m_serverEnvir.m_serverGroup;
    if (!m_userByAccId.empty())
    {
        std::map<unsigned int, CUser*>::iterator it = m_userByAccId.begin();
        for (; it != m_userByAccId.end(); ++it)
        {
            CUser* user = it->second;
            if (user->get_state() > 1)
            {
                unsigned int accId = user->get_acc_id();
                unsigned int bucketIdx = accId % 10;
                STTempUsers* bucket = &buckets[bucketIdx];
                unsigned short n = bucket->m_count;
                bucket->m_ids[n] = (int)accId;
                bucket->m_count = bucket->m_count + 1;
            }
        }
        for (int i = 0; i < 10; ++i)
        {
            STTempUsers* bucket = &buckets[i];
            if (bucket->m_count != 0)  // ORIG *(u16*)(buckets+i*0x964)
            {
                pkt.packetSize = (unsigned short)(bucket->m_count * 4 + 0xe);
                pkt.m_connNo = bucket->m_ids[0];  // ORIG *(u32*)(pkt+6)
                pkt.m_count = bucket->m_count;    // u16 计数（权威头 +0xa）
                memcpy(pkt.m_accIds, bucket->m_ids, (unsigned int)bucket->m_count * 4);  // ORIG pkt+0xe
                GlobalData::s_double_check_proxy->SendPacket(
                    (char*)&pkt, (unsigned int)(bucket->m_count * 4 + 0xe));
            }
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
    if (user == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf20,
                              "insert_waiting_room pUser parameter is null.");
        return 0;
    }
    const void* characR = user->getCurCharacR();
    if (characR == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              __PRETTY_FUNCTION__, 0xf27,
                              "Not selected character(a reason is fast returning to character screen on client)");
        return 0;
    }
    int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<std::pair<int, int> > vecA;
    std::vector<std::pair<int, int> > vecB;
    bool bGuildPointDel = false;
    if (curSec < Inter_MonitorGuildPointDel::getLastDeleteTime())
    {
        bGuildPointDel = false;
    }
    else if (Inter_MonitorGuildPointDel::getLastDeleteTime() <=
             user->getCurCharacLastPlayTick())
    {
        bGuildPointDel = false;
    }
    else
    {
        bGuildPointDel = true;
    }
    if (bGuildPointDel)
    {
        int a = 0;
        int b = 0xc84;
        vecA.push_back(std::make_pair(a, b));
    }
    if (user->getCurCharacGuildSecede() != 0)
    {
        std::pair<int, int> p =
            std::make_pair((ENUM_HARDCODE_ITEM)0, 0x4b0);
        vecA.push_back(std::move(p));
        user->setCurCharacGuildSecede(0);
        DB_GuildExpBookDelete::makeRequest((unsigned int)user->get_charac_no(-1), 0, (ENUM_SERVER_GROUP)(size_t)user->GetServerGroup());
    }
    user->ResetItemByScript(vecA);
    int curSec2 = GlobalData::s_systemTime_.getCurSec();
    // ORIG *(long*)(getCurCharacR()+0x7b) = _Charac_info::m_lastPlayTick
    //（CharacInfoFields 镜像，CUserCharacInfo::getCurCharacLastPlayTick 同址）。
    long lastPlay = user->getCurCharacLastPlayTick();
    CEnvironment* env = G_CEnvironment();
    if (CheckDailyScheduleTime(
            env->m_serverEnvir.m_fatigueTime, lastPlay, curSec2))  // ORIG env+0x37c
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
        cMyTrace tr(__PRETTY_FUNCTION__, 0xfa2, 0);
        tr("CGameManager::insert_game_world , !G_GameWorld()->reach_game_world( pUser ) , USER : %d(%s)",
           user->get_unique_id() & 0xffff, NumberToString(user->get_acc_id(), 0));
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 4);
        cMyTrace tr2(__PRETTY_FUNCTION__, 0xfab, 0);
        tr2("from (%d) m_id[%s] uid[%d]", 1,
            NumberToString(user->get_acc_id(), 0), user->get_unique_id() & 0xffff);
        return 0;
    }
    int eventGrow = (int)user->getCurCharacEventCharacterGrowtype();
    if (eventGrow > 0)
    {
        CEventCharacterHandlerInstance()->_makeEventCharacter(user, 0x28);
    }
    CExpandEquipslot* expandSlot = user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
    if (user->getCurChannelEquipslotSwitch() == 1)
    {
        char c = user->getCurExpandEquipslotSwitch();
        char d = user->getCurChannelEquipslotSwitch();
        expandSlot->EquipslotSwitch(user, 0, d, c);
    }
    else
    {
        if (user->getCurChannelEquipslotSwitch() == 0)
        {
            char c = user->getCurExpandEquipslotSwitch();
            char d = user->getCurChannelEquipslotSwitch();
            expandSlot->EquipslotSwitch(user, 0, d, c);
        }
    }
    int channelType = G_GameWorld()->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
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
    short punish = user->getPunishTradeAlert();
    if (punish == 0xe7 || punish == 0xe8)
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
    const char* name = user->getCurCharacName();
    int nameLen = (int)strlen(name);
    memcpy(monitor.m_name, name, nameLen);
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
        g_guildAgitMgr->AllocGuildAgitArea(user->get_charac_guildkey());
    }
    int characNo = user->getCurCharacNo();
    int loginTick = user->GetLoginTick();
    unsigned int uid = user->GetUID();
    GlobalData::s_timerQueue_->InsertTimer(
        TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)2, 0x3c, loginTick, uid);
    int characNo2 = user->getCurCharacNo();
    int loginTick2 = user->GetLoginTick();
    int randSec = get_rand_int(300) + 600;
    unsigned int uid2 = user->GetUID();
    GlobalData::s_timerQueue_->InsertTimer(
        TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)1, randSec, loginTick2, uid2);
    int characNo3 = user->getCurCharacNo();
    int loginTick3 = user->GetLoginTick();
    unsigned int uid3 = user->GetUID();
    GlobalData::s_timerQueue_->InsertTimer(
        TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)0x5f, 300, loginTick3, uid3);
    int characNo4 = user->getCurCharacNo();
    int loginTick4 = user->GetLoginTick();
    unsigned int uid4 = user->GetUID();
    GlobalData::s_timerQueue_->InsertTimer(
        TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)0x60, 600, loginTick4, uid4);
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
    if (user == 0)
    {
        // b28d-lane12: P227 全表达式临时链（消 N80 具名 ctor 前求值差）。
        cMyTrace(__PRETTY_FUNCTION__, 0x138b, 5)(
            "USER DISCONNECT [%s][%d]", __PRETTY_FUNCTION__, 0x138b);
        return;
    }
    HistoryLog::LogClose((FILE*)user->m_field796f8);
    if (user->get_state() != 0)
    {
        user->prepareDisconnect();
        G_GameWorld()->EraseLoginUser(user);
        user->SetSaveRentalInfoToExchange(false);
        if (user->get_state() < 3)
        {
            user->SetSaveRentalInfoToExchange(false);
            user->LogoutCachedCharacter(0);
            user->UpdateLogout(true);
            returnUserPool(user);
            // ORIG 0x8298d30：returnUserPool 后 SendLastMsgDBQueue(user)
            GetInstanceShutdowManager()->SendLastMsgDBQueue(user);
        }
        else
        {
            CheckOutTrade(user);
            CheckOutParty(user, false);
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
    if (user == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1271, 5)(
            "USER_EXIT [%s][%d]", __PRETTY_FUNCTION__, 0x1271);
        return false;
    }
    if (user->get_state() == 0)
    {
        unsigned int mid = user->get_acc_id();
        cMyTrace tr(__PRETTY_FUNCTION__, 0x127a, 5);
        tr("USER_EXIT STATE NONE [%s][%d]mid(%d)",
           __PRETTY_FUNCTION__, 0x127a, mid);
        return false;
    }
    HistoryLog::LogClose((FILE*)user->m_field796f8);   // ORIG *(user+0x796f8)
    user->prepareDisconnect();
    G_GameWorld()->EraseLoginUser(user);
    if (user->get_state() < 3)
    {
        user->SetSaveRentalInfoToExchange(false);
        user->LogoutCachedCharacter(0);
        user->UpdateLogout(false);
    }
    else
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
        cMyTrace tr(__PRETTY_FUNCTION__, 0x12dc, 0);
        tr("ID: %s - UID: %d from Logout",
           user->getCurCharacName(), user->get_unique_id() & 0xffff);
        int err = G_EpollHandler()->ResetEpoll(user->m_network.get_socket());
        if (err != 0)
        {
            LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__, 0x12e3,
                                  "(m_id: %s) G_EpollHandler()->ResetEpoll(pUser->get_socket(%d)) %d(%s)",
                                  NumberToString(user->get_acc_id(), 0),
                                  user->m_network.get_socket(), err, strerror(err));
        }
        user->ResetCurCharac();
        user->doLinkCharacDisconnect();
        returnUserPool(user);
        user->checkLogOutCorrectly();
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
    //（crane +0x318 不置空），其余容器/池成员（hash_map、StaticPool、
    // std::map/set、Mutex）按声明逆序自动析构。
    if (m_pQuickPartySystemMgr != 0)
        delete m_pQuickPartySystemMgr;
    m_pQuickPartySystemMgr = 0;
    if (m_pQuickPartyRewardMgr != 0)
        delete m_pQuickPartyRewardMgr;
    m_pQuickPartyRewardMgr = 0;
    if (m_pAuctionAveragePriceMgr != 0)
        delete m_pAuctionAveragePriceMgr;
    m_pAuctionAveragePriceMgr = 0;
    if (m_pCraneMinigameMgr != 0)
        delete m_pCraneMinigameMgr;
    // 其余容器/池成员（hash_map、StaticPool、std::map/set、Mutex）按声明逆序自动析构。
}

// ==== Wave6 QuickParty 访问器（声明 CGameManager.h:558 区）====
int CGameManager::GetActiveQuickPartyCnt() const
{
    return static_cast<int>(m_quickPartyMap.size());
}

int CGameManager::GetOccupiedQuickPartyPoolCnt()
{
    return m_quickPartyPool.Size() - m_quickPartyPool.Remain();
}
