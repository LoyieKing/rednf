#ifndef MONITOR_DNFPACKETBUFFER_H_
#define MONITOR_DNFPACKETBUFFER_H_
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include "PacketHeader.h"
#include "Thread.h"
#include "tinyxml.h"

int getErrno();

class BuddyList;
struct CACHE_CHARACTER_TYPE;
class CAppBase;
class CAppConfig;
class CAppInit;
class CAppLoadChecker;
class CAppStartInit;
class CAppStopInit;
class CApplication;
class CBaseEventAction;
class CBlackUser;
class CBuddy;
class CBuddyHandle;
class CBuddyRegisterManager;
class CCacheCharacterMgr;
struct CCacheCharacterTime;
class CCashObject;
class CDBServer;
class CDNFException;
class CDNFProhibitUser;
class CEventActionManager;
class CExchangeServer;
class CFloatingPointExceptSig;
class CFrameCountHandler;
class CGMAccounts;
class CGameServer;
class CIPCounter;
class CInitAccusationList;
class CInitAccusationListMgr;
class CInnerMsgHandler;
class CItemLimitEdition;
class CItemLimitEditionMgr;
class CKillUSRConfig;
class CLoginLogoutStatistics;
class CManagerServer;
class CMember;
class CMemberConfig;
class CMemberExpTbl;
class CMemberManager;
class CMemoryCashManager;
class CNullEventAction;
class COnTimeEventAction;
class COnTimeEventIdxLoad;
class COnTimeEventManager;
class COnTimeEventRewardEndTrigger;
class COnTimeEventRewardStartTrigger;
class CPacketBuffer;
template<int A, int B> class CPacketCounter;
class CPacketDecoder;
class CPacketTracer;
class CPacketTranslater;
class CPeer;
class CPeriodicMessageMgr;
class CProtocol;
class CRewardUserList;
class CScheduler;
class CSegmentationFaultSig;
class CServerConfig;
class CServerHandler;
class CServerInterface;
class CServerXml;
class CSignal;
class CSignalTranslator;
class CSourceVersionMgr;
template<class T, int N> class CSwapQueue;
class CSystemFailSig;
class CSystemTime;
class CSystemTimeHandler;
class CTableBase;
class CTaskScheduler;
class CTask_ChristmasEvent;
class CTcpAcceptThread;
class CTcpDBServer;
class CTcpGameServer;
class CTcpHandler;
class CTcpManagerServer;
class CTcpNetSystem;
class CTcpNetworkThread;
class CTcpRecvBuffer;
class CTcpSendBuffer;
class CTerminateSig;
class CThreadInterface;
class CTowerRank;
class CUdpHandler;
class CUdpNetworkThread;
class CUdpRecvBuffer;
class CUser;
class CUser1Sig;
class CUser2Sig;
class CUserManager;
class CVersionMgr;
class CVillageAttackedCountdownFirst;
class CVillageAttackedCountdownSecond;
class CVillageAttackedCountdownThird;
class CVillageAttackedEnd;
class CVillageAttackedManager;
class CVillageAttackedReward;
class CVillageAttackedServerGroupReward;
class CVillageAttackedStart;
class CollectItms;
class CommonTime;
class EndEffectTask;
class EpollHandler;
class EventAction;
class EventManager;
struct EventParam;
template<class T> class IQueue;
class LimitNpcBuyItemChangeInfo;
struct LimitNpcBuyItemInfo;
struct LimitNpcBuyItemInfoAll;
class LimitNpcBuyItemManager;
class LimitNpcBuyItemRequestInfo;
struct LimitNpcBuyItemUpdate;
template<class T> class MemPool;
struct NpcBuyLimitItem;
class Packet_Arad_ApplyEffect;
class Packet_Arad_DeleteEffect;
class Packet_Change_User_Handicap;
class Packet_CollectItemsResult;
class Packet_CollectItemsReward;
class Packet_CollectItemsRewardBroadcast;
class Packet_CollectItemsUpdate;
class Packet_DBMW_Add_Buddy;
class Packet_DBMW_Connection_Check;
class Packet_DBMW_Del_Buddy;
class Packet_DBMW_Query_Buddy_Info;
class Packet_DBMW_Query_Msg;
class Packet_DBMW_Register_To_BlackList;
class Packet_DBMW_Request_BlackList;
class Packet_DBMW_Statistic_Login_Logout;
class Packet_DB_InsertMail;
class Packet_DB_Member_Delete_As_Charac_Delete;
class Packet_DB_Query_Member;
class Packet_DB_Query_Member_Member;
class Packet_DB_Save_Member_Exp;
class Packet_DB_VillageAttackedRank;
class Packet_DMBW_Delete_To_BlackList;
class Packet_Delete_To_BlackList_Result;
class Packet_InnerPakcet_Login;
class Packet_InnerPakcet_Logout;
class Packet_Item_Limit_Edition_Load_Data_Req;
class Packet_Item_Limit_Edition_Sell_end;
class Packet_Item_Limit_Edition_Update;
class Packet_Load_Periodic_Message;
class Packet_MTG_OntimeEvent_RewardEnd;
class Packet_MTG_OntimeEvent_RewardStart;
class Packet_Manager_Event_Trigger_Ack;
class Packet_MiniCraneSeed;
class Packet_Monitor_Add_Buddy_Reply;
class Packet_Monitor_Call_Member_List_ToUser;
class Packet_Monitor_Del_Buddy_Reply;
class Packet_Monitor_Event_End;
class Packet_Monitor_Event_Start;
class Packet_Monitor_Member_Chat_ToUser;
class Packet_Monitor_Member_Chat_ToUser_Hyper_Link;
class Packet_Monitor_Member_Enter_Reply_ToResponser;
class Packet_Monitor_Member_Pay_Tax_ToUpper;
class Packet_Monitor_Member_Secede_To_Seceder;
class Packet_Monitor_Notice_Black_List;
class Packet_Monitor_Notice_Buddy_In_Out;
class Packet_Monitor_Notice_Delete_Member_Id;
class Packet_Monitor_Notice_MemberExp_LevelUp;
class Packet_Monitor_Notice_Member_Enter_Ok;
class Packet_Monitor_Notice_Member_Member_Login_out;
class Packet_Monitor_Other_Channel_Chat_ToUser;
class Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link;
class Packet_Monitor_Reply_Buddy_List;
class Packet_Monitor_Request_Member_Enter_To_Requester;
class Packet_Monitor_Request_Member_Enter_To_Responser;
class Packet_Monitor_SAVE_Member;
class Packet_Monitor_SAVE_Member_Update_Char_Info;
class Packet_Monitor_ServerEvent_Start;
class Packet_Monitor_Take_Screen_Shot;
class Packet_Monitor_UDP_HeartBeat;
class Packet_Monitor_User_Repel;
class Packet_Notice_Find_Factory_Hub_User;
class Packet_Punish_Cancel;
class Packet_Register_To_BlackList_RESULT;
class Packet_Req_Ontime_Event_Idx;
class Packet_Req_Ontime_Event_Item;
class Packet_Request_Charac_Tower_Ranking;
class Packet_Request_IPCounterList;
class Packet_Request_Load_Tower_Full_Rank;
class Packet_Request_Result_BlackList;
class Packet_SecuService_Connect_Web;
class Packet_Send_Periodic_Message;
class Packet_Send_Time_Sync;
class Packet_Send_Time_Sync_For_Login;
class Packet_Server_Queue_Load_Statistic;
class Packet_Set_CleanPad_Point;
class Packet_TOD_DoRandomSelect;
class Packet_Tcp_Server_Connect;
class Packet_VillageAttackedCountdown;
class Packet_VillageAttackedEnd;
class Packet_VillageAttackedReward;
class Packet_VillageAttackedRewardServer;
class Packet_VillageAttackedScore;
class Packet_VillageAttackedStart;
class Packet_VillageAttackedUpdate;
class Packet_Web_Notice_InGame_Advertisement;
class Packet_Web_Request_ARS_Info;
struct STBlackUserDBType;
struct STBuddyDBInfo;
struct STMemberDBInfo;
struct STMemberListInfo;
struct STPvPChannelInfo;
struct ST_KillUSRConfig;
struct ST_MemberConfig;
struct ST_MemberInfo;
struct ST_MemberProxy;
class ST_ServerInfo;
class StartEffectTask;
class TCPSocket;
struct TimeGateRewardType;
class TowerOfDespairReloadAPC_Task;
class TowerOfDespairWaitGameServerResponse_Task;
struct hyperlink_item_info;
struct stDeathTowerRecordMemberInfo_t;
struct stHuntingPoint;
struct stItemLimitEditionItemInfo_t;
struct stLoginLogout;
struct stLoginLogoutVariable;
struct stServerInfo;
struct stTowerRankElement_t;
struct stUserHuntingPoint;

class CUdpRecvBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    char m_data[0x1804];
};

class CTcpRecvBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    char m_data[0x1804];
};

class CTcpSendBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    char m_data[0x1804];
};

class CPacketBuffer
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* ptr);
    static void operator delete(void* ptr, unsigned int size);
    char m_data[0x1804];
};

template<class T, int N>
class CSwapQueue
{
public:
    CSwapQueue();
    ~CSwapQueue();
    void Push(T* item);
    void Pop();
    void Init();
    T* GetRecvQ();
    T* GetParseQ();
    void SwapQ();
    int m_recvIdx;   // +0
    int m_parseIdx;  // +4
    T m_queues[N];   // +8
};

template<class T>
class IQueue
{
public:
    IQueue();
    ~IQueue();
    static IQueue<T>* Get();
    void InitQueue(T* recv, T* parse);
    bool SwitchQueue();
    T* GetParseQueue();
    T* m_recv;   // +0
    T* m_parse;  // +4
};

template<class T>
class MemPool
{
public:
    MemPool();
    MemPool(unsigned int count);
    ~MemPool();
    void* alloc();
    void free(void* ptr, unsigned int size);
    void free(void* ptr);
    static void* headOfFreeList_;
    unsigned int m_size;            // +0
    unsigned int m_count;           // +4
    std::vector<void*> m_blocks;    // +8
};

