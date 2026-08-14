#ifndef MONITOR_DNFPACKETTRANSLATER_H_
#define MONITOR_DNFPACKETTRANSLATER_H_
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
#include "DNFMember.h"
#include "Arad_BirthdayEvent.h"
#include "BlackUser.h"
#include "LoginLogoutStatistics.h"

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
extern int village_attacked_scheduler[18];
extern int MAX_SCHEDULER_COUNT;
extern int HUNTING_POINT_WEIGTH_CONST;
extern int REWARD_BUFF_TIME;
extern int REWARD_PENALTY_TIME;
extern int COUNTDOWN_FIRST_TIME;
extern int COUNTDOWN_SECOND_TIME;
extern int COUNTDOWN_THIRD_TIME;

class CPacketTranslater
{
public:
    static void attach(CApplication* app);
    static void OnAddBuddy(PacketHeader* pkt);
    static void OnAddBuddyDBReply(PacketHeader* pkt);
    static void OnBroadcastMsg(PacketHeader* pkt);
    static void OnCallMemberList(PacketHeader* pkt);
    static void OnCeraUpdate(PacketHeader* pkt);
    static void OnChangeCharName(PacketHeader* pkt);
    static void OnChannelType(PacketHeader* pkt);
    static void OnCharLogin(PacketHeader* pkt);
    static void OnCharacterDelete(PacketHeader* pkt);
    static void OnCheckOverlappedAccusation(PacketHeader* pkt);
    static void OnDBMWDeleteToBlackList(PacketHeader* pkt);
    static void OnDBMWResisterToBlackList(PacketHeader* pkt);
    static void OnDBMWResponseBlackListOnLogin(PacketHeader* pkt);
    static void OnDelBuddy(PacketHeader* pkt);
    static void OnDelBuddyDBReply(PacketHeader* pkt);
    static void OnDeleteToBlackList(PacketHeader* pkt);
    static void OnDisableUserOneToOneChat_GM(PacketHeader* pkt);
    static void OnEventEnd(PacketHeader* pkt);
    static void OnEventItemUpdate(PacketHeader* pkt);
    static void OnEventStart(PacketHeader* pkt);
    static void OnExchangeServerInfo(PacketHeader* pkt);
    static void OnFindCharacName_useUID(PacketHeader* pkt);
    static void OnForbidChat(PacketHeader* pkt);
    static void OnGMRequestMid(PacketHeader* pkt);
    static void OnGameMonitorGMVillageAttacked(PacketHeader* pkt);
    static void OnGameServerRegist(PacketHeader* pkt);
    static void OnHeartBeat(PacketHeader* pkt);
    static void OnInnerPacketLogin(PacketHeader* pkt);
    static void OnInnerPacketLogout(PacketHeader* pkt);
    static void OnLoadPeriodicMessage(PacketHeader* pkt);
    static void OnLogin(PacketHeader* pkt);
    static void OnLogout(PacketHeader* pkt);
    static void OnLogoutComplete(PacketHeader* pkt);
    static void OnMemberEnterReply(PacketHeader* pkt);
    static void OnMemberSecede(PacketHeader* pkt);
    static void OnMonitorFindFactoryHubUser(PacketHeader* pkt);
    static void OnMonitorFullLevelBroadCast(PacketHeader* pkt);
    static void OnMonitorManagerConnectOK(PacketHeader* pkt);
    static void OnMonitorMegaPhoneMsg(PacketHeader* pkt);
    static void OnMonitorMegaPhoneMsgHyperLink(PacketHeader* pkt);
    static void OnMonitorPunishCancel(PacketHeader* pkt);
    static void OnMonitorSecuServiceConnWeb(PacketHeader* pkt);
    static void OnNoCache(PacketHeader* pkt);
    static void OnNoticeCharLiveOnTenMin(PacketHeader* pkt);
    static void OnNoticeMemberChatMsg(PacketHeader* pkt);
    static void OnNoticeMemberChatMsgHyperLink(PacketHeader* pkt);
    static void OnNoticeMessage(PacketHeader* pkt);
    static void OnNoticeOtherChannelChatMsg(PacketHeader* pkt);
    static void OnNoticeOtherChannelChatMsgHyperLink(PacketHeader* pkt);
    static void OnNoticeProhibitConnectUser(PacketHeader* pkt);
    static void OnNoticeSlang(PacketHeader* pkt);
    static void OnNotifyAuctionMail(PacketHeader* pkt);
    static void OnNotifyNewMail(PacketHeader* pkt);
    static void OnPayTaxToUpper(PacketHeader* pkt);
    static void OnPcRoomPlayTimeReward(PacketHeader* pkt);
    static void OnPvPChannelInfo(PacketHeader* pkt);
    static void OnPvPChannelUserCount(PacketHeader* pkt);
    static void OnQueryBuddyInfoDBReply(PacketHeader* pkt);
    static void OnRegisterEventIdx(PacketHeader* pkt);
    static void OnRegisterEventItem(PacketHeader* pkt);
    static void OnRegisterEventUserIdx(PacketHeader* pkt);
    static void OnRegisterGM_mid(PacketHeader* pkt);
    static void OnRegisterToBlackList(PacketHeader* pkt);
    static void OnRelayServerUserCheck(PacketHeader* pkt);
    static void OnRenew_GM_List(PacketHeader* pkt);
    static void OnReplyQueryMember(PacketHeader* pkt);
    static void OnReplyUserInfo(PacketHeader* pkt);
    static void OnRequestBlackList(PacketHeader* pkt);
    static void OnRequestMemberEnter(PacketHeader* pkt);
    static void OnRequestReloadPowerWarRanker(PacketHeader* pkt);
    static void OnResetTODAPCInfo(PacketHeader* pkt);
    static void OnResponseFullIPCounterList(PacketHeader* pkt);
    static void OnResponseIPCounterList(PacketHeader* pkt);
    static void OnResultLoadPeriodicMessage(PacketHeader* pkt);
    static void OnResultRegisterEventIdx(PacketHeader* pkt);
    static void OnServerMessageInfo(PacketHeader* pkt);
    static void OnSetARSInfo(PacketHeader* pkt);
    static void OnSetCleanPadPoint(PacketHeader* pkt);
    static void OnTakeScreenShot(PacketHeader* pkt);
    static void OnUpdateChangableCharInfo(PacketHeader* pkt);
    static void OnUpdateMiniCraneSeed(PacketHeader* pkt);
    static void OnUserRepel(PacketHeader* pkt);
    static void OnUserRepelByCharName(PacketHeader* pkt);
    static void OnVillageAttackedGMCommand(PacketHeader* pkt);
    static void OnVillageAttackedRank(PacketHeader* pkt);
    static void OnVillageMonsterFightResult(PacketHeader* pkt);
    static void OnWebChangeUserHandicap(PacketHeader* pkt);
    static void OnWebEmergencyPatchMessage(PacketHeader* pkt);
    static void OnWebNoticeInGameAD(PacketHeader* pkt);
    static void OnWebNoticeSingle(PacketHeader* pkt);
    static void OnWebQueryUserState(PacketHeader* pkt);
    static void OnWebRequestARSInfo(PacketHeader* pkt);
    static void onCollectItems(PacketHeader* pkt);
    static void onCollectItemsGm(PacketHeader* pkt);
    static void onCollectItemsResult(PacketHeader* pkt);
    static void onEndGameEventFromServer(PacketHeader* pkt);
    static void onIPCounterControl(PacketHeader* pkt);
    static void onItemLimitEditionBuyableRequest(PacketHeader* pkt);
    static void onItemLimitEditionLoadDataReq(PacketHeader* pkt);
    static void onItemLimitEditionLoadDataRpy(PacketHeader* pkt);
    static void onItemLimitEditionSellEnd(PacketHeader* pkt);
    static void onLoadBlackIPMonitor(PacketHeader* pkt);
    static void onLoadBlackIPMonitorDeleteIP(PacketHeader* pkt);
    static void onLoadBlackIPMonitorPartLoad(PacketHeader* pkt);
    static void onLoadCleanPadPoint(PacketHeader* pkt);
    static void onLoadPunishUserReq(PacketHeader* pkt);
    static void onReloadCountryCode(PacketHeader* pkt);
    static void onReloadSecurityRestrictPolicy(PacketHeader* pkt);
    static void onReplyLoadTowerFullRank(PacketHeader* pkt);
    static void onRequestCharacInfoByCharacName(PacketHeader* pkt);
    static void onRequestCharacTowerUpdateRank(PacketHeader* pkt);
    static void onRequestReloadTowerRanker(PacketHeader* pkt);
    static void onSocialEventRewardItemInfo(PacketHeader* pkt);
    static void onSocialEventRewardItemInfoAll(PacketHeader* pkt);
    static void onSocialEventRewardItemRequest(PacketHeader* pkt);
    static void onSocialEventRewardItemResponse(PacketHeader* pkt);
    static void onSocialEventRewardItemUpdate(PacketHeader* pkt);
    static void onStartGameEventFromServer(PacketHeader* pkt);
    static void onWebReqReloadAutoPunishRule(PacketHeader* pkt);
    static void RequestBlackListToDBMW(unsigned int charNo);
    static void SendColletItemsReward(unsigned int charNo, int itemId, const char* itemName,
                                      int nameLen, TimeGateRewardType::T type);
    static void SendRequestMemberEnterResult(CUser* user, unsigned char result,
                                             const char* name);
    static void SendNoticeMemberEnterPacketOk(CUser* user, CUser* other, unsigned char a,
                                             unsigned char b, unsigned char c, unsigned char d,
                                             unsigned char e);
    static void SendNoticeMemberEnterPacketReply(CUser* user, CUser* other, unsigned char a,
                                                 unsigned char b, unsigned char c, unsigned char d,
                                                 unsigned char e);
    static void SendRequestMemberDeleteResult(CUser* user, unsigned char result,
                                              const char* name);
    static CApplication* m_pclApp;
};

