#ifndef MONITOR_DNFMEMBER_H_
#define MONITOR_DNFMEMBER_H_
#include <deque>
#include "RawAccess.h"
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include "PacketHeader.h"
#include "Thread.h"
#include "tinyxml.h"

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

struct ST_MemberProxy
{
    ST_MemberProxy()
    {
        m_field0 = 0;
        m_flag4 = 0;
        m_field23 = 0;
        memset(m_name, 0, 0x1e);
    }
    void Reset();
    unsigned int m_field0;    // +0
    unsigned char m_flag4;    // +4
    char m_name[0x1e];        // +5
    unsigned int m_field23;   // +0x23
} __attribute__((packed));

struct STMemberDBInfo
{
    STMemberDBInfo();
    ST_MemberProxy m_member;    // +0
    unsigned char m_count27;    // +0x27
    ST_MemberProxy m_lowers[10];// +0x28
};

struct ST_MemberInfo
{
    ST_MemberInfo();
    unsigned char m_field0;    // +0
    unsigned char m_field1;    // +1
    char m_name[0x1e];         // +2
    unsigned char m_field20;   // +0x20
    unsigned char m_field21;   // +0x21
    unsigned int m_field22;    // +0x22
    unsigned int m_field26;    // +0x26
} __attribute__((packed));

struct STMemberListInfo
{
    STMemberListInfo();
    ST_MemberInfo m_info;       // +0
    unsigned char m_count;      // +0x2a
    ST_MemberInfo m_members[10]; // +0x2b
} __attribute__((packed));

struct STBuddyDBInfo
{
    char m_name[0x1e];        // +0
    short m_lev;              // +0x1e
    char m_job;               // +0x20
    char m_growType;          // +0x21
    unsigned int m_characNo;  // +0x22
    char m_sex;               // +0x26
} __attribute__((packed));

class CMember
{
public:
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    CMember(unsigned int key, CMemberManager* mgr);
    ~CMember();
    void QueryMember(CServerHandler* handler);
    unsigned int* GetMemberDBInfoW();
    void NoticeMemberLogin_Out(CUser* user, char flag);
    void CheckMemberRegisterFlag();
    void NoticeChatMsgToMemberMembersHyperLink(char* msg, int len, unsigned char count,
                                               const hyperlink_item_info* items, CUser* user);
    void NoticeChatMsgToMemberMembers(char* msg, int len, CUser* user);
    bool IsThereUpper() const { return m_dbInfo.m_member.m_field0 != 0; }
    int GetUpperMember_CharId() const;
    int FindLowerMember(unsigned int charNo) const;
    unsigned int GetLowerMemberCount() const;
    unsigned int* GetLowerMember_Proxy() const;
    int IncConnUpperMemberExp(unsigned int maxExp);
    void IncConnLowerMemberExp(unsigned int uCharNo, unsigned int maxExp);
    int IncConnLowerMemberExp(int index, unsigned int uCharNo, unsigned int maxExp);
    void NoticeLevelUpToLowers(unsigned int level);
    void LoadMember(STMemberDBInfo& info, short level, unsigned int a, unsigned int b);
    int DeleteMemberByName(char* name, unsigned int& outKey);
    unsigned int* GetUpperMember_Proxy();
    unsigned int* GetUpperMember_Proxy() const;
    void SetMemberDeleteTime(unsigned int t);
    void DebugPrintMemberMember(char* out);
    char IsEmpty();
    char CheckDailyScheduleTimeOver(int day, long time);
    char CheckDayHourScheduleTimeOver(int day, int hour, long time);
    void SetMemberRegisterFlag(bool flag);
    bool IsAbleToRegisterMember();
    unsigned int GetMemberKey();
    unsigned int GetUpperMemberExpLevel();
    int GetConnLowerMemberCnt();
    unsigned short GetMemberDBFlag();
    void SetMemberDBFlag(unsigned short flag);
    void SetMemberRegisterTime(unsigned int t);
    int InsertUpperMember(unsigned int charNo, unsigned char level, const char* name,
                          bool flag);
    int InsertLowerMember(unsigned int charNo, unsigned char level, const char* name,
                          bool flag);
    char IsAlreadyMemberMember(unsigned int charNo) const;
    void DeleteUpperMember(unsigned int charNo, bool flag);
    void DeleteLowerMember(unsigned int charNo, bool flag);
    unsigned char* GetMemberDBInfo() const;
    unsigned int m_key;              // +0
    unsigned short m_flag;          // +4
    STMemberDBInfo m_dbInfo;        // +6
    CMemberManager* m_memberManager;// +0x1b4
    bool m_state1b8;                // +0x1b8
    char m_pad1b9[3];               // +0x1b9
    unsigned int m_registerTime;    // +0x1bc
    unsigned int m_dayHourTime;     // +0x1c0
    char m_pad1c4[4];               // +0x1c4
};

#endif  // MONITOR_DNFMEMBER_H_
