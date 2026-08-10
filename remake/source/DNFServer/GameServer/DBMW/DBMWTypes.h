#ifndef DBMW_TYPES_H_
#define DBMW_TYPES_H_

#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include <sys/time.h>
#include <netinet/in.h>
#include <mysql.h>
#include <stdexcept>
#include "PacketHeader.h"
#include "Thread.h"

#include "Packet_InnerPakcet_Login.h"
#include "Packet_InnerPakcet_Logout.h"
#include "Packet_Monitor_Event_Start.h"
#include "Packet_Monitor_Event_End.h"
#include "Packet_Monitor_Manager_Connect_OK.h"
#include "Packet_Web_Notice_Single.h"
#include "Packet_Web_Notice_InGame_Advertisement.h"
#include "Packet_Web_Prohibit_User_Connect.h"

class CApplication;
class CUser;
class CServerHandler;
class CUdpRecvBuffer;
class CTcpRecvBuffer;
class CTcpSendBuffer;
class CPacketBuffer;
class CPeer;
class CTcpNetSystem;
class CUserManager;
class CUdpHandler;
class CUdpNetworkThread;
class CTcpNetworkThread;
class CTcpAcceptThread;
class CTcpHandler;
class EpollHandler;
class CDBHandle;
class CMySql;
class CDBManager;
class LimitNpcBuyItemRequestInfo;
class LimitNpcBuyItemResultInfo;
class LimitNpcBuyItemUpdate;
class Packet_DB_Load_Request_Guild_Board_Write;
class Packet_DB_Load_Request_Web_Guild_Board_Write;
class Packet_DB_Load_Guild_Agit;
class Packet_DB_Create_Guild_Agit;
class Packet_DB_Delete_Guild_Agit;
class Packet_DB_Upgrade_Guild_Agit;
class Packet_DB_Insert_Guild_Cargo_History;
class Packet_DB_Save_Power_War_Bonus_Point;
class Packet_DB_Save_Power_War_Point;
class Packet_DB_Save_Power_War_Statue_Ranker;
class Packet_DB_Save_Power_War_Point_Reward;
class Packet_Load_Periodic_Message;
class Packet_Result_Loading_Periodic_Message;
class Packet_DB_Save_Power_War_User_Rank;
class Packet_DB_Save_Power_War_Guild_Rank;
class Packet_Udp_Characteristic;
class Packet_StartGameEventFromServer;
class Packet_StopGameEventFromServer;
class Packet_P2P_Statistics;
class Packet_Goldcard_Event_Statistic_STD;
class Packet_DBMW_HellParty_Statistic_Item;
class Packet_DBMW_Packet_Overflow_Statistic;
class Packet_DBMW_Save_Error_Line_Statistic;
class Packet_TowerOfDespair_Statistic_STD;
class Packet_DB_VillageAttackedRank;
class Packet_GameServer2Statisctics2DBServer;
class Packet_Randombox_statistic_DB;
class Packet_DBMW_Query_Msg;
class Packet_DBMW_Query_String;
class Packet_Emblem_Create_Statistic_DB;
class Packet_DB_Write_Guild_Member_Memo;
class Packet_Server_Match_data_DBMW;
class Packet_Manager_Event_Trigger_Ack;
class Packet_DBMW_Loading_Time_Report;
class Packet_DBMW_Fatigue_Battery_Money_Statistic;
class Packet_Load_Periodic_Message;
class Packet_DB_Request_Guild_Secede;
class Packet_DBMW_Request_Guild_Create;
class Packet_DB_Reply_Query_Member;
class ST_MemberProxy;
class STGuildMemberProxy;
class STGuildJoinInfo;
class Packet_DBMW_Dungeon_Statistic_Party;
class Packet_DBMW_Dungeon_Statistic_Party_Job;
class Packet_DBMW_Dungeon_Statistic_Party_Charac;
class Packet_Item_Limit_Edition_Load_Data_Req;
class Packet_Item_Limit_Edition_Load_Data_Rpy;
class Packet_Item_Limit_Edition_Update;
class Packet_DBMW_Change_Char_Name;
class Packet_DB_Guild_Cargo_Upgrade;
class Packet_DB_Update_Guild_Cargo;
class Packet_Result_OnTimeEvent_Idx;
class CQueryCounter;
class CMonitorServer;
class CStatisticsServer;
class CTcpServer;
class CSignalTranslator;
class CSignal;
class CDnFTimer;
class CUnixTimer;
class CSystemTime;
class CSystemTimeHandler;
class CAppInit;
class CAppStartInit;
class CAppStopInit;
class CTableBase;
class CAppConfig;
class CServerConfig;
class CKillUSRConfig;
class CProtocol;
class CPacketDecoder;
class CPacketTracer;
class CSourceVersionMgr;
class CDNFProhibitUser;
class ST_ServerInfo;
class ST_Guild_War_Info;
class STGuildMemberProxy;
class Packet_Monitor_UDP_HeartBeat;
class CGuildManager;

enum ENUM_DB_HANDLE_IDX {
    DB_HANDLE_0 = 0, DB_HANDLE_1, DB_HANDLE_2, DB_HANDLE_3, DB_HANDLE_4,
    DB_HANDLE_5, DB_HANDLE_6, DB_HANDLE_7, DB_HANDLE_8, DB_HANDLE_9,
    DB_HANDLE_A, DB_HANDLE_B, DB_HANDLE_C, DB_HANDLE_D, DB_HANDLE_E,
    DB_HANDLE_F, DB_HANDLE_10
};
enum ENUM_DB_KIND { DB_KIND_0 = 0 };
enum ENUM_SERVER_GROUP { ENUM_SERVER_GROUP_0 = 0 };

#endif
