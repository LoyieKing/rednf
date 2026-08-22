#ifndef GAME_GLOBALDATA_H_
#define GAME_GLOBALDATA_H_

// ============================================================================
// df_game_r 还原 —— GlobalData（全局单例池）。
// 函数参照 docs/class_func_reports/GlobalData.md；s_* 地址/尺寸取自 ORIG nm。
// 对象型成员（s_systemTime_/s_accept_lock/s_accept_que/s_hades_/
// s_capchaDataManager/cg_）为真实类型声明；其定义置于全局 TU（避免单 TU
// 检查时额外产出 __static_initialization_and_destruction_0 符号）。
// ============================================================================

#include <queue>

#include "GameBase.h"
#include "CSystemTime.h"
#include "StreamPool.h"

// ---- THREAD_TYPE（ORIG mangling 需要枚举类型名）----
enum THREAD_TYPE
{
    THREAD_TYPE_NETWORK = 0,
    THREAD_TYPE_DISPATCH = 1,
    THREAD_TYPE_DB = 2,
    THREAD_TYPE_TIMER = 3,
    THREAD_TYPE_ACCEPT = 4,
    THREAD_TYPE_5 = 5,
    THREAD_TYPE_6 = 6,
    THREAD_TYPE_7 = 7,
    THREAD_TYPE_MONITOR_TCP = 8
};

// ---- 前向声明（本批不产出实现；类型来自 ORIG mangling/报告）----
class CItemAmplifier;
class CGlobalEffectManager;
class CTimeGate;
class CPowerManager;
class CFatigueBatteryHandle;
class CmmChannelProxy;
class CLogGameChannel;
class CCeraShop;
class ItemVendingMachine;
class Secu_DataControl;
class CSecu_ProtectionField;
class AcceptThread;
class RefPvpGrade;
class CDBConnector;
class CSimpleSSO;
class CGMAccounts;
class CSecurityCardCenter;
class CEventManager;
class Gm_List;
class CGM_Manager;
class CStatisticMgr;
class CBusinessImpl;
class CExpertJobMgr;
class CVillageObjectMgr;
class CVillageMonsterMgr;
class CRevengeDungeon;
class CDailyScheduleManager;
class COnlinePreliminaryTeamMgr;
class CGameMasterMgr;
class CExtractionMgr;
class NPC_Teleport;
class User_Teleport;
class CRandomOptionItemHandle;
class ServiceRestrictManager;
class RestrictGeolocation;
class CUser;
class PacketPool;
class DBMgr;
class MsgQueueMgr;
class DBThread;
class DispatchThread;
class NetworkThread;
class UdpThread;
class MonitorTcpThread;
class CHadesServerProxy;
class CDoubleConnCheckServerProxy;
class CSchoolServerProxy;
class CPCRoomServerProxy;
class CCommunityServerProxy;
class CAuctionServerProxy;
class CCeraAuctionServerProxy;
class CMonitorServerProxy;
class CGuildServerProxy;
class CStatisticServerProxy;
class CRijndael;
class CaptchaGen;
class GreyImage;
class Cipher;
class MySQL;
class CQueryCounter;
class CHumanCertifier;

template <class T> class CServerProxyMgr;

namespace ARAD
{
class Arad_ServerStateManager;
}

namespace WongWork
{
class CSimpleSSO;
class CGMAccounts;
class CSecurityCardCenter;
class CLogGameChannel;
class CCeraShop;
class CBlackIPMonitor;
class CAutoPunishRuleBlackIPMgr;
class CAutoPunishRuleHackTypeMgr;
class CCapchaDataManager;
namespace IPG { class CIPGHelper; }
}

namespace secretshop { class CSecretShop; }
namespace expert_job { class CExpertJobMgr; }
namespace village_object { class CVillageObjectMgr; }
namespace village_attacked { class CRevengeDungeon; class CVillageMonsterMgr; }
namespace online_preliminary { class COnlinePreliminaryTeamMgr; }
namespace break_away_prevent { class CBreakAwayPreventSystem; }
namespace game_master { class CGameMasterMgr; }
namespace StackableAction { class Action; }
namespace random_option { class CRandomOptionItemHandle; }
namespace expert_extraction { class CExtractionMgr; }
namespace sync_script { class CSyncScript; }
namespace XNuclear { class CHades_GameSvr; }

