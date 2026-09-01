// OnlinePreliminaryTeam.cpp —— online_preliminary 数据域方法实现。
// 权威头：OnlinePreliminary.h（勿在此 include CUserCharacInfo.h 旧类块）。
// 报告目录：docs/class_func_reports/online_preliminary__*/；无报告小函数
// 按 objdump 反汇编逐条转写并标注【推断：objdump】。
//
// 布局注记：ORIG 中 COnlinePreliminaryTeam::GetTeamId() const @0x8697892 读
// *(int*)(this+0x4)，而 GetCurrentUserCount 族操作 *(int*)(this+0x0)
// （0x8588d7e..0x8588dbd）；loadDataPreliminaryTeam/saveData 亦以 +0x4 为
// teamId 序列化键。故 GetTeamId 按 ORIG 修正为返回 m_field04
// 【推断修正：objdump 0x8697892】，GetTeamName(+0x14) 与原定义一致保持。

#include "OnlinePreliminary.h"

#include <cstring>

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "Stream.h"
#include "SigTypes.h"
#include "CUser.h"
#include "GameTypes.h"      // cMyTrace

namespace online_preliminary
{

// ============================================================================
// CharacterInServer
// ============================================================================

// ORIG W 0x08588d7e? no——0x085868d4 _ZNK18online_preliminary17CharacterInServerltERKS0_
// 报告 docs/class_func_reports/online_preliminary__COnlinePreliminaryMember/
//（operator< 为内联辅助，无独立报告）
// 【推断：objdump 0x85868d4】先按 serverId 低 16 位（movzwl）比较；
// 相等时再比较 uniqueId（int@+0x04）。
bool CharacterInServer::operator<(const CharacterInServer& rhs) const
{
    unsigned short s1 = static_cast<unsigned short>(m_serverId);
    unsigned short s2 = static_cast<unsigned short>(rhs.m_serverId);
    if (s1 < s2)
        return true;
    if (s1 == s2 && m_uniqueId < rhs.m_uniqueId)
        return true;
    return false;
}

// ============================================================================
// COnlinePreliminaryTeam（sizeof 0x40）
// ============================================================================

// ORIG T 0x08587eb8 C1：仅构造 +0x24 的 map（其余字段不初始化）。
COnlinePreliminaryTeam::COnlinePreliminaryTeam()
{
}

// ORIG T 0x08587ece D1：仅析构 +0x24 的 map。
COnlinePreliminaryTeam::~COnlinePreliminaryTeam()
{
}

// ORIG T 0x08697892 GetTeamId() const（W）：mov 0x4(%eax)。
// 【推断修正：objdump——旧桩误返 m_field00，ORIG 为 +0x4】
int COnlinePreliminaryTeam::GetTeamId() const
{
    return m_field04;
}

// ORIG W 0x0869789e GetTeamName() const：lea 0x14(%eax)（保留原语义）。
const char* COnlinePreliminaryTeam::GetTeamName() const
{
    return m_field14;
}

// ORIG T 0x08588374：clear m_playCountMap(+0x24)。
void COnlinePreliminaryTeam::ResetDailyMidnight()
{
    m_playCountMap.clear();
}

// ORIG T 0x0858838a Reset：+0/+4 清零；memset(+0x14,0,0x10)；
// +8/+0xc/+0x10 清零；clear map；+0x3c/+0x3d 标志清零。
void COnlinePreliminaryTeam::Reset()
{
    m_field00 = 0;
    m_field04 = 0;
    memset(m_field14, 0, 0x10);
    m_field08 = 0;
    m_winCount = 0;
    m_loseCount = 0;
    m_playCountMap.clear();
    m_hasWinFlag = 0;
    m_hasLoseFlag = 0;
}

// ORIG T 0x085883fe：++win(@0xc)；m_hasWinFlag(+0x3c)=1。
void COnlinePreliminaryTeam::IncreaseWinCount()
{
    ++m_winCount;
    m_hasWinFlag = 1;
}

// ORIG T 0x0858841a：++lose(@0x10)；+0x3c 置 1（ORIG 同写 0x3c，
// 非 0x3d——两条计数共用 dirty 标志）。
void COnlinePreliminaryTeam::IncreaseLoseCount()
{
    ++m_loseCount;
    m_hasWinFlag = 1;
}

// ORIG T 0x08588436：rating（位模式）写入 +0x08；+0x3c=1。
void COnlinePreliminaryTeam::SetRating(float rating)
{
    // ORIG 内联 float→int 位拷贝（mov），不调 memcpy（我们 17→ORIG 9）
    m_field08 = *reinterpret_cast<int*>(&rating);
    m_hasWinFlag = 1;
}

// ORIG W 0x08568eee GetRating() const【推断：objdump】：flds *(float*)(this+8)。
float COnlinePreliminaryTeam::GetRating() const
{
    return *reinterpret_cast<const float*>(&m_field08);   // ORIG flds *(float*)(this+8)（9 insns，不 memcpy）
}

// ---- CurrentUserCount 族：全部操作 *(int*)(this+0x0)，无标志写入 ----

// ORIG W 0x08588d96【推断：objdump】
void COnlinePreliminaryTeam::IncreaseCurrentUserCount()
{
    ++m_field00;
}

// ORIG W 0x08588da8【推断：objdump】
void COnlinePreliminaryTeam::DecreaseCurrentUserCount()
{
    --m_field00;
}

// ORIG W 0x08588d88【推断：objdump】
void COnlinePreliminaryTeam::SetCurrentUserCount(int n)
{
    m_field00 = n;
}

// ORIG W 0x08588d7e【推断：objdump】（非 const，mangled _ZN…GetCurrentUserCountEv）
int COnlinePreliminaryTeam::GetCurrentUserCount()
{
    return m_field00;
}

// ORIG T 0x085884b6：find；命中→flag=1、count++；未命中且 size>999 →
// cMyTrace 报警后直接 return（不置 0x3d）；否则插入 PlayCountInfoEx(1,true)；
// 最后 m_hasLoseFlag(+0x3d)=1。
void COnlinePreliminaryTeam::IncreaseEachOtherTeamPlayCount(int teamId)
{
    std::map<int, PlayCountInfoEx>::iterator it = m_playCountMap.find(teamId);
    if (it == m_playCountMap.end())
    {
        if (m_playCountMap.size() > 999)
        {
            // ORIG __PRETTY_FUNCTION__ 行号 0xca（202）
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xca, 0);
            trace("Online Preliminary each other team play count map size over! %d",
                  static_cast<unsigned int>(m_playCountMap.size()));
            return;
        }
        m_playCountMap.insert(std::make_pair(teamId, PlayCountInfoEx(1, true)));
    }
    else
    {
        it->second.m_flag = 1;
        it->second.m_count += 1;
    }
    m_hasLoseFlag = 1;
}

