// df_game_r 还原 —— disconnect_detecter::CDisconnectDetecter（断线检测器，
// G2-4 批次）。逐函数对照 docs/class_func_reports/disconnect_detecter__CDisconnectDetecter.md
// 与 ORIG 反汇编实现；数据流参照 Inter_Disconnect / Inter_DisconnectMonitorServer /
// Inter_HadesDisConnect / TimerDetectDisconnect 报告。
// 布局：+0x00 std::map<DetectedObjectKey,int> m_map（sizeof 0x18）
// DetectedObjectKey：+0 uid（CUser::GetUID）| +4 uniqueId（get_unique_id & 0xffff）
// map value = 最后收包时间（CSystemTime::getCurSec），超时 30s 判断线。
#include <map>
#include <utility>
#include "TimerQueue.h"

TimerQueue* G_TimerQueue();  // ORIG W 0x80f647c

// ---------------------------------------------------------------------------
// 外部依赖最小声明（实现属其它 TU）
// ---------------------------------------------------------------------------
class CSystemTime
{
public:
    int getCurSec();   // 0x080cbc9e
};

namespace GlobalData
{
extern CSystemTime s_systemTime_;   // 0x0941f714
}

class CUser
{
public:
    unsigned int GetUID() const;      // 0x080c8c96
    int get_unique_id() const;        // 0x080da37c
    void OnDisconnect();              // 0x086804ce
};

class CGameManager
{
public:
    CUser* getUser(int slotId, int increId);   // 0x082947a4
};

CGameManager* G_CGameManager();       // 0x080cc18e

class TimerDetectDisconnect
{
public:
    static void registNextTimer();  // ORIG T 0x086374e0
};

// ---- TimerDetectDisconnect::registNextTimer @ 0x086374e0 ----
// 断线检测器定时器：10 秒后触发 dispatch_sig(0x6d)，若 DoDetectingDisconnect
// 仍检测到断线用户则再次登记（自续期），见 TimerDetectDisconnect 报告。
void TimerDetectDisconnect::registNextTimer()
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x6d, 10, 0, 0);
}

// cMyTrace 由 TimerQueue.h 间接引入的 GameTypes.h 声明（实现属独立 TU）。

namespace disconnect_detecter
{

class DetectedObjectKey
{
public:
    DetectedObjectKey() {}
    DetectedObjectKey(int uid, int uniqueId);   // 0x0847266e W
    bool operator<(const DetectedObjectKey& other) const;  // 0x0847262a W

    int m_uid;        // +0x00
    int m_uniqueId;   // +0x04
};

class CDisconnectDetecter
{
public:
    CDisconnectDetecter();    // 0x082348cc W
    ~CDisconnectDetecter();   // 0x082348b8 W

    bool RegisterUser(CUser* user);            // 0x08472272 T
    bool UnregisterUser(CUser* user);          // 0x0847236a T
    bool RefreshRecvTime(CUser* user);         // 0x08472408 T
    bool DoDetectingDisconnect();              // 0x084724b0 T
    void OnDisconnect(CUser* user);            // 0x084725ba T

    bool _IsTimeout(int time);                 // 0x084721f4 T
    CUser* _GetUser(int uid, int uniqueId);    // 0x08472222 T

    std::map<DetectedObjectKey, int> m_map;    // +0x00
};

CDisconnectDetecter* GetInstanceDisconnectDetecter();  // 0x082348df W

// ============================================================================
// DetectedObjectKey 实现
// ============================================================================

DetectedObjectKey::DetectedObjectKey(int uid, int uniqueId)
    : m_uid(uid), m_uniqueId(uniqueId)
{
}

bool DetectedObjectKey::operator<(const DetectedObjectKey& other) const
{
    if (m_uid < other.m_uid)
    {
        return true;
    }
    if (m_uid != other.m_uid)
    {
        return false;
    }
    if (m_uniqueId < other.m_uniqueId)
    {
        return true;
    }
    return false;
}

// ============================================================================
// CDisconnectDetecter 实现
// ============================================================================

CDisconnectDetecter::CDisconnectDetecter()
    : m_map()
{
}

CDisconnectDetecter::~CDisconnectDetecter()
{
}

bool CDisconnectDetecter::RegisterUser(CUser* user)
{
    int mapSize = m_map.size();
    long curSec = GlobalData::s_systemTime_.getCurSec();
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
    std::pair<std::map<DetectedObjectKey, int>::iterator, bool> ret =
        m_map.insert(std::make_pair<DetectedObjectKey, long>(key, curSec));
    if (ret.second == false)
    {
        cMyTrace trace(
            "bool disconnect_detecter::CDisconnectDetecter::RegisterUser(CUser*)",
            0x25, 0);
        trace("RegisterUser() wrong");
        return 0;
    }
    if (mapSize == 0)
    {
        TimerDetectDisconnect::registNextTimer();
    }
    return 1;
}

bool CDisconnectDetecter::UnregisterUser(CUser* user)
{
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
    std::map<DetectedObjectKey, int>::iterator it = m_map.find(key);
    if (it != m_map.end())
    {
        m_map.erase(it);
        return true;
    }
    return false;
}

bool CDisconnectDetecter::RefreshRecvTime(CUser* user)
{
    DetectedObjectKey key(user->GetUID(), user->get_unique_id() & 0xffff);
    std::map<DetectedObjectKey, int>::iterator it = m_map.find(key);
    bool found = it != m_map.end();
    if (found)
    {
        it->second = GlobalData::s_systemTime_.getCurSec();
    }
    return found;
}

bool CDisconnectDetecter::DoDetectingDisconnect()
{
    for (std::map<DetectedObjectKey, int>::iterator it = m_map.begin();
         it != m_map.end(); )
    {
        if (_IsTimeout(it->second))
        {
            CUser* user = _GetUser(it->first.m_uid, it->first.m_uniqueId);
            if (user != 0)
            {
                OnDisconnect(user);
            }
            m_map.erase(it++);
        }
        else
        {
            ++it;
        }
    }
    return !m_map.empty();
}

void CDisconnectDetecter::OnDisconnect(CUser* user)
{
    user->OnDisconnect();
}

bool CDisconnectDetecter::_IsTimeout(int time)
{
    return GlobalData::s_systemTime_.getCurSec() - time > 30;
}

CUser* CDisconnectDetecter::_GetUser(int uid, int uniqueId)
{
    CUser* user = G_CGameManager()->getUser(uid, 2);
    if (user != 0)
    {
        if ((user->get_unique_id() & 0xffff) != uniqueId)
        {
            user = 0;
        }
    }
    return user;
}

CDisconnectDetecter* GetInstanceDisconnectDetecter()
{
    static CDisconnectDetecter instance;
    return &instance;
}

}  // namespace disconnect_detecter
