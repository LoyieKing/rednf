// ============================================================================
// df_game_r CSharedServerMessageManager（G2-5 批次，共享服务器公告管理）
// 逐函数对照 docs/class_func_reports/CSharedServerMessageManager.md 与
// CSharedServerMessageManager__CSharedServerMessageAdapter.md + ORIG 反汇编实现。
// 布局（ORIG ctor 0x0860079c / dtor 0x08600892 推导）：
//   +0x00 CSharedServerMessageAdapter m_adapter（4：stSharedServerMessageInfo*）
//   +0x04 unsigned char m_isSet
//   +0x08 EVENT_SERVER_MESSAGE m_event（unsigned char + vector，0x10）
//   sizeof = 0x18
// stSharedServerMessageInfo（Adapter 解引用对象）：
//   +0x00 vector<int> m_vecIndex | +0x0c vector<pair<int,int>> m_vecMapped
//   +0x18 vector<pair<int,int>> m_vecShareKind
// stSERVER_MESSAGE = 0x58（kind/index/len/msg[52]/len2/msg2[20]/endTime）。
// ============================================================================

#include <cstring>
#include <string.h>

#include <utility>
#include <vector>

// ---- 本地消息结构（布局按 ORIG 反汇编） ----
struct stSERVER_MESSAGE
{
    ~stSERVER_MESSAGE() {}

    unsigned char m_kind;    // +0x00
    unsigned char m_index;   // +0x01
    int m_lenMsg;            // +0x04
    char m_msg[52];          // +0x08
    int m_lenMsg2;           // +0x3c
    char m_msg2[20];         // +0x40
    int m_endTime;           // +0x54
};

struct EVENT_SERVER_MESSAGE
{
    EVENT_SERVER_MESSAGE() {}
    ~EVENT_SERVER_MESSAGE() {}

    void clear()
    {
        m_count = 0;
        m_vecMsg.clear();
    }

    unsigned char m_count;                   // +0x00
    std::vector<stSERVER_MESSAGE> m_vecMsg;  // +0x04
};

struct SIG_EVENT_SERVER_MESSAGE
{
    unsigned char m_count;          // +0x00
    stSERVER_MESSAGE m_msg[0x64];   // +0x04
};

struct stSharedServerMessageInfo
{
    std::vector<int> m_vecIndex;                     // +0x00
    std::vector<std::pair<int, int> > m_vecMapped;   // +0x0c
    std::vector<std::pair<int, int> > m_vecShareKind; // +0x18
};

struct SIG_SELECT_SERVER_MESSAGE  // 4 字节
{
    short m_server;    // +0x00
    short m_channel;   // +0x02
};

struct SIG_UPDATE_SERVER_MESSAGE  // 0x60
{
    int m_server;          // +0x00
    int m_channel;         // +0x04
    unsigned char m_kind;  // +0x08
    unsigned char m_index; // +0x09
    int m_lenMsg;          // +0x0c
    char m_msg[0x34];      // +0x10
    int m_lenMsg2;         // +0x44
    char m_msg2[0x14];     // +0x48
    int m_endTime;         // +0x5c
};

// ---- Stream / CStreamGuard（S1 组；本 TU 只声明 + 实例化模板） ----
class Stream
{
public:
    int size();
    int resize(int n);
    bool enable_in(int n);
    void in_ptr(int n);

    template <class T> T* GetInBuffer();

    void* m_vptr;    // +0x00
    int m_size;      // +0x04
    char* m_buffer;  // +0x08
    int m_inPtr;     // +0x0c
    int m_outPtr;    // +0x10
};

template <class T>
T* Stream::GetInBuffer()
{
    while (!enable_in((int)sizeof(T)))
        resize(size() * 2);
    char* p = m_buffer + m_inPtr;
    in_ptr((int)sizeof(T));
    return (T*)p;
}

class CStreamGuard
{
public:
    CStreamGuard(Stream* pStream, bool bOwn);
    ~CStreamGuard();
    CStreamGuard* operator->();
    CStreamGuard* operator*();
    Stream& operator<<(int v);

    template <class T> T* GetInBuffer();

    Stream* m_stream;  // +0x00
    bool m_bOwn;       // +0x04
};

template <class T>
T* CStreamGuard::GetInBuffer()
{
    return m_stream->GetInBuffer<T>();
}