class Packet_Item_Limit_Edition_Load_Data_Req : public PacketHeader
{
public:
    Packet_Item_Limit_Edition_Load_Data_Req();
    unsigned char m_fullLoad;   // +10（Ghidra log "FullLoad: %d"）
    unsigned int m_serverType;  // +11（Ghidra log "ServerType:%d"，与 serverGroup&0xff 比较）
    unsigned int m_loadTargetNum;  // +15（Ghidra log "LoadTargetNum: %d"）
    unsigned int m_ipgNo[24];   // +19
} __attribute__((packed));

class Packet_Item_Limit_Edition_Sell_end : public PacketHeader
{
public:
    Packet_Item_Limit_Edition_Sell_end();
    unsigned int m_serverType;   // +10（Ghidra log "ServerType: %d"）
    unsigned int m_sellEndNum;   // +14（Ghidra log "SellEndNum: %d"）
    unsigned int m_ipgNo[28];    // +18
} __attribute__((packed));

class Packet_Item_Limit_Edition_Load_Data_Rpy : public PacketHeader
{
public:
    signed char m_fullLoad;   // +10
    unsigned int m_count;     // +11
    struct __attribute__((packed)) Rec
    {
        unsigned int m_ipgno;  // +0
        char m_rest[0x44];     // +4 .. 0x47
    } m_items[28];             // +15，步长 0x48
} __attribute__((packed));

class Packet_Item_Limit_Edition_Update : public PacketHeader
{
public:
    Packet_Item_Limit_Edition_Update();
    unsigned int m_serverGroup;  // +10（processScheduledJob: Get_ServerGroup()）
    unsigned int m_count;     // +14
    struct __attribute__((packed)) Rec
    {
        unsigned int m_ipgno;        // +18
        unsigned int m_sellNum;      // +22
        char m_sellComplete;         // +26
    } m_records[28];                 // +18 .. +0x10d
} __attribute__((packed));