// ---- template impls: CSwapQueue/IQueue impls ----
// ---- CSwapQueue / IQueue 模板桩（后续从 guild 复用完整实现）----
template<class T, int N>
CSwapQueue<T, N>::CSwapQueue()
{
    Init();
}
template<class T, int N>
CSwapQueue<T, N>::~CSwapQueue() {}
template<class T, int N>
void CSwapQueue<T, N>::Push(T* item)
{
    GetRecvQ()->push(item);
}
template<class T, int N>
void CSwapQueue<T, N>::Pop()
{
    GetRecvQ()->pop();
}
template<class T, int N>
void CSwapQueue<T, N>::Init()
{
    m_recvIdx = 0;
    m_parseIdx = 1;
}
template<class T, int N>
T* CSwapQueue<T, N>::GetRecvQ()
{
    return &m_queues[m_recvIdx];
}
template<class T, int N>
T* CSwapQueue<T, N>::GetParseQ()
{
    return &m_queues[m_parseIdx];
}
template<class T, int N>
void CSwapQueue<T, N>::SwapQ()
{
    int t = m_recvIdx;
    m_recvIdx = m_parseIdx;
    m_parseIdx = t;
}

template<class T>
IQueue<T>::IQueue() {}
template<class T>
IQueue<T>::~IQueue() {}
template<class T>
IQueue<T>* IQueue<T>::Get()
{
    static IQueue<T> instance;
    return &instance;
}
template<class T>
void IQueue<T>::InitQueue(T* recv, T* parse)
{
    m_recv = recv;
    m_parse = parse;
}
template<class T>
bool IQueue<T>::SwitchQueue()
{
    if (m_recv->empty())
    {
        return false;
    }
    T* t = m_recv;
    m_recv = m_parse;
    m_parse = t;
    return true;
}
template<class T>
T* IQueue<T>::GetParseQueue() { return m_parse; }

#endif  // MONITOR_DNFPACKETBUFFER_H_