class StreamPool
{
public:
    Stream* Acquire(const char* file, int line);
};

class MsgQueueMgr
{
public:
    enum QUEUE_IDX
    {
        NETWORK_Q = 0,
        DISPATCH_Q = 1,
        DB_Q = 2
    };

    void put(QUEUE_IDX queue_idx, CStreamGuard& guard);
};

namespace GlobalData
{
extern StreamPool s_stream_pool;
extern MsgQueueMgr s_msgq_mgr;
}

// ---- CUser 族最小声明（实现属其它 TU；基类偏移 0 保证 this 直通） ----
enum INVEN_TYPE
{
    INVEN_TYPE_INVEN = 0,
    INVEN_TYPE_EQUIP = 1
};

enum eItemDelReason
{
    eItemDelReason_3 = 3,
    eItemDelReason_0x1d = 0x1d
};

enum ENUM_ITEMSPACE
{
    ENUM_ITEMSPACE_INVEN = 0
};

class PacketGuard;
class InterfacePacketBuf;

class CInventory
{
public:
    int check_item_exist(int itemId) const;
    bool delete_item(INVEN_TYPE type, int slot, int count,
                     eItemDelReason reason, bool bFlag);
};

class CUserCharacInfo
{
public:
    CInventory* getCurCharacInvenR() const;
    CInventory* getCurCharacInvenW();
};

class CUser : public CUserCharacInfo
{
public:
    enum eSendTarget
    {
        eSendTarget_1 = 1
    };

    void Send(PacketGuard& pg);
    unsigned int GetUID() const;
    bool SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot);
};

class CEnvironment
{
public:
    int get_channel_no();

    char m_pad[0x378];
    int m_field378;   // +0x378
};

CEnvironment* G_CEnvironment();

class cMyTrace
{
public:
    cMyTrace(const char* name, int line, int flag);
    void operator()(const char* fmt, ...);

    char m_pad[0x10];
};

// ---- PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + 内部字段） ----
class InterfacePacketBuf
{
public:
    bool put_header(int packetId, int packetSize);
    bool put_byte(int value);
    bool put_int(int value);
    bool put_str(char* str, int len);
    void finalize(bool isEncryptRequired);

    char m_pad[4];   // +0x00
};

class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();
};

// ============================================================================
// CSharedServerMessageManager
// ============================================================================
class CSharedServerMessageManager
{
public:
    class CSharedServerMessageAdapter
    {
    public:
        CSharedServerMessageAdapter() { m_pInfo = 0; }
        ~CSharedServerMessageAdapter() {}
        void SetServerMessageAddInfo(stSharedServerMessageInfo* pInfo) { m_pInfo = pInfo; }

        int GetMappedEventID(int nIndex);
        bool IsOnIndex(int nIndex);
        int GetShareKindWithIndex(int nIndex);

        stSharedServerMessageInfo* m_pInfo;   // +0x00
    };

    CSharedServerMessageManager();
    CSharedServerMessageManager(stSharedServerMessageInfo* pInfo);
    ~CSharedServerMessageManager();

    void SendServerMessageInfo(CUser* pUser, SIG_EVENT_SERVER_MESSAGE* pMsg, char bFlag);
    void SetServerMessage(SIG_EVENT_SERVER_MESSAGE* pMsg);
    stSERVER_MESSAGE* getCurMessage(int nIndex);
    bool IsChangableMessage(int nIndex, int nTime);
    void DeleteSpecificMessage(int nIndex);
    bool CheckUserCondition(CUser* pUser, int nIndex, int& nOut);
    void UpdateServerMessage(CUser* pUser, int nIndex, char* pMsg, char* pMsg2, int nEndTime);
    void UpdateServerMessageByBroadcast(int nIndex, char* pMsg, char* pMsg2, int nEndTime);
    void LoadServerMessageFromDB();

    bool IsSetServerMessage() { return m_isSet != 0; }
    EVENT_SERVER_MESSAGE* GetCurServerMessage() { return &m_event; }
    void setLoadingComplete() { m_isSet = 1; }

    CSharedServerMessageAdapter m_adapter;  // +0x00
    unsigned char m_isSet;                  // +0x04
    char m_pad[3];                          // +0x05
    EVENT_SERVER_MESSAGE m_event;           // +0x08
};

