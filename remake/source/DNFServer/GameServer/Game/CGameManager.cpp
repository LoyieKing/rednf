#include "GameRand.h"
// df_game_r CGameManager（G2 单例对象池/房间管理器）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/CGameManager.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 依赖的池化对象（CTradeSpace/PvP_Room/WarRoom/塔/祭坛/快速队伍/蓝弹珠）与
// 管理器类权威头属后续批次：本 TU 以尺寸垫 + 桩 ctor/dtor，跨类方法以
// asm-label extern 对齐 ORIG 符号（链接桩由主 agent / 后续批次提供）。
#include <string.h>

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
#include "CSystemTime.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ---- 跨类真实类型头（替代 extern "C" asm 桥；符号与 ORIG mangled 一致）----
#include "GameWorld.h"
#include "CDataManager.h"
#include "CMissionList_Charac.h"
#include "CEventCharacterHandler.h"
#include "CDungeon.h"
#include "CDungeonClearTracer.h"
#include "CLogGameChannel.h"
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
CSchoolMgr* g_schoolMgr;          // ORIG 全局，原 GameStubs.cpp 定义迁移
CGuildAgitManager* g_guildAgitMgr;  // ORIG 全局，原 GameStubs.cpp 定义迁移
namespace GlobalData
{
extern TimerQueue* s_timerQueue_;
}


char* NumberToString(unsigned int value, int radix);
CUserGlobalInfoHandle* CUserGlobalInfoHandleInstance();
CEventCharacterHandler* CEventCharacterHandlerInstance();
CShutdowManager* GetInstanceShutdowManager();
void* G_EpollHandler();
extern CSchoolMgr* g_schoolMgr;
extern CGuildAgitManager* g_guildAgitMgr;
bool CheckDailyScheduleTime(int nScheduleTime, long lLastPlayTime, long lCurTime);

// ============================================================================
// SIG_UPDATE_LINK_CHARAC_CONNECT_STATE（CStreamGuard::GetInBuffer 模板参数，
// ORIG mangled 名 36 字符；真实 struct 属后续批次）
// ============================================================================
struct SIG_UPDATE_LINK_CHARAC_CONNECT_STATE
{
    int m_field0;
    int m_field4;
    int m_field8;
};

// ============================================================================
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义）
// ============================================================================
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

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
        cMyTrace tr("StaticPool.cpp", 0x6c, 5);
        tr("[StaticPool] %s(%d) new fail", "StaticPool.cpp", 0x6c);
    }
    for (int i = 0; i < N; ++i)
        m_Queue.push(&m_pNode[i]);
    m_nErrorCode = 0;
}

template <class T, int N>
StaticPool<T, N>::~StaticPool()
{
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
        cMyTrace tr("StaticPool.cpp", 0x89, 0);
        tr("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 3;
    }
    if (pNode->bUse != 0)
    {
        register int use = pNode->bUse;
        cMyTrace tr("StaticPool.cpp", 0x8f, 0);
        tr("[StaticPool] bUse(%d) node(%p)", use, pNode);
        m_nErrorCode = 4;
    }
    pNode->bUse = pNode->bUse + 1;
    return &pNode->data;
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
        cMyTrace tr("StaticPool.cpp", 0xc3, 0);
        tr("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 7;
    }
    if (pNode->bUse != 1)
    {
        if (pNode->bUse != 0)
        {
            register int use = pNode->bUse;
            cMyTrace tr("StaticPool.cpp", 0xca, 0);
            tr("[StaticPool] bUse(%d) node(%p)", use, pNode);
            m_nErrorCode = 8;
        }
    }
    return &pNode->data;
}

template <class T, int N>
void StaticPool<T, N>::Free(T* p)
{
    if (p == 0)
    {
        cMyTrace tr("StaticPool.cpp", 0xa0, 5);
        tr("[StaticPool] %s(%d) free null", "StaticPool.cpp", 0xa0);
    }
    CNode* pNode = (CNode*)p;
    m_Queue.push(pNode);
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        cMyTrace tr("StaticPool.cpp", 0xaa, 0);
        tr("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 5;
    }
    if (pNode->bUse != 1)
    {
        register int use = pNode->bUse;
        cMyTrace tr("StaticPool.cpp", 0xb0, 0);
        tr("[StaticPool] bUse(%d) node(%p)", use, pNode);
        m_nErrorCode = 6;
        PrintBackTrace();
    }
    pNode->bUse = pNode->bUse - 1;
}

template <class T, int N>
int StaticPool<T, N>::GetIndex(T* p)
{
    if (p < (T*)m_pNode)
        return -1;
    int index = (int)((CNode*)p - m_pNode);
    CNode* pNode = (CNode*)p;
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        cMyTrace tr("StaticPool.cpp", 0x50, 0);
        tr("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 1;
    }
    if (pNode->bUse != 1)
    {
        if (pNode->bUse != 0)
        {
            register int use = pNode->bUse;
            cMyTrace tr("StaticPool.cpp", 0x56, 0);
            tr("[StaticPool] bUse(%d) node(%p)", use, pNode);
            m_nErrorCode = 2;
        }
    }
    if (index > N - 1)
        return -1;
    return index;
}
// ============================================================================
// WongWork::CMailBox —— ORIG 布局（反汇编 0x8551d98/0x85579b2 核对）：
//   +0x00 bool        m_bInit
//   +0x04 int         m_mailLoadCount
//   +0x08 int         m_loadedLetterCount
//   +0x10 StaticPool<CMail,20> m_mailPool
//   +0x40 map<unsigned int, CMail*> m_mailMap
//   +0x58 list<Stream*> m_streamList            （GCC4.4 list=8 字节）
//   +0x60 map::const_iterator m_mailIterator    （4 字节）
//   +0x64 map<int,bool> m_letterKeepCount
//   +0x7c int          m_xxx（Init 置 0）
//   +0x80 int          m_xxx（Init 置 0）
//   +0x84 vector<unsigned int> m_xxx
//   +0x90 map<unsigned int, SIG_LETTER_INFO> m_letterInfo
// 头文件仍为 m_pad[0x100]（ABI 大小一致），此处以偏移访问实现真实语义。
// ============================================================================
typedef std::map<unsigned int, WongWork::CMailBox::CMail*> CMailBoxMailMap;
typedef std::list<Stream*> CMailBoxStreamList;
typedef std::map<int, bool> CMailBoxKeepMap;
typedef std::vector<unsigned int> CMailBoxUIntVec;
struct SIG_LETTER_INFO
{
    char m_pad[0x12c];  // 大小推断自 SetMailBoxInfo 栈帧（300 字节左右）
};
typedef std::map<unsigned int, SIG_LETTER_INFO> CMailBoxLetterMap;