class Packet_Monitor_Event_Start : public PacketHeader
{
public:
    Packet_Monitor_Event_Start();
    unsigned int m_eventCode;    // +10（Ghidra log "eventCode(%d)"）
    union {
        unsigned int m_eventParam;  // +14 EventActionManager 按 uint 读写
        struct {
            unsigned short m_eventParam1;  // +14
            unsigned short m_eventParam2;  // +16
        };
    };
} __attribute__((packed));

class Packet_Monitor_Event_End : public PacketHeader
{
public:
    Packet_Monitor_Event_End();
    unsigned int m_eventCode;  // +10（Ghidra log "eventCode(%d)"）
} __attribute__((packed));

class Packet_Monitor_Take_Screen_Shot : public PacketHeader
{
public:
    Packet_Monitor_Take_Screen_Shot();
    unsigned char m_channel;  // +10（Ghidra log "channel(%d)"，0xff=全部）
    unsigned int m_time;      // +11（Ghidra log "time(%d)"）
} __attribute__((packed));

class Packet_DBMW_Query_Msg : public PacketHeader
{
public:
    Packet_DBMW_Query_Msg();
    unsigned int m_queryId;   // +0xa（dbmw set_query 第一参；SendMinTime 用 0x4ee2..4）
    unsigned int m_handleIdx; // +0xe（dbmw m_handles[] 索引；Village 用 6）
    char m_sql[0x1001];       // +0x12
} __attribute__((packed));

class Packet_VillageAttackedRewardServer : public PacketHeader
{
public:
    Packet_VillageAttackedRewardServer();
    unsigned int m_rewardType;  // +10（OnRewardVillageAttacked 置 0，透传）
} __attribute__((packed));

class Packet_VillageAttackedEnd : public PacketHeader
{
public:
    Packet_VillageAttackedEnd();
    unsigned int m_dungeonRemain;  // +10
    unsigned int m_huntingPoint;   // +14（=CVillageAttackedManager::m_field1c 当前狩猎点）
    unsigned int m_maxHuntingPoint;// +18（=CVillageAttackedManager::m_field20 最大狩猎点）
} __attribute__((packed));

class Packet_VillageAttackedUpdate : public PacketHeader
{
public:
    Packet_VillageAttackedUpdate();
    unsigned int m_remainTime;  // +10
    unsigned int m_huntingPoint;// +14
    unsigned int m_maxHuntingPoint;  // +18
} __attribute__((packed));

class Packet_VillageAttackedScore : public PacketHeader
{
public:
    Packet_VillageAttackedScore();
    unsigned int m_idByChannel;  // +10
    unsigned int m_uniqCharNo;   // +14
    unsigned int m_remainTime;   // +18
    unsigned int m_huntingPoint; // +22
    unsigned int m_maxHuntingPoint;  // +26
    int m_cur;                   // +30
    int m_max;                   // +34
} __attribute__((packed));

class Packet_VillageAttackedReward : public PacketHeader
{
public:
    Packet_VillageAttackedReward();
    unsigned int m_idByChannel;  // +10
    unsigned int m_uniqCharNo;   // +14
    int m_rewardType;            // +18
    int m_count;                 // +22
} __attribute__((packed));

class Packet_DBMW_Add_Buddy : public PacketHeader
{
public:
    Packet_DBMW_Add_Buddy();
    unsigned int m_uniqCharNo;  // +10
    char m_charName[0x1e];      // +14
} __attribute__((packed));

class Packet_DBMW_Del_Buddy : public PacketHeader
{
public:
    Packet_DBMW_Del_Buddy();
    unsigned int m_uniqCharNo;  // +10
    unsigned int m_buddyCharNo; // +14
    char m_charName[0x1e];      // +18
} __attribute__((packed));

class Packet_Monitor_Notice_Member_Member_Login_out : public PacketHeader
{
public:
    Packet_Monitor_Notice_Member_Member_Login_out();
    unsigned char m_flag;           // +10
    unsigned int m_idByChannel;     // +11
    unsigned int m_uniqCharNo;      // +15
    unsigned char m_channelNo;      // +19
    unsigned char m_type;           // +20
    char m_charName[0x1e];          // +21
    unsigned char m_expLevel;       // +51
    unsigned int m_uniqCharNo2;     // +52
} __attribute__((packed));

struct hyperlink_item_info
{
    char m_payload[0x68];
};

class Packet_Monitor_Member_Chat_ToUser_Hyper_Link : public PacketHeader
{
public:
    Packet_Monitor_Member_Chat_ToUser_Hyper_Link();
    unsigned int m_idByChannel;   // +10
    unsigned int m_uniqCharNo;    // +14
    char m_charName[0x1e];        // +18
    unsigned char m_itemCount;    // +48
    char m_items[0x138];          // +49
    unsigned char m_msgLen;       // +361
    char m_msg[0x100];            // +362
} __attribute__((packed));

class Packet_Monitor_Member_Chat_ToUser : public PacketHeader
{
public:
    Packet_Monitor_Member_Chat_ToUser();
    unsigned int m_idByChannel;   // +10
    unsigned int m_uniqCharNo;    // +14
    char m_charName[0x1e];        // +18
    unsigned char m_msgLen;       // +48
    char m_msg[0x100];            // +49
} __attribute__((packed));

class Packet_Monitor_Notice_Delete_Member_Id : public PacketHeader
{
public:
    Packet_Monitor_Notice_Delete_Member_Id();
    unsigned int m_idByChannel;  // +10
    unsigned int m_uniqCharNo;   // +14
} __attribute__((packed));

