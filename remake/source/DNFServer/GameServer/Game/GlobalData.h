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
#include "EventUpgradeJarInfo.h"   // Event_Upgrade_Jar 完整类型（s_pEvent_UpGrade_Jar）
#include "GameBase.h"
#include "Action.h"               // StackableAction::Action 权威定义（input_param/check/run）
#include "SPremiumInfo.h"         // g_SPremiumInfo 权威元素类型（SPremiumInfo 0x120）
#include "CSystemTime.h"
    #include "CEnvironment.h"        // STDBConnInfo / ENUM_DB_HANDLE_IDX（CDBConnector）
    #include "StreamPool.h"
#include <boost/function.hpp>

class InstanceRentalSystem;       // 前向：s_rentalSystem 强指针（Fini 单 mov 访存）

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
    class DBMgr;

    // ---- CDBConnector 完整声明（全局类；定义移自 GlobalData.cpp，实现仍在 .cpp）----
    typedef std::map<ENUM_DB_HANDLE_IDX, ENUM_SERVER_GROUP> DBGroupMap;
    typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DBInfoMap;
    // [Q3PrettSweepA 2026-09-11] ORIG `Connect` 的 prett 串用 DBConnInfo 拼写
    // （`bool CDBConnector::Connect(DBConnInfo)` size 0x39）⇒ 同底层别名，
    // mangled/ABI 零变化；其它成员仍用 DBInfoMap（各自 prett 拼写独立）。
    typedef DBInfoMap DBConnInfo;
    class CDBConnector
    {
    public:
        CDBConnector(DBMgr* dbMgr);
        ~CDBConnector();
        bool Init(STDBConnInfo& info);
        bool GetConnInfo(DBGroupMap& groupMap, DBInfoMap& infoMap);
        bool Connect(DBConnInfo infoMap);
        bool ConnectWithServerGroup(DBInfoMap infoMap);
        bool QueryConnInfo(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
                           DBInfoMap& outMap);
        bool OpenMaster();
        bool Open(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP group,
                  STDBConnInfo& info);
        void Close();
        void CloseMaster();
    private:
        DBMgr* m_dbMgr;            // +0x00
        STDBConnInfo m_connInfo;   // +0x04（0x168 字节）
    };
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
class CItem;
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
namespace village_attacked
{
class CRevengeDungeon
{
public:
    CRevengeDungeon();
    bool IsOpenRevengeDungeon();   // ORIG W 0x8234ff6（读 +0x0 字节）
    int GetCloseTime();            // ORIG _ZN16village_attacked15CRevengeDungeon11GetCloseTimeEv
    void SetCloseTime(unsigned int sec);        // ORIG W 0x0847dd36（InterWave1 批）
    void OpenRevengeDungeon();                  // ORIG W 0x0847dd1e
    void CloseRevengeDungeon();                 // ORIG W 0x0847dd2a
    bool m_isOpen;                 // +0x00
    unsigned int m_field04;        // +0x04（ORIG SetCloseTime 0x0847dd39 this
                                   //  先载直存 ⇒ 与 unsigned 形参全等）
};
class CVillageMonsterMgr;
}
namespace online_preliminary { class COnlinePreliminaryTeamMgr; }
namespace break_away_prevent { class CBreakAwayPreventSystem; }
namespace game_master { class CGameMasterMgr; }

namespace random_option { class CRandomOptionItemHandle; }
namespace expert_extraction { class CExtractionMgr; }
namespace sync_script { class CSyncScript; }
namespace XNuclear { class CHades_GameSvr; }