WongWork::CMailBox::CMailBox()
{
    // ORIG 0x085579b2：按布局顺序构造 7 个成员（StaticPool/map/list/iter/map/vector/map）
    new (m_pad + 0x10) StaticPool<CMail, 20>();
    new (m_pad + 0x40) CMailBoxMailMap();
    new (m_pad + 0x58) CMailBoxStreamList();
    new (m_pad + 0x60) CMailBoxMailMap::const_iterator();
    new (m_pad + 0x64) CMailBoxKeepMap();
    new (m_pad + 0x84) CMailBoxUIntVec();
    new (m_pad + 0x90) CMailBoxLetterMap();
}

WongWork::CMailBox::~CMailBox()
{
    // ORIG 0x08557aac：逆序析构 7 个成员
    reinterpret_cast<CMailBoxLetterMap*>(m_pad + 0x90)->~CMailBoxLetterMap();
    reinterpret_cast<CMailBoxUIntVec*>(m_pad + 0x84)->~CMailBoxUIntVec();
    reinterpret_cast<CMailBoxKeepMap*>(m_pad + 0x64)->~CMailBoxKeepMap();
    reinterpret_cast<CMailBoxStreamList*>(m_pad + 0x58)->~CMailBoxStreamList();
    reinterpret_cast<CMailBoxMailMap*>(m_pad + 0x40)->~CMailBoxMailMap();
    reinterpret_cast<StaticPool<CMail, 20>*>(m_pad + 0x10)->~StaticPool<CMail, 20>();
}

void WongWork::CMailBox::Init()
{
    // ORIG 0x08551d98：若已初始化（m_bInit==1）则清理全部邮件/流/信标状态
    if (m_pad[0] == 1)
    {
        CMailBoxMailMap& mailMap = *reinterpret_cast<CMailBoxMailMap*>(m_pad + 0x40);
        StaticPool<CMail, 20>& pool = *reinterpret_cast<StaticPool<CMail, 20>*>(m_pad + 0x10);
        for (CMailBoxMailMap::iterator it = mailMap.begin(); it != mailMap.end(); ++it)
        {
            CMail* mail = it->second;
            if (mail != 0)
            {
                // 内联 CMail::ResetPackage（ORIG 0x085519fe）：
                // +0x08=0; +0x28=0; Inven_Item@+0x2c reset; +0x70=0; +0x74=0
                char* mb = (char*)mail;
                *(char*)(mb + 0x8) = 0;
                *(int*)(mb + 0x28) = 0;
                reinterpret_cast<Inven_Item*>(mb + 0x2c)->reset();
                *(int*)(mb + 0x70) = 0;
                *(char*)(mb + 0x74) = 0;
                pool.Free(mail);
            }
        }
        mailMap.clear();
        m_pad[0] = 0;
        *(int*)(m_pad + 0x4) = 0;
        *(int*)(m_pad + 0x8) = 0;
        CMailBoxStreamList& streamList = *reinterpret_cast<CMailBoxStreamList*>(m_pad + 0x58);
        for (CMailBoxStreamList::iterator it = streamList.begin(); it != streamList.end(); ++it)
        {
            Stream* s = *it;
            if (s != 0)
                GlobalData::s_stream_pool->Free(s);
        }
        streamList.clear();
        reinterpret_cast<CMailBoxKeepMap*>(m_pad + 0x64)->clear();
        *(int*)(m_pad + 0x7c) = 0;
        *(int*)(m_pad + 0x80) = 0;
        reinterpret_cast<CMailBoxUIntVec*>(m_pad + 0x84)->clear();
        reinterpret_cast<CMailBoxLetterMap*>(m_pad + 0x90)->clear();
    }
}

void WongWork::CMailBox::SetLastLoadLetterIdx(unsigned int) {}
void WongWork::CMailBox::SetLoadState(bool, long) {}
void WongWork::CMailBox::SetLoadedLetterCount(int) {}
void WongWork::CMailBox::SetNotLoadedMailCount(int) {}
void WongWork::CMailBox::SetPackageLoadLack(const unsigned int*, int, std::set<unsigned int>&) {}
int WongWork::CMailBox::getMailLoadCount() { return 0; }
WongWork::CMailBox::CMail* WongWork::CMailBox::getNextMail()
{
    // ORIG 0x0855236a：迭代器 @0x60 指向 m_mailMap（+0x40）；未到 end 则返回
    // second 并前移，否则返回 0。
    CMailBoxMailMap& mailMap = *reinterpret_cast<CMailBoxMailMap*>(m_pad + 0x40);
    CMailBoxMailMap::const_iterator& it =
        *reinterpret_cast<CMailBoxMailMap::const_iterator*>(m_pad + 0x60);
    if (it == mailMap.end())
        return 0;
    CMail* mail = it->second;
    ++it;
    return mail;
}
void WongWork::CMailBox::incMailLoadCount() {}
void WongWork::CMailBox::reset() {}
void WongWork::CMailBox::setMailIterator()
{
    // ORIG 0x0855232e：迭代器 @0x60 = m_mailMap.begin()
    CMailBoxMailMap& mailMap = *reinterpret_cast<CMailBoxMailMap*>(m_pad + 0x40);
    *reinterpret_cast<CMailBoxMailMap::const_iterator*>(m_pad + 0x60) = mailMap.begin();
}
// void WongWork::CDungeonClear::clear() 已由 CDungeonClear.cpp 提供（合并移除）
advancealtar::StageControl::StageControl() { memset(m_pad, 0, sizeof(m_pad)); }
advancealtar::StageControl::~StageControl() {}

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
template class StaticPool<_IO_FILE, 50>;
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
template class DynamicPool<WongWork::IPG::SIPGData>;