// ----------------------------------------------------------------------------
// Adapter：索引/映射表查询
// ----------------------------------------------------------------------------
int CSharedServerMessageManager::CSharedServerMessageAdapter::GetMappedEventID(int nIndex)
{
    std::vector<std::pair<int, int> >::iterator it = m_pInfo->m_vecMapped.begin();
    while (it != m_pInfo->m_vecMapped.end())
    {
        std::pair<int, int>* pPair = &(*it);
        if (pPair == 0)
        {
            ++it;
            continue;
        }
        if (pPair->first == nIndex)
            return pPair->second;
        ++it;
    }
    return -1;
}

bool CSharedServerMessageManager::CSharedServerMessageAdapter::IsOnIndex(int nIndex)
{
    std::vector<int>::iterator it = m_pInfo->m_vecIndex.begin();
    while (it != m_pInfo->m_vecIndex.end())
    {
        if (*it == nIndex)
            return true;
        ++it;
    }
    return false;
}

int CSharedServerMessageManager::CSharedServerMessageAdapter::GetShareKindWithIndex(int nIndex)
{
    int nMapped = GetMappedEventID(nIndex);
    if (nMapped == -1)
        return -1;

    std::vector<std::pair<int, int> >::iterator it = m_pInfo->m_vecShareKind.begin();
    while (it != m_pInfo->m_vecShareKind.end())
    {
        std::pair<int, int>* pPair = &(*it);
        if (pPair == 0)
        {
            ++it;
            continue;
        }
        if (pPair->first == nMapped)
            return pPair->second;
        ++it;
    }
    return -1;
}

// ----------------------------------------------------------------------------
// 构造 / 析构
// ----------------------------------------------------------------------------
CSharedServerMessageManager::CSharedServerMessageManager()
{
    m_isSet = 0;
    m_event.clear();
}

CSharedServerMessageManager::CSharedServerMessageManager(stSharedServerMessageInfo* pInfo)
{
    m_isSet = 0;
    m_adapter.SetServerMessageAddInfo(pInfo);
    m_event.clear();
}

CSharedServerMessageManager::~CSharedServerMessageManager()
{
}

// ----------------------------------------------------------------------------
// 公告收发
// ----------------------------------------------------------------------------
void CSharedServerMessageManager::SendServerMessageInfo(CUser* pUser,
                                                       SIG_EVENT_SERVER_MESSAGE* pMsg,
                                                       char bFlag)
{
    PacketGuard pg;
    pg.put_header(0, 0xc0);
    pg.put_byte((int)bFlag);

    EVENT_SERVER_MESSAGE ev;
    ev.m_count = 0;
    for (int i = 0; i < pMsg->m_count; ++i)
    {
        if (m_adapter.IsOnIndex(pMsg->m_msg[i].m_index))
        {
            ++ev.m_count;
            ev.m_vecMsg.push_back(pMsg->m_msg[i]);
        }
    }

    if (ev.m_count <= 0x32)
    {
        pg.put_byte((int)ev.m_count);

        // ORIG 循环条件为 i<count || it!=end（两者同步推进，等价于 count 次）
        std::vector<stSERVER_MESSAGE>::iterator it = ev.m_vecMsg.begin();
        for (int i = 0; i < (int)ev.m_count || it != ev.m_vecMsg.end(); ++i)
        {
            stSERVER_MESSAGE* p = &(*it);
            pg.put_byte((int)p->m_index);
            pg.put_int(p->m_lenMsg);
            pg.put_str(p->m_msg, p->m_lenMsg);
            pg.put_int(p->m_lenMsg2);
            pg.put_str(p->m_msg2, p->m_lenMsg2);
            ++it;
        }

        pg.finalize(true);
        pUser->Send(pg);
    }
}

void CSharedServerMessageManager::SetServerMessage(SIG_EVENT_SERVER_MESSAGE* pMsg)
{
    if (m_isSet != 1)
    {
        if (pMsg->m_count == 0)
            return;

        m_event.m_count = pMsg->m_count;
        for (int i = 0; i < pMsg->m_count; ++i)
        {
            if (m_adapter.IsOnIndex(pMsg->m_msg[i].m_index))
                m_event.m_vecMsg.push_back(pMsg->m_msg[i]);
        }
        m_event.m_count = (unsigned char)m_event.m_vecMsg.size();
    }
    setLoadingComplete();
}

