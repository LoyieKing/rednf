#ifndef MONITOR_VILLAGEATTACKEDMANAGER_H_
#define MONITOR_VILLAGEATTACKEDMANAGER_H_
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
#include "TaskScheduler.h"

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

// ---- 附属声明 ----
int GetNextSchedule(tm t, int wday, int hour, int min);
int GetPrevSchedule(tm t, int wday, int hour, int min);
bool CheckDailyScheduleTimeOver(int day, long time);
bool CheckDayHourScheduleTimeOver(int day, int hour, long time);
bool compareTime(int const& a, int const& b);

enum ENUM_VILLAGE_ATTACKED_REWARD
{
    ENUM_VILLAGE_ATTACKED_REWARD_NONE = 0,
    ENUM_VILLAGE_ATTACKED_REWARD_BUFF = 1,
    ENUM_VILLAGE_ATTACKED_REWARD_PENALTY = 2
};

struct stUserHuntingPoint
{
    unsigned int m_huntingPoint;  // +0
    unsigned int m_characNo;      // +4
    bool operator<(const stUserHuntingPoint& other) const
    {
        return m_huntingPoint < other.m_huntingPoint;
    }
};

namespace village_attacked
{
int GetNextSchedule(tm t, int wday, int hour, int min);
void SetRealConfig();
void SetGMConfig(unsigned int a, unsigned int b, unsigned int c);

struct stHuntingPoint
{
    stHuntingPoint();
    unsigned int m_huntingPoint;  // +0
    unsigned int m_field4;        // +4
};

class CVillageAttackedManager
{
public:
    CVillageAttackedManager(CApplication* app);
    ~CVillageAttackedManager();
    void SendCharacRank();
    void SendFirstRankerRewardJpn(CUser* user, int rank);
    void OnSchedule();
    void InsertTimer(int startTime, int endTime);
    void OnEndVillageAttacked();
    void OnRewardVillageAttacked();
    void SendVillageAttackedEnd();
    void OnUpdateVillageAttacked();
    void OnCharacLogin(CUser* user);
    void SendVillageAttackedScore(CUser* user);
    void SendVillageAttackedReward(CUser* user, int rewardType);
    int* GetHuntingPoint(unsigned int charNo);
    void UpdateHuntingPoint(CUser** users, bool success, int* a, unsigned int* charNos);
    void SendVillageAttackedRewardJpn(CUser* user, int count);
    void SendMinTime();
    unsigned int GetElapseTime();
    void ClearDungeonCloseTime();
    int GetRemainTime();
    unsigned int GetDungeonRemainTime();
    int GetMaxHuntingPoint();
    void OnStartVillageAttacked();
    void OnCountdownVillageAttacked(int time);
    void SetRewardCloseTime(ENUM_VILLAGE_ATTACKED_REWARD rewardType);
    void RequestEventEnd(bool flag);
    void SendFirstRankerReward(unsigned int charNo);
    void SendMaxHuntingPoint();
    void OnServerGroupRewardVillageAttacked();
    void RequestEventStart(int time);
    void RequestEventPenaltyEnd();
    void SendRequestRevengeDungeon(char* pkt);
    void ProcessByMinute();
    void Reset();
    CApplication* m_app;                       // +0
    std::map<unsigned int, stHuntingPoint> m_huntingPoints;  // +4
    unsigned int m_field1c;                    // +0x1c
    unsigned int m_field20;                    // +0x20
    unsigned char m_state24;                   // +0x24
    char m_pad25[3];                           // +0x25
    int m_field28;                             // +0x28
    int m_field2c;                             // +0x2c
    int m_field30;                             // +0x30
    int m_field34;                             // +0x34
};

class CVillageAttackedCountdownFirst : public CTaskScheduler::CTask
{
public:
    CVillageAttackedCountdownFirst(unsigned int time, unsigned int flag, CVillageAttackedManager* mgr);
    ~CVillageAttackedCountdownFirst();
    void _DoExecute();
    CVillageAttackedManager* m_mgr;  // +0x10
};

class CVillageAttackedCountdownSecond : public CTaskScheduler::CTask
{
public:
    CVillageAttackedCountdownSecond(unsigned int time, unsigned int flag, CVillageAttackedManager* mgr);
    ~CVillageAttackedCountdownSecond();
    void _DoExecute();
    CVillageAttackedManager* m_mgr;  // +0x10
};

class CVillageAttackedCountdownThird : public CTaskScheduler::CTask
{
public:
    CVillageAttackedCountdownThird(unsigned int time, unsigned int flag, CVillageAttackedManager* mgr);
    ~CVillageAttackedCountdownThird();
    void _DoExecute();
    CVillageAttackedManager* m_mgr;  // +0x10
};

class CVillageAttackedStart : public CTaskScheduler::CTask
{
public:
    CVillageAttackedStart(unsigned int time, unsigned int flag, CVillageAttackedManager* mgr);
    ~CVillageAttackedStart();
    void _DoExecute();
    CVillageAttackedManager* m_mgr;  // +0x10
};

class CVillageAttackedEnd : public CTaskScheduler::CTask
{
public:
    CVillageAttackedEnd(unsigned int time, unsigned int flag, CVillageAttackedManager* mgr);
    ~CVillageAttackedEnd();
    void _DoExecute();
    CVillageAttackedManager* m_mgr;  // +0x10
};

class CVillageAttackedReward : public CTaskScheduler::CTask
{
public:
    CVillageAttackedReward(unsigned int time, unsigned int flag, CVillageAttackedManager* mgr);
    ~CVillageAttackedReward();
    void _DoExecute();
    CVillageAttackedManager* m_mgr;  // +0x10
};

class CVillageAttackedServerGroupReward : public CTaskScheduler::CTask
{
public:
    CVillageAttackedServerGroupReward(unsigned int time, unsigned int flag,
                                      CVillageAttackedManager* mgr);
    ~CVillageAttackedServerGroupReward();
    void _DoExecute();
    CVillageAttackedManager* m_mgr;  // +0x10
};

}

#endif  // MONITOR_VILLAGEATTACKEDMANAGER_H_