class Packet_Monitor_SAVE_Member_Update_Char_Info : public PacketHeader
{
public:
    Packet_Monitor_SAVE_Member_Update_Char_Info();
    unsigned int m_uniqCharNo;   // +10
    unsigned char m_flag;        // +14
} __attribute__((packed));

class Packet_Monitor_SAVE_Member : public PacketHeader
{
public:
    Packet_Monitor_SAVE_Member();
    unsigned char m_flag;        // +10（SaveMemberOnConnect: pkt.m_flag = flag）
    unsigned int m_upperCharNo;  // +11
    unsigned int m_lowerCharNo;  // +15
    unsigned char m_type;        // +19
} __attribute__((packed));

class Packet_Monitor_Request_Member_Enter_To_Requester : public PacketHeader
{
public:
    Packet_Monitor_Request_Member_Enter_To_Requester();
    unsigned int m_idByChannel;  // +10
    unsigned int m_uniqCharNo;   // +14
    unsigned char m_result;      // +18
    char m_name[0x1e];           // +19
} __attribute__((packed));

class Packet_Monitor_Request_Member_Enter_To_Responser : public PacketHeader
{
public:
    Packet_Monitor_Request_Member_Enter_To_Responser();
    unsigned int m_idByChannel;  // +10
    unsigned int m_uniqCharNo;   // +14
    unsigned short m_level;      // +18
    char m_name[0x1e];           // +20
} __attribute__((packed));

class Packet_Monitor_Member_Enter_Reply_ToResponser : public PacketHeader
{
public:
    Packet_Monitor_Member_Enter_Reply_ToResponser();
    unsigned char m_result;      // +10（结果码：1=成功，2/3/4=错误类型）
    unsigned char m_flag;        // +11（关系类型标志，==1 时附 extraCharNo）
    unsigned char m_flag2;       // +12
    unsigned int m_idByChannel;  // +13
    unsigned int m_uniqCharNo;   // +17
    unsigned char m_level;       // +21（调用传 GetLevel）
    char m_name[0x1e];           // +22
    unsigned char m_expLevel;    // +52（调用传 GetUpperMemberExpLevel）
    unsigned int m_extraCharNo;  // +53
} __attribute__((packed));

class Packet_Monitor_Notice_Member_Enter_Ok : public PacketHeader
{
public:
    Packet_Monitor_Notice_Member_Enter_Ok();
    unsigned char m_result;      // +10
    unsigned char m_flag;        // +11
    unsigned char m_flag2;       // +12
    unsigned int m_idByChannel;  // +13
    unsigned int m_uniqCharNo;   // +17
    unsigned char m_level;       // +21
    char m_name[0x1e];           // +22
    unsigned char m_expLevel;    // +52
    unsigned int m_extraCharNo;  // +53
} __attribute__((packed));

class Packet_Monitor_Member_Secede_To_Seceder : public PacketHeader
{
public:
    Packet_Monitor_Member_Secede_To_Seceder();
    unsigned int m_idByChannel;  // +10
    unsigned int m_uniqCharNo;   // +14
    unsigned char m_type;        // +18
    char m_name[0x1e];           // +19
} __attribute__((packed));

class Packet_Monitor_Member_Pay_Tax_ToUpper : public PacketHeader
{
public:
    Packet_Monitor_Member_Pay_Tax_ToUpper();
    unsigned int m_idByChannel;  // +10
    unsigned int m_uniqCharNo;   // +14
    int m_money;                 // +18
    int m_fatigue;               // +22
    char m_name[0x1e];           // +26
} __attribute__((packed));

class Packet_Notice_Find_Factory_Hub_User : public PacketHeader
{
public:
    Packet_Notice_Find_Factory_Hub_User();
    unsigned int m_idByChannel;  // +10
    unsigned char m_nameLen;     // +14
    char m_name[0x1e];           // +15
    unsigned char m_found;       // +45
    unsigned short m_field2e;    // +46
    unsigned int m_field30;      // +48
} __attribute__((packed));

class Packet_DBMW_Register_To_BlackList : public PacketHeader
{
public:
    Packet_DBMW_Register_To_BlackList();
    unsigned int m_charNo;       // +10
    char m_name[0x1e];           // +14
    unsigned int m_charNo2;      // +44
    int m_result;                // +48  DB 回包 0x5a=GM
} __attribute__((packed));

class Packet_Register_To_BlackList_RESULT : public PacketHeader
{
public:
    Packet_Register_To_BlackList_RESULT();
    unsigned int m_idByChannel;  // +10
    char m_name[0x1e];           // +14
    unsigned int m_charNo;       // +44
    unsigned char m_result;      // +48
} __attribute__((packed));

class Packet_DMBW_Delete_To_BlackList : public PacketHeader
{
public:
    Packet_DMBW_Delete_To_BlackList();
    unsigned int m_charNo;       // +10
    char m_name[0x1e];           // +14
    unsigned int m_charNo2;      // +44
} __attribute__((packed));

class Packet_Delete_To_BlackList_Result : public PacketHeader
{
public:
    Packet_Delete_To_BlackList_Result();
    unsigned int m_idByChannel;  // +10
    char m_name[0x1e];           // +14
    unsigned int m_charNo;       // +44
    unsigned char m_result;      // +48
} __attribute__((packed));

class Packet_Request_Result_BlackList : public PacketHeader
{
public:
    Packet_Request_Result_BlackList();
    unsigned int m_idByChannel;  // +10
    unsigned char m_count;       // +14
    STBlackUserDBType m_blackList[10];  // +15
} __attribute__((packed));

class Packet_Web_Notice_InGame_Advertisement : public PacketHeader
{
public:
    Packet_Web_Notice_InGame_Advertisement();
} __attribute__((packed));