// ============================================================================
// CGameManager 实现
// ============================================================================
CGameManager* G_CGameManager()
{
    return GlobalInstance<CGameManager>::inst_ptr();
}

bool CGameManager::init()
{
    void* pMgr = operator new(0x6c);
    try
    {
        new (pMgr) CraneMinigameManager();
    }
    catch (...)
    {
        operator delete(pMgr);
        throw;
    }
    m_pCraneMinigameMgr = (CraneMinigameManager*)pMgr;
    return GetCraneMinigameManager()->init() == 1;
}

short CGameManager::getNextUID()
{
    m_uid = (short)(m_uid + 1);
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
            const char* mapId = NumberToString(user->get_acc_id(), 1);
            const char* inId = NumberToString(accId, 0);
            cMyTrace tr("CUser* CGameManager::GetUserByAccId(memberIdentificationNumber_t)", 0x9ab, 5);
            tr("[%s][%d][IN m_id: %s][Map m_id: %s]",
               "CUser* CGameManager::GetUserByAccId(memberIdentificationNumber_t)", 0x9ab,
               inId, mapId);
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
    std::pair<const unsigned int, CUser*>& p) const
{
    return p.second != 0 &&
           (unsigned short)p.second->get_unique_id() == (short)m_uniqueId;
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
    short slot = (short)(slotId >> 0x10);
    short id = (short)slotId;
    Guard<Mutex> guard(&m_mutex);
    CUser* user = m_userPool.Get((int)slot);
    if (user == 0)
    {
        cMyTrace tr("CUser* CGameManager::getUser(int, int)", 0x95c, 5);
        tr(" No way! getUser , slot_id(%d) is abnormal!", (int)slot);
        user = 0;
    }
    else
    {
        short incre = user->GetIncreID();
        if (incre == 0)
        {
            user = 0;
        }
        else if (incre != id && incre != 0 && id != 0)
        {
            user = 0;
        }
    }
    return user;
}

void CGameManager::insertUserByAccID(CUser* user)
{
    unsigned int accId = user->get_acc_id();
    m_userByAccId[accId] = user;
    unsigned int accId2 = user->get_acc_id();
    if (accId2 == 0)
    {
        const char* name = user->get_acc_name();
        cMyTrace tr("void CGameManager::insertUserByAccID(CUser*)", 0x91d, 5);
        tr("[QQID_ERROR]CGameManager::insertUserByAccID  qq_id:%s", name);
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
    Guard<Mutex> guard(&m_mutex);
    CUser* user = m_userPool.Acquire();
    if (user == 0)
    {
        cMyTrace tr("CUser* CGameManager::createUser()", 0x8e3, 5);
        tr("createUser Failed! need to restart!");
        user = 0;
    }
    else
    {
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
            cMyTrace tr("CUser* CGameManager::createUser()", 0x8f6, 0);
            tr("STATIC MEMORY_POOL createUser error(%d), user(%x), last user m_acc_id(%s), idx(%d), new user idx(%d)",
               err, user, lastId, uid, newUid);
        }
        short nxt = getNextUID();
        user->SetIncreID(nxt);
        unsigned short uniqueId =
            CUserGlobalInfoHandleInstance()->get_uniqueid();
        user->set_unique_id(uniqueId);
    }
    return user;
}
void CGameManager::check_user_var(CUser* user)
{
    bool b = false;
    if (user->get_charac_guildkey() != 0)
    {
        if (user->get_guild_exp() != 0)
            b = true;
    }
    if (b)
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x92c,
                              "[CHECK_USER_VAR] RESET GUILD EXP ERROR!");
    }
    if (user->get_charac_level_before_dungeon() != 0)
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x92f,
                              "[CHECK_USER_VAR] RESET CHARACTER LEVEL BEFORE DUNGEON ERROR!");
    }
    if (user->CheckInParty())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x932,
                              "[CHECK_USER_VAR] RESET PARTY ERROR!");
    }
    if (user->CheckInTrade())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x935,
                              "[CHECK_USER_VAR] RESET TRADE INDEX ERROR!");
    }
    if (user->CheckInPvp())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x938,
                              "[CHECK_USER_VAR] RESET PVP INDEX ERROR!");
    }
    if (user->CheckInWarRoom())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x93b,
                              "[CHECK_USER_VAR] RESET WARROOM INDEX ERROR!");
    }
    if (user->getPlayingDungeonIndex() != -1)
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x93e,
                              "[CHECK_USER_VAR] RESET DUNGEON ERROR!");
    }
    if (user->IsProgLogout())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x941,
                              "[CHECK_USER_VAR] RESET PROG LOGOUT ERROR!");
    }
    if (user->m_characterView.isSaveCharacView())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x944,
                              "[CHECK_USER_VAR] RESET CHARACTER VIEW ERROR!");
    }
    if (user->checkInBlueMarble())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x948,
                              "[CHECK_USER_VAR] RESET BLUEMARBLE INDEX ERROR!");
    }
}

