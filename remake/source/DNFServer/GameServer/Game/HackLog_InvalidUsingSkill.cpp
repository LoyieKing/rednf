// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 DisPatcherCore.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN26CHackLog_InvalidUsingSkill17IsValidUsingSkillEPK25SKILL_USING_VALIDATE_INFO
//   ORIG TU     ：HackLog_InvalidUsingSkill.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include <string.h>               // memset/strncpy/strlen
#include "DisPatcherCore.h"
#include "CUser.h"
#include "CParty.h"
#include "GameWorld.h"
#include "CGameManager.h"
#include "CDataManager.h"         // G_CDataManager()/m_serverParameter（serviceOfShusia 计价）
#include "ServiceRestrictManager.h"
#include "Arad_DataManager.h"     // ARAD::Singleton<ServiceRestrictManager>::Get
#include "CInventory.h"           // use_money/GetInstanceValueStatistic/VALUE_STATISTIC_FIELD
#include "Statistics.h"           // Statistics::UseShusiaService
#include "CAssaultMgr.h"          // pvp_assault::GetInstanceAssaultMgr
#include "CompoundEmblemDeps.h"   // LineFunc 权威声明
#include "CUserProc.h"            // APSystem::CUserProc::ClearActionAndSendtoUser（UseCoin）
#include "CStreamGuard.h"         // CStreamGuard（G2 角色域 process 使用）
#include "StreamPool.h"           // StreamPool::Free(CStreamGuard&)/BigStreamPool
#include "MsgQueueMgr.h"          // MsgQueueMgr::put(DB_Q, guard)
#include "GlobalData.h"           // s_pGMAccounts_/s_big_stream_pool/s_stream_pool/
#include "PacketGuard.h"          // PacketGuard/InterfacePacketBuf（回滚/通知包）
#include "CDungeon.h"             // CDungeon 完整类型（find_dungeon 字段访问）
#include "CWorldMap.h"            // CWorldMap::hasDeathTower
#include "CCubeStatistic.h"       // GetInstanceCubeStatistic/collectCubeStatistics
#include "HeroMissionEvent.h"     // ARAD::Singleton<HeroMissionEvent>::Get/processMission
#include "CSystemTime.h"          // CSystemTime::getCurTickCount（exercise_mode）
#include "WarField.h"             // stMapMonsterKillChecker_t（死亡塔 +0xb58 视图）
#include "PvP_Room.h"             // PvP_Room 完整类型（PvPTimeout::process 调 time_out）
#include "SigTypes.h"             // SIG_LOAD_*/SIG_CREATE_CHARAC 权威布局（RequestDB*/makeGift）
#include "TimerQueue.h"          // G_TimerQueue/InsertTimer/TIMER_MESSAGE_0x51（RegistTimerSchedules）
#include "CItem.h"                // CItem::make_item（makeGift 虚调用需完整类型）
#include "HistoryLog.h"          // HistoryLog::WriteBuyItem（BuyItem::process）
#include "CGuildServerProxy.h"   // CGuildServerProxy::OnBuyItem（BuyItem::process）
#include "CRestrictCharacCreationEvent.h"  // getLimitCreateCharacter（check_error 消费）
#include "CStatisticServerProxy.h"  // statistc_proxy::add 发送代理
#include "../Statics/StatisticProxy.h"  // StatisticsPacket 权威布局
#include <stdarg.h>              // va_list（statistc_proxy::add 可变参）
#include "CSkill.h"              // CSkill::m_fitGrowType/IsStealable（UseSkill 黑客校验）
#include "CSwitchLog.h"           // CSwitchLog（PvP 加入日志）
#include "Secu_HackLogCheckByParty.h"  // MSG_MONSTER_DIE 完整布局 + Secu_HackLogCheck
#include "CHackLog_OverDamage.h"  // 过度伤害限值表（DieMob 反作弊链）
#include "CDeathTower.h"          // CDeathTower::handleKillMonster（DieMob state10）
#include "DisJoint.h"            // DisJoint/DisJointResult（DisJointItem::_disjoint）
#include "CExpertJob.h"          // expert_job::CExpertJob/CDisjointer（_disjoint 依赖）
#include "CSpecialItemRoutingManager.h"  // ProcessFailEtc（MoveMap::process 路由失败转移）
#include "CBattle_Field.h"          // check_start_point（CheckOneMapPlayTimeHackCnt
#include "BossTowerFunc.h"        // Global::CBossTowerFunc 权威头
#include "SkillSlot.h"            // addSkillOnCreateCharacter/_Mastered_skill
#include "APSystemTypes.h"        // APSystem::CDBAndExchangeServerProc
#include "CMonitorServerProxy.h"  // SendTcpPacket（SelectCharac 监视包）
#include "CServerProxyMgr.h"      // CServerProxyMgr<CMonitorServerProxy>::GetServerProxy
#include "../Monitor/LimitNpcBuyItem.h"  // LimitNpcBuyItemInfoAll（监视包布局）
#include "CDisconnectDetecter.h"   // disconnect_detecter（PvpHeartBeat）
#include "CPremiumLetheManager.h"  // ResetSkillReq（OneDayLetheTicket）
#include "Packet_Notice_Guild_War_Info.h"            // shared/packet/include
#include "Packet_Disable_User_OneToOneChat_Police.h" // shared/packet/include
#include "ST_Guild_War_Rank_Info.h"                   // Packet_Notice ctor 依赖（StubS3C）
#include "Packet_Monitor_Del_Buddy.h" // shared/packet/include（RemoveFriend 监视包）
#include "CSyncSlangFilter.h"                    // 敏感词过滤（SetPartyInfo check_error）
#include "TaiwanQueries.h"                       // Login::dispatch_sig 查询族
#include "TaiwanCash.h"                          // Cera/RentAvatar IPG helper
#include "AvatarRechargeServer.h"                // MoveItem::send 充值服
#include "CHackAnalyzer.h"                       // SellItem::dispatch_sig 反作弊链
#include "Packet_Party_Result_Statistic.h"       // shared/packet/include（UdpCharacteristic）
#include "Packet_Party_Ping_Statistic.h"         // shared/packet/include（UdpCharacteristic 三 ping，wave2）
#include "Packet_Pvp_Ping_Statistic.h"           // shared/packet/include（wave2）
#include "Packet_Fair_Pvp_Ping_Statistic.h"      // shared/packet/include（wave2）
#include "Packet_Abnormal_Exit_Statistic.h"      // shared/packet/include（UdpCharacteristic）
#include "RestrictNameDeps.h"                     // restrict_inputting_name::isUtf8StrInUnicodeRange（CreateCharac）
#include "AuctionProxy.h"                        // CAuctionServerProxy/CCeraAuctionServerProxy（Login auction 状态包）
#include "DB_InterDeps.h"                        // Exit::dispatch_sig DB makeRequest 声明
#include "Arad_EventPeriodDataManager.h"         // Exit::dispatch_sig delUserRewardData
#include "CPrivateStoreMgr.h"                    // private_store::GetInstancePrivateStoreMgr（Exit）
#include "CTimeGate.h"                            // s_timeGate_ 完整类型（QuestAction）
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PvP_Room.h"
#include "CParty.h"
#include "CGameManager.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "CSwitchLog.h"
#include "CPrivateStoreMgr.h"
#include <cstring>
#include "CPrivateStoreMgr.h"  // private_store::GetInstancePrivateStoreMgr
#include "Dispatchers.h"         // Dispatcher_AddSocketToAvatar 类声明
#include "Inven_Item.h"          // Inven_Item 完整类型
#include "CEquipItem.h"          // CEquipItem::{check_eType,getAvatarSocket}
#include "Packet_Monitor_Other_Channel_Chat.h"   // shared/packet/include 监视包权威头
#include "CCommunityServerProxy.h"              // SendTcpPacket（Community 监视包）
#include "DB_AccountCargoSync.h"                // DB_InsertPoliceSaveChatting::makeRequest
#include "RestrictNameDeps.h"
#include "DB_ReqChangeCharacName.h"
#include "GameTypes.h"            // cMyTrace
#include "CGM_Manager.h"
#include "ProxySecurityDeps.h"
#include "PacketHeader.h"
#include "Packet_Monitor_MegaPhone_Hyper_Link.h"

struct SKILL_USING_VALIDATE_INFO
{
    int m_job;               // +0x00
    unsigned char m_slot;    // +0x04
    char m_firstGrowType;    // +0x05
    char m_secondGrowType;   // +0x06

    SKILL_USING_VALIDATE_INFO(int job, unsigned char slot,
                              char firstGrowType, char secondGrowType)
    {   // ORIG 0x08234756：四成员顺序赋值
        m_job = job;
        m_slot = slot;
        m_firstGrowType = firstGrowType;
        m_secondGrowType = secondGrowType;
    }
};


class CHackLog_InvalidUsingSkill
{
public:
    static bool IsValidUsingSkill(const SKILL_USING_VALIDATE_INFO* info);   // 0x082864fc
    static bool IsSkillMatchFirstGrowType(const CSkill* skill, char type);  // 0x082865ca
    static bool IsSkillMatchSecondGrowType(const CSkill* skill, char a, char b);  // 0x08286612
    static int GetHackType();                                               // 0x08286656
};

#include "HackLog_InvalidUsingSkill/IsValidUsingSkill.inc"