class Packet_CollectItemsUpdate : public PacketHeader
{
public:
    Packet_CollectItemsUpdate();
    // 双向复用：UpdateCollectItems（monitor→DBMW）中
    // m_fieldA=CollectItms.m_field4 当前数, m_fieldF=m_field8 时间戳,
    // m_fieldE=Get_ServerGroup(), m_field13=m_fieldC。语义随方向而异，保留原名。
    unsigned int m_fieldA;   // +10
    unsigned char m_fieldE;  // +14
    unsigned int m_fieldF;   // +15
    unsigned char m_field13; // +19
} __attribute__((packed));

class Packet_CollectItemsResult : public PacketHeader
{
public:
    Packet_CollectItemsResult();
    // 双向复用：monitor→游戏服 m_fieldA=CollectItms.m_field0,
    // m_fieldE=m_field4, m_field12=m_field8；游戏服→monitor
    // onCollectItemsResult 读 m_fieldE→money, m_fieldA→uniqCharNo, m_field12→etc。
    unsigned int m_fieldA;   // +10
    unsigned int m_fieldE;   // +14
    unsigned int m_field12;  // +18
} __attribute__((packed));

class Packet_Server_Queue_Load_Statistic : public PacketHeader
{
public:
    Packet_Server_Queue_Load_Statistic();
    unsigned char m_flag;   // +10（canonical：Guild/DBMW 同名）
    unsigned char m_param;  // +11
    unsigned short m_value; // +12
} __attribute__((packed));

class Packet_InnerPakcet_Login : public PacketHeader
{
public:
    Packet_InnerPakcet_Login();
} __attribute__((packed));

class Packet_InnerPakcet_Logout : public PacketHeader
{
public:
    Packet_InnerPakcet_Logout();
} __attribute__((packed));

class Packet_Monitor_UDP_HeartBeat : public PacketHeader
{
public:
    Packet_Monitor_UDP_HeartBeat();
    unsigned char m_channel;  // +10（OnHeartBeat：频道号，0xc8=DB 完成，0xff=全部）
} __attribute__((packed));

class Packet_MiniCraneSeed : public PacketHeader
{
public:
    Packet_MiniCraneSeed();
    unsigned int m_seed;  // +10（SendMiniCraneRandomSeed: m_seed = getMiniCraneSeed()）
} __attribute__((packed));

class Packet_DB_Query_Member : public PacketHeader
{
public:
    Packet_DB_Query_Member();
    unsigned int m_memberKey;  // +10（QueryMember(key)）
} __attribute__((packed));

class Packet_DB_Query_Member_Member : public PacketHeader
{
public:
    Packet_DB_Query_Member_Member();
    unsigned int m_memberKey;  // +10（QueryMemberMember(key)）
} __attribute__((packed));

class Packet_Send_Time_Sync : public PacketHeader
{
public:
    Packet_Send_Time_Sync();
    unsigned short m_hour;  // +10（ProcessTimeSync: tm_hour）
    unsigned short m_min;   // +12（ProcessTimeSync: tm_min）
} __attribute__((packed));

class Packet_Monitor_Notice_Buddy_In_Out : public PacketHeader
{
public:
    Packet_Monitor_Notice_Buddy_In_Out();
    unsigned int m_charNo;       // +10
    unsigned int m_idByChannel;  // +14
    unsigned char m_channel;     // +18
    unsigned char m_flag1;       // +19（SendNoticeBuddyInOut flag1）
    unsigned char m_flag2;       // +20（SendNoticeBuddyInOut flag2）
    char m_name[0x1e];           // +21
    unsigned char m_flag3;       // +51（SendNoticeBuddyInOut flag3）
} __attribute__((packed));

class Packet_DBMW_Request_BlackList : public PacketHeader
{
public:
    Packet_DBMW_Request_BlackList();
    unsigned int m_charNo;  // +10
    unsigned char m_flag;   // +11
} __attribute__((packed));

class Packet_DB_Member_Delete_As_Charac_Delete : public PacketHeader
{
public:
    Packet_DB_Member_Delete_As_Charac_Delete();
    unsigned int m_charNo;  // +10
} __attribute__((packed));

class Packet_DBMW_Query_Buddy_Info : public PacketHeader
{
public:
    Packet_DBMW_Query_Buddy_Info();
    unsigned int m_charNo;  // +10（QueryBuddyInfo：lea -0x16 后 -0xc 就是这个字段）
} __attribute__((packed));

class Packet_Request_Charac_Tower_Ranking : public PacketHeader
{
public:
    Packet_Request_Charac_Tower_Ranking();
    unsigned int m_uniqCharNo;   // +10
    unsigned int m_idByChannel;  // +14
    unsigned int m_scores[5][4]; // +18 .. +0x61
} __attribute__((packed));

class Packet_Send_Time_Sync_For_Login : public PacketHeader
{
public:
    Packet_Send_Time_Sync_For_Login();
    unsigned int m_dbid;         // +10
    unsigned int m_idByChannel;  // +14
    unsigned short m_hour;       // +18
    unsigned short m_min;        // +20
} __attribute__((packed));

class Packet_Tcp_Server_Connect : public PacketHeader
{
public:
    Packet_Tcp_Server_Connect();
    unsigned char m_channel;  // +10
} __attribute__((packed));

class Packet_Monitor_Add_Buddy_Reply : public PacketHeader
{
public:
    Packet_Monitor_Add_Buddy_Reply();
    unsigned int m_charNo;      // +10
    unsigned int m_idByChannel; // +14
    char m_name[0x27];          // +18
    unsigned char m_channel;    // +57
    unsigned char m_result;     // +58
} __attribute__((packed));

class Packet_Monitor_Del_Buddy_Reply : public PacketHeader
{
public:
    Packet_Monitor_Del_Buddy_Reply();
    unsigned int m_charNo;      // +10
    unsigned int m_idByChannel; // +14
    char m_name[0x1e];          // +18
    unsigned char m_result;     // +48
} __attribute__((packed));