// ---- GlobalData（ORIG 形态 = class 静态成员）----
// [R18gd 2026-09-13] 完全复刻 ORIG：namespace→class（静态成员函数 + 静态数据成员）。
//   ① 3 件 prett 文本对齐：ORIG .rodata raw 逐字节 =
//      `static bool GlobalData::Init()`                    @0x08c1b1c0 size 0x1f
//      `static bool GlobalData::initTournamentDB(const char*)` @0x08c1b100 size 0x36
//      `static bool GlobalData::setCurrentPacketPool(THREAD_TYPE, pthread_t)` @0x08c1b0a0 size 0x45
//      （改前我方 0x18/0x2f/0x3e，恰缺 `static ` 7 字节）；
//   ② 符号名/mangled 零变化（namespace 成员与 class 静态成员同 mangling）：
//      GlobalData.cpp .o `nm --defined-only | comm -3` = 0（4395=4395）、undefined 589=589；
//   ③ 12 处 `namespace GlobalData { extern ... }` 重开点按「唯一声明点」合并（mangled/ABI 不变）：
//      CInventory.h / CUserCharacInfo.h 与 CInventory.cpp / CGameManager.cpp /
//      CPremiumLetheManager.cpp / CRelayBattleMgr.cpp / CItemShop.cpp → include 本头；
//      CBoosterGage.cpp / CDisconnectDetecter.cpp / CExpandEquipslot.cpp /
//      CSharedServerMessageManager.cpp 因本 TU 局部镜像类（StreamPool/CStreamGuard/
//      CSystemTime/Stream）与权威头重定义冲突，保留最小值域 extern 声明（符号与 ABI 一致）；
//      W6_SecMiscDeps.cpp 的 `using namespace GlobalData;` 改为 `GlobalData::` 限定（GCC 4.4
//      不支持类成员 using-declaration，探针实证）——10 处引用，符号/语义零变化；
//   ④ 逐 TU A/B（唯一变量 = 本头形态）：10 个代表 TU 四数改前=改后逐值相同
//      （GlobalData.cpp 877/1719/9/35/767、CUser.cpp 809/1728/5/58/586 等）。
class GlobalData
{
public:
// [Q3PrettSweepA 2026-09-11 登记的「评估后不修」项已于 2026-09-13 由 R18gd 落修，见上方判词。]
    static bool Init();                          // 0x08299fa0
    static void Fini();                          // 0x0829e0b2
    static bool InitCaptcha();                   // 0x0829d438
    static void GenerateCaptchaData();           // 0x0829d4b0
    static bool initTournamentDB(const char*);   // 0x0829dd82
    static bool setCurrentPacketPool(THREAD_TYPE type, unsigned long threadID);
                                      // 0x0829e4c4

// ---- 全局状态 ----
    static bool s_server_stop;                                   // 0940bd64 (1)
    static MsgQueueMgr* s_msgq_mgr;                              // 0940bd68
    static StreamPool* s_stream_pool;                            // 0940bd6c
    static PacketPool* s_packet_pool_list[9];                    // 0940bd80
    static BigStreamPool* s_big_stream_pool;                     // 0940bda4
    static SmallStreamPool* s_small_stream_pool;                 // 0940bda8
    static DBMgr* s_db_mgr;                                      // 0940bdac
    static DBThread* s_db_thr;                                   // 0940bdb0
    static DBThread* s_db_thr_for_logDB;                         // 0940bdb4
    static DispatchThread* s_dispatch_thr;                       // 0940bdb8
    static NetworkThread* s_net_thr;                             // 0940bdbc
    static UdpThread* s_udp_thr;                                 // 0940bdc0
    static AcceptThread* s_accept_thr;                           // 0940bdc4
    static MonitorTcpThread* s_monitor_tcp_thr;                  // 0940bdc8
    static Mutex s_accept_lock;                                  // 0940bdcc (0x18)
    static std::queue<CUser*, std::deque<CUser*> > s_accept_que; // 0940be00 (0x28)
    static CServerProxyMgr<CMonitorServerProxy>* s_monitor_proxy_mgr;   // 0940be28
    static CServerProxyMgr<CGuildServerProxy>* s_guild_proxy_mgr;       // 0940be2c
    static CServerProxyMgr<CStatisticServerProxy>* s_statistic_proxy_mgr; // 0940be30
    static CHadesServerProxy* s_hades_proxy;                     // 0940be34
    static CDoubleConnCheckServerProxy* s_double_check_proxy;    // 0940be38
    static CSchoolServerProxy* s_school_proxy;                   // 0940be3c
    static CPCRoomServerProxy* s_pcroom_proxy;                   // 0940be40
    static CCommunityServerProxy* s_community_proxy;             // 0940be44
    static CAuctionServerProxy* s_auction_proxy;                 // 0940be48
    static CCeraAuctionServerProxy* s_cera_auction_proxy;        // 0940be4c
    static CPowerManager* s_power_manager;                       // 0940be50
    static secretshop::CSecretShop* s_secret_shop;               // 0940be54
    static CFatigueBatteryHandle* s_fatigueBatteryHandle_;       // 0940be58
    static CmmChannelProxy* s_channel_proxy;                     // 0940be5c
    static CRijndael* s_pcryptRijndael_CharacView_;              // 0940be60
    static CRijndael* s_pcryptRijndael_Password_;                // 0940be64
    static CRijndael* s_pcryptRijndael_MousePassword_;           // 0940be68
struct SecuCipherKeyRec  // ORIG BSS 0x940be80 起 16 字节记录（0x940be80..0x941f700 = 0x4e20×0x10）
{
    Cipher* cipher;  // +0x00
    int keyId;       // +0x04（Sanicova::CPad::GetKeyIdx(0) 读）
    int tblId;       // +0x08（Sanicova::CPad::GetKeyIdx(1) 读）
    char m_padC[4];  // +0x0c
};
    static SecuCipherKeyRec s_secu_cipher_keystring_manager_[0x4e20];  // 0940be80
    static CTEA* s_pcryptTEA_MousePassword_;                     // 0941f700
    static CTEA* s_pcryptTEA_SecurityCard_;                      // 0941f704
    static WongWork::CSimpleSSO* s_psimpleSSO;                   // 0941f708
    static WongWork::CSecurityCardCenter* s_securityCardCenter;  // 0941f70c
    static WongWork::CGMAccounts* s_pGMAccounts_;                // 0941f710
    static CSystemTime s_systemTime_;                            // 0941f714 (0x14)
    static XNuclear::CHades_GameSvr s_hades_;                    // 0941f728 (8)
    static CEventManager* s_event_manager;                       // 0941f730
    static WongWork::IPG::CIPGHelper* s_pIPGHelper;              // 0941f734
    static InstanceRentalSystem* s_rentalSystem;                 // 0941f738
    static RefPvpGrade* s_ref_pvp_grade;                         // 0941f73c
    static WongWork::CLogGameChannel* s_pLogGameChannel;         // 0941f740
    static WongWork::CCeraShop* s_pCeraShop;                     // 0941f744
    static CDBConnector* s_db_connector;                         // 0941f748
    static void* s_pCompoundAvatar;                              // 0941f74c
    static void* s_pCompoundBindCube;                            // 0941f750
    static void* s_pTowerOfDespairMgr;                           // 0941f754
    static Event_Upgrade_Jar* s_pEvent_UpGrade_Jar;              // 0941f758
    static CGM_Manager* s_GM_Manager;                            // 0941f75c
    static ItemVendingMachine* s_pItemVendingMachine;            // 0941f760
    static CStatisticMgr* s_statisticMgr;                        // 0941f764
    static expert_job::CExpertJobMgr* s_ExpertJobMgr;            // 0941f768
    static break_away_prevent::CBreakAwayPreventSystem* s_BreakAwaySys; // 0941f76c
    static Gm_List* gmList;                                      // 0941f770
    static village_object::CVillageObjectMgr* s_villageObjectMgr;// 0941f774
    static game_master::CGameMasterMgr* s_GameMasterMgr;         // 0941f778
    static village_attacked::CVillageMonsterMgr* s_villageMonsterMgr; // 0941f77c
    static village_attacked::CRevengeDungeon* s_revengeDungeonMgr;    // 0941f780
    static CDailyScheduleManager* s_DailyScheduleManager;        // 0941f784
    static online_preliminary::COnlinePreliminaryTeamMgr* s_onlinePreliminaryTeamMgr; // 0941f788
    static WongWork::CCapchaDataManager s_capchaDataManager;     // 0941f7a0 (0x28)
    static Secu_DataControl* s_pSecuDataControl;                 // 0941f7c8
    static CSecu_ProtectionField* s_pSecuProtectionField;        // 0941f7cc
    static WongWork::CAutoPunishRuleBlackIPMgr* g_autoPunishRuleBlackIPMgr; // 0941f7d0
    static WongWork::CAutoPunishRuleHackTypeMgr* g_autoPunishRuleHackTypeMgr; // 0941f7d4
    static WongWork::CBlackIPMonitor* g_blackIPMonitor;          // 0941f7d8
    static unsigned int g_IPMonitorLastDBAccessTime;             // 0941f7dc
    static CBusinessImpl* s_antibotBusinessImpl_;                // 0941f7e0
    static void* s_antibotChecker_;                              // 0941f7e4
    static CItemAmplifier* s_itemAmplifier_;                     // 0941f7e8
    static CGlobalEffectManager* s_globalEffectManager_;         // 0941f7ec
    static ARAD::Arad_ServerStateManager* s_serverStateManager_; // 0941f7f0
    static CTimeGate* s_timeGate_;                               // 0941f7f4
    static unsigned int g_GoldControlDateFlag;                   // 0941f7f8
    static StackableAction::Action* g_Actions;                   // 0941f7fc
    static expert_extraction::CExtractionMgr* g_ExtractionMgr;   // 0941f800
    static CaptchaGen cg_;                                       // 0941f804 (0x14)
    static void* g_pTeleport[2];                                 // 0941f818 (8)
    static random_option::CRandomOptionItemHandle* g_randomOptionHandle; // 0941f820
    static void* s_pAvatarRoulette;                              // 0941f824
};

