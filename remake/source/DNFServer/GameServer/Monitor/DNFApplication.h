#ifndef MONITOR_DNFAPPLICATION_H_
#define MONITOR_DNFAPPLICATION_H_
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
#include "BuddyRegisterManager.h"
#include "DNFMemberManager.h"
#include "DNFPacketBuffer.h"
#include "DNFTickHandler.h"
#include "DNFUserManager.h"
#include "TaskScheduler.h"
#include "TcpNetSystem.h"

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

typedef std::queue<CUdpRecvBuffer*, std::deque<CUdpRecvBuffer*, std::allocator<CUdpRecvBuffer*> > > UdpRecvQueue;
typedef std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*, std::allocator<CTcpRecvBuffer*> > > TcpRecvQueue;

namespace WongWork
{
class CGMAccounts;
}

CAppBase* CApplicationInstance();
void ShowLogo();
unsigned int get_rand_int(int n);

class CPeriodicMessageMgr
{
public:
    CPeriodicMessageMgr();
    ~CPeriodicMessageMgr();
    void OnProcess(CServerHandler* handler);
    void OnTimerSendData(CServerHandler* handler);
    void SetMessageData(char* msg, int startHour, int endHour);
    void init();
    char m_msg[0x200];       // +0
    int m_startHour;         // +0x200
    int m_endHour;           // +0x204
};

class CollectItms
{
public:
    CollectItms();
    ~CollectItms();
    char m_data[0x10];
};

namespace init_accusation
{
class CInitAccusationListMgr
{
public:
    CInitAccusationListMgr(CApplication& app);
    ~CInitAccusationListMgr();
    bool setSchedule(bool const& flag);
    CApplication* getApp() const;
    char m_data[4];
};

class CInitAccusationList : public CTaskScheduler::CTask
{
public:
    CInitAccusationList(unsigned int time, unsigned int flag, CInitAccusationListMgr* mgr);
    ~CInitAccusationList();
    void _DoExecute();
    CInitAccusationListMgr* m_mgr;  // +0x10
};

}

class CAppBase
{
public:
    CAppBase();
    virtual ~CAppBase();
    virtual void Process();
    virtual void Init(int argc, char** argv) = 0;
    virtual void Load(int argc, char** argv) = 0;
    virtual void Free() = 0;
    void Create(int argc, char** argv);
    void Clear();
};

class CApplication : public CAppBase
{
public:
    CApplication();
    virtual ~CApplication();
    virtual void Process();
    virtual void Init(int argc, char** argv);
    virtual void Load(int argc, char** argv);
    virtual void Free();
    void CheckArgv(int argc, char** argv);
    void AttachAppInitor(char** argv);
    void ProcessTimeSync();
    void UpdateCollectItems();
    void UpdateMiniCraneSeed();
    void SendMiniCraneRandomSeed();
    void SetMiniCraneRandomSeed();
    void SwitchQueueTCP();
    void SwitchQueueUDP();
    void SendTestPacket_1();
    void SendTestPacket_2();
    CItemLimitEditionMgr* getItemLimitEditionMgr();
    LimitNpcBuyItemManager* getLimitNpcBuyItemManager();
    CTcpNetSystem* Get_TcpNetSystem();
    void OnGameServerDown(CGameServer* server);
    CServerHandler* Get_ServerHandler();
    CUserManager* Get_UserManager();
    CMemberManager* Get_MemberManager();
    CBuddyRegisterManager* Get_BuddyRegisterManager();
    CTaskScheduler* GetTaskScheduler();
    COnTimeEventManager* GetOnTimeEventManager();
    void* Get_UdpPacketRecvQ();
    UdpRecvQueue* Get_UdpPacketParseQ();
    CUdpHandler* Get_UdpHandler();
    void* Get_UdpQLock();
    void* Get_UdpBLock();
    char Send_Term_Signal(const std::string& file);
    void Send_Suspend_Signal(const std::string& file);
    void App_Stop();
    void TranslateSignal();
    int AddAccusationCharac(const std::string& a, const std::string& b, int c, char type);
    void ClearAccusationList();
    void Add_GM_id(unsigned int id);
    void Call_DeleteMember(unsigned int key, CUser* user);
    void Call_ResetBlackList(unsigned int charNo);
    void Call_ResetBuddyList(unsigned int charNo);
    void Call_ResetUserMemberInfo(unsigned int key);
    unsigned char Get_ServerGroup();
    void* GetGMAccounts();
    void* getIPCounter();
    void* getTowerRank();
    int getMiniCraneSeed() const;
    void* getCollectItems();
    void* FindGameServer(int id);
    void* FindTcpGameServer(unsigned int id);
    void OnTcpGameServerDown(CTcpGameServer* tcpGameServer);
    void* Get_MemoryCashManager();
    void* GetLoginLogoutStatistics();
    void* GetPeriodicMessageManager();
    void Remove_GM_id(unsigned int id);
    char isGM_regFromChannel(unsigned int channel);
    char isAbleUserChatWithGM(unsigned int channel, unsigned int charNo);
    void AddChattableUserWithGM(unsigned int channel, unsigned int charNo);
    void DisableChatUserWithGM(unsigned int channel, unsigned int charNo);