class Packet_DBMW_Connection_Check : public PacketHeader
{
public:
    Packet_DBMW_Connection_Check();
    unsigned char m_channel;  // +10
} __attribute__((packed));

class Packet_Monitor_Notice_Black_List : public PacketHeader
{
public:
    Packet_Monitor_Notice_Black_List();
    unsigned int m_dbid;         // +10
    unsigned int m_idByChannel;  // +14
    unsigned char m_count;       // +18
    unsigned int m_charNos[10];  // +19
} __attribute__((packed));

class Packet_CollectItemsReward : public PacketHeader
{
public:
    Packet_CollectItemsReward();
    unsigned int m_idByChannel;  // +10
    unsigned int m_charNo;       // +14
    unsigned char m_type;        // +18
    unsigned char m_nameLen;     // +19
    char m_name[0x1e];           // +20（ORIG ctor memset 0x1e，包大小 0x32）
} __attribute__((packed));

class Packet_CollectItemsRewardBroadcast : public PacketHeader
{
public:
    Packet_CollectItemsRewardBroadcast();
    unsigned int m_fieldA;       // +10
    unsigned int m_charNo;       // +14
    unsigned char m_type;        // +18
    unsigned char m_nameLen;     // +19
    char m_name[0x1e];           // +20
} __attribute__((packed));

class Packet_MTG_OntimeEvent_RewardStart : public PacketHeader
{
public:
    Packet_MTG_OntimeEvent_RewardStart();
    unsigned int m_eventIdx;   // +10
    unsigned int m_remainSec;  // +14（=m_field28*0x3c，剩余秒数）
    unsigned int m_itemIndex;  // +18（=COnTimeEventManager::m_field38）
    int m_itemCount;           // +22（=COnTimeEventManager::m_field3c）
} __attribute__((packed));

class Packet_Manager_Event_Trigger_Ack : public PacketHeader
{
public:
    Packet_Manager_Event_Trigger_Ack() : PacketHeader(0x1b6b, 0x16)
    {
        m_eventId = 0;
        m_flag = 0;
        m_group = 0;
    }
    unsigned int m_eventId;  // +10
    unsigned int m_flag;     // +14
    unsigned int m_group;    // +18
} __attribute__((packed));

class Packet_Req_Ontime_Event_Item : public PacketHeader
{
public:
    Packet_Req_Ontime_Event_Item() : PacketHeader(0x2345, 0xa) {}
} __attribute__((packed));

class Packet_Req_Ontime_Event_Idx : public PacketHeader
{
public:
    Packet_Req_Ontime_Event_Idx() : PacketHeader(0x2340, 0xa) {}
} __attribute__((packed));

class Packet_Register_Event_Idx : public PacketHeader
{
public:
    unsigned int m_eventIdx;  // +10
    signed char m_result;     // +14
} __attribute__((packed));

class Packet_Register_Event_User_Idx : public PacketHeader
{
public:
    unsigned int m_dbid;      // +10 FindUser
    unsigned int m_idx;       // +14
    short m_errortype;        // +18
} __attribute__((packed));

class Packet_Register_Event_Item : public PacketHeader
{
public:
    unsigned int m_idx;       // +10
    unsigned int m_cnt;       // +14
    short m_errortype;        // +18
} __attribute__((packed));

class Packet_Result_Register_Event_Idx : public PacketHeader
{
public:
    unsigned int m_eventIdx;  // +10
} __attribute__((packed));

class Packet_StartGameEventFromServer : public PacketHeader
{
public:
    unsigned int m_eventCode;  // +10
    unsigned int m_padE;       // +14
    unsigned int m_pad12;      // +18
    union {
        unsigned int m_eventParam;  // +22
        struct {
            unsigned short m_eventParam1;  // +22
            unsigned short m_eventParam2;  // +24
        };
    };
} __attribute__((packed));

class Packet_StopGameEventFromServer : public PacketHeader
{
public:
    unsigned int m_eventCode;  // +10
} __attribute__((packed));

class Packet_MTG_OntimeEvent_RewardEnd : public PacketHeader
{
public:
    Packet_MTG_OntimeEvent_RewardEnd();
} __attribute__((packed));

class Packet_Send_Periodic_Message : public PacketHeader
{
public:
    Packet_Send_Periodic_Message() : PacketHeader(0x1f4a, 0x20a)
    {
        memset(m_msg, 0, 0x200);
    }
    char m_msg[0x200];   // +0xa payload
} __attribute__((packed));

class Packet_DB_Save_Member_Exp : public PacketHeader
{
public:
    Packet_DB_Save_Member_Exp() : PacketHeader(0x641, 0x16) {}
    unsigned int m_memberKey;    // +10
    unsigned int m_upperCharId;  // +14
    unsigned int m_exp;          // +18
} __attribute__((packed));

class Packet_Monitor_Notice_MemberExp_LevelUp : public PacketHeader
{
public:
    Packet_Monitor_Notice_MemberExp_LevelUp() : PacketHeader(0x642, 0x13) {}
    unsigned int m_idByChannel;  // +10
    unsigned int m_uniqCharNo;   // +14
    unsigned char m_level;       // +18
} __attribute__((packed));

class BuddyList
{
public:
    BuddyList()
    {
        m_channelNo = 0xff;
        m_blackFlag = 0;
        m_online = 0;
        memset(m_pad3, 0, 0x27);
    }
    signed char m_channelNo;  // +0（SendConnectedBuddysList: 频道号，0xff=离线）
    char m_blackFlag;         // +1（IsBlackUser 时置 1）
    char m_online;            // +2（GetGameServer 非空时置 1）
    char m_pad3[0x27];        // +3 .. +0x29
} __attribute__((packed));

class Packet_Monitor_Reply_Buddy_List : public PacketHeader
{
public:
    Packet_Monitor_Reply_Buddy_List() : PacketHeader(0x3f2, 0x54f), m_dbid(0), m_count(0) {}
    unsigned int m_dbid;         // +0xa（SendConnectedBuddysList: GetDBID）
    unsigned char m_count;       // +0xe（好友数量，>0x20 截断）
    BuddyList m_buddies[32];     // +0xf
} __attribute__((packed));