// ORIG T 0x0858844c：find；miss→0；hit→value.count。
int COnlinePreliminaryTeam::GetEachOtherTeamPlayCount(int teamId) const
{
    std::map<int, PlayCountInfoEx>::const_iterator it =
        m_playCountMap.find(teamId);
    if (it == m_playCountMap.end())
        return 0;
    return static_cast<int>(it->second.m_count);
}

// ORIG W 0x08588d60 saveData【推断：objdump】＝顺序调用 _saveTeam + _saveTeamMatchList。
void COnlinePreliminaryTeam::saveData()
{
    _saveTeam();
    _saveTeamMatchList();
}

// ORIG T 0x08587ee4：m_hasWinFlag==0 直通返回 1；
// 流（"OnlinePreliminaryTeam.cpp",0x38）：<<0x1e9、<<-1，
// SIG_ONLINE_PRELIMINARY_TEAM(buf 0x20)：+0=teamId(+0x4)、+0x14=rating(+0x8
// 位模式)、+0x18=win、+0x1c=lose；put(DB_Q)；清 m_hasWinFlag。
void COnlinePreliminaryTeam::_saveTeam()
{
    if (!m_hasWinFlag)
        return;

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryTeam.cpp", 0x38),
        true);
    guard.operator*()->operator<<(0x1e9);
    guard.operator*()->operator<<(-1);

    // SIG_ONLINE_PRELIMINARY_MEMBER/TEAM 结构为 pad 占位；手工偏移等价写法
    //（+0 teamId / +0x14 rating / +0x18 win / +0x1c lose，见报告 _saveTeam.md）。
    char* p = reinterpret_cast<char*>(guard.GetInBuffer<SIG_ONLINE_PRELIMINARY_TEAM>());
    *(reinterpret_cast<int*>(p + 0x00)) = m_field04;

    unsigned int ratingBits = static_cast<unsigned int>(m_field08);
    memcpy(p + 0x14, &ratingBits, sizeof(ratingBits));
    *(reinterpret_cast<int*>(p + 0x18)) = m_winCount;
    *(reinterpret_cast<int*>(p + 0x1c)) = m_loseCount;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_hasWinFlag = 0;
}