// g_SPremiumInfo 为全局 C 名（ORIG B 0x09483a40；无命名空间），声明见 SPremiumInfo.h。
extern SPremiumInfo g_SPremiumInfo[0x6a];                    // 09483a40 (0x7740)

// ---- W6_SecMiscDeps 批追加（仅 append，不改既有布局）----
#include "CRijndael.h"          // CSecurityCardCenter 基类（ORIG +0x0 子对象 0x47c）
#include <list>

namespace WongWork
{
// WongWork::CSecurityCardCenter —— 安全卡服务中心。ORIG 布局：CRijndael@0x0
// sizeof 0x47c、list<unsigned char> m_characterSet @0x47c、ushort m_timeCount
// @0x484。ctor 0x082a6654 / init 0x085fe05e；其余方法定义 W6_SecMiscDeps.cpp。
// [b28d-solo-W6_SecMiscDeps] CRijndael 是**成员子对象**而非基类：ORIG nm 无
// _ZTV/_ZTIN8WongWork19CSecurityCardCenterE、无 D0/D1/D2；ORIG C1 0x082a6654 调
// _ZN9CRijndaelC1Ev（非 C2）且全程无派生类 vptr 写（CRijndael::C1 0x080b60ac
// 内部写 _ZTV9CRijndael+8）。布局/sizeof 0x488/mangled 全不变。
class CSecurityCardCenter
{
public:
    CSecurityCardCenter();
    void init();                                          // ORIG T 0x085fe05e
    char* encryptString(char const* src, char* out);      // ORIG T 0x085fe2e2
    char* generateCertKey(char* out);                     // ORIG T 0x085fe17e
    void _updateTimeCount();                              // ORIG T 0x085fe026
    char* generateServerKey(char* out);                   // ORIG T 0x085fe0ec
    bool checkPhoneNumber(char const* phone) const;       // ORIG T 0x085fe444
    char* generateCardData(char* out) const;              // ORIG T 0x085fe1ba

private:
    CRijndael m_cipher;                        // +0x0（ORIG C1 调 CRijndael::C1 的成员子对象）
    std::list<unsigned char> m_characterSet;   // +0x47c（init push_back 42 字符）
public:
    unsigned short m_timeCount;                // +0x484（_updateTimeCount；init 置 1）
};

// WongWork::CWebEvent —— Web 事件静态服务（m_strEventName D 0x093ace60，
// 10 槽×0x12；m_strEventIPGCode D 0x093acf20，10 槽×0xb）。
class CWebEvent
{
public:
    static unsigned int RecvInfoFromDB(CUser* user, unsigned int itemId,     // ORIG T 0x086c121e
                                       unsigned int count, unsigned int eventIdx,
                                       unsigned int extra);
    static bool RecvInfoFromWeb(unsigned int p1, unsigned int p2,           // ORIG T 0x086c11d0（透传 ReqSaveEventItem）
                                unsigned int p3, unsigned int p4,
                                unsigned int p5, unsigned int p6);
    static bool ReqSaveEventItem(unsigned int p1, unsigned int p2,           // ORIG T 0x086c1910
                                 unsigned int p3, unsigned int p4,
                                 unsigned int p5, unsigned int p6, bool last);
    static int GetItemType(int eventKind, int itemNo);                       // ORIG T 0x086c1aba（定义 LinkClosureB_Misc.cpp）
    static int _GetATIAvatarAbility(int itemIdx);                            // ORIG T 0x086c1a2e
    static char const* _GetEventString(int idx);                             // ORIG T 0x086c1ba8
    static char const* _GetEventIPGCode(int idx);                            // ORIG T 0x086c1bd4

    static char m_strEventName[10][0x12];        // _ZN8WongWork9CWebEvent14m_strEventNameE
    static char m_strEventIPGCode[10 * 0xb];        // _ZN8WongWork9CWebEvent17m_strEventIPGCodeE（一维，ORIG 0x86c1bd4 movzbl disp 证实）
};

} // namespace WongWork

#endif  // GAME_GLOBALDATA_H_