void CGameManager::returnUserPool(CUser* user)
{
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
            cMyTrace tr("void CGameManager::returnUserPool(CUser*)", 0xa0e, 0);
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
        LogManager::logFormat(1, "App.cpp", "CParty* CGameManager::GetParty()", 0xa5f,
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
    if (party == 0)
        return;
    int key = party->GetPartyIndex();
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.find(key);
    if (it != m_partyMap.end())
    {
        int eraseKey = party->GetPartyIndex();
        m_partyMap.erase(eraseKey);
        m_partyPool.Free(party);
    }
}

void CGameManager::CheckOutParty(CUser* user, bool flag)
{
    if (user->CheckInParty() == 1)
    {
        CParty* party = GetParty(user->GetPartyIndex());
        if (party != 0)
        {
            bool bVar1 = false;
            void* dungeon = *(void**)((char*)party + 0xcac);
            if (dungeon != 0)
            {
                bVar1 = *(char*)((char*)dungeon + 0x89f) > 0;
                if (((CDungeon*)dungeon)->isTowerOfDespairDungeon())
                    bVar1 = true;
            }
            bool bVar2 = false;
            if (dungeon != 0)
            {
                char c = ((CDungeon*)dungeon)->get_dimension_possible();
                if (c < 1 && *(char*)((char*)dungeon + 0x89c) == 0 &&
                    !((CDungeon*)dungeon)->isTournamentDungeon())
                    bVar2 = false;
                else
                    bVar2 = true;
                if (bVar2)
                    bVar1 = true;
            }
            ((CDungeonClearTracer*)((char*)party + 0xc7c))->Trace("void CGameManager::CheckOutParty(CUser*, bool)");
            if (*(int*)((char*)party + 0xcd8) != 1 && party->get_state() == 2)
            {
                bVar2 = false;
                if (dungeon != 0 && *(char*)((char*)dungeon + 0x85c) != 0)
                    bVar2 = true;
                bool bVar3 = false;
                if (dungeon != 0 && *(char*)((char*)dungeon + 0x87a) != 0)
                    bVar3 = true;
                if (!bVar1 && bVar2 && bVar3)
                    user->giveup_panalty();
            }
            party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
        }
    }
}

unsigned int CGameManager::CheckOutQuickParty(CParty* party, bool flag)
{
    unsigned int ret = 0;
    if (party != 0)
    {
        ret = (unsigned int)G_CGameManager()->GetQuickPartySystemManager()->cancel_quick_party(party, flag, 0);
        ret = ret ^ 1;
    }
    return ret;
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
    if (user->CheckInTrade() == 1)
    {
        CTradeSpace* tradeSpace = m_tradeSpacePool.Get(user->GetTradeSpace());
        tradeSpace->cancel_trade_by_dis(user);
        PutTradeSpace(tradeSpace);
    }
}

// ============================================================================
// PvP
// ============================================================================
PvP_Room* CGameManager::GetPvp()
{
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
        {
            CSwitchLog log("PvP_Room* CGameManager::GetPvp()", 0xaa9, 0, 0);
            log("pvp@log room(%d) new alloc", roomIdx);
        }
        m_pvpRoomMap[room->get_index()] = room;
        return room;
    }
    else
    {
        roomIdx = room->get_index();
        {
            CSwitchLog log("PvP_Room* CGameManager::GetPvp()", 0xab2, 0, 0);
            log("pvp@log room(%d) already alloc", roomIdx);
        }
        return 0;
    }
}

PvP_Room* CGameManager::GetPvp(int index, CUser* user, int param)
{
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
        {
            CSwitchLog log("PvP_Room* CGameManager::GetPvp(int, CUser*, int)", 0xacc, 0, 0);
            log("pvp@log %s,%s, room(%d), %d", accName, name, roomIdx, param);
        }
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
    if (room != 0)
    {
        int roomIndex = room->get_index();
        {
            CSwitchLog log("void CGameManager::PutPvp(PvP_Room*)", 0xae4, 0, 0);
            log("pvp@log room(%d)", roomIndex);
        }
        int key = room->get_index();
        __gnu_cxx::hash_map<int, PvP_Room*,
            __gnu_cxx::hash<int>, std::equal_to<int>,
            std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(key);
        if (it != m_pvpRoomMap.end())
        {
            int eraseKey = room->get_index();
            m_pvpRoomMap.erase(eraseKey);
            m_pvpRoomPool.Free(room);
        }
    }
}

