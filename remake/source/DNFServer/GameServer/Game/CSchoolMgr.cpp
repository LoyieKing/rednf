// df_game_r 还原 —— CSchoolMgr（学院 PvP 频道用户/学院分区管理器，G2-4 批次）。
// 逐函数对照 docs/class_func_reports/CSchoolMgr.md 与 ORIG 反汇编实现。
// 布局（ORIG ctor 0x856f206 / dtor 0x856f0ba 推导）：
//   CSchoolMgr ：+0x00 std::map<uint,uint> m_schoolUserCount
//                +0x18 CSchoolArea m_schoolArea（sizeof 0x28）
//   CSchoolArea：+0x00 std::map<uint,int> m_areaMap
//                +0x18 std::stack<int> m_areaStack
//   CSchoolMember：CBelong(+0 vptr) + CUser*（sizeof 8）；IsRight 覆写纯虚。
// 外部依赖（RDARScriptStringManager / GameWorld::send_group / CUser 访问器）
// 以 asm-label extern 或本地视图声明对齐 ORIG 符号。
#include <stdio.h>
#include <string.h>

#include <map>
#include <stack>
#include <deque>

#include "InterfacePacketBuf.h"
#include "CEnvironment.h"
#include "GameWorld.h"

// ---------------------------------------------------------------------------
// 外部符号声明（实现属其它 TU / GameStubs）
// ---------------------------------------------------------------------------
class CUser;

extern "C" char* sub_RDARScriptStringManager_findString(
    void* mgr, int idx, const char* key, bool* out)
    asm("_ZNK23RDARScriptStringManager10findStringEiPKcPb");
extern "C" void sub_GameWorld_send_group(void* world, void* packet, void* belong)
    asm("_ZN9GameWorld10send_groupER11PacketGuardP7CBelong");

// ---------------------------------------------------------------------------
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char）
// ---------------------------------------------------------------------------
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// ---------------------------------------------------------------------------
// CBelong / CSchoolMember（send_group 的归属对象；IsRight 为纯虚）
// ---------------------------------------------------------------------------
class CBelong
{
public:
    CBelong();
    virtual bool IsRight(CUser* user) = 0;
};

class CSchoolMember : public CBelong
{
public:
    CSchoolMember(CUser* user);             // 0x085690b6 W
    virtual bool IsRight(CUser* user);      // 0x085690dc W

    CUser* m_user;   // +0x04
};

// ---------------------------------------------------------------------------
// CSchoolArea（学院分区：area 号映射 + 空闲区号栈）
// ---------------------------------------------------------------------------
class CSchoolArea
{
public:
    CSchoolArea();                 // 0x0856f18c W
    ~CSchoolArea();                // 0x0856f074 W

    int GetAreaIndex();                       // 0x085689a6 W
    void SetAreaMapping(unsigned int schoolNo, int areaIdx);  // 0x085689ec W
    void AddSchoolArea(unsigned int schoolNo);                // 0x08568a3e W
    void DelSchoolArea(unsigned int schoolNo);                // 0x08568a74 W
    void SetSchoolArea(int start, int end);                   // 0x086d23bc W
    int GetAreaIndex(unsigned int schoolNo);                  // 0x086d23f8 W

    std::map<unsigned int, int> m_areaMap;   // +0x00
    std::stack<int> m_areaStack;             // +0x18
};

// ---------------------------------------------------------------------------
// CSchoolMgr
// ---------------------------------------------------------------------------
class CSchoolMgr
{
public:
    CSchoolMgr();                 // 0x0856f206 W
    ~CSchoolMgr();                // 0x0856f0ba W

    void AddUser(CUser* user);                // 0x085683a0 T
    void DelUser(CUser* user);                // 0x085684bc T
    void SendBroadcastUserIn(CUser* user);    // 0x08567fe8 T
    void SendBroadcastUserOut(CUser* user);   // 0x085681ce T
    unsigned int GetUserCount(unsigned int schoolNo);  // 0x084ed394 W
    void SetSchoolArea(int start, int end);   // 0x086d245c W
    int GetSchoolArea(unsigned int schoolNo); // 0x086d2480 W

    std::map<unsigned int, unsigned int> m_schoolUserCount;  // +0x00
    CSchoolArea m_schoolArea;                                 // +0x18
};

// ============================================================================
// CSchoolArea 实现
// ============================================================================

CSchoolArea::CSchoolArea()
    : m_areaMap(), m_areaStack()
{
}

CSchoolArea::~CSchoolArea()
{
}

int CSchoolArea::GetAreaIndex()
{
    if (m_areaStack.empty())
    {
        return -1;
    }
    int areaIdx = m_areaStack.top();
    m_areaStack.pop();
    return areaIdx;
}

void CSchoolArea::SetAreaMapping(unsigned int schoolNo, int areaIdx)
{
    m_areaMap.insert(std::make_pair(schoolNo, areaIdx));
}