stSERVER_MESSAGE* CSharedServerMessageManager::getCurMessage(int nIndex)
{
    std::vector<stSERVER_MESSAGE>::iterator it = m_event.m_vecMsg.begin();
    while (it != m_event.m_vecMsg.end())
    {
        if (it->m_index == nIndex)
            return &(*it);
        ++it;
    }
    return 0;
}

bool CSharedServerMessageManager::IsChangableMessage(int nIndex, int nTime)
{
    stSERVER_MESSAGE* pMsg = getCurMessage(nIndex);
    if (pMsg == 0)
        return true;
    if (pMsg->m_endTime + 0x78 < nTime)
        return true;
    return false;
}

void CSharedServerMessageManager::DeleteSpecificMessage(int nIndex)
{
    std::vector<stSERVER_MESSAGE>::iterator it = m_event.m_vecMsg.begin();
    while (it != m_event.m_vecMsg.end())
    {
        if (it->m_index == nIndex)
        {
            m_event.m_vecMsg.erase(it);
            --m_event.m_count;
            return;
        }
        ++it;
    }
}

bool CSharedServerMessageManager::CheckUserCondition(CUser* pUser, int nIndex, int& nOut)
{
    int nMapped = m_adapter.GetMappedEventID(nIndex);
    if (nMapped == -1)
        return false;

    int nSlot = 0;
    switch (nMapped)
    {
    case 1:
        break;
    case 0:
    case 3:
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x28d3aa);
        if (nSlot == -1)
            return false;
        break;
    case 2:
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x1d9d);
        if (nSlot == -1)
            return false;
        break;
    default:
        return false;
    }
    return true;
}

// ----------------------------------------------------------------------------
// 公告更新（含物品条件校验 + 广播到 DB 队列）
// ----------------------------------------------------------------------------
void CSharedServerMessageManager::UpdateServerMessage(CUser* pUser, int nIndex,
                                                      char* pMsg, char* pMsg2, int nEndTime)
{
    stSERVER_MESSAGE* pCur = getCurMessage(nIndex);
    if (pCur == 0)
    {
        if (!m_adapter.IsOnIndex(nIndex))
        {
            cMyTrace tr("void CSharedServerMessageManager::UpdateServerMessage(CUser*, int, char*, char*, int)",
                        0x126, 0);
            tr("SERVER_MESSAGE : NOT EXIST INDEX and NOT EXIST DATA");
            return;
        }

        int nKind = m_adapter.GetShareKindWithIndex(nIndex);
        if (nKind == -1)
            return;

        stSERVER_MESSAGE local;
        local.m_kind = (unsigned char)nKind;
        local.m_index = (unsigned char)nIndex;
        local.m_lenMsg2 = strlen(pMsg2);
        strncpy(local.m_msg2, pMsg2, local.m_lenMsg2 + 1);
        local.m_lenMsg = strlen(pMsg);
        strncpy(local.m_msg, pMsg, local.m_lenMsg + 1);
        local.m_endTime = nEndTime;

        m_event.m_vecMsg.push_back(local);
        ++m_event.m_count;
        pCur = &local;
    }
    else
    {
        pCur->m_lenMsg = strlen(pMsg);
        strncpy(pCur->m_msg, pMsg, pCur->m_lenMsg + 1);
        pCur->m_lenMsg2 = strlen(pMsg2);
        strncpy(pCur->m_msg2, pMsg2, pCur->m_lenMsg2 + 1);
        pCur->m_endTime = nEndTime;
    }

    if (!IsSetServerMessage())
        setLoadingComplete();

    int nMapped = m_adapter.GetMappedEventID(pCur->m_index);
    int nSlot = 0;
    if (nMapped == 2)
    {
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x1d9d);
        if (nSlot == -1)
            return;
        if (!pUser->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, nSlot, 1,
                                                      eItemDelReason_3, true))
            return;
        if (!pUser->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_INVEN, nSlot))
            return;
    }
    else if (nMapped == 3)
    {
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x28d3aa);
        if (nSlot == -1)
            return;
        if (!pUser->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, nSlot, 1,
                                                      eItemDelReason_3, true))
            return;
        if (!pUser->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_INVEN, nSlot))
            return;
    }
    else if (nMapped == 0)
    {
        nSlot = pUser->getCurCharacInvenR()->check_item_exist(0x28d3aa);
        if (nSlot == -1)
            return;
        if (!pUser->getCurCharacInvenW()->delete_item(INVEN_TYPE_EQUIP, nSlot, 1,
                                                      eItemDelReason_0x1d, true))
            return;
        if (!pUser->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_INVEN, nSlot))
            return;
    }

    Stream* pStream = GlobalData::s_stream_pool.Acquire("SharedServerMessageManager.cpp", 0x16c);
    CStreamGuard guard(pStream, true);
    **guard << 0x16c;
    unsigned int nUID = pUser->GetUID();
    **guard << (int)nUID;

    SIG_UPDATE_SERVER_MESSAGE* pOut = guard->GetInBuffer<SIG_UPDATE_SERVER_MESSAGE>();
    pOut->m_index = pCur->m_index;
    pOut->m_server = G_CEnvironment()->m_field378;
    if (pCur->m_kind == 0)
        pOut->m_channel = G_CEnvironment()->get_channel_no();
    else if (pCur->m_kind == 1)
        pOut->m_channel = 0;
    pOut->m_kind = pCur->m_kind;
    pOut->m_lenMsg2 = pCur->m_lenMsg2;
    pOut->m_lenMsg = pCur->m_lenMsg;
    strncpy(pOut->m_msg, pCur->m_msg, pOut->m_lenMsg + 1);
    strncpy(pOut->m_msg2, pCur->m_msg2, pOut->m_lenMsg2 + 1);
    pOut->m_endTime = pCur->m_endTime;

    GlobalData::s_msgq_mgr.put(MsgQueueMgr::DB_Q, guard);
}