// ORIG T 0x0858800c：m_hasLoseFlag==0 直通；流（file,0x4e）：<<0x1ed、<<-1；
// SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST(0x328)：+0=teamId、+4=count；
// 遍历 map，flag==1 条目写入 +8+i*8=key、+0xc+i*8=count 并清 flag；
// 计满 100 → put 后 Set(Acquire(file,0x66)) 重建同格式头部续写；
// 收尾 count==0 → StreamPool::Free(guard)，否则 put；清 m_hasLoseFlag。
void COnlinePreliminaryTeam::_saveTeamMatchList()
{
    if (!m_hasLoseFlag)
        return;

    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryTeam.cpp", 0x4e),
        false);
    guard.operator*()->operator<<(0x1ed);
    guard.operator*()->operator<<(-1);

    char* p = reinterpret_cast<char*>(
        guard.GetInBuffer<SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST>());
    *(reinterpret_cast<int*>(p + 0)) = m_field04;
    *(reinterpret_cast<int*>(p + 4)) = 0;

    for (std::map<int, PlayCountInfoEx>::iterator it = m_playCountMap.begin();
         it != m_playCountMap.end(); ++it)
    {
        if (!it->second.m_flag)
            continue;
        it->second.m_flag = 0;

        int idx = *(reinterpret_cast<int*>(p + 4));
        *(reinterpret_cast<int*>(p + 8 + idx * 8)) = it->first;
        *(reinterpret_cast<unsigned int*>(p + 0xc + idx * 8)) =
            it->second.m_count;
        *(reinterpret_cast<int*>(p + 4)) = idx + 1;

        if (*(reinterpret_cast<int*>(p + 4)) == 100)
        {
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            guard.Set(GlobalData::s_stream_pool->Acquire(
                "OnlinePreliminaryTeam.cpp", 0x66));
            p = reinterpret_cast<char*>(
                guard.GetInBuffer<SIG_ONLINE_PRELIMINARY_TEAM_MATCH_LIST>());
            *(reinterpret_cast<int*>(p + 0)) = m_field04;
            *(reinterpret_cast<int*>(p + 4)) = 0;
        }
    }

    if (*(reinterpret_cast<int*>(p + 4)) == 0)
        GlobalData::s_stream_pool->Free(guard);
    else
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_hasLoseFlag = 0;
}