void CGameManager::CheckOutPvp(CUser* user, bool flag)
{
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
                CSwitchLog log("void CGameManager::CheckOutPvp(CUser*, bool)", 0x11a1, 0, 0);
                log("pvp@log %s,%s, room(%d),%d,%d",
                    accName, characName, roomIdx, battleMode, outFlagInt);
            }
            if (ret < 0)
            {
                LogManager::logFormat(1, "App.cpp", "void CGameManager::CheckOutPvp(CUser*, bool)", 0x11bd,
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
    for (;;)
    {
        if (it == m_pvpRoomMap.end())
            return 0;
        PvP_Room* room = it->second;
        if (room != 0 && room->get_pvp_battle_mode() != 6)
        {
            bool ok = true;
            if ((quick && room->IsExistPassword() == 1) ||
                room->IsQuickJoinablePVPRoom(user) == 0)
                ok = false;
            if (ok)
                return room;
        }
        ++it;
    }
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
                              "QuickParty::CQuickParty* CGameManager::GetQuickParty()", 0xb64,
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
    if (qp == 0)
        return;
    int key = qp->get_quick_party_index();
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(key);
    if (it != m_quickPartyMap.end())
    {
        int eraseKey = qp->get_quick_party_index();
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
    if (room == 0)
        return;
    int key = room->GetIndex();
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(key);
    if (it != m_warRoomMap.end())
    {
        int eraseKey = room->GetIndex();
        m_warRoomMap.erase(eraseKey);
        m_warRoomPool.Free(room);
    }
}

void CGameManager::CheckOutWarRoom(CUser* user)
{
    if (user->CheckInWarRoom() == 1)
    {
        WarRoom* room = GetWarRoom(user->GetWarRoomIndex());
        if (room != 0)
        {
            int ret = room->WalkOutUser(user);
            if (ret == 4)
            {
                LogManager::logFormat(1, "App.cpp", "void CGameManager::CheckOutWarRoom(CUser*)", 0x117b,
                                      "CGameManager::CheckOutWarRoom Can't find user charno(%d)",
                                      user->get_charac_no(-1));
            }
            room->CheckState();
        }
    }
}

WarRoom* CGameManager::FindJoinableWarRoom(CUser* user)
{
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.begin();
    WarRoom* room;
    for (;;)
    {
        if (it == m_warRoomMap.end())
            return 0;
        room = it->second;
        if (room != 0 && room->IsJoinable(user) == 0)
            break;
        it++;
    }
    return room;
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
    if (tower == 0)
        return;
    int key = tower->getIdx();
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(key);
    if (it != m_deathTowerMap.end())
    {
        int eraseKey = tower->getIdx();
        m_deathTowerMap.erase(eraseKey);
        m_deathTowerPool.Free(tower);
    }
}

void CGameManager::checkOutDeathTower(CUser* user)
{
    if (user->checkInDeathTower() == 1)
    {
        WongWork::CDeathTower* tower = getDeathTower(user->getDeathTowerIndex());
        if (tower != 0 && tower->onLeaveUser(user) != 1)
        {
            LogManager::logFormat(1, "App.cpp", "void CGameManager::checkOutDeathTower(CUser*)", 0x123a,
                                  "DeathTower checkOutDeathTower ID[%d] ACCID[%s]",
                                  tower->getIdx(),
                                  NumberToString(user->get_acc_id(), 0));
        }
    }
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
    if (tower == 0)
        return;
    int key = tower->getIdx();
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(key);
    if (it != m_bossTowerMap.end())
    {
        int eraseKey = tower->getIdx();
        m_bossTowerMap.erase(eraseKey);
        m_bossTowerPool.Free(tower);
    }
}

void CGameManager::checkOutBossTower(CUser* user)
{
    if (user->checkInBossTower() == 1)
    {
        WongWork::CBossTower* tower = getBossTower(user->getBossTowerIndex());
        if (tower != 0)
        {
            typedef char (*LeaveFn)(void*, void*);
            LeaveFn leave = *(LeaveFn*)(*(void**)tower + 0x14);
            if (leave(tower, user) != 1)
            {
                LogManager::logFormat(1, "App.cpp", "void CGameManager::checkOutBossTower(CUser*)", 0x11d9,
                                      "BossTower checkOutBossTower ID[%d] ACCID[%s]",
                                      tower->getIdx(),
                                      NumberToString(user->get_acc_id(), 0));
            }
        }
    }
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
    if (control == 0)
        return;
    int key = control->getIndex();
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(key);
    if (it != m_stageMap.end())
    {
        int eraseKey = control->getIndex();
        m_stageMap.erase(eraseKey);
        m_stagePool.Free(control);
    }
}

void CGameManager::checkOutAdvanceAltar(CUser* user)
{
    if (user != 0 && user->checkInAdvanceAltar() == 1)
    {
        advancealtar::StageControl* control =
            getAdvanceAltar(user->getAdvanceAltarIndex());
        if (control != 0 && control->leaveUser() != 1)
        {
            LogManager::logFormat(1, "App.cpp",
                                  "void CGameManager::checkOutAdvanceAltar(CUser*)", 0x11ef,
                                  "invalid user : advance altar logout ID[%d] ACCID[%s]",
                                  control->getIndex(),
                                  NumberToString(user->get_acc_id(), 0));
        }
    }
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
        if (control->onTimerStageTick() == 1)
        {
            it++;
        }
        else
        {
            __gnu_cxx::hash_map<int, advancealtar::StageControl*,
                __gnu_cxx::hash<int>, std::equal_to<int>,
                std::allocator<advancealtar::StageControl*> >::iterator eraseIt = it;
            it++;
            m_stageMap.erase(eraseIt);
            m_stagePool.Free(control);
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
    if (marble == 0)
        return;
    int key = marble->getMemoryPoolIndex();
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(key);
    if (it != m_blueMarbleMap.end())
    {
        int eraseKey = marble->getMemoryPoolIndex();
        m_blueMarbleMap.erase(eraseKey);
        m_blueMarblePool.Free(marble);
    }
}

void CGameManager::checkOutBlueMarble(CUser* user)
{
    if (user->checkInBlueMarble() == 1)
    {
        BlueMarble* marble = getBlueMarble(user->getBlueMarbleIndex());
        if (marble != 0)
        {
            marble->leaveUser(user);
        }
    }
}

void CGameManager::allocBlueMarble()
{
    void* dm = G_CDataManager();
    void* script = ((CDataManager*)dm)->getBlueMarbleScript();
    for (int i = 0; i < 300; ++i)
    {
        BlueMarble* marble = getBlueMarble();
        marble->getMemoryPoolIndex();
        for (int j = 0; j < *(int*)((char*)script + 0x30); ++j)
        {
            marble->setUserInfo((BlueMarbleScriptManager*)script);
        }
        marble->setScript((BlueMarbleScriptManager*)script);
    }
}

BlueMarble* CGameManager::findJoinableBlueMarble()
{
    void* dm = G_CDataManager();
    void* script = ((CDataManager*)dm)->getBlueMarbleScript();
    if (script == 0)
        return 0;
    int count = *(int*)((char*)script + 0x30);
    BlueMarble* result = 0;
    for (int n = 0; n < count; ++n)
    {
        __gnu_cxx::hash_map<int, BlueMarble*,
            __gnu_cxx::hash<int>, std::equal_to<int>,
            std::allocator<BlueMarble*> >::iterator it =
                m_blueMarbleMap.begin();
        for (; it != m_blueMarbleMap.end(); ++it)
        {
            BlueMarble* marble = it->second;
            if (marble != 0)
            {
                if (marble->checkJoin() &&
                    marble->getWaitUserCount() == count - 1)
                    return marble;
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
                                const void* characR = member->getCurCharacR();
                                guard.put_byte(*(const char*)((const char*)characR + 0xeb9 + j));
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
    while (true)
    {
        if (it == m_pvpRoomMap.end())
            break;
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
    while (true)
    {
        if (it == m_warRoomMap.end())
            break;
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
}

// ============================================================================
// WarRoom 分配 / 计数管理
// ============================================================================
void CGameManager::WarRoomAlloc()
{
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
            *(char*)room = (char)area;
            *((char*)room + 1) = (char)count;
            room->Prepare();
            void* dm3 = G_CDataManager();
            if (count < *(int*)((char*)dm3 + 8 + (hourTable * 10 + area + 0x223c) * 4))
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
    void* dm = G_CDataManager();
    void* script = ((CDataManager*)dm)->GetChannelScript();
    void* env = G_CEnvironment();
    unsigned int channelNo = ((CEnvironment*)env)->get_channel_no();
    channel_info_t* info = ((channel_script_t*)script)->getChannelInfo(*(unsigned char*)((char*)env + 0x378), channelNo);
    for (int a = 0; a < 10; ++a)
    {
        for (int b = 0; b < 0x18; ++b)
        {
            void* dm2 = G_CDataManager();
            float f = *(float*)((char*)info + 8 + (a + 4) * 4);
            void* dm3 = G_CDataManager();
            *(int*)((char*)dm2 + 8 + (b * 10 + a + 0x223c) * 4) =
                (int)(*(float*)((char*)dm3 + 4 + (b + 0x21ec) * 4) * f);
        }
    }
}

void CGameManager::WarRoomCountManage(int hourIndex)
{
    cMyTrace tr0("void CGameManager::WarRoomCountManage(int)", 0x306f, 0);
    tr0("WarRoomCountManage Start");
    int current[10];
    for (int i = 0; i < 10; ++i)
        current[i] = 0;
    int total = 0;
    for (int area = 0; area < 10; ++area)
    {
        int n = 0;
        for (;;)
        {
            void* dm = G_CDataManager();
            int need = ((CDataManager*)dm)->m_warAreaCounter.GetWarRoomCountAtPeekTime(area);
            if (need <= n)
                break;
            total = total + 1;
            WarRoom* room = GetWarRoom();
            if (room == 0)
            {
                LogManager::logFormat(1, "App.cpp",
                                      "void CGameManager::WarRoomCountManage(int)", 0x307d,
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
    cMyTrace tr1("void CGameManager::WarRoomCountManage(int)", 0x3088, 0);
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
                cMyTrace tr3("void CGameManager::WarRoomCountManage(int)", 0x30cf, 0);
                tr3("WarRoomCountManage end");
            }
            return;
        }
        void* dm = G_CDataManager();
        int diff = *(int*)((char*)dm + 8 + (hourTable * 10 + area + 0x223c) * 4) -
                   current[area];
        cMyTrace tr2("void CGameManager::WarRoomCountManage(int)", 0x3097, 0);
        tr2("WarRoomCountManage diff(%d) currentWarRoomCount(lev%d)", diff, current[area]);
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            void* dm2 = G_CDataManager();
            int lastIdx = ((CDataManager*)dm2)->m_warAreaCounter.GetWarRoomCountLastIndex(area);
            while (need != 0)
            {
                lastIdx = lastIdx - 1;
                WarRoom* room = G_CGameManager()->GetWarRoom();
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
                firstIdx = firstIdx + 1;
                WarRoom* room = G_CGameManager()->GetWarRoom();
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
    int current[10];
    for (int i = 0; i < 10; ++i)
        current[i] = 0;
    int total = 0;
    for (int area = 0; area < 10; ++area)
    {
        int n = 0;
        for (;;)
        {
            void* dm = G_CDataManager();
            int need = ((CDataManager*)dm)->m_warAreaCounter.GetWarRoomCountAtPeekTime(area);
            if (need <= n)
                break;
            total = total + 1;
            WarRoom* room = GetWarRoom();
            if (room == 0)
            {
                LogManager::logFormat(1, "App.cpp",
                                      "void CGameManager::WarRoomCountManageTest(int)", 0x30e2,
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
        void* dm = G_CDataManager();
        int diff = *(int*)((char*)dm + 8 + (hourTable * 10 + area + 0x223c) * 4) -
                   current[area];
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            void* dm2 = G_CDataManager();
            int lastIdx = ((CDataManager*)dm2)->m_warAreaCounter.GetWarRoomCountLastIndex(area);
            while (need != 0)
            {
                lastIdx = lastIdx - 1;
                WarRoom* room = G_CGameManager()->GetWarRoom();
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
                firstIdx = firstIdx + 1;
                WarRoom* room = G_CGameManager()->GetWarRoom();
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
    Stream* stream = GlobalData::s_stream_pool->Acquire("App.cpp", 0x125b);
    CStreamGuard guard(stream, true);
    (**guard) << 0x1e5;
    (**guard) << -1;
    SIG_UPDATE_LINK_CHARAC_CONNECT_STATE* info =
        guard->GetInBuffer<SIG_UPDATE_LINK_CHARAC_CONNECT_STATE>();
    memset(info, 0, 0xc);
    *(unsigned int*)info = accId;
    *(unsigned int*)((char*)info + 4) = characNo;
    *(unsigned int*)((char*)info + 8) = (unsigned int)action;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

bool CGameManager::IsNoGameGuardApplingUser(std::string& name, char* addr)
{
    std::set<std::string>::iterator it = m_noGameGuard.find(name);
    return it != m_noGameGuard.end();
}

// ============================================================================
// 管理器
// ============================================================================
QuickParty::CQuickPartySystemManager* CGameManager::GetQuickPartySystemManager()
{
    if (m_pQuickPartySystemMgr == 0)
    {
        void* pMgr = operator new(0x18);
        try
        {
            new (pMgr) QuickParty::CQuickPartySystemManager();
        }
        catch (...)
        {
            operator delete(pMgr);
            throw;
        }
        m_pQuickPartySystemMgr = (QuickParty::CQuickPartySystemManager*)pMgr;
    }
    return m_pQuickPartySystemMgr;
}

QuickParty::CQuickPartyRewardManager* CGameManager::GetQuickPartyRewardManager()
{
    if (m_pQuickPartyRewardMgr == 0)
    {
        void* pMgr = operator new(0x228);
        new (pMgr) QuickParty::CQuickPartyRewardManager();
        m_pQuickPartyRewardMgr = (QuickParty::CQuickPartyRewardManager*)pMgr;
    }
    return m_pQuickPartyRewardMgr;
}

CPremiumLetheManager* CGameManager::GetPremiumLetheManager()
{
    if (m_pPremiumLetheMgr == 0)
    {
        void* pMgr = operator new(1);
        new (pMgr) CPremiumLetheManager();
        m_pPremiumLetheMgr = (CPremiumLetheManager*)pMgr;
    }
    return m_pPremiumLetheMgr;
}

CSharedServerMessageManager* CGameManager::GetSharedServerMessageManager()
{
    if (m_pSharedServerMessageMgr == 0)
    {
        void* dm = G_CDataManager();
        void* pMgr = operator new(0x18);
        new (pMgr) CSharedServerMessageManager((stSharedServerMessageInfo*)((char*)dm + 0x7d8));
        m_pSharedServerMessageMgr = (CSharedServerMessageManager*)pMgr;
    }
    return m_pSharedServerMessageMgr;
}

CSpecialItemRoutingManager* CGameManager::GetSpecialItemRoutingManager()
{
    if (m_pSpecialItemRoutingMgr == 0)
    {
        void* pMgr = operator new(1);
        new (pMgr) CSpecialItemRoutingManager();
        m_pSpecialItemRoutingMgr = (CSpecialItemRoutingManager*)pMgr;
    }
    return m_pSpecialItemRoutingMgr;
}

CConditionEventManager* CGameManager::GetConditionEventManager()
{
    if (m_pConditionEventMgr == 0)
    {
        void* pMgr = operator new(1);
        new (pMgr) CConditionEventManager();
        m_pConditionEventMgr = (CConditionEventManager*)pMgr;
    }
    return m_pConditionEventMgr;
}

CAuctionAveragePrice* CGameManager::GetAuctionAveragePriceManager()
{
    if (m_pAuctionAveragePriceMgr == 0)
    {
        void* pMgr = operator new(0x18, std::nothrow);
        if (pMgr != 0)
        {
            new (pMgr) CAuctionAveragePrice();
        }
        m_pAuctionAveragePriceMgr = (CAuctionAveragePrice*)pMgr;
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
        unsigned short len = *(unsigned short*)((char*)&pkt + 2);
        CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)(int)group);
        proxy->SendTcpPacket((char*)&pkt, len);
    }
    else if (type == 0xc9)
    {
        unsigned short len = *(unsigned short*)((char*)&pkt + 2);
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)(int)group);
        proxy->SendTcpPacket((char*)&pkt, len);
    }
    memset((char*)&pkt + 0xc, 0, 0x16da);
}



void CGameManager::Send_userinfos_to_upper_server(unsigned char channelType)
{
    int userCount = 0;
    Packet_Monitor_UDP_Reply_UserInfo pkt;
    char* p = (char*)&pkt;
    p[11] = (char)((CEnvironment*)G_CEnvironment())->get_channel_no();
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() && channelType == 0xc9)
    {
        int idx = 0;
        int group = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
        while (idx < GlobalData::s_monitor_proxy_mgr->GetEndIndex())
        {
            Packet_ChannelType ctype;
            *(unsigned int*)((char*)&ctype + 0xc) =
                (unsigned int)((CEnvironment*)G_CEnvironment())->get_channel_no();
            *(unsigned int*)((char*)&ctype + 0x10) =
                (unsigned int)G_GameWorld()->GetChannelType();
            CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)group);
            proxy->SendTcpPacket((char*)&ctype, *(unsigned short*)((char*)&ctype + 8));
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
                    *(unsigned int*)(p + 12 + n * 0x4e) = user->get_acc_id();
                    *(unsigned int*)(p + 16 + n * 0x4e) = user->get_charac_no(-1);
                    *(unsigned int*)(p + 20 + n * 0x4e) = user->GetUID();
                    *(unsigned int*)(p + 24 + n * 0x4e) = user->get_charac_guildkey();
                    p[28 + n * 0x4e] = user->get_charac_job();
                    p[29 + n * 0x4e] = user->getCurCharacGrowType();
                    *(unsigned short*)(p + 30 + n * 0x4e) = user->get_charac_level();
                    *(unsigned int*)(p + 62 + n * 0x4e) = user->get_charac_memberkey();
                    strcpy(p + 32 + n * 0x4e, user->getCurCharacName());
                    p[82 + n * 0x4e] = user->getSex();
                    memcpy(p + 83 + n * 0x4e, user->GetSsnString(), 6);
                    char ipBuf[16];
                    ((CNetwork<4096, 450000>*)((char*)user + 0xe0))
                        ->GetPeerIP2(ipBuf, 0x10);
                    strcpy(p + 66 + n * 0x4e, ipBuf);
                    lastUser = user;
                    userCount = userCount + 1;
                    if (userCount > 0x4a)
                    {
                        *(unsigned short*)(p + 2) = 0x16e6;
                        p[10] = (char)userCount;
                        SendUserInfoToUpperServer(pkt, channelType, user, (ENUM_SERVER_GROUP)group);
                        userCount = 0;
                    }
                }
            }
            if (userCount != 0)
            {
                *(unsigned short*)(p + 2) = (short)(userCount * 0x4e + 0xc);
                p[10] = (char)userCount;
                SendUserInfoToUpperServer(pkt, channelType, lastUser, (ENUM_SERVER_GROUP)group);
                userCount = 0;
            }
            group = GlobalData::s_monitor_proxy_mgr->GetNextIndex(groupIdx);
        }
    }
}

void CGameManager::send_userinfos_to_cutoff_server()
{
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
    ((char*)&pkt)[12] = (char)((CEnvironment*)G_CEnvironment())->get_channel_no();
    ((char*)&pkt)[13] = *(char*)((char*)G_CEnvironment() + 0x378);
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
                ((int*)buckets)[bucketIdx * 0x259 + n] = (int)accId;
                bucket->m_count = bucket->m_count + 1;
            }
        }
        for (int i = 0; i < 10; ++i)
        {
            if (*(unsigned short*)((char*)buckets + i * 0x964) != 0)
            {
                *(unsigned short*)((char*)&pkt + 2) =
                    *(unsigned short*)((char*)buckets + i * 0x964) * 4 + 0xe;
                *(unsigned int*)((char*)&pkt + 6) =
                    *(unsigned int*)((char*)buckets + 4 + i * 0x964);
                *(unsigned short*)((char*)&pkt + 10) =
                    *(unsigned short*)((char*)buckets + i * 0x964);
                memcpy((char*)&pkt + 16,
                       (char*)buckets + 4 + i * 0x964,
                       (unsigned int)*(unsigned short*)((char*)buckets + i * 0x964) * 4);
                GlobalData::s_double_check_proxy->SendPacket((char*)&pkt, (unsigned int)*(unsigned short*)((char*)buckets + i * 0x964) * 4 + 0xe);
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
    if (user == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf20,
                              "insert_waiting_room pUser parameter is null.");
        return 0;
    }
    const void* characR = user->getCurCharacR();
    if (characR == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf27,
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
    const void* characR2 = user->getCurCharacR();
    long lastPlay = *(long*)((char*)characR2 + 0x7b);
    void* env = G_CEnvironment();
    if (CheckDailyScheduleTime(
            *(int*)((char*)env + 0x37c), lastPlay, curSec2))
    {
        std::vector<std::pair<int, int> > vecC;
        user->deleteDailyStackableItem(vecC, 3, 8);
        user->deleteDailyStackableItem(vecC, 0x39, 0x68);
    }
    user->deleteSpecificItem(vecA, vecB);
    if (user->send_itemspace(0xc) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf58,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_ACCOUNT_CARGO ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (user->send_itemspace(0) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf5f,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (user->send_itemspace(1) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf64,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_AVATAR ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (user->send_itemspace(2) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf69,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CARGO ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (user->send_itemspace(7) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf7a,
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
        cMyTrace tr("bool CGameManager::insert_game_world(CUser*)", 0xfa2, 0);
        tr("CGameManager::insert_game_world , !G_GameWorld()->reach_game_world( pUser ) , USER : %d(%s)",
           user->get_unique_id() & 0xffff, NumberToString(user->get_acc_id(), 0));
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 4);
        cMyTrace tr2("bool CGameManager::insert_game_world(CUser*)", 0xfab, 0);
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
    *(unsigned int*)((char*)&monitor + 0xa) = user->get_acc_id();
    ((char*)&monitor)[0xe] = (char)((CEnvironment*)G_CEnvironment())->get_channel_no();
    ((char*)&monitor)[0x3d] = (char)G_GameWorld()->GetChannelType();
    *(unsigned int*)((char*)&monitor + 0xf) = (unsigned int)user->get_charac_no(-1);
    *(unsigned int*)((char*)&monitor + 0x13) = user->get_charac_guildkey();
    ((char*)&monitor)[0x17] = user->get_charac_job();
    ((char*)&monitor)[0x18] = user->getCurCharacGrowType();
    *(unsigned short*)((char*)&monitor + 0x19) = user->get_charac_level();
    *(unsigned int*)((char*)&monitor + 0x1b) = user->get_charac_memberkey();
    const char* name = user->getCurCharacName();
    int nameLen = (int)strlen(name);
    memcpy((char*)&monitor + 0x1f, user->getCurCharacName(), nameLen);
    ((char*)&monitor)[0x3e] = user->IsReturnUserFirstLogin();
    int serverGroup = (int)(size_t)user->GetServerGroup();
    CMonitorServerProxy* monitorProxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)serverGroup);
    monitorProxy->SendTcpPacket((char*)&monitor, 0x3f);
    int guildGroup = (int)(size_t)user->GetServerGroup();
    CGuildServerProxy* guildProxy = GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)guildGroup);
    guildProxy->SendTcpPacket((char*)&monitor, 0x3f);
    g_schoolMgr->AddUser(user);
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
    if (user == 0)
    {
        cMyTrace tr("void CGameManager::user_disconnect(CUser*)", 0x138b, 5);
        tr("USER DISCONNECT [%s][%d]",
           "void CGameManager::user_disconnect(CUser*)", 0x138b);
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
CGameManager::CGameManager()
    : m_mutex(), m_uid(0), m_pad1a(), m_noGameGuard(), m_gameGuard2(),
      m_userPool(), m_tradeSpacePool(), m_partyPool(), m_pvpRoomPool(),
      m_warRoomPool(), m_deathTowerPool(), m_bossTowerPool(), m_stagePool(),
      m_quickPartyPool(), m_blueMarblePool(), m_userByAccId(), m_userByAccId2(),
      m_pvpRoomMap(), m_partyMap(), m_warRoomMap(), m_deathTowerMap(), m_bossTowerMap(),
      m_stageMap(), m_quickPartyMap(), m_blueMarbleMap(), m_pQuickPartySystemMgr(0),
      m_pQuickPartyRewardMgr(0), m_pPremiumLetheMgr(0), m_pSharedServerMessageMgr(0),
      m_pConditionEventMgr(0), m_pSpecialItemRoutingMgr(0), m_pAuctionAveragePriceMgr(0),
      m_pCraneMinigameMgr(0)
{
}