class Packet_DB_InsertMail : public PacketHeader
{
public:
    Packet_DB_InsertMail();
    unsigned int m_characNo;   // +0xa（收件人 charNo）
    unsigned int m_fieldE;     // +0xe（=dbmw InsertMail hE）
    unsigned int m_field12;    // +0x12（=dbmw InsertMail h12）
    unsigned int m_field16;    // +0x16（=dbmw InsertMail h16）
    char m_subject[0x15];      // +0x1a（邮件标题）
    char m_content[0x100];     // +0x2f（邮件正文，与 dbmw Packet_DBMW_Insert_Mail 一致）
    unsigned int m_delayHours; // +0x12f（延迟小时数）
} __attribute__((packed));

class Packet_VillageAttackedStart : public PacketHeader
{
public:
    Packet_VillageAttackedStart() : PacketHeader(0x1773, 0x16)
    {
        m_remainTime = 0;
        m_curHuntingPoint = 0;
        m_maxHuntingPoint = 0;
    }
    int m_remainTime;       // +10（OnStartVillageAttacked：lea -0x1e 后 -0x14）
    int m_curHuntingPoint;  // +14（-0x10）
    int m_maxHuntingPoint;  // +18（-0xc）
} __attribute__((packed));

class Packet_VillageAttackedCountdown : public PacketHeader
{
public:
    Packet_VillageAttackedCountdown() : PacketHeader(0x1776, 0xe), m_countdown(0) {}
    int m_countdown;  // +0xa
} __attribute__((packed));

class Packet_DB_VillageAttackedRank : public PacketHeader
{
public:
    Packet_DB_VillageAttackedRank() : PacketHeader(0x177b, 0x1b) {}
    signed char m_group;  // +10
    int m_time0a;         // +11
    int m_time0b;         // +15
    int m_time1a;         // +19
    int m_time1b;         // +23
} __attribute__((packed));

class Packet_Request_Load_Tower_Full_Rank : public PacketHeader
{
public:
    // ORIG：仅调用 PacketHeader ctor，不置零（+0xa/+0xe 保持未初始化）
    Packet_Request_Load_Tower_Full_Rank() : PacketHeader(0x4cc, 0x12) {}
    int m_rankNo;          // +0xa
    unsigned int m_channel;// +0xe
} __attribute__((packed));

class Packet_Request_IPCounterList : public PacketHeader
{
public:
    Packet_Request_IPCounterList() : PacketHeader(0x1038, 0xc)
    {
        m_type = 5;
        m_param = 0;
    }
    unsigned char m_type;   // +0xa（SendDBMWRequestIPCounter flag）
    unsigned char m_param;  // +0xb（SendDBMWRequestIPCounter b）
} __attribute__((packed));

class Packet_Monitor_ServerEvent_Start : public PacketHeader
{
public:
    Packet_Monitor_ServerEvent_Start() : PacketHeader(0x44e, 0x12) {}
    int m_eventCode;             // +10
    unsigned short m_param1;     // +14
    unsigned short m_pad16;      // +16
    unsigned short m_param2;     // +18
} __attribute__((packed));

class Packet_TOD_DoRandomSelect : public PacketHeader
{
public:
    Packet_TOD_DoRandomSelect() : PacketHeader(0x2712, 0xa) {}
} __attribute__((packed));

class Packet_DBMW_Statistic_Login_Logout : public PacketHeader
{
public:
    Packet_DBMW_Statistic_Login_Logout();
    unsigned int m_channelNo;   // +0xa（服务器组/频道号）
    stLoginLogoutVariable m_entries[255]; // +0xe
    unsigned int m_occCount0;   // +0x608（=CLoginLogoutStatistics::m_fieldac）
    unsigned int m_occCount4;   // +0x60c（=m_fieldb0）
    unsigned int m_loginCount;  // +0x610（=m_fieldb4）
    unsigned int m_logoutCount; // +0x614（=m_fieldb8）
} __attribute__((packed));

class Packet_Arad_DeleteEffect : public PacketHeader
{
public:
    Packet_Arad_DeleteEffect(int group, int code);
    int m_group;  // +10
    int m_code;   // +14
} __attribute__((packed));

class Packet_Monitor_Call_Member_List_ToUser : public PacketHeader
{
public:
    Packet_Monitor_Call_Member_List_ToUser();
    unsigned int m_idByChannel;     // +10
    unsigned int m_uniqCharNo;      // +14
    STMemberListInfo m_memberList;  // +18 (0x12)
} __attribute__((packed));

class Packet_Punish_Cancel : public PacketHeader
{
public:
    Packet_Punish_Cancel();
    unsigned int m_idByChannel;  // +10
    unsigned short m_type;       // +14（透传，OnMonitorPunishCancel 原样转发）
    unsigned short m_param;      // +16（透传）
} __attribute__((packed));

class Packet_Set_CleanPad_Point : public PacketHeader
{
public:
    Packet_Set_CleanPad_Point();
    unsigned int m_idByChannel;  // +10
    unsigned short m_point;      // +14（透传点数值）
} __attribute__((packed));

class Packet_Request_Set_CleanPad_Point : public PacketHeader
{
public:
    unsigned int m_dbid;         // +10 FindUser
    unsigned short m_point;      // +14
} __attribute__((packed));

class Packet_SecuService_Connect_Web : public PacketHeader
{
public:
    Packet_SecuService_Connect_Web();
    unsigned int m_idByChannel;  // +10
    unsigned char m_fieldE;      // +14
    unsigned char m_fieldF;      // +15
    char m_payload[5];           // +16
} __attribute__((packed));