// ORIG T 0x085882b8：data[0]→teamId(+0x4)；strncpy(name,data+4,0xf)；
// data[0x14]→rating(+0x8)、data[0x18]→win、data[0x1c]→lose；返回 1。
void COnlinePreliminaryTeam::loadDataPreliminaryTeam(char* data)
{
    m_field04 = *(reinterpret_cast<int*>(data));
    strncpy(m_field14, data + 4, 0xf);
    m_field08 = *(reinterpret_cast<int*>(data + 0x14));
    m_winCount = *(reinterpret_cast<int*>(data + 0x18));
    m_loseCount = *(reinterpret_cast<int*>(data + 0x1c));
}

// ORIG T 0x0858831a：data 布局 int matchCount@+4 + 每 8 字节记录
// （key@0/value(count)@4）；逐条写入 m_playCountMap[key].m_count。
int COnlinePreliminaryTeam::loadDataPreliminaryTeamMatchList(char* data)
{
    int count = *(reinterpret_cast<int*>(data + 4));
    for (int i = 0; i < count; ++i)
    {
        int key = *(reinterpret_cast<int*>(data + 8 + i * 8));
        int value = *(reinterpret_cast<int*>(data + 8 + i * 8 + 4));
        m_playCountMap[key].m_count = static_cast<unsigned int>(value);
    }
    return 1;
}

// ============================================================================
// COnlinePreliminaryMember（CData 派生，sizeof 0x14）
// ============================================================================

// ORIG T 0x08585f34 C1：charac_expand::CData ctor + 置本类虚表（编译器生成）。
// ORIG 不初始化 m_rating/m_winCount/m_loseCount（构造后为垃圾值）。
COnlinePreliminaryMember::COnlinePreliminaryMember()
{
}

// ORIG T 0x08585f50 D1 / 0x08585f80 D0：恢复基类虚表 + 基类析构（编译器生成）。
COnlinePreliminaryMember::~COnlinePreliminaryMember()
{
}

// ORIG T 0x08585f9e：pData+8→rating(位)、+0xc→win、+0x10→lose；返回 true。
bool COnlinePreliminaryMember::loadData(CUser* /*pUser*/, char* pData)
{
    memcpy(&m_rating, pData + 8, sizeof(m_rating));
    m_winCount = *(reinterpret_cast<int*>(pData + 0xc));
    m_loseCount = *(reinterpret_cast<int*>(pData + 0x10));
    return true;
}

// ORIG T 0x08585fd6：流（"OnlinePreliminaryData.cpp",0x21）：<<0x1e7、<<uid；
// SIG_ONLINE_PRELIMINARY_MEMBER(0x14)：+0=u16 ServerGroup、+4=curCharacNo、
// +8=rating(位)、+0xc=win、+0x10=lose；put(DB_Q)；返回 1。
bool COnlinePreliminaryMember::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryData.cpp", 0x21),
        true);
    guard.operator*()->operator<<(0x1e7);
    guard.operator*()->operator<<(static_cast<int>(pUser->GetUID()));

    char* p = reinterpret_cast<char*>(
        guard.GetInBuffer<SIG_ONLINE_PRELIMINARY_MEMBER>());
    *(reinterpret_cast<unsigned short*>(p + 0)) =
        static_cast<unsigned short>(pUser->GetServerGroup());
    *(reinterpret_cast<unsigned int*>(p + 4)) =
        static_cast<unsigned int>(pUser->getCurCharacNo());
    memcpy(p + 8, &m_rating, sizeof(m_rating));
    *(reinterpret_cast<int*>(p + 0xc)) = m_winCount;
    *(reinterpret_cast<int*>(p + 0x10)) = m_loseCount;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

// ORIG T 0x08586100：三字段清零（rating 位清 0）。
void COnlinePreliminaryMember::_reset()
{
    m_rating = 0.0f;
    m_winCount = 0;
    m_loseCount = 0;
}

// ORIG T 0x08586124 / 0x08586146 / 0x08586168：改字段后 alter() 标脏。
void COnlinePreliminaryMember::IncreaseWinCount()
{
    ++m_winCount;
    alter();
}