    bool m_loaded;                      // +4
    int m_reserved8;                    // +8
    CAppInit* m_appInit;                // +0xc
    CUserManager m_userManager;         // +0x10
    CAppConfig* m_appConfig;            // +0x8c
    void* m_field90;                    // +0x90
    CMemberConfig* m_memberConfig;      // +0x94
    CMemberExpTbl* m_memberExpTbl;      // +0x98
    CKillUSRConfig* m_serverHandler;    // +0x9c
    CServerHandler* m_serverHandler2;   // +0xa0
    CFrameCountHandler m_frameCount;    // +0xa4
    CInnerMsgHandler* m_innerMsgHandler;  // +0xd4
    CSwapQueue<UdpRecvQueue, 2> m_udpSwapQueue;  // +0xd8
    CUdpHandler* m_udpHandler;          // +0x130
    CUdpNetworkThread* m_udpThread;     // +0x134
    CMutex m_udpQLock;                  // +0x138
    CMutex m_udpBLock;                  // +0x150
    CTcpNetSystem m_tcpNetSystem;       // +0x168
    CTaskScheduler* m_taskScheduler;    // +0x2c8
    WongWork::CGMAccounts* m_field2cc;  // +0x2cc
    CMemberManager m_memberManager;     // +0x2d0
    CBuddyRegisterManager m_buddyMgr;   // +0x300
    CMemoryCashManager* m_memoryCash;   // +0x318
    void* m_field31c;                   // +0x31c
    class COnTimeEventManager* m_onTimeEventMgr;  // +0x320
    CTowerRank* m_towerRank;            // +0x324
    CItemLimitEditionMgr* m_itemLimitMgr;  // +0x328
    CIPCounter* m_ipCounter;            // +0x32c
    CLoginLogoutStatistics* m_field330; // +0x330
    void* m_field334;                   // +0x334
    std::set<std::pair<const std::string, int> > m_set338;  // +0x338
    std::map<const std::string, int> m_map350;        // +0x350
    std::map<unsigned int, std::list<unsigned int> > m_map368;  // +0x368
    CPeriodicMessageMgr* m_periodicMsg; // +0x380
    LimitNpcBuyItemManager* m_limitNpc; // +0x384
    void* m_field388;                   // +0x388
    int m_miniCraneSeed;                // +0x38c
    short m_timeSyncHour;               // +0x390
};

// ---- CApplicationInstance / ShowLogo（monitor 单例入口）----
CAppBase* CApplicationInstance();
void ShowLogo();

#endif  // MONITOR_DNFAPPLICATION_H_