void CSharedServerMessageManager::UpdateServerMessageByBroadcast(int nIndex,
                                                                 char* pMsg, char* pMsg2,
                                                                 int nEndTime)
{
    stSERVER_MESSAGE* pCur = getCurMessage(nIndex);
    if (pCur == 0)
    {
        if (!m_adapter.IsOnIndex(nIndex))
        {
            cMyTrace tr("void CSharedServerMessageManager::UpdateServerMessageByBroadcast(int, char*, char*, int)",
                        0x1c6, 0);
            tr("SERVER_MESSAGE : NOT EXIST INDEX and NOT EXIST DATA");
            return;
        }

        int nKind = m_adapter.GetShareKindWithIndex(nIndex);
        if (nKind == -1)
            return;

        stSERVER_MESSAGE local;
        local.m_kind = (unsigned char)nKind;
        local.m_index = (unsigned char)nIndex;
        local.m_lenMsg2 = strlen(pMsg2);
        strncpy(local.m_msg2, pMsg2, local.m_lenMsg2 + 1);
        local.m_lenMsg = strlen(pMsg);
        strncpy(local.m_msg, pMsg, local.m_lenMsg + 1);
        local.m_endTime = nEndTime;

        m_event.m_vecMsg.push_back(local);
        ++m_event.m_count;
        pCur = &local;
    }
    else
    {
        pCur->m_lenMsg = strlen(pMsg);
        strncpy(pCur->m_msg, pMsg, pCur->m_lenMsg + 1);
        pCur->m_lenMsg2 = strlen(pMsg2);
        strncpy(pCur->m_msg2, pMsg2, pCur->m_lenMsg2 + 1);
        pCur->m_endTime = nEndTime;
    }

    if (!IsSetServerMessage())
        setLoadingComplete();
}

void CSharedServerMessageManager::LoadServerMessageFromDB()
{
    Stream* pStream = GlobalData::s_stream_pool.Acquire("SharedServerMessageManager.cpp", 0x21b);
    CStreamGuard guard(pStream, true);
    **guard << 0x16d;
    **guard << -1;

    SIG_SELECT_SERVER_MESSAGE* pOut = guard->GetInBuffer<SIG_SELECT_SERVER_MESSAGE>();
    pOut->m_server = (short)G_CEnvironment()->m_field378;
    pOut->m_channel = (short)G_CEnvironment()->get_channel_no();

    GlobalData::s_msgq_mgr.put(MsgQueueMgr::DB_Q, guard);
}