void COnlinePreliminaryMember::IncreaseLoseCount()
{
    ++m_loseCount;
    alter();
}

void COnlinePreliminaryMember::SetRating(float rating)
{
    m_rating = rating;
    alter();
}

// ============================================================================
// COnlinePreliminaryMemberMatchList（CData 派生，sizeof 0x20）
// ============================================================================

// ORIG T 0x08586398 C1：CData ctor + 构造 +0x08 map。
COnlinePreliminaryMemberMatchList::COnlinePreliminaryMemberMatchList()
{
}

// ORIG T 0x085863e6 D1 / 0x08586448 D0。
COnlinePreliminaryMemberMatchList::~COnlinePreliminaryMemberMatchList()
{
}

// ORIG T 0x08586466：count@data+8；每条 12B：u16 serverId@+0xc、
// u32 uniqueId@+0x10、u32 playCount@+0x14；写入 m_peerMap[key].m_count。
bool COnlinePreliminaryMemberMatchList::loadData(CUser* /*pUser*/, char* pData)
{
    int count = *(reinterpret_cast<int*>(pData + 8));
    for (int i = 0; i < count; ++i)
    {
        unsigned short serverId =
            *(reinterpret_cast<unsigned short*>(pData + i * 0xc + 0xc));
        unsigned int uniqueId =
            *(reinterpret_cast<unsigned int*>(pData + i * 0xc + 0x10));
        CharacterInServer key(static_cast<int>(serverId), uniqueId);
        m_peerMap[key].m_count =
            *(reinterpret_cast<unsigned int*>(pData + i * 0xc + 0x14));
    }
    return true;
}

// ORIG T 0x08586510：流（"OnlinePreliminaryData.cpp",0x82）：<<0x1eb、<<uid；
// SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST(0x4bc)：+0=u16 group、+4=u32 characNo、
// +8=count；遍历 map，flag==1 条目写入 +0xc+i*0xc=sid(u16)、+0x10=uid、
// +0x14=count 并清 flag；计满 100 → put(主 guard) 后新流（Acquire file,0x9d）
// 建临时守卫写 MEMBER 格式头（<<0x1e7、<<uid、group/characNo/count 复位）并将
// 工作指针切至其缓冲（ORIG 忠实行为，含守卫析构）；收尾 count==0 → Free，
// 否则 put；最后析构主守卫。返回 1。
bool COnlinePreliminaryMemberMatchList::_saveData(CUser* pUser)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("OnlinePreliminaryData.cpp", 0x82),
        true);
    guard.operator*()->operator<<(0x1eb);
    guard.operator*()->operator<<(static_cast<int>(pUser->GetUID()));

    typedef SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST SigT;
    char* p = reinterpret_cast<char*>(guard.GetInBuffer<SigT>());
    *(reinterpret_cast<unsigned short*>(p + 0)) =
        static_cast<unsigned short>(pUser->GetServerGroup());
    *(reinterpret_cast<unsigned int*>(p + 4)) =
        static_cast<unsigned int>(pUser->getCurCharacNo());
    *(reinterpret_cast<int*>(p + 8)) = 0;

    for (std::map<CharacterInServer, PlayCountInfo>::iterator it =
             m_peerMap.begin();
         it != m_peerMap.end(); ++it)
    {
        if (!it->second.m_flag)
            continue;
        it->second.m_flag = 0;

        int idx = *(reinterpret_cast<int*>(p + 8));
        *(reinterpret_cast<unsigned short*>(p + 0xc + idx * 0xc)) =
            static_cast<unsigned short>(it->first.m_serverId);
        *(reinterpret_cast<unsigned int*>(p + 0x10 + idx * 0xc)) =
            it->first.m_uniqueId;
        *(reinterpret_cast<unsigned int*>(p + 0x14 + idx * 0xc)) =
            it->second.m_count;
        *(reinterpret_cast<int*>(p + 8)) = idx + 1;

        if (*(reinterpret_cast<int*>(p + 8)) == 100)
        {
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            // 分批重建：新流临时守卫；写完 header 后指针切至其 InBuffer，
            // 临时守卫随即析构（对应 ORIG D1 @0x08586777）。
            CStreamGuard tmp(
                GlobalData::s_stream_pool->Acquire("OnlinePreliminaryData.cpp",
                                                   0x9d),
                true);
            tmp.operator*()->operator<<(0x1e7);
            tmp.operator*()->operator<<(static_cast<int>(pUser->GetUID()));
            p = reinterpret_cast<char*>(
                tmp.GetInBuffer<SIG_ONLINE_PRELIMINARY_MEMBER_MATCH_LIST>());
            *(reinterpret_cast<unsigned short*>(p + 0)) =
                static_cast<unsigned short>(pUser->GetServerGroup());
            *(reinterpret_cast<unsigned int*>(p + 4)) =
                static_cast<unsigned int>(pUser->getCurCharacNo());
            *(reinterpret_cast<int*>(p + 8)) = 0;
        }
    }

    if (*(reinterpret_cast<int*>(p + 8)) == 0)
        GlobalData::s_stream_pool->Free(guard);
    else
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}