// ---- GlobalData（ORIG 为类静态成员；CUserCharacInfo.h 同名单名空间已存在，
//      故统一为 namespace 形态——静态方法→命名空间函数、静态成员→extern 全局，
//      mangled 名不变：_ZN11GlobalData4InitEv / _ZN11GlobalData11s_msgq_mgrE）----
namespace GlobalData
{
bool Init();                          // 0x08299fa0
void Fini();                          // 0x0829e0b2
bool InitCaptcha();                   // 0x0829d438
void GenerateCaptchaData();           // 0x0829d4b0
bool initTournamentDB(const char*);   // 0x0829dd82
bool setCurrentPacketPool(THREAD_TYPE type, unsigned long threadID);
                                      // 0x0829e4c4

// ---- 全局状态 ----
extern bool s_server_stop;                                   // 0940bd64 (1)
extern MsgQueueMgr* s_msgq_mgr;                              // 0940bd68
extern StreamPool* s_stream_pool;                            // 0940bd6c
extern PacketPool* s_packet_pool_list[9];                    // 0940bd80
extern BigStreamPool* s_big_stream_pool;                     // 0940bda4
extern SmallStreamPool* s_small_stream_pool;                 // 0940bda8
extern DBMgr* s_db_mgr;                                      // 0940bdac
extern DBThread* s_db_thr;                                   // 0940bdb0
extern DBThread* s_db_thr_for_logDB;                         // 0940bdb4
extern DispatchThread* s_dispatch_thr;                       // 0940bdb8
extern NetworkThread* s_net_thr;                             // 0940bdbc
extern UdpThread* s_udp_thr;                                 // 0940bdc0
extern AcceptThread* s_accept_thr;                           // 0940bdc4
extern MonitorTcpThread* s_monitor_tcp_thr;                  // 0940bdc8
extern Mutex s_accept_lock;                                  // 0940bdcc (0x18)
extern std::queue<CUser*, std::deque<CUser*> > s_accept_que; // 0940be00 (0x28)
extern CServerProxyMgr<CMonitorServerProxy>* s_monitor_proxy_mgr;   // 0940be28
extern CServerProxyMgr<CGuildServerProxy>* s_guild_proxy_mgr;       // 0940be2c
extern CServerProxyMgr<CStatisticServerProxy>* s_statistic_proxy_mgr; // 0940be30
extern CHadesServerProxy* s_hades_proxy;                     // 0940be34
extern CDoubleConnCheckServerProxy* s_double_check_proxy;    // 0940be38
extern CSchoolServerProxy* s_school_proxy;                   // 0940be3c
extern CPCRoomServerProxy* s_pcroom_proxy;                   // 0940be40
extern CCommunityServerProxy* s_community_proxy;             // 0940be44
extern CAuctionServerProxy* s_auction_proxy;                 // 0940be48
extern CCeraAuctionServerProxy* s_cera_auction_proxy;        // 0940be4c
extern CPowerManager* s_power_manager;                       // 0940be50
extern secretshop::CSecretShop* s_secret_shop;               // 0940be54
extern CFatigueBatteryHandle* s_fatigueBatteryHandle_;       // 0940be58
extern CmmChannelProxy* s_channel_proxy;                     // 0940be5c
extern CRijndael* s_pcryptRijndael_CharacView_;              // 0940be60
extern CRijndael* s_pcryptRijndael_Password_;                // 0940be64
extern CRijndael* s_pcryptRijndael_MousePassword_;           // 0940be68
extern Cipher* s_secu_cipher_keystring_manager_[0x4e20];     // 0940be80
extern CTEA* s_pcryptTEA_MousePassword_;                     // 0941f700
extern CTEA* s_pcryptTEA_SecurityCard_;                      // 0941f704
extern WongWork::CSimpleSSO* s_psimpleSSO;                   // 0941f708
extern WongWork::CSecurityCardCenter* s_securityCardCenter;  // 0941f70c
extern WongWork::CGMAccounts* s_pGMAccounts_;                // 0941f710
extern CSystemTime s_systemTime_;                            // 0941f714 (0x14)
extern XNuclear::CHades_GameSvr s_hades_;                    // 0941f728 (8)
extern CEventManager* s_event_manager;                       // 0941f730
extern WongWork::IPG::CIPGHelper* s_pIPGHelper;              // 0941f734
extern char g_SPremiumInfo[0x7740];                          // 09483a40
extern void* s_rentalSystem;                                 // 0941f738
extern RefPvpGrade* s_ref_pvp_grade;                         // 0941f73c
extern WongWork::CLogGameChannel* s_pLogGameChannel;         // 0941f740
extern WongWork::CCeraShop* s_pCeraShop;                     // 0941f744
extern CDBConnector* s_db_connector;                         // 0941f748
extern void* s_pCompoundAvatar;                              // 0941f74c
extern void* s_pCompoundBindCube;                            // 0941f750
extern void* s_pTowerOfDespairMgr;                           // 0941f754
extern void* s_pEvent_UpGrade_Jar;                           // 0941f758
extern CGM_Manager* s_GM_Manager;                            // 0941f75c
extern ItemVendingMachine* s_pItemVendingMachine;            // 0941f760
extern CStatisticMgr* s_statisticMgr;                        // 0941f764
extern expert_job::CExpertJobMgr* s_ExpertJobMgr;            // 0941f768
extern break_away_prevent::CBreakAwayPreventSystem* s_BreakAwaySys; // 0941f76c
extern Gm_List* gmList;                                      // 0941f770
extern village_object::CVillageObjectMgr* s_villageObjectMgr;// 0941f774
extern game_master::CGameMasterMgr* s_GameMasterMgr;         // 0941f778
extern village_attacked::CVillageMonsterMgr* s_villageMonsterMgr; // 0941f77c
extern village_attacked::CRevengeDungeon* s_revengeDungeonMgr;    // 0941f780
extern CDailyScheduleManager* s_DailyScheduleManager;        // 0941f784
extern online_preliminary::COnlinePreliminaryTeamMgr* s_onlinePreliminaryTeamMgr; // 0941f788
extern WongWork::CCapchaDataManager s_capchaDataManager;     // 0941f7a0 (0x28)
extern Secu_DataControl* s_pSecuDataControl;                 // 0941f7c8
extern CSecu_ProtectionField* s_pSecuProtectionField;        // 0941f7cc
extern WongWork::CAutoPunishRuleBlackIPMgr* g_autoPunishRuleBlackIPMgr; // 0941f7d0
extern WongWork::CAutoPunishRuleHackTypeMgr* g_autoPunishRuleHackTypeMgr; // 0941f7d4
extern WongWork::CBlackIPMonitor* g_blackIPMonitor;          // 0941f7d8
extern unsigned int g_IPMonitorLastDBAccessTime;             // 0941f7dc
extern CBusinessImpl* s_antibotBusinessImpl_;                // 0941f7e0
extern void* s_antibotChecker_;                              // 0941f7e4
extern CItemAmplifier* s_itemAmplifier_;                     // 0941f7e8
extern CGlobalEffectManager* s_globalEffectManager_;         // 0941f7ec
extern ARAD::Arad_ServerStateManager* s_serverStateManager_; // 0941f7f0
extern CTimeGate* s_timeGate_;                               // 0941f7f4
extern int g_GoldControlDateFlag;                            // 0941f7f8
extern StackableAction::Action* g_Actions;                   // 0941f7fc
extern expert_extraction::CExtractionMgr* g_ExtractionMgr;   // 0941f800
extern CaptchaGen cg_;                                       // 0941f804 (0x14)
extern void* g_pTeleport[2];                                 // 0941f818 (8)
extern random_option::CRandomOptionItemHandle* g_randomOptionHandle; // 0941f820
extern void* s_pAvatarRoulette;                              // 0941f824
}

#endif  // GAME_GLOBALDATA_H_