void CSchoolArea::AddSchoolArea(unsigned int schoolNo)
{
    int areaIdx = GetAreaIndex();
    if (areaIdx >= 0)
    {
        SetAreaMapping(schoolNo, areaIdx);
    }
}

void CSchoolArea::DelSchoolArea(unsigned int schoolNo)
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(schoolNo);
    if (it != m_areaMap.end())
    {
        m_areaStack.push(it->second);
        m_areaMap.erase(it);
    }
}

void CSchoolArea::SetSchoolArea(int start, int end)
{
    for (int area = start; area < end; ++area)
    {
        m_areaStack.push(area);
    }
}

int CSchoolArea::GetAreaIndex(unsigned int schoolNo)
{
    std::map<unsigned int, int>::iterator it = m_areaMap.find(schoolNo);
    if (it != m_areaMap.end())
    {
        return it->second;
    }
    return -1;
}

// ============================================================================
// CBelong / CSchoolMember 实现
// ============================================================================

CBelong::CBelong()
{
}

CSchoolMember::CSchoolMember(CUser* user)
    : CBelong(), m_user(user)
{
}

bool CSchoolMember::IsRight(CUser* user)
{
    if (m_user->GetSchoolNo() == user->GetSchoolNo() && m_user != user)
    {
        return true;
    }
    return false;
}

// ============================================================================
// CSchoolMgr 实现
// ============================================================================

CSchoolMgr::CSchoolMgr()
    : m_schoolUserCount(), m_schoolArea()
{
}

CSchoolMgr::~CSchoolMgr()
{
}

void CSchoolMgr::AddUser(CUser* user)
{
    if (!G_GameWorld()->IsSchoolPvPChannel())
    {
        return;
    }
    SendBroadcastUserIn(user);
    unsigned int schoolNo = user->GetSchoolNo();
    std::map<unsigned int, unsigned int>::iterator it =
        m_schoolUserCount.find(schoolNo);
    if (it != m_schoolUserCount.end())
    {
        it->second += 1;
    }
    else
    {
        m_schoolUserCount.insert(std::make_pair(user->GetSchoolNo(), 1));
        m_schoolArea.AddSchoolArea(user->GetSchoolNo());
    }
}

void CSchoolMgr::DelUser(CUser* user)
{
    if (!G_GameWorld()->IsSchoolPvPChannel())
    {
        return;
    }
    SendBroadcastUserOut(user);
    unsigned int schoolNo = user->GetSchoolNo();
    std::map<unsigned int, unsigned int>::iterator it =
        m_schoolUserCount.find(schoolNo);
    if (it != m_schoolUserCount.end())
    {
        it->second -= 1;
        if (it->second == 0)
        {
            m_schoolUserCount.erase(it);
            m_schoolArea.DelSchoolArea(user->GetSchoolNo());
        }
    }
}

void CSchoolMgr::SendBroadcastUserIn(CUser* user)
{
    char* channelMsg = sub_RDARScriptStringManager_findString(
        (void*)0x0949b140, 4, "channel_connect", 0);
    if (channelMsg != 0)
    {
        char buf[255] = {0};
        sprintf(buf, channelMsg, user->getCurCharacName(),
                G_CEnvironment()->get_channel_no());
        PacketGuard guard;
        guard.put_header(0, 0xc);
        guard.put_byte(0);
        guard.put_short(0);
        guard.put_byte(0);
        guard.put_int(strlen(buf));
        guard.put_str(buf, strlen(buf));
        guard.finalize(true);
        CSchoolMember member(user);
        sub_GameWorld_send_group(G_GameWorld(), &guard, &member);
    }
}

void CSchoolMgr::SendBroadcastUserOut(CUser* user)
{
    char* channelMsg = sub_RDARScriptStringManager_findString(
        (void*)0x0949b140, 4, "channel_disconnect", 0);
    if (channelMsg != 0)
    {
        char buf[255] = {0};
        sprintf(buf, channelMsg, user->getCurCharacName());
        PacketGuard guard;
        guard.put_header(0, 0xc);
        guard.put_byte(0);
        guard.put_short(0);
        guard.put_byte(0);
        guard.put_int(strlen(buf));
        guard.put_str(buf, strlen(buf));
        guard.finalize(true);
        CSchoolMember member(user);
        sub_GameWorld_send_group(G_GameWorld(), &guard, &member);
    }
}

unsigned int CSchoolMgr::GetUserCount(unsigned int schoolNo)
{
    std::map<unsigned int, unsigned int>::iterator it =
        m_schoolUserCount.find(schoolNo);
    if (it != m_schoolUserCount.end())
    {
        return it->second;
    }
    return 0;
}

void CSchoolMgr::SetSchoolArea(int start, int end)
{
    m_schoolArea.SetSchoolArea(start, end);
}

int CSchoolMgr::GetSchoolArea(unsigned int schoolNo)
{
    return m_schoolArea.GetAreaIndex(schoolNo);
}