// ORIG T 0x08586848：clear map(+0x08)。
void COnlinePreliminaryMemberMatchList::_reset()
{
    m_peerMap.clear();
}

// ORIG T 0x0858685e：经 vtable +0x1c 槽虚调用 _reset。
void COnlinePreliminaryMemberMatchList::ResetDailyMidnight()
{
    _reset();
}

// ORIG T 0x08586208：find；命中→flag=1、count++；未命中且 size>999 →
// cMyTrace 报警后直接 return（不 alter）；否则插入 PlayCountInfo(1,true)；
// 末尾 alter()。
void COnlinePreliminaryMemberMatchList::IncreasePeerPlayCount(
    int serverId, unsigned int uniqueId)
{
    CharacterInServer key(serverId, uniqueId);
    std::map<CharacterInServer, PlayCountInfo>::iterator it =
        m_peerMap.find(key);
    if (it == m_peerMap.end())
    {
        if (m_peerMap.size() > 999)
        {
            // ORIG __PRETTY_FUNCTION__ 行号 0x65（101）
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x65, 0);
            trace("Online Preliminary peer play count map size over! %d",
                  static_cast<unsigned int>(m_peerMap.size()));
            return;
        }
        m_peerMap.insert(std::make_pair(key, PlayCountInfo(1, true)));
    }
    else
    {
        it->second.m_flag = 1;
        it->second.m_count += 1;
    }
    alter();
}

// ORIG T 0x08586184（W 内联）：find；miss→0；hit→value.count。
unsigned int COnlinePreliminaryMemberMatchList::GetPeerPlayCount(
    int serverId, unsigned int uniqueId) const
{
    CharacterInServer key(serverId, uniqueId);
    std::map<CharacterInServer, PlayCountInfo>::const_iterator it =
        m_peerMap.find(key);
    if (it == m_peerMap.end())
        return 0;
    return it->second.m_count;
}

// ============================================================================
// CTeamFactory：StaticPool<COnlinePreliminaryTeam,600> 薄包装
//（ctor/dtor 转调基类 StaticPool 生命周期，定义在 GlobalData.cpp）
// ============================================================================

// ORIG T 0x08588dd8：转调基类 StaticPool::Acquire（0x08588e56）。
COnlinePreliminaryTeam* CTeamFactory::Acquire()
{
    return StaticPool<COnlinePreliminaryTeam, 600>::Acquire();
}

// ORIG T 0x08588dec：转调基类 StaticPool::Free。
void CTeamFactory::Free(COnlinePreliminaryTeam* team)
{
    StaticPool<COnlinePreliminaryTeam, 600>::Free(team);
}

}  // namespace online_preliminary