class Packet_Monitor_User_Repel : public PacketHeader
{
public:
    Packet_Monitor_User_Repel();
    unsigned int m_idByChannel;  // +10  inbound: dbid，转发前改写为 idByChannel
    unsigned int m_charNo;       // +14
} __attribute__((packed));

class Packet_Monitor_Other_Channel_Chat_ToUser : public PacketHeader
{
public:
    Packet_Monitor_Other_Channel_Chat_ToUser();
    unsigned int m_senderCharId;  // +10
    unsigned char m_serverId;     // +14（canonical：共享头 server_id）
    unsigned int m_idByChannel;   // +15
    unsigned int m_uniqCharNo;    // +19
    char m_name[0x1e];            // +23
    unsigned char m_type;         // +53
    unsigned char m_msgLen;       // +54
    char m_msg[0x100];            // +55
} __attribute__((packed));

class Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link : public PacketHeader
{
public:
    Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link();
    unsigned int m_senderCharId;  // +10
    unsigned char m_serverId;     // +14（canonical：共享头 server_id）
    unsigned int m_idByChannel;   // +15
    unsigned int m_uniqCharNo;    // +19
    char m_name[0x1e];            // +23
    unsigned char m_type;         // +53
    unsigned char m_itemCount;    // +54
    char m_items[3][0x68];        // +55
    unsigned char m_msgLen;       // +367
    char m_msg[0x100];            // +368
} __attribute__((packed));

class Packet_Change_User_Handicap : public PacketHeader
{
public:
    Packet_Change_User_Handicap();
    unsigned int m_charNo;   // +10（OnWebChangeUserHandicap 透传）
    unsigned int m_fieldE;   // +14（透传，语义未明）
    unsigned int m_field12;  // +18（透传，语义未明）
} __attribute__((packed));

class Packet_Web_Request_ARS_Info : public PacketHeader
{
public:
    Packet_Web_Request_ARS_Info();
    unsigned char m_flag;  // +10
} __attribute__((packed));

class Packet_Arad_ApplyEffect : public PacketHeader
{
public:
    Packet_Arad_ApplyEffect(int group, int code, unsigned int time);
    int m_group;   // +10
    int m_code;    // +14
    unsigned int m_time;  // +18
} __attribute__((packed));

class Packet_Load_Periodic_Message : public PacketHeader
{
public:
    Packet_Load_Periodic_Message();
} __attribute__((packed));

class Packet_Result_Load_Periodic_Message : public PacketHeader
{
public:
    char m_msg[0x200];  // +10
    int m_start;        // +0x20a
    int m_end;          // +0x20e
} __attribute__((packed));

class Packet_Web_Emergency_Patch_Message : public PacketHeader
{
public:
    char m_channels[0x32];  // +10
    unsigned char m_len;    // +60
    char m_text[0xff];      // +61
} __attribute__((packed));

class Packet_Social_Event_Reward_Item_Response : public PacketHeader
{
public:
    unsigned int m_count;  // +10
    struct __attribute__((packed)) Rec
    {
        unsigned int m_itemId;
        unsigned int m_maxCount;
        unsigned int m_sellCount;
    } m_items[30];         // +14，步长 0xc
} __attribute__((packed));

class Packet_Request_Revenge_Dungeon : public PacketHeader
{
public:
    unsigned int m_rewardType;  // +10
    unsigned int m_remainTime;  // +14
} __attribute__((packed));

class Packet_IPCounter_Control : public PacketHeader
{
public:
    unsigned char m_type;   // +10
    unsigned char m_value;  // +11
} __attribute__((packed));

class Packet_IP_Counter_List : public PacketHeader
{
public:
    unsigned char m_dataStats;  // +10
    unsigned char m_dataSize;   // +11
} __attribute__((packed));

class Packet_Cera_Update : public PacketHeader
{
public:
    unsigned int m_dbid;  // +10 FindUser
} __attribute__((packed));

class Packet_Event_Item_Update : public PacketHeader
{
public:
    unsigned int m_dbid;       // +10 FindUser
    unsigned int m_charNo;     // +14
    unsigned int m_eventType;  // +18
    unsigned int m_itemId;     // +22
    unsigned int m_stack;      // +26
    unsigned int m_tableId;    // +30
} __attribute__((packed));

class Packet_Web_Query_User_State : public PacketHeader
{
public:
    unsigned int m_dbid;  // +10 FindUser
    char m_padE[4];       // +14
    char m_online;        // +18
} __attribute__((packed));

class Packet_DB_Reply_Query_Member : public PacketHeader
{
public:
    char m_success;               // +10
    unsigned int m_memberId;      // +11
    unsigned int m_maxCreateTime; // +15
    unsigned int m_maxDeleteTime; // +19
    STMemberDBInfo m_info;        // +23
} __attribute__((packed));

class Packet_DBMW_Add_Buddy_Reply : public PacketHeader
{
public:
    unsigned int m_charNo;   // +10
    STBuddyDBInfo m_buddy;   // +14  m_characNo 位于 +0x30
    signed char m_result;    // +53
} __attribute__((packed));

class Packet_DBMW_Del_Buddy_Reply : public PacketHeader
{
public:
    unsigned int m_charNo;      // +10
    unsigned int m_buddyCharNo; // +14
    char m_name[0x1e];          // +18
    signed char m_result;       // +48
} __attribute__((packed));

class Packet_DBMW_Query_Buddy_Info_Reply : public PacketHeader
{
public:
    unsigned int m_charNo;    // +10
    unsigned char m_count;    // +14
    STBuddyDBInfo m_items[1]; // +15
} __attribute__((packed));

class Packet_Reply_Load_Tower_Full_Rank : public PacketHeader
{
public:
    char m_reset;          // +10
    unsigned int m_count;  // +11
    unsigned int m_total;  // +15
} __attribute__((packed));

#endif  // MONITOR_DNFPACKETTRANSLATER_H_
