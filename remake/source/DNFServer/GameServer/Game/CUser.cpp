#include "CEventScriptMng.h"   // EventClassify::CEventScriptMng（FatigueUp/set_server_fatigue_count）
#include "GameRand.h"
// ============================================================================
// df_game_r 还原 —— CUser（G1-3 批次，在线用户会话聚合根）
// 布局见 CUser.h；本文件按 ORIG 反汇编（docs/class_func_reports/CUser.md）
// 逐函数转录，identical 口径验证：source/toolchain/check_tu_game_orig.sh。
// ============================================================================

#include <string.h>
#include <cstdarg>
#include "TimeLog_.h"      // CUser::LogHistory（ORIG 0x0867c8d8 时间头）
#include "cHistoryTrace.h" // CUser::LogHistory 日志输出
#include "CSwitchLog.h"   // CUser::SetPvpIndex（ORIG 0x08669ee8 pvp@log）
#include "HistoryLog.h"  // CUser::master_new_skill WriteLearnSkill（ORIG 0x0866b4d7）
#include "CUser.h"
#include "W6_HackPunishDeps.h"   // onUpdateHackCount：CAutoPunishRuleHackTypeMgr::checkMaxHackCntOver / WongWork::HackTypeInfo_t

#include "CUser.h"
#include "MoneyLogPacket.h"   // CUser::SendMoenyLog（ORIG 0x0868f472 载荷 0x1e）
#include "Packet_Notice_Guild_War_Point_Change.h"   // 权威类（原局部镜像已删）
#include "SigTypes.h"        // SIG_LOAD_ETC/SIG_LOAD_DUNGEON_CLEAR/SIG_REQ_BLACK_COUNT/SIG_ATTENDANCE_EVENT 完整布局
#include "CDungeonClear.h"   // WongWork::CDungeonClear::toString
#include "CCharacList.h"          // CCharacList::m_characs（calc_lev_stat 角色表）
#include "CharacManageScript.h"   // CharacManageScript 完整类型（管理等级链）
#include "GameResultType.h"  // GameResultType（GetRecord 序列化 0x50/元素）
#include "CTitleBook.h"   // item_lock::CItemLock / stItemLockRef 权威定义
#include "CGameManager.h"
#include "CInventory.h"
#include "CPrivateStoreMgr.h"   // b22：private_store 权威头（局部影子类退役，
                                //        deleteSpecificItem 转真 RemovePrivateStore）
#include "DB_LogBuyItem.h"   // DB_LogBuyItem 权威声明（P104 统一：makeRequest 定义本文件）
#include "APSystemTypes.h"
#include "GameBasicsScripts.h"
#include "CParty.h"
#include "SkillSlot.h"
#include "CSkill.h"       // master_new_skills 成长展开 get_group（ORIG 866b9c0）
#include "WarRoom.h"
#include "WarRoom.h"
#include "PvP_Room.h"   // add_pvp_result/update_pvp_point 房间查询（get_user_seat 等）
#include "SyncScriptDeps.h"   // toTString（RewardItem2DeleteInvalidItem 字符串表翻译）
#include "PacketBuf.h"
#include "GameTypes.h"
#include "CCapchaDataManager.h"  // WongWork::CCapchaDataManager（GlobalData::s_capchaDataManager）
#include "CDungeon.h"          // CDungeon::m_89e（exceptedStateMoneyLog 搬运副本标记）
#include "DBInsertGiveBoxEventReward.h"  // GiveMeBoxReward 转发（ORIG 0x08693758）
#include "Packet_Monitor_Increase_Guild_Exp.h"  // WorkPerFiveMin 公会上报包（0x40e,0x1c）
#include "CMonitorServerProxy.h"  // SendNoticeCharLive（WorkPerTenMin）
#include "CGuildServerProxy.h"    // SendPacket（WorkPerFiveMin）
#include "InterfacePacketBuf.h"
#include "cMyTraceNoop.h"  // SendCmdErrorPacket PAY_TYPE 命中 trace（ORIG 0x867c0ad/0x867c0c8）
#include "CExpertJob.h"              // charac_expand::CDataMgr slot0 CCharacExpertJob（StubS3A ctor）
#include "CExpertJobList.h"          // STExpertJobEtcScript（SetCharacLevel 专家职业经验）
#include "nexon_packet.h"           // stLoginPacket（SendLogin/LogoutToPCRoomServer）
#include "PCRoomServerProxy.h"     // CPCRoomServerProxy::SendTcpPacket（PC 房登录/登出通知）
#include "W9_EventSmall.h"           // slot1 CCharacBloodDungeon
#include "CPvPLiveEventData.h"       // slot3 CPvPLiveEventData
#include "Redeem_Item__CRedeemItem.h"// slot6 Redeem_Item::CRedeemItem
#include "OnlinePreliminary.h"       // slot4/5 online_preliminary 成员
#include "CLeagueMatch.h"            // CLeagueMatch::GetPlayCountPerOneTeam（make_basic_info 校队块）
#include "CFairPvPScore.h"           // slot7 fair_pvp::CFairPvPScore
#include "CMissionList_Charac.h"     // slot8 CMissionList_Charac
#include "CUserCharacInfo.h"         // CExpandEquipslot/CTitleBook 完整类型
#include "CExpandEquipslot.h"        // CExpandEquipslot 权威虚表（getData @ vtable+8；GetDataFn 桥真实化）
#include "DataManagerInitDepends.h"  // TOD_UserState
#include "CQuestShop.h"              // CQuestShop
#include "CKillMonsterInfo.h"        // CKillMonsterInfo
#include "CAchievement.h"            // CAchievement
#include "CItemDictionary.h"         // CItemDictionary
#include "CBoosterGage.h"            // CBoosterGage
#include "CBurningFatigueEvent.h"     // FatigueUp 燃烧疲劳 getRefillFatigue（ORIG 0x810a166）
#include "CCharacterDayEvent.h"       // FatigueUp 角色日事件 GetEventJob（ORIG 0x8697886）
#include "AvatarRechargeServer.h"     // FatigueUp CalcDurability（ORIG 0x819049a）
#include "BestClearTime.h"           // BestClearTime
#include "CCharacInformNotice.h"     // CCharacInformNotice
#include "CItemMakingSkill.h"        // CItemMakingSkill（若存在）
#include "GameWorld.h"
#include "LogManager.h"
#include "CStatisticServerProxy.h"
#include "RDARScriptStringManager.h"   // g_scriptStringManager_.findString
#include "CMyFileLog.h"   // InsertOnTimeEventReward OnTimeEvent 日志
#include "CItemAmplifier.h"
#include "CMonitorServerProxy.h"
#include "CServerProxyMgr.h"
#include "GlobalData.h"
#include "Arad_DataManager.h"    // ARAD::Singleton<Arad_DataManager>（ReturnToSelectCharacList）
#include "Arad_EventPeriodDataManager.h"  // ARAD::Arad_EventPeriodDataManager::delUserRewardData
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn (ENUM_INTERNALPACKET_JPN)
#include "ARAD_Database.h"           // ARAD::DATABASE::sourceCargoSize（仓库快照长度）
#include "TaiwanCash.h"             // WongWork::IPG::CIPGHelper 虚接口（DoGetCeraPointItem）
#include "CExpertJob.h"          // expert_job::CExpertJobMgr::OnLeaveUser
#include "CHandlePremium.h"      // WongWork::CHandlePremium（handleReturn2CharacView）
#include "W8LT_DBTask.h"        // DB_UpdateSchoolPoint（RecoverFatigue 学校点落库）
#include "DB_InterDeps.h"       // DB_UpdateCharacPlayTime（ReturnToSelectCharacList）
#include "COnTimeEvent.h"       // COnTimeEvent::delete_reward_user（delete_ontime_reward_list）
#include "MoreTypes.h"         // arad::SigAradEventItemLog（SendAradEventItemLog）
#include "CPowerManager.h"    // s_power_manager 完整类型（make_basic_info 排名查询）
#include "AuctionProxy.h"    // CAuctionServerProxy/CCeraAuctionServerProxy 完整类型（IsRunning）
#include "CDeathTowerRanking.h"
#include "Packet_Register_GM_MID.h"
#include "Packet_Randombox_statistic.h"  // CUser::SendRandomBoxStatistic（ORIG 0x08676826）
#include "Packet_Event_UpgradeJar_Output_Statistic_GTS.h"  // SendEventJarOutputItemStatistics
#include "CConditionEventManager.h"
#include "CCoinEventPerDay.h"     // CCoinEventPerDay（GetCoinNoPerDay 权威实现）

#include "CAssaultMgr.h"
#include "CMissionList_Charac.h" // CMissionList_Charac 任务列表查询（charac_expand 槽位 8）
#include "RefPvpGrade.h"        // RefPvpGrade::GetPvpCurrRankPoint/GetPvpNextRankPoint
#include "QuestClear.h"         // WongWork::CQuestClear::getClearedQuest（UserQuest +0x04）
#include "CUserGlobalInfoHandle.h"  // CUserGlobalInfoHandleInstance/reset_uniqueid_flag
#include "CBoosterGage.h"       // charac_expand::CData（CDataMgr 20 槽 ResetDaily 循环）
#include "STExpertJobScript.h"  // 权威 STExpertJobScript（isBoundaryExpValue/GetLevel）
#include "Redeem_Item__CRedeemItem.h"  // Redeem_Item::CRedeemItem（isAddableFilter/AddRedeemList）
#include <algorithm>
#include <iostream>
#include "CQuestShop.h"          // set_grow_type 清 QP
#include "Quest.h"               // Quest/MonsterKillItem（CheckQuestMonster）
#include "CUserProc.h"           // APSystem::_ActionGroupIndex / CUserProc
#include "HeroMissionEvent.h"
#include "ServiceRestrictManager.h"     // ARAD::Singleton<HeroMissionEvent>
#include "EpollHandler.h"        // G_EpollHandler
#include "MsgQueueMgr.h"         // s_msgq_mgr->put
#include "CStreamGuard.h"                  // CStreamGuard（SendAradEventItemLog）
#include "StreamPool.h"                    // GlobalData::s_stream_pool->Acquire
#include "Arad_StatisticsEventItemList.h"  // Arad_DataManager Singleton/needStatistics
// SIG_* 布局经 CDoubleConnCheckServerProxy.h → SigTypes.h 可见。
// ORIG T 0x08655327（自由函数）
void SetUserManagerToolIP(CUser* user, SIG_UPDATE_LOGOUT* sig);
#include "CDoubleConnCheckServerProxy.h"
// DisConnSig / prepareDisconnect 统计与退出落库依赖
#include "CSystemTime.h"                    // GlobalData::s_systemTime_::getCurSec
#include "Packet_User_Ting_TimeCheck_Statistic_Add.h"  // DisConnSig Ting 时段统计包
#include "Packet_Client_Spec_Statistic.h"   // DisConnSig HWSpec 统计包
#include "DBUpdateExitTime.h"               // prepareDisconnect 退出时间落库
#include "TaiwanQueries.h"                  // prepareDisconnect GarenaAuthData::getTimeStamp
#include "CEnvironment.h"                   // prepareDisconnect getProcessSequence
#include "Dispatchers.h"                    // prepareDisconnect Dispatcher_SecurityCardAuthCancel
class DB_ForceSaveSimpleSSO { public: static void makeRequest(unsigned int, int); };  // ORIG T 0x084281ae（定义 W7LinkDB.cpp）

// stBuySkillInfo 完整定义（与 CEventCharacterHandler.cpp 一致；CUser.h 仅前向声明）
struct stBuySkillInfo
{
    char m_field0;      // +0x00
    char m_skillIdx;    // +0x01
    char m_count;       // +0x02
};


// g_ServerString_（ORIG 0x93fa660 B C 名；全局定义在 ServerCommon/ServerXml.cpp）
namespace np_server_xml
{
class CServerXml
{
public:
    std::string GetServerString(int nIndex, bool* pFlag) const;
};
}

extern np_server_xml::CServerXml g_ServerString_;


// ORIG W 0x082a70ab（自由函数）
class EpollHandler;
EpollHandler* G_EpollHandler();


// b22 类建模（Model22）：原「private_store 域最小前向声明」的规避理由
// （SkillSlot.h↔SigTypes.h 包含序冲突）经双序探针实测**已不存在**（两序
// 均 COMPILE OK，冲突注记过时）——改为 include 权威头 CPrivateStoreMgr.h
// （本文件头区），局部影子类删除（§4.4 唯一声明点）。

// CBattle_Field（本地最小视图；get_dungeon_index 定义于 CBattle_Field.cpp，
// ORIG 0x0822d0d8，符号 _ZN13CBattle_Field17get_dungeon_indexEv。完整类头
// CBattle_Field.h 与 WarField.h 的 map_item/map_monster 重复类型冲突，属
// FixMapMonsterReal/FixWarFieldTypes 域，此处仅声明所需方法）
#ifndef DNF_GAME_CBATTLE_FIELD_H_
class CBattle_Field
{
public:
    int get_dungeon_index();
    char m_pad[0x128];            // +0x00..+0x127
    short m_villageMonsterId;     // +0x128（exceptedStateMoneyLog 读取，完整类 CBattle_Field.h:421）
};
#endif  // DNF_GAME_CBATTLE_FIELD_H_（batch-16：CParty.h 折叠后 BF 完整头可达，占位类需让位）


// ---- 外部符号声明（对应 TU 翻译后移除） ----
extern unsigned char _NS_PI_2ND_GetDefaultRandomHashKey();

extern char* GetPacketName(ENUM_PACKETCLASS packetClass, unsigned short packetId);   // ORIG T 0x082a2112

extern bool IsLightServer();   // ORIG 0x0822ad44 W（bool 实证：giveup_panalty 0x086786cb test %al 无 setne）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08668823 | _Z29_postCheckForceChangeGrowTypeP5CUserii
extern void _postCheckForceChangeGrowType(CUser* user, int firstGrow, int secondGrow);  // 0x08668823 T
extern void _postCheckForceChangeGrowType4AheadQuest(CUser* user, Quest* quest);  // 0x086686ed T（定义 StubS3A_deps.cpp）


namespace ARAD
{
namespace DISPATCHER
{
void make_cmd_packetheader_jpn(PacketGuard& packet, ENUM_CMDPACKET cmd, bool flag);
}
}

namespace item_lock
{
}
// CItemLock 权威声明在 CTitleBook.h（item_lock::CItemLock）

// ---- G1-3 临时外部桩（对应 TU 翻译后移除） ----
// ORIG T 0x0808ca12：mov $0x7,%eax 直返 7
unsigned char _NS_PI_2ND_GetDefaultRandomHashKey() { return 7; }
// ORIG W 0x08695058：movl $0,0x10(%eax) —— 清 m_field10(+0x10)。
void WongWork::CMCAPManager::resetExposedCount() { m_field10 = 0; }
// 0x0864809c：antibot 校验/加密发送主链路（依据 AntibotSend.md 反编译；
// s_antibotChecker_ vtable +0x34 校验 / +0x28 加密，ORIG 栈缓冲此处以 vector 等价）。
extern void _NS_PI_2ND_SendHash_SC(char* data, unsigned short len, CUser* user); // ORIG T 0x082751ee
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864809c | _ZN5CUser11AntibotSendER9PacketBuf
bool CUser::AntibotSend(PacketBuf& packet)   // b28c-L5 SIGFIX：ORIG rodata 串
                                              // "bool CUser::AntibotSend(PacketBuf&)"（P174）
{
    // ORIG T 0x0864809c 全文转写（报告 AntibotSend.md Ghidra C+asm 双段读完）：
    // ①verify（vtable+0x34）先行，成功后才进加密缓冲段；②加密缓冲=**VLA +
    // guard.clear + memset 500+len**（0x864822b/245：orig memset@plt=1、无
    // vector——alloca 步进 `-(len+0x212)&0xfffffff0`）；③encrypt 返回 **非0=失败**
    // → trace(0x807)+encFailed，**返回0=成功** → put_binary/Send 后 return
    // （0x86483b0 早退，非我旧版 ==0 续跑）；④encrypt 调用实参求值序 get_len→
    // get_packet→getAntibotKey（右→左）；⑤trace1 的 key/len 在 C1 **之前**求值；
    // ⑥isEncryptRequired 缓存（encReq 真则二段不再调，0x864842b else 路）。
    unsigned short packetId =
        *(unsigned short*)(packet.get_packet(0) + 1);
    bool encReq = false;      // local_31
    bool encFailed = false;   // local_32
    if (packet.isEncryptRequired())
    {
        encReq = true;
        // §4.5 例外：s_antibotChecker_（GlobalData.h `void*`）承自外部
        // 【b22 终判·第三方】CreateAntiServerObj(ORIG 0x082a6539 W) 反汇编实证
        // = strlen/strncat/memcpy 拼路径 → dlopen → dlsym → call *%eax：checker
        // 对象来自运行时加载的外部 .so（反外挂 SDK），其 C++ 类/构造/vtable 均
        // 不在 df_game_r 编译单元内 ⇒ 本仓建模该类无从取证（未知槽的签名不可
        // 声明、§4.1 禁造），typedef 槽指针直调即 ORIG 行为的忠实表达——建模
        // 收益评估否决，此案终结。全族七槽用法同源：VerifyFn+0x34/EncryptFn+
        // 0x28/Fn3+0x08/Fn1+0x24/ReportFn+0x38/AddClient+0x04/P2PAddClient+
        // 0x20（本文件 :7746 与 PacketDispatcher/InterDispatcher 各点同判）。
        typedef int (*VerifyFn)(void*, int, unsigned short);   // vtable +0x34
        if ((*(VerifyFn*)((*(void***)GlobalData::s_antibotChecker_)[0x34 / 4]))(
                GlobalData::s_antibotChecker_, getAntibotKey(), packetId) != 0)
        {
            const int bufLen = packet.get_len() + 500;   // local_6c
            char buf[bufLen];                            // ORIG VLA（alloca 步进）
            int outLen = bufLen;                         // local_24
            PacketGuard guard;                           // local_68（块作用域，
                                                       // dtor 于早退前 0x86483b5）
            ((InterfacePacketBuf*)&guard)->clear();
            memset(buf, 0, bufLen);
            ((InterfacePacketBuf*)&guard)->put_header(0, 0x231);
            typedef int (*EncryptFn)(void*, int, char*, int, char*, int*);  // +0x28
            // 实参右→左求值：get_len→get_packet→getAntibotKey（P24/実参序）
            int r = (*(EncryptFn*)((*(void***)GlobalData::s_antibotChecker_)[0x28 / 4]))(
                GlobalData::s_antibotChecker_, getAntibotKey(),
                packet.get_packet(0), packet.get_len(), buf, &outLen);
            if (r != 0)
            {
                int tl = packet.get_len();      // trace 实参先于 C1（0x864832e/33b）
                int tk = getAntibotKey();
                cMyTrace trace("bool CUser::AntibotSend(PacketBuf&)", 0x807, 0);
                trace("[RecvBufferForEncrypt] result : %d, PacketID : %d, "
                      "Key : %d, Length : %d", r, (int)packetId, tk, tl);
                encFailed = true;
            }
            else
            {
                ((InterfacePacketBuf*)&guard)->put_binary(buf, bufLen);
                ((InterfacePacketBuf*)&guard)->finalize(false);
                return Send(guard);   // 0x8648313 起 return = Send 返回值透传
            }
        }
    }
    PacketBuf sendBuf;
    char* data = packet.get_packet(0);
    int len = packet.get_len();
    bool doEnc;
    if (encReq)
        doEnc = encFailed;
    else
        // ORIG 0x08648420：else 支 = isEncryptRequired()（encReq=false 时
        // encFailed 恒 false，ORIG 汇编无 encFailed 参与）。
        doEnc = (packet.isEncryptRequired() != 0);
    if (doEnc)
    {
        sendBuf.copy(packet);
        sendBuf.encrypt_packet();
        data = sendBuf.get_packet(0);
        len = sendBuf.get_len();
        sendBuf.encFinalize();
    }
    _NS_PI_2ND_SendHash_SC(data, (unsigned short)len, this);
    int sret = m_network.send_packet(data, len);
    if (sret < 0)
    {
        char* nm = (char*)getCurCharacName();     // 0x86484f7 先于 C1
        cMyTrace trace("bool CUser::AntibotSend(PacketBuf&)", 0x834, 0);
        trace("User %s - send_packet() fail length(ID:%d,LEN:%d),",
              nm, (int)packetId, len);
        if (packetId != 199)
            DisConnSig((DISCONN_SIG)6, false, 0);
        return 0;
    }
    return 1;
}

_Quest_Authen_Data::_Quest_Authen_Data() throw() { reset(); }  // throw()：ORIG UserQuest C1 0x8697b8a 无 EH pad
// ORIG reset 0x0814aa16 逐字段清：movw +0/+2、movl +8、movb +0xc、movl +0x10/+0x14
// （不动 +0x04/+0x18 起字段）；旧 memset 0x48 语义与形态均不符。
void _Quest_Authen_Data::reset()
{
    m_characNo = 0;
    m_characClass = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field14 = 0;
}
bool mission_list_is_clear(void* data) { return false; }

void CUser::_379::reset()
{
    m_field0 = 0;
    m_field1 = 0;
    // ORIG 0x86454e8-0x86454f5 单装载双 store = DImode 清零形；ll 成员直写复现（w2 probe 实证）。
    m_hackTradeGold = 0;
    m_hackTradeCnt = 0;
}

// ORIG C1 0x086940c6：仅 movb $0,(%eax)（清 m_pad[0] 首字节）。
STGuildAgitDBInfo::STGuildAgitDBInfo()
{
    m_pad[0] = 0;
}
void CUser::resetInformNoticeFlag()
{
    // ORIG memset(this+0x8ebb1, 0, 1)：只清当前通知 flag 首字节（m_informNoticeFlags[0]），
    // 不动其后的 15B（已保存的通知 flag 板）。
    memset(m_informNoticeFlags, 0, 1);
}

void CUser::disableSaveInformNoticeFlag()
{
    m_saveInformNoticeFlag = 0;   // ORIG 0x868fa6c 写 +0x8ebc1
}

void CUser::enableSaveInformNoticeFlag()
{
    m_saveInformNoticeFlag = 1;   // ORIG enableSave 写 +0x8ebc1
}

namespace WongWork
{
// WongWork::CDeathTowerRanking 权威声明见 CDeathTowerRanking.h（唯一声明点，
// 定义在 CDeathTower.cpp；本地同名类已删除，§7）。
}

namespace APSystem
{
// _SIG_LOAD_ACTION_POINT 权威定义见 CDataManager.h（经其传递包含），
// 本 TU 不再本地镜像；DB_UpdateActionPoint::makeRequest 仅指针使用。
class DB_UpdateActionPoint
{
public:
    // 与 ORIG/DB_AccountCargoSync.h 一致：int + const _SIG_LOAD_ACTION_POINT& + bool，
    // 符号 _ZN8APSystem20DB_UpdateActionPoint11makeRequestEiRKNS_22_SIG_LOAD_ACTION_POINTEb。
    static void makeRequest(int uid, const _SIG_LOAD_ACTION_POINT& data,
                            bool save);
};
}   // APSystem（完整声明见 CUserProc.h）

// ORIG T 0x0812887e _ZN13DB_LogBuyItem11makeRequestEjjjj20ENUM_DBLOG_ITEM_TYPE：
// characNo==0 早退；StreamPool Acquire("localchina/localchina_DBThread.cpp",0x59b)
// → CStreamGuard(stream,true) → 每条 << 前 call operator*（**guard）
// → <<0x251/<<-1/<<accId/<<characNo/<<itemIdx/<<type/<<value
// → NumberToString(accId,0) 独立求值 → cMyTrace(__PRETTY_FUNCTION__,0x5af,0)
// → trace(fmt,accStr,characNo,itemIdx,type,value) → s_msgq_mgr->put(LOGDB_Q,guard)。
void DB_LogBuyItem::makeRequest(unsigned int accId, unsigned int characNo,
                                unsigned int itemIdx, unsigned int type,
                                ENUM_DBLOG_ITEM_TYPE value)
{
    if (characNo == 0)
        return;
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                            "localchina/localchina_DBThread.cpp", 0x59b),
                        true);
    **guard << 0x251;
    **guard << -1;
    **guard << accId;
    **guard << characNo;
    **guard << itemIdx;
    **guard << type;
    **guard << (int)value;
    cMyTrace(__PRETTY_FUNCTION__, 0x5af, 0)(
        "DB_LogBuyItem::makeRequest m_id=%s nCharacNo=%d nItemID=%u nCount=%u nType=%d",
        NumberToString(accId, 0), characNo, itemIdx, type, value);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

class DB_SecurityCardUpdateCancelCnt
{
public:
    static void makeRequest(unsigned int accId, unsigned int cancelCnt);
};

class DB_PassPadUpdateCancelCnt
{
public:
    static void makeRequest(unsigned int accId, unsigned int cancelCnt,
                            char* webAddress);
};

namespace WongWork
{
// ORIG W 0x086959a8：movl $0x0,0x48(%eax) —— 清 +0x48。
void CSecurityCard::resetCancelCnt() { m_cancelCnt48 = 0; }
// ORIG W 0x0822ef4c：mov 0x48(%eax),%eax —— 直返 +0x48。
int CSecurityCard::getCancelCnt() { return m_cancelCnt48; }
}


// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x081b4300 | _ZN20UserMercenaryInfoMgr9AddCharacER21CHARAC_LOAD_MERCENARY
int UserMercenaryInfoMgr::AddCharac(CHARAC_LOAD_MERCENARY& info)
{
    // ORIG T 0x081b4300：未命中（CheckValidAppend==1）才添加；先置
    // m_avatarBonus=0xa，再按 8 个 avatar 槽物品取装备 grade 的最小值；
    // 非头像装备/非 CEquipItem 直接清 0。最后 push_back 返回 1。
    if (CheckValidAppend(info) != 1)
        return 0;
    info.m_avatarBonus = 0xa;
    for (int i = 0; i < 8; ++i)
    {
        CItem* item = G_CDataManager()->find_item(info.m_avatarItems[i]);
        CEquipItem* equip = (item == 0) ? 0 : dynamic_cast<CEquipItem*>(item);
        if (equip == 0 || !equip->IsAvatarItem())
        {
            info.m_avatarBonus = 0;
            break;
        }
        // [b23S3C] P48/P0：ORIG get_grade 双臂各重取一次（+0xb5/+0xc9，无 grade 槽）
        if (equip->get_grade() < static_cast<int>(static_cast<signed char>(info.m_avatarBonus)))
            info.m_avatarBonus = static_cast<unsigned char>(equip->get_grade());
    }
    m_characs.push_back(info);
    return 1;
}
// ORIG T 0x081b441a：按 characNo 线性查找并 erase（ORIG 经 FindIndexByCharacNo，
// 内联等价）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x081b441a | _ZN20UserMercenaryInfoMgr12RemoveCharacEj
void UserMercenaryInfoMgr::RemoveCharac(unsigned int characNo)
{
    for (size_t i = 0; i < m_characs.size(); ++i)
    {
        if ((unsigned int)m_characs[i].m_characNo == characNo)
        {
            m_characs.erase(m_characs.begin() + i);
            return;
        }
    }
}
using expert_job::CCharacExpertJob;

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0832a778 | _ZN13charac_expand8CDataMgrC1Ev
charac_expand::CDataMgr::CDataMgr()
{
    // slot0 +0x00：ORIG operator new(0x30) → memset 全块 → CCharacExpertJob::CCharacExpertJob()
    void* mem0 = ::operator new(0x30);
    memset(mem0, 0, 0x30);
    expert_job::CCharacExpertJob* expert =
        new (mem0) expert_job::CCharacExpertJob();   // placement ctor（ORIG memset 后 ctor）
    m_pad[0] = expert;

    // slot1 +0x04：operator new(0x2c) → memset → CCharacBloodDungeon
    void* mem1 = ::operator new(0x2c);
    memset(mem1, 0, 0x2c);
    CCharacBloodDungeon* blood = new (mem1) CCharacBloodDungeon();
    m_pad[1] = blood;

    // slot2..slot19：ORIG 纯 new + 默认 ctor（无 memset）
    m_pad[2] = new item_lock::CItemLock();            // new(0x58)
    m_pad[3] = new CPvPLiveEventData();                  // new(0xc)
    m_pad[4] =
        new online_preliminary::COnlinePreliminaryMember();                                          // new(0x14)
    m_pad[5] =
        new online_preliminary::COnlinePreliminaryMemberMatchList();                                 // new(0x20)
    m_pad[6] = new Redeem_Item::CRedeemItem();    // new(0x10)
    m_pad[7] = new fair_pvp::CFairPvPScore();      // new(0x5c)

    // slot8 +0x20：operator new(0x158) → memset 全块 → CMissionList_Charac
    void* mem8 = ::operator new(0x158);
    memset(mem8, 0, 0x158);
    CMissionList_Charac* mission = new (mem8) CMissionList_Charac();
    m_pad[8] = mission;

    m_pad[9] = new CExpandEquipslot();                    // new(0x89c)
    m_pad[10] = new CItemMakingSkill();                    // new(0x20)
    m_pad[11] = new CQuestShop();                                // new(0x48)
    m_pad[12] = new CKillMonsterInfo();                    // new(0x50)
    m_pad[13] = new TOD_UserState();                          // new(0x20)
    m_pad[14] = new CTitleBook();                                // new(0x6b58)
    m_pad[15] = new CAchievement();                            // new(0x28)
    m_pad[16] = new CItemDictionary();  // new(0x1390)；CItemDictionary 现真继承 charac_expand::CData
    m_pad[17] = new CBoosterGage();                            // new(0xc)
    m_pad[18] = new BestClearTime();                          // new(0x24)
    m_pad[19] = new CCharacInformNotice();              // new(0x18)
}
// ORIG 0x0832ac48 T：遍历 0x14 个 CData* 槽，非空则经 vtable+0x14 槽
//（CData 虚表 D0/deleting 析构）删除对象。
charac_expand::CDataMgr::~CDataMgr()
{
    for (int i = 0; i < 0x14; ++i)
    {
        // ORIG：无判空，直接经虚表 +0x14（D0）删除（null 时 ORIG 崩溃语义照抄）。
        delete m_pad[i];
    }
}
advancealtar::CharacAdvanceAltarManager::CharacAdvanceAltarManager() : m_field0(0) {}
advancealtar::CharacAdvanceAltarManager::~CharacAdvanceAltarManager() {}
// ORIG C1 0x0808bf68：写 vptr（编译器自动）+ movb $0x27,0x4 —— +0x04 key 种子。
PIReceiverManager::PIReceiverManager() { m_pad[0] = 0x27; }
PIReceiverManager::~PIReceiverManager() {}
PISenderManager::PISenderManager() { m_pad[0] = 0x27; }
PISenderManager::~PISenderManager() {}
// ORIG T 0x0833c28a：休息(3)↔战斗(1) 两态切换；无宠物早退。
void user_creature::CCreatureMgr::TurnStomach(bool fighting)
{
    if (m_equippedCreature != 0)
    {
        if (fighting)
        {
            if (m_equippedCreature->GetState() == 3)
                m_equippedCreature->SetState(m_user, (CREATURE_STATE)1);
        }
        else
        {
            if (m_equippedCreature->GetState() == 1)
                m_equippedCreature->SetState(m_user, (CREATURE_STATE)3);
        }
    }
}
// ORIG T 0x0832ac94：`this[type*4]`（CData* 槽数组）；type > 0x13 返回 0。
// 与 GetDataR 同形（ORIG 两函数字节一致）；enum 底层无符号 → jbe。
CExpandEquipslot* charac_expand::CDataMgr::GetData(ENUM_CHARAC_EXPAND_TYPE type) const
{
    if (!((unsigned int)type <= 0x13))
        return 0;
    return (CExpandEquipslot*)m_pad[(int)type];
}
// ORIG T 0x0832acb2：`this[type*4]`（CData* 槽数组）；type > 0x13 返回 0。
// 与 update/ResetDaily 一致的 CData*[0x14] 槽解释；enum 底层无符号 → jbe。
CExpandEquipslot* charac_expand::CDataMgr::GetDataR(ENUM_CHARAC_EXPAND_TYPE type) const
{
    if (!((unsigned int)type <= 0x13))
        return 0;
    return (CExpandEquipslot*)m_pad[(int)type];
}
// ORIG T 0x0832ad78：对 20 个 CData* 槽（+0x00..+0x4c）依次调用非虚 CData::reset。
void charac_expand::CDataMgr::reset()
{
    for (int i = 0; i <= 0x13; ++i)
    {
        m_pad[i]->reset();
    }
}

// ORIG 0x0832ade8：对 20 个成员（+0x00..+0x4c，各为 charac_expand::CData*）
// 依次调用虚表槽 0（ResetDailyMidnight）。
// ORIG 0x0832adaa：同上，调用虚表槽 3（ResetDaily）。
void charac_expand::CDataMgr::ResetDailyMidnight()
{
    for (int i = 0; i < 0x14; ++i)
    {
        m_pad[i]->ResetDailyMidnight();
    }
}

void charac_expand::CDataMgr::ResetDaily()
{
    for (int i = 0; i < 0x14; ++i)
    {
        m_pad[i]->ResetDaily();
    }
}

// ORIG T 0x0832acd0：对 20 个 CData* 槽（m_pad）isSave→saveData 全链；任一失败返回 0。
char charac_expand::CDataMgr::update(CUser* pUser)
{
    // [b28c-11] AE（32/32）：`isSave() != 0` + bool 字段/返回（CBoosterGage.h
    // m_bIsSave/isSave bool 化）复现 ORIG `call isSave; je` 直判（char 形 4.4
    // 恒物化 setne %al; test %al，P73）。
    char result = 1;
    for (int i = 0; i < 0x14; ++i)
    {
        if (m_pad[i]->isSave() != 0)
        {
            if (m_pad[i]->saveData(pUser) != 1)
                result = 0;
        }
    }
    return result;
}

char charac_expand::CDataMgr::update(CUser* pUser, ENUM_CHARAC_EXPAND_TYPE type)
{
    // [b28c-11] AE（25/25）：同上（bool isSave 直判，无 setne 物化）。
    char result = 1;
    if (m_pad[(int)type]->isSave() != 0)
    {
        if (m_pad[(int)type]->saveData(pUser) != 1)
            result = 0;
    }
    return result;
}
// ORIG C1 0x08697068：清 byte+0、int+4/+8/+0xc、short+0x10/+0x12。
GrowthWeaponEvent::GrowthWeaponEvent()
{
    m_eventFlag = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldc = 0;
    m_field10 = 0;
    m_field12 = 0;
}
// ORIG C1 0x08695ad0：基类 C1（同 this）后置 +0x38/+0x39/+0x3a=1。
Secu_HackLogCheck::Secu_HackLogCheck()
{
    m_flag223 = 1;
    m_flag224 = 1;
    m_flag225 = 1;
}
// ORIG C1 0x08695ab0：清 +4/+5/+6（m_active223/224/225）。
// 定义挪至 Secu_HackLogCheck C1 之后（技巧 31 规则2：callee 同 TU 前置定义
// 会收紧 caller 帧；挪后 caller 0x8695ad3 ORIG sub $0x18 宽松帧恢复）。
Secu_MonsterCheck::Secu_MonsterCheck()
{
    m_active223 = 0;
    m_active224 = 0;
    m_active225 = 0;
}
// ORIG C1 0x08694f68：构造 3 个 user_gold_info 子对象（+0x04/+0x40/+0x7c）。
Secu_GoldControl::Secu_GoldControl()
{
    // ORIG C1 0x08694f68：m_total/m_cur/m_backup 三个 user_gold_info 成员
    // 默认构造（编译器自动插入，各调 user_gold_info::C1，与 ORIG 同形）。
}
// ORIG C1 0x0848683c：IData C1 + PlayInfo C1（基类）+ Character::Reset()（0x08486818
// = PlayInfo::Reset() + movl $0,0x3c）。ORIG 不初始化 Character 自身字段。
Character::Character()
{
    Reset();
}
// ORIG W 0x080cbc6c：movl $0,0x0 + movb $0,0x4/0x5/0x6 —— 清 eventId/m_pad4/isWin/needSave。
// ORIG C1 0x08697128：movl $0,+0 / movb $0,+4/+5/+6（m_eventId/m_pad4/m_isWin/m_needSave）。
BingoData::BingoData()
{
    m_eventId = 0;
    m_pad4 = 0;
    m_isWin = false;
    m_needSave = false;
}
void BingoData::clear()
{
    m_eventId = 0;
    m_pad4 = 0;
    m_isWin = false;
    m_needSave = false;
}
GrowthCreatureEvent::GrowthCreatureEvent()
{
    // ORIG 0x086970a4（W）：vector<eventCharacInfo> 默认构造后调用 clear()。
    m_vec.clear();
}
CharacLevelUpGiftAccountOnce::CharacLevelUpGiftAccountOnce()
{
    // ORIG 0x086970e6（W）：multimap<int,int> 默认构造后调用 clear()。
    m_map.clear();
}
// ORIG C1 0x084866d4：仅 call PlayInfo::Reset()。
PlayInfo::PlayInfo()
{
    Reset();
}
// ORIG W 0x08486818：PlayInfo::Reset() 后清 +0x3c。
void Character::Reset()
{
    PlayInfo::Reset();
    m_f3c = 0;
}
// 定义挪至 Character::Reset 之后（技巧 31 规则2：恢复 caller 0x848681b
// ORIG sub $0x18 宽松帧；PlayInfo 自身 ctor 0x84866d4 亦在定义之前，不受影响）。
void PlayInfo::Reset()
{
    m_f00 = 0;
    m_f04 = 0;
    m_f08 = 0;
    m_f0c = 0;
    m_f10 = 0;
    m_f14 = 0;
    m_f18 = 0;
    m_f1c = 0;
    m_f20 = 0;
    m_f24 = 0;
    m_f28 = 0;
    m_f2c = 0;
    m_f30 = 0;
    m_f34 = 0;
    m_f38 = 0;
}

// ORIG W 0x084867ee：PlayInfo::operator+=(other) 后 m_f3c 自增。
void Character::operator+=(const Character& other)
{
    PlayInfo::operator+=(other);
    // ORIG 0x8486806-0x8486812：t=m_f3c; m_f3c=t+1 四段形；尾 0x8486815 leave;ret
    // 无 return-reload → 返回类型 void（Main 已落 CUser.h:1874，ORIG 0x8484dcd 调用点不消费）。
    m_f3c = m_f3c + 1;
}

// ============================================================================
// CUser 实现
// ============================================================================

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864e410 | _ZN5CUserC1Ev
CUser::CUser()
    : m_field704a8(0), m_field79670(0), m_field796d8(0), m_chState((ch_state)0)
{
    m_field8ec24 = 0;
    m_field8ec2c = 0;
    m_premium.m_mailBox = 0;
    m_field796d9 = 0;
    m_field8cfd0 = 300;
    m_field8cfd4 = 0;
    m_field8cfd8 = 0;
    setAntibotKey(-1);
    setClientVersion(0);
    m_mcap.m_serialHitCount = 0;
    m_mcap.m_monsterDieHackCnt = 0;
    m_postChecksumData.m_field54 = 0;
    m_field796f8 = 0;
    reset();
    m_historyLog.SetUser(this);
    m_field705d8 = 0;
    m_field8e094 = 0;
    ResetUsedCoinInDungeon();
    m_piSender.m_field10 = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_field8eb18 = _NS_PI_2ND_GetDefaultRandomHashKey();
    m_accountCargo.Clear();
    m_field8eb1c = 0;
    ClearItemSellCount();
    m_field8eb44 = 0;
    m_field8eb48 = 0;
    m_field8eb99 = 0;
    resetInformNoticeFlag();
    disableSaveInformNoticeFlag();
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_rentalInfo.clear();
    m_field8cfcc = (UserSpace::T)0;
    m_postChecksumData.m_firstLogin = 0;   // 块内 +0x30
    m_advanceAltarIdx = 0xffff;
    m_field8eba8 = 0;
    m_field8ebac = 0;
    m_field8ebb0 = 0;
    m_field8ec29 = 0;
    m_rentalFlag3 = 0;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864ef9c | _ZN5CUserD1Ev
CUser::~CUser()
{
    m_powerUp = 0;
}

void CUser::reset()
{
    ResetCurCharac();
    m_gmBossDifficulty = 0;
    m_gmBossMapIndex = 0;
    m_field8cf00 = 0;
    m_field8ead8 = 0;
    m_premium.InitPremium();
    m_characList.clear();
    m_characList.reserve(8);
    m_guildAgitDB.m_pad[2] = 0;
    m_characterView.reset();
    m_hackAnalyzer.resetHackInfo();
    for (int i = 0x65; i < 0xfa1; ++i)
        getHackAnalyzer()->resetServerHackAccumulatedCnt((WongWork::ENUM_HACKTYPE)i);
    m_accHacking.resetInfo();
    if (m_premium.m_mailBox != 0)
    {
        WongWork::CMailBoxHelper::FreeMailBox((WongWork::CMailBox*)m_premium.m_mailBox);
        m_premium.m_mailBox = 0;
    }
    m_list711ec.clear();
    InitFinishPoint();
    set_guildwar_point_per_pvpplay(0);
    memset(&m_guildDB, 0, 0xbd);
    memset(&m_guildAgitDB, 0, 1);
    m_guildAgitDB.m_pad[1] = 0;
    m_codeHackCheck.reset();
    SetReliablePerson(false);
    SetOverTradeGoldCount(0);
    setUserEventCharacterFlag(0);
    m_vec8cef4.clear();
    m_field8d0f0 = 0;
    m_field8d0f1 = 0;
    m_field8d0f2 = 0;
    m_field8e074 = 0;
        m_character.PlayInfo::Reset();
    m_field8d0f8 = 0;
    m_field8d0f4 = 0;
    m_379.reset();
    setAntibotKey(-1);
    setClientVersion(0);
    m_dungeonGainedGold.reset();
    m_postChecksumData.m_field44 = 0;
    m_rating = 0;
    m_field8cfc0 = 0xffffffff;
    m_postChecksumData.m_totalFatigueUsed = 0;
    m_levelupEvents.clear();
    m_characExpand.reset();
    m_premium.m_field98c = 0;
    m_limitItemUsage.clear();
    m_quest.m_field761c = 0;
    m_accId = 0;
    // ORIG reset 0x864a5c0+164..195 语句序：59 → accountCargo.Clear → 5c(0x7965c)
    // → 5e → 60 → dup(0x7966c) → str0 → str1 → 58(0x7965c int) → 56。
    m_postChecksumData.m_field59 = 0;
    m_accountCargo.Clear();
    m_postChecksumData.m_field60 = 0;
    m_postChecksumData.m_field62 = 0;
    m_postChecksumData.m_field64 = 0;
    m_dupMsgSlotIdx = 0;
    m_str79664[0].clear();
    m_str79664[1].clear();
    m_postChecksumData.m_field5c = 0;
    m_postChecksumData.m_field5a = 0;
    ResetUsedCoinInDungeon();
    DBUpdateDBLogItem();
    ResetDBLogItem();
    m_actionPoint.Reset();
    CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
    reset_server_fatigue_data();
    reset_ingame_event_history();
    reset_ingame_event_history_update();
    m_mercenaryMgr.m_characs.clear();
    m_field6ef68 = 0;
    m_field6ef69 = 0xff;
    resetMoneyLog();
    m_field4 = 0;
    m_field5 = 0;
    m_receivedRequestType = 0xff;
    m_field796d8 = 0;
    m_quest.reset();
    m_field8ebd8 = 0;
    m_field8ebd4 = m_field8ebd8;
    m_field8ebd0 = m_field8ebd4;
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_bingo.clear();
    m_levelUpGift.m_map.clear();
    m_growthCreature.m_vec.clear();
    GetGameOptionRef()->reset_load_hotkey_flag();
    m_rentalFlag3 = 0;
    m_field8ec31 = 0;
    m_field8ec32 = 0;
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865851c | _ZN5CUser14ResetCurCharacEv
void CUser::ResetCurCharac()
{
    getPVPResultRefW()->Clear();
    getBattleRecordRefW()->clear();
    m_list8cf80.clear();
    getDungeonClearRefW()->clear();
    m_gmBossDifficulty = 0;
    m_gmBossMapIndex = 0;
    if (getCurCharacR() != 0)
    {
        ((CInventory*)getCurCharacInvenR())->GetAvatarItemMgrW()->Reset();
        ((SkillSlot*)getCurCharacSkillR())->clear_all_skills_both();
        ((CCargo*)getCurCharacCargoR())->reset();
        getCurCharacQuestR()->reset();
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->Reset();
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->SetUser(this);
        if (isDisguiseCharac() != 0)
            setDisguiseCharac(false, 0, 0);
    }
    reset_guild_temp_exp();
    reset_guild_exp();
    reset_level_before_dungeon();
    for (int i = 1; i < 5; ++i)
    {
        // ORIG（CUser::ResetCurCharac 反汇编实证）：characNo = getCurCharacNo()，
        // 然后 getRankTable(i)->unregistBestRecord(characNo)（成员签名，见 CDeathTowerRanking.h）。
        unsigned int characNo = getCurCharacNo();
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistBestRecord(characNo);
        characNo = getCurCharacNo();
        ((WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i))
            ->unregistRanking(characNo);
    }
    resetCurCharacSchoolPoint();
    if (getCurCharacR() != 0)
    {
        GameWorld* pWorld = G_GameWorld();
        int job = get_charac_job();
        // +4+(job+0x58)*4 = +0x164+job*4 = GameWorld::m_channelUserCount[job]
        int* pCnt = &pWorld->m_channelUserCount[job];
        *pCnt = *pCnt - 1;
    }
    setCurCharac(0);
    setTagCharac(0);
    m_tradeSpaceIdx = 0xffff;
    m_field8d004 = 0xffff;
    m_pvpRoomIdx = 0xffff;
    m_field8d008 = 0xffff;
    m_field8d00c = 0xffff;
    m_field8cfcc = (UserSpace::T)0;
    m_field6ef68 = 0;
    m_field6ef69 = 0xff;
    m_guildAgitDB.m_pad[2] = 0;
    m_playingDungeonIdx = 0xffffffff;
    m_postChecksumData.m_field48 = 0;
    m_postChecksumData.m_field4c = 0;
    if (m_premium.m_mailBox != 0)
        ((WongWork::CMailBox*)m_premium.m_mailBox)->Init();
    set_guildwar_point_per_pvpplay(0);
    memset(&m_guildDB, 0, 0xbd);
    memset(&m_guildAgitDB, 0, 1);
    resetGuildCreateFlag();
    resetTempGuildNameAddr();
    reset_pvp_masterid_walkingout_me();
    m_mcap.resetExposedCount();
    m_field8d1dc = 0;
    m_field8d1e0 = 0;
    m_quest.m_field761c = 0;
    m_pcRoomAuth = 0;   // CUser+0x8cef1（原 m_quest.m_field761d 别名）
    memset(&m_reward, 0, 4);
    setForceDropFlag(false);
    m_rentalFlag1 = 1;
    m_rentalFlag2 = 0;
    m_field8ec29 = 0;
}

void CUser::reset_server_fatigue_data()
{
    m_serverFatigueDbType = 0;
    m_serverFatigueValue = 0;
    m_map8eb50.clear();
}

// ORIG 0x0818d3fe：在 map<uchar,short>(+0x8eb50) 中按 type 查疲劳值，未命中返回 0。
short CUser::get_server_fatigue_count(unsigned char type)
{
    // ORIG 0x868d454 call _Rb_tree_iterator::ne + je 尾岛（技巧 30：命中 fall 在前）。
    std::map<unsigned char, short>::iterator it = m_map8eb50.find(type);
    if (it != m_map8eb50.end())
        return it->second;
    return 0;
}

// ORIG 0x0818d4c4：若当前疲劳 DB 类型(+0x8eb4d) == type 返回 (+0x8eb4c) 否则 0。
int CUser::get_fatigue_count_db(int type)
{
    // ORIG 0x868d4ca-0x868d4d4：双零扩展 + `cmp 0xc(%ebp),%eax` arg 槽直折、**无 cur 槽**
    // （sub 无、pop ebp 直返）。b28d-solo-CUser AE：内联条件（具名 cur 落 -0x4 致帧 0x10）。
    if ((unsigned char)m_serverFatigueDbType == type)
        return (unsigned char)m_serverFatigueValue;
    return 0;
}

void CUser::reset_ingame_event_history()
{
    m_map8eb68.clear();
}

void CUser::reset_ingame_event_history_update()
{
    m_map8eb80.clear();
}

void CUser::reset_pvp_masterid_walkingout_me()
{
    m_set8cfe4.clear();
}

void CUser::set_guildwar_point_per_pvpplay(int point)
{
    m_guildWarPoint = point;
}

int CUser::get_guildwar_point_per_pvpplay()
{
    return m_guildWarPoint;
}

XNuclear::CHades* CUser::getHades()
{
    return &m_hades;
}

void* CUser::GetPVPRoom()
{
    // ORIG 0x86551a2 js 尾岛 + 0x86551d1 jmp merge：正向 return（probe12 B 实证复现 js;body;jmp;mov$0;merge）
    if (m_pvpRoomIdx >= 0)
        return G_CGameManager()->GetPvp(m_pvpRoomIdx, this, 0);
    return 0;
}

// ORIG T 0x0868860a：转发 party->SendSecretShopItemList(this)（声明 CUser.h:3432）
void CUser::SendSecretShopItemList()
{
    CParty* party = (CParty*)GetParty();
    if (party != 0)
        party->SendSecretShopItemList(this);
}

void* CUser::GetPICSMap()
{
    return &m_map8eabc;
}

bool CUser::CheckMoney(int money)
{
    // ORIG 0x866af2f/0x866af3c：accName→%esi、level→%ebx 跨 call 缓存（push esi/ebx
    // 帧 0x20）；limit@-0x10、curMoney@-0xc；尾 `cmp -0x10,%eax; jg F; mov $1; jmp;
    // F: mov $0` 直分支岛。4.4 goto-RET0 形在 label 前恒插 1 字节 nop 填充（探针
    // nl11_probe1/2 实证），if/else return 形无 nop（仅 cmp 操作数序 4.4 恒折反向）。
    register const char* accName = get_acc_name();   // b28d-solo-CUser：裸 register（非绑定，P0 合规）跨 call 保活 %esi
    register int level = get_charac_level();   // b28d-solo-CUser：裸 register 跨 call 保活 %ebx（P231/P248）
    int limit = G_CDataManager()->GetMoneyLimitPerLevel(level, accName);
    int curMoney = getCurCharacMoney();
    if (curMoney + money <= limit)
        return true;
    else
        return false;
}

int CUser::getDungeonIdxAfterClear()
{
    CParty* party = (CParty*)GetParty();
    if (!party)
        return -1;
    // CParty+0xb24 = BF 区内嵌视图（CParty.h m_battleField@+0xb24 建模区）。
    return party->m_battleField.get_dungeon_index(
        );
}

void CUser::resetMoneyLog()
{
    for (int i = 0; i <= 6; ++i)
        m_moneyLogs[i].reset();
    m_moneyLogIdx = 0;
}

void CUser::ResetDBLogItem()
{
    m_dbLogItems.clear();
}

void CUser::DBUpdateDBLogItem() const
{
    // b28c-L5：ORIG 0x0868950x 增量段调
    //   `_ZN9__gnu_cxx17__normal_iteratorIPKN5CUser14_DBLogItemInfoESt6vectorIS2_SaIS2_EEEppEi`
    //   （**后置** ++，带独立 sret 临时槽 -0x1c：`lea -0x1c; movl $0,0x8(%esp);
    //   lea -0x24,%edx; mov %edx,0x4(%esp); mov %eax,(%esp); call ppEi; sub $4,%esp`）；
    //   4.4 对 `++it` 走 ppEv（返回引用）⇒ 少 sret 槽 + 少 `sub $0x4,%esp`（ndiff 10）。
    //   改后置形后该 sret 槽本身即把迭代器阶梯顶到 ORIG 位置 ⇒ 原「技巧 31 dead
    //   slotPad」变为多余（会让 it 深一格到 -0x28、全帧平移），删除。
    for (std::vector<_DBLogItemInfo>::const_iterator it = m_dbLogItems.begin();
         it != m_dbLogItems.end(); it++)
    {
        DB_LogBuyItem::makeRequest(get_acc_id(), getCurCharacNo(), it->m_itemIdx,
                                   it->m_type, it->m_value);
    }
}

CParty* CUser::GetParty()
{
    // ORIG 0x8655160 js 尾岛（0x8655182 mov $0）：正向 early-return（probe12 B 形）
    if (m_field8d004 >= 0)
        return G_CGameManager()->GetParty(m_field8d004);
    return 0;
}

// CUser::GetUserPosInParty @ 0x08688638 —— 返回 own 在队伍中的槽位，不在返回 -1。
int CUser::GetUserPosInParty()
{
    CParty* party = (CParty*)GetParty();
    // ORIG 0x8688650 je 尾岛（0x8688666 mov $-1）：正向 early-return（同 GetPVPRoom 族 probe12 B 形）
    if (party != 0)
        return party->GetUserPosition(this);
    return -1;
}

WarRoom* CUser::GetWarRoom()
{
    // ORIG 0x86551f2 js 尾岛（0x8655214 mov $0）：正向 early-return（probe12 B 形）
    if (m_field8d008 >= 0)
        return G_CGameManager()->GetWarRoom(m_field8d008);
    return 0;
}

// ORIG T 0x0864df12：遍历 m_characList（this+0x796e8），首字段 m_characNo
// 与 characNo 相等则返回下标，否则返回 -1。
int CUser::get_charac_idx(unsigned int characNo) const
{
    // ORIG 0x864df12：prologue 连构 begin→-0x10、end→-0x14（尾循环 cond 双 lea
    // 无重构，技巧 25 具名缓存消重）；声明序 end 后占远槽。
    int idx = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator itend = m_characList.end();
    for (; it != itend; ++it)
    {
        if ((unsigned int)it->m_characNo == characNo)
            return idx;
        ++idx;
    }
    return -1;
}
int CUser::get_charac_no(int param)
{
    // ORIG 0x08677a?? prologue cmp $-1 的 fall-through 块 = param==-1 路径
    //（技巧 17：等号形让 -1 块前置，!= 形被 4.4 后置）。
    if (param == -1)
    {
        if (getCurCharacR() == 0)
            return 0;
        return *((unsigned int*)getCurCharacR());
    }
    if (param < 0 || (int)m_characList.size() <= param)
        return 0;
    return *((unsigned int*)&m_characList[param]);
}

bool CUser::CheckItemLock(int a, int b) const
{
    if (getCurCharacR() == 0)
        return 0;
    // ORIG 0x86469??：call getCurCharacInvenR 结果留 %eax（this 最后入栈），实参经 %edx
    // 中转——无 pInven 具名槽（直调链式形）。
    const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(a, b);
    if (item == 0)
        return 1;
    unsigned char lock =
        item->m_amp.GetLock();   // Inven_Item::m_amp @+0x11
    if (lock == 0)
        return 0;
    // ORIG 0x86469d4：test;je → mov $0 尾岛 / fall mov $1（正向 if；`!()` 反产 xor 物化）。
    if (((item_lock::CItemLock*)GetCharacExpandDataR(
              (ENUM_CHARAC_EXPAND_TYPE)2))
            ->CheckItemLock(lock))
        return 1;
    return 0;
}

unsigned short CUser::getCurCharacTotalFatigue() const
{
    // ORIG 0x865776c：total dead-init `movw $0,-0xa`（技巧 8）+ call 物化 `mov %ax,-0xa`
    // (0x865777d)；rate 无槽：test %ax,%ax;setne;test;je 物化（技巧 22，0x8657791-99）。
    unsigned short total = 0;
    total = getCurCharacFatigue();
    // b28d-solo-CUser AE: 条件内联免 bool 槽（ORIG 0x8657791-99 setne;test;je 无槽，
    // 具名 nz 使 4.4 落 -0x9 并推 total 槽 -0xa→-0xc；probe_bool a3/a5）。
    if (m_premium.GetAdvantageFatigueRate() != 0)
    {
        // ORIG 槽反推（声明序即分配序，技巧 21：先声明占近槽）：maxPremium@-0xe、
        // premiumFatigue@-0xc（min 双 lea 0x86577c8-0x86577d2 实参序 (premium,maxPremium)
        // 对应 std::min(a,b) 签名 (T,T) 取 &a,&b → min 第1参=premium@-0xc? lea -0xe 入
        // 0x4(%esp)（第2参）、lea -0xc 入 (%esp)（第1参）⇒ min(premiumFatigue,maxPremium)
        // 保留；added@-0x10、maxTotal@-0x12（后声明远槽——此处三者 ORIG 强制 -0x12<-0xe<-0xc
        // 交叠，maxTotal 声明最先占最远 16bit 槽位序，见下）。
        // 槽序=声明序（4.4 先声明近槽）：premium@-0xc/maxPremium@-0xe/added@-0x10/
        // maxTotal@-0x12（ORIG 0x86577a6-0x86577c4 装载序与赋值分开写）。
        unsigned short premiumFatigue;
        unsigned short maxPremium;
        unsigned short added;
        unsigned short maxTotal;
        maxTotal = getCurCharacTotalMaxFatigue();
        maxPremium = getCurCharacMaxPremiumFatigue();
        premiumFatigue = getCurCharacPremiumFatigue();
        // ORIG 0x86577da-0x86577e1：`_ZSt3minIt` = min<unsigned short>（t=ushort），
        // movzwl (%eax) 解引用 + `add -0xa,%ax`、截断写 added@-0x10。
        added = std::min(premiumFatigue, maxPremium) + total;
        // ORIG 0x86577f2-0x86577fa：total = min(added, maxTotal)（双 lea 形）。
        total = std::min(added, maxTotal);
    }
    return total;  // ORIG 0x86577fe 单点 movzwl -0xa(%ebp)
}

unsigned short CUser::getCurCharacTotalMaxFatigue() const
{
    // ORIG 0x86??：`movw $0,-0xa` dead init + call 结果 `mov %ax,-0xa` 双写（技巧 8）；
    // rate 无槽（register asm("ax")，call 结果直落 test;setne;test;je 物化）。
    unsigned short total = 0;
    total = getCurCharacMaxFatigue();
    // ORIG 0x8??：`test %ax,%ax; setne %al; test %al,%al; je`（技巧 22：register bool
    // 结果 !=0 物化）。b28d-solo-CUser AE：条件内联免 bool 槽（probe_bool a3/a5）。
    if (m_premium.GetAdvantageFatigueRate() != 0)
        total = (unsigned short)(total + getCurCharacMaxPremiumFatigue());
    return total;
}

// ============================================================================
// CUser::make_basic_info —— ORIG _ZN5CUser15make_basic_infoEPcc @0x0865a44e
// 报告：docs/class_func_reports/CUser/make_basic_info.md（## 汇编 + ## 反编译 C
// 全文核对）。按 type 分派：0=ENUM_USERINFO_MINIMUM、1=ENUM_USERINFO_ADDITION、
// 2=账号角色列表（m_characList 循环）、3=ENUM_USERINFO_OTHER_CHARACTER；
// 其余值直接返回 1。三个需当前角色的分支在 getCurCharacR()==0 时
// LogManager::logFormat 报错并返回 0；各分支 lock → 填包 → unlock。
// ============================================================================

// ---- checkSpecialChar / checkSpecialCharName（ORIG 自由函数，分支 2 使用）----
// ORIG _Z16checkSpecialCharPc T 0x0829efae / _Z20checkSpecialCharNamePc T 0x0829f01f。
// 依赖的 CDataManager 名字过滤四方法均已建模；后续若其它 TU 需要应迁往公共 TU，
// 届时删除此处定义避免重复符号。
bool checkSpecialChar(char* name)
{
    if (G_CDataManager()->getCharacNameFilterPass())
    {
        G_CDataManager()->setCharacNameFilterPass(false);
        return false;
    }
    return G_CDataManager()->hasSpecialCharacter(name)
           || G_CDataManager()->hasPreventString(name);
}

bool checkSpecialCharName(char* name)
{
    if (G_CDataManager()->getCharacNameFilterPass())
    {
        G_CDataManager()->setCharacNameFilterPass(false);
        return false;
    }
    return G_CDataManager()->hasPreventStringName(name);
}

namespace
{

#pragma pack(push, 1)

// PvpResultType 只读视图：权威最小定义（CUserCharacInfo.h）中 +0x5c 尚在 m_pad5c
// 内；ORIG 分支 0/3 以 int 读该槽（全量布局见 PvpResultType.h：今日胜场【推断】）。
struct PvpResultBasicView
{
    int m_totalWinCount;            // +0x00
    int m_field04;                  // +0x04
    int m_field08;                  // +0x08
    int m_field0c;                  // +0x0c
    int m_field10;                  // +0x10
    int m_pvpGrade;                 // +0x14（与 get_pvp_grade 同源）
    int m_pad18[(0x5c - 0x18) / 4];
    int m_field5c;                  // +0x5c（分支 0/3 写出槽）
};
typedef char pvp_view_size_ok[(sizeof(PvpResultBasicView) == 0x60) ? 1 : -1];

// getCurCharacR() 当前角色数据扩展字节视图（权威声明中这些偏移均在 pad 区；
// +0x39 起 0x14 字节与 CommonTime 创建时间区对齐【推断】）。
struct CurCharacByteView
{
    char m_pad00[0x39];
    char m_field39;                 // +0x39（分支 3）
    char m_field3a;                 // +0x3a（分支 3）
    char m_field3b;                 // +0x3b（分支 3）
    char m_pad3c[0x75 - 0x3c];
    unsigned char m_field75;        // +0x75（分支 0，无符号写出）
};

// m_characList 元素扩展视图：权威 struct _Charac_info（CUser.h，pack(1)，总长
// 0x14d1）中段为 pad；此处仅命名 ORIG 分支 2 访问的偏移（视图止于 +0x14ca）。
struct CharacListElemView
{
    int m_characNo;                 // +0x000
    char m_name[0x1d];              // +0x004
    char m_pad21[0x26 - 0x21];
    char m_job;                     // +0x026
    short m_level;                  // +0x027
    char m_field29;                 // +0x029
    char m_pad2a[0x39 - 0x2a];
    CommonTime m_createTime;        // +0x039（UpdateCharacView → long 写记录 +0x8c）
    char m_pad4d[0x83 - 0x4d];
    char m_forbidPlayFlag;          // +0x083（禁玩标记；到期清零并投递 SIG_FORBID_USER_TO_PLAY）
    int m_forbidPlayLeftSec;        // +0x084（剩余秒）
    char m_pad88[0xda - 0x88];
    char m_inventory[0xe40 - 0xda]; // +0x0da 每角色 CInventory 子对象（MakeEquipList 接收者）
    char m_fielde40;                // +0xe40（bit1 → 记录 flags bit4）
    char m_pade41[0xe5d - 0xe41];
    int m_fielde5d;                 // +0xe5d（UpdateCharacView 直拷记录 +0x90）
    char m_pade61[0xe66 - 0xe61];
    unsigned char m_reenterable;    // +0xe66（now < expireSec → 1，随 viewState 写出）
    char m_pade67[0x1240 - 0xe67];
    unsigned char m_field1240;      // +0x1240
    char m_pad1241[0x1246 - 0x1241];
    char m_field1246;               // +0x1246
    char m_pad1247[0x12ad - 0x1247];
    int m_expireSec;                // +0x12ad（与 CSystemTime::getCurSec() 比较）
    char m_pad12b1[0x14c6 - 0x12b1];
    int m_viewState;                // +0x14c6（==0xff 时写 8）
};
typedef char elem_view_size_ok[(sizeof(CharacListElemView) == 0x14ca) ? 1 : -1];

// GetGuildDBInfo()（STGuildDBInfoOnly，CUser.h 中 +0x00..0x94 为 pad）访问视图：
// +0x00 公会名字符串、+0x1b 随名写出的字节、+0x44 成员数、成员表自 +0x45 起
// 5 字节/项（int 等级 + char 职位）。
struct GuildMemberEntryView
{
    int m_grade;                    // 条目 +0x00（>0xc7 才写出）
    char m_position;                // 条目 +0x04（有符号写出）
};

struct GuildDbInfoView
{
    char m_guildName[0x1b];         // +0x00
    unsigned char m_field1b;        // +0x1b
    char m_pad1c[0x44 - 0x1c];
    unsigned char m_memberCount;    // +0x44
    GuildMemberEntryView m_members[1]; // 实际数量由 m_memberCount 给出
};
typedef char guild_view_size_ok[(sizeof(GuildDbInfoView) == 0x4a) ? 1 : -1];

// SIG_FORBID_USER_TO_PLAY 前 9 字节写入视图（SigTypes.h 现为 0x100 pad 占位；
// ORIG：memset 9 字节后 +0x00=characNo、+0x04=0、+0x05..0x08=0）。
struct SigForbidUserToPlayBody
{
    unsigned int m_characNo;        // +0x00
    unsigned char m_field04;        // +0x04
    unsigned int m_field05;         // +0x05..0x08
};

#pragma pack(pop)

}  // namespace

// b28d-lane5 N封顶: N97/P230 cmov; P11 符号扩展; P11 反向符号扩展; N18a/P200 拓宽中轉; P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0865a44e | _ZN5CUser15make_basic_infoEPcc
bool CUser::make_basic_info(char* buf, char type)   // b28c-L5 SIGFIX（P174 串 bool）
{
    InterfacePacketBuf& pkt = *(InterfacePacketBuf*)buf;
    PacketGuard& pg = *(PacketGuard*)buf;

    if (type == 1)
    {
        // ---- 分支 1：ENUM_USERINFO_ADDITION（ORIG +0xbb9）----
        if (getCurCharacR() == 0)
        {
            LogManager::logFormat(1, "user.cpp",
                                  __PRETTY_FUNCTION__,
                                  0x31c3,
                                  "Error ENUM_USERINFO_ADDITION, getCurCharacR() == 0");
            return 0;
        }
        lock();
        pkt.put_short((int)m_field704a8);
        int exp;
        if (G_GameWorld()->IsCharacterLevelRevisionChannel())
            exp = G_CDataManager()->get_level_exp(0x55);
        else
            exp = get_charac_exp();
        pkt.put_int(exp);
        make_charac_stat(pg);
        pkt.put_byte((int)GetCurExEquipSlotStat());
        ((CInventory*)getCurCharacInvenR())
            ->MakeEquipList(&pg, true, (ENUM_USERINFO)1, (ENUM_EQUIPSLOT)0);
        pkt.put_byte(GetCurCharacSkillTreeIndex());

        // 公会技能/成员双组块 ×2（保留索引-回填计数模式）
        const GuildDbInfoView* guild =
            (const GuildDbInfoView*)GetGuildDBInfo();
        unsigned char memberCount = guild->m_memberCount;
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[6 + (i + 0x20) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[6 + (i + 0x20) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[6 + 1 + (i + 0x20) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[0xe + (i + 0xe8) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[0xe + (i + 0xe8) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[0xe + 1 + (i + 0xe8) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }

        const user_creature::CCreatureMgr* creatureMgr =
            ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR();
        pkt.put_byte(creatureMgr->GetEquipedCreatureLevel());
        pkt.put_byte(6);
        for (int i = 0; i < 6; ++i)
            pkt.put_byte((int)(char)getDemensionInoutValue((char)i));
        if (!GlobalData::s_revengeDungeonMgr->IsOpenRevengeDungeon())
            pkt.put_byte(0);
        else
            pkt.put_byte((int)GetCurRevengeDungeonCount());
        pkt.put_byte((int)(char)getUltimateInoutValue(2));
        const TOD_UserState* tod = (const TOD_UserState*)GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xd);
        pkt.put_byte(tod->getEnterCount());
        int limit = (int)(char)G_CDataManager()->get_limit_inout_count(
            ENUM_BLOOD_TYPE_1);
        const CCharacBloodDungeon* blood =
            (const CCharacBloodDungeon*)GetCharacExpandData(
                (ENUM_CHARAC_EXPAND_TYPE)1);
        if (limit - blood->GetEnterCount() < 0)
            pkt.put_byte(0);
        else
            pkt.put_byte(limit - blood->GetEnterCount());
        ((CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb))
            ->makePacket_BuyingStatusList(pg, false);
        pkt.put_byte(GetManageLevel());
        unlock();
    }
    else if (type < 2)
    {
        if (type == 0)
        {
            // ---- 分支 0：ENUM_USERINFO_MINIMUM（ORIG +0x52）----
            if (getCurCharacR() == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__,
                    0x308c,
                    "Error ENUM_USERINFO_MINIMUM, getCurCharacR() == 0");
                return 0;
            }
            lock();
            pkt.put_short((int)(get_unique_id() & 0xffff));
            // ORIG 0x0865a..：put_int(strlen)/put_str/put_str 的 strlen 各自重调 getCurCharacName()。
            pkt.put_int((int)strlen(getCurCharacName()));
            pkt.put_str(getCurCharacName(), (int)strlen(getCurCharacName()));
            pkt.put_byte(get_charac_job());
            pkt.put_byte((int)getCurCharacGrowType());
            if (G_GameWorld()->IsCharacterLevelRevisionChannel())
                pkt.put_byte(0x55);
            else
                pkt.put_byte(get_charac_level());
            pkt.put_byte(get_pvp_grade());
            const PvpResultBasicView* pvp =
                (const PvpResultBasicView*)getPVPResultR();
            pkt.put_byte(pvp->m_field5c);
            pkt.put_byte(GetUserState());
            ((CInventory*)getCurCharacInvenR())
                ->MakeEquipList(&pg, false, (ENUM_USERINFO)0,
                                (ENUM_EQUIPSLOT)0);
            get_acc_id();
            pkt.put_byte(0);
            if (G_GameWorld()->IsSchoolPvPChannel() && GetSchoolNo() != 0)
                pkt.put_int((int)GetSchoolNo());
            if (G_GameWorld()->GetChannelType() == 0xe)
                pkt.put_int(GetCurCharacOnlinePreliminaryTeamId());
            else
                pkt.put_int(get_charac_guildkey());
            pkt.put_byte((int)IsPermissionPrivateStore());

            // ORIG 0x0865b..：每处 creature 访问重取 getCurCharacInvenR()->GetCreatureMgrR()
            // （P36/P44），creatureName 仍缓存（ORIG 亦缓存 local_9c）。
            pkt.put_int(
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR()
                    ->GetCreatureItemId());
            const char* creatureName =
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR()
                    ->GetCreatureName();
            pkt.put_int((int)strlen(creatureName));
            pkt.put_str(creatureName, (int)strlen(creatureName));
            pkt.put_byte(
                ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR()
                        ->IsDieCreature()
                    ? 0
                    : 1);

            pkt.put_byte(isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) ? 1 : 0);
            pkt.put_int(GetPCRoomNo());
            pkt.put_int(getCurrCharacBlackCount());

            const GuildDbInfoView* guild =
                (const GuildDbInfoView*)GetGuildDBInfo();
            if (G_GameWorld()->IsSchoolPvPChannel() && GetSchoolNo() != 0)
            {
                // ORIG：CLeagueMatch::GetPlayCountPerOneTeam() 读全局
                // nPlayerCountPerOneTeam_（+0x00 字节）→ put_byte(& 0xff)。
                pkt.put_byte((int)CLeagueMatch::GetPlayCountPerOneTeam());
                pkt.put_int((int)m_str8e080.length());
                pkt.put_str(m_str8e080.c_str(), (int)m_str8e080.length());
            }
            else if (G_GameWorld()->GetChannelType() == 0xe)
            {
                // ORIG：online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam()
                // （内部取 G_CDataManager()->GetPvPTeamCount()）→ put_byte(& 0xff)。
                pkt.put_byte(
                    (int)(online_preliminary::COnlinePreliminary::GetPlayCountPerOneTeam()
                          & 0xff));
                const char* teamName = GetCurCharacOnlinePreliminaryTeamName();
                pkt.put_int((int)strlen(teamName));
                pkt.put_str(teamName, (int)strlen(teamName));
            }
            else
            {
                pkt.put_byte((int)guild->m_field1b);
                pkt.put_int((int)strlen(guild->m_guildName));
                pkt.put_str(guild->m_guildName, (int)strlen(guild->m_guildName));
            }

            pkt.put_int(GetCurCharacChaosPoint());
            pkt.put_byte(0xe < GetAge() ? 1 : 0);
            pkt.put_byte((int)(char)getPowerSide());
            pkt.put_byte((int)(char)isJoinPowerWar());
            pkt.put_byte(GetCurCharacExpertJobType());
            pkt.put_int(GetCurCharacExpertJobExp());
            pkt.put_byte((int)getDisguiseKind());
            pkt.put_short((int)getDisguiseIndex());
            pkt.put_byte((int)(char)get_charac_visible_values());

            if (G_GameWorld()->IsForbiddenChat(getCurCharacName()))
            {
                unsigned int left =
                    G_GameWorld()->GetLeftTimeOfForbiddenChat(getCurCharacName());
                if (left == 0)
                    G_GameWorld()->EnableChat(getCurCharacName());
                {
                    cMyTrace trace(__PRETTY_FUNCTION__,
                                   0x3154, 0);
                    trace("%s FCT1: %ld\n", getCurCharacName(), left);
                }
                left = left / 60000u;
                {
                    cMyTrace trace(__PRETTY_FUNCTION__,
                                   0x3156, 0);
                    trace("%s FCT2: %ld\n", getCurCharacName(), left);
                }
                pkt.put_int((int)left);
            }
            else
            {
                pkt.put_int(0);
            }
            if (G_GameWorld()->IsForbiddenMove(getCurCharacName()))
            {
                unsigned int left =
                    G_GameWorld()->GetLeftTimeOfForbiddenMove(getCurCharacName());
                if (left == 0)
                    G_GameWorld()->EnableMove(getCurCharacName());
                {
                    cMyTrace trace(__PRETTY_FUNCTION__,
                                   0x3163, 0);
                    trace("%s FMT1: %ld\n", getCurCharacName(), left);
                }
                left = left / 60000u;
                {
                    cMyTrace trace(__PRETTY_FUNCTION__,
                                   0x3165, 0);
                    trace("%s FMT2: %ld\n", getCurCharacName(), left);
                }
                pkt.put_int((int)left);
            }
            else
            {
                pkt.put_int(0);
            }

            pkt.put_int((int)getCurCharacHelpAbuseComputedRatio());
            const CurCharacByteView* curBytes =
                (const CurCharacByteView*)getCurCharacR();
            pkt.put_byte((int)curBytes->m_field75);
            pkt.put_short((int)getCurCharacUsedFatigue());
            pkt.put_byte((int)GetServerGroup());
            pkt.put_short((int)(short)GetUserPowerWarPoint());
            pkt.put_byte(GlobalData::s_power_manager->GetRankingByCharacNo(
                (unsigned int)getCurCharacNo()));
            pkt.put_short((int)(short)GetCurCharacEmotion());
            pkt.put_byte((int)(char)GetCurCharacSkillTreeIndex());
            pkt.put_byte(IsReturnUser() != 0 ? 1 : 0);
            char linkType = getCharacLinkType();
            pkt.put_byte((int)getLinkCharacSlotIndex());
            pkt.put_byte((int)linkType);
            unlock();
        }
    }
    else if (type == 2)
    {
        // ---- 分支 2：账号角色列表（ORIG +0x19e3；无当前角色守卫）----
        lock();
        pkt.put_byte((int)getCharacSlotLimit());
        pkt.put_byte((int)getSlotEffectCount());
        pkt.put_byte(GetManageLevel());
        pkt.put_int(GetManagePoint());
        int written = 0;
        int countIdx = pkt.get_index();
        pkt.put_short(written);
        const int nowSec = GlobalData::s_systemTime_.getCurSec();
        for (int i = 0; i < (int)m_characList.size(); ++i)
        {
            if (G_GameWorld()->GetChannelType() == 0xe
                && (unsigned int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_characNo != getOnlinePreliminaryCharacNo())
                continue;
            pkt.put_short(written);
            ++written;
            pkt.put_int((int)strlen(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name));
            pkt.put_str(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name, (int)strlen(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name));
            bool special = checkSpecialChar(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name) != false
                           || checkSpecialCharName(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_name) != false;
            pkt.put_byte(special ? 1 : 0);
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_job);
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_field29);
            pkt.put_byte((int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_level);
            pkt.put_byte(0);
            if (!(nowSec < reinterpret_cast<CharacListElemView&>(m_characList[i]).m_expireSec))
                reinterpret_cast<CharacListElemView&>(m_characList[i]).m_reenterable = 0;
            else
                reinterpret_cast<CharacListElemView&>(m_characList[i]).m_reenterable = 1;
            if (reinterpret_cast<CharacListElemView&>(m_characList[i]).m_viewState == 0xff)
                pkt.put_byte(8);
            else
                pkt.put_byte(reinterpret_cast<CharacListElemView&>(m_characList[i]).m_reenterable != 0 ? 4 : 0);
            ((CInventory*)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_inventory)
                ->MakeEquipList(&pg, false, (ENUM_USERINFO)2,
                                (ENUM_EQUIPSLOT)0);
            get_acc_id();
            pkt.put_byte(0);
            pkt.put_int(get_charac_guildkey());
            pkt.put_byte((int)IsPermissionPrivateStore());
            pkt.put_byte(isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) ? 1 : 0);
            pkt.put_int(GetPCRoomNo());
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_fielde40);
            pkt.put_byte((int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_field1240);
            pkt.put_byte((int)(char)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_field1246);
            if (reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayFlag == '\0')
            {
                pkt.put_int(0);
            }
            else if (reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayLeftSec < 1)
            {
                pkt.put_int(0);
                reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayFlag = 0;
                reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayLeftSec = 0;
                Stream* stream =
                    GlobalData::s_stream_pool->Acquire("user.cpp", 0x344c);
                CStreamGuard guard(stream, true);
                // ORIG 0x0865c4f9/0x0865c524/0x0865c53e：`*guard << v`（operator*
                // 链产生 deEv）+ `guard->GetInBuffer`（ptEv 链），P103/P132。
                (*guard)->operator<<(0x93);
                (*guard)->operator<<((int)GetUID());
                // ORIG 0x0865b40b：GetInBuffer<SIG_FORBID_USER_TO_PLAY>()（真 SIG 类型
                // 保证 mangled `GetInBufferI23SIG_FORBID_USER_TO_PLAYEEPT_v` 匹配）；
                // 前 9 字节 payload 借 SigForbidUserToPlayBody 视图写。
                SIG_FORBID_USER_TO_PLAY* sig =
                    guard.operator->()->GetInBuffer<SIG_FORBID_USER_TO_PLAY>();
                SigForbidUserToPlayBody* body =
                    (SigForbidUserToPlayBody*)sig;
                memset(body, 0, 9);
                body->m_characNo = (unsigned int)reinterpret_cast<CharacListElemView&>(m_characList[i]).m_characNo;
                body->m_field04 = 0;
                body->m_field05 = 0;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
            }
            else
            {
                int days = reinterpret_cast<CharacListElemView&>(m_characList[i]).m_forbidPlayLeftSec / 0x15180;
                pkt.put_int(days < 1 ? 1 : days);
            }
        }
        pkt.put_short(countIdx, written);
        unlock();
    }
    else if (type == 3)
    {
        // ---- 分支 3：ENUM_USERINFO_OTHER_CHARACTER（ORIG +0x127f）----
        if (getCurCharacR() == 0)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__,
                0x328c,
                "Error ENUM_USERINFO_OTHER_CHARACTER, getCurCharacR() == 0");
            return 0;
        }
        lock();
        pkt.put_short((int)m_field704a8);
        int exp;
        if (G_GameWorld()->IsCharacterLevelRevisionChannel())
            exp = G_CDataManager()->get_level_exp(0x55);
        else
            exp = get_charac_exp();
        pkt.put_int(exp);
        make_charac_stat(pg);
        pkt.put_byte((int)GetCurExEquipSlotStat());
        ((CInventory*)getCurCharacInvenR())
            ->MakeEquipList(&pg, true, (ENUM_USERINFO)3, (ENUM_EQUIPSLOT)0);
        pkt.put_byte(GetCurCharacSkillTreeIndex());

        const GuildDbInfoView* guild = (const GuildDbInfoView*)GetGuildDBInfo();
        unsigned char memberCount = guild->m_memberCount;
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[6 + (i + 0x20) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[6 + (i + 0x20) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[6 + 1 + (i + 0x20) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }
        {
            int countIdx = pkt.get_index();
            int count = 0;
            pkt.put_byte(0);
            for (int i = 0; i < 0xcc; ++i)
            {
                if (getCurCharacSkillR()[0xe + (i + 0xe8) * 2] != '\0')
                {
                    pkt.put_byte(
                        (int)(unsigned char)getCurCharacSkillR()[0xe + (i + 0xe8) * 2]);
                    pkt.put_byte(
                        (int)(char)getCurCharacSkillR()[0xe + 1 + (i + 0xe8) * 2]);
                    ++count;
                }
            }
            const GuildMemberEntryView* members =
                reinterpret_cast<const GuildMemberEntryView*>(guild->m_members);
            for (int j = 0; j < (int)memberCount; ++j)
            {
                if (getGuildMemberGrade() != 0 && getGuildMemberGrade() != 0
                    && members[j].m_grade > 0xc7)
                {
                    pkt.put_byte(members[j].m_grade);
                    pkt.put_byte((int)(char)members[j].m_position);
                    ++count;
                }
            }
            pkt.put_byte(countIdx, count);
        }

        const user_creature::CCreatureMgr* creatureMgr =
            ((const CInventory*)getCurCharacInvenR())->GetCreatureMgrR();
        pkt.put_byte(creatureMgr->GetEquipedCreatureLevel());
        // ORIG 0x0865c..：每字段重调 getPVPResultR()（P44），共 4 次。
        pkt.put_int(((const PvpResultBasicView*)getPVPResultR())->m_totalWinCount);
        pkt.put_int(((const PvpResultBasicView*)getPVPResultR())->m_field04);
        pkt.put_int(((const PvpResultBasicView*)getPVPResultR())->m_pvpGrade);
        pkt.put_byte(((const PvpResultBasicView*)getPVPResultR())->m_field5c);
        const CurCharacByteView* curBytes =
            (const CurCharacByteView*)getCurCharacR();
        pkt.put_byte((int)(char)curBytes->m_field39);
        pkt.put_byte((int)(char)curBytes->m_field3a);
        pkt.put_byte((int)(char)curBytes->m_field3b);
        pkt.put_int((int)getCurCharacHelpAbuseComputedRatio());
        pkt.put_short((int)(short)GetUserPowerWarPoint());
        pkt.put_int(GetGuildPowerWarPoint());
        const char* guildName = GetGuildName();
        pkt.put_int((int)strlen(guildName));
        pkt.put_str(guildName, (int)strlen(guildName));
        pkt.put_byte((int)GetGuildLevel());
        pkt.put_byte((int)(char)getPowerSide());
        pkt.put_byte(IsExistGuildAgit() ? 1 : 0);
        ((CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb))
            ->makePacket_BuyingStatusList(pg, false);
        pkt.put_byte(GetManageLevel());
        unlock();
    }
    return 1;
}
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865dd14 | _ZN5CUser10send_equipEi
void CUser::send_equip(int slot)
{
    // ORIG 0x0865dd14：state(+0x8cfc4) ∈ {5,8,10,0xc} 且 slot==0x16 → 发给自己；
    // 否则（slot<0xc 或 0x16）→ 广播 make_basic_info(type 0) 给全房间。
    if (slot >= 0xc && slot != 0x16)
        return;
    int state = m_chState;   // +0x8cfc4（推断：角色状态）
    bool inState = (state == 0xc) || (state == 5) || (state == 8) || (state == 10);
    if (inState)
    {
        if (slot != 0x16)
            return;
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
    }
    else
    {
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 2);
        ((InterfacePacketBuf*)&packet)->put_byte(0);
        ((InterfacePacketBuf*)&packet)->put_short(1);
        make_basic_info((char*)&packet, (char)0);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        G_GameWorld()->send_all(packet, this);
    }
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x086893ac | _ZN5CUser12AddDBLogItemEjj20ENUM_DBLOG_ITEM_TYPE
void CUser::AddDBLogItem(unsigned int itemIdx, unsigned int type,
                         ENUM_DBLOG_ITEM_TYPE value)
{
    // ORIG 0x08689360：循环步进用后置 it++（ppEi 符号），非 ++it。
    // 槽序实证（2026-09-05 CUserC8 修正前批方向）：ORIG info=-0x14（近，5 处
    // lea -0x14）/it=-0x20（远）——4.4 后声明占近槽（技巧 21 正用）：info 声明
    // 置于 for-init 之后。
    // 残差 N82/P112 族：4.1 it=-0x14/info=-0x20 vs 4.4 it=-0x20/info=-0x1c，
    // begin() sret 槽分配跨版本恒差；dead 前置/后置只整体平移不换相对序
    // （nl11_probe6 实证），源形不可复现。
    for (std::vector<_DBLogItemInfo>::iterator it = m_dbLogItems.begin();
         it != m_dbLogItems.end(); it++)
    {
        if (it->m_itemIdx == itemIdx)
        {
            it->m_type += type;
            return;
        }
    }
    _DBLogItemInfo info;
    memset(&info, 0, 0xc);
    info.m_itemIdx = itemIdx;
    info.m_type = type;
    info.m_value = value;
    m_dbLogItems.push_back(info);
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08681224 | _ZN5CUser13addAvatarItemEmccPKc20eAvatarItemAddReason
int CUser::addAvatarItem(unsigned long itemIdx, char param2, char param3,
                         const char* ipgAgency, eAvatarItemAddReason reason)
{
    CDataManager* pMgr = G_CDataManager();
    CEquipItem* pItem = (CEquipItem*)pMgr->find_item(itemIdx);
    if (pItem == 0)
        return -1;
    std::vector<stAvatarTypeSelect_t>* pSelect =
        (std::vector<stAvatarTypeSelect_t>*)pItem->getAvatarTypeSelect();
    int usablePeriod = pItem->getUsablePeriod();
    if (param3 < (int)pSelect->size())
        usablePeriod = (int)(*pSelect)[param3].m_field0;
    CInventory* pInven = (CInventory*)getCurCharacInvenW();
    return pInven->AddAvatarItem(itemIdx, usablePeriod, 0, param2, param3,
                                 ipgAgency, reason, 0, 0);
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867c65a | _ZN5CUser18SendUpdateItemListENS_11eSendTargetE14ENUM_ITEMSPACEi
int CUser::SendUpdateItemList(eSendTarget target, ENUM_ITEMSPACE space, int slot)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte((int)space);
    ((InterfacePacketBuf*)&packet)->put_short(1);
    // ORIG 0x0867c65a：空间→INVEN_TYPE 映射（1→2、7→3、3→0、其余→1）；
    // 每分支独立重调 getCurCharacInvenR（访问器结果禁缓存，战术 P183/P48）；
    // MakeItemPacket != 1 → 返回 0（不发包），否则 finalize + SendPacket 返回 1。
    switch (space)
    {
    case 1:
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)2, slot, packet) != 1)
            return 0;
        break;
    case 7:
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)3, slot, packet) != 1)
            return 0;
        break;
    case 3:
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)0, slot, packet) != 1)
            return 0;
        break;
    default:
        if (getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet) != 1)
            return 0;
        break;
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return SendPacket(target, packet);
}

// 0x0868bc7c
// ORIG：无角色 → -1；无专家职业 → 0；否则按 GetExpertJobScript 的
// isBoundaryExpValue/GetLevel（_ZN17STExpertJobScript*）计算等级，
// 边界值时用 ExpertJobEtcScript 内 map<uchar,short>（等级→所需角色等级）修正：
// 查不到或角色等级不足 → 等级 -1。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868bc7c | _ZN5CUser20GetCurExpertJobLevelEi
int CUser::GetCurExpertJobLevel(int exp)
{
    if (getCurCharacR() == 0)
        return -1;
    if (GetCurCharacExpertJobType() == 0)
        return 0;
    STExpertJobScript* script = (STExpertJobScript*)G_CDataManager()->GetExpertJobScript(GetCurCharacExpertJobType());
    if (script == 0)
        return 0;
    char isBoundary = script->isBoundaryExpValue(exp);
    int level = script->GetLevel((unsigned int)exp);
    if (isBoundary != 0)
    {
        void* etc = G_CDataManager()->GetExpertJobEtcScript();
        // 推断：etc 对象首部为 std::map<uchar,short>（等级 → 所需角色等级）
        if (etc != 0)
        {
            std::map<unsigned char, short>* m =
                (std::map<unsigned char, short>*)etc;
            std::map<unsigned char, short>::iterator it =
                m->find((unsigned char)level);
            if (it != m->end() && it->second <= get_charac_level())
                return level;
        }
        level = level - 1;
    }
    return level;
}

bool CUser::is_clear_stealingSkillMission() const
{
    // ORIG：GetCharacExpandData(8) 结果暂存 -0xc(%ebp) 后调
    // CMissionList_Charac::isClearStealingSkillMission
    //（mission_list_is_clear 为 stub 返回 false，禁用）
    void* data = GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    return reinterpret_cast<CMissionList_Charac*>(data)
        ->isClearStealingSkillMission();
}

// ORIG T 0x08679736：公会技能公告。文案 = ServerString(0x3ea) + "%d "(count) +
// skillName + ServerString(0x3eb)；总长 ≥0x78 丢弃；公会公告板写入
// (boardNo=0, charNo, job=-1, growType=-1, guildKey)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08679a36 | _ZN5CUser21makeGuildSkillMessageEPKci
void CUser::makeGuildSkillMessage(const char* skillName, int count)
{
    if (skillName == 0)
        return;
    char countStr[256];
    memset(countStr, 0, sizeof(countStr));
    std::string body = g_ServerString_.GetServerString(0x3ea, 0);
    sprintf(countStr, "%d ", count);
    body += countStr;
    body += std::string(skillName);
    body += g_ServerString_.GetServerString(0x3eb, 0);
    if (body.length() >= 0x78)
        return;
    char board[120];
    memset(board, 0, sizeof(board));
    memcpy(board, body.c_str(), body.length());
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    if (proxy != 0)
        proxy->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(),
                                   -1, -1, (unsigned int)get_charac_guildkey());
}

// ============================================================================
// CUser::makeGuildItemUpgradMessage @ 0x8679cee（返回 1 已广播 / 0 未达条件）
//   公会强化公告。isAmplify && to>10 用 "+to itemName"，否则 from>10 用
//   "+{from+1} itemName"。消息 = GetServerString(0x3e8) + 数字串 +
//   GetServerString(0x3ec 强化 / 0x3ed 降级) + 角色名；长度>0x77 不发，
//   否则经 CGuildServerProxy::SendGuildBoardWrite 广播。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08679cee | _ZN5CUser26makeGuildItemUpgradMessageEPKciib
void CUser::makeGuildItemUpgradMessage(const char* itemName, int from,
                                       int to, bool isAmplify)
{
    // ORIG T 0x08679cee 全录（汇编+Ghidra C 双段读完）：返回类型 void
    // （epilogue 0x867a236 无 %eax 写，0/1 仅内部 %ebx goto 判定——Main 已落
    // CUser.h 声明 void）。结构：入口 board[120] rep stos 清零 →
    // **双分支整段复制**（4.1 无跨分支复用，gap196 主因）：
    //   分支1 isAmplify&&to>10：name/pre(0x3e8)/suf(0x3ec) 三 string 构造 →
    //     msg[0xff] memset → sprintf("+%d %s",to) → pre+=msg → pre+=suf →
    //     name+=pre → len>0x77 seta 物化 → !tooLong: memcpy(board,c_str,len)
    //     （len 先算存寄存器再 c_str，Ghidra sVar2/pvVar3 序）→ sent=1；
    //   分支2 !isAmplify&&from>10：同构 key=0x3ed、"+%d %s" from+1（lea 0x1）；
    //   两分支块末 dtor suf/pre/name 反序链 + EH resume pad →
    //   共享 SEND 尾（0x867a1c7）：guildkey、characNo、group 三取后
    //   GetServerProxy、SendGuildBoardWrite(board,0,characNo,-1,-1,guildkey)。
    // 【N 封顶（Main 批准 2026-09-05）】语义/调用序/清零形全对齐；残差：
    //   slot 漂移 12B（4.4 memcpy 临时落槽 vs 4.1 寄存器承载，N76/N82 族）、
    //   sent 二次归一化 `test;jne;mov$0;jmp;mov$1` 岛（N7 channel-predicate 形）、
    //   EH pad 布局。
    // 【2026-09-05 21:0x 事故重建】本函数体曾被 StringResource 批次误写覆盖，
    // 从 HEAD(batch-10) 恢复后按会话记录重放。
    char board[0x78] = {0};
    // ORIG 无顶部初始化（%ebx 仅分支内 0x8679e9d mov$0 / 0x8679e43 mov$1 两路
    // 径赋值后共享 dtor 尾）→ if/else 双赋值形，无 sent=0 预写。
    int sent;
    if (isAmplify && to > 10)
    {
        std::string name(getCurCharacName());
        std::string pre = g_ServerString_.GetServerString(0x3e8, 0);
        std::string suf = g_ServerString_.GetServerString(0x3ec, 0);
        char msg[0xff] = {0};   // ORIG 0x8679dd0 非对齐 peel+rep stos 展开
        sprintf(msg, "+%d %s", to, itemName);
        pre += msg;
        pre += suf;
        name += pre;
        bool tooLong;
        tooLong = (name.length() > 0x77);   // ORIG 0x8679e93 seta %al 物化
        if (tooLong) {
            sent = 0;
        } else {
            memcpy(board, name.c_str(), (unsigned int)name.length());
            sent = 1;
        }
    }
    else if (!isAmplify && from > 10)
    {
        std::string name(getCurCharacName());
        std::string pre = g_ServerString_.GetServerString(0x3e8, 0);
        std::string suf = g_ServerString_.GetServerString(0x3ed, 0);
        char msg[0xff] = {0};   // ORIG 0x867a02c 同形第二份
        sprintf(msg, "+%d %s", from + 1, itemName);   // ORIG 0x867a08a lea 0x1(%eax)
        pre += msg;
        pre += suf;
        name += pre;
        bool tooLong2;
        tooLong2 = (name.length() > 0x77);
        if (tooLong2) {
            sent = 0;
        } else {
            memcpy(board, name.c_str(), (unsigned int)name.length());
            sent = 1;
        }
    }
    else
    {
        return;
    }
    if (sent == 0)
        return;
    unsigned int guildKey = (unsigned int)get_charac_guildkey();
    unsigned int characNo = (unsigned int)getCurCharacNo();
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    proxy->SendGuildBoardWrite(board, 0, characNo, -1, -1, guildKey);
}

// ORIG 0x086787fc：改成长类型主分支（set_grow_type.md 汇编+反编译 C 全文核对）。
// 校验 first<6/second<3；second==0 重做任务列表(0x1f→0x20)；reason==1||second==0
// 清 QP；写历史日志；setCurCharGrowthType → enableSaveCharacView → second==0 时
// adjust_charac_stat + state==3 回 ENUM_USERINFO_ADDITION 包；reason∈{2,0} 且
// second==0 时双树清空重建+CSkillChanger+send_skill_info；get_give_skill 取技能表
// growtype_skill 双树（buf!=null 编码回填双树计数块）；成功后 _UpdateUserInfo4Guild
// + Guild/Monitor SendCharLevelGrowType + processGrowTypeEvent。
// name 实为 InterfacePacketBuf*（CUserOnlyOrig.cpp:4952/4977 传 &ack 需双树编码回填；null=直接应用）。
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x086787fc | _ZN5CUser13set_grow_typeEhhPc21eChangeGrowTypeReason
void CUser::set_grow_type(unsigned char firstGrow, unsigned char secondGrow,
                          char* name, eChangeGrowTypeReason reason)
{
    if (firstGrow >= 6 || secondGrow >= 3)
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x713b, "User %s - grow type < 1 || grow type >= GROW_TYPE_NUMBER",
            getCurCharacName());
        return;
    }

    if (secondGrow == 0)
    {
        // ORIG：主职业变更 → 使命列表 kind 0x1f 移除、0x20 重建。
        CMissionList_Charac* ml =
            (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        ml->Remove_Mission(0x1f);
        ml->MakeMissionList_JustKind(*this, 0x20);
    }
    if (reason == (eChangeGrowTypeReason)1 || secondGrow == 0)
    {
        CQuestShop* questShop =
            (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
        if (questShop != 0)
        {
            questShop->clearQP(this);
            questShop->sendCharacQp(this, 1);
        }
    }

    // ORIG:if/else 两分支各自调用 getCurCharacGrowType/getCurCharacVill/get_charac_job。
    if (secondGrow == 0)
    {
        char oldGrow = getCurCharacGrowType();
        m_historyLog.ChangeGrowType(
            get_charac_job(), (int)(signed char)getCurCharacVill(),
            (int)firstGrow, (int)(signed char)oldGrow, 1, reason);
    }
    else
    {
        char oldGrow = getCurCharacGrowType();
        m_historyLog.ChangeGrowType(
            get_charac_job(), (int)(signed char)getCurCharacVill(),
            (int)secondGrow, (int)(signed char)oldGrow, 2, reason);
    }
    InterfacePacketBuf* buf = (InterfacePacketBuf*)name;
    setCurCharGrowthType((char)firstGrow, (char)secondGrow);
    m_characterView.enableSaveCharacView();
    if (secondGrow == 0)
    {
        // ORIG：主职业切换重算属性；角色选择视口（state==3）回 ADDITION 包。
        adjust_charac_stat();
        if (m_chState == 3)
        {
            PacketGuard pg;
            InterfacePacketBuf* pkt = (InterfacePacketBuf*)&pg;
            pkt->clear();
            pkt->put_header(0, 2);
            pkt->put_byte(1);
            pkt->put_short(1);
            make_basic_info((char*)pkt, (char)1);
            pkt->finalize(true);
            Send(pg);
        }
    }

    std::vector<std::pair<int, int> > giveVec;
    char giveOk = 0;
    // ORIG:每 get_give_skill 站点独立调用 G_CDataManager() 并解引 +0x14。
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        giveOk = 1;
    }
    else if (reason == (eChangeGrowTypeReason)2 ||
             reason == (eChangeGrowTypeReason)0)
    {
        if (secondGrow == 0)
        {
            // ORIG：双树清空重建（addSkillOnCreateCharacter 树 A +0x46 / 树 B +0x1de）、
            // CSkillChanger 初始化 + send_skill_info 后按 get_give_skill(...,type 1)。
            // ORIG:clear/set_parent/addSkill 各自调用 getCurCharacSkillW()。
            ((SkillSlot*)getCurCharacSkillW())->clear_all_skills_both();
            ((SkillSlot*)getCurCharacSkillW())->set_parent(this);
            addSkillOnCreateCharacter(
                ((SkillSlot*)getCurCharacSkillW())->m_skillSlot, get_charac_job());
            addSkillOnCreateCharacter(
                ((SkillSlot*)getCurCharacSkillW())->m_skillSlot2, get_charac_job());
            {
                WongWork::CSkillChanger changer;
                changer.SkillInitialize(this, 0, false);
            }
            send_skill_info();
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, 0, giveVec, 1);
        }
        else
        {
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, secondGrow, giveVec, 2);
        }
    }
    else if (reason == (eChangeGrowTypeReason)1)
    {
        // ORIG:secondGrow==0 与否则两站点各自调用。
        if (secondGrow == 0)
        {
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, 0, giveVec, 1);
        }
        else
        {
            giveOk = G_CDataManager()->m_characList->m_characs[get_charac_job()].get_give_skill(
                firstGrow, secondGrow, giveVec, 0);
        }
    }
    if (giveOk == 1)
    {
        // ORIG:pair<int,int> 默认构造站点(local_5c)。
        std::pair<int, int> pairLocal5c;
        if (buf == 0)
        {
            // ORIG：直接应用 —— 每项双树 growtype_skill。
            for (size_t i = 0; i < giveVec.size(); ++i)
            {
                // ORIG:每字段读独立 operator[](second→first 序),ix ×4/迭代。
                int second0 = giveVec[i].second;
                int first0 = giveVec[i].first;
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                    get_charac_job(), first0, second0,
                    (ENUM_SKILL_TREE_KIND)0);
                int second1 = giveVec[i].second;
                int first1 = giveVec[i].first;
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                    get_charac_job(), first1, second1,
                    (ENUM_SKILL_TREE_KIND)1);
            }
        }
        else
        {
            // ORIG:树 0 / 树 1 两个显式循环(非 tree 变量循环)。
            {
                int countIdx = buf->get_index();
                int cnt = 0;
                buf->put_byte(0);
                for (size_t i = 0; i < giveVec.size(); ++i)
                {
                    const std::pair<int, int>& pr = giveVec[i];
                    int r = ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                        get_charac_job(), pr.first, pr.second,
                        (ENUM_SKILL_TREE_KIND)0);
                    if (r > 0)
                    {
                        buf->put_byte(r);
                        buf->put_byte(pr.first);
                        buf->put_byte(pr.second);
                        ++cnt;
                    }
                }
                buf->put_byte(countIdx, cnt);
            }
            {
                int countIdx = buf->get_index();
                int cnt = 0;
                buf->put_byte(0);
                for (size_t i = 0; i < giveVec.size(); ++i)
                {
                    const std::pair<int, int>& pr = giveVec[i];
                    int r = ((SkillSlot*)getCurCharacSkillW())->growtype_skill(
                        get_charac_job(), pr.first, pr.second,
                        (ENUM_SKILL_TREE_KIND)1);
                    if (r > 0)
                    {
                        buf->put_byte(r);
                        buf->put_byte(pr.first);
                        buf->put_byte(pr.second);
                        ++cnt;
                    }
                }
                buf->put_byte(countIdx, cnt);
            }
        }
        _UpdateUserInfo4Guild();
        // ORIG:guild/monitor 两段各自重调 getCurCharacGrowType/get_charac_level/
        // get_charac_no/get_acc_id/GetServerGroup(无共享局部)。
        char guildGrow = getCurCharacGrowType();
        short guildLevel = (short)get_charac_level();
        unsigned int guildCharacNo = (unsigned int)get_charac_no(-1);
        unsigned int guildAccId = get_acc_id();
        CGuildServerProxy* guild =
            GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
        if (guild != 0)
            guild->SendCharLevelGrowType(guildAccId, guildCharacNo,
                                         guildLevel, (short)guildGrow);
        char monGrow = getCurCharacGrowType();
        short monLevel = (short)get_charac_level();
        unsigned int monCharacNo = (unsigned int)get_charac_no(-1);
        unsigned int monAccId = get_acc_id();
        CMonitorServerProxy* monitor =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        if (monitor != 0)
            monitor->SendCharLevelGrowType(monAccId, monCharacNo,
                                           monLevel, (short)monGrow);
        processGrowTypeEvent(firstGrow, secondGrow, reason);
    }
    else
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__, 0x720c,
            "User %s - CUser::set_grow_type first_grow_type %d, second_grow_type %d",
            getCurCharacName(), (int)firstGrow, (int)secondGrow);
    }
}

// ---- 基础访问器（G1-3 桩替换） ----
unsigned int CUser::get_acc_id() const
{
    return m_accId;
}

const char* CUser::get_acc_name() const
{
    return m_accName;
}

int CUser::get_state()
{
    return m_chState;
}

void CUser::set_state(ch_state state)
{
    // ORIG 0x867edb2-0x867ee27（probe s13 实证）：`if(cur==0) goto END` 产
    // `test;sete;test;jne` 物化岛；bFight 绑 asm("al")（call 结果原位 no-op）
    // 末用点 4.4 出 `movzbl %al,%ebx` 单条跨 call 持值（技巧 22b 新形）；
    // invenR/GetCreatureMgrW 全链式零槽（技巧 37）；尾 `call;jmp; nop; epilogue`。
    m_chState = state;
    if (m_chState == 5)
        ClearItemSellCount();
    if (getCurCharacR() == 0)
        goto ST_END;
    {
        // b28d-solo-CUser：ORIG `call is_fighting; movzbl %al,%ebx` 无 bool 槽（跨
        // getCurCharacInvenR/GetCreatureMgrW 保活 %ebx）。具名 bFight 落 -0x9 ⇒ 内联实参。
        ((CInventory*)getCurCharacInvenR())
            ->GetCreatureMgrW()
            ->TurnStomach(is_fighting());
    }
ST_END:;
}

unsigned int CUser::GetUID() const
{
    // ORIG 0x80c8c9c movzwl 704a6,%eax；0x80c8ca3 cwtl；0x80c8ca4 movzwl %ax,%edx。
    // b28d-solo-CUser N封顶: 4.4 折叠 `(unsigned short)(short)` 中间 cwtl（探针 6 形：
    //   具名 rawHi→落槽、cast 链/语句表达式均无 cwtl），4.1 保留三段 = N18a 折叠族。
    return (unsigned short)(short)m_field704a6 | ((int)(short)m_slotIDX << 16);
}

unsigned short CUser::get_unique_id() const
{
    return (unsigned short)m_field704a8;
}

CDungeonGainedGold* CUser::getDungeonGainedGold()
{
    return &m_dungeonGainedGold;
}

WongWork::CHackAnalyzer* CUser::getHackAnalyzer()
{
    return &m_hackAnalyzer;
}

bool CUser::getGmQuestFlag()
{
    return m_gameOption.m_gmQuestFlag;
}

int CUser::GetAccountLastPlayTime()
{
    return m_premium.m_lastPlayTime;
}

ENUM_SERVER_GROUP CUser::GetServerGroup() const
{
    return (ENUM_SERVER_GROUP)m_postChecksumData.m_serverGroup;
}

CExpandEquipslot* CUser::GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE type) const
{
    int frame_pad; (void)frame_pad;   // ORIG 0x80dd587 sub $0x18：调用点 2 outgoing 槽 + 16B 对齐（4.1 帧舍入），dead 局部无指令
    return m_characExpand.GetData(type);
}

CExpandEquipslot* CUser::GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE type) const
{
    int frame_pad; (void)frame_pad;   // 值版重载无 ORIG 对应符号；与引用版（0x828b5de sub $0x18）保持一致
    return m_characExpand.GetDataR(type);
}

CExpandEquipslot* CUser::GetCharacExpandDataR(ENUM_CHARAC_EXPAND_TYPE& type) const
{
    int frame_pad; (void)frame_pad;   // ORIG 0x828b5de sub $0x18：2 outgoing 槽帧，4.4 同-TU 调用只舍到 0x8，dead 局部补 16B（无指令）
    return m_characExpand.GetDataR(type);
}

// ---- quest point / piece helpers（ORIG：CUser 0x868ad88 / 0x868afd0 / 0x868adc2 / 0x868af66）----

void CUser::clearInitQuestPoint()
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->setInitQp(0);
    }
    return;
}

void CUser::clearQuestPiece()
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->setPiece(0);
    }
    return;
}

void CUser::incInitQuestPoint(int val)
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->incInitQp(val);
    }
    return;
}

void CUser::incQuestPiece(int val, int piece, bool send)
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    // ORIG 0x868af8c：双 early-return 形（probe k2 实证）——`je nop; body; je leave;
    // call; jmp leave; nop; leave`。getPiece 结果 spill -0xc dead（技巧 8）+
    // shop=-0x10（技巧 21 先声明远槽）。
    if (shop == 0)
        return;
    int deadPiece = shop->getPiece();
    (void)deadPiece;
    shop->addPiece(val);
    if (!send)
    {
        shop->sendCharacQuestPiece(this);
        return;
    }
}

void CUser::gainWinPoint(int winPoint, eWPAddReason reason)
{
    // ORIG _ZN5CUser12gainWinPointEi12eWPAddReason @0x0864fd2c。
    if (winPoint != 0)
    {
        // ORIG 0x864fd2c：call 结果留 %eax 直读直存（`mov 0x58(%eax),%edx; add; mov %edx,0x58(%eax)`
        // 无具名槽——链式形；具名 wpW/wpR 反产 spill 槽撑帧 0x28）。
        getPVPResultW()->m_winPoint += winPoint;
        m_historyLog.WPAdd(getPVPResultR()->m_winPoint, winPoint, reason);
    }
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868adfc | _ZN5CUser17setInitQuestPointEv
void CUser::setInitQuestPoint()
{
    // ORIG 0x868adfc T：清空 init QP 与 piece，再按 NPC 任务表逐条累加已清任务奖励点。
    clearInitQuestPoint();
    clearQuestPiece();

    std::multimap<int, int>* npcList =
        G_CDataManager()->m_questList->getQuestNPCList(ENUM_QUEST_GRADE_NONE);
    if (npcList == 0)
        return;

    for (std::multimap<int, int>::const_iterator it = npcList->begin();
         it != npcList->end(); ++it)
    {
        Quest* quest = G_CDataManager()->find_quest(it->second);
        if (quest != 0)
        {
            WongWork::CQuestClear* cleared =
                (WongWork::CQuestClear*)((char*)getCurCharacQuestR() + 4);
            if (cleared->isClearedQuest((unsigned int)it->second))
            {
                incInitQuestPoint(quest->m_field138);
                incQuestPiece(quest->m_field13a, quest->m_index, true);
            }
        }
    }
}

WongWork::CUserPremium* CUser::GetPremiumInfo() const
{
    return (WongWork::CUserPremium*)&m_premium;
}

// ORIG W 0x0864f8e0：mov 0x854(%eax),%eax 直读 +0x854（outPremiumCount）。
int WongWork::CUserPremium::GetAdvantageCoin() const
{
    return m_outPremiumCount;   // +0x854（ORIG mov 0x854(%eax) 直读；m_pad 字段化后此越界读改具名成员）
}

// ORIG 0x08694a54：movzbl 0x85c(%eax),%eax 直读 +0x85c（overEquipFlag byte）。
bool WongWork::CUserPremium::isAffectedUnlimitFatigue() const
{
    return m_overEquipFlag;
}

UserQuest* CUser::getCurCharacQuestR() const
{
    return (UserQuest*)&m_quest;
}

// ORIG W 0x0814aa5e：enableSaveQuest() 后返回 m_quest（+0x858d4）。
UserQuest* CUser::getCurCharacQuestW()
{
    enableSaveQuest();
    return (UserQuest*)&m_quest;
}

void CUser::DecreaseGuildPowerWarPoint(unsigned int point)
{
    m_guildDB.m_powerWarPoint -= point;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867ed68 | _ZN5CUser11is_fightingEv
bool CUser::is_fighting()   // CUser.h:2556 已 bool 化（Main 落，ORIG 0x867ed68 尾 mov$0/$1 直返 + caller test %al 无 setne）
{
    register int bit;   // 跨标签 goto 用无初始化 POD（P177）
    // ORIG 0x867ed68：无 state/bit 槽（P0 全寄存器直通）——ja 直跳后置 zero 块、
    // 两 mask 岛（mov$1/mov$0）后置共享（goto 形复现 jcc 方向与块序）；
    // (bit&0xdf)!=0 与 fall 同值返 0 = 死块照抄（P171）。残差：ORIG shl 源 eax
    // 单读直通 + ebx→eax→edx 双中转（N18a/寄存器直通跨版本）。
    if ((unsigned)m_chState > 10)
        goto zero;
    bit = 1 << (int)m_chState;
    if ((bit & 0x720) != 0)
        goto one;
    if ((bit & 0xdf) == 0)
        goto zero;
    return false;
one:
    return true;
zero:
    return false;
}

void CUser::set_before_area(int area)
{
    m_field8cfb8 = area;
}

void CUser::set_area(int area)
{
    int frame_pad; (void)frame_pad;   // ORIG 0x8647867 sub $0x18：2 outgoing 槽 + 16B 帧舍入（4.1 特征），dead 局部无指令
    set_before_area(m_field8cfbc);
    m_field8cfbc = area;
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x086813be | _ZN5CUser8get_areaEb
int CUser::get_area(bool param)
{
    if (param) {
        if (G_GameWorld()->IsSchoolPvPChannel() &&
            getCurCharacVill() == 2 &&
            m_field8cfc0 == 4) {
            return m_field8cfc0;
        }
    }
    if (param && getCurCharacVill() == 8) {
        if (m_field8cfc0 == 0 || m_field8cfc0 == 1 ||
            m_field8cfc0 == 2 || m_field8cfc0 == 3) {
            return m_field8cfc0;
        }
    }
    return m_field8cfbc;
}

int CUser::get_pvp_WinningRate_relateMission() const
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    return mission->getWinningRate();
}

int CUser::get_aura_avatar_option_value(int idx)
{
    // ORIG：判空 = 4.1 bool 物化 `test eax;setne al;test;je`（技巧 22 形，指针
    // 结果免槽）；界检查单条无符号 `cmp $2; jg`（(unsigned)idx>2 吸收负数）；
    // 取值**二次** getCurCharacR call（结果直用 `add $0x4a0; mov 0x1(%eax,%edx,4)`
    // scaled 单级，scaled 分解残差记 N）。
    // b28 重构（P0/N96 去绑定）：界检查 `>=0 && <=2` 正向包裹 ⇒ 4.4 产
    // `cmpl $0,mem; js BAD; cmpl $2,mem; jg BAD` 双跳共享尾 -1 块（P222 族）。
    if (getCurCharacR() == 0)
        return -1;
    if (idx >= 0 && idx <= 2)
        return getCurCharacR()->m_auraValues[idx];
    return -1;
}

void CUser::SetTradeSpace(int idx)
{
    lock();
    m_tradeSpaceIdx = (short)idx;
    unlock();
}

bool CUser::CheckFatigue()
{
    if (m_accId == 0)
        return true;
    return getCurCharacTotalFatigue() < getCurCharacTotalMaxFatigue();
}

// ORIG T 0x0864f7fe sz0xeb：扣除 1 点疲劳（先消耗高级会员疲劳额度）。
//   min(premiumFatigue, maxPremiumFatigue) → 不足时回落到普通疲劳。
// b28d-lane5 N封顶: N97/P230 cmov; N82/P112 槽号分配 | ORIG 0x0864f7fe | _ZN5CUser20gainCurCharacFatigueEi
void CUser::gainCurCharacFatigue(int value)
{
    unsigned int maxPrefatigue = 0;
    if (m_premium.GetAdvantageFatigueRate() != 0)
        maxPrefatigue = getCurCharacMaxPremiumFatigue() & 0xffff;

    unsigned int prefatigue = getCurCharacPremiumFatigue() & 0xffff;
    // ORIG 0x864f854 调用 std::min<int>(int const&, int const&)（外部函数调用，
    // 非三元/内联）——写显式 std::min<int> 以产出同符号调用点。
    int premiumVal = std::min<int>(prefatigue, maxPrefatigue);
    int remain = (int)premiumVal - value;
    if (remain < 0)
    {
        if ((int)premiumVal > 0)
            setCurCharacPremiumFatigue(0);
        unsigned int normal = getCurCharacFatigue() & 0xffff;
        int sum = (int)normal + remain;
        setCurCharacFatigue(sum < 0 ? 0 : sum);
    }
    else
    {
        setCurCharacPremiumFatigue((unsigned short)(premiumVal - (unsigned int)value));
    }
}

short CUser::has_within_Mission() const
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    // ORIG 0x8662739 call 后直 leave;ret（无归一），调用点 0x865d433 族 `test %ax,%ax`（16 位判）⇒ short 直返
    return mission->getIndex_byKind(0x1b);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x086552e6 | _ZN5CUser13getBlueMarbleEv
void* CUser::getBlueMarble()
{
    // ORIG 0x8??：无 idx 槽——`movzwl mem,%eax; test %ax,%ax; js`（ushort 零装载
    // + js 符号判、body fall-through）+ 二次装载 `movswl %ax,%ebx`。关键：short→int
    // 的拓宽必须**作为普通实参直传**（隐式转换），4.4 才会在 %ax 上直接出 2 操作数
    // movswl 并入 callee-saved %ebx（跨 G_CGameManaging 调用存活）；若写成
    // `register int idx asm("ebx")` 中间变量则 4.4 退化成 cwtl+mov（probe s3 实证）。
    // 尾 `call;jmp merge` + `mov $0` 尾岛（goto 钉块序，技巧 23）。
    if (m_field8d00c < 0)
        goto RET0;
    return G_CGameManager()->getBlueMarble(m_field8d00c);
RET0:
    return 0;
}

void CUser::checkLogOutCorrectly()
{
    m_guildAgitDB.m_pad2[0] = 1;   // +0x8d0df（ORIG 单指令 movb 写；组件视图折叠）
}

bool CUser::find_pvp_masterid_walkingout_me(unsigned int id)
{
    if (m_set8cfe4.empty())
        return false;
    // ORIG 0x865cf32 `test;je → mov $1 尾岛`：eq + 命中返常量 1（技巧 17 fall 块在前 +
    // 技巧 30 尾岛）；`return *it` 反产 pt/deref 块。
    std::set<unsigned int>::iterator it = m_set8cfe4.find(id);
    if (it == m_set8cfe4.end())
        return false;
    return true;
}

void CUser::insert_pvp_masterid_walkingout_me(unsigned int id)
{
    m_set8cfe4.insert(id);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0865cfac | _ZN5CUser20update_old_pvp_pointEv
void CUser::update_old_pvp_point()
{
    // ORIG 0x8??：writable `%mov eax,%ebx` 单条跨 call 存活；P0/N96 去绑定改
    // 链式单语句（LHS receiver 跨 RHS call 保活，P52），残差记 N18a。
    getPVPResultW()->m_oldExpPoint = getPVPResultR()->m_expPoint;
}

void CUser::SetSaveRentalInfoToExchange(bool flag)
{
    m_rentalFlag1 = flag;
}

void CUser::DeleteRentalItemInfo(int idx)
{
    m_rentalInfo[idx].clear();
}


bool CUser::isAffectedPremium(ENUM_PREMIUM_TYPE type) const
{
    return m_premium.CheckPremium((int)type);
}

bool CUser::IsPermissionPrivateStore()
{
    return isAffectedPremium((ENUM_PREMIUM_TYPE)8);
}

// CMaxFatigueFactorEvent：ORIG 仅 GetMaxFatigueFactor（W 0x0812ce54）被消费，
// 权威类未建模；本文件以最小类声明 + reinterpret_cast 视图消费（GetRepeatEvent(2)
// 返回的 CEventBase* 实际指向该事件对象）。
class CMaxFatigueFactorEvent
{
public:
    int GetMaxFatigueFactor();   // ORIG W _ZN22CMaxFatigueFactorEvent19GetMaxFatigueFactorEv
};

// 0x08657ada
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08657ada | _ZN5CUser14RecoverFatigueEi
void CUser::RecoverFatigue(int value)
{
    // ORIG 0x08657ada：角色为空直接返回（跳过全部）；否则先写回
    // premium 疲劳（ushort 截断）与当前疲劳
    if (getCurCharacR() == 0)
        return;
    setCurCharacPremiumFatigue((unsigned short)value);
    setCurCharacFatigue(value);

    // ORIG 0x08657ada：学校点非零 → DB 上报并清零
    if (getCurCharacSchoolPoint() != 0)
    {
        DB_UpdateSchoolPoint::makeRequest(get_acc_id(),
                                          getCurCharacSchoolPoint(), GetUID());
        resetCurCharacSchoolPoint();
    }

    // ORIG：事件 2（CMaxFatigueFactorEvent）活跃 → m_maxFatigue = factor*DEFAULT/100，
    // 否则 DEFAULT_MAX_FATIGUE。
    CEventBase* ev2 = GlobalData::s_event_manager->GetRepeatEvent(2);
    _Charac_info* cur = getCurCharacW();
    if (cur != 0)
    {
        if (ev2 != 0 && ev2->IsEventing((CUser*)0))
        {
            cur->m_field35 =
                (unsigned short)(
                    reinterpret_cast<CMaxFatigueFactorEvent*>(ev2)
                        ->GetMaxFatigueFactor()
                    * DEFAULT_MAX_FATIGUE / 100);
        }
        else
        {
            cur->m_field35 = (unsigned short)DEFAULT_MAX_FATIGUE;
        }
        if (IsHavePremiumAdvantage())
        {
            if (ev2 != 0 && ev2->IsEventing((CUser*)0))
            {
                // base = (short)(factor*DEFAULT/100) - (short)DEFAULT
                short base = (short)(
                        reinterpret_cast<CMaxFatigueFactorEvent*>(ev2)
                            ->GetMaxFatigueFactor()
                        * DEFAULT_MAX_FATIGUE / 100)
                           - (short)DEFAULT_MAX_FATIGUE;
                setCurCharacMaxPremiumFatigue(
                    (unsigned short)(
                        (unsigned short)GetPremiumInfoW()->GetAdvantageFatigueRate()
                            * DEFAULT_MAX_FATIGUE / 100
                      + (unsigned short)base
                            * (unsigned short)GetPremiumInfoW()->GetAdvantageFatigueRate()
                            / 100));
            }
            else
            {
                setCurCharacMaxPremiumFatigue(
                    (unsigned short)(
                        (unsigned short)getCurCharacMaxFatigue()
                            * (unsigned short)GetPremiumInfoW()->GetAdvantageFatigueRate()
                            / 100));
            }
        }
    }

    // ORIG：事件 0x57 活跃（ORIG 无空检查）→ m_maxFatigue += DataManager 0x51c；
    // rate!=0 且 PC 房 → premium = rate*DEFAULT/100 + 0x51c，否则 rate*DEFAULT/100。
    if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing((CUser*)0))
    {
        _Charac_info* curW = getCurCharacW();
        curW->m_field35 = (unsigned short)(
            // dm+0x51c = SPS(+0x68)+0x4b4 = m_weekendBouns4b4.m_bonusFatiguePercent
            curW->m_field35
            + *reinterpret_cast<const unsigned short*>(
                &G_CDataManager()->m_serverParameter.m_weekendBouns4b4
                     .m_bonusFatiguePercent));
        if (GetPremiumInfoW()->GetAdvantageFatigueRate() != 0
            && GetPCRoomNo() != 0)
        {
            setCurCharacMaxPremiumFatigue(
                (unsigned short)(
                    (unsigned short)GetPremiumInfoW()->GetAdvantageFatigueRate()
                        * DEFAULT_MAX_FATIGUE / 100
                  + *reinterpret_cast<const unsigned short*>(
                        &G_CDataManager()->m_serverParameter.m_weekendBouns4b4
                             .m_bonusFatiguePercent)));
        }
        else
        {
            setCurCharacMaxPremiumFatigue(
                (unsigned short)(
                    (unsigned short)GetPremiumInfoW()->GetAdvantageFatigueRate()
                        * DEFAULT_MAX_FATIGUE / 100));
        }
    }
    else
    {
        if (IsHavePremiumAdvantage())
        {
            setCurCharacMaxPremiumFatigue(
                (unsigned short)(
                    (unsigned short)getCurCharacMaxFatigue()
                        * (unsigned short)GetPremiumInfoW()->GetAdvantageFatigueRate()
                        / 100));
        }
    }
    resetCurCharacUsedFatigue();
}

void CUser::LogHistory(const char* fmt, ...)
{
    // ORIG 0x0867c8d8：日志前缀（Hangame 服 'X' 否则 'D'）+ 固定头（账号/角色/
    // 时间/成长/职业/等级/IP/端口/频道）+ 变参 fmt 追加，经 cHistoryTrace 输出。
    // 布局对齐（ORIG 0x867c8fe 帧 0x4bc）：typeFlag@-0x19、history@-0x1a、
    // timeLog@-0x22、accIdBuf[24]@-0x3a、ap@-0x40、buf[1024]@-0x440，
    // sprintf 实参右→左求值，temp 槽自 -0x470 起。
    char typeFlag;
    if (isHangameUser())
        typeFlag = 'X';
    else
        typeFlag = 'D';
    cHistoryTrace history;
    TimeLog_ timeLog;
    timeLog.setTime();
    char accIdBuf[24];
    memset(accIdBuf, 0, 4);
    strcpy(accIdBuf, NumberToString(get_acc_id(), 7));
    va_list ap;
    char buf[1024];
    sprintf(buf,
        "\"%s\",%s,%c,%02d%02d%02d,\"%s\",%d,%d,%d,%d,%s,%s,%d,%d, ",
        m_accName, NumberToString(get_acc_id(), 0), typeFlag,
        (int)(char)(unsigned char)timeLog.m_hour,
        (int)(char)(unsigned char)timeLog.m_min,
        (int)(char)(unsigned char)timeLog.m_sec,
        getCurCharacR() ? getCurCharacName() : "",
        getCurCharacNo(), get_charac_level(), get_charac_job(),
        (int)getCurCharFirstGrowType(),
        getWebAddress(),
        m_network.GetPeerIP2(reinterpret_cast<char*>(this) + 0x6eef0, 0x10),
        (int)m_network.get_port(),
        G_CEnvironment()->m_serverEnvir.m_channelNo);
    va_start(ap, fmt);
    vsprintf(buf + strlen(buf), fmt, ap);
    va_end(ap);
    history(buf);
}

bool CUser::isBlackUser(unsigned int accId)
{
    if (m_vec8cef4.empty())
        return false;
    for (std::vector<unsigned int>::iterator it = m_vec8cef4.begin();
         it != m_vec8cef4.end(); ++it) {
        if (*it == accId)
            return true;
    }
    return false;
}

bool CUser::IsHavePremiumAdvantage() const
{
    return m_premium.GetAdvPremiumCount() != 0;
}

// b28d-solo-CUser AE: ORIG 0x0867?? `call; test %eax,%eax; setne %al; test %al,%al; je`
//   = 条件内联形（探针 /tmp/probe_bool.cpp a3/a5 逐条同形）；具名 bool 局部 4.4 恒
//   落槽（a2/a4：mov %al,-0x9; cmpb $0,-0x9）⇒ 删局部内联即归位。| _ZN5CUser13IsEquipAvatarEv
bool CUser::IsEquipAvatar()
{
    if (getCurCharacR() != 0)
        return ((const CInventory*)getCurCharacInvenR())->IsEquipAvatar();
    return false;
}

const char* CUser::getWebAddress()
{
    return m_gameOption.m_webAddress;
}

WongWork::CSecurityCard* CUser::getSecurityCard()
{
    return &m_securityCard;
}

CGameOption* CUser::GetGameOptionRef()
{
    return &m_gameOption;
}

Sanicova::CPad* CUser::getPad()
{
    return &m_cpad;
}

long CUser::getBeforeAreaTime() const
{
    return m_field79670;
}

void CUser::setBeforeAreaTime(long t)
{
    m_field79670 = t;
}

int CUser::getLastPacketID()
{
    return m_lastPacketID;
}

void CUser::setLastPacketID(int id)
{
    m_lastPacketID = id;
}

int CUser::getPlayingDungeonIndex()
{
    return m_playingDungeonIdx;
}

void CUser::setPlayingDungeonIndex(int index)
{
    m_playingDungeonIdx = index;
}

UserSpace::T CUser::getMoveSpace() const
{
    return (UserSpace::T)m_field8cfcc;
}

void CUser::setMoveSpace(UserSpace::T space)
{
    m_field8cfcc = space;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08678526 | _ZN5CUser13CheckMoveTownEi
int CUser::CheckMoveTown(int vill)
{
    if (G_GameWorld()->IsPVPChannel())
        return 0;
    // ORIG 0x8678548 `cmpl $0xa,0xc(%ebp); ja`：4.1 对 arg 槽直折无符号比较；
    // 4.4 恒先载 %eax（具名 uv 槽双 spill 更差，probe），保留 cast 形 ndiff=1。
    if (static_cast<unsigned int>(vill) <= 10)   // ORIG: ja 无符号比较
    {
        // N6 消解：直写表达式 `(1 << vill) & 0x580` 被 4.4 前端折成 `sar+and$1`
        // （probe p17.f2）；改具名 bit 局部保 shl 链。裸 `register` 提示（非
        // asm("reg") 绑定，P0/N96 合规）令 4.4 免槽回写，复现 ORIG
        // `mov $1; shl %cl; mov %ebx,%eax; and $0x580` 全寄存器形（同 is_fighting 先例）。
        register int bit;
        bit = 1 << vill;
        if (bit & 0x580)
            return 0;
    }
    	Village* village = G_GameWorld()->GetVillage(vill);
	if (!village)
		return 8;
	if (get_charac_level() < village->m_field2c)
		return 8;
	if (village->m_questIdx != 0)
	{
		if (!getCurCharacQuestR()->isClearQuest(village->m_questIdx))
			return 7;
	}
	return 0;
}

unsigned char CUser::isUseCraneStart() const
{
    return m_field8eb98;
}

void CUser::setUseCraneStart(bool flag)
{
    m_field8eb98 = flag;
}

unsigned char CUser::getForceDropFlag()
{
    return m_gameOption.m_forceDropFlag;
}

void CUser::setForceDropFlag(bool flag)
{
    m_gameOption.m_forceDropFlag = flag;
}

unsigned char CUser::isDungeonClearHackCheck()
{
    if (m_mcap.m_field54c == 3)
        return 1;
    return 0;
}

void CUser::setDungeonClearHackCheck(bool flag)
{
    if (flag) {
        // b28d-solo-CUser AE: ORIG 0x8649bab-0x8649bb4 `movzbl 0x8e93c; mov %eax,%edx;
        //   or $0x1,%edx`（无 cur 槽，sub $0x4）；具名 char cur 落 -0x2 多两段 ⇒ 内联。
        m_mcap.m_field54c = (m_mcap.m_field54c | 1);
    } else {
        m_mcap.m_field54c = (m_mcap.m_field54c | 2);   // ORIG 0x8649bc5-0x8649bce or $0x2,%edx 同形
    }
}

unsigned char CUser::isAllowLocationGeoState()
{
    return m_rentalFlag3;
}

void CUser::setAllowLocationGeoState(bool flag)
{
    m_rentalFlag3 = flag;
}

unsigned char CUser::isComboSkillSet()
{
    return m_field8ec29;
}

void CUser::setComboSkillSet(bool flag)
{
    m_field8ec29 = flag;
}

bool CUser::lock4DataLoad()
{
    // ORIG 0x864a9ac：test %al,%al; je WRITE; mov $0; jmp merge; WRITE: movb $1,0x8d0f0; mov $1
    //（bool 返回 if() 直判；isLocked4DataLoad 定义置于调用点之后以松帧，CDMSB 技巧）
    if (isLocked4DataLoad())
        return 0;
    m_field8d0f0 = 1;
    return 1;
}

bool CUser::unlock4DataLoad()
{
    // ORIG 0x864a9dd：xor $0x1,%eax; test; je WRITE（!bool 取反形）; WRITE: movb $0; mov $1
    if (!isLocked4DataLoad())
        return 0;
    m_field8d0f0 = 0;
    return 1;
}

bool CUser::lockCreateCharac()
{
    if (isLockedCreateCharac())   // ORIG 0x864aa88 test %al;je 同 lock4DataLoad
        return 0;
    m_field8d0f2 = 1;
    return 1;
}

bool CUser::unlockCreateCharac()
{
    if (!isLockedCreateCharac())  // ORIG 0x864aab6 xor$1;test;je 族
        return 0;
    m_field8d0f2 = 0;
    return 1;
}

bool CUser::lockDeleteCharac()
{
    if (isLockedDeleteCharac())   // ORIG 0x864aa1a test %al;je 同族
        return 0;
    m_field8d0f1 = 1;
    return 1;
}

bool CUser::unlockDeleteCharac()
{
    if (!isLockedDeleteCharac())  // ORIG 0x864aa48 xor$1;test;je 同族
        return 0;
    m_field8d0f1 = 0;
    return 1;
}

bool CUser::isLocked4DataLoad()
{
    return m_field8d0f0;   // ORIG 0x864aa0a movzbl 直返（bool 字段兼容）
}

bool CUser::isLockedCreateCharac()
{
    return m_field8d0f2;   // ORIG 0x864aae6 movzbl 0x8d0f2 直返
}

bool CUser::isLockedDeleteCharac()
{
    return m_field8d0f1;   // ORIG 0x864aa78 movzbl 0x8d0f1 直返
}

STExpReward CUser::get_growth_power_exp_reward()
{
    // ORIG 把 CUser+0x8d24d 的 dword 直接写入 sret（无 STExpReward ctor，9 insns）。
    // 真值成员 return 直写 = 4.4 唯一复现形（probe g5/g6 实证；cast/引用/临时均 memcpy）。
    return m_reward;
}

void CUser::setAntibotKey(int key)
{
    m_antibotKey = key;
}

void CUser::setClientVersion(unsigned int version)
{
    m_clientVersion = version;
}

unsigned int CUser::getClientVersion()
{
    return m_clientVersion;
}

void CUser::ResetUsedCoinInDungeon()
{
    m_field8eae0 = 0;
}

void CUser::ClearItemSellCount()
{
    m_itemSellCount = 0;
    memset(&m_sellCountData, 0, 0x1c);
}

// ---- 发包族（PacketGuard 局部 + InterfacePacketBuf） ----
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086485ba | _ZN5CUser4SendER11PacketGuard
bool CUser::Send(PacketGuard& packet)   // b28c-L5 SIGFIX：ORIG 串 bool；
                                        // 调用点 0x082e6c2b `xor$1;test%al` bool callee 直判
{
    PacketBuf* pBuf = ((InterfacePacketBuf*)&packet)->get();
    if (pBuf->is_finallized() == 0)
    {
        unsigned short packetId = *((unsigned short*)pBuf->get_packet(1));
        const char* name = getCurCharacName();
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__, 0x8a0,
                              "User %s - Packet.is_finallized packet_id(%d) return false",
                              name, (int)packetId);
        return 0;
    }
    // ORIG 0x086486e5/0x086486ea：两个死 PacketBuf::at() 直调（结果丢弃，P184 死直调保留）
    pBuf->at(0);
    pBuf->at(1);
    unsigned short packetId = *((unsigned short*)pBuf->get_packet(1));
    unsigned char area = pBuf->at(0);
    GetPacketName((ENUM_PACKETCLASS)area, packetId);
    unsigned char b = pBuf->at(1);
    if (b != 6)
    {
        // ORIG 块内 at(1)/at(0) 各重调一次，非复用 b/area
        m_pad8cf0c[0] = pBuf->at(1);
        m_pad8cf0c[1] = pBuf->at(0);
    }
    return AntibotSend(*pBuf);
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867b8fe | _ZN5CUser10SendPacketENS_11eSendTargetER11PacketGuard
bool CUser::SendPacket(eSendTarget target, PacketGuard& packet)  // b28d-lane5：CUser.h 已落 bool（Main），定义同步
{
    if (target == (eSendTarget)1)
        Send(packet);
    else if (target < (eSendTarget)2)
    {
        if (target == (eSendTarget)0)
            G_GameWorld()->send_all(packet, this);
    }
    else if (target == (eSendTarget)2)
    {
        CParty* pParty = (CParty*)GetParty();
        if (pParty == 0)
            Send(packet);
        else
            pParty->send_to_party(packet);
    }
    else if (target == (eSendTarget)3)
    {
        WarRoom* pRoom = (WarRoom*)GetWarRoom();
        if (pRoom == 0)
            Send(packet);
        else
            pRoom->SendToRoom(packet);
    }
    return 1;
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c1d4 | _ZN5CUser13SendCmdPacketENS_11eSendTargetE14ENUM_CMDPACKETi
bool CUser::SendCmdPacket(eSendTarget target, ENUM_CMDPACKET cmd, int param)  // b28c-L5 SIGFIX（P174）
{
    // ORIG 0x867c1d4 块序（技巧 23 goto 钉）：true 路径 put_byte×2 后 **jmp 到位于
    // false(log) 块之后的共享 finalize+SendPacket 段（0x867c279）**；false 块
    // cmd→%ebx 缓存、logFormat、%ebx=0 汇合 0x867c2c4 dtor 尾。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    if (static_cast<unsigned int>(cmd) != 0x41)
        // P0/N96 去绑定；ORIG `mov 0x10,%eax; cmp $0x41,%eax` reg cmp——
        // P209：4.4 对 unsigned 比较恒先载寄存器（signed enum 直折 mem cmp 反形）。
        goto BAD;
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->put_byte(param);
    goto DO_FIN;
BAD:
    LogManager::logFormat(1, "user.cpp",
                          __PRETTY_FUNCTION__,
                          0x7741, "User %s - CmdID %d invalid", getCurCharacName(),
                          (int)cmd);
    return 0;
DO_FIN:
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return SendPacket(target, packet);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867bea0 | _ZN5CUser15SendCmdOkPacketE14ENUM_CMDPACKET
bool CUser::SendCmdOkPacket(ENUM_CMDPACKET cmd)  // b28d-lane5：ORIG 0x0867bf10 `call Send; mov %eax,%ebx`
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

bool CUser::SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error)  // b28d-lane5
{
    PacketGuard packet;
    return SendCmdErrorPacket(cmd, error, packet);
}

// b28d-lane5 AE: CUser.h 返回 int→bool（Main 代落）+ 定义同步（ORIG 尾 call Send 直返）| ORIG 0x0867c022 | _ZN5CUser18SendCmdErrorPacketE14ENUM_CMDPACKETh8PAY_TYPE
bool CUser::SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error, PAY_TYPE pay)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error);
    ((InterfacePacketBuf*)&packet)->put_byte((int)pay);   // ORIG 写 1 字节（put_int 会破坏包格式）
    ((InterfacePacketBuf*)&packet)->finalize(true);
    // ORIG 0x867c0a0-0x867c0c8：finalize 后 cMyTraceNoop()("CmdID %d, nErrorCode %d", cmd, error)
    // （format @0x8cefc8e；禁追踪 noop 空对象，临时 @-0x9）。1st=CmdID(cmd) 2nd=nErrorCode(error)。
    cMyTraceNoop()("CmdID %d, nErrorCode %d", (int)cmd, (int)error);
    return Send(packet);
}

// b28d-lane5 AE: CUser.h 返回 int→bool（Main 代落）+ 定义同步 | ORIG 0x0867bfae | _ZN5CUser18SendCmdErrorPacketE14ENUM_CMDPACKEThR11PacketGuard
bool CUser::SendCmdErrorPacket(ENUM_CMDPACKET cmd, unsigned char error, PacketGuard& packet)
{
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c110 | _ZN5CUser23SendCmdErrorPacket2ByteE14ENUM_CMDPACKETt
bool CUser::SendCmdErrorPacket2Byte(ENUM_CMDPACKET cmd, unsigned short error)  // b28d-lane5
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_short(error & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x08692824 | _ZN5CUser21SendCmdErrorPacketJpnE14ENUM_CMDPACKETt
bool CUser::SendCmdErrorPacketJpn(ENUM_CMDPACKET cmd, unsigned short error)  // b28d-lane5
{
    PacketGuard packet;
    ARAD::DISPATCHER::make_cmd_packetheader_jpn(packet, cmd, false);
    ((InterfacePacketBuf*)&packet)->put_short(error & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

// ORIG T 0x08680a96：拍卖操作前置门（AuctionPreCheck.md）。
// 控制流：
//   1) pay==1（cera 拍卖）且 CCeraAuctionServerProxy::IsRunning()!=1 → 报 0x8c，返回 0；
//   2) pay==0（普通拍卖）且 CAuctionServerProxy::IsRunning()!=1 → 报 0x8c，返回 0；
//   3) get_state()<3 或 getCurCharacR()==0 → 返回 0x8151；
//   4) CSecu_ProtectionField::Check(this, 0x35) !=0 → 报对应错误，返回 0；
//   5) 全部通过 → 返回 -1。
// b28d-lane5 N封顶: N96+P168a④ 清 `register bool fail asm("al")` 后 bool 暂存落栈槽
// （movb $1,-0xd / cmpb $0,-0xd），ORIG 为寄存器 `mov $1,%eax`；无绑定即不可达（P0 硬约束）。
// | ORIG 0x08680a96 | _ZN5CUser15AuctionPreCheckE14ENUM_CMDPACKET8PAY_TYPE
int CUser::AuctionPreCheck(ENUM_CMDPACKET cmd, PAY_TYPE pay)
{
    // ORIG 0x86??：fail 不落槽（值留存 %al：`xor$1;[物化];test;je`）——register asm(al)。
    bool fail;

    if (pay == (PAY_TYPE)1)
    {
        // ORIG 0x8680aa8-0xac2：inner je 与外层 else **共享** mov $0 岛（技巧 23 goto
        // 钉块序：goto SET0 使 4.4 折叠双 else）。
        if (!GlobalData::s_cera_auction_proxy->IsRunning())
            fail = true;
        else
            goto SET01;
    }
    else
    {
    SET01:
        fail = false;
    }

    if (fail)
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (pay == (PAY_TYPE)0)
    {
        if (!GlobalData::s_auction_proxy->IsRunning())
            fail = true;
        else
            goto SET02;
    }
    else
    {
    SET02:
        fail = false;
    }

    if (fail)
    {
        SendCmdErrorPacket(cmd, 0x8c, pay);
        return 0;
    }

    if (get_state() < 3 || getCurCharacR() == 0)
    {
        return 0x8151;
    }

    unsigned int err =
        GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)0x35);
    if (err != 0)
    {
        SendCmdErrorPacket(cmd, (unsigned char)err, pay);
        return 0;
    }

    return -1;
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867bd84 | _ZN5CUser22SendCmdPeerErrorPacketE14ENUM_CMDPACKET22ENUM_PEER_REQUEST_TYPEh
bool CUser::SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                   unsigned char error)  // b28d-lane5：ORIG 0x0867be6f 保存 4 参版返回值
{
    PacketGuard packet;
    return SendCmdPeerErrorPacket(cmd, type, error, packet);
}

bool CUser::SendCmdPeerErrorPacket(ENUM_CMDPACKET cmd, ENUM_PEER_REQUEST_TYPE type,
                                   unsigned char error, PacketGuard& packet)  // b28d-lane5
{
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(1, (int)cmd);
    ((InterfacePacketBuf*)&packet)->put_byte(0);
    ((InterfacePacketBuf*)&packet)->put_byte(error & 0xff);
    ((InterfacePacketBuf*)&packet)->put_byte((int)type);
    if ((int)type == 4)
        ((InterfacePacketBuf*)&packet)->put_int(0);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    return Send(packet);
}

void CUser::SendZeroExpPacket()
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x107);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

void CUser::SendMaxEquipLevel()
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->clear();
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x1b6);
    ((InterfacePacketBuf*)&packet)->put_short(GetCurCharacMaxEquipLevel());
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

void CUser::SendFatigue()
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x24);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacTotalFatigue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacTotalMaxFatigue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacUsedFatigue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_short(getCurCharacFatigueGrownUpBuff() & 0xffff);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// ============================================================================
// G1-4 批次：索引 / 状态访问器（ORIG 逐符号转录）
// ============================================================================

bool CUser::CheckInTrade()
{
    return (~(unsigned short)m_tradeSpaceIdx & 0x8000) != 0;
}

int CUser::isEnableAvatarSocketAction()
{
    // ORIG _ZN5CUser26isEnableAvatarSocketActionEv @ 0x086810a8
    // 检查链：安全保护(0x11) → 私摊忙 → 幽灵角色 → 交易中 → 袭击区 → PVP 频道。
    // 安全保护命中返回其错误码，其余任一命中返回 0x13（禁止），全部通过返回 0（允许）。
    int ret = GlobalData::s_pSecuProtectionField->Check(this, (SECURITY_PROTCTION)0x11);
    if (ret != 0)
        return ret;
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(this))
        return 0x13;
    if (IsCurCharacGhost())
        return 0x13;
    if (CheckInTrade())
        return 0x13;
    if (GetAssaultPlace() != 0)
        return 0x13;
    if (G_GameWorld()->IsPVPChannel())
        return 0x13;
    return 0;
}

bool CUser::CheckInParty()
{
    return (~(unsigned short)m_field8d004 & 0x8000) != 0;
}

bool CUser::checkInBlueMarble()
{
    return (~(unsigned short)m_field8d00c & 0x8000) != 0;
}

bool CUser::CheckInWarRoom()
{
    return (~(unsigned short)m_field8d008 & 0x8000) != 0;
}

bool CUser::checkInDeathTower()
{
    return (~(unsigned short)m_field8d00e & 0x8000) != 0;
}

bool CUser::checkInBossTower()
{
    return (~(unsigned short)m_bossTowerIdx & 0x8000) != 0;
}

bool CUser::checkInAdvanceAltar() const
{
    return (~(unsigned short)m_advanceAltarIdx & 0x8000) != 0;
}

short CUser::getAdvanceAltarIndex() const
{
    return (short)(unsigned short)m_advanceAltarIdx;
}

short CUser::getBlueMarbleIndex()   // CUser.h:3011 short（CGM 落，定义同步）
{
    return (short)m_field8d00c;
}

void CUser::setBlueMarbleIndex(short index)
{
    m_field8d00c = index;
}
short CUser::getDeathTowerIndex()
{
    return (short)(unsigned short)m_field8d00e;
}

void CUser::setDeathTowerIndex(short idx)
{
    m_field8d00e = (unsigned short)idx;
}

WongWork::CDeathTower* CUser::getDeathTower()
{
    // ORIG 0x086552a4：idx<0 => NULL；否则 G_CGameManager()->getDeathTower(idx)
    // ORIG 直读 m_field8d00e（movzwl+test %ax+js 正路径顺序，无局部栈槽）。
    if ((short)(unsigned short)m_field8d00e >= 0)
        return G_CGameManager()->getDeathTower(
            (int)(short)(unsigned short)m_field8d00e);
    return 0;
}

// ---- Redeem_Item::CRedeemItem（GetCharacExpandData(6) 返回，未建模为
// CExpandEquipslot* 槽位；权威声明见 Redeem_Item__CRedeemItem.h，实现于
// Redeem_Item__CRedeemItem.cpp，ORIG 0x085f76f0 / 0x085f71a8）----

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086472c0 | _ZN5CUser14Add_RedeemInfoERK10Inven_Itemib
int CUser::Add_RedeemInfo(const Inven_Item& item, int count, bool isRedeem)
{
    // ORIG 0x086472c0：GetCharacExpandData(6)->isAddableFilter(item)，可加则记录历史并 AddRedeemList
    Redeem_Item::CRedeemItem* cdata =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    if (cdata == 0) return 0;
    if (!cdata->isAddableFilter(item)) return 0;

    CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
    if (pItem && pItem->is_stackable())
        m_historyLog.RedeemItemAdd(item.m_addInfo, item.get_add_info());
    else
        m_historyLog.RedeemItemAdd(item.m_addInfo, 1);

    return cdata->AddRedeemList(item, count, isRedeem);
}

short CUser::getBossTowerIndex()
{
    return (short)(unsigned short)m_bossTowerIdx;
}

void CUser::setBossTowerIndex(short index)
{
    m_bossTowerIdx = index;
}

short CUser::GetPartyIndex()
{
    return (short)(unsigned short)m_field8d004;
}

bool CUser::CheckInPvp()
{
    return (~(unsigned short)m_pvpRoomIdx & 0x8000) != 0;
}

short CUser::GetPvpIndex()
{
    return (short)(unsigned short)m_pvpRoomIdx;
}

short CUser::GetWarRoomIndex()
{
    return (short)(unsigned short)m_field8d008;
}

int CUser::GetTradeSpace()
{
    return (short)(unsigned short)m_tradeSpaceIdx;
}

void CUser::SetPartyIndex(short index)
{
    lock();
    m_field8d004 = index;
    unlock();
}

void CUser::SetWarRoomIndex(short index)
{
    m_field8d008 = index;
}

void CUser::SetSlotIDX(short idx)
{
    m_slotIDX = idx;
}

short CUser::GetIncreID() const
{
    // 字段 short（Main 回滚 :2248，混合号性 N 类）——ORIG 0x82a6842 `movzwl` 需
    // (unsigned short) 中间 cast 形（short 直读反产 movswl/符号形）。
    return (short)(unsigned short)m_field704a6;
}

void CUser::SetIncreID(short id)
{
    m_field704a6 = id;
}

void CUser::set_unique_id(unsigned short id)
{
    m_field704a8 = id;
}

void CUser::set_position(unsigned short x, unsigned short y, char z)
{
    m_posX = x;
    m_posY = y;
    m_direction = z;
}

unsigned short CUser::get_posX()
{
    return m_posX;
}

unsigned short CUser::get_posY()
{
    return m_posY;
}

char CUser::get_direction()
{
    return m_direction;
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x082f0e70 | _ZN5CUser12GetUserStateEv
int CUser::GetUserState()
{
    int state = m_chState;
    if (state > 3)
    {
        if (state == 5 || state == 10)
            return 1;
        if (state == 8)
            return 3;
        if (state == 6 || state == 7)
            return 2;
        if (state == 12)
            return 1;
        if (state == 11)
            return 7;
        if (state == 12)
            return 1;
        return 0;
    }
    return CUserCharacInfo::GetUserStateByAssault();
}

int CUser::GetAge()
{
    return m_clientSpec.m_age;
}

bool CUser::IsOverBirthDay()
{
    // ORIG 0x0867ec66：出生月/日是否已过（用于 PowerWar 最低年龄判定）。
    // ORIG 0x867ec66 形：cur@-0x18 → localtime_r **返回值**落槽 pLocal@-0x14
    // （0x867ec8d mov %eax,-0x14），month/day 各自 reload pLocal（0x867ec90/9c，
    // T-CU14 同款）；birth 字段无具名槽——两处成员直读+展宽直发 cmp。
    time_t cur = GlobalData::s_systemTime_.getCurSec();
    struct tm local;
    tm* pLocal = localtime_r(&cur, &local);
    int curMonth = pLocal->tm_mon + 1;
    int curDay = pLocal->tm_mday;
    if (m_clientSpec.m_birthMonth < curMonth)
        return true;
    if (m_clientSpec.m_birthMonth == curMonth &&
        m_clientSpec.m_birthDay < curDay)
        return true;
    return false;
}

bool CUser::IsProgLogout()
{
    return m_clientSpec.m_progLogout;
}

int CUser::GetSeedFromDate()
{
    return m_clientSpec.m_seedFromDate;
}

void CUser::SetGMUpgradeMode(ENUM_GM_ITEM_UPGRADE mode)
{
    // ORIG 0x822fea7-0x822fead：mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0x711dc(%eax)
    // （this-先 + 无帧 pop ebp）。b28d-solo-CUser N封顶: 4.4 恒 arg-先（this 后载）；
    //   删 self 已消帧 0x10+leave，残 1 条装载序 = N76/P211 边界（字段 uint 冻结于 CUser.h:900，
    //   形参 enum 不可同型化；cast/enum/uint 三形实测同）。
    m_clientSpec.m_gmUpgradeMode = mode;
}

bool CUser::isGMUser()
{
    return m_clientSpec.m_gmUserFlag;
}

bool CUser::IsGameMasterMode() const
{
    return m_clientSpec.m_gmUserFlag;
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086796dc | _ZN5CUser17SetGameMasterModeEb
void CUser::SetGameMasterMode(bool flag)
{
    m_clientSpec.m_gmUserFlag = flag;
    Packet_Register_GM_MID pkt;
    if (flag)
    {
        unsigned int characNo = getCurCharacNo();
        ENUM_SERVER_GROUP group = GetServerGroup();
        CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(group);
        proxy->SendTcpPacket((char*)&pkt, 0xe);
    }
}
bool CUser::GetEventCreateDnfReward()
{
    return m_eventCreateDnfReward;
}

int CUser::SetETC(short key, int value)
{
    // ORIG W 0x080f9968 _ZN5CUser6SetETCEsi：
    // key 截断 short → m_mapEtc.operator[](key) → *ret = value → return value。
    // （m_mapEtc 已在 CUser.h +0x8eaa4 建模，旧 TODO 注释已过时。）
    m_mapEtc[key] = value;
    return value;
}

// ============================================================================
// G1-4 批次：CodeHackCheckStorage / 返回用户 / 预登录数据
// ============================================================================

void CUser::setTournamentAccount(bool flag)
{
    m_postChecksumData.m_field31 = flag;
}

unsigned char CUser::isTournamentAccount()
{
    return m_postChecksumData.m_field31;
}

void CUser::setJoinedSchoolPointEvent(bool flag)
{
    m_postChecksumData.m_field44 = flag;
}

void CUser::setOnlinePreliminaryServerGroup(ENUM_SERVER_GROUP group)
{
    m_postChecksumData.m_onlineServerGroup = group;
}

void CUser::setOnlinePreliminaryCharacNo(unsigned int characNo)
{
    m_postChecksumData.m_onlineCharacNo = characNo;
}

void CUser::setOnlinePreliminaryPlayType(int playType)
{
    m_postChecksumData.m_onlinePlayType = playType;
}

int CUser::getOnlinePreliminaryPlayType()
{
    return m_postChecksumData.m_onlinePlayType;
}

unsigned int CUser::getOnlinePreliminaryCharacNo()
{
    return m_postChecksumData.m_onlineCharacNo;
}

void CUser::SetPCRoomNo(unsigned int no)
{
    m_postChecksumData.m_field54 = no;
}

void CUser::SetFirstLogin()
{
    m_postChecksumData.m_firstLogin = 1;
}

void CUser::SetReturnUser()
{
    m_postChecksumData.m_field59 = 1;
}

void CUser::ResetReturnUser()
{
    m_postChecksumData.m_field59 = 0;
}

bool CUser::IsReturnUser()
{
    return m_postChecksumData.m_field59;
}

void CUser::SetReturnUserFirstUserLogin(bool flag)
{
    m_postChecksumData.m_field5a = flag;
}

bool CUser::IsReturnUserFirstLogin()
{
    return m_postChecksumData.m_field5a;
}

int CUser::GetReturnUserExpireTime()
{
    return m_postChecksumData.m_field5c;
}

void CUser::SetReturnUserExpireTime(int time)
{
    m_postChecksumData.m_field5c = time;
}

void CUser::EnableSaveMemberBonusFatigue()
{
    m_postChecksumData.m_field60 = 1;
}

void CUser::incAccountMemberBonusFatigue(short value)
{
    m_postChecksumData.m_field62 = (short)(m_postChecksumData.m_field62 + value);
}

void CUser::resetAccountMemberBonusFatigue()
{
    m_postChecksumData.m_field62 = 0;
}

short CUser::getAccountMemberBonusFatigue()
{
    return (short)(unsigned short)m_postChecksumData.m_field62;
}

void CUser::resetAccountUsedFatigue()
{
    m_postChecksumData.m_totalFatigueUsed = 0;
}

void CUser::incChattingMessageCount(int count)
{
    m_postChecksumData.m_field64 += count;
}

void CUser::setChattingMessageCount(int count)
{
    m_postChecksumData.m_field64 = count;
}

int CUser::getChattingMessageCount()
{
    return m_postChecksumData.m_field64;
}

void CUser::SetGameMasterCharacter(CGameMasterCharacter* pCharac)
{
    m_postChecksumData.m_gameMasterCharac = pCharac;
}

int CUser::getStdDropRate()
{
    return m_postChecksumData.m_field28;
}

Secu_AccountHacking* CUser::getAcccountHackInfo()
{
    return &m_accHacking;
}

void CUser::setHackUserTimer()
{
    m_379.m_field0 = 1;
}

void CUser::setHackUserFlag()
{
    m_379.m_field1 = 1;
}

unsigned char CUser::isHackUser() const
{
    return m_379.m_field1;
}

bool CUser::isHackUserTimer() const
{
    return m_379.m_field0;
}

// ============================================================================
// G1-4 批次：突破奖励 / 账号货物 / 商城 / 日常
// ============================================================================

STBreakAwayRewards::STBreakAwayRewards()
{
    clear();
}

void STBreakAwayRewards::clear()
{
    m_rewardItemId = 0;
    m_rewardAItemCnt = 0;
    m_luckPoint = 0;
    m_fieldc = 0;
    m_itemOrder = 0;
}

void STBreakAwayRewards::operator=(const STBreakAwayRewards& other)
{
    m_rewardItemId = other.m_rewardItemId;
    m_rewardAItemCnt = other.m_rewardAItemCnt;
    m_luckPoint = other.m_luckPoint;
    m_fieldc = other.m_fieldc;
    m_itemOrder = other.m_itemOrder;
}

int CUser::getBreakAwayRewardItemId()
{
    return m_breakAway.m_rewardItemId;
}

void CUser::setBreakAwayRewardItemId(int id)
{
    m_breakAway.m_rewardItemId = id;
}

int CUser::getBreakAwayRewardAItemCnt()
{
    return m_breakAway.m_rewardAItemCnt;
}

void CUser::setBreakAwayRewardAItemCnt(int cnt)
{
    m_breakAway.m_rewardAItemCnt = cnt;
}

int CUser::getUserBreakAwayRewardLuckPoint()
{
    return m_breakAway.m_luckPoint;
}

void CUser::setUserBreakAwayRewardLuckPoint(int point)
{
    m_breakAway.m_luckPoint = point;
}

int CUser::getBreakAwayRewardItemOrder()
{
    return m_breakAway.m_itemOrder;
}

STBreakAwayRewards* CUser::getBreakAwayRewards()
{
    return &m_breakAway;
}

void CUser::setBreakAwayRewards(const STBreakAwayRewards& rewards)
{
    int frame_pad; (void)frame_pad;   // ORIG 0x831d52b sub $0x18：operator= 调用点 2 outgoing 槽帧舍入，dead 局部无指令
    m_breakAway = rewards;
}

int CUser::getBreakAwayQuestTime()
{
    return m_gameOption.m_breakAwayQuestTime;
}

void CUser::setBreakAwayQuestTime(unsigned int time)
{
    m_gameOption.m_breakAwayQuestTime = time;   // ORIG 0x822fc7c this-先 uint 直写（字段已 uint 化）
}

int CUser::getBreakAwayDungeonClearCnt()
{
    return m_gameOption.m_breakAwayDungeonClearCnt;
}

void CUser::setBreakAwayDungeonClearCnt(int cnt)
{
    m_gameOption.m_breakAwayDungeonClearCnt = cnt;
}

void CUser::subBreakAwayDungeonClearCnt()
{
    m_gameOption.m_breakAwayDungeonClearCnt -= 1;
    setBreakAwayDungeonClear(true);
}

void CUser::setBreakAwayDungeonClear(bool flag)
{
    m_gameOption.m_breakAwayDungeonClear = flag;
}

unsigned char CUser::isBreakAwayRewardData()
{
    return m_breakAwayRewardData;   // CUser+0x703c0（CGameOption 尾部 8B 收敛后）
}

void CUser::setGmQuestFlag(bool flag)
{
    m_gameOption.m_gmQuestFlag = flag;   // ORIG 0x822fca1 bool=bool 直落（字段已 bool 化，(char) cast 产 arg-先序）
}

void CUser::setWebAddress(char* addr)
{
    strncpy(m_gameOption.m_webAddress, addr, 0x10);
}

int CUser::GetCera()
{
    return (int)m_cera;
}

void CUser::SetCera(unsigned int cera)
{
    m_cera = cera;
}

unsigned int CUser::UseCera(unsigned int cera)
{
    if (m_cera >= cera)
        m_cera -= cera;
    else
        m_cera = 0;
    return m_cera;
}

void CUser::setRegDate(unsigned int date)
{
    m_regDate = date;
}

void CUser::setRegTime(long time)
{
    m_regTime = (int)time;
}

int CUser::getRegTime() const
{
    return m_regTime;
}

void CUser::resetUsedCera()
{
    m_guildAgitDB.m_usedCera = 0;
}

void CUser::addUsedCera(unsigned int cera)
{
    m_guildAgitDB.m_usedCera += cera;
}

unsigned int CUser::getUsedCera()
{
    return m_guildAgitDB.m_usedCera;
}

void CUser::SetVillageAttackedReward(bool flag)
{
    m_quest.m_field761c = flag;
}

void CUser::setPcRoomAuth(bool flag)
{
    m_pcRoomAuth = flag;   // CUser+0x8cef1（= m_quest+0x761d；UserQuest 收敛后）
}

void CUser::fillUserInfo4Antibot()
{
    m_field8cf58 = -1;
    strncpy(m_pad8cf18, getCurCharacName(), 0x14);
}

char* CUser::getUserInfo4Antibot()
{
    return m_pad8cf18;
}

void CUser::SetAccountLastPlayTime(long time)
{
    m_premium.m_lastPlayTime = (int)time;
}

void CUser::SetLoginTime(long time)
{
    m_premium.m_loginTime = (int)time;
}

long CUser::GetLoginTime()
{
    return m_premium.m_loginTime;
}

void CUser::SetLogInOutState(unsigned char state)
{
    m_premium.m_field98c = state;
}

unsigned char CUser::GetLogInOutState()
{
    return m_premium.m_field98c;
}

int CUser::get_public_ip_address()
{
    return m_publicIp;
}

void CUser::set_public_ip_address(int ip)
{
    m_publicIp = ip;
}

void CUser::set_local_ip_address(int ip)
{
    m_localIp = ip;
}

void CUser::setHangameUser(bool flag)
{
    m_hangameUser = flag;
}

bool CUser::isHangameUser()
{
    return m_hangameUser;
}

bool CUser::IsPowerUp()
{
    return m_powerUp;
}

void CUser::SetPowerUp(bool flag)
{
    m_powerUp = flag;   // ORIG 直写 mov %dl,0x8d253（P32 bool 字段直赋）
}

void CUser::SetRating(float rating)
{
    m_rating = rating;
}

void CUser::SetSchoolNo(unsigned int no)
{
    m_schoolNo = no;
}

unsigned int CUser::GetSchoolNo()
{
    return m_schoolNo;
}

void CUser::SetSchoolName(const char* name)
{
    m_str8e080 = name;
}

unsigned char CUser::GetTutorialSkipable()
{
    return m_tutorialSkipable;
}

void CUser::SetTutorialSkipable(unsigned char flag)
{
    m_tutorialSkipable = flag;
}

int CUser::getHumanCertifyTimerKey()
{
    return m_humanCertifyTimerKey;
}

void CUser::setHumanCertifyErrorCnt(int cnt)
{
    m_humanCertifyErrorCnt = cnt;
}

unsigned char CUser::isSendMailCertified()
{
    return m_field8e3ed;
}

void CUser::setRestingUserRestrict(bool flag)
{
    m_mcap.m_field544 = flag;
}

void CUser::setPunishTradeAlert(short alert)
{
    m_mcap.m_field546 = alert;
}

short CUser::getPunishTradeAlert()
{
    return (short)(unsigned short)m_mcap.m_field546;
}

void CUser::resetDungeonClearHackCheck()
{
    m_mcap.m_field54c = 0;
}

unsigned char CUser::IsRecvEvent()
{
    return m_mcap.m_recvEventWaiting;
}

int CUser::GetAutoClientRate()
{
    return m_mcap.m_field1c;
}

bool CUser::isARSUserKick()
{
    return m_mcap.m_field561;
}

void CUser::setSecuReward(bool flag)
{
    m_hades.m_secuReward = flag;
}

unsigned char CUser::NeedSecuReward()
{
    return m_hades.m_secuReward;
}

void CUser::setSecuType(unsigned char type)
{
    m_hades.m_secuType = type;
}

unsigned char CUser::getSecuType()
{
    return m_hades.m_secuType;
}

unsigned char CUser::getRewardType()
{
    return m_hades.m_rewardType;
}

void CUser::setRewardType(unsigned char type)
{
    m_hades.m_rewardType = type;
}

void CUser::setSecuGrade(char grade)
{
    m_hades.m_secuGrade = grade;
}

void CUser::setFlagPresentAvengerTitle(bool flag)
{
    m_ceraUserInfo.m_field62 = flag;
}

void CUser::incCharacSlotLimit(unsigned char count)
{
    m_ceraUserInfo.m_field61 += count;
}

// ORIG 0x08230860 (W)：movzbl +0x8d245（m_ceraUserInfo.m_field61）
unsigned char CUser::getCharacSlotLimit() const
{
    return m_ceraUserInfo.m_field61;
}

// ORIG 0x08230870 (W)：movzbl +0x8d244（m_ceraUserInfo.m_field60）
unsigned char CUser::getSlotEffectCount() const
{
    return static_cast<unsigned char>(m_ceraUserInfo.m_field60);
}

// ORIG 0x08230880 (W)：写 +0x8d244（m_ceraUserInfo.m_field60）
void CUser::setSlotEffectCount(unsigned char count)
{
    m_ceraUserInfo.m_field60 = count;   // ORIG 0x865c… 0x8230893 mov %dl 直写无归一 ⇒ 字段 uchar（Main 已回改），同型 this-先直落
}

// ORIG 0x08676826 (T)：组 Packet_Randombox_statistic（header 0x17b6/0xc，boxKind@+0xa、
// actionKind@+0xb）经 CStatisticServerProxy::SendPacket(pkt, 0xc) 发统计服务器（组 0）。
void CUser::SendRandomBoxStatistic(int boxKind, ENUM_RANDOMBOX_ACTION_KIND actionKind)
{
    // ORIG 0x8676826：无 proxy 判空/无具名槽（call 结果 %eax 直传 (%esp)，&pkt 先
    // 载 %ebx 跨 call，技巧 37），SendPacket 无条件调（非虚 null-this 安全）。
    Packet_Randombox_statistic pkt;
    pkt.m_boxKind = static_cast<char>(boxKind);
    pkt.m_actionKind = static_cast<char>(actionKind);
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&pkt), 0xc);
}

// ORIG 0x0868f6bc (T)：find_item(itemIdx)→verifyStackableItemProcess
int CUser::verifyStackableItemActionPlace(StackableAction::Type type, unsigned int itemIdx)
{
    CItem* item = G_CDataManager()->find_item(itemIdx);
    return verifyStackableItemProcess(type, item);
}

// ORIG 0x0868f60e (T)：is_stackable→verifyStackableAction→verifyUsablePlace→verifyUsableServerType
int CUser::verifyStackableItemProcess(StackableAction::Type type, const CItem* item)
{
    if (item == 0)
        return 0;
    if (!item->is_stackable())
        return 0;
    // ORIG 0x0868f60e：三处调用每次重读 0x10(%ebp) arg 槽（`mov 0x10(%ebp),%eax;
    // mov 0xc/0x8(%ebp),%edx; mov %edx,0x4(%esp); mov %eax,(%esp)`）——具名 cast
    // 中转变量会膨胀 -0x10/-0xc 槽（技巧 36/37），cast 内联。
    if (!((CStackableItem*)item)->verifyStackableAction(type))
        return 0;
    if (!((CStackableItem*)item)->verifyUsablePlace(this))
        return 0;
    if (!((CStackableItem*)item)->verifyUsableServerType())
        return 0;
    return 1;
}

void CUser::set_ontime_last_recv_idx(int idx)
{
    m_ceraUserInfo.m_ontimeLastRecvIdx = idx;
}

int CUser::get_ontime_last_recv_idx() const
{
    return m_ceraUserInfo.m_ontimeLastRecvIdx;
}

void CUser::set_update_ontime_last_recv_idx()
{
    m_ceraUserInfo.m_updateOntime = 1;
}

void CUser::setCheckPickUpRandomOptionItem(bool flag)
{
    m_field8eb44 = flag;
}

unsigned char CUser::getCheckPickUpRandomOptionItem()
{
    return m_field8eb44;
}

void CUser::set_cera_item_order_no(unsigned int no)
{
    m_levelUpGift.m_field18 = no;
}

unsigned int CUser::get_cera_item_order_no()
{
    return m_levelUpGift.m_field18;
}

void CUser::setPcRoomPlayStartTime(unsigned int time)
{
    m_pcRoomPlayStartTime = time;
}

unsigned char CUser::IsDeleteEventDungeonInvitation() const
{
    return m_deleteEventDungeonInvitation;
}

unsigned char CUser::IsUsedCoinInDungeon() const
{
    return m_field8eae0;
}

void CUser::ResetCoinCount()
{
    m_field8eb1c = 0;
}

int CUser::GetUsedCoinCount()
{
    return m_field8eb1c;
}

// ORIG W 0x082604ec _ZNK5CUser21getCurCharacQuestRefR Ev（const）
// 返回 m_quest（+0x858d4）引用指针；与 getCurCharacQuestR 同址、独立符号。
UserQuest* CUser::getCurCharacQuestRefR() const
{
    return const_cast<UserQuest*>(&m_quest);
}

// ORIG W 0x0830edaa _ZN5CUser36getAddIndependentDropRateFromPremiumE17ENUM_PREMIUM_TYPEj
// 转发 m_premium（+0x7121c）独立掉落率查询（参数直传，ORIG 无交换）。
unsigned int CUser::getAddIndependentDropRateFromPremium(ENUM_PREMIUM_TYPE type, unsigned int level)
{
    return m_premium.GetAddIndependentDropRate(type, level);
}

// ORIG T 0x08689576 _ZN5CUser23EventDungeonClearRewardERKSt6vectorISt4pairIiiESaIS2_EE
// 事件地下城清剿奖励发放：逐 (itemId,count)；find_item 失败直接结束；
// 背包无该物品 → make_item 构造后 insertItemIntoInventory（reason 13，log=1，
// stackable=false）；已有 → ItemAdd 日志 + 数量累加 update_item；
// 成功落位后 SendUpdateItemList(1, 0, slot)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08689576 | _ZN5CUser23EventDungeonClearRewardERKSt6vectorISt4pairIiiESaIS2_EE
void CUser::EventDungeonClearReward(std::vector<std::pair<int, int> > const& items)
{
    const int count = items.size();
    for (int i = 0; i < count; ++i)
    {
        int itemId = items[i].first;
        int itemCnt = items[i].second;
        CItem* data = G_CDataManager()->find_item(itemId);
        if (data == NULL)
            break;
        Inven_Item item;
        int slot = getCurCharacInvenR()->GetInvenData(itemId, item);
        if (slot < 0)
        {
            data->make_item(item);
            item.m_addInfo = itemId;
            item.m_addInfo2 = itemCnt;
            slot = getCurCharacInvenW()->insertItemIntoInventory(
                item, eItemAddReason_0xd, true, false);
        }
        else
        {
            int addInfo = item.get_add_info();
            m_historyLog.ItemAdd(INVEN_TYPE_EQUIP, itemCnt, addInfo, item,
                                 eItemAddReason_0xd);
            item.m_addInfo2 += itemCnt;
            getCurCharacInvenW()->update_item(INVEN_TYPE_EQUIP, slot, item);
        }
        if (slot >= 0)
        {
            SendUpdateItemList(eSendTarget_1, ENUM_ITEMSPACE_0, slot);
        }
    }
}

void CUser::set_trick_area(int area)
{
    m_field8cfc0 = area;
}

int CUser::get_before_area()
{
    return m_field8cfb8;
}

// ============================================================================
// G1-4 批次：锁 / 账号货物 / 其余访问器
// ============================================================================

CAccountCargo* CUser::GetAccountCargo()
{
    return &m_accountCargo;
}

bool CUser::IsExistAccountCargo()
{
    return m_accountCargo.GetCapacity() != 0;
}

void CUser::DeleteCargo()
{
    m_accountCargo.Clear();
}

void CUser::lock()
{
    m_mutex1.lock();
}

void CUser::unlock()
{
    m_mutex1.unlock();
}

int CUser::getBackupSeed()
{
    return m_backupSeed;
}

char CUser::getPowerSide()
{
    return m_guildDB.m_field95;
}

char* CUser::getClientHWSpec()
{
    return (char*)&m_clientSpec;
}

void CUser::SetGuildDBInfo(STGuildDBInfoOnly& info)
{
    memcpy(&m_guildDB, &info, 0xbd);
}

void CUser::ChangeGuildName(char* name)
{
    memset(&m_guildDB, 0, 0x17);
    memcpy(&m_guildDB, name, 0x16);
}

void CUser::SetGuildAgitFlag(bool flag)
{
    if (flag)
        m_guildDB.m_agitExist = 1;
    else
        m_guildDB.m_agitExist = 0;
}

void CUser::SetGuildAgitDBInfo(STGuildAgitDBInfo& info)
{
    memcpy(&m_guildAgitDB, &info, 1);
}

void CUser::SetGuildCargoCapacity(unsigned int capacity)
{
    m_field8e094 = capacity;
}

void CUser::SetTodayGuildMember(bool flag)
{
    m_guildAgitDB.m_todayGuildMember = flag;
}

unsigned char CUser::IsTodayGuildMember()
{
    return m_guildAgitDB.m_todayGuildMember;
}

void CUser::SetLastLoginCharacNo(int characNo)
{
    m_field8d0f4 = characNo;
}

void CUser::SetLastLoginChannelNo(int channelNo)
{
    m_field8d0f8 = channelNo;
}

void CUser::SetValidLastLoginCharac(bool flag)
{
    m_field8d0fc = flag;
}

void CUser::SetReportMannerlessUserCount(int count)
{
    m_field8d110 = count;
}

int CUser::GetChecksumAnswerCount()
{
    return m_field8cfd8;
}

void CUser::IncChecksumAnswerCount()
{
    m_field8cfd8 += 1;
}

int CUser::GetCheckConnInterval()
{
    return m_field8cfd0;
}

void CUser::SetResumeChecksumTime(int time)
{
    m_field8cfd4 = time;
}

void CUser::incre_check_count()
{
    m_field8cfc8++;
}

short CUser::get_connect_count()
{
    return (short)(unsigned short)m_field8cfc8;
}

void CUser::setTotalHackUserTradeGold(unsigned long long gold)
{
    // ORIG 0x84ec940：mov this,%ecx; mov -0x8,%eax; mov -0x4,%edx; mov %eax,0x8d1ac(%ecx);
    // mov %edx,0x8d1b0(%ecx)。ll 成员直写= 4.4 同形（probe w2 实证）。
    m_379.m_hackTradeGold = gold;
}

void CUser::setTotalHackUserTradeCnt(unsigned int cnt)
{
    m_379.m_hackTradeCnt = cnt;
}

void CUser::SetRevivalTime(long time)
{
    m_field8d1dc = (int)time;
}

long CUser::GetRevivalTime()
{
    return m_field8d1dc;
}

void CUser::SetUseAPCPotionTime(long time)
{
    m_field8d1e0 = (int)time;
}

long CUser::GetUseAPCPotionTime()
{
    return m_field8d1e0;
}

void CUser::SetDailyBadge(short* badge)
{
    for (int i = 0; i <= 2; ++i)
        m_dailyBadge[i] = badge[i];
}

void CUser::SetUserDetailInfo(char* a, char* b, char* c)
{
    strncpy(&m_accName[0x16], a, 0x80);
    strncpy(&m_accName[0x96], b, 0x80);
    strncpy(&m_accName[0x116], c, 0x10);
}

// ============================================================================
// G1-4 批次：角色列表 / 视图
// ============================================================================

const char* CUser::get_charac_name(int idx) const
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return 0;
    return m_characList[idx].m_name;
}

unsigned char CUser::GetCharacJob(int idx) const   // CUser.h:3290 uchar（DCoreC6 caller cmp %al 证据，定义同步）
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return 0xb;
    // ORIG 0x822ff5c：单条 movzbl 0x26(%eax),%eax。4.4 对 char 字段两种写法均多一级
    // 扩展（值 cast=双 movzbl；地址重解释=add+双 movzbl），需字段 uchar 化（Main 评估中，
    // Dispatchers 0x6306 movsbl 依赖 char 语义）——暂取值 cast（ndiff 更小）。
    return (unsigned char)m_characList[idx].m_field26;
}

unsigned short CUser::getCharacLevelByCharacNo(int characNo) const  // ORIG caller 0x84d205b mov %ax 截断+cmpw 实证 16 位返回（Main 落头 2026-09-05）
{
    // ORIG 0x0868e5xx：begin→-0x10、end→-0x14 prologue 双缓存（技巧 25）；
    // return 无显式 cast（ORIG 尾单条 movzwl 直返，(unsigned short) 强转反产两段）。
    int idx = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator itend = m_characList.end();
    for (; it != itend; ++idx, ++it)   // ORIG 0x84ecc4f ++idx 先于 ppEv
    {
        if (it->m_characNo == characNo)
            // 函数返回类型 ushort（Main 落头）后裸 return：4.4 隐式 HImode 截断
            // 单条 movzwl 0x27(%eax)（ORIG 0x84ecc49 同形）；显式 cast 反产
            // movzwl %ax 二段展宽（probe 三试）——本行为定稿。
            return it->m_level;
    }
    return -1;
}

const char* CUser::get_charac_name_by_charac_no(int characNo) const
{
    // ORIG 0x0863bece：循环下标 int，size 有符号比较（setg）。
    for (int i = 0; i < static_cast<int>(m_characList.size()); ++i)
    {
        if (m_characList[i].m_characNo == characNo)
            return m_characList[i].m_name;
    }
    return 0;
}

bool CUser::setCharacName(int characNo, const char* name)   // ORIG W 0x084ecc7c：命中 true/未中 false
{
    // ORIG W 0x84ecc7c：prologue begin→-0x10、end→-0x14 双缓存；latch ++idx 先于
    // ++it（技巧 25 具名缓存消 end 重构）。
    int idx = 0;
    std::vector<_Charac_info>::iterator it = m_characList.begin();
    std::vector<_Charac_info>::iterator itend = m_characList.end();
    for (; it != itend; ++idx, ++it)
    {
        if (it->m_characNo == characNo)
        {
            strncpy(it->m_name, name, 0x1d);
            m_characterView.enableSaveCharacView();
            return true;
        }
    }
    return false;
}

void CUser::SetCharacViewState(int state)
{
    m_characterView.enableSaveCharacView();
    CUserCharacInfo::setCharacViewState(state);
}

void CUser::SetCharacViewState(int characNo, int state)
{
    if (characNo < 0 || characNo >= (int)m_characList.size())
        return;
    m_characterView.enableSaveCharacView();
    m_characList[characNo].m_viewState = state;
}

int CUser::GetCharacViewState(int idx) const
{
    if (idx < 0 || idx >= (int)m_characList.size())
        return -1;
    return m_characList[idx].m_viewState;
}

bool CUser::SetCharacViewStateByCharacNo(int characNo, int state)
{
    // ORIG 0x0868??：prologue begin→-0xc、end→-0x10 双缓存（技巧 25/T-CU3）。
    std::vector<_Charac_info>::iterator it = m_characList.begin();
    std::vector<_Charac_info>::iterator itend = m_characList.end();
    for (; it != itend; ++it)
    {
        if (it->m_characNo == characNo)
        {
            m_characterView.enableSaveCharacView();
            it->m_viewState = state;
            return true;
        }
    }
    return false;
}

// ============================================================================
// 批次 A 访问器（ORIG 反汇编逐偏移补全：0x8101048 / 0x82301ae / 0x827565a /
// 0x85a932a / 0x868a99a / 0x868fa46 / 0x86912b0 / 0x8691cd0 / 0x8692af6 等）
// ============================================================================

void CUser::sendEventInfo()
{
}

void CUser::questAutoClear(int questIdx)
{
}

void CUser::_onCompoundItem(int itemIdx)
{
}

void CUser::_updateNPCRelationship()
{
}

void CUser::processNPCGiftOnLevelUp()
{
}

char* CUser::GetUserName()
{
    return m_accName + 0x16;
}

char* CUser::GetUserEMail()
{
    return m_accName + 0x116;
}

const char* CUser::GetSsnString()
{
    return m_accName + 0x96;
}

int CUser::getSex()   // 返回 int（CUser.h 判据同源）
{
    const char* ssn = GetSsnString();
    if (strlen(ssn) < 0xd)
        return -1;
    if (ssn[6] == '2' || ssn[6] == '4')
        return 0;
    return 1;
}

void* CUser::GetCSHashSet()
{
    return &m_set8eb00;
}

char* CUser::GetGuildName()
{
    return (char*)&m_guildDB;
}

unsigned char CUser::GetGuildLevel()
{
    return m_guildDB.m_fundLevel;   // +0x8d03b（成员视图折叠单指令 movzbl）
}

void* CUser::GetHackCheck()
{
    return &m_hackLogCheck;
}

void* CUser::getBingoData()
{
    return &m_bingo;
}

void* CUser::GetGuildDBInfo()
{
    return &m_guildDB;
}

char* CUser::getGarenaAuthData()
{
    return reinterpret_cast<char*>(&m_garenaAuthData);  // +0x6ef92 Taiwan::GarenaAuthData 成员视图（签名保持 char*）
}

void* CUser::GetPISenderManager()
{
    return &m_piSender;
}

void* CUser::GetPIReceiverManager()
{
    return &m_piReceiver;
}

void* CUser::getGrowthWeaponEventdata()
{
    return &m_growthWeapon;
}

void* CUser::get_ingame_event_history()
{
    return &m_map8eb68;
}

void* CUser::get_ingame_event_history_update()
{
    return &m_map8eb80;
}

void* CUser::getGrowthCreatureEventdata()
{
    return &m_growthCreature;
}

void* CUser::get_server_fatigue_day_data()
{
    return &m_map8eb50;
}

void* CUser::getCharacLevelUpGift_AccountOnce()
{
    return &m_levelUpGift;
}

void* CUser::GetMailBox()
{
    return *(void**)((char*)&m_premium.m_mailBox);
}

int CUser::GetMileage()
{
    return m_mileage;
}

int CUser::GetPCRoomNo()
{
    return m_postChecksumData.m_field54;
}

unsigned int CUser::GetCeraPoint()
{
    return m_ceraPoint;
}

int CUser::getAntibotKey()
{
    return m_antibotKey;
}

int CUser::GetManageLevel()
{
    return m_field8ebd0;
}

int CUser::GetManagePoint()
{
    return m_field8ebd4;
}

int CUser::GetUserMaxLevel()
{
    return m_field8ebd8;
}

void CUser::onDungeonClear(bool flag)
{
}

int CUser::GetDebugCommand()
{
    return m_debugCommand;
}

int CUser::GetTradePunishType()
{
    return m_postChecksumData.m_tradePunishType;
}

int CUser::getLastLotteryTime()
{
    return m_mcap.m_lastLotteryTime;
}

int CUser::GetNonClientRandInt()
{
    return m_piSender.m_nonClientRandInt;
}

bool CUser::GetNonClientFlag()   // CUser.h:2630 bool（TimerC6 证据，定义同步）
{
    return m_piSender.m_nonClientFlag;
}

int CUser::GetLastLoginCharacNo()
{
    return m_field8d0f4;
}

int CUser::GetLastLoginChannelNo()
{
    return m_field8d0f8;
}

bool CUser::IsValidLastLoginCharac()
{
    return m_field8d0fc;
}

int CUser::getRecipeProbability()
{
    return m_field8ec2c;
}

int CUser::get_local_ip_address()
{
    return *(int*)((char*)&m_localIp);
}

unsigned int CUser::GetGuildCargoCapacity()
{
    return m_field8e094;
}

int CUser::GetGuildPowerWarPoint()
{
    return m_guildDB.m_powerWarPoint;
}

long CUser::GetResumeChecksumTime()
{
    return m_field8cfd4;
}

CGameMasterCharacter* CUser::GetGameMasterCharacter()
{
    return *(CGameMasterCharacter**)((char*)&m_postChecksumData.m_gameMasterCharac);
}

int CUser::getBreakAwayAccureCera()
{
    return m_gameOption.m_breakAwayAccureCera;
}

int CUser::getBreakAwayRewardOrder()
{
    return m_gameOption.m_breakAwayRewardOrder;   // +0x654
}

int CUser::get_multiboxLotteryItemFailCnt()
{
    return m_multiboxLotteryFailCnt;   // CUser+0x703bc
}

int CUser::GetCurCharacUsedFatigueQuantity()
{
    return m_field8eba8;
}

bool CUser::isHumanCertified()
{
    return m_humanCertified;
}

unsigned char CUser::isLoadingHackGold()
{
    return (unsigned char)m_mcap.m_loadingHackGold;
}

bool CUser::isSaveInformNoticeFlag()
{
    return m_saveInformNoticeFlag;   // ORIG movzbl 0x8ebc1 直返（m_informNoticeFlags 起 +0x10）
}

int CUser::CheckMaxLuckyLevel()
{
    return 1;
}

// ---- 批次 A 第 2 组（ORIG 0x82300e8 / 0x82301d8 / 0x8230246 / 0x8692a08 /
// 0x8692b74 / 0x86971ce / 0x86975b6 / 0x8568e48 等） ----

void CUser::OnRecvEvent()
{
    m_mcap.m_recvEventWaiting = true;   // +0x18
}

unsigned char CUser::GetBuyingGold()
{
    return m_field8ead8;   // ORIG movzbl 直返（无 test/setne）
}

void CUser::SetBuyingGold(bool flag)
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl,0x8ead8 完全一致）。
    m_field8ead8 = flag;
}

unsigned char CUser::GetFirstLogin()
{
    return m_postChecksumData.m_firstLogin;   // ORIG movzbl 直返（成员 bool）
}

void CUser::SetProgLogout()
{
    m_clientSpec.m_progLogout = 1;   // 字段已 bool 化（ORIG IsProgLogout@0x82a6796 movzbl 直返）；&-cast 反而多余
}

bool CUser::isRestingUser()
{
    return m_mcap.m_field544;   // ORIG movzbl 直返（无 test/setne）
}

void CUser::InitReceivedRequestType()
{
    m_receivedRequestType = 0xff;
}

bool CUser::isBreakAwayDungeonClear()
{
    return m_gameOption.m_breakAwayDungeonClear;   // ORIG movzbl 直返（无 test/setne）
}

bool CUser::isUsedBreakAwayLuckPoint()
{
    return m_usedBreakAwayLuckPoint;   // ORIG movzbl 0x703c1 直返
}

unsigned char CUser::isJoinedSchoolPointEvent()
{
    return (unsigned char)m_postChecksumData.m_field44;
}

void CUser::DisableSaveMemberBonusFatigue()
{
    m_postChecksumData.m_field60 = false;
}

unsigned char CUser::IsEnableSaveMemberBonusFatigue()
{
    return (unsigned char)m_postChecksumData.m_field60;
}

void CUser::SetProperLevelDungeonUser()
{
    m_field8eb99 = true;
}

void CUser::ClearProperLevelDungeonUser()
{
    m_field8eb99 = false;
}

bool CUser::IsProperLevelDungeonUser()
{
    return m_field8eb99;
}

bool CUser::isCharacLinkMessageFlag()
{
    return m_breakAway.m_field14;
}

// b28d-solo-CUser：CUser.h:2718 经 Main 代落 unsigned char→bool（P292：ORIG 调用点
//   0x08652c9a `call; test %al,%al; je` 无 setcc）。ORIG 定义体 0x086973f8 直返
//   `movzbl 0x703d9; pop; ret`（P9 直返字节）⇒ 体写 (bool) 转换试直返形。
bool CUser::isLinkCharacDisconnectFlag()
{
    return (bool)m_breakAway.m_field15;
}

unsigned char CUser::getDisconnectLinkCharacSlotIdx()
{
    return (unsigned char)m_breakAway.m_field16;
}

unsigned char CUser::is_update_ontime_last_recv_idx()
{
    return (unsigned char)m_ceraUserInfo.m_updateOntime;   // §4.3 具名(UserInfo+0x68)
}

void CUser::reset_update_ontime_last_recv_idx()
{
    m_ceraUserInfo.m_updateOntime = false;                 // §4.3 具名(UserInfo+0x68)
}

unsigned char CUser::CheckLoadRentalInfoFromExchange()
{
    return m_rentalFlag2;   // ORIG movzbl 0x6ef91 直返
}

unsigned char CUser::GetPuUser()
{
    return m_field796d8;   // +0x796d8（成员直返折叠单指令 movzbl）
}

void CUser::SetCeraPoint(unsigned int point)
{
    m_ceraPoint = point;
}

void CUser::SetDebugCommand(int cmd)
{
    m_debugCommand = cmd;
}

void CUser::decre_check_count()
{
    *(short*)((char*)&m_field8cfc8) = 0;
}

void CUser::setLastLotteryTime(unsigned long t)
{
    // ORIG 0x82307cb-0x82307d1：this-先 uint 直写、无帧。b28d-solo-CUser AE：删具名 self。
    m_mcap.m_lastLotteryTime = t;
}

void CUser::SetNonClientRandInt(int v)
{
    m_piSender.m_nonClientRandInt = v;
}

void CUser::recipeForceProbability(int v)
{
    *(int*)((char*)&m_field8ec2c) = v;
}

void CUser::setBreakAwayAccureCera(int v)
{
    m_gameOption.m_breakAwayAccureCera = v;
}

void CUser::setCharacAntibotSerialNum(int v)
{
    *(int*)((char*)&m_field8ec24) = v;
}

void CUser::set_multiboxLotteryItemFailCnt(int v)
{
    m_multiboxLotteryFailCnt = v;   // CUser+0x703bc
}

void CUser::resetPlayExpAdd()
{
    m_gameOption.m_playExpAdd = 0;  // 字段 unsigned 化后直写复原 ORIG `movl $0x0,0x703a4(%eax)` 单条
}

void CUser::resetCleanpadFailCnt()
{
    *(int*)((char*)&m_humanCertifyErrorCnt) = 0;
}

bool CUser::isCleanPadVerifyLimit()
{
    return *(int*)((char*)&m_humanCertifyErrorCnt) > 2;
}

void CUser::resetTotalPcRoomPlayTime()
{
    m_totalPcRoomPlayTime = 0;
}

void CUser::ResetCurCharacUsedFatigueQuantity()
{
    m_field8eba8 = 0;   // ORIG 0x86975d4 movl $0x0,0x8eba8(%eax) 单条位移直写（&-cast 反产 add+mov 双条）
}

// ORIG _ZN5CUser24set_server_fatigue_countEs T 0x0868d32a：按当日
// get_today_time_day 在 m_map8eb50 累加，无表项则插入。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868d32a | _ZN5CUser24set_server_fatigue_countEs
void CUser::set_server_fatigue_count(short v)
{
    unsigned char todayDay =
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->get_today_time_day();
    std::map<unsigned char, short>::iterator it = m_map8eb50.find(todayDay);
    if (it != m_map8eb50.end())
    {
        it->second = static_cast<short>(it->second + v);
    }
    else
    {
        m_map8eb50.insert(std::make_pair(todayDay, v));
    }
}

// ORIG _ZN5CUser31IncCurCharacUsedFatigueQuantityEj W 0x08110e1e：m_field8eba8 += n。
void CUser::IncCurCharacUsedFatigueQuantity(unsigned int n)
{
    m_field8eba8 += n;
}

// ORIG _ZN5CUser35IncCurCharacUsedGiftFatigueQuantityEj W 0x08110e3a：m_field8ebac += n。
void CUser::IncCurCharacUsedGiftFatigueQuantity(unsigned int n)
{
    m_field8ebac += n;
    // ORIG 0x08110e57：累加后 SetChangedGiftFatigueQuantity(true)
    SetChangedGiftFatigueQuantity(true);
}

// ORIG _ZN5CUser27ProcProperLevelDungeonClearEv T 0x0868f5fa（0x13 字节）：单行转发。
void CUser::ProcProperLevelDungeonClear()
{
    GiveMeBoxReward();
}

// ORIG _ZN5CUser15GiveMeBoxRewardEv T 0x08693758：单行转发 DB 请求。
void CUser::GiveMeBoxReward()
{
    DBInsertGiveBoxEventReward::makeRequest(this);
}

// ORIG _ZN5CUser18SetTradePunishTypeEi T 0x08689890：printf 日志后
// m_postChecksumData.m_tradePunishType |= type。
void CUser::SetTradePunishType(int type)
{
    printf("CUser::SetTradePunishType(%d) \n", type);
    m_postChecksumData.m_tradePunishType =
        m_postChecksumData.m_tradePunishType | type;
}

// ORIG _ZN5CUser10SetMileageEi T 0x086509c6：LogHistory 记录新旧值后写 m_mileage。
void CUser::SetMileage(int mileage)
{
    LogHistory("Mileage %s,%d,%d,%d", "Set", mileage, m_mileage, mileage);
    m_mileage = mileage;
}

// ORIG _ZN5CUser12AddMoneyPlusEi T 0x0868f266：当日滚窗 m_money[0]+=value，m_money[2]=now。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0868f266 | _ZN5CUser12AddMoneyPlusEi
void CUser::AddMoneyPlus(int value)
{
    // 残差 N81 附注族（4.4 恒形）：尾句 `+=` 的 load 4.4 恒 eax 中转+mov 转发+
    // add mem 直加（ORIG 4.1 mem→ecx 直载+value→eax+add eax,ecx）。穷举 +=/自加
    // 表达式/操作数交换/引用局部/寄存器复刻（nl11_probe4/5/7）均不可翻。
    if (exceptedStateMoneyLog())
    {
        return;
    }
    const int now = GlobalData::s_systemTime_.getCurSec();
    NextCharacMoneyLog(now);
    m_moneyLogs[m_moneyLogIdx].m_money[2] = now;
    m_moneyLogs[m_moneyLogIdx].m_money[0] += value;
}

// ORIG _ZN5CUser13AddMoneyMinusEi T 0x0868f310：当日滚窗 m_money[1]+=value，m_money[2]=now。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0868f310 | _ZN5CUser13AddMoneyMinusEi
void CUser::AddMoneyMinus(int value)
{
    // 残差 N81 附注族，同 AddMoneyPlus（load 中转寄存器 4.4 恒 eax）。
    if (exceptedStateMoneyLog())
    {
        return;
    }
    const int now = GlobalData::s_systemTime_.getCurSec();
    NextCharacMoneyLog(now);
    m_moneyLogs[m_moneyLogIdx].m_money[2] = now;
    m_moneyLogs[m_moneyLogIdx].m_money[1] += value;
}

// ORIG _ZN5CUser18NextCharacMoneyLogEi T 0x0868f1c0：当前槽含时间且与
// 传入时刻不同日 → 槽推进（钳 6）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868f1c0 | _ZN5CUser18NextCharacMoneyLogEi
void CUser::NextCharacMoneyLog(int now)
{
    if (m_moneyLogs[m_moneyLogIdx].m_money[2] != 0)
    {
        struct tm currentTm;
        struct tm oldTm;
        const time_t current = now;
        const time_t old = m_moneyLogs[m_moneyLogIdx].m_money[2];
        localtime_r(&current, &currentTm);
        localtime_r(&old, &oldTm);
        if (currentTm.tm_yday != oldTm.tm_yday)
        {
            m_moneyLogIdx = m_moneyLogIdx + 1;
            if (m_moneyLogIdx > 6)
            {
                m_moneyLogIdx = 6;
            }
        }
    }
}

// ORIG _ZN5CUser21exceptedStateMoneyLogEv T 0x0868f3ba：组队村庄怪标记/搬运副本/
// 状态 8/村庄 7 任一命中 → 跳过货币滚窗。
bool CUser::exceptedStateMoneyLog()
{
    // ORIG T 0x868f3ba 全录（报告缺失，objdump 71 insn 转写）：GetParty() 每使用点
    // 重调共 4 次（0x868f3c6/3da/3fd/417，无具名槽，T-CU15）；条件全走 4.1 bool
    // 物化 `test;setne %al;test %al,%al;je`（技巧 22 register bool al）；m_89e
    // uchar `movzbl 0x89e;test %al` 免物化直判；get_state `cmp $0x8,%eax; sete`、
    // getCurCharacVill `cmp $0x7,%al` 8 位形（callee uchar 返回）。bf 读保留
    // CParty+0xb24 视图（§4.3 具名，0xc4c 单级系 4.1 复合折叠，4.4 两段为 N）。
    // b28d-solo-CUser AE: ORIG 0x868f3ba 全段 `setne %al; test %al,%al; je` / `movzbl
    // 0x89e; test %al,%al` 均无槽（sub $0x18）；具名 gp/bfNZ/m89e/st8/vil7 各落 -0x9..-0xd
    // 使帧 +0x10 ⇒ 全部内联条件（probe_bool a3/a5）。GetParty() 每使用点重调（T-CU15）。
    if (GetParty() != 0)
    {
        if ((*(&((CParty*)GetParty())->m_battleField)).m_villageMonsterId != 0)
            return true;
        if (((CParty*)GetParty())->m_battleField.m_pDungeon != 0)
        {
            // b28d-solo-CUser N封顶: 唯一残差 = 本行 uchar 直接测试。ORIG 0x868f3ba
            //   `movzbl 0x89e; test %al,%al; je`（无 setne），4.4 在本 CFG（多出口+
            //   共享尾）恒插 `setne %al; test %al,%al`（探针 probe_uc2/uc3/uc4：
            //   f2/f3 与 f1/f14/f18 同上下文均复现；return 1/!=0/扁平 goto 三形无效）。
            //   字段 m_89e 为 CDungeon.h:793 unsigned char，bool 化会截断脚本赋值
            //   CDungeon.cpp:421（script.m_28d 可 >1）且跨 TU 消费（CParty.cpp 等）⇒ 不改。
            if (((CParty*)GetParty())->m_battleField.m_pDungeon->m_89e)
                return true;
        }
    }
    if (get_state() == 8)
        return true;
    if (getCurCharacVill() == 7)
        return true;
    return false;
}


int CUser::UseWinPoint(int winPoint, eWPSubReason reason)
{
    useWinPoint(winPoint, reason);
    if (winPoint != 0)
    {
        m_historyLog.WPSub(getPVPResultR()->m_winPoint, winPoint, reason);
    }
    return getPVPResultR()->m_winPoint;
}

// ORIG _ZN5CUser17AddSecuRewardItemEh T 0x08688328（0x14 字节）：仅参数存
// 未使用局部（ORIG 即空操作）。
void CUser::AddSecuRewardItem(unsigned char rewardType)
{
    // ORIG 0x868832b sub $0xa4 + 0x8688334 mov %al,-0xa4(%ebp)（4.1 把 arg→local 复制折叠为单条）。
    // 4.4 双槽（probe17）；big[0x9c] 垫 0xa4 帧。local 初始化多余——删，仅留 arg  spill。
    char big[0x9c]; (void)big;
    (void)rewardType;
}

// ORIG _ZN5CUser24GenerateCleanPadVeriDataERtRhS0_ T 0x08649ac4：生成三段
// CleanPad 验证种子（ushort randInt(0xffff)+1 / uchar randInt(6) /
// ushort randInt(0xffff)+1）存 m_mcap 并回填出参。
void CUser::GenerateCleanPadVeriData(unsigned short& outA, unsigned char& outB,
                                     unsigned short& outC)
{
    m_mcap.m_cleanPadVeriA =
        static_cast<unsigned short>(get_rand_int(0xffff) + 1);
    m_mcap.m_cleanPadVeriB = static_cast<unsigned char>(get_rand_int(6));
    m_mcap.m_cleanPadVeriC =
        static_cast<unsigned short>(get_rand_int(0xffff) + 1);
    outA = m_mcap.m_cleanPadVeriA;
    outB = m_mcap.m_cleanPadVeriB;
    outC = m_mcap.m_cleanPadVeriC;
}

// ORIG _ZN5CUser15reqHumanCertifyEv T 0x0867f26e（0x252 字节）：未认证时发起
// CleanPad 人工认证——记日志/暴露计数、从验证队列随机取一条填 0x7d 包
// （byte 5 + int + binary + 认证种子三段 + 错误计数 + 0），发送后记 tick。
bool CUser::reqHumanCertify()
{
    if (!isHumanCertified())
    {
        return false;
    }
    m_historyLog.RequestCleanPad(m_mcap.getPoint(), 0);
    m_mcap.incExposedCount();
    if (GlobalData::s_capchaDataManager.isEmpty())
    {
        return false;
    }
    MSG_UDP_CAPCHA_CERTIFY* msg = GlobalData::s_capchaDataManager.peekData();
    memcpy(m_capchaCertify, msg->m_text, 0xa);

    PacketGuard pg;
    pg.put_header(0, 0x7d);
    pg.put_byte(5);
    pg.put_int(msg->m_size);
    // ORIG 0x867f366-0x867f376：len→%eax、ptr→%edx（技巧 28 register asm("eax") 钉住
    // len 寄存器；ptr 计算被挤到 %edx）。
    int len = msg->m_size;
    pg.put_binary((const char*)msg->m_image, len);
    pg.put_byte(m_mcap.getPoint() > 0x251b ? 1 : 0);

    unsigned short veriA = 0;
    unsigned char veriB = 0;
    unsigned short veriC = 0;
    GenerateCleanPadVeriData(veriA, veriB, veriC);
    pg.put_short(veriA);
    pg.put_byte(veriB);
    pg.put_short(veriC);
    pg.put_byte((unsigned char)m_humanCertifyErrorCnt);
    pg.put_byte(0);
    pg.finalize(true);
    Send(pg);
    m_mcap.m_capchaReqTick = GlobalData::s_systemTime_.getCurTickCount();
    return true;
}

// ORIG _ZN5CUser22sendNotiPcRoomPlayTimeEv T 0x0868f83c：包 0x1a8（网吧时长通知）。
void CUser::sendNotiPcRoomPlayTime()
{
    PacketGuard pg;
    pg.put_header(0, 0x1a8);
    pg.put_int(getTotalPcRoomPlayTime());
    pg.finalize(true);
    Send(pg);
}

// ORIG _ZN5CUser12SendCashDataEb T 0x0865082a：包 0x35（商城货币数据；
// flag=1 时全 0，否则 Cera/Mileage/CeraPoint）。
void CUser::SendCashData(bool flag)
{
    PacketGuard pg;
    pg.put_header(0, 0x35);
    pg.put_byte(1);
    if (flag)
    {
        pg.put_int(0);
        pg.put_int(0);
        pg.put_int(0);
    }
    else
    {
        pg.put_int(GetCera());
        pg.put_int(GetMileage());
        pg.put_int(GetCeraPoint());
    }
    pg.finalize(true);
    Send(pg);
}

// ORIG _ZN5CUser15recover_staminaEv T 0x0865784e：满体力外每分钟恢复
// （+9，钳 100）并回发包 0x4。
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0865784e | _ZN5CUser15recover_staminaEv
int CUser::recover_stamina()
{
    // ORIG 0x8657859-0x8657874：两次独立装载 `cmp $0x2,%eax; jle` / `cmp $0x5,%eax; je`，
    // 均落 0x865797d `mov $0,%eax` dead 岛后 fall epi（P0/N96：dead eax 置零与
    // 尾 dead 读为 4.1 寄存器遗留，4.4 无源码等价发射，去绑定后记 N 封顶）。
    if (m_chState <= 2 || m_chState == 5)
        goto RET0;
    {
        if ((bool)(getCurCharacR()->m_stamina <= 0x63))
        {
            addCurcharacStamina(9);
            // ORIG 0x86578b9-0x86578c0：cmp $0x64,%al; seta 物化。
            if ((bool)(getCurCharacR()->m_stamina > 0x64))
            {
                setCurCharacStamina(0x64);
            }
            PacketGuard pg;
            pg.put_header(0, 0x4);
            pg.put_byte(getCurCharacR()->m_stamina);
            pg.finalize(true);
            Send(pg);
        }
        // ORIG 0x8657969-0x8657978：pg dtor merge 后 dead 读（技巧 16 原地双零扩展）；
        // 两分支共落。
        // ORIG 0x8657969 尾：call getCurCharacR + `movzbl 0x75(%eax),%eax;
        // movzbl %al,%eax` = 成功路返回 stamina 字段（第二 movzbl 为 4.1 int
        // 返回截断产物，4.4 单段零扩展，残差 1 条记跨版本 N）。
        return getCurCharacR()->m_stamina;
    }
    goto RET0;   // 不可达守卫镜像（保持 ORIG 块序：成功 fall 在前、mov$0 岛在后）
RET0:;
    return 0;
}

// ORIG _ZN5CUser13WorkPerTenMinEv T 0x08653196：无角色时 trace；有激活角色
// 则向监视代理上报角色在线（characNo）。
void CUser::WorkPerTenMin()
{
    if (m_chState <= 2)
    {
        return;
    }
    if (getCurCharacR() == 0)
    {
        // ORIG 0x86531f4-：NumberToString 两 call 先执行、结果暂 %ebx，cMyTrace
        // 临时构造成其后（0x8653215 mov %eax,%ebx 证据）→ 临时链式（技巧 3）。
        cMyTrace("CUser::WorkPerTenMin()", 0x1d71, 5)(
            "WorkPerTenMin() m_id(%s) character - m_selected is null..",
            NumberToString(get_acc_id(), 0));
        return;
    }
    if (get_charac_memberkey() != 0)
    {
        GlobalData::s_monitor_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendNoticeCharLive(getCurCharacNo());
    }
}

// ORIG _ZN5CUser14WorkPerFiveMinEv T 0x08652f0c：五/十分钟公会上报族
// （一般经验与今日经验两段同构）；无角色时 trace 返回。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08652f0c | _ZN5CUser14WorkPerFiveMinEv
void CUser::WorkPerFiveMin()
{
    if (m_chState <= 2)
    {
        return;
    }
    if (getCurCharacR() == 0)
    {
        cMyTrace trace("CUser::WorkPerFiveMin()", 0x1d24, 5);
        trace("WorkPerFiveMin() m_id(%s) character - m_selected is null..",
              NumberToString(get_acc_id(), 0));
        return;
    }
    if (get_guild_exp() != 0)
    {
        Packet_Monitor_Increase_Guild_Exp packet;
        packet.m_charNo = getCurCharacNo();
        packet.m_guildKey = get_charac_guildkey();
        packet.m_addExp = get_guild_exp();
        packet.m_notify = m_field796d9;
        packet.m_expType = 0;
        m_historyLog.GuildAddExp(get_charac_guildkey(),
                                 static_cast<const char*>(GetGuildDBInfo()),
                                 getCurCharacNo(), get_guild_exp());
        GlobalData::s_guild_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&packet), 0x1c);
        m_field796d9 = 0;
        reset_guild_exp();
    }
    if (get_guild_today_exp() != 0)
    {
        Packet_Monitor_Increase_Guild_Exp packet;
        packet.m_charNo = getCurCharacNo();
        packet.m_guildKey = get_charac_guildkey();
        packet.m_addExp = get_guild_today_exp();
        packet.m_notify = m_field796d9;
        packet.m_expType = 2;
        m_historyLog.GuildAddExp(get_charac_guildkey(),
                                 static_cast<const char*>(GetGuildDBInfo()),
                                 getCurCharacNo(), get_guild_today_exp());
        GlobalData::s_guild_proxy_mgr
            ->GetServerProxy(GetServerGroup())
            ->SendPacket(reinterpret_cast<char*>(&packet), 0x1c);
        m_field796d9 = 0;
        reset_guild_today_exp();
    }
}

int CUser::get_server_fatigue_day_size()
{
    return (int)m_map8eb50.size();
}

void CUser::IncreaseUsedCoinCount()
{
    *(int*)((char*)&m_field8eb1c) = *(int*)((char*)&m_field8eb1c) + 1;
}

void CUser::setLoadHackGold(bool flag)
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl 完全一致）。
    m_mcap.m_loadingHackGold = flag;
}

void CUser::SetNonClientFlag(bool flag)
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl 完全一致）。
    m_piSender.m_nonClientFlag = flag;
}

void CUser::setHumanCertified(bool flag)
{
    // ORIG：bool 转换指针形式（mov this; movzbl flag; mov %dl 完全一致）。
    *(bool*)((char*)&m_humanCertified) = flag;
}

void CUser::incTradeCount()
{
    m_tradeCount = m_tradeCount + 1;
}

float CUser::GetRating()
{
    return m_rating;
}

void CUser::backupSeed()
{
    m_backupSeed = m_checkSumSeed;  // ORIG 0x859506f: *(this+0x796e4) = *(this+0x796e0)
}

void CUser::ResetCharacExpandData()
{
    m_characExpand.reset();
}

bool CUser::IsExistGuildAgit()
{
    return m_guildDB.m_agitExist != 0;   // +0x8d0be（成员视图折叠单指令 movzbl）
}

short CUser::GetDailyBadge(ENUM_BADGE_TYPE type)
{
    return m_dailyBadge[type];
}

// ORIG @0x08568e0c（W）：out[i]=m_dailyBadge[i]，i=0..2
// （ORIG 寻址 (i+0x47040)*2+4 即 m_dailyBadge 逐字读）。
void CUser::GetDailyBadge(short* out)
{
    for (int i = 0; i <= 2; ++i)
        out[i] = m_dailyBadge[i];
}

// ORIG @0x08681c30。On Time Event 奖励发放：
// 双日志 ./log/%s/OnTimeEvent → itemNo==0 短路返 0 → 包头(1,0x120)+byte1+
// get_index(记录未用)+byte1 → find_item(itemNo)（无表项返 0）→
// create_item(NONE,item,count)→有效性(m_addInfo||m_addInfo2)→插背包
// （reason 0xd；失败系统邮件 msg_132/133 + item.reset，成功 SendUpdateItem）
// → put_int/put_short → finalize 发送，返 1。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08681c30 | _ZN5CUser23InsertOnTimeEventRewardEjj
bool CUser::InsertOnTimeEventReward(unsigned int itemNo, unsigned int itemCount)  // b28c-L5 SIGFIX（P174）
{
    char logDir[256];
    memset(logDir, 0, sizeof(logDir));
    sprintf(logDir, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());

    unsigned int characNo = get_charac_no(-1);
    char* accStr = NumberToString(get_acc_id(), 0);
    {
        CMyFileLog log(
            "bool CUser::InsertOnTimeEventReward(unsigned int, unsigned int)",
            0x869f);
        log(logDir, "On Time Event:InsertOnTimeEventReward(%s, %u)",
            accStr, characNo);
    }

    if (itemNo == 0)
        return 0;

    // ORIG 0x8681d98：物品索引无效（find_item 空）→ 直接返回 0（不发包）
    if (G_CDataManager()->find_item((int)itemNo) == 0)
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x120);
    packet.put_byte(1);
    int pktIndex = packet.get_index();   // ORIG 记录后未再使用
    (void)pktIndex;
    packet.put_byte(1);

    {
        Inven_Item item;                 // 默认构造（ORIG ctor 0x080cb854）
        item.m_addInfo = (int)itemNo;    // +0x02 预填 item index（ORIG -0x88 写）
        G_CDataManager()->getItemList()->create_item(
            (ENUM_ITEM_CREATE_TYPE)0, item, (int)itemCount);

        bool give =
            (item.m_addInfo != 0) || (item.get_add_info() != 0);
        if (give)
        {
            int slot = getCurCharacInvenW()->insertItemIntoInventory(
                item, (eItemAddReason)0xd, true, true);
            if (slot < 0)
            {
                char title[20];          // strncpy(src, 0x14)，不补零
                const char* sTitle = g_scriptStringManager_.findString(
                    4, "game_server_msg_132", (bool*)0);
                strncpy(title, sTitle ? sTitle : "", 0x14);

                char msg[256];
                const char* sMsg = g_scriptStringManager_.findString(
                    4, "game_server_msg_133", (bool*)0);
                memset(msg, 0, sizeof(msg));
                strncpy(msg, sMsg ? sMsg : "", 0xff);

                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    title, item, 0, (unsigned int)getCurCharacNo(),
                    msg, (int)strlen(msg), 0, GetServerGroup(), false, false);

                item.reset();
            }
            else
            {
                SendUpdateItem(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
            }

            packet.put_int((int)itemNo);
            packet.put_short((short)itemCount);
        }
    }

    packet.finalize(true);
    Send(packet);
    return 1;
}

int CUser::get_charac_count()
{
    return (int)m_characList.size();
}

bool CUser::isTradePunishOverThirtyDays()
{
    return m_postChecksumData.m_tradePunishOverThirtyDays;   // +0x7962c（ORIG movzbl 直返；原误读 m_firstLogin）
}


void CUser::sendBlueMarbleEnterCount()
{
    PacketGuard packet;
    packet.put_header(0, 0x1b4);
    packet.put_byte(getBlueMarbleEnterCount());
    packet.finalize(true);
    Send(packet);
}

void CUser::ResetDailyCharacExpandDataMidnight()
{
    m_characExpand.ResetDailyMidnight();
}

void CUser::ResetDailyCharacExpandData()
{
    m_characExpand.ResetDaily();
}

void CUser::resetNPCRelationShipDailyData() {}

void CUser::sendNPCRelationShipFavor() {}

void CUser::SendProperDungeonClearCount()
{
    // ORIG 0x8681833-0x868183b：`cmp $2; setle; test; jne skip` = state<=2 早退
    // （body 在 state>2 执行，此前极性写反已修正）；0x868188a `mov %ax,-0xa;
    // movswl -0xa,%eax` = short 具名槽（pg 基址随之平移 -0x18）。
    if (get_state() <= 2)
        return;
    if (!getCurCharacR())
        return;
    PacketGuard packet;
    packet.put_header(0, 0x10b);
    short cnt = GetProperDungeonClearCount();
    packet.put_short(cnt);
    packet.finalize(true);
    Send(packet);
}

bool CUser::acceptable_within_mission() const
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    // ORIG 0x8??：grade 走 %ebx（`mov %eax,%ebx` 技巧 2），`mov %ebx,0x4(%esp)` 直传；
    // mission=-0x10 / idx=-0xc。b28d-solo-CUser：裸 register 复现 ebx 保活（P231）。
    register int grade = get_pvp_grade();
    int idx = G_CDataManager()->get_WithinMissionIndex(grade);
    if (idx == 0)
        return false;
    return mission->MakeMissionList_JustKind(*this, 0x1b);
}

bool CUser::isDuplicationMessage(const std::string& msg)
{
    // ORIG 0x08688826：比较实参 = `lea 0x79664/0x79668(%eax),%edx` + `mov %eax,
    // 0x4(%esp)` ⇒ **m_str[i] == msg**（str 在左占 this 槽，原 msg==str 反序）；
    // ORIG -0x9 槽 = dup 结果槽；索引无槽——`movzbl 0x7966c; movsbl %al,%eax`
    // 原地（(char) 有符号 cast 内联，具名 idx 局部多占槽致 dup 漂到 -0xa）。
    bool dup = (m_str79664[0] == msg) || (m_str79664[1] == msg);
    m_str79664[(char)m_dupMsgSlotIdx] = msg;   // 写址 this+0x79664+idx*4（Ghidra (idx+0x1e598)*4 换算）
    m_dupMsgSlotIdx = m_dupMsgSlotIdx ^ 1;   // ORIG 0x8688?? `mov 0x8; movzbl 0x7966c` 重读成员
    return dup;
}

void CUser::DimensionInoutUpdate(bool flag1, bool flag2)
{
    // ORIG i 槽 -0xc（probe：4.4 首声明 4B 局部占 -0x10；前置 dead int 占远槽
    // -0x10 后 i 平移至 -0xc，技巧 31）。
    int deadpad;
    (void)deadpad;
    for (int i = 0; i <= 5; i++)
    {
        if (flag1)
        {
            // ORIG：value 无槽——call 结果原地 `movsbl %al,%ecx`（右实参先评估）；
            // i 次之 `mov -0xc,%eax; movsbl %al,%edx`。
            setDemensionInoutValue((char)i,
                                   (char)G_CDataManager()->get_dimensionInout(i));
        }
    }
    if (flag2)
    {
        setUltimateInoutValue(2,
                              (char)G_CDataManager()->get_limit_inout_count(
                                  (ENUM_BLOOD_TYPE)2));
    }
}

void CUser::send_pvp_record()
{
    PacketGuard packet;
    packet.put_header(0, 0x30);
    // ORIG：getPVPResultRefR 每处 put 独立重调 7 次——访问器结果禁缓存
    // （战术 P183/P48）。
    packet.put_int(getPVPResultRefR()->m_totalWinCount);
    packet.put_int(getPVPResultRefR()->m_totalLoseCount);
    packet.put_int(getPVPResultRefR()->m_expPoint);
    packet.put_int(GlobalData::s_ref_pvp_grade
                       ->GetPvpCurrRankPoint(getPVPResultRefR()->m_pvpGrade));
    packet.put_int(GlobalData::s_ref_pvp_grade
                       ->GetPvpNextRankPoint(getPVPResultRefR()->m_pvpGrade));
    packet.put_byte(getPVPResultRefR()->m_pvpGrade);
    // ORIG 0x: `mov 0x5c(%eax),%eax` 32 位直读 m_todayWinCount(int@+0x5c) 后
    // 交 put_byte(int)——用字节指针 `*(unsigned char*)&` 会被 4.4 折成
    // add+movzbl 载低字节（多 3 条），改传 int 直读对齐 ORIG。
    packet.put_byte(getPVPResultRefR()->m_todayWinCount);
    packet.finalize(true);
    Send(packet);
}

void CUser::send_clear_quest_list()
{
    PacketGuard packet;
    packet.put_header(0, 0x163);
    packet.put_int(0x7530);
    // ORIG 0x08??：quest/cleared 均无槽——`call getCurCharacQuestR; add $0x4,%eax;
    // mov %eax,(%esp); call getClearedQuest` 全链式（技巧 37/36：具名指针中转 =
    // 槽膨胀主因，pg 基址 -0x1c 应回 -0x14）。
    packet.put_binary(
        (const char*)reinterpret_cast<WongWork::CQuestClear*>(
            (char*)getCurCharacQuestR() + 4)->getClearedQuest(),
        0x7530);
    packet.finalize(true);
    Send(packet);
}

void CUser::processReturnUserQuestAutoClear()
{
    if (IsReturnUser() && IsReturnUserFirstLogin())
    {
        questAutoClear(get_charac_level() - 1);
    }
}

bool CUser::update_pvp_rank(const PvpResultType& result)
{
    getPVPResultRefW()->AddNewResult(result);
    return true;
}

void CUser::saveTaxMoneyForUpperMember(int money)
{
    // b28d-solo-CUser AE: ORIG 0x864f8fb-0x864f902 `sete %al; test %al,%al; jne`
    //   （P292 非 bool 返回的 ==0 物化）；具名 mk0 落槽（mov %al,-0x9; cmpb $0,-0x9）
    //   ⇒ 删局部内联条件（probe_bool a3）。2/3 守卫直折 test %al/je + cmpl/jle。
    if (get_charac_memberkey() == 0)
        return;
    // ORIG 0x864f90f-0x864f917 正向嵌套（技巧 19）：`test %al,%al; je` /
    // `cmpl $0x0,0xc; jle` 均直落 leave，无 xor/中转 jmp。
    if (is_connect_upper_member())
    {
        if (money > 0)
            add_member_pay_tex_money_to_upper(money);
    }
}

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fa08 | _ZN5CUser20gainExpAsUpperMemberEi
int CUser::gainExpAsUpperMember(int exp)
{
    if (get_charac_memberkey() == 0)
        return 0;
    // ORIG 0x0864fa08：connect 块 fall-through（技巧59），未连接尾块 set(0)+ret0；
    // 率常量 = 0.01f（@0x08cf3958 = 0x3c23d70a，此前 0.1f 为语义错）；
    // result<=0 亦调 set_charac_member_bonus_exp(0)（ORIG 3 call 点）。
    if (is_connect_upper_member())
    {
        // b28d-lane5：ORIG 0x0864fa2x `movzbl %al,%eax; mov %ax,-0x1a; filds -0x1a`
        // ⇒ 原码局部为 **short**（非 uchar，P0 槽往返 + 宽度证据）；率计算
        // `fstps -0x10` 具名 float 局部（P252）后再 `fildl exp; fmuls -0x10`。
        short level = get_uppermember_exp_level();
        float rate = level * 0.01f;
        int result = (int)(exp * rate);
        if (result > 0)
            set_charac_member_bonus_exp(result);
        else
            set_charac_member_bonus_exp(0);
        return result;
    }
    set_charac_member_bonus_exp(0);
    return 0;
}

int CUser::gainPowerWarRewardExp(int)
{
    return 0;
}

void CUser::add_guild_point_item()
{
    int threshold = G_CDataManager()->m_guildParameter.m_field4;   // ORIG 0x865ccd9 槽 -0xc
    // ORIG 0x865cce8 cmp -0xc(%ebp),%eax; setl;test;je skip：call 结果寄存器直参与比较
    // （不物化 rand 局部，probe21 f 实证复现；rand 入槽反产 spill+reload 三行）
    if (get_rand_int(0x2710) < threshold)
        add_inventory_item(0xc84);
    return;   // ORIG 0x865cd05 body 尾 jmp merge + 0x865cd07 nop 岛（技巧 7 尾置 return）
}

// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0867c844 | _ZN5CUser19SendMoneyFullReasonE22ENUM_MONEY_FULL_REASONmm
bool CUser::SendMoneyFullReason(ENUM_MONEY_FULL_REASON reason, unsigned long a, unsigned long b)  // b28d-lane5：ORIG 0x0867c8da 返回 Send 结果
{
    PacketGuard packet;
    packet.put_header(0, 0x33);
    packet.put_byte(reason);
    packet.put_int(a);
    packet.put_int(b);
    packet.finalize(true);
    return Send(packet);
}

bool CUser::AddDungeonClear(int dungeonIdx, int clearCount)
{
    // ORIG 0x086??：getClearedDungeonDiff 结果 movsbl %al,%eax 原地单段（无 char
    // 槽物化）+ 比较 setl %al;test;je 尾物化岛（技巧 22 register bool al，直折
    // jge 反形）；残差 movswl %ax,%ebx（4.1 arg 截断 sext 直落 ebx）N18a。
    // 二修：getter 结果直用免槽（技巧 37，ORIG call 后 %eax 即 this 实参）、
    // 正向 `if(less)`（`!less` 反产 xor$1 取反）。
    // b28d-solo-CUser AE: ORIG `cmp; setl %al; test %al,%al; je` 无槽；具名 less 落 -0x9
    // 使帧 +0x10 ⇒ 内联条件（probe_bool a6/a7）。
    if (getDungeonClearRefR()->getClearedDungeonDiff(dungeonIdx) < clearCount)
    {
        getDungeonClearRefW()->addClearedDungeon(dungeonIdx, clearCount);
        return true;
    }
    return false;
}
void CUser::SendConditionEventInfo()
{
    PacketGuard packet;
    packet.put_header(0, 0xea);
    CConditionEventManager* pMgr = G_CGameManager()->GetConditionEventManager();
    if (pMgr)
    {
        int eventIdx = pMgr->GetCurEventIndex();
        // ORIG 0x868195d/0x868196b：`mov %al,-0xa/-0x9`（char 槽直存）+ movsbl 读
        // ——局部为 char（signed byte），put_byte 传参符号扩展。
        char step = (char)GetCurConditionEventStep();
        char rewardStep = (char)GetCurConditionEventRewardStep();
        packet.put_short(eventIdx);
        packet.put_byte(step);
        packet.put_byte(rewardStep);
        packet.finalize(true);
        Send(packet);
    }
}

void CUser::ResetCurCharacUsedGiftFatigueQuantity()
{
    if (m_field8ebac != 0)
    {
        m_field8ebac = 0;
        SetChangedGiftFatigueQuantity(true);
    }
}

void CUser::SetChangedGiftFatigueQuantity(bool flag)
{
    m_field8ebb0 = flag;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08657570 | _ZN5CUser15ResetDailyQuestEv
void CUser::ResetDailyQuest()
{
    std::list<int> questList;
    // ORIG 0x865753x：G_CDataManager()->m_questList 结果不留槽（技巧 37），
    // 直接 `mov %eax,(%esp)` 传 GetDailyQuestList——具名 pQuestList 反产 -0x10 spill
    // 使整帧平移 4（ORIG it=-0x18/end=-0x1c 证据）。
    G_CDataManager()->m_questList->GetDailyQuestList(questList);
    for (std::list<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        // ORIG 0x08657574：getCurCharacQuestRefR 链式调用 isClearQuest（无中间变量、不判空）
        if (getCurCharacQuestRefR()->isClearQuest(questId))
        {
            getCurCharacQuestW()->resetClearQuest(questId);
        }
    }
}

// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x08657662 | _ZN5CUser18ResetTrainingQuestEv
void CUser::ResetTrainingQuest()
{
    std::vector<int> questList;
    short level = get_charac_level();
    QuestList* pQuestList = G_CDataManager()->m_questList;
    pQuestList->GetTrainingQuestList(questList, level);
    for (std::vector<int>::iterator it = questList.begin(); it != questList.end(); ++it)
    {
        int questId = *it;
        UserQuest* pQuest = getCurCharacQuestR();
        if (pQuest && pQuest->isClearQuest(questId))
        {
            getCurCharacQuestW()->resetClearQuest(questId);
        }
    }
}

// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x085df9b2 | _ZN5CUser11SetPvpIndexEs
void CUser::SetPvpIndex(short idx)
{
    // ORIG 0x08669ee8：CSwitchLog("SetPvpIndex",0x2339,0,0) 记
    // "pvp@log %s,%s,room(%d)"（acc_name/charac_name/idx）后加锁写 m_pvpRoomIdx。
    CSwitchLog log("SetPvpIndex", 0x2339, 0, 0);
    log("pvp@log %s,%s,room(%d)", get_acc_name(), getCurCharacName(), (int)idx);
    lock();
    m_pvpRoomIdx = idx;
    unlock();
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0865cadc | _ZN5CUser18add_inventory_itemEj
void CUser::add_inventory_item(unsigned int itemId)
{
    	CItem* pItem = G_CDataManager()->find_item(itemId);
	if (!pItem)
		return;
	Inven_Item invenItem;
	pItem->make_item(invenItem);
	invenItem.m_addInfo2 = 1;  // count
	invenItem.m_addInfo = itemId;  // item id
	// ORIG 0x0865cad2/0x0865cae6：slot 先以 -1 初始化（movl $0xffffffff,-0xc），
	// 调用回写后 `== -1` bool 物化比较（cmpl/sete/test/jne）。
	int slot = -1;
	slot = getCurCharacInvenW()->insertItemIntoInventory(
	    invenItem, (eItemAddReason)0x18, true, true);
	// b28d-solo-CUser：ORIG 0x0865cae6 `cmpl; sete %al; test %al,%al; jne`（bool 物化，
	//   P295 while 形复现；`(bool)(x==-1)` 直折 je 无 sete）。
	while (slot == -1)
		return;
	// ORIG 0x0865cb19：MakeItemPacket 成功则 finalize+Send（0xe/byte 0/short 1）
	PacketGuard packet;
	((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
	((InterfacePacketBuf*)&packet)->put_byte(0);
	((InterfacePacketBuf*)&packet)->put_short(1);
	// ORIG 0x0865cb??：`call; xor $1,%eax; test %al,%al; jne 尾` = if(!f()) goto
	// 尾出口反形（4.4 正向 if 生 test/je 无 xor）。
	if (!getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, slot, packet))
		goto done;
	((InterfacePacketBuf*)&packet)->finalize(true);
	Send(packet);
done:;
}

void CUser::send_MissionList()
{
    CMissionList_Charac* mission =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    mission->Send_MissionList(*this);
}

void CUser::send_aura_avatar_option()
{
    if (getCurCharacR() == 0) return;
    PacketGuard packet;
    packet.put_header(0, 0x17a);
    packet.put_byte(3);
    for (int idx = 0; idx < 3; ++idx) {
        packet.put_byte((char)idx);
        int now = GlobalData::s_systemTime_.getCurSec();
        // [b23S3C] P48/P0：ORIG 双臂各重取 getCurCharacR（无 cur/expire 槽），
        // if/else 分支岛（P56/P198），put_int 两分支各一次
        if (now < getCurCharacR()->m_auraValues[idx])
            packet.put_int(getCurCharacR()->m_auraValues[idx] - now);
        else
            packet.put_int(0);
    }
    packet.finalize(true);
    Send(packet);
}

// ===================== GameStubs 迁移（ORIG 强符号 T；最小正确语义） =====================
// 全部签名经 nm -C /home/wangyilei/下载/df_game_r 核实。

// 0x086489f4（DisConnSig.md 汇编+反编译 C 全文核对）
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x086489f4 | _ZN5CUser10DisConnSigE11DISCONN_SIGbi
void CUser::DisConnSig(DISCONN_SIG sig, bool flag, int param)
{
    Mutex& mtx = m_mutex2;   // ORIG this+0x8cfa0 实证
    mtx.lock();

    // ORIG 0x08648a45..0x08648a63：持锁后立即经 Arad_DataManager::findGameScript
    // ("Arad_EventPeriodDataManager") 取周期事件管理并 delUserRewardData(this)，
    // 不受后文 socket/state 短路影响（无条件先执行）。
    {
        std::string name("Arad_EventPeriodDataManager");
        ARAD::Arad_DataManager* adm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        ARAD::Arad_EventPeriodDataManager* epdm =
            (ARAD::Arad_EventPeriodDataManager*)adm->findGameScript(name);
        epdm->delUserRewardData(this);
    }

    int sock = m_network.m_sock;   // ORIG this+0xe0
    int chState = m_chState;       // ORIG this+0x8cfc4
    if (sock < 0 || chState == 0)
    {
        // ORIG +0x137 trace（0x911）：无 socket / 未登录态仅留痕后返回。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x911, 5);
        trace("this %x,incre_id= %d, idx= %d, m_socket<0,m_ChState==NONE , "
              "DisConnSig %s, From %d",
              this, (int)GetIncreID(), (int)GetSlotIDX(),
              NumberToString(get_acc_id(), 0), (int)sig);
        mtx.unlock();
        return;
    }

    if (IsProgLogout())
    {
        // ORIG +0x21c trace（0x917）：已在登出中。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x917, 5);
        trace("[Prog log out...]this %x,incre_id= %d, idx= %d, DisConnSig %s, From %d",
              this, (int)GetIncreID(), (int)GetSlotIDX(),
              NumberToString(get_acc_id(), 0), (int)sig);
        mtx.unlock();
        return;
    }
    SetProgLogout();

    // ORIG +0x240：GetPeerIP2 后按有无 IP 打两条 trace（0x939 无 IP / 0x937 有 IP）；
    // send/recv 计数 = +0x8cf0d/+0x8cf0c 双字节、+0x8cf08（m_lastPacketID）。
    char ip[16];
    bool haveIp = m_network.GetPeerIP2(ip, 0x10) != 0;
    {
        const unsigned char* sendCnt = (const unsigned char*)m_pad8cf0c;  // +0x8cf0c
        if (!haveIp)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x939, 0);
            trace("from (%d) m_id[%s] uid[%d], send(%d:%d), recv(%d), "
                  "ip(GetPeerIP:NONE), dungeon(%d)",
                  (int)sig, NumberToString(get_acc_id(), 0), (int)m_field704a8,
                  (int)sendCnt[1], (int)sendCnt[0], m_lastPacketID,
                  getPlayingDungeonIndex());
        }
        else
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x937, 0);
            trace("from (%d) m_id[%s] uid[%d], send(%d:%d), recv(%d), ip(%s), dungeon(%d)",
                  (int)sig, NumberToString(get_acc_id(), 0), (int)m_field704a8,
                  (int)sendCnt[1], (int)sendCnt[0], m_lastPacketID, ip,
                  getPlayingDungeonIndex());
        }
    }

    // ORIG +0x2c0：sig==5 时按 accId/ip 有无重映射断线原因（0x30/0x31/0x32）。
    int reason = (int)sig;
    if (reason == 5)
    {
        if (get_acc_id() == 0 && ip[0] != '\0')
        {
            reason = 0x30;
            flag = true;
        }
        else if (get_acc_id() == 0 && ip[0] == '\0')
        {
            reason = 0x32;
            flag = true;
        }
        else
        {
            reason = 0x31;
            flag = true;
        }
    }
    SendClientCrashDownLogToLogDB(reason);   // ORIG T 0x0864870e

    // ORIG +0x4a5：SetCurTime 后读小时（CommonTime+0x03，布局见 CommonTime.cpp
    // 注释：+0x00 year/+0x01 month/+0x02 day/+0x03 hour）∈(0x12,0x17) 且登录时长
    // (0,0x2a31) → 组 Ting 时段统计包发统计服务器（组 0，0x12 字节）。
    CommonTime nowTime;
    nowTime.SetCurTime();
    char hour = *((char*)&nowTime + 3);   // +0x03 m_hour（CommonTime.cpp 权威布局）
    if (0x12 < hour && hour < 0x17)
    {
        Packet_User_Ting_TimeCheck_Statistic_Add ting;
        int playSec = (int)(GlobalData::s_systemTime_.getCurSec() - GetLoginTime());
        (void)get_acc_id();          // ORIG +0x504 另读 accId（无后续使用）
        if (0 < playSec && playSec < 0x2a31)
        {
            CStatisticServerProxy* proxy =
                GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
            if (proxy != 0)
                proxy->SendPacket((char*)&ting, 0x12);
        }
    }

    if (sock == -1)
    {
        // ORIG +0x567：socket 已断 → 仅留痕。
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x971,
            "[CUser::DisConnSig] Already Disconnected! Socket(-1) ACCID[%s]",
            NumberToString(get_acc_id(), 0));
    }
    else
    {
        // ORIG +0x5b5：reason∉{6,0x30,0x31,0x32} → 先发包 199(int reason)。
        if (reason != 6 && reason != 0x30 && reason != 0x31 && reason != 0x32)
        {
            PacketGuard pkt;
            ((InterfacePacketBuf*)&pkt)->put_header(0, 199);
            ((InterfacePacketBuf*)&pkt)->put_int(reason);
            ((InterfacePacketBuf*)&pkt)->finalize(true);
            if (Send(pkt) != 1)
            {
                LogManager::logFormat(
                    1, "user.cpp", __PRETTY_FUNCTION__, 0x988,
                    "[CUser::DisConnSig] Fail to disconnect reason send: reason[%d]",
                    reason);
            }
        }
        // ORIG +0x640：ResetEpoll 失败 → errno 留痕（0x98e）。
        int err = G_EpollHandler()->ResetEpoll(sock);
        if (err != 0)
        {
            LogManager::logFormat(
                1, "user.cpp", __PRETTY_FUNCTION__, 0x98e,
                "(m_id: %s) G_EpollHandler()->ResetEpoll(pUser->get_socket(%d)) %d(%s)",
                NumberToString(get_acc_id(), 0), sock, err, strerror(err));
        }
        // ORIG +0x730：组流(<<1<<uid)（0x991）。
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x991);
        CStreamGuard guard(s, true);
        guard << (int)1;
        guard << (int)GetUID();
        // ORIG +0x7c9：+0x8d0df != 1（= m_guildAgitDB.m_pad2[0]，语义未建模【推断】）
        // 时补崩溃统计 + HWSpec 上报。
        if (m_guildAgitDB.m_pad2[0] != (char)1)
        {
            if (!flag)
            {
                // ORIG +0x7f0：crash 观测计数（key=lastPacketID / dungeonIdx / party）。
                StatisticsToObserveCrash* crash =
                    G_GameWorld()->getStatisticsToObserveCrash();
                crash->incUserLastCrashPacketID(m_lastPacketID);
                crash->incUserLastCrashDungeonID(getPlayingDungeonIndex());
                crash->incAbnormalDownUserCount();
                crash->incUserLastCrashPlayStatus(CheckInParty() ? 1 : 0);
            }
            // ORIG +0x883：Client_Spec 统计包（header 0x9ce/0xc06）：+0xa type=1、
            // +0xb count=hwSpec[1]、+0xc errCode=param、+0x10 errLine=最后登录频道、
            // +0x12 起 memcpy hwSpec[+4]（0xc/条 × count）；SendPacket 长
            // count*0xc+0x12（布局对照 shared/packet/include 权威头）。
            char* hwSpec = getClientHWSpec();
            unsigned char hwCount = (unsigned char)hwSpec[1];
            Packet_Client_Spec_Statistic specPkt;
            char* specBase = (char*)&specPkt;
            specPkt.m_type = 1;                                       // +0x0a
            specPkt.m_count = hwCount;                                // +0x0b
            specPkt.m_errorCode = param;                              // +0x0c（movl）
            specPkt.m_errorLine = (short)GetLastLoginChannelNo();     // +0x10
            memcpy((char*)specPkt.m_specs, hwSpec + 4, hwCount * 0xc);
            int specLen = (int)(unsigned short)(hwCount * 0xc + 0x12);
            CStatisticServerProxy* proxy = GlobalData::s_statistic_proxy_mgr
                ->GetServerProxy((ENUM_SERVER_GROUP)GetServerGroup());
            if (proxy != 0)
                proxy->SendPacket(specBase, specLen);
        }
        // ORIG +0x95a：队列 1 投递（在 if(+0x8d0df) 块之后，无条件执行）。
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }
    mtx.unlock();
}

// 0x08665400：PvP 技能树发放主循环（givePvPSkillTree.md）：
// secondGrow∈[0..CurSecondGrow]、grade∈[a..pvp_grade]；脚本 miss 跳过；
// mode 1/2 只发树 0/树 1，mode 3 双树；b=true 且已有等级≥目标跳过。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08665400 | _ZN5CUser16givePvPSkillTreeEibi
void CUser::givePvPSkillTree(int a, bool b, int c)
{
    // ORIG T 0x8665400 全录（报告双段读完）：mode 判断为 **param_3==1/2/3 三分支
    // 复制**（SkillR/W、FindSkill、growtype_skill 各 4 次调用，勿 hoist 指针/合并
    // 条件——旧 `c==1||c==3` 共享形 call 计数 17 vs ORIG 46）；内层 **iterator
    // 扫描**（begin/end/++/pt，每字段读独立 pt 调用共 16）；b 真才 FindSkill
    //（短路 `b && ...`），条件 `FindSkill < second`（FindSkill 先、second 后读）；
    // growtype 实参求值序 second→first→job→SkillW（Ghidra uVar1/uVar2 序）。
    for (int secondGrow = 0;
         secondGrow <= (int)(signed char)getCurCharSecondGrowType(); ++secondGrow)
    {
        for (int grade = a; grade <= get_pvp_grade(); ++grade)
        {
            std::vector<std::pair<int, int> >* tree = 0;
            if (G_CDataManager()->m_pvpSkillTree.getPvPSkillTree(
                    get_charac_job(), (int)(signed char)getCurCharFirstGrowType(),
                    secondGrow, grade, b, &tree) == 0)
                continue;   // P168a②直分支形：ORIG `test %al,%al; je` 无 xor 无岛
                            // ——`== false`/`!fn()` 均产 xor 段（判据②定字 ==0）
            for (std::vector<std::pair<int, int> >::iterator it = tree->begin();
                 it != tree->end(); ++it)
            {
                if (c == 1)
                {
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)0) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)0);
                    }
                }
                else if (c == 2)
                {
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)1) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)1);
                    }
                }
                else if (c == 3)
                {
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)0) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)0);
                    }
                    if (!b || ((SkillSlot*)getCurCharacSkillR())
                                  ->FindSkill(it->first,
                                              (ENUM_SKILL_TREE_KIND)1) < it->second)
                    {
                        ((SkillSlot*)getCurCharacSkillW())
                            ->growtype_skill(get_charac_job(), it->first,
                                             it->second, (ENUM_SKILL_TREE_KIND)1);
                    }
                }
            }
        }
    }
}

// 0x0867a95c

// 0x086804ce
// ORIG：断线回调。state==7（PvP 房间）时通知 PvP_Room::Disconnect；
// 在袭击区域（GetAssaultPlace != 0）时通知 CAssaultMgr::OnLeaveAssaultPlace(user, false)。
void CUser::OnDisconnect()
{
    if (get_state() == 7)
    {
        // ORIG 0x08??：pvpIdx 走 %ebx（`movswl %ax,%ebx` 单条跨 G_CGameManagerv
        // 存活）。short→int 拓宽必须作为 GetPvp 首实参**直传**（隐式转换，同
        // getBlueMarble probe s3）；`register int asm("ebx")` 中间变量反产 cwtl+mov。
        // pRoom 槽落 -0xc（无 short spill，技巧 21）。末段 ORIG 对
        // GetInstanceAssaultMgr 结果**无判空**、%eax 直用（技巧 37）。
        PvP_Room* pRoom =
            G_CGameManager()->GetPvp(GetPvpIndex(), this, 0);
        if (pRoom != 0)
            pRoom->Disconnect(this);
    }
    if (GetAssaultPlace() != 0)
        pvp_assault::GetInstanceAssaultMgr()->OnLeaveAssaultPlace(this, false);
}

// 0x0867a95c（返回 1 成功 / 0 角色为空；DisPatcher_DebugCommand 以 !=1 判失败）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867a95c | _ZN5CUser14SetCharacLevelEi
int CUser::SetCharacLevel(int level)
{
    // 0x0867a95c 主分支（SetCharacLevel.md）：等级钳制[1,200]；按方向取
    // GetSpAtLevelUp 计算 SP 增量加到树 0/1 并 clamp；exp=get_level_exp；
    // SFP：<50 清 idx2/3，旧级<0x33 则=level-0x31 否则各+差值；逐级加/减
    // 成长属性、写日志、premium 金币奖励；专家/公会/监控/副本通知收尾。
    if (getCurCharacR() == 0)
        return 0;
    if (level < 1) level = 1;
    if (level > 200) level = 200;

    int oldLevel = get_charac_level();
    int spGain = (oldLevel < level)
                     ? G_CDataManager()->GetSpAtLevelUp(level)
                     : G_CDataManager()->GetSpAtLevelUp(level + 1);
    int spDiff = (level - oldLevel) * spGain;

    setCurCharacExp(G_CDataManager()->get_level_exp(level));

    for (int kind = 0; kind <= 1; ++kind)
    {
        ENUM_SKILL_TREE_KIND k = (ENUM_SKILL_TREE_KIND)kind;
        int v = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(k) + spDiff;
        if (v > 0x3fffffff) v = 0x3fffffff;
        if (v < 0) v = 0;
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(v, k);
    }

    if (level < 0x32)
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(0, (ENUM_SKILL_TREE_KIND)2);
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(0, (ENUM_SKILL_TREE_KIND)3);
    }
    else
    {
        int sfp2, sfp3;
        if (oldLevel < 0x33)
        {
            sfp2 = sfp3 = level - 0x31;
        }
        else
        {
            sfp2 = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                       (ENUM_SKILL_TREE_KIND)2) + (level - oldLevel);
            sfp3 = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                       (ENUM_SKILL_TREE_KIND)3) + (level - oldLevel);
        }
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(sfp2, (ENUM_SKILL_TREE_KIND)2);
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(sfp3, (ENUM_SKILL_TREE_KIND)3);
    }

    // 成长类型属性表：G_CDataManager()->m_characList（ORIG set_grow_type 0x08678c36
    // `mov 0x14(%eax),%edi` 指针装载 + imul 0x7dc 实证；CCharacList::m_characs 表）
    CCharacList* growTable = G_CDataManager()->m_characList;
    const _Additioal_info* growStat = (const _Additioal_info*)growTable->m_characs[get_charac_job()]
        .get_growtype_stat(getCurCharFirstGrowType(), getCurCharSecondGrowType());

    while (get_charac_level() != level)
    {
        if (get_charac_level() < level)
        {
            // ORIG 0x867acca：先加成长属性再升级（升/降分支对称）
            ((_Additioal_info*)getCurCharacAddInfoRefW())->add(*growStat);
            incCurCharacLevel();
            m_historyLog.LevelUp(get_charac_level(), (eLevelUpReason)1);
            int bonus = m_premium.GetGoldBonus(get_charac_level());
            if (bonus != 0)
            {
                getCurCharacInvenW()->gain_money(bonus, (eMoneyAddReason)0xd, true, 0);
                SendUpdateItemList((eSendTarget)1, (ENUM_ITEMSPACE)0, 0);
            }
        }
        else
        {
            ((_Additioal_info*)getCurCharacAddInfoRefW())->subtract(*growStat);
            decCurCharacLevel();
            m_historyLog.LevelDown(get_charac_level(), (eLevelDownReason)0);
        }
        m_premium.RecalcAdditionalInfo(this);
    }
    setCurCharacLevel(level);

    // ORIG 0x867ae4c：专家职业——类型非 0 且等级命中限定等级 → 增加专家职业经验
    STExpertJobEtcScript* ejScript =
        (STExpertJobEtcScript*)G_CDataManager()->GetExpertJobEtcScript();
    if (GetCurCharacExpertJobType() != 0 && ejScript->isExpertJobLimitLevel(level))
    {
        expert_job::CExpertJob::IncreaseExpertJobExp(this, GetCurCharacExpertJobExp());
    }

    makeGuildLevelUpMessage(level);   // ORIG T 0x8679754

    if (level == 0x46 &&
        GlobalData::s_pGMAccounts_->isGM(get_acc_id()) != 1)
    {
        char pkt[0x28];   // Packet_Monitor_Max_Level_BroadCast（名字位于 +0x0a，推断）
        memset(pkt, 0, sizeof(pkt));
        strncpy(pkt + 0x0a, getCurCharacName(), 0x1e);
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        if (proxy != 0)
            proxy->SendPacket(pkt, 0x28);
    }

    // ORIG 0x867af66：等级==副本 0x2aff 最低要求等级时，按副本开放状态广播进出场
    if (get_charac_level() == G_GameWorld()->getDungeonMinimumRequiredLevel(0x2aff))
    {
        bool open = G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff) != 0;
        G_GameWorld()->send_user_dungeon_inout_message(this, 0x2aff, open);
    }

    if (get_charac_level() > 0x3b)
        DimensionInoutUpdate(true, true);

    m_characterView.enableSaveCharacView();
    return 1;
}

// 0x0866afa2：习得单技能主分支（master_new_skill.md）：
// skillIdx∈[0xc8,0xd0] 走公会技能（需会长/副会长）并经 guild proxy 上报；
// 否则普通 SkillSlot::master_new_skill。回包 header(1,0x1f)：失败字节0+错误码
// 映射{-1:0x15,-2:0x16,-3:0x12,-4:0x01,-5:0x5b,-6:0x5c}；成功 byte1+treeIdx+
// 剩余SP/SFP(short)+新槽位+skillIdx+等级。
// b28d-lane5 N封顶: P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x0866afa2 | _ZN5CUser16master_new_skillER14stBuySkillInfob
void CUser::master_new_skill(stBuySkillInfo& info, bool flag)
{
    (void)flag;
    int treeIdx = GetCurCharacSkillTreeIndex();
    unsigned char skillIdx = (unsigned char)info.m_skillIdx;
    int ret;
    int learnCnt = 0;
    int needGold = 0;
    short needGSP = 0;
    bool isGuildSkill = (skillIdx >= 0xc8 && skillIdx <= 0xd0);

    if (isGuildSkill)
    {
        if (!IsGuildMaster() && !IsSubGuildMaster())
            return;
        // ORIG 0x0866b..：经 GetGuildDBInfo() 访问器取 guildDB(+0x42=m_guildSkillPoint)。
        unsigned short guildLevel =
            ((STGuildDBInfoOnly*)GetGuildDBInfo())->m_guildSkillPoint;
        ret = ((SkillSlot*)getCurCharacSkillR())->master_new_guild_skill(
            (int)skillIdx, get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            *reinterpret_cast<STGuildDBInfoOnly*>(GetGuildDBInfo()), (int)getCurCharacMoney(),
            needGold, learnCnt, needGSP, (int)guildLevel);
        if (ret >= 0)
        {
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
            if (proxy != 0)
                proxy->SendBuyGuildSkill((unsigned int)get_charac_no(-1),
                                         (unsigned int)get_charac_guildkey(),
                                         (int)skillIdx, learnCnt, needGSP,
                                         (unsigned int)needGold);
        }
    }
    else
    {
        if (treeIdx == -1 || treeIdx == 0)
        {
            if (info.m_field0 == 1)
                return;
        }
        else if (info.m_field0 != 1)
        {
            return;
        }
        ret = ((SkillSlot*)getCurCharacSkillW())->master_new_skill(
            (int)skillIdx, get_charac_level(), get_charac_job(),
            (int)getCurCharFirstGrowType(), (int)getCurCharSecondGrowType(),
            (int)info.m_count);
    }

    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(1, 0x1f);
    if (ret < 0)
    {
        pkt->put_byte(0);
        // ORIG 0x0866b0xx 跳表(0x8cef5b0)：每 ret 值一独立 put_byte 常量站点（6 case），
        // 非数组索引。展开以匹配 put_byte 调用多重集。
        switch (ret)
        {
            case -6: pkt->put_byte(0x5c); break;
            case -5: pkt->put_byte(0x5b); break;
            case -4: pkt->put_byte(0x01); break;
            case -3: pkt->put_byte(0x12); break;
            case -2: pkt->put_byte(0x16); break;
            case -1: pkt->put_byte(0x15); break;
            default: break;
        }
    }
    else
    {
        pkt->put_byte(1);
        // ORIG 0x0866b3b2..：put_byte/sp/sfpKind 判定各重调 GetCurCharacSkillTreeIndex()
        //（P44），sfp 按 (tree==-1||tree==0) 分 kind2/kind3 两独立 get_remain_sfp 站点。
        pkt->put_byte(GetCurCharacSkillTreeIndex());
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)GetCurCharacSkillTreeIndex()));
        int treeForSfp = GetCurCharacSkillTreeIndex();
        if (treeForSfp == -1 || GetCurCharacSkillTreeIndex() == 0)
            pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)2));
        else
            pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
                (ENUM_SKILL_TREE_KIND)3));
        if (!isGuildSkill)
        {
            pkt->put_byte(ret);
            pkt->put_byte(skillIdx);
            int level = ((SkillSlot*)getCurCharacSkillR())->get_skill_level(ret);
            pkt->put_byte(level);
        }
        else
        {
            pkt->put_byte(0xff);
            pkt->put_byte(skillIdx);
            pkt->put_byte(learnCnt);
        }
        // ORIG 0x0866b4b4：if/else 之后两分支共用一次 getCurCharacSkillR()+get_skill_level()
        // 再 WriteLearnSkill（原实现误置于 !isGuildSkill 内，公会技能分支漏记日志）。
        int learnLevel = ((SkillSlot*)getCurCharacSkillR())->get_skill_level(ret);
        HistoryLog::WriteLearnSkill((FILE*)m_field796f8, (int)skillIdx, learnLevel);
    }
    pkt->finalize(true);
    Send(packet);
}

// 0x0867b048

// ORIG：GM 改成长类型。无角色/参数越界(0..5, 0..1) → 0 并 cerr 报错；
// 否则 debugCheckGrowTypeSkill 校验 → set_grow_type(reason=2) →
// _postCheckForceChangeGrowType 后处理，返回 1。
// 返回 int：ORIG 0x0867b048 尾 `mov eax,0x1; ret`、早退 `mov eax,0x0`
// （Model22 桥清零实证；声明 CUser.h:3330 由 Main 代落）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867b048 | _ZN5CUser17ChangeGrowType_GMEii
int CUser::ChangeGrowType_GM(int a, int b)
{
    if (getCurCharacR() == 0)
        return 0;
    if (a < 0 || a > 5 || b < 0 || b > 1)
    {
        std::cerr << "Invalid grow type , first :" << a << "second :" << b << std::endl;
        return 0;
    }
    int level = get_charac_level();
    // ORIG 0x0867b5f4：GetPremiumInfo() 转发后 getOverSkillLevel，不直读 m_premium
    unsigned int overLevel = GetPremiumInfo()->getOverSkillLevel();
    SkillSlot* slotW = (SkillSlot*)getCurCharacSkillW();
    slotW->debugCheckGrowTypeSkill(level + (int)(overLevel & 0xffff),
                                   get_charac_job(), a, b);
    set_grow_type((unsigned char)a, (unsigned char)b, 0, (eChangeGrowTypeReason)2);
    _postCheckForceChangeGrowType(this, a, b);
    return 1;
}

// 0x0867b6d4
// ORIG：添加物品。角色空/交易中/物品不存在/count<0 → -1；
// 构造 Inven_Item（make_item），stackable 则 set_add_info(count)；
// *space 默认 0（生物物品 → 7）；slot∈(0,0x20) 且为装备 → SetUpgrade(slot)；
// insertItemIntoInventory(reason, true, true)，失败 -1；
// 生物物品再 InsertCreatureItem。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867b6d4 | _ZN5CUser7AddItemEii14eItemAddReasonR14ENUM_ITEMSPACEi
int CUser::AddItem(int itemIdx, int count, eItemAddReason reason,
                   ENUM_ITEMSPACE& space, int slot)
{
    if (getCurCharacR() == 0)
        return -1;
    if (CheckInTrade() != 0)
        return -1;
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return -1;
    if (count < 0)
        return -1;
    Inven_Item inven;
    item->make_item(inven);
    if (item->is_stackable() != 0)
        inven.set_add_info(count);
    space = (ENUM_ITEMSPACE)0;
    if (inven.IsCreatureItemType() != 0)
        space = (ENUM_ITEMSPACE)7;
    if (slot > 0 && slot < 0x20 && inven.m_field1 == (unsigned char)1)
        inven.SetUpgrade((unsigned char)slot);
    CInventory* invenW = getCurCharacInvenW();
    int ret = invenW->insertItemIntoInventory(inven, reason, true, true);
    if (ret < 0)
        return -1;
    if (inven.IsCreatureItemType() != 0)
    {
        int expiration = item->getExpirationDate();
        int usable = item->getUsablePeriod();
        getCurCharacInvenW()->GetCreatureMgrW()->InsertCreatureItem(
            &inven, ret, (int)reason, usable, expiration);
    }
    return ret;
}

// 0x0864cb66
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864cb66 | _ZN5CUser21LogoutCachedCharacterEh
void CUser::LogoutCachedCharacter(unsigned char param)
{
    // 0x0864cb66 主分支（LogoutCachedCharacter.md）：货币日志 → 缓存角色内存 →
    // Guild/Monitor 代理 Logout → param==0 双开检测 UDP Logout → PC 房通知 →
    // 清 ontime 奖励 → HeroMission 存档。
    SendMoenyLog();
    unsigned char cached = (unsigned char)CacheCharacterMemory();

    // ORIG 0x0864cb66：Guild 段与 Monitor 段各自完整重新取参（P48 no-CSE），
    // 无 GetServerProxy 判空——直接解引用调用 SendLogout（P140/P171 照抄）。
    unsigned int memberKey = (unsigned int)get_charac_memberkey();
    unsigned int guildKey = (unsigned int)get_charac_guildkey();
    unsigned int characNo = (unsigned int)get_charac_no(-1);
    char* name = (char*)getCurCharacName();
    int channel = G_CEnvironment()->get_channel_no();
    unsigned int accId = get_acc_id();
    CGuildServerProxy* guildProxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    guildProxy->SendLogout(accId, channel, name, characNo,
                           guildKey, memberKey, param);

    memberKey = (unsigned int)get_charac_memberkey();
    guildKey = (unsigned int)get_charac_guildkey();
    characNo = (unsigned int)get_charac_no(-1);
    name = (char*)getCurCharacName();
    channel = G_CEnvironment()->get_channel_no();
    accId = get_acc_id();
    CMonitorServerProxy* monProxy =
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
    monProxy->SendLogout(accId, channel, name, characNo,
                         guildKey, memberKey, (int)param, cached);

    if (param == 0)
    {
        CEnvironment* env = G_CEnvironment();
        int dcGroup = env->m_serverEnvir.m_serverGroup;   // §4.3 具名(env+0x378)
        int ch = G_CEnvironment()->get_channel_no();       // ORIG 二次取 env
        GlobalData::s_double_check_proxy->SendLogout(get_acc_id(), ch, dcGroup);
    }

    if (getCurCharacR() != 0)
        SendLogoutToPCRoomServer();       // ORIG T 0x0868170c

    delete_ontime_reward_list();          // ORIG T 0x0868c3bc
    // ORIG T 0x08166d38：ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, true)
    ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, true);
}

// 0x08689a22：表情配置重算（ReCalcChattingEmoticon.md）。读 GameOption 表情配置 →
// CDataManager 重算 → 写回；变化则组 SIG_SAVE_CHATTING_EMOTICON（0x214：+0 accId、
// +4.. 配置 0x210）投递队列 2。SIG 当前 pad 布局按偏移写入（推断）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08689a22 | _ZN5CUser22ReCalcChattingEmoticonEv
void CUser::ReCalcChattingEmoticon()
{
    CGameOption* opt = GetGameOptionRef();
    CChattingEmoticonConfig cfg = opt->getEmoticonInfo();
    G_CDataManager()->ReCalcEmoticonConf(cfg, *this);
    if (!opt->SetEmoticonInfo(cfg))
        return;

    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0xa076);
    CStreamGuard guard(s, true);
    guard << (int)0x2d7;
    guard << (int)GetUID();
    char* sig = (char*)guard.GetInBuffer<SIG_SAVE_CHATTING_EMOTICON>();
    *(unsigned int*)(sig + 0) = get_acc_id();
    memcpy(sig + 4, &cfg, 0x210);
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// 0x08689010：表驱动重置列表（ResetItemByScript.md）：元素 +0 itemspace、
// +4 到期秒；到期则压 (space,0)。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08689010 | _ZN5CUser17ResetItemByScriptERSt6vectorISt4pairIiiESaIS2_EE
void CUser::ResetItemByScript(std::vector<std::pair<int, int> >& list)
{
    // 残差 P75/N80 族（跨版本实参装载中转寄存器差）：两处 operator[](i) 的
    // 实参 i 装载 ORIG 4.1 经 %edx 中转入 0x4(%esp)，4.4 恒 %eax。求值序/槽位/
    // 调用全同，仅中转寄存器名差，源形不可复现。
    // p167 差集（b28）：ORIG 走 `operator[](uint) const`×2 + make_pair<int const&,int>
    // ⇒ 原码以 const 指针访问（容器 cv 限定决定实例化，P96）。
    const std::vector<stResetItemInfo::ResetItem>* items =
        G_CDataManager()->GetResetItemInfo();
    int now = GlobalData::s_systemTime_.getCurSec();
    for (size_t i = 0; i < items->size(); ++i)
    {
        if ((*items)[i].m_resetSec <= now)
            list.push_back(std::make_pair((*items)[i].m_space, 0));
    }
}

// 0x08689b90：聊天表情配置包(0x19b)。24 行：index、m_nCount(行内 +0x14)、
// 名字（行首 0x14 字节）。行大小 0x16（CChattingEmoticonConfigRow, pack(1)）。
void CUser::SendChattingEmoticon()
{
    // ORIG 0x8689b90：GetGameOptionRef 结果**落槽 -0x10** 再作 this 调
    // getEmoticonInfo（sret 就地构 cfg@-0x22c）；行地址**每使用点重算**
    // （`imul $0x16` 段，无 row 缓存变量——strlen/put_str 各自 lea+imul）。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x19b);
    ((InterfacePacketBuf*)&packet)->put_short(0x18);
    CGameOption* go = GetGameOptionRef();
    CChattingEmoticonConfig cfg = go->getEmoticonInfo();
    for (int i = 0; i < 0x18; ++i)
    {
        ((InterfacePacketBuf*)&packet)->put_short(i);
        ((InterfacePacketBuf*)&packet)->put_short(cfg.m_Row[i].m_nCount);
        ((InterfacePacketBuf*)&packet)->put_int((int)strlen(cfg.m_Row[i].m_data));
        ((InterfacePacketBuf*)&packet)->put_str(cfg.m_Row[i].m_data,
                                                (int)strlen(cfg.m_Row[i].m_data));
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// 0x08653270：登出前数据保存主链路（UpdateData.md）。守卫 state>2 且频道!=7；
// CheckNFix；会员税上报并清零；_SaveCharacData/UpdateCharacView/HeroMission；
// ---- UpdateData uses DB request classes declared in W8LT_DBTask.h ----
// The declarations are real dispatcher types; makeRequest definitions remain
// in W8LT_DBTask.cpp and this TU only invokes them.
// §6.2 闭合：P167 调用点差集 CLEAR (0/0, 271=271)。修复：hack 计数循环
// 守卫+取值双读 getServerHackCnt（3×getHackAnalyzer/2×getServerHackCnt）；
// SIG_OBJECT_BRING_UP m_serverGroup 改内联 m_serverEnvir.m_serverGroup（ORIG
// 直读 +0x378 &0xff，无 get_server_group() 调用）。verdict orig1246/our1293，
// 残余 gap 属 N82/-O0 帧差异，逐块 vs Ghidra C 全对齐。

// §6.2 巨件核销：P167 271/271 [0缺0多]；gap 1246→1293（our+47）；残差归因 N82
// （4.1 无 lifetime 槽复用 ⇒ 全帧槽号错位，ndiff 941 与语义完成度反向）+
// CStreamGuard 链式写法的槽物化差；EXTRA=0 已排除多发语义（任务书关注点），
// 语义完整仅剩 codegen 差，N 封顶（详见 docs/asm_translation_patterns.md 负例表）。
// 各 isSaveXxx 脏块经 StreamPool+CStreamGuard 组包投递队列 2 后 disable。
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; N82/P112 槽号分配 | ORIG 0x08653270 | _ZN5CUser10UpdateDataEv
void CUser::UpdateData()
{
    if (m_chState <= 2)
        return;
    if (G_GameWorld()->GetChannelType() == 7)
        return;
    // ORIG +0x9f：无当前角色 → trace（0x1d99）后直接返回，不发任何保存。
    if (getCurCharacR() == 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1d99, 5);
        trace("CUser::UpdateData() character %s - m_selected is null",
              getCurCharacName());
        return;
    }
    // ORIG +0x121：数据加载未完成 → trace（0x1da1）后直接返回。
    if (isLocked4DataLoad())
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x1da1, 5);
        trace("Can not save character until loading done(m_id: %s, charac_no: %d",
              NumberToString(get_acc_id(), 0), getCurCharacNo());
        return;
    }

    if (getCurCharacInvenR()->checkToFix())
        getCurCharacInvenW()->CheckNFix();

    if (getCurCharacR() != 0 && get_charac_memberkey() != 0)
    {
        int payTaxMoney = get_member_pay_tex_money_to_upper();
        int payTaxFatigue = get_member_pay_tex_fatigue_to_upper();
        CMonitorServerProxy* monProxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        if (monProxy != 0)
            monProxy->SendMemberPayTax(is_connect_upper_member() != 0,
                                       (unsigned int)getCurCharacNo(),
                                       payTaxMoney, payTaxFatigue);
        if (payTaxMoney > 0)
            set_member_pay_tex_money_to_upper(0);
        if (payTaxFatigue > 0)
            set_member_pay_tex_fatigue_to_upper(0);
    }

    _SaveCharacData();                    // ORIG T 0x08650aa2
    UpdateCharacView();
    ARAD::Singleton<HeroMissionEvent>::Get()->saveEventData(this, false);  // ORIG T 0x08166d38

    if (isSaveInven() || isSaveCargo())
    {
        // ORIG +0x27b：state(+0x8cfc4)==5（角色选择视口阶段）仅存背包。
        if (m_chState == 5)
            SaveInventoryOnly();
        else
            SaveInventory();
        disableSaveInven();
        disableSaveCargo();
    }

    WongWork::CMailBoxHelper::ReqDBSendStoredMail(this);

    if (isSaveSkill())
    {
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1ded);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<((int)0x24);              // SIG_LOAD_SKILL
        guard.operator*()->operator<<((int)GetUID());
        SIG_LOAD_SKILL* sig = guard.operator->()->GetInBuffer<SIG_LOAD_SKILL>();
        memset(sig, 0, sizeof(SIG_LOAD_SKILL));           // 0x414
        // ORIG +0x394：+0x00 槽写 getCurCharacNo（movl 4B 实证）。
        sig->m_characNo = (unsigned int)getCurCharacNo();
        // ORIG 0x0865360e `movzbl 0xa850(%eax)`（unsigned char 宽实证）
        sig->m_field411 = G_CDataManager()->m_pvpSkillTree.m_field50;
        int sp1, sp2, sfp1, sfp2;
        ((SkillSlot*)getCurCharacSkillR())->get_remain_sp(&sp1, &sp2, &sfp1, &sfp2);
        sig->m_remainSpTree1 = sp1;
        sig->m_remainSpTree2 = sp2;
        sig->m_remainSfpTree1 = sfp1;
        sig->m_remainSfpTree2 = sfp2;
        _Mastered_skill* buf0 =
            ((SkillSlot*)getCurCharacSkillR())->get_skillslot_buf((ENUM_SKILL_TREE_KIND)0);
        _Mastered_skill* buf1 =
            ((SkillSlot*)getCurCharacSkillR())->get_skillslot_buf((ENUM_SKILL_TREE_KIND)1);
        if (buf0 != 0 && buf1 != 0)
        {
            // ORIG 拷贝 0x198 字节（= 0xcb 槽 × 2B，越界 2 字节读忠实保留）。
            memcpy(sig->m_skillSlotTree1, buf0, 0x198);
            memcpy(sig->m_skillSlotTree2, buf1, 0x198);
        }
        // ORIG +0x49a：技能命令区（+0x348，200 字节）拷贝后清零自身区（+0x703db）。
        STSkillCommandData::STCommand* cmdBuf = get_skill_command_buf();
        if (cmdBuf != 0)
            memcpy(sig->m_commands, cmdBuf, 200);
        memset(m_skillCommands, 0, sizeof(m_skillCommands));   // 200 = 0xc8
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        updateComboSkill();                 // ORIG T 0x08692396
        disableSaveSkill();
    }

    if (isSavePvP())
    {
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1e2c);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<((int)0x25);              // SIG_SAVE_PVP
        guard.operator*()->operator<<((int)GetUID());
        SIG_SAVE_PVP* sig = guard.operator->()->GetInBuffer<SIG_SAVE_PVP>();
        memset(sig, 0, sizeof(SIG_SAVE_PVP));      // 0x88
        sig->m_characNo = getCurCharacNo();
        // ORIG：PvpResultType::Set((PvpResultType*)sig, getPVPResultRefR()) 整体拷入前段。
        ((PvpResultType*)sig)->Set(*getPVPResultRefR());
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSavePvP();
    }

    if (isSaveDungeonClear())
    {
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1e4e);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<((int)0x27);              // SIG_SAVE_DUNGEON_CLEAR
        guard.operator*()->operator<<((int)GetUID());
        char* sig = (char*)guard.operator->()->GetInBuffer<SIG_SAVE_DUNGEON_CLEAR>();
        memset(sig, 0, 0x180c);
        *(unsigned int*)(sig + 0) = (unsigned int)getCurCharacNo();
        *(unsigned int*)(sig + 4) = get_acc_id();
        // ORIG +0x77b：CDungeonClear::toString(sig+0xc, 0x1800) 序列化；
        // 返回负值（shr 31）走 "Quest buffer overflow" 报错 trace。
        if (getDungeonClearRefR()->toString(sig + 0xc, 0x1800) < 0)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x1e5e, 5);
            trace("Quest buffer overflow (m_id: %s, charac_no: %d",
                  NumberToString(get_acc_id(), 0), getCurCharacNo());
        }
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveDungeonClear();
    }

    if (isSaveBattleRecord())
        disableSaveBattleRecord();     // ORIG +0x870：战斗记录脏标记仅清除

    if (isSaveQuest())
    {
        Stream* s = GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1e8c);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<((int)0x29);              // SIG_UPDATE_QUEST
        guard.operator*()->operator<<((int)GetUID());
        SIG_UPDATE_QUEST* sig = guard.operator->()->GetInBuffer<SIG_UPDATE_QUEST>();
        memset(sig, 0, sizeof(SIG_UPDATE_QUEST));
        // questR+0x04 即 UserQuest::m_cleared（CQuestClear；+0x7538=m_quest[i]、
        // +0x7588=m_trigger[i]、+0x75f0/0x75f4 在 m_authenData 内）。
        // ORIG 每处重调 getCurCharacQuestR()（P44，含循环内），共 6 站点。
        if (getCurCharacQuestR()->m_cleared.checkSaveFlag())
        {
            memcpy(sig->m_clearedQuest,
                   getCurCharacQuestR()->m_cleared.getClearedQuest(), 30000);
            sig->m_clearedFlag = 1;
        }
        sig->m_characNo = (unsigned int)getCurCharacNo();
        for (int i = 0; i < 0x14; ++i)
        {
            sig->m_quest[i] =
                (unsigned int)getCurCharacQuestR()->m_quest[i];
            sig->m_trigger[i] =
                (unsigned int)getCurCharacQuestR()->m_trigger[i];
        }
        sig->m_urgentQuest =
            (unsigned int)getCurCharacQuestR()->m_authenData.m_urgentQuest;
        memcpy(sig->m_authen, getCurCharacQuestR()->m_authenData.m_authen, 0x28);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        // ORIG +0xa70：put 后清 QuestClear 保存标记再 disable。
        getCurCharacQuestW()->m_cleared.resetSaveFlags();
        disableSaveQuest();
    }

    // ORIG +0xaae：宠物（creature）项脏块经背包 CreatureMgr 落盘（读引用+R 视图取 W mgr，
    // 同 CUser.cpp 830 行先例 cast 掉 const）。
    if (((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->IsSaveCreatureItem())
        ((CInventory*)getCurCharacInvenR())->GetCreatureMgrW()->SaveCreatureItem();

    // ORIG 判定与取值各重调一次 GetCurCharacEventItemCount()（P44）。
    if (GetCurCharacEventItemCount() != 0)
    {
        // ORIG +0xafd：DB_UpdateItemCountValue::makeRequest(get_acc_id(), count)（SIG 0x240）。
        DB_UpdateItemCountValue::makeRequest(get_acc_id(),
                                             (unsigned int)GetCurCharacEventItemCount());
        ResetCurCharacEventItemCount();
    }

    if (isSaveInformNoticeFlag())
    {
        Stream* s2 = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1f2b);
        CStreamGuard guard(s2, true);
        guard.operator*()->operator<<((int)0x2f5);             // InformNoticeFlagData
        guard.operator*()->operator<<((int)-1);
        char* sig = (char*)guard.operator->()->GetInBuffer<InformNoticeFlagData>();
        memset(sig, 0, 0x14);
        *(unsigned int*)(sig + 0) = get_acc_id();
        // ORIG +0xc05：i∈[0,0x10) 逐位 getInformNoticeFlag(i) 填 +4..+0x13。
        for (int i = 0; i < 0x10; ++i)
            *(sig + 4 + i) = (char)getInformNoticeFlag(i);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        disableSaveInformNoticeFlag();
    }

    if (GetCurCharacUsedFatigueQuantity() != 0)
    {
        // ORIG +0xc90：DB_UpdateUserFatigueQuantity::makeRequest(accId, qty)（SIG 0x287）。
        DB_UpdateUserFatigueQuantity::makeRequest(
            get_acc_id(),
            (unsigned int)GetCurCharacUsedFatigueQuantity());
        ResetCurCharacUsedFatigueQuantity();
    }
    // ORIG +0xcd8：赠礼疲劳脏标记 → 三参 DB 请求（SIG 0x288，accId/characNo/giftQty
    // 依报告反编译 C 实参序）后复位标记。
    if (GetChangedGiftFatigueQuantity())
    {
        DB_UpdateUserGiftFatigueQuantity::makeRequest(
            get_acc_id(), getCurCharacNo(),
            (unsigned int)GetCurCharacUsedGiftFatigueQuantity());
        SetChangedGiftFatigueQuantity(false);
    }

    // ORIG +0xd24：事件 0x19b（OBJECT_BRING_UP）进行中且攒有材料数 → 投递 SIG 并复位。
    EventClassify::CEventScriptMng* esm =
        (EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng();
    if (esm->is_eventing(0x19b) && getCurUseBringUpMaterialCount() != 0)
    {
        Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x1f73);
        CStreamGuard guard(s, true);
        guard.operator*()->operator<<((int)0x2ca);             // SIG_OBJECT_BRING_UP
        guard.operator*()->operator<<((int)-1);
        SIG_OBJECT_BRING_UP* sig = guard.operator->()->GetInBuffer<SIG_OBJECT_BRING_UP>();
        sig->m_accId = get_acc_id();
        sig->m_characNo = getCurCharacNo();
        // ORIG：+0x08 = *(G_CEnvironment()+0x378) & 0xff = m_serverEnvir.m_serverGroup
        // 低字节。P167：ORIG 为内联字段直读（无 get_server_group() 调用），
        // 故用 m_serverEnvir.m_serverGroup 内联访问，勿经 get_server_group() 访问器。
        sig->m_serverGroup = (unsigned char)G_CEnvironment()->m_serverEnvir.m_serverGroup;
        sig->m_usedMaterialCnt = getCurUseBringUpMaterialCount();
        sig->m_rewardFlag = 0;           // ORIG：+0x0d 恒 0
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
        resetCurUseBringUpMaterialCount();
    }

    // ORIG 判定与取值各重调一次 GetCurCharacEventAddItemCount()（P44）。
    if (GetCurCharacEventAddItemCount() != 0)
    {
        // ORIG +0xedc：DB_UpdateAddItemCountValue::makeRequest(accId, count)（SIG 0x280）。
        DB_UpdateAddItemCountValue::makeRequest(get_acc_id(),
                                                (unsigned int)GetCurCharacEventAddItemCount());
        ResetCurCharacEventAddItemCount();
    }

    // ORIG +0xee8：hack 类型区间 [0x65,0xfa0] 服务器计数落库并复位。
    for (int hackType = 0x65; hackType <= 0xfa0; ++hackType)
    {
        // ORIG 0x086541dd：守卫读一次 getServerHackCnt（iVar25 != 0），进入后
        // 再读第二次作为 ReqDBSaveHackInfo 实参（3 次 getHackAnalyzer / 2 次
        // getServerHackCnt，P167 缺口），勿缓存单次计数。
        if (getHackAnalyzer()->getServerHackCnt(
                (WongWork::ENUM_HACKTYPE)hackType) != 0)
        {
            WongWork::CHackAnalyzer::ReqDBSaveHackInfo(
                get_acc_id(), (unsigned int)get_charac_no(-1),
                (unsigned short)hackType, GetETC((short)hackType),
                getHackAnalyzer()->getServerHackCnt(
                    (WongWork::ENUM_HACKTYPE)hackType));
            getHackAnalyzer()->resetServerHackCnt(
                (WongWork::ENUM_HACKTYPE)hackType);
        }
    }
    // ORIG +0xfd6：MC 点>499 → 曝光计数与点落库（type 0x329）后清曝光。
    if (m_mcap.getPoint() > 499)
    {
        WongWork::CHackAnalyzer::ReqDBSaveHackInfo(
            get_acc_id(), (unsigned int)get_charac_no(-1), 0x329,
            (int)m_mcap.getPoint(), (int)m_mcap.getExposedCount());
        m_mcap.resetExposedCount();
    }

    // ORIG +0x1046：学校点（脏累加）落库后复位。
    if (getCurCharacSchoolPoint() != 0)
    {
        DB_UpdateSchoolPoint::makeRequest(get_acc_id(),
                                          getCurCharacSchoolPoint(),
                                          (int)GetUID());
        resetCurCharacSchoolPoint();
    }

    // ORIG +0x109a：血之最高纪录/维度进出/断线运气/断线副本计数等脏块逐一落库。
    if (isBloodBestRecordUpdate())
        UpdateBloodRecord();
    if (get_charac_level() > 0x3b &&
        (IsSaveDemensionInoutValue() || IsSaveBloodInoutValue()))
        UpdateDimensionInout();
    if (isUsedBreakAwayLuckPoint())
        UpdateBreakAwayLuckPoint();
    if (isBreakAwayDungeonClear())
        UpdateBreakAwayDungeonClearCnt();
    // ORIG +0x115b：角色链接奖励 exp/gold 任一带值 → 提交（state=2）。
    if (getCharacLinkBonusExp() != 0 || getCharacLinkBonusGold() != 0)
        UpdateCharacLinkBonus(getCharacLinkBonusExp(), getCharacLinkBonusGold(),
                              (unsigned int)getLinkCharacNo(getLinkCharacSlotIndex()),
                              (ENUM_CHARAC_LINK_BONUS_STATE)2);

    // ORIG +0x11c3：NPC 关系 / 扩展数据链 / 条件事件 / 适正副本数。
    _updateNPCRelationship();
    m_characExpand.update(this);
    _saveConditionEventInfo(getCurCharacNo(), GetCurConditionEventStep(),
                            GetCurConditionEventRewardStep());
    _saveProperDungeonClearCount(getCurCharacNo(),
                                 GetProperDungeonClearCount());
    if (isSaveCharacOption())
        _updateSaveOption();

    // ORIG +0x1275：账号仓库有改动 → DB_SaveAccountCargo（SIG 0x1f1；随后 SetStable）。
    if (IsExistAccountCargo())
    {
        CAccountCargo* cargo = GetAccountCargo();
        if (cargo->IsAlter())
        {
            DB_SaveAccountCargo::makeRequest((int)GetUID(), get_acc_id(),
                                             cargo);
        }
    }
    // ORIG +0x12c7：金币保护存库 + 杂项周期落库。
    GetGoldControl()->SavetoDB(true, true, false);
    SaveMercenaryData();
    updatePcRoomPlayTime();
    updateBlueMarbleEnterCount();
    updateBingoData();
}

// 0x086554aa：登出主链路（UpdateLogout.md）。SIG_UPDATE_LOGOUT(0x2c, 0x8b 字节)：
// accId/characNo/gcNo/fatigue；state>1 记 playTime/IP/机器码/会员奖励疲劳并复位；
// 两条 trace（0x22e0/0x22e2）；写登录时间戳；历史日志 Logout；DB 杂项（复仇/
// ontime/疲劳出勤）；!param 发包(1,3)；清商城内购限制。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086554aa | _ZN5CUser12UpdateLogoutEb
void CUser::UpdateLogout(bool param)
{
    UpdateCharacView();

    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x2260);
    CStreamGuard guard(s, true);
    guard << (int)0x2c;
    guard << (int)GetUID();
    SIG_UPDATE_LOGOUT* sig = guard.GetInBuffer<SIG_UPDATE_LOGOUT>();
    memset(sig, 0, sizeof(SIG_UPDATE_LOGOUT));
    sig->m_accId = (unsigned int)get_acc_id();
    // ORIG 0x865557c：IntegratedPvP 频道用 get_gc_no_hardcode(GetServerGroup())。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        sig->m_gcNo =
            (unsigned int)G_CEnvironment()->get_gc_no_hardcode(GetServerGroup());
    else
        sig->m_gcNo = (unsigned int)G_CEnvironment()->get_gc_no();
    if (getCurCharacR() != 0)
    {
        sig->m_characNo = (unsigned int)getCurCharacNo();
        sig->m_fatigue = GetFatigue();
    }
    ((XNuclear::CHades*)getHades())->Send_Logout();   // ORIG 0x8655605

    long now = (long)GlobalData::s_systemTime_.getCurSec();
    sig->m_stateFlag = 0;
    if (get_state() > 1)
    {
        sig->m_stateFlag = 1;
        sig->m_playTime =
            (int)(now - m_premium.m_loginTime);   // 推断：登录时刻
        sig->m_field17 = (unsigned int)m_tradeCount;   // CUser+0x8d10c（ORIG movl）
        sig->m_playExpAdd = m_gameOption.m_playExpAdd;   // ORIG this+0x703a4 = m_gameOption+0x640
        sig->m_totalFatigueUsed =
            (unsigned short)m_postChecksumData.m_totalFatigueUsed;   // ORIG this+0x79642 实证
        sig->m_puUser = GetPuUser();
        SetUserManagerToolIP(this, reinterpret_cast<SIG_UPDATE_LOGOUT*>(sig));  // ORIG 0x865569e 实证传结构基址

        char mac[0x41];
        memset(mac, 0, sizeof(mac));
        memcpy(mac, m_network.get_mac_addr(), 0x41);
        strncpy(sig->m_mac, mac, 0x41);
        sig->m_saveMemberBonusFlag = (unsigned char)(IsEnableSaveMemberBonusFatigue() ? 1 : 0);
        DisableSaveMemberBonusFatigue();
        sig->m_memberBonusFatigue =
            (unsigned short)getAccountMemberBonusFatigue();
        resetAccountMemberBonusFatigue();
    }
    {
        // ORIG 0x8655791/0x8655803：0x71ba4 更新前后各打一条 trace。
        int mid = get_unique_id();
        int playTime = sig->m_playTime;
        cMyTrace tr1(__PRETTY_FUNCTION__, 0x22e0, 0);
        tr1("trace mid:%d playTime:%d login_time:%d now_time:%d",
            mid, playTime, m_premium.m_loginTime, (int)now);
    }
    m_premium.m_loginTime = now;
    {
        int mid = get_unique_id();
        int playTime = sig->m_playTime;
        cMyTrace tr2(__PRETTY_FUNCTION__, 0x22e2, 0);
        tr2("trace mid:%d playTime:%d login_time:%d now_time:%d",
            mid, playTime, m_premium.m_loginTime, (int)now);
    }

    bool sendOut = get_acc_id() != 0 ||
                   G_GameWorld()->IsIntegratedPvPBaseChannel() ||
                   G_GameWorld()->GetChannelType() == 7;
    if (sendOut)
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    else
        GlobalData::s_stream_pool->Free(guard);

    if (m_network.get_str_ip()[0] != '\0' && getCurCharacName() != 0)
    {
        int startTick = GetCharacStartPlayTick();
        if (startTick > 0)
        {
            long cur = (long)GlobalData::s_systemTime_.getCurSec();
            m_historyLog.Logout(getWebAddress(), m_network.get_str_ip(),
                                m_network.get_port(),
                                (unsigned short)G_CEnvironment()->m_serverEnvir.m_channelNo,
                                (int)(cur - startTick), GetLogInOutState());
        }
    }

    DBUpdateDBLogItem();
    ResetDBLogItem();

    {
        unsigned int characNo = getCurCharacNo();
        unsigned char villageCnt = GetCurVillageAttackCount();
        unsigned char revengeCnt = GetCurRevengeDungeonCount();
        if (IsVillageAttackDBUpdate())
            DB_UpdateRevengeData::makeRequest(characNo, villageCnt, revengeCnt);
    }
    if (is_update_ontime_last_recv_idx())
    {
        DB_UpdateOntimeLastRecvIdx::makeRequest(GetUID(), get_acc_id(),
                                                get_ontime_last_recv_idx());
        reset_update_ontime_last_recv_idx();
    }
    if (((EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng())
            ->is_eventing(0x56))
    {
        if (get_server_fatigue_day_size() != 0)
        {
            DB_UpdateFatigueAttendance::makeRequest(
                get_acc_id(), m_map8eb50);
            reset_server_fatigue_data();
        }
    }
    reset_ingame_event_history();

    if (!param)
    {
        PacketGuard pkt;
        ((InterfacePacketBuf*)&pkt)->put_header(1, 3);
        ((InterfacePacketBuf*)&pkt)->put_byte(1);
        ((InterfacePacketBuf*)&pkt)->finalize(true);
        Send(pkt);
    }
    CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
}

// 0x0867e734：日删可堆叠物品（deleteDailyStackableItem.md）。区间 [a,b] 三重过滤
// （空槽/find_item/is_stackable/+0x3c5>0 日删标记）后删除并压 (space=0, slot)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867e734 | _ZN5CUser24deleteDailyStackableItemERSt6vectorISt4pairIiiESaIS2_EEii
void CUser::deleteDailyStackableItem(std::vector<std::pair<int, int> >& list,
                                     int a, int b)
{
    if (getCurCharacR() == 0)
        return;
    CInventory* inven = (CInventory*)getCurCharacInvenR();
    for (int slot = a; slot <= b; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot((INVEN_TYPE)1, slot);
        if (item.m_addInfo == 0)
            continue;
        CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
        if (ci == 0 || !ci->is_stackable())
            continue;
        if (*(const char*)((const char*)ci + 0x3c5) <= 0)   // 推断：日删标记
            continue;
        enableSaveInven();
        inven->delete_item((INVEN_TYPE)1, slot, item.m_addInfo2,
                           (eItemDelReason)0x13, true);
        list.push_back(std::make_pair((int)ENUM_ITEMSPACE_0, slot));
    }
}

// 0x0867dba0：指定物品清理编排（deleteSpecificItem.md）：关私摊 → 五域清理 →
// 邮箱超时/匹配/无效物品移除（header 0x62 包）→ 无效物品邮件补偿 → redeem 清理。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0867dba0 | _ZN5CUser18deleteSpecificItemERKSt6vectorISt4pairIiiESaIS2_EERS4_
void CUser::deleteSpecificItem(const std::vector<std::pair<int, int> >& list,
                               std::vector<std::pair<int, int> >& out)
{
    if (getCurCharacR() == 0)
        return;

    private_store::CPrivateStoreMgr* mgr =
        private_store::GetInstancePrivateStoreMgr();
    private_store::CPrivateStore* store =
        (private_store::CPrivateStore*)mgr->FindStoreSeller(this);
    if (store != 0)
    {
        // 真成员调用（b22 消解旧 §4.5 例外）：ISeller 槽 +0x04
        // RemovePrivateStore(bool)，CPrivateStoreMgr.h:54/217 权威声明
        // （ORIG T 0x85c5fd0，vtable @0x8cbeb60 实证），编译器自动出
        // vtbl[1] 间接调用，与原手写槽取法同形。
        store->RemovePrivateStore(false);
    }

    _deleteSpecificItemInven((INVEN_TYPE)0, list, out, 10, 0x15);
    _deleteSpecificItemInven((INVEN_TYPE)1, list, out, 3, 0x137);
    _deleteSpecificItemCargo(list, out);
    _deleteSpecificItemAccountCargo(list, out);
    _deleteSpecificItemTitieBook(list);

    WongWork::CMailBox* mailBox =
        (WongWork::CMailBox*)m_premium.m_mailBox;   // ORIG this+0x71b98 实证
    if (mailBox != 0)
    {
        std::vector<std::pair<int, int> > mailDel;
        unsigned int rmList[21];
        int rmCount = 0;
        long nowSec = (long)GlobalData::s_systemTime_.getCurSec();
        mailBox->setMailIterator();
        for (WongWork::CMailBox::CMail* mail = mailBox->getNextMail();
             mail != 0; mail = mailBox->getNextMail())
        {
            // ORIG：超时(IsTimeOver)/已匹配/无效物品 三者均移除；无效物品
            // 记入 mailDel 供 RewardItem2DeleteInvalidItem 邮件补偿。
            bool remove = false;
            if (mail->IsTimeOver(nowSec))
            {
                remove = true;
            }
            else
            {
                const Inven_Item& pkg = mail->GetPackageItem();
                if (_isMatchedItem(pkg.m_addInfo, list) == -1)
                {
                    if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                            pkg.m_addInfo))
                    {
                        mailDel.push_back(
                            std::make_pair(pkg.m_addInfo, pkg.m_addInfo2));
                        remove = true;
                    }
                }
                else
                {
                    remove = true;
                }
            }
            if (remove && rmCount < 20)
            {
                rmList[++rmCount] = mail->GetLetterId();
                mailBox->RemoveMail(mail->GetLetterId());
            }
        }
        if (rmCount != 0)
        {
            rmList[0] = (unsigned int)rmCount;
            WongWork::CMailBoxHelper::ReqDBRemoveMail(this, rmList, false);
            PacketGuard packet;
            InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
            pkt->put_header(0, 0x62);
            pkt->put_int(rmCount);
            for (int i = 1; i <= rmCount; ++i)
                pkt->put_int((int)rmList[i]);
            pkt->finalize(true);
            Send(packet);
        }
        RewardItem2DeleteInvalidItem("Mail", mailDel);
    }
    _deleteSpecificRedeemItem(list);
}

// 0x08652c8e
// ORIG：联动角色断线处理。标志开启时：若数据加载/建角/删角被锁定则记错误日志；
// 否则把联动角色槽位移到列表尾（ChangeCharacSlot 两次）、清标志并刷新角色视图。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x08652c8e | _ZN5CUser22doLinkCharacDisconnectEv
void CUser::doLinkCharacDisconnect()
{
    // b28d-solo-CUser：callee 已 bool 化（CUser.h:2718 Main 代落）⇒ 正向包裹复现
    //   ORIG 0x08652c8e `call; test %al,%al; je <跳过 body>`（P292 表 if(f()) bool；
    //   负向 `==0) return;` 反产 `xor $1; test; jne`）。
    if (isLinkCharacDisconnectFlag())
    {
    // b28d-solo-CUser N封顶(P84/P214): ORIG 0x08652c9a 段为 `mov $1,%eax / jmp /
    //   mov $0,%eax / mov %al,-0xd / cmpb $0,-0xd`（4.1 对 || 汇聚物化+落槽），
    //   4.4 恒短路直跳免物化（单句/预清两句式实测：预清多 movb $0,-0xd）。
    bool locked = isLocked4DataLoad() != 0 || isLockedCreateCharac() != 0 ||
                  isLockedDeleteCharac() != 0;
    if (locked)
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__, 0x1cc1,
            "LINK_LOG : checkLinkCharacDisconnect ERROR!! m_id(%s)",
            NumberToString(get_acc_id(), 0));
        return;
    }
    // P48：ORIG 0x08652d68/0x08652d94 两 ChangeCharacSlot 各自重调 size()（共 2 次）。
    int slotIdx = (int)getDisconnectLinkCharacSlotIdx() - 1;
    ChangeCharacSlot((unsigned int)slotIdx, (unsigned int)((int)m_characList.size() - 1));
    ChangeCharacSlot((unsigned int)slotIdx, (unsigned int)((int)m_characList.size() - 1));
    setLinkCharacDisconnectFlag(false);
    UpdateCharacView();
    }
}

// 0x08651b7a
// ORIG：把 m_characList（vector<_Charac_info> @ +0x796e8）中下标 a 的元素移动到
// 下标 b 处（a<b 时先 insert(begin()+b+1, v[a]) 再 erase(begin()+a)；
// a>b 时 insert(begin()+b, v[a]) 再 erase(begin()+a+1)），随后
// enableSaveCharacView(&m_characterView)。越界（a/b >= size）直接返回。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08651b7a | _ZN5CUser16ChangeCharacSlotEjj
int CUser::ChangeCharacSlot(unsigned int a, unsigned int b)
{
    if (a < m_characList.size())
    {
        if (b < m_characList.size())
        {
            if (a < b)
            {
                m_characList.insert(m_characList.begin() + (b + 1), m_characList[a]);
                m_characList.erase(m_characList.begin() + a);
            }
            else
            {
                m_characList.insert(m_characList.begin() + b, m_characList[a]);
                m_characList.erase(m_characList.begin() + (a + 1));
            }
            m_characterView.enableSaveCharacView();
            return 0;
        }
    }
    return 0;
}

// ORIG 0x086973dc：设置联动角色断线标志（bool 字段 this-先直写 movb）。
void CUser::setLinkCharacDisconnectFlag(bool flag)
{
    m_breakAway.m_field15 = flag;
}

// ---- UpdateCharacView 链（报告 docs/class_func_reports/CUser/UpdateCharacView.md、
// updateCharacViewVisibleData.md、checkRentedItem.md；ORIG 0x8651740/0x8651322/0x8677188）----

// _Charac_info avatar 装备槽源视图（位于每角色 CInventory 子对象内部；
// updateCharacViewVisibleData 循环 1/2 按 info + i*0x3d + 0xf6 寻址）。
// pack(1)：ORIG 0x86513b6 `mov 0x2(%eax),%eax` / 0x86513c4 `mov 0x7(%eax),%ebx`
// 非对齐 int 读实证。
#pragma pack(push, 1)
struct AvatarSlotSrcView
{
    char m_pad0[2];
    int m_itemIndex;                    // +0x02（非 0 才处理）
    char m_pad6;
    int m_expansionKey;                 // +0x07（GetExpansionInfo 键）
};
#pragma pack(pop)

// 当前角色不匹配时的可见缓存源槽（info + k*8 + 0x75e，12 槽直拷）。
// pack(1)：ORIG 以 pEntry+0x2/+0x7 非对齐读 int（0x86513b6/0x86513c4）。
#pragma pack(push, 1)
struct VisibleCacheSlotSrcView
{
    int m_itemIndex;
    short m_fieldA;
    short m_fieldB;
};
#pragma pack(pop)

// SIG_UPDATE_CHARAC_VIEW 单记录视图（stride 0x94）：首槽 rec[0] 头 = acc_id，
// rec[i>0] 头 ← 前一角色 _Charac_info+0x12ad；数据体 CHARAC_LOGIN_DATA(0x90)。
// FindRentalInfoByItemIndex：ORIG 局部仿函数（W 0x8697d2c/0x8697d3a），按
// RentalInfo.m_itemIndex 匹配。
struct FindRentalInfoByItemIndex
{
    explicit FindRentalInfoByItemIndex(int itemIndex) : m_itemIndex(itemIndex) {}
    bool operator()(const InstanceRentalSystem::RentalInfo& info) const
    {
        return info.m_itemIndex == m_itemIndex;
    }

    int m_itemIndex;
};

struct SigUpdateCharacViewRecord
{
    int m_headField;                    // +0x00（rec[0]=acc_id；rec[i>0]←前一角色 expireSec）
    CHARAC_LOGIN_DATA m_data;           // +0x04（0x90）
};
typedef char sig_charac_view_rec_size_ok[(sizeof(SigUpdateCharacViewRecord) == 0x94) ? 1 : -1];

// ORIG T 0x08677188：在 m_rentalInfo[param] 视图中查找 characNo 的租借记录里
// 是否存在 itemIndex 匹配项。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677188 | _ZN5CUser15checkRentedItemEjii
bool CUser::checkRentedItem(unsigned int characNo, int itemIndex, int param)
{
    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> > rentalMap;
    GetInstanceRentalInfo(rentalMap, param);

    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> >::const_iterator it =
        rentalMap.find(characNo);
    if (it == rentalMap.end())
        return false;

    const std::vector<InstanceRentalSystem::RentalInfo>& infos = it->second;
    return std::find_if(infos.begin(), infos.end(),
                        FindRentalInfoByItemIndex(itemIndex)) != infos.end();
}

// updateCharacViewVisibleData 循环 1/2 源视图：info + i*0x3d + 0xe0 元素
// （ORIG 0x865139f/0x8651495 imul $0x3d + add $0xe0 实证），字段 +0x18/+0x1d。
#pragma pack(push, 1)
struct CharacAvatarEntry
{
    char m_pre[0x18];
    int m_itemIndex;                    // +0x18
    char m_pad1c;
    int m_expansionKey;                 // +0x1d（非对齐 int，ORIG 实证）
    char m_rest[0x3d - 0x21];
};
#pragma pack(pop)
typedef char avatar_entry_size_ok[(sizeof(CharacAvatarEntry) == 0x3d) ? 1 : -1];
// expansion 数据对（ORIG 0x8651453 `movzwl (%eax),%edx` / 0x8651467
// `movzwl 0x2(%eax),%edx` 成员 disp 形态）。
struct ExpansionPair
{
    unsigned short m_a;                 // +0x00
    unsigned short m_b;                 // +0x02
};
// 循环 3 可见缓存元素（ORIG 0x86515b5 `mov 0xe(%eax,%edx,8)` scale：
// base=info+0xe，索引 k+0xea，stride 8，字段 +0/+4/+6）。
struct VisibleCachePair8
{
    unsigned int m_itemIndex;           // 绝对 +0x75e
    unsigned short m_fieldA;            // 绝对 +0x762
    unsigned short m_fieldB;            // 绝对 +0x764
};
typedef char vis_pair8_size_ok[(sizeof(VisibleCachePair8) == 8) ? 1 : -1];

// ORIG T 0x08651322：填充单角色的可见数据区（avatar 外观槽 / 租赁槽 /
// 非当前角色缓存直拷 / 改名状态）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08651322 | _ZN5CUser27updateCharacViewVisibleDataERK12_Charac_infoR17CHARAC_LOGIN_DATA
void CUser::updateCharacViewVisibleData(const _Charac_info& info,
                                        struct CHARAC_LOGIN_DATA& data)
{
    memset(data.m_avatarSlots, 0, 0x60);
    if ((getCurCharacR() != 0 &&
         getCurCharacR()->m_characNo ==
             reinterpret_cast<const CharacListElemView&>(info).m_characNo)
            ? true : false)
    {
        data.m_renameOrViewState = getRequestRenameState();

        // 槽 0..8：avatar 装备槽（有物品且扩展信息存在时写索引/颜色对）。
        for (int i = 0; i < 9; ++i)
        {
            const AvatarSlotSrcView* slot =
                reinterpret_cast<const AvatarSlotSrcView*>(
                    i * 0x3d + 0xe0 + reinterpret_cast<const char*>(&info) +
                    0x16);
            if (slot->m_itemIndex != 0)
            {
                const void* expansion =
                    reinterpret_cast<CInventory*>(
                        reinterpret_cast<char*>(const_cast<_Charac_info*>(&info)) +
                        0xda)
                        ->GetAvatarItemMgrR()
                        ->GetExpansionInfo(slot->m_expansionKey);
                if (expansion != 0)
                {
                    if (reinterpret_cast<CInventory*>(
                            reinterpret_cast<char*>(
                                const_cast<_Charac_info*>(&info)) + 0xda)
                            ->GetClearAvatar(i) != 0)
                        data.m_avatarSlots[i].m_itemIndex =
                            reinterpret_cast<CInventory*>(
                                reinterpret_cast<char*>(
                                    const_cast<_Charac_info*>(&info)) + 0xda)
                                ->GetClearAvatar(i);
                    else
                        data.m_avatarSlots[i].m_itemIndex = slot->m_itemIndex;
                    data.m_avatarSlots[i].m_expansionA =
                        reinterpret_cast<const ExpansionPair*>(expansion)->m_a;
                    data.m_avatarSlots[i].m_expansionB =
                        reinterpret_cast<const ExpansionPair*>(expansion)->m_b;
                }
            }
        }

        // 槽 9..11：租赁位（任一租期类型命中则整槽清零，否则仅写索引）。
        for (int i = 9; i < 12; ++i)
        {
            if (checkRentedItem(
                    (unsigned int)reinterpret_cast<const CharacListElemView&>(info)
                        .m_characNo,
                    reinterpret_cast<const CharacAvatarEntry*>(
                        i * 0x3d + reinterpret_cast<const char*>(&info) +
                        0xe0)->m_itemIndex,
                    1) ||
                checkRentedItem(
                    (unsigned int)reinterpret_cast<const CharacListElemView&>(info)
                        .m_characNo,
                    reinterpret_cast<const CharacAvatarEntry*>(
                        i * 0x3d + reinterpret_cast<const char*>(&info) +
                        0xe0)->m_itemIndex,
                    2))
            {
                memset(&data.m_avatarSlots[i], 0, 8);
            }
            else
            {
                data.m_avatarSlots[i].m_itemIndex =
                    reinterpret_cast<const CharacAvatarEntry*>(
                        i * 0x3d + reinterpret_cast<const char*>(&info) +
                        0xe0)->m_itemIndex;
                data.m_avatarSlots[i].m_expansionA = 0;
                data.m_avatarSlots[i].m_expansionB = 0;
            }
        }
    }
    else
    {
        // 非当前角色：从可见缓存表（info + k*8 + 0x75e）12 槽直拷。
        for (int k = 0; k < 12; ++k)
        {
            data.m_avatarSlots[k].m_itemIndex =
                (reinterpret_cast<const VisibleCachePair8*>(
                     reinterpret_cast<const char*>(&info) + 0xe) + k + 0xea)
                    ->m_itemIndex;
            data.m_avatarSlots[k].m_expansionA =
                (reinterpret_cast<const VisibleCachePair8*>(
                     reinterpret_cast<const char*>(&info) + 0xe) + k + 0xea)
                    ->m_fieldA;
            data.m_avatarSlots[k].m_expansionB =
                (reinterpret_cast<const VisibleCachePair8*>(
                     reinterpret_cast<const char*>(&info) + 0xe) + k + 0xea)
                    ->m_fieldB;
        }
        data.m_renameOrViewState =
            reinterpret_cast<const CharacListElemView&>(info).m_viewState;
    }
}

// ORIG T 0x08651740：角色视图变化落库包。频道 !=7 且 isSaveCharacView() 时组
// SIG_UPDATE_CHARAC_VIEW（header 6 + UID），memset 全块后逐 _Charac_info 填
// 记录（stride 0x94）并调 updateCharacViewVisibleData，投递 DB_Q 后清保存标志。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08651740 | _ZN5CUser16UpdateCharacViewEv
void CUser::UpdateCharacView()
{
    if (G_GameWorld()->GetChannelType() == 7)
        return;
    if (!m_characterView.isSaveCharacView())
        return;

    // b28d-lane5：ORIG 0x08651740 无 Stream* 槽——Acquire 结果 %eax 直传 ctor（P0）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x1b1f), true);

    guard << (int)6;
    guard << (int)GetUID();

    SIG_UPDATE_CHARAC_VIEW* sig = guard.GetInBuffer<SIG_UPDATE_CHARAC_VIEW>();
    memset(sig, 0, sizeof(SIG_UPDATE_CHARAC_VIEW));

    sig->m_serverGroup = (unsigned char)GetServerGroup();

    SigUpdateCharacViewRecord* recs =
        reinterpret_cast<SigUpdateCharacViewRecord*>(sig);
    recs[0].m_headField = get_acc_id();   // ORIG `mov %eax,(sig)` 4B @+0x00

    // ORIG 无任何缓存：目标记录每次都按 sig + recIdx*0x94 + 偏移重算，源字段每次
    // 都重新 it.operator->()（Ghidra C 每个字段前都有一次 __normal_iterator::
    // operator->）；begin()/end() 各调一次转成 const_iterator 后再进循环。
    int recIdx = 0;
    std::vector<_Charac_info>::const_iterator it = m_characList.begin();
    std::vector<_Charac_info>::const_iterator endIt = m_characList.end();
    for (; it != endIt; ++it)
    {
        recs[recIdx].m_data.m_characNo = it->m_characNo;
        strncpy(recs[recIdx].m_data.m_name, it->m_name, 0x1e);
        recs[recIdx].m_data.m_job = (unsigned char)it->m_field26;
        recs[recIdx].m_data.m_field19 = (unsigned char)it->m_field29;
        recs[recIdx].m_data.m_level = it->m_level;

        updateCharacViewVisibleData(*it, recs[recIdx].m_data);

        recs[recIdx].m_data.m_lastPlayTime =
            (long)reinterpret_cast<const CharacListElemView&>(*it).m_createTime;
        recs[recIdx].m_data.m_field8c =
            reinterpret_cast<const CharacListElemView&>(*it).m_fielde5d;

        // bit0 ← 禁玩标记（info+0x1240 非零）。
        recs[recIdx].m_data.m_flags = (unsigned char)(
            ((unsigned int)recs[recIdx].m_data.m_flags & 0xfeu) |
            ((reinterpret_cast<const CharacListElemView&>(*it).m_field1240 != 0) ? 1u : 0u));

        recs[recIdx].m_data.m_field1d = it->m_field14b7;
        recs[recIdx].m_data.m_field1e = it->m_field14b8;

        // bit4 ← info+0xe40 的 bit1（ORIG shl $6 / sar $7 取位）。
        recs[recIdx].m_data.m_flags = (unsigned char)(
            ((unsigned int)recs[recIdx].m_data.m_flags & 0xefu) |
            (((((unsigned int)(unsigned char)
                    reinterpret_cast<const CharacListElemView&>(*it).m_fielde40
                << 6) >> 7) & 1u) << 4));

        recs[recIdx + 1].m_headField =
            reinterpret_cast<const CharacListElemView&>(*it).m_expireSec;

        ++recIdx;
    }

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    m_characterView.disableSaveCharacView();
}

// 0x086786be
// ORIG：放弃惩罚通知。非轻量服、+0x711d8 标志为 0（推断：惩罚冷却标志）、
// 未受影响 premium(0xc)、等级 > 0x11、moveSpace != 1 时，
// 发包(0x21)并 setCurCharacStamina(10)。
void CUser::giveup_panalty()
{
    // ORIG 0x86786cb `test %al,%al; jne` 无 setne 物化 ⇒ IsLightServer bool 返回（声明 DataManagerInitDepends.h:42/62 待 bool 化；(bool)cast 在 char 声明下反产 setne，故不加）
    if (IsLightServer())
        return;
    // ORIG 0x086786d6 `movzbl 0x711d8; test; jne`（bool 直读；= clientSpec.m_gmUserFlag）
    if (m_clientSpec.m_gmUserFlag)
        return;
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) != 0)
        return;
    if (get_charac_level() <= 0x11)
        return;
    if (isAffectedPremium((ENUM_PREMIUM_TYPE)0xc) != 0)
        return;
    // ORIG 0x8678742 cmp $0x1;setne;test;je merge：物化 true→fall body（probe20 b 形）
    if ((bool)(getMoveSpace() != 1))
    {
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x21);
    setCurCharacStamina((unsigned char)0xa);
    ((InterfacePacketBuf*)&packet)->put_byte(10);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
    }
}

// 0x08658910
// ORIG：登出清理——清登录时间/播放经验/多开彩票失败计数、移除 GM 标记、
// 复位 uid/槽位/自增 ID/账号名、断网并整体 reset。
void CUser::log_out()
{
    m_tradeCount = 0;                     // CUser+0x8d10c（ORIG movl $0）
    resetPlayExpAdd();
    set_multiboxLotteryItemFailCnt(0);
    // ORIG 0x8658948/0x8658955 两次 call get_acc_id：右实参先评估存 %ebx→0x8(%esp)，左→%eax→0x4(%esp)
    // （gcc 实参右起评估；0x8 槽值非 0 常量而是第二调用结果，ORIG cmp 语义等价——原码即双调用）
    GlobalData::s_pGMAccounts_->removeGM(get_acc_id(), get_acc_id());
    m_field796f8 = 0;
    m_chState = (ch_state)0;
    m_accId = 0;
    CUserGlobalInfoHandleInstance()->reset_uniqueid_flag(m_field704a8);
    m_field704a8 = 0;
    SetSlotIDX(0);
    SetIncreID(0);
    memset(m_accName, 0, 0x15);
    m_field8cfc8 = 0;
    m_field8cfdc = 0;               // 状态标志字节（ORIG movb $0,0x8cfdc）
    m_network.disconnect();
    reset();
}

// 0x086487ea
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086487ea | _ZN5CUser17prepareDisconnectEv
void CUser::prepareDisconnect()
{
    // 0x086487ea 主分支（prepareDisconnect.md 反编译 C 全文转写）：state<2 早退；
    // 非人证用户记 hack 0x3b9、SSO 强存计数 + DB_ForceSaveSimpleSSO；安全卡/密保
    // 请求态为 1 时 Dispatcher_SecurityCardAuthCancel::process(this)；antibotChecker
    // vtable +0x08 四参注销 / +0x24 两参注销；DBUpdateExitTime(accId, uid, ts)。
    if (get_state() < 2)
        return;
    if (!isHumanCertified())
    {
        setHumanCertified(true);
        getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x3b9, 1, 0, 0);
        m_humanCertifyErrorCnt += 1;   // ORIG this+0x8e0fc 实证：SSO 强制保存计数
        DB_ForceSaveSimpleSSO::makeRequest((unsigned int)get_acc_id(), m_humanCertifyErrorCnt);
    }
    bool cancelAuth = false;
    if (getSecurityCard()->getRequestState() == 1)
        cancelAuth = true;
    else if (getPad()->getRequestState() == 1)
        cancelAuth = true;
    if (cancelAuth)
        Dispatcher_SecurityCardAuthCancel::process(this);

    cMyTrace t("void CUser::prepareDisconnect()", 0x8ed, 0);
    t("[Taiwan, Antibot] Delete client. (sequence:%d, mid:%u)",
      G_CEnvironment()->getProcessSequence(), (unsigned int)get_acc_id());

    void* checker = GlobalData::s_antibotChecker_;
    void** vtbl = *(void***)checker;
    // §4.5 例外：antibot 类未建模（见 AntibotSend 处 b22 单），
    // Fn3=+0x08（DeleteClient 上报）/ Fn1=+0x24（DeleteClient 尾标）。
    typedef void (*Fn3)(void*, int, int, unsigned int);   // vptr+0x08
    typedef void (*Fn1)(void*, unsigned int);             // vptr+0x24
    ((Fn3)vtbl[2])(checker, G_CEnvironment()->getProcessSequence(), 0,
                   (unsigned int)get_acc_id());
    ((Fn1)vtbl[9])(checker, (unsigned int)get_acc_id());

    // ORIG 0x086487ea：直读 m_garenaAuthData(+0x6ef92) 调 getTimeStamp，不经 getGarenaAuthData
    unsigned int ts = m_garenaAuthData.getTimeStamp();
    DBUpdateExitTime::makeRequest(
        (unsigned int)get_acc_id(),
        (unsigned int)(unsigned short)get_unique_id(), ts);
}

// 0x0865db6c
// ORIG：按物品空间号生成并发送对应列表包：
//   0/1 → CInventory::MakeItemList(1/2)、2 → CCargo::MakeItemList、
//   7 → 生物列表 + MakeItemList(3)、0xc → CAccountCargo::SendItemList（不存在时返回 1）。
// b28d-lane5 N封顶: P0 清绑连带——同 TU callee（IsExistAccountCargo 等）清绑后帧变化，
// 4.4 按 P210 精确继承 outgoing ⇒ 本函数帧 0x20→0x30、PacketGuard 槽 -0x14→-0x1c；
// 另 v^=1 段清绑后落栈槽物化（P168a④）。| ORIG 0x0865db6c | _ZN5CUser14send_itemspaceEi
bool CUser::send_itemspace(int space)   // 返回形随 CUser.h:2843 bool 化（P22 调用点直测形）
{
    // P168a/X2 五段化（2026-09-05 CUserC8，Main 派单）：ORIG 0x86?? 每 MakeItemList
    // case `call; xor $0x1,%eax; test %al,%al; je` **带分支消费者**（判据③通过），
    // char 暂存槽 -0x9 与 `cmp $1/setne` 岛形均为我方旧形失分点（判据②岛→直分支）。
    // 裸 `!fn()` 对本批 MakeItemList(bool 返回) 实测产 xor 直落（判据①），case7 用
    // !=1 系 char 域实测等价保留。
    // X1 精确形（Main 广播定字 2026-09-05，int callee 调用点自救——MakeItemList/
    // SendCreatureItemList 均 int 返回，裸 `!fn()` 实测产 test eax;sete 三段，
    // uchar 绑 %eax + `^= 1` 后 `if (v)` 得 `xor $0x1,%eax; test %al,%al; je`
    // 与 ORIG 0x8675 段系同形；赋值零成本（call 结果已在 %eax），块作用域隔离）。
    PacketGuard packet;
    switch (space)
    {
    case 0:
    {
        unsigned char v0 =
            (unsigned char)getCurCharacInvenR()->MakeItemList((INVEN_TYPE)1, &packet);
        v0 ^= 1;
        if (v0) return 0;
        break;
    }
    case 1:
    {
        unsigned char v1 =
            (unsigned char)getCurCharacInvenR()->MakeItemList((INVEN_TYPE)2, &packet);
        v1 ^= 1;
        if (v1) return 0;
        break;
    }
    case 2:
    {
        unsigned char v2 =
            (unsigned char)getCurCharacCargoR()->MakeItemList(&packet);
        v2 ^= 1;
        if (v2) return 0;
        break;
    }
    case 7:
    {
        unsigned char v3 = (unsigned char)
            getCurCharacInvenR()->GetCreatureMgrR()->SendCreatureItemList();
        v3 ^= 1;
        if (v3) return 0;
        unsigned char v4 =
            (unsigned char)getCurCharacInvenR()->MakeItemList((INVEN_TYPE)3, &packet);
        v4 ^= 1;
        if (v4) return 0;
        break;
    }
    case 0xc:
        // ORIG 0x865dc90 段：`call IsExistAccountCargo; test %al,%al; je <ret1岛>`——
        // 真支 fall-through 直取成员对象地址（`mov 0x8(%ebp),%eax; add $0x6effc,%eax`）
        // 调 m_accountCargo.SendItemList()，⇒ 原码是成员对象直调而非 GetAccountCargo()
        // （此处 4.1 头文件内联的 getter 已在我们 TU 外联，多出一次 call）。
        if (IsExistAccountCargo())
            return m_accountCargo.SendItemList();
        return 1;
    default:
        return 0;
    }
    Send(packet);
    return 1;
}

// 0x0868c170（返回 1 已通知 / 0 未满足条件）
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868c170 | _ZN5CUser31send_ontime_reward_start_notifyEv
bool CUser::send_ontime_reward_start_notify()   // b28c-L5 SIGFIX（P174）
{
    // 0x0868c170 主分支（send_ontime_reward_start_notify.md）：事件 0x33 空 →
    // 日志+0；非活跃 / 奖励已开始（get_reward_state!=0）→ 0；日志块（文件名
    // ./log/<env>/OnTimeEvent + "On Charac Login"）后，ontime_id > 本地
    // last_recv_idx 且剩余时间 >=0 才发包 header(0,0x105) 单 int，返回 1。
    CEventBase* eventRaw = GlobalData::s_event_manager->GetRepeatEvent(0x33);
    if (eventRaw == 0)
    {
        cMyTrace trace("bool CUser::send_ontime_reward_start_notify()", 0xa7f7, 5);
        trace("[ON_TIME_EVENT] send_ontime_reward_start_notify() error");
        return 0;
    }
    if (!eventRaw->IsEventing((CUser*)0))
        return 0;
    COnTimeEvent* ev = (COnTimeEvent*)eventRaw;
    if (ev->get_reward_state() != 0)
        return 0;
    char fileName[0x40];
    memset(fileName, 0, sizeof(fileName));
    sprintf(fileName, "./log/%s/OnTimeEvent", G_CEnvironment()->get_file_name());
    CMyFileLog log("bool CUser::send_ontime_reward_start_notify()", 0xa803);
    log(fileName, "On Time Event : On Charac Login(%s, %u)",
        NumberToString(get_acc_id(), 0), get_charac_no(-1));
    if (ev->get_ontime_id() <= get_ontime_last_recv_idx())
        return 0;
    int remain = (int)ev->get_reward_end_time() - (int)time(0);
    if (remain < 0)
        return 0;
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x105);
    ((InterfacePacketBuf*)&packet)->put_int(remain);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
    return 1;
}

// 0x0866c46a:技能信息包(send_skill_info.md)。
// 用具名语义字段访问：玩家技能经 SkillSlot（m_skillSlot 树0 / m_skillSlot2 树1，
// _Mastered_skill{索引,等级}），公会技能经 STGuildDBInfoOnly（m_skillLearnCnt 计数、
// m_skills[16] 5 字节条目 STGuildSkill{技能ID,等级}）。背包命令段经
// findSkillCommandData(索引) 取 STSkillCommandData{count, m_key0..}。
// ORIG 语义要点：公会槽位号 = i + 0x8a；公会技能判定 = 双 grade 非零 且 技能ID > 0xc7。
void CUser::send_skill_info()
{
    // ORIG 0x0866c46a: send_skill_info.md
    PacketGuard packet;
    packet.put_header(0, 0x13);

    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0));
    int countIdx0 = packet.get_index();
    int totalCount0 = 0;
    packet.put_byte(0);

    for (int i = 0; i < 0xcc; ++i)
    {
        if (reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot[i].m_index != 0)
        {
            packet.put_byte(i);
            packet.put_byte(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot[i].m_index);
            packet.put_byte((signed char)reinterpret_cast<const SkillSlot*>(
                getCurCharacSkillR())->m_skillSlot[i].m_level);
            totalCount0++;
            STSkillCommandData cmd0;
            if (findSkillCommandData(reinterpret_cast<const SkillSlot*>(
                    getCurCharacSkillR())->m_skillSlot[i].m_index, cmd0))
            {
                packet.put_byte((signed char)cmd0.m_count);
                for (int k = 0; k < (signed char)cmd0.m_count; ++k)
                {
                    packet.put_byte((signed char)cmd0.m_keyBytes[k]);
                }
            }
            else
            {
                packet.put_byte(0);
            }
        }
    }

    STGuildDBInfoPacketView* guild =
        reinterpret_cast<STGuildDBInfoPacketView*>(GetGuildDBInfo());
    int guildCount = (unsigned char)guild->m_skillSection.m_skillLearnCnt;
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        guildCount = 0;
    }

    for (int i = 0; i < guildCount; ++i)
    {
        if (getGuildMemberGrade() && getGuildMemberGrade() &&
            guild->m_skillSection.m_skills[i].m_field0 > 0xc7)
        {
            packet.put_byte(i + 0x8a);
            packet.put_byte(guild->m_skillSection.m_skills[i].m_field0);
            packet.put_byte((signed char)guild->m_skillSection.m_skills[i].m_field4);
            totalCount0++;
            packet.put_byte(0);
        }
    }
    packet.put_byte(countIdx0, totalCount0);
    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1));
    countIdx0 = packet.get_index();
    totalCount0 = 0;
    packet.put_byte(0);

    for (int i = 0; i < 0xcc; ++i)
    {
        if (reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot2[i].m_index != 0)
        {
            packet.put_byte(i);
            packet.put_byte(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
                ->m_skillSlot2[i].m_index);
            packet.put_byte((signed char)reinterpret_cast<const SkillSlot*>(
                getCurCharacSkillR())->m_skillSlot2[i].m_level);
            totalCount0++;
            STSkillCommandData cmd1;
            if (findSkillCommandData(reinterpret_cast<const SkillSlot*>(
                    getCurCharacSkillR())->m_skillSlot2[i].m_index, cmd1))
            {
                packet.put_byte((signed char)cmd1.m_count);
                for (int k = 0; k < (signed char)cmd1.m_count; ++k)
                {
                    packet.put_byte((signed char)cmd1.m_keyBytes[k]);
                }
            }
            else
            {
                packet.put_byte(0);
            }
        }
    }

    for (int i = 0; i < guildCount; ++i)
    {
        if (getGuildMemberGrade() && getGuildMemberGrade() &&
            guild->m_skillSection.m_skills[i].m_field0 > 0xc7)
        {
            packet.put_byte(i + 0x8a);
            packet.put_byte(guild->m_skillSection.m_skills[i].m_field0);
            packet.put_byte((signed char)guild->m_skillSection.m_skills[i].m_field4);
            totalCount0++;
            packet.put_byte(0);
        }
    }
    packet.put_byte(countIdx0, totalCount0);

    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2));
    packet.put_short(reinterpret_cast<const SkillSlot*>(getCurCharacSkillR())
        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3));
    packet.finalize(true);
    Send(packet);
    sendComboSkillList();
}

// 0x0866cb04

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0866cb04 | _ZN5CUser17CheckQuestMonsterEii21ENUM_QUEST_ENEMY_TYPE
bool CUser::CheckQuestMonster(int a, int b, ENUM_QUEST_ENEMY_TYPE type)
{
    // 0x0866cb04 主分支（CheckQuestMonster.md）：遍历进行中任务表
    // UserQuest::m_quest[i]（<=0 跳过；0x13 上限），Quest::m_type==3/4 或
    // UserQuest::m_trigger[i] 触发位；CheckKillMonster(a,
    // m_authenData.m_characClass@+0x75da, b, type) 掷掉落：rate = 炼金/背包
    // GetQuestItemDropBouns/队伍倍率/premium 加成；掷 tryCount 次 rand(100)；
    // 入包 0xe（新槽 insert / 已有槽累加封顶；needCount<=已有则删掉落项重掷）。
    extern Inven_Item g_emptySlot;   // ORIG 全局（0x943ddc0）
    // ORIG 以 getCurCharacQuestRefR()（= &CUser::m_quest，@+0x858d4）为基址：
    // +0x7538 表 = m_quest[i]、+0x7588 = m_trigger[i]、+0x75da = m_characClass。
    const UserQuest* questRef = getCurCharacQuestRefR();
    for (int i = 0; i <= 0x13; ++i)
    {
        int questIdx = questRef->m_quest[i];
        if (questIdx <= 0)
            continue;
        Quest* quest = G_CDataManager()->find_quest(questIdx);
        if (quest == 0)
        {
            LogManager::logFormat(1, "user.cpp",
                "bool CUser::CheckQuestMonster(int, int, ENUM_QUEST_ENEMY_TYPE)",
                0x54f7, "G_CDataManager()->find_quest(%d) fail", questIdx);
            continue;
        }
        if (quest->m_type != 4 && quest->m_type != 3 &&
            questRef->m_trigger[i] == 0)
            continue;

        std::vector<MonsterKillItem> kills;
        if (quest->CheckKillMonster(a, questRef->m_authenData.m_characClass, b,
                                    type, kills) != 1)
            continue;

        bool again = true;
        while (again)
        {
            again = false;
            if (kills.empty())
                break;
            int pick = get_rand_int((int)kills.size());
            if (pick < 0 || pick >= (int)kills.size())
                break;
            const MonsterKillItem& drop = kills[pick];
            const int itemIdx = drop.m_field10;    // 掉落道具 idx
            const int tryCount = drop.m_field14;   // 掷骰次数
            const int dropRate = drop.m_field18;   // 基础掉率
            const int needCount = drop.m_field1c;  // 需要数量（-1=无限）

            int bonus = 0;
            for (int s = 0; s < 0x1a; ++s)
            {
                Inven_Item* it =
                    const_cast<CInventory*>(getCurCharacInvenR())->GetInvenRef((INVEN_TYPE)0, s);
                if (it != 0 && it->m_addInfo != 0)
                    bonus += dropRate *
                             G_CDataManager()->getItemList()
                                 ->GetQuestItemDropBouns(
                                     reinterpret_cast<unsigned long&>(
                                         it->m_addInfo)) /
                             100;
            }
            int rate = expert_job::CAlchemist::GetQuestItemDropRate(
                           this, dropRate) +
                       bonus;
            if (G_CEnvironment()->m_serverEnvir.m_serverType == 2)
            {
                CParty* party = (CParty*)GetParty();
                if (party != 0)
                {
                    int count = party->get_member_count();
                    float mult;
                    if (count < 1)
                        mult = 1.0f;
                    else
                        mult = *(float*)((char*)G_CDataManager() + 0x10 +
                                         (count + 0xf7) * 4);
                    rate = (int)((float)rate * mult);
                }
            }
            if (isAffectedPremium((ENUM_PREMIUM_TYPE)0x4f))
            {
                unsigned int p =
                    getAddQuestItemDropRateFromPremium(
                        (ENUM_PREMIUM_TYPE)0x4f);
                rate = dropRate * (int)(p / 100) + rate;
            }

            int gained = 0;
            for (int t = 0; t < tryCount; ++t)
                if (get_rand_int(100) < rate)
                    ++gained;
            if (gained == 0)
                break;

            Inven_Item item;
            CItem* ci = G_CDataManager()->find_item(itemIdx);
            ci->make_item(item);
            item.set_add_info(gained);
            if (gained > 999)
            {
                cMyTrace tr("bool CUser::CheckQuestMonster(int, int, "
                            "ENUM_QUEST_ENEMY_TYPE)",
                            0x5579, 5);
                tr("CUser::CheckQuestMonster, mon_item.item.set_add_info , "
                   "ITEM #%d, %d , QUEST#%d",
                   itemIdx, gained, questIdx);
                item.set_add_info(1000);
            }

            PacketGuard packet;
            InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
            pkt->put_header(0, 0xe);
            pkt->put_byte(0);
            pkt->put_short(1);

            Inven_Item existItem;
            int slot = getCurCharacInvenR()->GetInvenData(itemIdx, existItem);
            if (slot < 0)
            {
                int newSlot = getCurCharacInvenW()->insertItemIntoInventory(
                    item, (eItemAddReason)8, true, true);
                if (newSlot > 0)
                {
                    pkt->put_short(newSlot);
                    pkt->put_int(itemIdx);
                    pkt->put_int(item.get_add_info());
                    pkt->put_byte(item.GetItemAttr());
                    pkt->put_short(item.m_fieldb);
                    pkt->put_byte(item.m_field0);
                    pkt->put_int(item.m_fieldd);
                    pkt->put_byte(item.m_amp.getAbilityType());
                    pkt->put_short(item.m_amp.getAbilityValue());
                    if (item.isAvatarItemType())
                    {
                        pkt->put_int(0x1e);
                        const WongWork::CAvatarItemMgr* mgr =
                            getCurCharacInvenR()->GetAvatarItemMgrR();
                        const char* socketData =
                            mgr->getJewelSocketData(item.get_add_info());
                        pkt->put_binary(socketData, 0x1e);
                        char* expansion = (char*)mgr->GetExpansionInfo(
                            item.get_add_info());
                        pkt->put_int(4);
                        if (expansion == 0)
                        {
                            stAvatarExpansionInfo_t init4;
                            init4.init();
                            pkt->put_binary((char*)&init4, 4);
                        }
                        else
                        {
                            pkt->put_binary(expansion, 4);
                        }
                    }
                    pkt->put_byte(0);
                    pkt->put_packet(g_emptySlot);
                    pkt->finalize(true);
                    Send(packet);
                }
            }
            else
            {
                if (needCount == -1)
                {
                    existItem.set_add_info(existItem.get_add_info() +
                                           item.get_add_info());
                }
                else if (needCount <= existItem.get_add_info())
                {
                    for (std::vector<MonsterKillItem>::iterator it =
                             kills.begin();
                         it != kills.end(); ++it)
                    {
                        if (memcmp(&drop, &*it, 0x20) == 0)
                        {
                            kills.erase(it);
                            break;
                        }
                    }
                    again = true;
                }
                else
                {
                    int cur = existItem.get_add_info();
                    if (cur + item.get_add_info() < needCount)
                        existItem.set_add_info(cur + item.get_add_info());
                    else
                        existItem.set_add_info(needCount);
                }
                getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot,
                                                  existItem);
                pkt->put_short(slot);
                pkt->put_int(itemIdx);
                pkt->put_int(existItem.get_add_info());
                pkt->put_byte(0);
                pkt->put_short(0);
                pkt->put_byte(0);
                pkt->put_int(0);
                pkt->put_byte(existItem.m_amp.getAbilityType());
                pkt->put_short(existItem.m_amp.getAbilityValue());
                if (existItem.isAvatarItemType())
                {
                    pkt->put_int(0x1e);
                    const WongWork::CAvatarItemMgr* mgr =
                        getCurCharacInvenR()->GetAvatarItemMgrR();
                    const char* socketData =
                        mgr->getJewelSocketData(existItem.get_add_info());
                    pkt->put_binary(socketData, 0x1e);
                    char* expansion = (char*)mgr->GetExpansionInfo(
                        existItem.get_add_info());
                    pkt->put_int(4);
                    if (expansion == 0)
                    {
                        stAvatarExpansionInfo_t init4;
                        init4.init();
                        pkt->put_binary((char*)&init4, 4);
                    }
                    else
                    {
                        pkt->put_binary(expansion, 4);
                    }
                }
                pkt->put_byte(0);
                pkt->put_packet(existItem);
                pkt->finalize(true);
                Send(packet);
            }
        }
    }
}
// §6.2 巨件核销：P167 100/100 [0缺0多]；gap 616→634（our+18）；残差归因 N82
// 槽号错位 + esm 访问器重物化族（P112），语义完整仅剩 codegen 差，N 封顶
// （详见 docs/asm_translation_patterns.md 负例表）。
    // 0x08655c60 主分支（FatigueUp.md）：v==0→1；BreakAway 幸运点先结算；
// b28d-lane5 N封顶: N18a/P200 拓宽中轉; P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x08655c60 | _ZN5CUser9FatigueUpEi
void CUser::FatigueUp(int v)
{
    // Hades+GoldControl 双扣减；state==5 时混沌点扣减与累计(+0x8e064)；
    // +0x8e08c=v；prev 疲劳逢 50 上报 hack 0x263；0x9c/0x32/0x64 清动作组
    // 0x16/0x28/0x2a；事件脚本 inc/gift/bring_up(0x19b)/process_add_fatigue；
    // 学园点；英雄任务 MissionNo=2；repeat 事件 0x29(角色日)/0x2b(疲劳buff)；
    // AvatarRechargeServer 计耐久；+0x79642 累计；公会 temp exp；上限/燃烧/
    // premium 分支（详见 docs/class_func_reports/CUser/FatigueUp.md）。
    if (v == 0)
        v = 1;
    unsigned short prevFatigue =
        (unsigned short)getCurCharacUsedFatigue();

    // ORIG 0x08655c88：getUserBreakAwayRewardLuckPoint 仅调一次，结果缓存到槽
    // 供 IncCurCharacLuckPoint 复用（asm +0x033 mov %eax,-0xc(%ebp) + +0x03f 重载）。
    // 逗号操作符短路形匹配 ORIG `iVar8=getUserBreakAwayRewardLuckPoint(),0<iVar8`。
    int luckPoint;
    if (getCurCharacR() != 0 &&
        (luckPoint = getUserBreakAwayRewardLuckPoint(), luckPoint > 0))
    {
        IncCurCharacLuckPoint(luckPoint);
        setUserBreakAwayRewardLuckPoint(0);
        setUsedBreakAwayLuckPoint(true);
    }

    // XNuclear::CHades::UseFatigue（ORIG T 0x084b9552；权威定义见 CHades.cpp）
    ((XNuclear::CHades*)getHades())->UseFatigue((unsigned int)v);
    GetGoldControl()->UseFatigue(v);

    if (get_state() == 5 && getCurCharacR() != 0)
    {
        IncCurCharacAssaultFatigueCount();
        if (GetCurCharacAssaultFatigueCount() > 1)
        {
            SubCurCharacChaosPoint(1);
            SetCurCharacAssaultFatigueCount(0);
            *(int*)((char*)this + 0x8e064) += v;   // ORIG 混沌疲劳累计
        }
    }
    m_fatigueGain = v;                             // +0x8e08c
    incCurCharacUsedFatigue((unsigned short)v);
    unsigned short nowFatigue = (unsigned short)getCurCharacUsedFatigue();
    // ORIG：imul $0x147b（除以 25 魔术数）后乘 -0x2a（42）——ORIG 实际为
    // now - 42*floor(now/50) == 0（仅 now==0 触发）；NEW 曾误写 -0x32(50)。
    if ((short)((short)nowFatigue + (short)((nowFatigue >> 1 & 0x7fff) / 0x19) * -0x2a) == 0)
        getHackAnalyzer()->addServerHackCnt(this, (WongWork::ENUM_HACKTYPE)0x263, 1, 0, 0);

    if ((short)getCurCharacUsedFatigue() == 0x9c)
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x16, 0, (ENUM_PACKETCLASS)0);
    if ((short)getCurCharacUsedFatigue() == 0x32)
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x28, 0, (ENUM_PACKETCLASS)0);
    else if ((short)getCurCharacUsedFatigue() == 100)
        APSystem::CUserProc::ClearActionAndSendtoUser(
            this, (APSystem::_ActionGroupIndex)0x2a, 0, (ENUM_PACKETCLASS)0);

    // ---- 事件脚本（EventClassify::CEventScriptMng）----
    EventClassify::CEventScriptMng* esm = static_cast<
        EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    esm->inc_user_fatigue(this, (unsigned int)v);
    esm = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    esm->send_gift_fatigue_item(this, (unsigned int)v);
    esm = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    if (esm->is_eventing(0x19b))
    {
        esm = static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng());
        esm->send_bring_up_item_fatigue(this, v);
    }

    if (isJoinedSchoolPointEvent())
        calcCurCharacSchoolPoint(prevFatigue);

    ARAD::Singleton<HeroMissionEvent>::Get()->processMission(
        this, (HeroMissionCondition::MissionNo::T)2, (unsigned int)v);

    // ---- repeat 事件 0x29：角色日（job 匹配）----
    CEventBase* dayEvent =
        GlobalData::s_event_manager->GetRepeatEvent(0x29);
    if (dayEvent != 0 && dayEvent->IsEventing((CUser*)0))
    {
        if (get_charac_job() ==
            static_cast<CCharacterDayEvent*>(dayEvent)->GetEventJob())
        {
            _processCharacterDayEvent(prevFatigue, (unsigned int)v);
        }
    }
    // ---- repeat 事件 0x2b：疲劳 buff ----
    CEventBase* buffEvent =
        GlobalData::s_event_manager->GetRepeatEvent(0x2b);
    if (buffEvent != 0 && buffEvent->IsEventing((CUser*)0))
    {
        _processFatigueBuffEvent(prevFatigue, (unsigned int)v);
    }

    ARAD::Singleton<AvatarRechargeServer>::Get()->CalcDurability(this, v);
    m_postChecksumData.m_totalFatigueUsed =
        (short)(m_postChecksumData.m_totalFatigueUsed + (short)v);   // +0x79642
    if (get_charac_level() != 0)
    {
        int weight =
            G_CDataManager()->m_guildParameter.getGuildExpWeight(get_charac_level());
        add_guild_temp_exp((weight * v) / 100);
    }
    decCurCharacFatigueGrownUpBuff((unsigned int)v);
    if (static_cast<unsigned char>(m_guildDB.m_fundLevel) + 1 > 0x10)  // +0x8d03b = m_guildDB.m_fundLevel(+0x1b)
        reset_guild_temp_exp();
    processFatigueCharac(v);

    // ---- premium 无限疲劳：直接返回总疲劳 ----
    if (m_premium.isAffectedUnlimitFatigue())
    {
        getCurCharacTotalFatigue();
        return;
    }
    // ---- repeat 事件 1：幸运点 * v ----
    CEventBase* ev1 = GlobalData::s_event_manager->GetRepeatEvent(1);
    // ORIG 0x0865619f：GetRepeatEvent(1) 后直接 IsEventing，无判空。
    if (ev1->IsEventing((CUser*)0))
    {
        IncCurCharacLuckPoint(
            // ORIG 0x086561bc `mov 0xe4(%eax)`（4B int；SPS +0x7c=m_7c）
            G_CDataManager()->m_serverParameter.m_7c * v);
        getCurCharacTotalFatigue();
        return;
    }
    // ---- repeat 事件 10：非活跃时走常规上限/燃烧/premium 结算 ----
    CEventBase* ev10 = GlobalData::s_event_manager->GetRepeatEvent(10);
    // ORIG 0x086561ff：GetRepeatEvent(10) 后直接 IsEventing，无判空。
    if (!ev10->IsEventing(this))
    {
        if ((short)getCurCharacFatigue() == -1)
        {
            getCurCharacFatigue();
            return;
        }
        if (get_charac_memberkey() != 0 && is_connect_upper_member())
            add_member_pay_tex_fatigue_to_upper(v);
        static_cast<EventClassify::CEventScriptMng*>(
            G_CDataManager()->get_event_script_mng())
            ->process_add_fatigue_count(this, v);

        unsigned short refill = 0;
        CEventBase* burn =
            GlobalData::s_event_manager->GetRepeatEvent(0xf);
        // ORIG 0x086562d7：GetRepeatEvent(0xf) 后直接 IsEventing，无判空。
        if (burn->IsEventing((CUser*)0))
        {
            refill = static_cast<CBurningFatigueEvent*>(burn)->getRefillFatigue(
                getCurCharacUsedFatigue(), (unsigned short)v);
        }
        if (IsHavePremiumAdvantage())
        {
            unsigned short prem = getCurCharacPremiumFatigue();
            unsigned int maxPrem = getCurCharacMaxPremiumFatigue() & 0xffff;
            if ((int)((unsigned int)prem + v) <= (int)maxPrem)
            {
                setCurCharacPremiumFatigue((unsigned short)(prem + v));
                if (refill != 0)
                {
                    gainCurCharacFatigue((unsigned int)refill);
                    SendFatigue();
                }
                IncCurCharacLuckPoint(
                    // ORIG 0x086563ae `mov 0xe4(%eax)`（4B int；SPS +0x7c=m_7c）
                    G_CDataManager()->m_serverParameter.m_7c * v);
                getCurCharacTotalFatigue();
                return;
            }
            if (getCurCharacPremiumFatigue() <
                (unsigned short)getCurCharacMaxPremiumFatigue())
            {
                setCurCharacPremiumFatigue(
                    (unsigned short)getCurCharacMaxPremiumFatigue());
                v = ((unsigned int)prem + v) -
                    (getCurCharacMaxPremiumFatigue() & 0xffff);
            }
        }
        // ORIG 0x086565b0（P48 no-CSE）：getCurCharacMaxFatigue 在条件与赋值处
        // 各自重取，不缓存 maxFatigue 局部。
        unsigned int curFatigue = (unsigned int)getCurCharacFatigue() & 0xffff;
        int newFatigue;
        if ((int)(getCurCharacMaxFatigue() & 0xffff) < (int)(curFatigue + v))
            newFatigue = (int)(getCurCharacMaxFatigue() & 0xffff);
        else
            newFatigue = (int)(curFatigue + v);
        setCurCharacFatigue(newFatigue);
        IncCurCharacLuckPoint(
            // ORIG 0x086564a2 `mov 0xe0(%eax)`（4B int；SPS +0x78=m_78）
            (newFatigue - (int)curFatigue) * G_CDataManager()->m_serverParameter.m_78);
        if (refill != 0)
        {
            gainCurCharacFatigue((unsigned int)refill);
            SendFatigue();
        }
        getCurCharacFatigue();
        return;
    }
    getCurCharacTotalFatigue();
}

// 0x0867ba5c（SendNotiPacket.md）：header(0,cmd)；cmd==2 → byte+short(1)+
// make_basic_info；cmd==0x25 → level/exp/组队/会员 exp/SP/SFP/胜点(+0x58)/
// 疲劳/seria bonus/周末(事件 0x57 活跃才发并 reset)/premium bonus(reset)。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867ba5c | _ZN5CUser14SendNotiPacketENS_11eSendTargetE15ENUM_NOTIPACKETi
bool CUser::SendNotiPacket(eSendTarget target, ENUM_NOTIPACKET cmd, int param)
{
    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(0, cmd);
    if (cmd == (ENUM_NOTIPACKET)2)
    {
        pkt->put_byte(param);
        pkt->put_short(1);
        make_basic_info((char*)&packet, (char)param);
    }
    else if (cmd == (ENUM_NOTIPACKET)0x25)
    {
        pkt->put_byte(get_charac_level());
        pkt->put_int(get_charac_exp());
        pkt->put_int(get_charac_party_bonus_exp());
        pkt->put_int(get_charac_member_bonus_exp());
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)0));
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(
            (ENUM_SKILL_TREE_KIND)1));
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)2));
        pkt->put_short(((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(
            (ENUM_SKILL_TREE_KIND)3));
        pkt->put_int(getPVPResultR()->m_winPoint);
        pkt->put_int(get_charac_fatigue_buf_bonus_exp());
        pkt->put_int(get_charac_seria_buf_bonus_exp());
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x57);
        if (ev != 0 && ev->IsEventing((CUser*)0))
        {
            pkt->put_int(getWeekendBonusExp());
            resetWeekendBonusExp();
        }
        else
        {
            pkt->put_int(0);
        }
        pkt->put_int(getPremiumBonusExp());
        resetPremiumBonusExp();
    }
    else
    {
        LogManager::logFormat(1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x76d1, "User %s - NotifyID = %d", getCurCharacName(), cmd);
        return false;   // b28c-L5 SIGFIX：ORIG 串 bool；失败臂 `mov $0x0,%ebx` 汇合尾
    }
    pkt->finalize(true);
    SendPacket(target, packet);
    return true;        // ORIG 0x0867bd4e `call SendPacket; mov $0x1,%ebx; …mov %ebx,%eax`
}

// 0x0865c936
// ORIG：公会战积分累加。PvP 房间内且非观察者、公会战事件(0x21)进行中且
// 有公会时：通知公会服 + 发包(0x6e) + 累加 +0x8cfe0（m_guildWarPoint）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865c936 | _ZN5CUser20add_guild_pvp_resultEi
void CUser::add_guild_pvp_result(int v)
{
    PvP_Room* pRoom = (PvP_Room*)GetPVPRoom();
    if (pRoom == 0)
        return;
    int seat = pRoom->get_user_seat(this);
    if (pRoom->IsPvpObserver(seat) != 0)
        return;
    CEventBase* pEvent = GlobalData::s_event_manager->GetRepeatEvent(0x21);
    if (pEvent != 0 && pEvent->IsEventing((CUser*)0) != 0 &&
        get_charac_guildkey() != 0)
    {
        Packet_Notice_Guild_War_Point_Change notice;
        // 推断：+0 处 guildkey、+4 处点数（ORIG 手动填充后 SendPacket(0xf)）
        *(unsigned int*)((char*)&notice + 0) = (unsigned int)get_charac_guildkey();
        *(char*)((char*)&notice + 4) = (char)v;
        CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
        if (proxy != 0)
            proxy->SendPacket((char*)&notice, 0xf);
        send_pvp_record();
        PacketGuard packet;
        ((InterfacePacketBuf*)&packet)->put_header(0, 0x6e);
        ((InterfacePacketBuf*)&packet)->put_byte(v);
        ((InterfacePacketBuf*)&packet)->finalize(true);
        Send(packet);
        m_guildWarPoint = m_guildWarPoint + v;
    }
}

// 0x0865d986
// ORIG：PVP 每日/每周游玩次数累计。PvpResultType 布局（CUserCharacInfo.h）：
//   +0x60 当日游玩次数（隔天重置为 0）、+0x64 累计游玩次数、
//   +0x68 当日基准时间戳（0 点 + 0x15180）。字段未具名，按 ORIG 偏移访问（推断）。
void CUser::add_pvp_play_info(unsigned int a, unsigned int b)
{
    time_t now = GlobalData::s_systemTime_.getCurSec();
    // PvpResultType：+0x60 m_todayLoseCount / +0x64 m_todayPlayCount /
    // +0x68 m_nextResetTime（ORIG add_pvp_play_info 0x0865d9a7/0x0865da2b/
    // 0x0865da3f 均 movl 4B 实证）。
    if (getPVPResultRefR()->m_nextResetTime + 0x15180 < (int)now)
    {
        getPVPResultRefW()->m_todayLoseCount = 0;
        // ORIG 0x0865d??：&tmv 存指针槽 -0xc（`lea -0x3c,%eax; mov %eax,-0xc`），
        // 三清零与 mktime 均 `mov -0xc,%eax` reload 解引用——4.4 值形会直写折掉
        // 指针槽（技巧 36 反向：ORIG 保留中转），显式指针对象还原。
        tm tmv;
        // ORIG 0x0865da??：指针 = **localtime_r 返回值直用**（call 后 %eax 即 &tmv，
        // `mov %eax,-0xc` 存槽，无二次 lea）。
        tm* pTmv = localtime_r(&now, &tmv);
        (*pTmv).tm_hour = 0;
        (*pTmv).tm_min = 0;
        (*pTmv).tm_sec = 0;
        getPVPResultRefW()->m_nextResetTime = mktime(pTmv) + 0x15180;
    }
    getPVPResultRefW()->m_todayLoseCount += a;
    getPVPResultRefW()->m_todayPlayCount += b;
}

// 0x0865c678
// ORIG：PvP 结果结算。观战者直接返回；winPointBuffer(+0x796fc) 清零并入账；
// 胜/败计数（整合 PvP 基础频道且渠道类型==0 时不计）；额外胜点补发并记 WPAdd 日志；
// 战斗模式 4/5 且 out 非空时按 mob_reward×胜点增量 + gainPowerWarRewardExp 计算奖励。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865c678 | _ZN5CUser14add_pvp_resultEbPj
void CUser::add_pvp_result(bool flag, unsigned int* out)
{
    PvP_Room* room = (PvP_Room*)GetPVPRoom();
    if (room == 0)
        return;
    int seat = room->get_user_seat(this);
    if (room->IsPvpObserver(seat))
        return;
    lock();
    const PvpResultType* before = getPVPResultR();
    int beforeWinPoint = before->m_winPoint;
    int delta = m_winPointBuffer;
    m_winPointBuffer = 0;
    if (!flag)
    {
        // ORIG：整合 PvP 基础频道且渠道类型 0 → 不计败场。
        if (!(G_GameWorld()->IsIntegratedPvPBaseChannel() &&
              G_CDataManager()->GetPvPChannelType() == 0))
        {
            getPVPResultW()->m_totalLoseCount += 1;
        }
    }
    else
    {
        delta = delta + 1;
        if (!(G_GameWorld()->IsIntegratedPvPBaseChannel() &&
              G_CDataManager()->GetPvPChannelType() == 0))
        {
            getPVPResultW()->m_totalWinCount += 1;
        }
    }
    getPVPResultW()->m_winPoint += delta;
    m_historyLog.WPAdd(getPVPResultR()->m_winPoint, delta, eWPAddReason_0);
    int extra =
        room->get_extra_win_point(getPVPResultW()->m_winPoint - beforeWinPoint);
    if (extra != 0)
    {
        getPVPResultW()->m_winPoint += extra;
        m_historyLog.WPAdd(getPVPResultR()->m_winPoint, extra, eWPAddReason_0);
    }
    unlock();
    int battleMode = room->get_pvp_battle_mode();
    bool rewardMode = (battleMode == 4) || (room->get_pvp_battle_mode() == 5);
    if (rewardMode && out != 0)
    {
        unsigned int mobReward = 0;
        int level = get_charac_level();
        G_CDataManager()->get_mob_reward(level, mobReward);
        *out = (unsigned int)((getPVPResultR()->m_winPoint - beforeWinPoint) *
                              (int)mobReward);
        *out = *out + (unsigned int)gainPowerWarRewardExp((int)*out);
    }
    send_pvp_record();
    SetReliablePerson(false);
    SetOverTradeGoldCount(0);
}

// 0x0864fac4
// ORIG：公会技能已学查询。GetGuildDBInfo 内 +0x44 成员数、+0x45 起
// 每 5 字节 {skillIdx(int), level(byte)}，返回匹配项的 level 字节。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fac4 | _ZN5CUser17IsGuildSkillLearnEi
bool CUser::IsGuildSkillLearn(int skillIdx)
{
    STGuildDBInfoOnly* gdb = (STGuildDBInfoOnly*)GetGuildDBInfo();
    if (gdb == 0)
        return false;
    unsigned char count = gdb->m_skillLearnCnt;   // ORIG movzbl 0x44
    const STGuildSkill* skills = (const STGuildSkill*)gdb->m_skills;
    for (int i = 0; i < (int)count; ++i)
    {
        // ORIG 0x0864fb01 `mov 0x5(...)`（idx int）/ 0x0864fb1c `movzbl`
        // （level 零扩展）；ORIG 直返 level 字节（非布尔规范化——见回报，
        // CUser.h 返回类型待属主核对 P9）。
        if (skills[i].m_field0 == skillIdx)
            return (unsigned char)skills[i].m_field4;
    }
    return false;
}

// 0x0864fb3a
// ORIG：公会技能经验换算。无公会/非公会成员/未学技能 0xc9 → 0；
// 否则按职业 find_skill 返回的 vector<vector<int>>（+0x108）取
// 公会技能经验：ORIG 0x0864fb3a（sz 0x100，报告 gainGuildSkillExp.md）。
// 忠实转写：find_skill(job, 0xc9)；索引 = IsGuildSkillLearn(0xc9)-1（前检查
// 已保证其为 1 → 恒 0）；取权重表第 0 行的 at(gradeIdx)；无 find_skill/边界/
// empty 防御检查（ORIG 直接 operator[]+at，越界走 at 异常路径）。
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0864fb3a | _ZN5CUser17gainGuildSkillExpEi
int CUser::gainGuildSkillExp(int exp)
{
    if (get_charac_guildkey() == 0)
        return 0;
    if (getGuildMemberGrade() == 0)
        return 0;
    // ORIG 0x864fb7d-0x864fc2e 系：learned@-0x11 **dead init 0 + 赋值两段**（技巧 8）
    // → cmpb $0,-0x11; sete %al 物化 guard（技巧 22）；skill@-0x10 dead init；
    // job/gradeIdx 共用 %ebx 跨 call（块作用域分离）；gradeWeights 无具名槽
    // （skill+0x108 直发 op[]，T-CU15）；at() 结果 `mov (%eax),%eax` 落 weight 槽
    // -0xc 再 imul（具名物化补齐槽后 learned/skill 平移归位）。
    char learned = 0;
    learned = (char)IsGuildSkillLearn(0xc9);
    // P0/N96：去绑定。ORIG `cmpb $0,-0x11; sete %al; test; jne` 物化岛为 4.1
    // 槽比零产物（P197④：4.4 对槽比恒出 cmp/jne 直跳，源形不可复现）。
    // P225 修正 P197④ 边界：while(learned==0){return 0;} 复现
    // `cmpb $0; sete %al; test; je BODY` 物化形（probe p7.t5 逐条实证）。
    while (learned == 0)
    {
        return 0;
    }
    CSkill* skill = 0;
    skill = G_CDataManager()->find_skill(get_charac_job(), 0xc9);
    // ORIG 0x0864fbdc-0x86fbfd：gradeIdx（=learned-1）跨 operator[] call 存活于
    // %ebx（callee-saved）；P200/P197② 直传形——拓宽表达式作 at() 实参直传，
    // 4.4 即出 `movsbl -0x11; sub $1; mov %eax,%ebx` 并入 %ebx（禁 register-asm
    // 中间变量：退化 cwtl+mov 两步，见 getBlueMarble/OnDisconnect probe s3）。
    int weight = (skill->m_field108)[0].at((int)learned - 1);
    return (exp * weight) / 100;
}

// ---- 0x08659faa make_charac_stat(PacketGuard&)：角色属性块序列化。
// 报告：docs/class_func_reports/CUser/make_charac_stat.md（汇编 0x8659faa..0x865a44d
// + Ghidra 反编译 C）。三通路，各尾部统一 put_int(0x53)/put_binary(0x52)/put_byte：
//   C) GameWorld::IsCharacterStatRevisionChannel()（W 0x86977da）为真（ORIG
//      落入块）→ AddInfo 副本被 FairPvPStatScript::FindCharacStat（T 0x8693c66，
//      G_CDataManager()+0x4df8 即 m_fairPvPStat）按 (job, firstGrowType) 查得的
//      CharacterStatus（0x54）非零字段覆盖：头 2 int + 4 u16、循环1 u16
//      [0x10..0x16]、循环2 u16[0x18..0x38]、尾段簇（记录尾段比 _Additioal_info
//      整体错后 2 字节：0x3c↔0x3a … 0x50↔0x4e）；
//   B) 否则有 VIP 特权 → AddInfo 副本 += WongWork::CUserPremium::GetAdditionalInfo
//      （W 0x8694a88，sret 临时）经 _Additioal_info::operator+=（W 0x8693cea）；
//   A) 否则 → 直接输出 getCurCharacAddInfoR() 原块（无副本）。
// 副本经拷贝初始化生成 ORIG 同款内联逐块复制（无 memset 构造）。
// b28d-lane5 N封顶: P11 符号扩展; P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x08659faa | _ZN5CUser16make_charac_statER11PacketGuard
void CUser::make_charac_stat(PacketGuard& packet)
{
    if (G_GameWorld()->IsCharacterStatRevisionChannel())
    {
        // 通路 C：FairPvP 表非零字段覆盖副本。
        _Additioal_info addInfo =
            *static_cast<const _Additioal_info*>(getCurCharacAddInfoR());
        int growType = (int)getCurCharFirstGrowType();
        int job = get_charac_job();
        CharacterStatus* stat =
            G_CDataManager()->m_fairPvPStat.FindCharacStat(job, growType);
        if (stat != 0)
        {
            if (stat->m_field00 != 0)
                addInfo.m_field00 = stat->m_field00;
            if (stat->m_field04 != 0)
                addInfo.m_field04 = stat->m_field04;
            if (stat->m_field08[0] != 0)
                addInfo.m_field08 = stat->m_field08[0];
            if (stat->m_field08[1] != 0)
                addInfo.m_field0a = stat->m_field08[1];
            if (stat->m_field08[2] != 0)
                addInfo.m_field0c = stat->m_field08[2];
            if (stat->m_field08[3] != 0)
                addInfo.m_field0e = stat->m_field08[3];
            for (int i = 0; i <= 3; ++i)   // ORIG 循环1：+0x10..+0x16
            {
                if (stat->m_field08[i + 4] != 0)
                    addInfo.m_field10[i] = stat->m_field08[i + 4];
            }
            for (int j = 0; j <= 0x10; ++j)   // ORIG 循环2：+0x18..+0x38
            {
                if (stat->m_field08[j + 8] != 0)
                    addInfo.m_field18[j] = stat->m_field08[j + 8];
            }
            if (stat->m_field3c != 0)
                addInfo.m_field3a = stat->m_field3c;
            if (stat->m_field40 != 0)
                addInfo.m_field3e = stat->m_field40;
            if (stat->m_field42 != 0)
                addInfo.m_field40 = stat->m_field42;
            if (stat->m_field44 != 0)
                addInfo.m_field42 = stat->m_field44;
            if (stat->m_field48 != 0)
                addInfo.m_field46 = stat->m_field48;
            if (stat->m_field4a != 0)
                addInfo.m_field48 = stat->m_field4a;
            if (stat->m_field4c != 0)
                addInfo.m_field4a = stat->m_field4c;
            if (stat->m_field4e != 0)
                addInfo.m_field4c = stat->m_field4e;
            if (stat->m_field50 != 0)
                addInfo.m_field4e = stat->m_field50;
        }
        packet.put_int(0x53);
        packet.put_binary(reinterpret_cast<const char*>(&addInfo), 0x52);
        packet.put_byte(getCurCharacR()->m_stamina);
    }
    else if (IsHavePremiumAdvantage())
    {
        // 通路 B：AddInfo 副本叠加 VIP 附加属性。
        _Additioal_info addInfo =
            *static_cast<const _Additioal_info*>(getCurCharacAddInfoR());
        addInfo += m_premium.GetAdditionalInfo();
        packet.put_int(0x53);
        packet.put_binary(reinterpret_cast<const char*>(&addInfo), 0x52);
        packet.put_byte(getCurCharacR()->m_stamina);
    }
    else
    {
        // 通路 A：put_int(0x53) → put_binary(AddInfo 原指针, 0x52)。
        packet.put_int(0x53);
        packet.put_binary(
            static_cast<const char*>(getCurCharacAddInfoR()), 0x52);
        packet.put_byte(getCurCharacR()->m_stamina);
    }
}

// ---- 0x0869729a incPlayExpAdd：m_gameOption.m_playExpAdd += v（ORIG W 内联）----
void CUser::incPlayExpAdd(int v)
{
    // ORIG 0x86972a0-0x86972a9：mov field,%edx; mov 0xc(%ebp),%eax; add %eax,%edx（不折叠 add MEM）。
    // 4.4 字段载入恒经 %eax 中转，单段直落 %edx 不可造（N18a/N96；register 绑定全域禁用）。
    m_gameOption.m_playExpAdd = m_gameOption.m_playExpAdd + v;
}

// 0x0866a9a0 gain_sp：SP 双树（0/1）clamp 0x7fffffff 后累加。
void CUser::gain_sp(int v)
{
    // ORIG 0x866a3fe（80 insn 无回跳 = **源码两段手动展开**；remain 无变量：
    // else 支重呼 SkillR+get_remain；cap=INT_MAX-v 每段表达式内联先算（4.1 落
    // %ebx 三行），cmp 结果 setl 物化岛（技巧 22 register bool al 方向）。
    if (0x7fffffff - v <
        ((SkillSlot*)getCurCharacSkillR())
            ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0))
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)0);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0) + v,
                (ENUM_SKILL_TREE_KIND)0);
    }
    if (0x7fffffff - v <
        ((SkillSlot*)getCurCharacSkillR())
            ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1))
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)1);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())
            ->set_remain_sp_at_index(
                ((SkillSlot*)getCurCharacSkillR())
                    ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)1) + v,
                (ENUM_SKILL_TREE_KIND)1);
    }
}

// 0x0866aad2 gain_sfp：SFP 双树（2/3），v==0 直接返回，clamp 0x7fffffff。
void CUser::gain_sfp(int v)
{
    // ORIG 0x866aad2 守卫是正向 `if (v != 0) {…}`（同已 identical 的 gain_sp 形）：
    // 写 `if (v == 0) return;` 早返形会让 4.4 在末支 else 的 call 后多发一条
    // `jmp END`（早返块的 exit 边强制跳转），正向块形则自然 fall-through。
    // 两段展开：cap=INT_MAX-v 每段先算、else 支重呼 SkillR+get_remain_sfp、无缓存变量。
    if (v != 0)
    {
        if (0x7fffffff - v <
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2))
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)2);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(
                    ((SkillSlot*)getCurCharacSkillR())
                        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2) + v,
                    (ENUM_SKILL_TREE_KIND)2);
        }
        if (0x7fffffff - v <
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3))
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(0x7fffffff, (ENUM_SKILL_TREE_KIND)3);
        }
        else
        {
            ((SkillSlot*)getCurCharacSkillW())
                ->set_remain_sfp_at_index(
                    ((SkillSlot*)getCurCharacSkillR())
                        ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)3) + v,
                    (ENUM_SKILL_TREE_KIND)3);
        }
    }
}

// 0x0866ac0e history_log_sp：before==-1 或当前值变化时记 SPAdd(tree 0)。
void CUser::history_log_sp(int before, int delta, eSPAddReason reason)
{
    // ORIG 0x866ac0e 三调用复制形（判据③直分支消费者）：==-1 真支调一次读+SPAdd；
    // 假支 cmp 直落 je；SPAdd 前**再调一次**重取（无 remain 槽）。免缓存三处展开。
    if (before == -1)
    {
        m_historyLog.SPAdd(0,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0),
            delta, reason);
    }
    else if ((((SkillSlot*)getCurCharacSkillR())
                  ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0)
              != before))
    {
        m_historyLog.SPAdd(0,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sp_at_index((ENUM_SKILL_TREE_KIND)0),
            delta, reason);
    }
}

// 0x0866acd0 history_log_sfp：同上（tree 2 / SFPAdd）。
void CUser::history_log_sfp(int before, int delta, eSPAddReason reason)
{
    // ORIG 同构三调用形（tree 2 / SFPAdd，0x0866acd0）。
    if (before == -1)
    {
        m_historyLog.SFPAdd(2,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2),
            delta, reason);
    }
    else if ((((SkillSlot*)getCurCharacSkillR())
                  ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2)
              != before))
    {
        m_historyLog.SFPAdd(2,
            ((SkillSlot*)getCurCharacSkillR())
                ->get_remain_sfp_at_index((ENUM_SKILL_TREE_KIND)2),
            delta, reason);
    }
}

// 0x866273c Remove_Mission：转发 charac_expand 槽 8 的任务列表。
void CUser::Remove_Mission(short kind)
{
    CMissionList_Charac* list =
        (CMissionList_Charac*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
    list->Remove_Mission((int)kind);
}

// 0x0868e9c0
bool CUser::isCompetitionMercenary() const
{
    // ORIG 0x868e9c0：curCharacR 非空 → FindIndexByCharacNo(m_characNo) →
    // 0xffffffff 早退 → CheckCharacIndex → m_characs[idx].GetCompetitionState()==1
    // ORIG 两次 getCurCharacR call（无具名指针槽，技巧 37）：判空一次、
    // 取 m_characNo 再一次。
    if (getCurCharacR() == 0)
        return false;
    // b28c-L5（P218 应用）：下标局部 unsigned int → **int**。ORIG 末段
    //   `mov -0xc(%ebp),%eax / mov 0x8(%ebp),%edx / add $0x6ef6c,%edx /
    //    mov %eax,0x4(%esp) / mov %edx,(%esp) / call vector::operator[]`
    //   是「实参值先物化、容器基址后算」；4.4 对 size_t 形参的下标恒先算基址并
    //   折 `lea disp(%eax),%edx`（反例 ndiff 4 封顶判词已作废）。下标声明 int
    //   ⇒ 实参走 size_t 转换中转，装载序翻为 ORIG 的值先形（P218 探针 w4 同族）。
    //   与 -1 哨兵比较同时回归 ORIG 的 `cmpl $0xffffffff,-0xc(%ebp); jne` 槽比形。
    int idx = (int)m_mercenaryMgr.FindIndexByCharacNo(
        static_cast<unsigned int>(getCurCharacR()->m_characNo));
    if (idx == 0xffffffff)
        return false;
    if (!m_mercenaryMgr.CheckCharacIndex(static_cast<unsigned char>(idx)))
        return false;
    // 残差（ndiff 4，跨版本 N）：末段 ORIG `mov -0xc(%ebp),%eax; mov 0x8(%ebp),%edx;
    // add $0x6effc,%edx`（4.1 先载实参 RHS + mov+add 折成员基址）vs 4.4
    // `mov 0x8(%ebp),%eax; lea 0x6effc(%eax),%edx`（成员对象地址 lea 单条 + 实参后算）；
    // 同一地址两种物化形，4.1↔4.4 求值顺序+lea/add 等效（probe icm/lea 穷举至 N）。
    return m_mercenaryMgr.m_characs[idx].GetCompetitionState() == 1;
}

// 0x0865cfd8
// ORIG：PvP 积分增减。负值：事件(0x36)进行且 grade∈[10,19] 半额（0.5f）→ 0.9f →
// GetPvPPenaltyRevision 三级衰减；整合 PvP 基础渠道 ×70%；0xf 频道超档按
// GetPvPChannelGradePenalty 衰减；任务内限制升降档；重算 grade 并在变更时
// §6.2 巨件核销：P167 102/102 [0缺0多]；gap 641→645（our+4）；残差归因 N82
// 槽号错位族（x87 ROUND 岛跨版本形态差），语义完整仅剩 codegen 差，N 封顶
// （详见 docs/asm_translation_patterns.md 负例表）。
// 重发任务信息包并重建 0x1e 任务列表。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0865cfd8 | _ZN5CUser16update_pvp_pointEi
bool CUser::update_pvp_point(int v)   // b28c-L5 SIGFIX（P174）
{
    if (v == 0)
        return 1;
    if (v < 0)
    {
        if (v < -100000 || v > 100000)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x35d6, 5);
            trace("pvp_exp_error5 %d, charac_no(%d), grade(%d), exp(%d)",
                  v, get_charac_no(-1), get_pvp_grade(),
                  getPVPResultRefR()->m_expPoint);
        }
        if (getPVPResultRefR()->m_pvpGrade < 10)
            return 1;
        // ORIG 0x0865d0bb：GetRepeatEvent(0x36) 后直接解引用 IsEventing，无判空。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x36)
                ->IsEventing((CUser*)0))
        {
            // P183：ORIG 两次独立 getPVPResultRefR() 重物化（asm +0x106/+0x119），
            // 禁 int grade 缓存变元。
            if (getPVPResultRefR()->m_pvpGrade >= 10 &&
                getPVPResultRefR()->m_pvpGrade <= 0x13)
                v = (int)(0.5f * (float)v);
        }
        v = (int)(0.9f * (float)v);
        float revision =
            G_CDataManager()->m_serverParameter.GetPvPPenaltyRevision(
                getPVPResultRefR()->m_pvpGrade);
        v = (int)(revision * (float)v);
        if (v < -100000 || v > 100000)
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x35ed, 5);
            trace("pvp_exp_error6 %d, charac_no(%d), grade(%d), exp(%d)",
                  v, get_charac_no(-1), get_pvp_grade(),
                  getPVPResultRefR()->m_expPoint);
        }
    }
    // 整合 PvP 基础渠道且渠道类型 0 → 打七折。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel() &&
        G_CDataManager()->GetPvPChannelType() == 0)
    {
        v = (v * 70) / 100;
    }
    // 0xf 频道正向积分超过当前渠道最高档 → 按档位差衰减。
    if (G_GameWorld()->GetChannelType() == 0xf && v > 0)
    {
        int diff = getPVPResultRefR()->m_pvpGrade -
                   G_CDataManager()->GetMaxGradePvPChannel();
        if (diff > 0)
        {
            int penalty =
                G_CDataManager()->m_serverParameter.GetPvPChannelGradePenalty(
                    diff);
            v = (v * penalty) / 100;
        }
    }
    if (v < -100000 || v > 100000)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3607, 5);
        trace("pvp_exp_error7 %d, charac_no(%d), grade(%d), exp(%d)",
              v, get_charac_no(-1), get_pvp_grade(),
              getPVPResultRefR()->m_expPoint);
    }
    PvP_Room* room = (PvP_Room*)GetPVPRoom();
    if (room == 0)
        return 1;
    int seat = room->get_user_seat(this);
    if (room->IsPvpObserver(seat))
        return 1;
    bool inMission = has_within_Mission();
    if (!inMission)
    {
        if (G_GameWorld()->GetChannelType() == 0xf &&
            acceptable_within_mission() && v > 0)
            v = 0;
    }
    else
    {
        PacketGuard packet;
        if (v < 1)
        {
            if (v < 0)
            {
                Remove_Mission((short)0x1b);
                Remove_Mission((short)7);
                ((InterfacePacketBuf*)&packet)->put_header(0, 0x158);
                ((InterfacePacketBuf*)&packet)->put_byte(1);
                ((InterfacePacketBuf*)&packet)->finalize(true);
                Send(packet);
            }
        }
        else if (G_GameWorld()->GetChannelType() == 0xf)
        {
            v = 0;
        }
        else
        {
            Remove_Mission((short)0x1b);
            Remove_Mission((short)7);
            ((InterfacePacketBuf*)&packet)->put_header(0, 0x158);
            ((InterfacePacketBuf*)&packet)->put_byte(0);
            ((InterfacePacketBuf*)&packet)->finalize(true);
            Send(packet);
        }
    }
    if (v < -100000 || v > 100000)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x3653, 5);
        trace("pvp_exp_error8 %d, charac_no(%d), grade(%d), exp(%d)",
              v, get_charac_no(-1), get_pvp_grade(),
              getPVPResultRefR()->m_expPoint);
    }
    getPVPResultRefW()->m_expPoint += v;
    if (getPVPResultRefR()->m_expPoint < 0)
    {
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__,
                              0x365c, "PVP correct now<%d> point<%d>",
                              getPVPResultRefR()->m_expPoint, v);
        getPVPResultRefW()->m_expPoint = 0;
    }
    if (getPVPResultRefR()->m_pvpGrade >= 0x1e)
        return 1;
    if (!has_within_Mission() && G_GameWorld()->GetChannelType() == 0xf)
        acceptable_within_mission();
    int newGrade =
        GlobalData::s_ref_pvp_grade->GetPvpGrade(
            getPVPResultRefR()->m_expPoint);
    if (newGrade < 1)
    {
        LogManager::logFormat(1, "user.cpp", __PRETTY_FUNCTION__,
                              0x36bc, "PVP Grade Incorrect %d",
                              getPVPResultRefR()->m_expPoint);
    }
    else
    {
        // ORIG 0x0865d9..：newGrade<1 时无早退，走 else 块（Ghidra C 结构）。
        if (newGrade < getPVPResultRefR()->m_pvpGrade)
        {
            // 降档保护：grade∈[10,0xf] 时按当前档下限回填积分。
            // P183：每处字段访问独立 getPVPResultRefR() 重物化（ORIG 3 处额外 R 调用），
            // 禁用缓存变元（int grade 缓存即缺 3 call 语义缺口）。
            if (getPVPResultRefR()->m_pvpGrade >= 10 &&
                getPVPResultRefR()->m_pvpGrade <= 0xf)
            {
                getPVPResultRefW()->m_expPoint =
                    GlobalData::s_ref_pvp_grade->GetPvpPoint(
                        getPVPResultRefR()->m_pvpGrade) + 1;
                newGrade = getPVPResultRefR()->m_pvpGrade;
            }
        }
        int oldGrade = getPVPResultRefR()->m_pvpGrade;
        getPVPResultRefW()->m_pvpGrade = newGrade;
        if (has_within_Mission() && getPVPResultRefR()->m_pvpGrade > oldGrade)
            getPVPResultRefW()->m_pvpGrade = oldGrade;   // 任务中禁止升档
        if (getPVPResultRefR()->m_pvpGrade != oldGrade)
        {
            PacketGuard packet;
            getCurCharacQuestR()->get_quest_info(
                reinterpret_cast<char*>(&packet));
            if (G_GameWorld()->GetChannelType() != 0xf)
            {
                Remove_Mission((short)0x1e);
                CMissionList_Charac* list = (CMissionList_Charac*)GetCharacExpandData(
                    (ENUM_CHARAC_EXPAND_TYPE)8);
                list->MakeMissionList_JustKind(*this, 0x1e);
            }
            Send(packet);
        }
    }
    return 1;
}
// 0x08657450
// ORIG：每日重置——副本金币数据、安全卡/密保取消次数（非 0 时归零并回写 DB）、
// NPC 好感度、ActionPoint（APSystem）落库并结算今日行动/勋章奖励。
void CUser::resetDailyData()
{
    getDungeonGainedGold()->reset();
    if (getSecurityCard()->getCancelCnt() != 0)
    {
        getSecurityCard()->resetCancelCnt();
        DB_SecurityCardUpdateCancelCnt::makeRequest(get_acc_id(), 0);
    }
    if (getPad()->getCancelCnt() > 0)
    {
        getPad()->setCancelCnt(0);
        DB_PassPadUpdateCancelCnt::makeRequest(get_acc_id(), 0,
                                               (char*)getWebAddress());
    }
    resetNPCRelationShipDailyData();
    // ORIG 0x08657450：resetNPCRelationShipDailyData 后尾调 sendNPCRelationShipFavor；
    // ActionPoint 经 m_actionPoint.GetActionPoint()（返回 this+0x08 数据体）传递，
    // 不可取 &m_actionPoint（+0x00 偏移不同）。
    sendNPCRelationShipFavor();
    APSystem::DB_UpdateActionPoint::makeRequest(GetUID(),
        *(const APSystem::_SIG_LOAD_ACTION_POINT*)m_actionPoint.GetActionPoint(), true);
    APSystem::CUserProc::SetTodayActionAndCheckMedalReward(this);
}

// 0x08657f10
// ORIG：每日币恢复。getCurCharacR() 为空直接返回；若每日币事件(4)进行中，
// 按角色等级取当日币数并读背包币；premium 有优势时加 premium 币加成；
// 最后按背包币是否足够（coin < need）设置 SetOpencoin(0/1)。
void CUser::RecoverCoin(unsigned int v)
{
    // ORIG 0x8657f10：getCurCharacR 结果 sete 物化 guard（技巧 22 register bool al）；
    // pEvent@-0xc（先赋后测 `cmpl $0,-0xc; je` 短路岛 @8657f46-4a，技巧 40）、
    // IsEventing 尾 `test %al; je F; mov$1;F:` 物化（@8657f66）；三处 GetCoin 结果
    // 直读 %eax `cmp 0xc(%ebp),%eax` 免槽（技巧 37，ORIG 0x8657faa/0x8657fe2/0x8657ff8）。
    if (getCurCharacR() == 0)
        return;
    CCoinEventPerDay* pEvent =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    // P0/N96 去绑定：短路守卫直判（P224 极性族）——具名 eventOn bool 槽反使
    // pEvent 平移 -0xc→-0x10（ORIG eventOn 为 4.1 al 物化无槽，N96 代价）。
    if (pEvent != 0 && pEvent->IsEventing((CUser*)0))
    {
        v = pEvent->GetCoinNoPerDay(get_charac_level());
        // ORIG 0x8657faa：`call GetCoin; cmp 0xc(%ebp),%eax` 后无消费者 =
        // 4.1 发射**空 if 死比较**（P184 死块照抄；probe p15.d1 实证 4.4
        // `if (call() != v) {}` 保留裸 dead cmp 无 setne 展开）。
        if (getCurCharacInvenR()->GetCoin() != v)
        {
        }
    }
    if (IsHavePremiumAdvantage())
    {
        v += m_premium.GetAdvantageCoin();
        if (getCurCharacInvenR()->GetCoin() != v)  // ORIG 0x8657fe2 同形死比较
        {
        }
    }
    if (getCurCharacInvenR()->GetCoin() < v)
        SetOpencoin((char)0);
    else
        SetOpencoin((char)1);
}

// 0x086568fc
// ORIG：开放标志包(0xb1)。角色存在时：每日币事件(4)进行中取等级 1 的当日币数，
// premium 有优势加币加成；计算缺口（need - coin）并在 opencoin 标志下发包。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x086568fc | _ZN5CUser12SendOpenflagEv
void CUser::SendOpenflag()
{
    if (getCurCharacR() == 0)
        return;
    unsigned int coinNeed = 0;
    int lack = 0;
    CCoinEventPerDay* pEvent =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    bool eventOn = (pEvent != 0) && (pEvent->IsEventing((CUser*)0) != 0);
    if (eventOn)
        coinNeed = (unsigned int)pEvent->GetCoinNoPerDay(1);
    if (IsHavePremiumAdvantage())
        // ORIG 0x086568fc：GetPremiumInfo() 转发后 GetAdvantageCoin，不直读 m_premium
        coinNeed = coinNeed + (unsigned int)GetPremiumInfo()->GetAdvantageCoin();
    unsigned int coin = getCurCharacInvenR()->GetCoin();
    bool enough = !(coin < coinNeed && GetOpencoin() != (char)1);
    if (enough)
    {
        lack = 0;
    }
    else
    {
        lack = (int)(coinNeed - getCurCharacInvenR()->GetCoin());
    }
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xb1);
    ((InterfacePacketBuf*)&packet)->put_byte((int)GetOpencoin());
    ((InterfacePacketBuf*)&packet)->put_int(lack);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
}

// 0x086931c4
// ORIG：无效物品删除补偿。逐条记 DeleteInvaildItem 日志并按脚本换算奖励道具；
// 邮件发件人/正文按脚本字符串表（表 4）翻译，空则用 defalt_string；
// 每 10 件一批 ReqDBSendNewSystemMultiMail，余件单独一封。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086931c4 | _ZN5CUser28RewardItem2DeleteInvalidItemERKSsRKSt6vectorISt4pairIiiESaIS4_EE
void CUser::RewardItem2DeleteInvalidItem(
    const std::string& name, const std::vector<std::pair<int, int> >& list)
{
    static const char* defalt_string = "";   // ORIG 局部静态 defalt_string(0x93ace50)

    if (list.empty() || getCurCharacR() == 0)
        return;

    std::vector<Inven_Item> items;
    for (size_t i = 0; i < list.size(); ++i)
    {
        m_historyLog.DeleteInvaildItem(name, list[i].first, list[i].second);
        CItem* itemDef = G_CDataManager()->find_item(list[i].first);
        if (itemDef == 0)
            continue;
        int rewardIdx = 0;
        int rewardCnt = 0;
        if (G_CDataManager()->m_deleteInvalidItem.GetRewardItemInfo(
                list[i].first, rewardIdx, rewardCnt) &&
            rewardCnt > 0)
        {
            CItem* rewardDef = G_CDataManager()->find_item(rewardIdx);
            if (rewardDef != 0)
            {
                Inven_Item add;
                add.m_addInfo = rewardIdx;   // +0x02 先写奖励物品索引
                rewardDef->make_item(add);   // ORIG vtable 槽 2 填充默认字段
                if (rewardDef->is_stackable())
                    add.m_addInfo2 = rewardCnt;   // +0x07 堆叠数量
                items.push_back(add);
            }
        }
    }
    if (items.empty())
        return;

    std::string str0;
    std::string str1;
    char sender[0x1e];
    memset(sender, 0, sizeof(sender));
    char body[0x100];
    memset(body, 0, sizeof(body));
    G_CDataManager()->m_deleteInvalidItem.GetMailInfo(str0, str1);
    const char* s0 =
        str0.empty() ? defalt_string
                     : g_scriptStringManager_.findString(4, toTString(str0), 0);
    strncpy(sender, s0, 0x1d);
    const char* s1 =
        str1.empty() ? defalt_string
                     : g_scriptStringManager_.findString(4, toTString(str1), 0);
    strncpy(body, s1, 0xff);

    unsigned int count = 0;
    Inven_Item batch[10];
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        batch[count].setCopy(items[i]);
        count++;
        if (count == 10)
        {
            count = 0;
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                sender, batch, 10u, 0u, (unsigned int)getCurCharacNo(), body,
                (int)strlen(body), 0u, GetServerGroup(), false);
        }
    }
    if (count != 0)
    {
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
            sender, batch, count, 0u, (unsigned int)getCurCharacNo(), body,
            (int)strlen(body), 0u, GetServerGroup(), false);
    }
}

// 0x08656caa
// ORIG：每日登录物品补给。等级>0x13 时道具 0x1057 补至 3 个（无则插入、有则
// 堆叠补满并记 ItemAdd 日志）；随后 ResetItemByScript/RefillItemByScript/
// deleteSpecificItem 脚本链并回发受影响槽位包；祭坛免费票补满；截止时间前
// 发放道具 0x1cab（背包满走 game_server_msg_24/25 系统邮件）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08656caa | _ZN5CUser12AddDailyItemEv
void CUser::AddDailyItem()
{
    int level = get_charac_level();
    if (level > 0x13)
    {
        CItem* def = G_CDataManager()->find_item(0x1057);
        if (def != 0)
        {
            Inven_Item tmp;
            int slot = getCurCharacInvenR()->GetInvenData(0x1057, tmp);
            if (slot < 0)
            {
                def->make_item(tmp);
                tmp.m_addInfo = 0x1057;
                tmp.m_addInfo2 = 3;   // 数量
                slot = getCurCharacInvenW()->insertItemIntoInventory(
                    tmp, eItemAddReason_0xd, true, false);
            }
            else if (tmp.m_addInfo2 < 3)
            {
                m_historyLog.ItemAdd((INVEN_TYPE)1, 3 - tmp.m_addInfo2,
                                     tmp.get_add_info(), tmp,
                                     (eItemAddReason)0xd);
                tmp.m_addInfo2 = 3;
                getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot, tmp);
            }
            if (slot >= 0)
                SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }
    std::vector<std::pair<int, int> > resetList;
    std::vector<std::pair<int, int> > delList;
    ResetItemByScript(resetList);
    RefillItemByScript(resetList);
    deleteSpecificItem(resetList, delList);
    for (std::vector<std::pair<int, int> >::const_iterator it = delList.begin();
         it != delList.end(); ++it)
    {
        if (it->first >= 0 && it->first <= 0x1b && it->second >= 0)
            SendUpdateItemList(eSendTarget_1, (ENUM_ITEMSPACE)it->first,
                               it->second);
    }

    m_advanceAltar.refillTicketFree();   // ORIG 0x0865f6dc：每日门票免费补满
    if (GlobalData::s_systemTime_.getCurSec() < (time_t)0x4811f1f0)
    {
        CItem* def = G_CDataManager()->find_item(0x1cab);
        if (def != 0)
        {
            Inven_Item tmp;
            def->make_item(tmp);
            tmp.m_addInfo = 0x1cab;
            tmp.m_addInfo2 = 1;
            int slot = getCurCharacInvenW()->insertItemIntoInventory(
                tmp, eItemAddReason_0xd, true, true);
            if (slot == -1)
            {
                char sender[0x15];
                strncpy(sender,
                        g_scriptStringManager_.findString(
                            4, "game_server_msg_24", 0),
                        0x14);
                char body[0x100];
                strncpy(body,
                        g_scriptStringManager_.findString(
                            4, "game_server_msg_25", 0),
                        0xff);
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    sender, tmp, 0u, (unsigned int)getCurCharacNo(), body,
                    (int)strlen(body), 0u, GetServerGroup(), false, false);
            }
            else
            {
                PacketGuard packet;
                ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
                ((InterfacePacketBuf*)&packet)->put_byte(0);
                ((InterfacePacketBuf*)&packet)->put_short(1);
                bool ok = getCurCharacInvenR()->MakeItemPacket(
                    (INVEN_TYPE)1, slot, packet);
                if (ok)
                {
                    ((InterfacePacketBuf*)&packet)->finalize(true);
                    Send(packet);
                }
            }
        }
    }
}

// 0x0868aaea
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868aaea | _ZN5CUser24SendOneADayItemShopIndexERKSt6vectorISt4pairIiiESaIS2_EE
void CUser::SendOneADayItemShopIndex(
    const std::vector<std::pair<int, int> >& list)
{
    // ORIG 0x868aaf2：GetRepeatEvent(0x6d) → vtable+0x34(0)；返回真才发包
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x6d);
    if (event == 0)
        return;
    if (!event->IsEventing(0))
        return;

    PacketGuard packet;                                          // ORIG local_20
    InterfacePacketBuf& pkt = *(InterfacePacketBuf*)&packet;
    pkt.put_header(0, 0x155);
    int count = (int)list.size();
    pkt.put_byte(count);
    for (int i = 0; i < count; ++i)
    {
        pkt.put_int(list[i].first);                              // ORIG operator[] → *piVar2
        pkt.put_int(list[i].second);                             // ORIG put_int(second)
    }
    pkt.finalize(true);
    Send(packet);
}

// 0x0867c2d8
// ORIG：单槽位物品更新包(0xe)。GetInvenRef 空 → return 0；否则写槽头 +
// 物品字段（addInfo2 为 0 时首 int 写 -1）、强化选项、avatar 宝石槽 0x1e 字节
// 与扩展信息 4 字节、生物装备剩余天数（type==5），追加三转编年史与整件包体。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867c2d8 | _ZN5CUser14SendUpdateItemENS_11eSendTargetE14ENUM_ITEMSPACEi
int CUser::SendUpdateItem(eSendTarget target, ENUM_ITEMSPACE space, int slot)
{
    int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)space);
    const Inven_Item* item = getCurCharacInvenR()->GetInvenRef(invenType, slot);
    if (item == 0)
        return 0;
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xe);
    ((InterfacePacketBuf*)&packet)->put_byte((int)space);
    ((InterfacePacketBuf*)&packet)->put_short(1);
    ((InterfacePacketBuf*)&packet)->put_short(slot);
    int uid = item->m_addInfo2;   // +0x07
    if (uid == 0)
        ((InterfacePacketBuf*)&packet)->put_int(-1);
    else
        ((InterfacePacketBuf*)&packet)->put_int(item->m_addInfo);   // +0x02
    ((InterfacePacketBuf*)&packet)->put_int(uid);
    ((InterfacePacketBuf*)&packet)->put_byte((int)item->GetItemAttr());
    ((InterfacePacketBuf*)&packet)->put_short((int)item->m_fieldb);
    ((InterfacePacketBuf*)&packet)->put_byte((int)(unsigned char)item->m_field0);
    ((InterfacePacketBuf*)&packet)->put_int(item->m_fieldd);
    ((InterfacePacketBuf*)&packet)->put_byte(
        (int)item->m_amp.getAbilityType());
    ((InterfacePacketBuf*)&packet)->put_short(
        (int)item->m_amp.getAbilityValue());
    if (item->isAvatarItemType())
    {
        ((InterfacePacketBuf*)&packet)->put_int(0x1e);
        // P183：ORIG 对 get_add_info 各次访问独立重物化 getCurCharacInvenR()
        // ->GetAvatarItemMgrR()（socket 一次、expansion 再一次），禁 mgr 缓存。
        const char* socketData =
            getCurCharacInvenR()->GetAvatarItemMgrR()
                ->getJewelSocketData(item->get_add_info());
        ((InterfacePacketBuf*)&packet)->put_binary(socketData, 0x1e);
        const char* expansion =
            static_cast<const char*>(
                getCurCharacInvenR()->GetAvatarItemMgrR()
                    ->GetExpansionInfo(item->get_add_info()));
        ((InterfacePacketBuf*)&packet)->put_int(4);
        if (expansion == 0)
        {
            stAvatarExpansionInfo_t init4;
            init4.init();
            // ORIG 3 处 put_binary 均走 const 重载（asm 0815096a）。
            ((InterfacePacketBuf*)&packet)->put_binary(
                reinterpret_cast<const char*>(&init4), 4);
        }
        else
        {
            ((InterfacePacketBuf*)&packet)->put_binary(expansion, 4);
        }
    }
    if (item->m_field1 == (unsigned char)5)
    {
        int tick = 0;
        tick = OS_API::GetDateTimeTick();
        int remainDate = getCurCharacInvenR()->GetCreatureMgrR()->GetRemainDate(
            item->get_add_info(), tick);
        ((InterfacePacketBuf*)&packet)->put_int(remainDate);
    }
    G_CDataManager()->m_itemUpgrade->make3rdChroniclePacket(this, *item,
                                                            &packet);
    ((InterfacePacketBuf*)&packet)->put_packet(*item);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    SendPacket(target, packet);
    return 1;
}

// 0x0868de0a
// ORIG：光环 avatar 选项变更 -> DB_UpdateAuraAvatarOption::makeRequest(uid, characNo, idx, value)
void CUser::UpdateAuraAvatarOption(int a, int b)
{
    if (a >= 0 && a <= 2)
    {
        DB_UpdateAuraAvatarOption::makeRequest(GetUID(), getCurCharacNo(), a, b);
    }
}

// 0x0868dff8
// ORIG：光环 avatar 装备检查。无角色 → 1；槽 9 无物品/不可装备 → 0x11；
// 物品为光环 avatar 选项 → 回写选项值并返回 0，否则 0x17。
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0868dff8 | _ZN5CUser20is_equip_aura_avatarEcRi
char CUser::is_equip_aura_avatar(char slot, int& out)
{
    if (getCurCharacR() == 0)
        return 1;
    Inven_Item* item = getCurCharacInvenW()->GetInvenRef(0, 9);
    if (item == 0)
        return 0x11;
    if (item->isEmpty() != 0)
        return 0x11;
    if (item->isEquipableItemType() != (char)1)
        return 0x11;
    // ORIG 0x868e088-0x868e09c：itemIdx 载 %eax 后 `mov %eax,%ebx` 跨 G_CDataManager
    // call 缓存（push %ebx/帧 0x34），find_item 结果落 -0xc（equip 槽），item 槽 -0x10。
    // P0/N96 去绑定：字段作实参直传（P200 拓宽并入 callee-saved，跨 receiver
    // call 保活），残差记 N18a。
    CEquipItem* equip = (CEquipItem*)G_CDataManager()->find_item(item->m_addInfo);
    if (equip == 0)
        return 0x11;
    if (equip->is_aura_avatar_option(slot) != (char)1)
        return 0x17;
    out = equip->get_aura_avatar_option_value();
    return 0;
}

// 0x0867e092
// ORIG：每日道具过期删除（idx 区间 [3,8] 与 [0x39,0x68]）后聚合发送：29 个 0xe 包
// 按物品空间预置包头并记录计数字段位置；cargo/账号金库直接 MakeItemPacket，
// 背包空间空槽写固定"已删"载荷；最后按非零计数回填数量并逐包下发。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867e092 | _ZN5CUser19processDelDailyItemEv
void CUser::processDelDailyItem()
{
    extern Inven_Item g_emptySlot;   // ORIG 全局（0x943ddc0）

    std::vector<std::pair<int, int> > list;
    deleteDailyStackableItem(list, 3, 8);
    deleteDailyStackableItem(list, 0x39, 0x68);
    if (list.empty())
        return;

    PacketGuard pkts[29];
    int counts[29];
    int indices[29];
    memset(counts, 0, sizeof(counts));
    for (int i = 0; i < 0x1d; ++i)
    {
        ((InterfacePacketBuf*)&pkts[i])->put_header(0, 0xe);
        ((InterfacePacketBuf*)&pkts[i])->put_byte(i);
        indices[i] = ((InterfacePacketBuf*)&pkts[i])->get_index();
        ((InterfacePacketBuf*)&pkts[i])->put_short(0);
    }
    for (std::vector<std::pair<int, int> >::const_iterator it = list.begin();
         it != list.end(); ++it)
    {
        const int space = it->first;
        const int slot = it->second;
        if (space == 2)
        {
            getCurCharacCargoR()->MakeItemPacket(&pkts[space], slot);
            counts[space]++;
        }
        else if (space == 0xc)
        {
            if (IsExistAccountCargo())
            {
                GetAccountCargo()->MakeItemPacket(&pkts[space], slot);
                counts[space]++;
            }
        }
        else
        {
            int invenType = 1;
            if (space == 3)
                invenType = 0;
            else if (space == 7)
                invenType = 3;
            else if (space == 1)
                invenType = 2;
            Inven_Item slotItem =
                getCurCharacInvenR()->GetInvenSlot(invenType, slot);
            if (slotItem.m_addInfo == 0)   // ORIG：test dword [ret+2]
            {
                InterfacePacketBuf* pkt = (InterfacePacketBuf*)&pkts[space];
                pkt->put_short(slot);
                pkt->put_int(-1);
                pkt->put_int(0);
                pkt->put_byte(0);
                pkt->put_short(0);
                pkt->put_byte(0);
                pkt->put_int(0);
                pkt->put_byte(0);
                pkt->put_short(0);
                pkt->put_byte(0);
                pkt->put_packet(g_emptySlot);
            }
            counts[space]++;
        }
    }
    for (int i = 0; i < 0x1d; ++i)
    {
        if (counts[i] != 0)
        {
            ((InterfacePacketBuf*)&pkts[i])->put_short(indices[i], counts[i]);
            ((InterfacePacketBuf*)&pkts[i])->finalize(true);
            Send(pkts[i]);
        }
    }
}

// 0x086802b8（reqHumanCertify4ClearMap.md 反编译全文转写）：
// flag 为真时按 mcap 点数概率（get_rand_int(10000) < point）触发；点数 ≥100.0
// 时按 (100-point/100) 与 0 取大降 stdDropRate（ServiceRestrict(3,1) 受限时）
// 并 DB_SaveUserHandicap 上报（DB 域待接线，暂略）；随后 reqHumanCertify()
// 通过则取消人证并 InsertTimer(0,uid,9,0x3c,key++,−1)。
// b28d-lane5 N封顶: N85 x87比较lowering; N82/P112 槽号分配 | ORIG 0x086802b8 | _ZN5CUser24reqHumanCertify4ClearMapEb
void CUser::reqHumanCertify4ClearMap(bool flag)
{
    bool forceCertify;
    if (flag)
    {
        forceCertify = get_rand_int(0x2710) < m_mcap.getPoint();
    }
    else
    {
        forceCertify = true;
    }
    if (!forceCertify)
        return;

    unsigned int point = m_mcap.getPoint();
    if ((double)point >= 100.0)
    {
        int rate = std::max(100 - (int)(point / 100), 0);
        if (rate < getStdDropRate())
        {
            {
                ServiceRestrictManager* srm =
                    ARAD::Singleton<ServiceRestrictManager>::Get();
                if (srm != 0 && srm->isRestricted(RestrictCategory::Enum_3, 1))
            {
                setStdDropRate(rate);
                int nowRate = getStdDropRate();
                DB_SaveUserHandicap::makeRequest(get_acc_id(), 0x9c41, 0x65,
                                                 nowRate, 1, 0);
                }
            }
        }
    }

    if (reqHumanCertify())
    {
        setHumanCertified(false);
        if (0x7ffffffe < m_humanCertifyTimerKey)
            m_humanCertifyTimerKey = 0;
        m_humanCertifyTimerKey += 1;
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, GetUID(),
                                    (TIMER_MESSAGE)9, 0x3c,
                                    m_humanCertifyTimerKey, (unsigned int)-1);
    }
}

// ---- CUser::master_new_skills @ 0x0866b53e ----
// 批量习得技能：按 _Mastered_skill 数组计算 SP/SFP 用量并扣减对应技能树余量，
// 将技能槽写入（set_skill_slot_at_index），同时记录 SP/SFP 消耗日志。
// PvP 树频道直接返回 true；普通树按 secondGrow/firstGrow 走成长型
// get_give_skill 展开（ORIG 866b844-866bcd5 逐段转写）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0866b53e | _ZN5CUser17master_new_skillsEP15_Mastered_skill20ENUM_SKILL_TREE_KIND
bool CUser::master_new_skills(_Mastered_skill* skills, ENUM_SKILL_TREE_KIND kind)
{
    int usedSp = 0;
    int usedSfp = 0;
    ((SkillSlot*)getCurCharacSkillR())->calcUsedSP(skills, usedSp, usedSfp);

    int remainSp = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(kind);
    if (remainSp < usedSp)
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(0, kind);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sp_at_index(remainSp - usedSp, kind);
    }

    ENUM_SKILL_TREE_KIND sfpKind =
        (kind == SKILL_TREE_NONE || kind == SKILL_TREE_1)
            ? SKILL_TREE_SFP_1
            : SKILL_TREE_SFP_2;
    int remainSfp = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(sfpKind);
    if (remainSfp < usedSfp)
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(0, sfpKind);
    }
    else
    {
        ((SkillSlot*)getCurCharacSkillW())->set_remain_sfp_at_index(remainSfp - usedSfp, sfpKind);
    }

    // ORIG 0x866b6eb/0x866b7c4：SFPSub/SPSub 的此处调用实参顺序是
    // (扣减后余量, treeKind, 消耗量, reason)，不同于头文件参数名；保留
    // 原始调用值及扣减后的余量，避免改变 LogHistory 字段顺序。
    const int remainSfpAfter = ((SkillSlot*)getCurCharacSkillR())->get_remain_sfp_at_index(sfpKind);
    m_historyLog.SFPSub(remainSfpAfter, (int)sfpKind, usedSfp,
                        (eSPSubReason)1);
    const int remainSpAfter = ((SkillSlot*)getCurCharacSkillR())->get_remain_sp_at_index(kind);
    m_historyLog.SPSub(remainSpAfter, (int)kind, usedSp,
                       (eSPSubReason)1);

    ((SkillSlot*)getCurCharacSkillW())->set_skill_slot_at_index((char*)skills, 0x198, kind);

    if (G_GameWorld()->IsPvPSkilTreeChannel())
        return true;

    // ORIG 866b852：secondGrow 非零 → get_give_skill(flag=0) 展开。
    if (getCurCharSecondGrowType() != 0)
    {
        std::vector<std::pair<int, int> > giveVec;
        // ORIG 0x0866b53e：giveVec 后构造独立 pair 局部（C1），循环内逐字段拷入
        std::pair<int, int> pr;
        CCharacList* chars = G_CDataManager()->m_characList;
        char got = chars->m_characs[get_charac_job()].get_give_skill(
            getCurCharFirstGrowType(), getCurCharSecondGrowType(), giveVec, 0);
        if (got == 0)
            return false;
        for (int i = 0; i < (int)giveVec.size(); ++i)
        {
            pr = giveVec[i];
            int skillId = pr.first;
            int level = pr.second;
            CSkill* skill =
                G_CDataManager()->find_skill(get_charac_job(), skillId);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp",
                    "bool CUser::master_new_skills(_Mastered_skill*, "
                    "ENUM_SKILL_TREE_KIND)",
                    0x52bd, "G_CDataManager()->find_skill( %d, %d )",
                    get_charac_job(), skillId);
                return false;
            }
            int group = skill->get_group();
            int slotNo = ((SkillSlot*)getCurCharacSkillR())->get_skillslot_no(skillId, group, kind, true);
            if (slotNo < 0)
            {
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(get_charac_job(), skillId, level, kind);
            }
        }
        return true;
    }

    // ORIG 866ba98：firstGrow 非零 → get_give_skill(flag=1) 展开。
    if (getCurCharFirstGrowType() != 0)
    {
        std::vector<std::pair<int, int> > giveVec;
        // ORIG 0x0866b53e：同第二块，giveVec 后 pair 局部（C1）+ 循环内拷入
        std::pair<int, int> pr;
        CCharacList* chars = G_CDataManager()->m_characList;
        char got = chars->m_characs[get_charac_job()].get_give_skill(
            getCurCharFirstGrowType(), getCurCharSecondGrowType(), giveVec, 1);
        if (got == 0)
            return false;
        for (int i = 0; i < (int)giveVec.size(); ++i)
        {
            pr = giveVec[i];
            int skillId = pr.first;
            int level = pr.second;
            CSkill* skill =
                G_CDataManager()->find_skill(get_charac_job(), skillId);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "user.cpp",
                    "bool CUser::master_new_skills(_Mastered_skill*, "
                    "ENUM_SKILL_TREE_KIND)",
                    0x52e0, "G_CDataManager()->find_skill( %d, %d )",
                    get_charac_job(), skillId);
                return false;
            }
            int group = skill->get_group();
            int slotNo = ((SkillSlot*)getCurCharacSkillR())->get_skillslot_no(skillId, group, kind, true);
            if (slotNo < 0)
            {
                ((SkillSlot*)getCurCharacSkillW())->growtype_skill(get_charac_job(), skillId, level, kind);
            }
        }
        return true;
    }
    return true;
}

// 0x0867cbe4（从 GameStubs_remaining.cpp 迁移，语义对齐 CUser/ProcPremiumFatigue.md）
void CUser::ProcPremiumFatigue()
{
    if (getCurCharacR() == 0)
        return;
    // ORIG 0x0867cbe4：正向 `if (IsHavePremiumAdvantage())` 包体（Ghidra
    // `if (cVar2 != '\0')`，产 `test %al;je <ret>`；不写早退 `==0` 形以免 xor）。
    if (IsHavePremiumAdvantage())
    {
        // P48 双分支各自重取 GetAdvantageFatigueRate（不缓存）；pcRoom 命中才加
        // dm+0x51c 周末加成。
        if (GlobalData::s_event_manager->GetRepeatEvent(0x57)->IsEventing(0) &&
            m_premium.GetAdvantageFatigueRate() != 0 && GetPCRoomNo() != 0)
        {
            setCurCharacMaxPremiumFatigue(
                static_cast<unsigned short>(
                    (short)((static_cast<int>(static_cast<unsigned short>(
                                m_premium.GetAdvantageFatigueRate())) *
                            DEFAULT_MAX_FATIGUE) / 100) +
                    G_CDataManager()->m_serverParameter
                        .m_weekendBouns4b4.m_bonusFatiguePercent));
        }
        else
        {
            setCurCharacMaxPremiumFatigue(
                static_cast<unsigned short>(
                    (static_cast<int>(static_cast<unsigned short>(
                         m_premium.GetAdvantageFatigueRate())) *
                     DEFAULT_MAX_FATIGUE) / 100));
        }
    }
}
// ORIG T 0x0868866e（报告 docs/class_func_reports/CUser/isSecurityServiceUsed.md）：
// 安全键盘激活 或 安全卡激活 或 secuGrade(+0x8e97f) 非零 → 已启用安全服务。
bool CUser::isSecurityServiceUsed()
{
    // ORIG 0x8688689/0x86886a0：|| 短路链双 `test; jne mov$1` 共享尾岛 + 末项
    // movzbl flag 的 `je mov$0 / fall mov$1` + bool 结果二次物化岛。
    if (getPad()->isActivate() || getSecurityCard()->isActivate() ||
        m_hades.m_secuGrade)
        return true;
    return false;
}

// ORIG W 0x0863bea2：return m_premium(+0x7121c).GetTotalPremiumCount() != 0
// （scout 报告误关联 0x867cd20 = _UpdateUserInfo4Guild，已按真实符号核实）。

// ORIG T 0x0864fc3e CheckLimitMiniumAge：唯一 owner 为 CUserOnlyOrig.cpp
// （W7_BuildHarmonize 去重：该版含 outUnique=unique_id 低16位写出，更忠实）。
bool CUser::IsPremiumUser() const
{
    return m_premium.GetTotalPremiumCount() != 0;
}

// ============================================================================
// 租借物品序列化链（CUser::SaveInventory 直接依赖；m_rentalInfo@+0x6ef78 为
// map<uint, map<uint, vector<InstanceRentalSystem::RentalInfo>>>）。
// ============================================================================

// ORIG T 0x08676dba：`out = m_rentalInfo[param]`（外层 map 键 param，值
// map<uint, vector<RentalInfo>>）；仅当键存在时拷贝。
void CUser::GetInstanceRentalInfo(
    std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> >& out,
    int param)
{
    std::map<unsigned int, std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> > >::iterator it =
        m_rentalInfo.find((unsigned int)param);
    if (it == m_rentalInfo.end())
        return;
    out = it->second;
    return;
}

// ORIG T 0x086773ea：返回 m_rentalInfo[param][charac_no] 是否存在；存在则拷贝到 out。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086773ea | _ZN5CUser26getCurrentCharacRentalInfoERSt6vectorIN20InstanceRentalSystem10RentalInfoESaIS2_EEi
bool CUser::getCurrentCharacRentalInfo(
    std::vector<InstanceRentalSystem::RentalInfo>& out, int param)
{
    if (getCurCharacR() == 0)
        return false;
    unsigned int characNo = (unsigned int)getCurCharacNo();
    if (characNo == 0)
        return false;

    std::map<unsigned int, std::vector<InstanceRentalSystem::RentalInfo> > inner;
    GetInstanceRentalInfo(inner, param);
    std::map<unsigned int,
             std::vector<InstanceRentalSystem::RentalInfo> >::iterator it =
        inner.find(characNo);
    if (it == inner.end())
        return false;
    out = it->second;
    return true;
}

// ORIG T 0x08677516：`m_rentalInfo[param][charac_no] = in`（vector 拷贝）。
void CUser::setCurrentCharacRentalInfo(
    std::vector<InstanceRentalSystem::RentalInfo>& in, int param)
{
    // ORIG 0x8677516：cur 落 -0xc 槽判空（mov %eax,-0xc; cmpl $0,-0xc; je）；
    // characNo = 直读 cur->m_characNo（`mov (%eax),%eax` @0x867752x，非
    // getCurCharacNo 调用——我们旧形多一次 call 且判空被折 test;setne）。
    const _Charac_info* cur = getCurCharacR();
    if (cur == 0)
        return;
    unsigned int characNo = (unsigned int)cur->m_characNo;
    if (characNo == 0)
        return;
    m_rentalInfo[(unsigned int)param][characNo] = in;
}

// ORIG T 0x08677584：遍历当前角色租借列表，对仍存在的租借物品在序列化缓冲
// （SIG_SAVE_INVENTORY）中清除对应背包槽；并回写租借条目的 inEquip 标志。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677584 | _ZN5CUser32removeRentalItemInfoFromInvenBufEP18SIG_SAVE_INVENTORYi
bool CUser::removeRentalItemInfoFromInvenBuf(SIG_SAVE_INVENTORY* sig, int param)  // b28c-L5 SIGFIX
{
    if (sig == 0)
        return 0;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, param) != 1)
        return 0;

    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        int itemIndex = rental.m_itemIndex;
        const CInventory* inven = getCurCharacInvenR();
        if (inven->CheckExistItemInInventoryOrEquipment(itemIndex, slot, bInEquip) == 1)
        {
            rental.m_inEquip = bInEquip;
            if (bInEquip == false)
            {
                sig->m_invenType1[slot].reset();   // 背包（类型1）槽
            }
            else
            {
                slot -= 10;
                sig->m_invenType0[slot].reset();   // 装备（类型0）槽
            }
        }
        else
        {
            LogManager::logFormat(
                1, "user.cpp",
                __PRETTY_FUNCTION__,
                0x6d15,
                "Rent error. CUser::removeRentalItemInfoFromInvenBuf return false. Acc(%d), itemIndex(%d)",
                get_acc_id(), itemIndex);
            getCurCharacInvenR()->ShowAllExistItemsForErrorCheck();
        }
    }

    setCurrentCharacRentalInfo(rentalList, param);
    return 1;
}

// 0x0864fe52（ORIG CUser::SaveInventory()）
// ORIG 反编译控制流（docs/class_func_reports/CUser/SaveInventory.md）：
//   1) isLocked4DataLoad() 非零 → cMyTrace 报错 "Can not save character until
//      loading done(m_id: %s, charac_no: %d" 并返回 0；
//   2) G_GameWorld()->GetChannelType()==7 → 返回 0；
//   3) 否则 BigStreamPool::Acquire + CStreamGuard 序列化 0xad99 字节的
//      SIG_SAVE_INVENTORY：header(0x23)+UID → memset 整块 0 → _GetSaveInvenBase 填
//      header 区 → 写 m_saveFlag=0 → GetInvenStart(0/1/3) 三块背包 + inven[22..25]
//      宠物尾 → RemoveKCItem（state==10 时再 RemoveFPItem）→ cargo 容量+数据 →
//      m_characExpand.update(this,9) → removeRentalItemInfoFromInvenBuf(1/2) →
//      MsgQueueMgr::put(DB_Q,guard) → 返回 1。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864fe52 | _ZN5CUser13SaveInventoryEv
bool CUser::SaveInventory()   // b28c-L5 SIGFIX（P174）
{
    if (isLocked4DataLoad())
    {
        char* accIdStr = NumberToString(get_acc_id(), 0);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x181a, 5);
        trace("Can not save character until loading done(m_id: %s, charac_no: %d",
              accIdStr, getCurCharacNo());
        return 0;
    }
    if (G_GameWorld()->GetChannelType() == 7)
    {
        return 0;
    }

    Stream* stream =
        GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x1828);
    CStreamGuard guard(stream, true);
    // ORIG：每条 << 前 call operator*()，GetInBuffer 前 operator->()（P167 缺 3）。
    guard.operator*()->operator<<((int)0x23);          // SIG_SAVE_INVENTORY
    guard.operator*()->operator<<((int)GetUID());

    SIG_SAVE_INVENTORY* sig =
        guard.operator->()->GetInBuffer<SIG_SAVE_INVENTORY>();
    memset(sig, 0, 0xad99);
    _GetSaveInvenBase(sig);
    sig->m_saveFlag = 0;

    // P183：ORIG 每块背包独立 getCurCharacInvenR() 重物化（4 处），禁 inven 缓存。
    memcpy(sig->m_invenType0, getCurCharacInvenR()->GetInvenStart(0), 0x53e);
    memcpy(sig->m_invenType1, getCurCharacInvenR()->GetInvenStart(1), 0x4a58);
    memcpy(sig->m_invenType3, getCurCharacInvenR()->GetInvenStart(3), 0x39aa);
    // CInventory +0x55a = &m_invenItem[22]（ORIG 原始偏移；宠物尾 4 项）
    memcpy(sig->m_creatureTail,
           &getCurCharacInvenR()->m_invenItem[22], 0xf4);

    CInventory::RemoveKCItem(sig->m_invenType0, sig->m_invenType1);
    if (get_state() == 10)
    {
        CInventory::RemoveFPItem(sig->m_invenType0, sig->m_invenType1);
    }

    // P183：ORIG 两次独立 getCurCharacCargoR()（GetCapacity 一次、get_cargo_start
    // 再一次），禁 CCargo* 缓存变元。
    sig->m_cargoCapacity = getCurCharacCargoR()->GetCapacity();
    memcpy(sig->m_cargo, getCurCharacCargoR()->get_cargo_start(),
           ARAD::DATABASE::sourceCargoSize);

    m_characExpand.update(this, (ENUM_CHARAC_EXPAND_TYPE)9);
    removeRentalItemInfoFromInvenBuf(sig, 1);
    removeRentalItemInfoFromInvenBuf(sig, 2);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// ============================================================================
// _postCheckForceChangeGrowType（免费函数，TSV 修复，ORIG 0x08668823 T）。
// ORIG：遍历 QuestList hash_map<int,Quest*>（G_CDataManager()+0x18→getQuestList），
// 逐任务按 CUserCharacInfo::get_charac_job 做 checkPossibleJob，再按 quest 内
// m_fieldB8==1（比对 firstGrow == quest.m_fieldC0）/==2（checkMatchedGrowType +
// secondGrow == quest.m_fieldC0）判定，命中的任务经 _postCheckForceChangeGrowType4AheadQuest
// → CQuestClear::setClearedQuest 清任务并发包。
// 依赖 Quest/QuestList/CQuestClear 等当前未建模类型，提供可链接的签名匹配骨架（推断）。
// ============================================================================
void _postCheckForceChangeGrowType(CUser* user, int firstGrow, int secondGrow)
{
    QuestList* questList = G_CDataManager()->m_questList;   // ORIG G_CDataManager()+0x18
    __gnu_cxx::hash_map<int, Quest*>::const_iterator it =
        questList->m_questMap.begin();
    for (; it != questList->m_questMap.end(); ++it)
    {
        Quest* quest = it->second;
        // ORIG 0x8668893：checkPossibleJob 取反后为真 → 跳过
        if (!quest->checkPossibleJob((ENUM_CHARACTERJOB)user->get_charac_job()))
            continue;

        if (quest->m_fieldB8 == 1)
        {
            // +0xb8==1：+0xc0 == firstGrow 命中
            if (quest->m_fieldC0 == firstGrow)
            {
                _postCheckForceChangeGrowType4AheadQuest(user, quest);
                // ORIG 0x8668939：getCurCharacQuestW()+0x4 → CQuestClear::setClearedQuest(idx)
                ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
                    ->setClearedQuest((unsigned int)quest->m_index);
            }
        }
        else if (quest->m_fieldB8 == 2)
        {
            // +0xb8==2：checkMatchedGrowType(job, firstGrowType, -1) 且 +0xc0 == secondGrow
            char growType = user->getCurCharFirstGrowType();     // movsbl
            if (!quest->checkMatchedGrowType((ENUM_CHARACTERJOB)user->get_charac_job(),
                                             growType, -1))
                continue;
            if (quest->m_fieldC0 != secondGrow)
                continue;
            _postCheckForceChangeGrowType4AheadQuest(user, quest);
            ((WongWork::CQuestClear*)((char*)user->getCurCharacQuestW() + 4))
                ->setClearedQuest((unsigned int)quest->m_index);
        }
    }

    // ORIG 0x8668984：PacketGuard → getCurCharacQuestR()->get_quest_info(pg) → Send
    PacketGuard packet;
    user->getCurCharacQuestR()->get_quest_info(
        reinterpret_cast<char*>(&packet));                       // ORIG mangled 形参 Pc
    user->Send(packet);
}
// STGuildDBInfoOnly ctor（ORIG W 0x08693f56 sz 0x16f；无报告，objdump 转写：
// 逐字段清零 → 循环构造 16×STGuildSkill（+0x45 步长 5）→ 尾部标量清零 →
// memset 子成员 → +0x1c (x|1)&~2 → memset skills 覆盖 → memset masterName）。
// 注意 ORIG 在循环构造 STGuildSkill（-1/0xff）后又 memset(+0x45,0,0x50) 覆盖清零，
// 净效果 skills 全 0（CUser.h 冻结 m_skills 为 char[0x50]，无构造语义，直接清零）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08693f56 | _ZN17STGuildDBInfoOnlyC1Ev
STGuildDBInfoOnly::STGuildDBInfoOnly()
{
    m_masterNo = 0;                 // +0x17
    m_fundLevel = 0;                // +0x1b
    m_field1E = 0;                  // +0x1e
    m_memberCount = 0;              // +0x22
    m_guildPoint = 0;               // +0x24
    m_guildRank = 0;                // +0x28
    m_fundExp = 0;                  // +0x29
    m_subMasterCnt = 0;             // +0x2d
    m_guildSkillPoint = 0;          // +0x42
    m_skillLearnCnt = 0;            // +0x44
    // ORIG 此处循环 16× call STGuildSkill::C1Ev（+0x45 步长 5；ctor 写
    // m_field0=-1 + m_field4=0xff，即每 5 字节全 0xff）。m_skills 为 char[0x50]，
    // 无法直接 in-band ctor 调用（§8.2 冻结头），以等宽 memset 表达；随后被
    // 下方 memset 覆盖清零——该循环净效果为零，属 N 封顶的表示差异。
    for (int i = 0xf; i != -1; --i)
        memset(m_skills + i * 5, 0xff, 5);
    m_field95 = 0;                  // +0x95
    m_powerSecedeTime = 0;          // +0x96
    m_powerWarPoint = 0;            // +0x9a
    m_agitExist = 0;                // +0x9e
    m_powerJoinCount = 0;           // +0x9f
    m_fundMoney = 0;                // +0xa0
    m_fieldB9 = 0;                  // +0xb9
    memset(m_subMasterData, 0, 0x14);   // +0x2e
    memset(m_guildName, 0, 0x17);       // +0x00
    // ORIG 0x08693f56：+0x1c 为 byte 位操作（movzbl;or $1;movb / movzbl;and $~2;movb），
    // 只动低字节，不触碰 +0x1d；m_ability 是 unsigned short，需经 byte 视图改写。
    {
        unsigned char* flags = reinterpret_cast<unsigned char*>(&m_ability);
        *flags = (unsigned char)(*flags | 1);
        *flags = (unsigned char)(*flags & ~2);
    }
    memset(m_skills, 0, 0x50);          // +0x45
    memset(m_masterName, 0, 0x15);      // +0xa4
}

// ===================== U1 批次（FixUndefU1）=====================

// ORIG T 0x08679754 sz0x2e2：仅 level%10==0 写公会公告板；文案 = 角色名 +
// ServerString(0x3e9) + "%d"(level) + ServerString(0x3e8)；len>=0x78 丢弃。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08679754 | _ZN5CUser23makeGuildLevelUpMessageEi
void CUser::makeGuildLevelUpMessage(int level)
{
    if (level % 10 != 0)
        return;

    char numBuf[256];
    memset(numBuf, 0, sizeof(numBuf));

    std::string body(getCurCharacName());
    body += g_ServerString_.GetServerString(0x3e9, 0);
    sprintf(numBuf, "%d", level);
    body += numBuf;
    body += g_ServerString_.GetServerString(0x3e8, 0);
    if (body.length() >= 0x78)
        return;

    char board[120];
    memset(board, 0, sizeof(board));
    memcpy(board, body.c_str(), body.length());

    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(GetServerGroup());
    if (proxy != 0)
        proxy->SendGuildBoardWrite(board, 0, (unsigned int)getCurCharacNo(),
                                   -1, -1, (unsigned int)get_charac_guildkey());
}

// ORIG T 0x0867cd20 sz0x18d：guildkey!=0 时组 header 0x63 包广播队列 2。
// b28d-lane5 N封顶: P11 符号扩展; N82/P112 槽号分配 | ORIG 0x0867cd20 | _ZN5CUser21_UpdateUserInfo4GuildEv
void CUser::_UpdateUserInfo4Guild()
{
    if (get_charac_guildkey() == 0)
        return;

    // b28d-lane5：ORIG 0x0867cd4x 无 Stream* 槽——Acquire 结果经 %eax 直传
    // CStreamGuard ctor 实参（P0 右值内联；具名 s 反产 -0xc 槽 + 回读）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x793f), true);
    // ORIG：每处写经 guard.operator*()->operator<<(…) 显式代理链（operator* 调用点
    // 补齐；SetCharacInfo/SaveInventory 判例）。
    guard.operator*()->operator<<((int)0x63);                       // header
    guard.operator*()->operator<<((int)-1);
    guard.operator*()->operator<<((int)get_charac_guildkey());
    guard.operator*()->operator<<((unsigned int)get_acc_id());
    guard.operator*()->operator<<((int)getCurCharacNo());
    // b28d-lane5：ORIG 0x0867ce2x `movsbl %al,%ebx` ⇒ 调用点按 signed char 扩展
    // （P11；getter 本体 uchar 直返不变，仅本消费点收窄）。
    guard.operator*()->operator<<((int)(signed char)getCurCharacGrowType());
    guard.operator*()->operator<<((int)get_charac_level());
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ORIG T 0x0864c85a sz0x30b：把当前角色内存快照填入 CACHE_CHARACTER_TYPE 后交
// exchange_server 缓存。控制流/子对象偏移/错误路径逐指令核对 CacheCharacterMemory.md：
//   getCurCharacR()==0 -> false；get_state()<3 -> false；characNo=*curR(首字段)；
//   mem=exchange_server::AcquireCharacterMemory(characNo)（失败 trace + false）；
//   依次 GetRentInfo(+0x703)/GetCharacInfo(+0x0)/GetInventoryInfo(+0x703)/
//   quest(+0x15b4f)/ticket(+0x1d157)/dungeonClear(+0x1d2f3)/blacklist(+0x1eb03)/
//   actionPoint(+0x1eb13)/expand vtable 槽+8(+0x34884)/altar(+0x1f804)/etc(+0x1fd20)；
//   三个子对象内字段拷贝 + memcpy(+0x3f850, this+0x8d0fd, 0xc)；
//   CacheCharacterMemory 失败 -> FreeCharacterMemory + trace + false；成功置 lastLogin + true。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0864c85a | _ZN5CUser20CacheCharacterMemoryEv
bool CUser::CacheCharacterMemory()   // b28c-L5 SIGFIX（P174）
{
    if (getCurCharacR() == 0)
        return 0;
    if (get_state() < 3)
        return 0;

    // ORIG 0x0864c8..：characNo = *((uint*)getCurCharacR()) 直读首字段 m_characNo
    //（P167：再次 getCurCharacR，勿经 getCurCharacNo() 访问器）。
    unsigned int characNo = (unsigned int)getCurCharacR()->m_characNo;
    int characIdx = get_charac_idx(characNo);
    if (characIdx < 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x113c, 0);
        tr("get_charac_idx() return = %d", characIdx);
        return 0;
    }

    CACHE_CHARACTER_TYPE* mem = exchange_server::AcquireCharacterMemory((int)characNo);
    if (mem == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1150, 0);
        tr("AcquireCharacterMemory() return = NULL, charac_no(%d)", (int)characNo);
        return 0;
    }
    GetRentInfo(mem->m_inventory);
    GetCharacInfo(&mem->m_charac);
    GetInventoryInfo(&mem->m_inventory);
    getCurCharacQuestR()->get_quest(&mem->m_quest);
    GetTicket(reinterpret_cast<SIG_LOAD_TICKET*>(mem->m_ticket));
    GetDungeonClear(reinterpret_cast<SIG_LOAD_DUNGEON_CLEAR*>(mem->m_dungeonClear));
    GetBlackListCount(reinterpret_cast<SIG_REQ_BLACK_COUNT*>(mem->m_blackList));
    APSystem::CDBAndExchangeServerProc::GetActionPointFromMemory(
        *this, mem->m_actionPoint);
    CExpandEquipslot* expand = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xc);
    // 真实虚调用 CExpandEquipslot::getData(char*) @ vtable+8（CExpandEquipslot.h
    // 权威 slot2）。§4.5 禁 typedef 槽指针直调。
    expand->getData((char*)&mem->m_expand);
    advancealtar::DBAndExchangeServerProc::getFromMemory(
        *this, mem->m_altar);
    GetEtcInfo(&mem->m_etc);

    // 三个子对象内字段直拷（ORIG 报告偏移）
    // m+0x1fd2c=etc.m_fieldC / m+0x686=charac.m_tagCharacNo / m+0x70b=inv.m_field8
    //（均 movl 4B，CacheCharacterMemory.md 0x08670d3x 区）。
    mem->m_etc.m_fieldC = mem->m_charac.m_tagCharacNo;
    mem->m_inventory.m_field8 = mem->m_etc.m_fieldC;
    memcpy(mem->m_memberFlag, m_pad8d0fd, 0xc);

    if (exchange_server::CacheCharacterMemory((int)characNo, mem))
    {
        SetLastLoginChannelNo(G_CEnvironment()->get_channel_no());
        SetLastLoginCharacNo((int)characNo);
        return 1;
    }
    exchange_server::FreeCharacterMemory(mem);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1190, 0);
    tr("CacheCharacterMemory() return = false");
    return 0;
}

// ============================================================================
// 角色内存快照填充（CacheCharacterMemory 依赖）——ORIG 报告逐函数实现。
// ============================================================================

// ORIG 0x0864de58 _ZNK5CUser15GetDungeonClearEP22SIG_LOAD_DUNGEON_CLEAR
// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x0864de58 | _ZNK5CUser15GetDungeonClearEP22SIG_LOAD_DUNGEON_CLEAR
bool CUser::GetDungeonClear(SIG_LOAD_DUNGEON_CLEAR* sig) const
{
    sig->m_selected = 0;
    // b28d-lane5：ORIG 0x0864de5f-0x0864de6d 无 cur 槽——`call getCurCharacR;
    // test %eax,%eax; sete %al; test %al,%al; je <body>`（P197④ call 结果寄存器
    // 直比产 sete）+ trace 块 fall-through、`return false` 常量岛。具名 cur 局部
    // 反产 `mov %eax,-0xc; cmpl $0,-0xc` 槽比形（P0 槽-局部双向对应律）。
    if (getCurCharacR() == 0)
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1400, 5);
        tr("CUser::GetDungeonClear User  m_selected is null");
        return false;
    }
    getDungeonClearRefR()->toString(sig->m_clearData, 0x1800);
    return true;
}

// ORIG 0x0867f1d4 _ZNK5CUser17GetBlackListCountEP19SIG_REQ_BLACK_COUNT
int CUser::GetBlackListCount(SIG_REQ_BLACK_COUNT* sig) const
{
    sig->m_count = 0;
    // ORIG 0x867f1ef-0x867f1f4：call;mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,0x8(%eax)。
    // b28d-solo-CUser N封顶: 4.4 恒 LHS-first 直存（无 %edx 中转）；具名 bc/ps（原形）落双槽
    //   -0x10/-0xc、语句表达式形反劣（push %ebx/sub $0x14）⇒ P211 异型中转在 call 返回侧不可造。
    sig->m_blackCount = (unsigned)getCurrCharacBlackCount();
    return 1;
}

// ============================================================================
// 角色内存快照填充：GetRentInfo/GetCharacInfo/GetInventoryInfo/GetTicket（CacheCharacterMemory 依赖）
// 及 helper 链（GetPcRoomRentInfo/GetDimensionRentInfo/getCharacInfoDetail/getTagCharacInfo/
// getTagCharacInventory/GetInventory/removeRentalItemInfoFromInvenLoadBuf）。
// 具名填充 SIG_LOAD_CHARAC/INVENTORY/TICKET（SigTypes.h 权威布局）。
// ============================================================================

// ORIG T 0x086769b4 sz0x35：SIG_LOAD_INVENTORY 的租借区（pcroom@+0x153e4 /
// dimension@+0x153f0）由 GetPcRoomRentInfo/GetDimensionRentInfo 填充。
void CUser::GetRentInfo(SIG_LOAD_INVENTORY& sig)
{
    GetPcRoomRentInfo(reinterpret_cast<SIG_SAVE_PC_ROOM_RENT_ITEM*>(&sig.m_rentPcRoom));
    GetDimensionRentInfo(reinterpret_cast<SIG_SAVE_DIMENSION_RENT_ITEM*>(&sig.m_rentDimension));
}

// ORIG T 0x086769ea：PC 房间租借物品 → SIG_SAVE_PC_ROOM_RENT_ITEM（最多 2 个 0xc 条目）。
// 逐条目读取 getCurrentCharacRentalInfo(list,1)，对仍存在物品写 [0]itemIndex/[4]byte/[5]byte/
// [6]inEquip/[8]int。参数为租借清单类型 1。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x086769ea | _ZN5CUser17GetPcRoomRentInfoEP26SIG_SAVE_PC_ROOM_RENT_ITEM
int CUser::GetPcRoomRentInfo(SIG_SAVE_PC_ROOM_RENT_ITEM* sig)
{
    if (sig == 0)
        return 0;
    sig->clear();
    if (!m_rentalFlag1)   // ORIG 0x8676a10/0x8676c12 movzbl; xor $0x1,%eax; test;je（bool 取反形）
        return 1;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, 1) != 1)
        return 1;

    int index = 0;
    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        const CInventory* inven = getCurCharacInvenR();
        if (inven->CheckExistItemInInventoryOrEquipment(rental.m_itemIndex, slot, bInEquip) == 1)
        {
            sig->m_entry[index].m_itemIndex = rental.m_itemIndex;
            sig->m_entry[index].m_field4 = rental.m_field8;
            sig->m_entry[index].m_field5 = rental.m_field9;   // ORIG movzbl 0x9
            sig->m_entry[index].m_inEquip = bInEquip ? 1 : 0;
            sig->m_entry[index].m_field8 = rental.m_rentItemId;  // ORIG mov 0xc
            ++index;
            if (index > 1)
                return 0;
        }
    }
    return 1;
}

// ORIG T 0x08676bec：次元租借物品 → SIG_SAVE_DIMENSION_RENT_ITEM（最多 12 个 8 字节条目）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08676bec | _ZN5CUser20GetDimensionRentInfoEP28SIG_SAVE_DIMENSION_RENT_ITEM
int CUser::GetDimensionRentInfo(SIG_SAVE_DIMENSION_RENT_ITEM* sig)
{
    if (sig == 0)
        return 0;
    sig->clear();
    if (!m_rentalFlag1)   // ORIG 0x8676a10/0x8676c12 movzbl; xor $0x1,%eax; test;je（bool 取反形）
        return 1;

    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, 2) != 1)
        return 1;

    int index = 0;
    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        const CInventory* inven = getCurCharacInvenR();
        if (inven->CheckExistItemInInventoryOrEquipment(rental.m_itemIndex, slot, bInEquip) == 1)
        {
            sig->m_entry[index].m_itemIndex = rental.m_itemIndex;
            sig->m_entry[index].m_field4 = rental.m_field8;
            sig->m_entry[index].m_field5 = rental.m_field9;   // ORIG movzbl 0x9
            sig->m_entry[index].m_inEquip = bInEquip ? 1 : 0;
            sig->m_entry[index].m_field7 = rental.m_fieldB;   // ORIG movzbl 0xb
            ++index;
            if (index > 11)
                return 0;
        }
    }
    return 1;
}

// ORIG T 0x0864bc04 sz0x1b5：填充 SIG_LOAD_CHARAC。
//   +0x0 acc_id / +0x4 charac_no(*curR 首字段) / +0x8 result(0 初始, 失败置 1) /
//   +0xc CHARAC_DATA（getCharacInfoDetail）/ +0x686 tag_charac_no /
//   +0x68a striker_skill_index / +0x68b TAG_CHARAC_DATA（getTagCharacInfo）。
bool CUser::GetCharacInfo(SIG_LOAD_CHARAC* sig) const
{
    sig->m_accId = get_acc_id();
    // ORIG 0x0864bcc8：无条件 mov (%eax) 解引用（getCurCharacR 恒非空，无检查）
    sig->m_characNo = (unsigned int)getCurCharacR()->m_characNo;
    int characIdx = get_charac_idx(sig->m_characNo);
    sig->m_result = 0;

    // b28c-L5：清 `register bool asm("al")` 存量绑定（P0/N96）。ORIG 0x0864bdf2
    //   `call getCharacInfoDetail; xor $0x1,%eax; test %al,%al; je <正常路径>`
    //   = P168a① 「callee 返回 bool ⇒ 调用点一律裸形」（该 callee 的 ORIG 体尾
    //   仅 `mov $0x1/$0x0,%eax` 两出口 → P159 串/尾形双证 bool，声明已由 Main 代落
    //   CUser.h:3121 int→bool）。X1 约束形/物化 bool 局部均产 sete|setne 三段，
    //   唯有 bool callee + 裸 `!call()` 出单条 xor。
    // err trace 保具名（ORIG err@-0x30 远槽），主块 trace 临时链改全表达式形。
    if (!getCharacInfoDetail(characIdx, sig->m_characNo,
                             reinterpret_cast<CHARAC_DATA*>(&sig->m_characData)))
    {
        cMyTrace tr(__PRETTY_FUNCTION__, 0x1052, 0);
        tr("getCharacInfoDetail() return = false");
        sig->m_result = 1;
        return false;
    }

    int tagIdx = get_charac_idx((unsigned int)getCurCharacTagCharacNo());
    if (tagIdx >= 0 && tagIdx < (int)m_characList.size())
    {
        // b28c-L5：删 (unsigned int) cast。ORIG 0x0864bd1x `mov 0xc(%ebp),%edx;
        //   mov %eax,0x686(%edx)` = 值留 %eax、base 占 %edx 的**同型直存**形；
        //   原字段 unsigned int ← int 返回值异型使 4.4 多一段 edx 中转（P211）。
        //   字段已随 SigTypes_C.h 改回 int（广播）。
        sig->m_tagCharacNo = getCurCharacTagCharacNo();
        sig->m_strikerSkillIndex = getCurCharacStrikerSkillIndex();
    }
    getTagCharacInfo(tagIdx, sig->m_tagCharacNo,
                     reinterpret_cast<TAG_CHARAC_DATA*>(&sig->m_tagCharacData));
    {
        // ORIG 0x864bcf6/0x864bd01：两值先求值直落 callee-saved %esi/%ebx，
        // 跨 ctor/operator() 存活；主 trace 临时链（P227 全表达式形）翻转与
        // err-trace 的槽分配序（ORIG 主@-0x20/err@-0x30）。
        // b28c-L5：原 `register int …` 无 asm 绑定（合规），但 4.4 对具名
        // register 局部仍按 P0 落槽 ⇒ 保留具名局部写法（ORIG 两值确在 ctor 前
        // 物化并经 %esi/%ebx 跨 call 保活，P227 实证该形态由「实参位内联 +
        // 全表达式临时链」自然产生，无需绑定）。
        // b28c-L5 两处装载形修正：
        //   ① `mov 0x59(%eax),%esi`（ORIG 单 disp，%eax=sig）⇒ 必须走**真成员链**
        //      `sig->m_characData.m_field4d`（4.4 折 base+0xc+0x4d 为一条 disp32）；
        //      原 reinterpret_cast<CHARAC_DATA*>(&m_characData) 使 4.4 先 `add $0xc`
        //      物化子对象地址再读 +0x4d（+1 条）。
        //   ② `mov 0x4(%eax),%ebx`（ORIG 一步直落 callee-saved）⇒ 删多余 (int) cast：
        //      字段 unsigned int 而目标寄存器承载 int 时 4.4 恒插 `mov %eax,%ebx`
        //      中转（P211/N18a 族，varargs 实参位无需显式拓宽）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1066, 0)(
            "CharLastPlayTick char(%d) t(%d)", sig->m_characNo,
            sig->m_characData.m_field4d);
    }
    return true;
}

// ORIG T 0x0864d646：填充 SIG_LOAD_INVENTORY 的背包/金币/装备镜像/cargo/avatar 区。
// creature tail 视图：&m_invenType3+0x38b6 = sig+0x12787（ORIG 单条 add，4.4 对
// 成员地址+常量恒拆双 add ⇒ P82 常量视图折叠单 lea）。
namespace
{
struct SIG_LI_CreatureTailView
{
    char m_pad[0x12787];
    char m_tail[0xf4];
};
}   // namespace
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864d646 | _ZN5CUser12GetInventoryEP18SIG_LOAD_INVENTORY
bool CUser::GetInventory(SIG_LOAD_INVENTORY* sig)
{
    // P183：ORIG 每字段访问独立 getCurCharacInvenR()/getCurCharacCargoR() 重物化
    //（P167 缺 getCurCharacInvenR×11 + getCurCharacCargoR×1），禁指针缓存。
    sig->m_money = getCurCharacInvenR()->get_money();
    sig->m_coin = getCurCharacInvenR()->GetCoin();
    sig->m_payCoin = getCurCharacInvenR()->GetPayCoin();
    sig->m_eventCoin = getCurCharacInvenR()->GetEventCoin();
    getCurCharacInvenR()->GetInvenData(1, &sig->m_invenType1, 0x4a58);
    getCurCharacInvenR()->GetInvenData(2, &sig->m_invenType2, 0x1905);
    getCurCharacInvenR()->GetInvenData(3, &sig->m_invenType3, 0x39aa);
    // 装备镜像 = CInventory::m_invenItem 前 22 项（0x53e B）
    memcpy(&sig->m_equipMirror, getCurCharacInvenR()->m_invenItem, 0x53e);
    // creature tail：m_invenType3 区内 +0x38b6 跨条目 0xf4 B（源 m_invenItem[22..25]，
    // ORIG 0x0864d646 忠实保留跨对齐写）。
    memcpy(((SIG_LI_CreatureTailView*)sig)->m_tail,
           &getCurCharacInvenR()->m_invenItem[22], 0xf4);
    getCurCharacInvenR()->GetClearAvatar(sig->m_clearAvatar);   // b28d-solo-CUser：数组退化免 cast（P255/g17 形）
    sig->m_cargoCapacity = getCurCharacCargoR()->GetCapacity();   // 同型直存（GetCapacity 已 unsigned 化）
    getCurCharacCargoR()->get_cargo(&sig->m_cargo);
    sig->m_avatarCoin = getCurCharacInvenR()->m_avatarCoin.Get();   // CInventory::m_avatarCoin@+0x658
    sig->m_inventoryCapacity = getCurCharacInvenR()->get_inventory_capacity();
    return true;
}

// ORIG T 0x0864d4ba：填充 SIG_LOAD_INVENTORY（含 GetInventory + creature + avatar + tag + rent）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864d4ba | _ZN5CUser16GetInventoryInfoEP18SIG_LOAD_INVENTORY
bool CUser::GetInventoryInfo(SIG_LOAD_INVENTORY* sig)
{
    sig->m_result = 0;
    // ORIG 0x864d4ba：getData 虚调用 receiver 无槽（P0 内联右值直通
    // `mov (%eax),%edx; add $8; mov (%edx),%ecx; call *%ecx`）——具名 expand
    // 局部落 -0x14/-0x10 槽生槽化 DIFF。
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)2)->getData(
        (char*)&sig->m_locks[0]);   // 真实虚调用 getData @ vtable+8
    GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9)->getData(
        (char*)&sig->m_expandEquip);

    // X 族定稿（2026-09-05 CUserC8，判据②直分支/③消费者齐）：ORIG 0x864d534-
    // 0x864d5b3 为三段 `call; xor $0x1,%eax; test %al,%al; jne 共享FAIL岛` 的
    // **短路直落链**（无 bFailed 暂存、无 (x^1)|| 逗号物化）——ORIG xor $1（非
    // 1 语义即真）⇒ 条件用 `== 1`（uchar 提升 int 零扩展，bool 返回会多 movzbl）；
    // GetItem 两出参指针先求值缓存 %esi/%ebx（lea 在 getCurCharacInvenR 调用前）。

    // ORIG 三段均 `call; xor $1,%eax; test %al,%al; jne FAIL岛` = !boolcall()
    // 直判形（P168a①）——GetInventory/GetCreatureItem/GetItem 已 bool 化；
    // 无 bFailed 暂存槽（P198② 内联直测，帧 0x10）。
    if (!GetInventory(sig) ||
        !getCurCharacInvenR()->GetCreatureMgrR()->GetCreatureItem(sig) ||
        !(((CInventory*)getCurCharacInvenR())->GetAvatarItemMgrR()->GetItem(
              sig->m_avatarDetail,          // b28d-solo-CUser：真实类型数组退化（免 cast 出 lea）
              &sig->m_avatarCount)))
    {
        sig->m_result = 1;
        return false;   // ORIG FAIL 岛 mov$0
    }

    if (getTagCharacR() != 0)
        getTagCharacInventory(reinterpret_cast<TAG_CHARAC_INVEN_DATA*>(&sig->m_tagInven));
    removeRentalItemInfoFromInvenLoadBuf(sig, 1);
    removeRentalItemInfoFromInvenLoadBuf(sig, 2);
    sig->m_maxEquipLevel = GetCurCharacMaxEquipLevel();
    return true;   // ORIG 成功尾 mov$1
}

// ORIG T 0x0869067c：TAG 角色背包 → TAG_CHARAC_INVEN_DATA。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0869067c | _ZNK5CUser21getTagCharacInventoryEP21TAG_CHARAC_INVEN_DATA
bool CUser::getTagCharacInventory(TAG_CHARAC_INVEN_DATA* out) const
{
    // b28c-L5 判词（N30 族·lea-vs-add 单点残留）：本波已翻正两处——
    //   ① GetInvenData 的 `add $0x1c(%esp)` 出参组（原 cast 形，与 ORIG 同）；
    //   ② `lea 0x3ff8(%eax),%ebx`（把 TAG_CHARAC_INVEN_DATA::m_clearAvatar 由
    //      char[0x28] 改真实类型 unsigned long[10] 后免 cast，4.4 即折单条 lea，
    //      与 CInventory::GetClearAvatar(unsigned long*) 权威形参同型）。
    //   唯一残留：`lea 0x1e47(%eax),%ebx` vs 4.4 `add $0x1e47,%eax; mov %eax,%ebx`。
    //   同一基址的前一个取址 `lea 0x1e43(%eax),%esi` 两侧一致 ⇒ 差异由 4.4 的
    //   第二次 reload+目标寄存器选择决定（实测 `out->m_avatarDetail` 与
    //   `&out->m_avatarDetail[0]` 两写法产物相同），属 P30 尾判的
    //   「穷举后 add-vs-lea 不齐 ⇒ 结构性伪影不强求」。
    if (getTagCharacR() == 0)
        return false;
    // ORIG：getTagCharacInvenR() 返回 CInventory*（位于 _Charac_info.m_inven，本源码返回 Inven_Item* 同址）
    // P48 逐点重调（ORIG 0x86906b1/0x86906d8/0x8690708/0x8690731 共 4 次）；
    // cargo/GetItem 各 call 结果寄存器直传（无中间槽）。
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->GetInvenData(2, out->m_items2, 0x1905);
    memcpy(out->m_items,
           reinterpret_cast<const CInventory*>(getTagCharacInvenR())->m_invenItem, 0x53e);
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->GetClearAvatar(out->m_clearAvatar);   // b28d-solo-CUser：数组退化
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->GetAvatarItemMgrR()->GetItem(
        out->m_avatarDetail,   // b28d-solo-CUser：真实类型数组退化（免 cast 出 lea）
        &out->m_avatarCount);
    out->m_cargoCapacity =
        (unsigned int)getTagCharacCargoR()->GetCapacity();
    return true;
}

// ORIG T 0x0867776c：从 SIG_LOAD_INVENTORY 背包镜像清除已不存在的租借物品。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867776c | _ZN5CUser36removeRentalItemInfoFromInvenLoadBufEP18SIG_LOAD_INVENTORYi
bool CUser::removeRentalItemInfoFromInvenLoadBuf(SIG_LOAD_INVENTORY* sig, int param)  // b28c-L5 SIGFIX
{
    if (sig == 0)
        return 0;
    std::vector<InstanceRentalSystem::RentalInfo> rentalList;
    if (getCurrentCharacRentalInfo(rentalList, param) != 1)
        return 0;
    for (std::vector<InstanceRentalSystem::RentalInfo>::iterator it = rentalList.begin();
         it != rentalList.end(); ++it)
    {
        InstanceRentalSystem::RentalInfo& rental = *it;
        bool bInEquip = false;
        int slot = 0;
        const CInventory* inven = getCurCharacInvenR();
        if (inven->CheckExistItemInInventoryOrEquipment(rental.m_itemIndex, slot, bInEquip) == 1)
        {
            rental.m_inEquip = bInEquip;
            if (bInEquip)
                sig->m_equipMirror[slot].reset();
            else
                sig->m_invenType1[slot].reset();
        }
        else
        {
            LogManager::logFormat(
                1, "user.cpp",
                __PRETTY_FUNCTION__,
                0x6d54,
                "Rent error. CUser::removeRentalItemInfoFromInvenLoadBuf return false. Acc(%d), itemIndex(%d)",
                get_acc_id(), rental.m_itemIndex);
            getCurCharacInvenR()->ShowAllExistItemsForErrorCheck();
        }
    }
    setCurrentCharacRentalInfo(rentalList, param);
    return 1;
}

// ORIG T 0x0867846e sz0xb7：遍历 m_list8cf80（list<int>）票表 → SIG_LOAD_TICKET。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0867846e | _ZNK5CUser9GetTicketEP15SIG_LOAD_TICKET
void CUser::GetTicket(SIG_LOAD_TICKET* sig) const
{
    sig->m_field4 = 0;
    int index = 0;
    for (std::list<int>::const_iterator it = m_list8cf80.begin();
         it != m_list8cf80.end() && index <= 99; ++it, ++index)
    {
        sig->m_tickets[index] = *it;
    }
    sig->m_count = index;
}

// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864bdba | _ZNK5CUser19getCharacInfoDetailEijP11CHARAC_DATA
bool CUser::getCharacInfoDetail(int param_1, unsigned int characNo, CHARAC_DATA* out) const
    // b28c-L5 SIGFIX（Main 已代落 CUser.h:3121）：ORIG @0x0864bdba 全部出口只写
    //   `mov $0x1/$0x0,%eax`（P159 尾形）+ 唯一外部消费点 GetCharacInfo @0x0864bdf2
    //   `call; xor $0x1,%eax; test %al,%al; je`（P22/P168a① bool callee 直判形）。
{
    // ORIG T 0x0864bdba：op[] 每字段重调 ~64 次（4.4 O0 不 CSE call ⇒ 逐处展开
    // 同形；具名引用缓存折成单取是 gap343 主因）。字段序 = ORIG 汇编写序
    // （报告 Ghidra C 段逐项对齐；宽度 cast 均经 ORIG 指令实证 2026-09-05 CUserC8）。
    if (param_1 < 0 || param_1 >= (int)m_characList.size())
        return 0;
    if ((unsigned int)m_characList[param_1].m_characNo != characNo)
        return 0;
    out->m_characNo = m_characList[param_1].m_characNo;                                   // mov (%edx),%eax←(%eax)
    strncpy(out->m_name, m_characList[param_1].m_name, 0x1e);                             // strncpy 0x1e
    out->m_field22 = m_characList[param_1].m_field22;                                     // +0x22←+0x22 movb
    out->m_field24 = m_characList[param_1].m_field26;                                     // +0x24←+0x26 movb(job)
    out->m_field25 = (unsigned short)m_characList[param_1].m_level;                       // +0x25←+0x27 movzwl 零扩展
    out->m_field28 = m_characList[param_1].m_field2b;                                     // +0x28←+0x2b movl
    out->m_field27 = m_characList[param_1].m_field29;                                     // +0x27←+0x29 movb
    out->m_unkB2.m_fieldb2 = m_characList[param_1].m_stamina;                             // +0xb2←+0x75 movb
    memcpy(&out->m_add, &m_characList[param_1].m_field88, 0x52);                          // +0x60←+0x88 0x52B
    out->m_field2c = m_characList[param_1].m_field33;
    out->m_field2e = m_characList[param_1].m_field35;
    out->m_field30 = m_characList[param_1].m_field37;
    out->m_field32 = m_characList[param_1].m_field4f;
    out->m_field4d = m_characList[param_1].m_field7b;                                     // +0x4d←+0x7b movl
    out->m_unkB2.m_field118 = m_characList[param_1].m_field7f;                            // +0x118←+0x7f movl
    out->m_guildId = m_characList[param_1].m_guildId;                                     // +0x56←+0xdbe movl
    out->m_field5a = m_characList[param_1].m_fielddc2;                                    // +0x5a←+0xdc2 movb
    out->m_field5b = m_characList[param_1].m_field77;
    out->m_field5d = m_characList[param_1].m_field79;
    out->m_field5f = (m_characList[param_1].m_fielde22 != 0) ? 1 : 0;                     // +0x5f←(e22!=0)
    ((const CommonTime*)m_characList[param_1].m_delReqTime)->GetCreateTime(out->m_field34); // +0x39→+0x34
    out->m_unkB2.m_fieldb3 = m_characList[param_1].m_field4d;                             // +0xb3←+0x4d 2B
    out->m_unkB2.m_fieldb5 = m_characList[param_1].m_field51;                             // +0xb5←+0x51 movl
    memcpy(out->m_unkB2.m_fieldb9, m_characList[param_1].m_tradeRestrictData, 0x10);      // +0xb9←+0x55 0x10B
    out->m_unkB2.m_fieldfe = m_characList[param_1].m_resetDwordDf4;                       // +0xfe←+0xdf4
    out->m_unkB2.m_field102 = m_characList[param_1].m_resetDwordDf8;                      // +0x102←+0xdf8
    out->m_unkB2.m_fieldc9 =
        ((const CReliablePerson*)m_characList[param_1].m_reliablePerson)->GetTradeGoldTotal();
    out->m_unkB2.m_fieldcd =
        ((const CReliablePerson*)m_characList[param_1].m_reliablePerson)->GetTradeGoldTotalBillion();
    out->m_unkB2.m_fieldd1 =
        ((const CReliablePerson*)m_characList[param_1].m_reliablePerson)->GetTradeGoldDaily();
    out->m_unkB2.m_fieldd5 = m_characList[param_1].m_fielde49;
    out->m_unkB2.m_fieldd9 = m_characList[param_1].m_fielde4d;
    out->m_unkB2.m_fielddd = m_characList[param_1].m_fielde51;
    out->m_unkB2.m_fielde1 = m_characList[param_1].m_fielde55;
    out->m_unkB2.m_fielde5 = m_characList[param_1].m_fielde59;
    out->m_unkB2.m_fielde9 = m_characList[param_1].m_fielde5d;
    out->m_unkB2.m_fielded = m_characList[param_1].m_fielde45;
    out->m_unkB2.m_fieldf1 = m_characList[param_1].m_fielde68;
    out->m_unkB2.m_fieldf5 = m_characList[param_1].m_fielde70;                            // movb
    out->m_unkB2.m_fieldf6 = m_characList[param_1].m_fielde71;                            // movl
    out->m_unkB2.m_fieldfa = m_characList[param_1].m_fielde79;
    out->m_unkB2.m_field10a = m_characList[param_1].m_fielde81;                           // ORIG 序 10a 先于 106
    out->m_unkB2.m_field106 = m_characList[param_1].m_fielde7d;
    out->m_unkB2.m_i134 = (int)get_acc_id();                           // +0x134←get_acc_id
    out->m_unkB2.m_field10e = m_characList[param_1].m_field121f;                          // 2B
    out->m_field23 = m_characList[param_1].m_field25;                                     // +0x23←+0x25 movb（ORIG 尾段）
    out->m_unkB2.m_field110 = m_characList[param_1].m_field1221;                          // movl
    out->m_unkB2.m_field114 = m_characList[param_1].m_field1225;                          // movl
    out->m_unkB2.m_field11c = (unsigned short)m_characList[param_1].m_field122C;          // movzwl 122c
    out->m_unkB2.m_field11e = m_characList[param_1].m_field1239;
    out->m_unkB2.m_field11f = m_characList[param_1].m_field123e;
    out->m_unkB2.m_field120 = m_characList[param_1].m_field123f;
    out->m_unkB2.m_field121 = (unsigned char)m_characList[param_1].m_field1241;
    out->m_unkB2.m_field122 = m_characList[param_1].m_field126c;                          // 2B
    out->m_unkB2.m_b124 = (char)m_characList[param_1].m_field126e;
    out->m_unkB2.m_i125 = m_characList[param_1].m_field126f;                              // movl
    out->m_unkB2.m_b129 = (char)m_characList[param_1].m_field1275;
    out->m_unkB2.m_b12a = (char)m_characList[param_1].m_field1276;
    out->m_unkB2.m_b12b = (char)m_characList[param_1].m_field1277;
    out->m_unkB2.m_b12d = (char)m_characList[param_1].m_field127a;                        // ORIG 序 12d 先于 12c
    out->m_unkB2.m_b12c = (char)m_characList[param_1].m_field1278;
    out->m_unkB2.m_b12e = m_characList[param_1].m_tagVisibleFlags;                        // movzbl e40
    out->m_unkB2.m_b12f = (char)m_characList[param_1].m_field1280;
    out->m_unkB2.m_i130 = m_characList[param_1].m_field12ad;                              // movl
    return 1;
}

// ORIG T 0x086904f0：从 m_characList[param_1] 填充 TAG_CHARAC_DATA（SIG_LOAD_CHARAC +0x68b 区）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086904f0 | _ZNK5CUser16getTagCharacInfoEijP15TAG_CHARAC_DATA
int CUser::getTagCharacInfo(int param_1, unsigned int characNo, TAG_CHARAC_DATA* out) const
{
    if (param_1 < 0 || param_1 >= (int)m_characList.size())
        return 0;
    const _Charac_info& src = m_characList[param_1];
    if ((unsigned int)src.m_characNo != characNo)
        return 0;
    out->m_characNo = src.m_characNo;
    strncpy(out->m_name, src.m_name, 0x1d);
    out->m_field21 = src.m_field26;                   // job（ORIG movzbl 读）
    out->m_field22 = (unsigned short)src.m_level;     // ORIG movzwl 0x27（零扩展实证）
    out->m_field24 = src.m_field29;
    out->m_field25 = src.m_field1239;
    memcpy(&out->m_add, &src.m_field88, 0x52);
    return 1;
}

// ORIG T 0x0864d646：CInventory/CCargo 序列化 helper（供 GetInventoryInfo）。

// ORIG 0x08677dd8 _ZNK5CUser9GetRecordEP12SIG_LOAD_ETC
// 遍历 getBattleRecordRefR() 的 map<int,GameResultType>，每元素 0x50 字节序列化到
// sig+0x3c8+idx*0x50（最多 100 个），末尾写 count 到 sig+0x3c4。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08677dd8 | _ZNK5CUser9GetRecordEP12SIG_LOAD_ETC
void CUser::GetRecord(SIG_LOAD_ETC* sig) const
{
    const std::map<int, GameResultType>* rec = getBattleRecordRefR();
    int index = 0;
    for (std::map<int, GameResultType>::const_iterator it = rec->begin();
         it != rec->end() && index <= 99; ++it, ++index)
    {
        memcpy(&sig->m_records[index], &it->second, 0x50);
    }
    sig->m_recordCount = (int)rec->size();
}

// ORIG 0x08688dc4 _ZNK5CUser17GetLimitItemUsageEP12SIG_LOAD_ETC
// 遍历 m_limitItemUsage 向量，每元素序列化到 sig+(i+0x263)*0x10 的 0x10 字节区。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08688dc4 | _ZNK5CUser17GetLimitItemUsageEP12SIG_LOAD_ETC
void CUser::GetLimitItemUsage(SIG_LOAD_ETC* sig) const
{
    if (sig == 0) return;
    sig->m_limitItemCount = 0;   // ORIG movl $0,0x2638
    int index = 0;
    for (std::vector<LimitItemUsage>::const_iterator it = m_limitItemUsage.begin();
         it != m_limitItemUsage.end(); ++it, ++index)
    {
        SIG_ETC_LIMIT_ITEM* dst = &sig->m_limitItems[index];
        dst->m_field8 = it->m_field15;    // ORIG mov 0x14(dst)
        dst->m_fieldc = it->m_field19;    // ORIG mov 0x18(dst)
        dst->m_itemIdx = it->m_fieldc;    // ORIG mov 0x10(dst)
        dst->m_used = it->m_field10;      // ORIG movb 0xc(dst)
    }
    sig->m_limitItemCount = index;
}

// ORIG 0x086910a6 _ZNK5CUser27GetCharacCashAttendanceInfoER20SIG_ATTENDANCE_EVENT
void CUser::GetCharacCashAttendanceInfo(SIG_ATTENDANCE_EVENT& info) const
{
    *(char*)&info = 0;
}

// ---- 四个 CUser 字段访问器（CacheCharacterMemory 域；ORIG W 弱符号）----
int CUser::getTotalPcRoomPlayTime() const            // 0x08110e10：*(int*)(this+0x8eba0)
{
    return m_totalPcRoomPlayTime;
}
unsigned int CUser::getPcRoomPlayTimeRewardCnt() const   // 0x08110dec：单 movzbl 0x8eba4(%eax)；b28d-Main 改 unsigned int（P248档①，见 CUser.h 注）
{
    return m_pcRoomPlayTimeRewardCnt;
}
int CUser::GetCurCharacUsedGiftFatigueQuantity() const   // 0x08110e6c：*(int*)(this+0x8ebac)
{
    return m_field8ebac;
}
unsigned char CUser::getFlagPresentAvengerTitle() const  // 0x08697578：*(uchar*)(this+0x8d246)
{
    return m_ceraUserInfo.m_field62;
}

// ORIG 0x0868ddc2 _ZNK5CUser19GetAuraAvatarOptionER18stAuraAvatarOption
// 读当前角色 _Charac_info+0x1281..+0x1289 三 int → option。ORIG 无 cur 局部：
// getCurCharacR() 判定(0x868ddce)与拷贝(0x868dde2)各重调一次（P48，调用点差集归零）。
// 记 N（跨版本 regmove，N18a 族）：4.1 O0 将 option 基址驻 %edx 单装载 +
// `0x1281(%eax)` 索引三连；4.4 O0 对引用形参逐条重装载，register-asm 绑定反产
// 冗余 regmove（probe 四形 21/23/26/28/30 均不达 24）——语义+调用点全对，残 +4。
void CUser::GetAuraAvatarOption(stAuraAvatarOption& option) const
{
    // b28c-L5：清 `register const _Charac_info* asm("eax")` 存量绑定（P0/N96）。
    //   判词（布局视图缺口，非源形问题）：ORIG 拷贝段是
    //     `mov 0xc(%ebp),%edx`（**目标** option 基址驻 %edx）+
    //     `mov 0x1281(%eax),%ecx …`（**源**以 this 相对 disp32 直读）的 12 字节
    //   POD 结构整体赋值形。当前 `_Charac_info` 只有 `struct{int×3}`/`int[3]`
    //   两视图、无 stAuraAvatarOption 成员 ⇒ 任何写法都只能先 `add $0x1281`
    //   物化源地址（4.4 把源基址留给 callee-saved、目标挤到 %eax，装载方向反）。
    //   已按 §5.3 申请在 CUser.h:692 union 内补具名结构视图（Main 已代落
    //   `stAuraAvatarOption m_auraAvatarOption;`，+0x1281，0xc 字节，零布局影响）。
    // ORIG 0x0868ddc2 拷贝段 =
    //   `mov 0xc(%ebp),%edx / mov 0x1281(%eax),%ecx / mov %ecx,(%edx) /
    //    mov 0x1285(%eax),%ecx / mov %ecx,0x4(%edx) / mov 0x1289(%eax),%eax /
    //    mov %eax,0x8(%edx)`
    // —— 目标基址**单装载驻 %edx** + 源基址驻 %eax 连续三 dword，正是 12 字节
    // POD 结构整体赋值的 4.1 展开形（P83：结构整体赋值 vs 逐字段/ memcpy 三形互斥；
    // 逐字段写 4.4 恒每字段重装载两侧基址 ⇒ 多 4 条，原绑定期靠钉 %eax 勉强凑）。
    // 源子对象已由 CUserCharacInfo.h:499 union 具名建模（m_auraAvatarOption @+0x1281）
    // ⇒ 用具名结构成员整体拷贝，无需指针缓存亦无裸偏移（§4.3）。
    if (getCurCharacR() == 0)
        return;
    option = getCurCharacR()->m_auraAvatarOption;   // +0x1281 具名结构整体拷贝
}

// ORIG 0x0868f03c _ZNK5CUser18GetCurCharacOptionER15CharacterOption
// memcpy 0x200 字节自 _Charac_info+0x12b7（CharacterOption = char[0x200]）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868f03c | _ZNK5CUser18GetCurCharacOptionER15CharacterOption
void CUser::GetCurCharacOption(CharacterOption& option) const
{
    const _Charac_info* cur = getCurCharacR();
    if (cur != 0)
    {
        memcpy(&option, cur->m_data12B7, sizeof(CharacterOption));
    }
}

// ORIG 0x0864dcf2 _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0864dcf2 | _ZNK5CUser8GetSkillEP14SIG_LOAD_SKILLP15SIG_COMBO_SKILL
bool CUser::GetSkill(SIG_LOAD_SKILL* skill, SIG_COMBO_SKILL* combo) const
{
    // b28c-L5 判词（N30/N18a 族·lea-vs-add 跨版本折叠差，语义零差）：ORIG 对
    //   `&skill->m_remainSpTree{1,2}/m_remainSfpTree{1,2}` 三个 callee-saved 实参
    //   恒走「地址算术在 %eax 上做 → 再 mov 到 %edi/%esi/%ebx」两条
    //   （`mov 0xc(%ebp),%eax; add $0x340,%eax; mov %eax,%edi`），而 4.4-O0 折叠为
    //   单条 `lea 0x340(%eax),%edi`（我方 -3 insn）。同函数内**非 callee-saved**
    //   的那处 `mov 0xc(%ebp),%eax; add $0x1a8,%eax`（memcpy 目标）两侧完全一致，
    //   证明差异只在「目标寄存器是否 callee-saved」这一 lea 折叠决策上，与源形无关
    //   （cast/具名局部/语句表达式三形实测均维持 lea 单条）⇒ P30 尾判：
    //   「穷举后 add-vs-lea 与寄存器名仍不齐 → 结构性伪影不强求」。
    //   本波已翻正项：get_skill_command_buf 的具名结果指针 + 判空（原缺，
    //   ORIG `mov %eax,-0x1c(%ebp); cmpl $0x0,-0x1c(%ebp); je` 实证 P0 有槽往返）
    //   ⇒ 同时把 cmds 数组顶到 ORIG 的 -0xe4（原 -0xe0 帧差 4 即此槽缺失所致）。
    skill->m_field04 = 0;
    // ORIG：getCurCharacSkillR 在 get_remain_sp/get_skill_slot/getComboSkillSaveData
    // 三处各自重调——访问器结果禁缓存（战术 P183/P48）。
    ((SkillSlot*)getCurCharacSkillR())
        ->get_remain_sp(&skill->m_remainSpTree1, &skill->m_remainSpTree2,
                        &skill->m_remainSfpTree1, &skill->m_remainSfpTree2);
    ((SkillSlot*)getCurCharacSkillR())
        ->get_skill_slot((char*)skill->m_skillSlotTree1,
                         (char*)skill->m_skillSlotTree2, 0x198);
    STSkillCommandData::STCommand cmds[50];
    memset(cmds, 0, sizeof(cmds));
    // b28c-L5：ORIG 0x0864ddxx 在 call 之后另有一次「cmds 基址落槽 + 判空」：
    //   `call get_skill_command_buf / lea -0xe4(%ebp),%eax / mov %eax,-0x1c(%ebp) /
    //    cmpl $0x0,-0x1c(%ebp) / je +0x127 / …mov -0x1c(%ebp),%eax → memcpy 源`
    //   ⇒ 原码有具名指针局部（P0：该值有槽往返），且声明点在 call **之后**
    //   （4.1 初始化前置族 N88 的反向：此处落槽动作确实发生在 call 之后）。
    //   该具名槽同时把 cmds 数组顶到 -0xe4（我方原形 -0xe0 的 4 字节差即此槽缺失，
    //   非 N95 帧基差）。memcpy 源改用该指针（ORIG 从 -0x1c 回读）。
    get_skill_command_buf(cmds);
    STSkillCommandData::STCommand* pCmdBuf = cmds;
    if (pCmdBuf != 0)
    {
        memcpy(skill->m_commands, pCmdBuf, sizeof(cmds));
    }
    // ORIG 0x0864de1e `movzbl 0xa850(%eax)`（unsigned char 宽实证）
    skill->m_field411 = G_CDataManager()->m_pvpSkillTree.m_field50;
    ((SkillSlot*)getCurCharacSkillR())->getComboSkillSaveData(combo);
    return 1;
}

// ORIG 0x08654768 _ZNK5CUser21get_skill_command_bufEPN18STSkillCommandData9STCommandE
// 将 CUserCharacInfo 角色内存 +0x1254 处（getSkillCommand()）的 skill command map
// （std::map<int,STSkillCommandData>）逐项压缩为 STCommand（32 位位域）写入 out[]，最多 50 项。
// map 为空指针时 out 不变；返回 out（ORIG 返回未定义值，调用方 GetSkill 忽略返回值）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配（槽 -0x10/-0x14/-0x18 与中转寄存器差，语义/调用点全同）| ORIG 0x0865459a | _ZNK5CUser21get_skill_command_bufEPN18STSkillCommandData9STCommandE
STSkillCommandData::STCommand* CUser::get_skill_command_buf(STSkillCommandData::STCommand* out) const
{
    std::map<int, STSkillCommandData>* pMap =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(const_cast<char*>(getSkillCommand()));
    if (pMap == 0)
        return out;
    int idx = 0;
    for (std::map<int, STSkillCommandData>::iterator it = pMap->begin();
         it != pMap->end() && idx <= 0x31; ++it, ++idx)
    {
        const STSkillCommandData& v = it->second;
        STSkillCommandData::STCommand cmd;
        cmd.m_command = v.m_command;   // bits 0-13（& 0x3fff）
        cmd.m_count   = v.m_count;     // bits 29-31
        cmd.m_key0    = v.m_key0;      // bits 14-16
        cmd.m_key1    = v.m_key1;      // bits 17-19
        cmd.m_key2    = v.m_key2;      // bits 20-22
        cmd.m_key3    = v.m_key3;      // bits 23-25
        cmd.m_key4    = v.m_key4;      // bits 26-28
        out[idx] = cmd;
    }
    return out;
}

// ORIG 0x0865459a _ZN5CUser21get_skill_command_bufEv
// 同上，但写入成员 m_skillCommands[50]（+0x703db）并返回其指针；map 为空指针返回 0。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配（槽 -0x10/-0x14/-0x18 与中转寄存器差）| ORIG 0x0865459a | _ZN5CUser21get_skill_command_bufEv
STSkillCommandData::STCommand* CUser::get_skill_command_buf()
{
    std::map<int, STSkillCommandData>* pMap =
        reinterpret_cast<std::map<int, STSkillCommandData>*>(const_cast<char*>(getSkillCommand()));
    if (pMap == 0)
        return 0;
    int idx = 0;
    for (std::map<int, STSkillCommandData>::iterator it = pMap->begin();
         it != pMap->end() && idx <= 0x31; ++it, ++idx)
    {
        const STSkillCommandData& v = it->second;
        STSkillCommandData::STCommand cmd;
        cmd.m_command = v.m_command;   // bits 0-13（& 0x3fff）
        cmd.m_count   = v.m_count;     // bits 29-31
        cmd.m_key0    = v.m_key0;      // bits 14-16
        cmd.m_key1    = v.m_key1;      // bits 17-19
        cmd.m_key2    = v.m_key2;      // bits 20-22
        cmd.m_key3    = v.m_key3;      // bits 23-25
        cmd.m_key4    = v.m_key4;      // bits 26-28
        m_skillCommands[idx] = cmd;
    }
    return m_skillCommands;
}

// ORIG 0x0867794c _ZNK5CUser10GetEtcInfoEP12SIG_LOAD_ETC
// 依 SIG_LOAD_ETC 具名布局填充。expand 子区经 CExpandEquipslot vtable 槽 +8（getData）
// 直调（同 CacheCharacterMemory 的 0xc 槽位写法）；混合区内子对象用文档化偏移定位。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867794c | _ZNK5CUser10GetEtcInfoEP12SIG_LOAD_ETC
int CUser::GetEtcInfo(SIG_LOAD_ETC* sig) const
{
    sig->m_field8 = 0;
    // ORIG 0x08677979：PvpResultType::Get(source, dest) 成员直调，非裸 memcpy。
    getPVPResultR()->Get(sig->m_pvpResult);
    GetRecord(sig);
    GetContinuousItemInfo(sig);
    GetLimitItemUsage(sig);
    sig->m_oneDayLetheFlag = GetOneDayLetheFlag();
    memcpy(sig->m_backupSkill, GetCurCharacBackupSkill(), 0x199);
    sig->m_oneDayLetheFlag2ND = GetOneDayLetheFlag2ND();
    memcpy(sig->m_backupSkill2ND, GetCurCharacBackupSkill2ND(), 0x199);
    sig->m_curConditionEventStep = GetCurConditionEventStep();
    sig->m_curConditionEventRewardStep = GetCurConditionEventRewardStep();
    sig->m_properDungeonClearCount = GetProperDungeonClearCount();

    // expand type 6 → +0x40ce（真实虚调用 getData @ vtable+8）
    CExpandEquipslot* e6 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)6);
    e6->getData((char*)sig->m_expandData6);

    sig->m_curVillageAttackCount = GetCurVillageAttackCount();
    sig->m_curRevengeDungeonCount = GetCurRevengeDungeonCount();
    sig->m_flagPresentAvengerTitle = getFlagPresentAvengerTitle();
    sig->m_totalPcRoomPlayTime = getTotalPcRoomPlayTime();
    sig->m_pcRoomPlayTimeRewardCnt = getPcRoomPlayTimeRewardCnt();
    sig->m_curCharacUsedGiftFatigueQuantity = GetCurCharacUsedGiftFatigueQuantity();
    // ORIG 调 std::bitset<32>::set()（无参，全部 32 位置 1 → 0xffffffff）；
    // 直接赋值会经 bitset(unsigned long) ctor（EXTRA）而非 set()。
    sig->m_bitset.set();

    // skill 区（+0xc9c0）+ combo（+0x14aaa）
    GetSkill((SIG_LOAD_SKILL*)sig->m_mixedRegion, (SIG_COMBO_SKILL*)sig->m_comboSkill);

    // 混合区内 expand 子区（m_mixedRegion 具名成员 + 区内文档化下标）。
    // ORIG 顺序：type 7/0xd/8/0xb 无条件；tag 角色存在时只填充 tag 角色技能槽，
    // 其余 type 0xa/0x10/0xe/0xf/0x11/0x12、aura、option、attendance、
    // blueMarble、type 0x13 均无条件执行。
    CExpandEquipslot* e7 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)7);
    e7->getData(&sig->m_mixedRegion[0xcf1f - 0xc9c0]);
    CExpandEquipslot* ed = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xd);
    ed->getData(&sig->m_mixedRegion[0xd718 - 0xc9c0]);
    CExpandEquipslot* e8 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)8);
    e8->getData(&sig->m_mixedRegion[0xcdd5 - 0xc9c0]);
    CExpandEquipslot* eb = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xb);
    eb->getData(&sig->m_mixedRegion[0xd5a7 - 0xc9c0]);

    // tag 角色技能槽（+0x10/+0x1a8）
    if (getTagCharacR() != 0)
    {
        // SIG_LOAD_ETC 新结构：tag 区为 TAG_CHARAC_ETC_DATA（2 行 × 0x198）。
        ((SkillSlot*)getTagCharacSkillR())->get_skill_slot(
            reinterpret_cast<char*>(sig->m_tagEtc.m_skillSlot),
            reinterpret_cast<char*>(sig->m_tagEtc.m_skillCombo), 0x198);
    }

    // ORIG 0x0867794c：expand type 0xa → mix+0xd6fc（0xd6fc - 0xc9c0）。
    CExpandEquipslot* ea = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xa);
    ea->getData(&sig->m_mixedRegion[0xd6fc - 0xc9c0]);
    CExpandEquipslot* e10 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x10);
    e10->getData(&sig->m_mixedRegion[0xd73c - 0xc9c0]);
    CExpandEquipslot* ee = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xe);
    ee->getData((char*)sig->m_expandDataE);
    CExpandEquipslot* ef = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0xf);
    ef->getData((char*)sig->m_expandDataF);
    CExpandEquipslot* e11 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x11);
    e11->getData(&sig->m_mixedRegion[0xecc8 - 0xc9c0]);
    CExpandEquipslot* e12 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x12);
    e12->getData(&sig->m_mixedRegion[0xecd0 - 0xc9c0]);

    // aura（+0xd730）/ character option（+0xeac8）在混合区内
    GetAuraAvatarOption(
        *(stAuraAvatarOption*)&sig->m_mixedRegion[0xd730 - 0xc9c0]);
    GetCurCharacOption(
        *(CharacterOption*)&sig->m_mixedRegion[0xeac8 - 0xc9c0]);

    // attendance（+0x14a99）+ blueMarble（+0x14a98）+ expand 0x13（+0x14a9a）
    GetCharacCashAttendanceInfo(*(SIG_ATTENDANCE_EVENT*)&sig->m_attendanceEvent);
    sig->m_blueMarbleEnterCount = getBlueMarbleEnterCount();
    CExpandEquipslot* e13 = GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)0x13);
    // e13 区 = attendanceEvent 内 +1 字节起（0x14a99 起 0x11 字节，0x14a9a=+1）。
    e13->getData((char*)&sig->m_attendanceEvent + 1);

    return 1;
}

// ORIG T 0x08650aa2 sz0x87f：SIG_SAVE_CHARAC(0x10d) 组包；四个脏块分支后投递
// 队列 2 并清 save 标志。个别字段写入为报告偏移直写（推断标注见注释）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08650aa2 | _ZN5CUser15_SaveCharacDataEv
void CUser::_SaveCharacData()
{
    SetAccountLastPlayTime(GlobalData::s_systemTime_.getCurSec());

    if (getCurCharacVill() == 7)   // power war 村
    {
        setCurCharacLastPlayTickPowerWar(GlobalData::s_systemTime_.getCurSec());
        SetCurCharacChaosDieTime(0);
        m_characterView.enableSaveCharacView();
    }
    setCurCharacLastPlayTick(GlobalData::s_systemTime_.getCurSec());

    if (!isSaveCharacInfo() && !IsSaveCharacStat() && !isSaveCharacEtc() &&
        !IsSaveCharacManageInfo())
        return;

    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x19c3);
    CStreamGuard guard(s, true);
    guard << (int)0x22;                      // SIG_SAVE_CHARAC
    guard << (int)GetUID();
    SIG_SAVE_CHARAC* sig = guard.GetInBuffer<SIG_SAVE_CHARAC>();
    memset(sig, 0, sizeof(SIG_SAVE_CHARAC));

    sig->m_dungeonGainGold = getDungeonGainedGold()->getGainedGold();
    sig->m_ssoId = (unsigned int)get_acc_id();
    sig->m_reportCnt = (unsigned int)m_field8d110;   /* ORIG this+0x8d110 实证 */
    sig->m_ssoTradeGoldDaily = GetOverTradeGoldCount();
    sig->m_rating = (float)GetRating();              // ORIG fstps 0xf6（4B float 实证）
    sig->m_tutorialSkipable = (unsigned char)GetTutorialSkipable();
    sig->m_characNo = (unsigned int)getCurCharacNo();

    sig->m_bSaveInfo = (unsigned char)isSaveCharacInfo();
    sig->m_bSaveStat = (unsigned char)IsSaveCharacStat();
    sig->m_bSaveInfoSub = (unsigned char)isSaveCharacEtc();
    sig->m_bSaveManageInfo = (unsigned char)IsSaveCharacManageInfo();
    sig->m_bSaveCubePremiumInfo = (unsigned char)isSavedPremiumCubeInfo();
    if (sig->m_bSaveCubePremiumInfo)
    {
        // ORIG 0x08650d30/0x08650d39 lea 直传包内字段地址（0x10b/0x10c）。
        getContractOfCubePremiumInfo(sig->m_cubeSelected, sig->m_cubeType);
    }
    if (sig->m_bSaveInfo)   // info 块
    {
        sig->m_lev = get_charac_level();             // ORIG mov %dx（2B）
        sig->m_growType = (char)getCurCharacGrowType();
        // ORIG 0x08650d8c memcpy 0x52 → sig+0xb ⇔ _Additioal_info 块（源 _Charac_info+0x88）
        memcpy(&sig->m_maxHP, &getCurCharacR()->m_field88, 0x52);
        sig->m_expertJob = (char)GetCurCharacExpertJobType();
    }
    if (sig->m_bSaveStat)   // stat 块
    {
        sig->m_exp = getCurCharacExp();
        sig->m_village = (char)getCurCharacVill4Save();
        sig->m_fatigue = (short)GetFatigue();        // ORIG mov %ax（2B）
        sig->m_premiumFatigue = getCurCharacPremiumFatigue();
        sig->m_hp = (unsigned char)getCurCharacStamina();
        sig->m_usedFatigue = getCurCharacUsedFatigue();
        sig->m_tutorialFlag = getCurCharacTutorialFlag();
        if (getCurCharacR() != 0)
            memcpy(sig->m_escaladeTutorialFlag, getCurCharacEscaladeTutorialFlag(), 0x10);
        sig->m_tradeGoldTotal = getCurCharacTradeGoldTotal();
        sig->m_tradeGoldTotalBillion = getCurCharacTradeGoldTotalBillion();
        sig->m_tradeGoldDaily = getCurCharacTradeGoldDaily();
        sig->m_helpAbuseRatio = getCurCharacDungeonHelpAbuseRatio();
        sig->m_helpAbuseExp = getCurCharacDungeonHelpAbuseExp();
        sig->m_chaosPoint = GetCurCharacChaosPoint();
        sig->m_chaosExp = GetCurCharacChaosExp();
        sig->m_chaosModeCount = GetCurCharacChaosModeCount();
        sig->m_chaosKillCount = GetCurCharacChaosKillCount();
        sig->m_chaosDieCount = GetCurCharacChaosDieCount();
        sig->m_chaosDieTime = GetCurCharacChaosDieTime();
        sig->m_openFlag = (char)GetOpencoin();
        sig->m_assaultCount = GetAssaultCount();
        sig->m_chaosKillTime = GetCurCharacChaosKillTime();
        sig->m_luckPoint = GetCurCharacLuckPoint();
        sig->m_dungeonPlayCount = GetCurCharacDungeonPlayCount();
        sig->m_expertJobExp = GetCurCharacExpertJobExp();
        sig->m_fatigueBatteryCharging = (short)GetFatigueBatteryCharging();
        sig->m_powerWarPoint = (short)GetUserPowerWarPoint();
        sig->m_villagePrev = (char)getCurCharacVillPrev();
        sig->m_powerWarAssaultCount = GetPowerWarAssaultCount();
        sig->m_powerWarAssaultVictoryCount = GetPowerWarAssaultVictoryCount();
        sig->m_fatigueGrownupBuff = getCurCharacFatigueGrownUpBuff();
        sig->m_growthPowerReward = (unsigned char)get_growth_power_reward();
        sig->m_memberDungeonFlag = (unsigned char)getMemberDungeonFlag();
        sig->m_memberBonusFatigue = (short)getCurCharacMemberBonusFatigue();
        sig->m_lastPlayDungeonIndex = getCurCharacLastPlayDungeonIndex();
        sig->m_chaosResponTime = GetCurCharacChaosResponTime();
        sig->m_addEquipslotFlag = (char)getCurEquipslotFlag();
        sig->m_channelEquipslotSwitch = (char)getCurChannelEquipslotSwitch();
        sig->m_expandEquipslotSwitch = (char)getCurExpandEquipslotSwitch();
        sig->m_lastPlayTimePowerwar = getCurCharacLastPlayTickPowerWar();
        sig->m_emotion = (short)GetCurCharacEmotion();
        sig->m_skillTreeIndex = (char)GetCurCharacSkillTreeIndex();
        sig->m_addSlotFlag = (char)GetCurExEquipSlotStat();
        getCurCharacInvenW()->GetAvatarItemMgrW()->SaveEndurance();
    }
    if (sig->m_bSaveInfoSub)   // etc 块
    {
        sig->m_guildRight = (unsigned char)get_guild_create_right();
        sig->m_maxPremiumFatigue = getCurCharacMaxPremiumFatigue();
        sig->m_eventCharacLevel = (char)getCurCharacEventCharacterGrowtype();
    }
    if (sig->m_bSaveManageInfo)   // manage info 块
    {
        sig->m_strikerSkillIndex = (unsigned char)getCurCharacStrikerSkillIndex();
        sig->m_tagCharacNo = getCurCharacTagCharacNo();
        sig->m_maxEquipLevel = GetCurCharacMaxEquipLevel();
    }

    sig->m_lastPlayTime = getCurCharacLastPlayTick();
    sig->m_visibleFlags = (unsigned char)get_charac_visible_values(); /*推断*/
    sig->m_visibleFlags &= (unsigned char)~0x01;  // setBit_Byte(b,false,0)

    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    disableSaveCharacInfo();
    disableSaveCharacStat();
    disableSaveCharacEtc();
    disableSaveCharacManageInfo();
    disableSavedPremiumCubeInfo();
}

// ORIG T 0x0864d84e sz0x2a：param!=0 时 push_back 到受限商品列表(+0x711ec)。
void CUser::SetRestrictedGoods(int itemIdx)
{
    if (itemIdx != 0)
    {
        m_list711ec.push_back(itemIdx);
    }
    return;
}

// ORIG T 0x0864d878 sz0x91：线性查找列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864d878 | _ZN5CUser17IsRestrictedGoodsEi
bool CUser::IsRestrictedGoods(int itemIdx)
{
    if (itemIdx == 0)
        return false;
    // b28d-solo-CUser N封顶(N86/N89): ORIG 每轮重算 this+0x711ec 且 begin/end 各占独立
    //   sret 临时槽 -0xc/-0x10；4.4 缓存基址复用 -0x14 槽（非 const iterator 形实测
    //   ndiff 不变 20，回退）。
    for (std::list<int>::const_iterator it = m_list711ec.begin();
         it != m_list711ec.end(); ++it)
    {
        if (*it == itemIdx)
            return true;
    }
    return false;
}

// ORIG T 0x0864d9a4 sz0x17b：header(0,0x44)；flag=true 单条，false 全列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864d9a4 | _ZN5CUser19SendRestrictedGoodsEbi
void CUser::SendRestrictedGoods(bool flag, int itemIdx)
{
    PacketGuard packet;
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)&packet;
    pkt->put_header(0, 0x44);
    if (flag)
    {
        pkt->put_byte(0);
        pkt->put_short(1);
        pkt->put_int(itemIdx);
    }
    else
    {
        pkt->put_byte(1);
        pkt->put_short((int)m_list711ec.size());
        for (std::list<int>::const_iterator it = m_list711ec.begin();
             it != m_list711ec.end(); ++it)
        {
            pkt->put_int(*it);
        }
    }
    pkt->finalize(true);
    Send(packet);
}

// ORIG T 0x08656636 sz0xad：遍历 m_characList 统计 _Charac_info+0x27(short)==type。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08656636 | _ZN5CUser24GetSpecialLevelCharacCntEi
int CUser::GetSpecialLevelCharacCnt(int type)
{
    // ORIG 0x8656636：const_iterator 循环（prologue begin + 每轮 end 临时重构，
    // 帧 0x38 容纳三 8B 迭代器临时）；index 形（size/operator[]）与 ORIG 块序不合。
    int cnt = 0;
    for (std::vector<_Charac_info>::const_iterator it = m_characList.begin();
         it != m_characList.end(); ++it)
    {
        if ((int)it->m_level == type)
            ++cnt;
    }
    return cnt;
}

// ORIG _ZNK5CUser22HasCurCharacRentalItemEi T 0x086772d2 sz0x117：
// 外层 map 键=rental 类型，内层键=当前角色 characNo（+0x00），vector 非空即有。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086772d2 | _ZNK5CUser22HasCurCharacRentalItemEi
bool CUser::HasCurCharacRentalItem(int type) const
{
    const _Charac_info* cur = getCurCharacR();
    if (cur == 0 || *(const unsigned int*)cur == 0)
        return false;
    std::map<unsigned int, std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> > >::const_iterator outer =
        m_rentalInfo.find((unsigned int)type);
    if (outer == m_rentalInfo.end())
        return false;
    std::map<unsigned int,
        std::vector<InstanceRentalSystem::RentalInfo> >::const_iterator inner =
        outer->second.find(*(const unsigned int*)cur);
    if (inner == outer->second.end())
        return false;
    return !inner->second.empty();
}

// ORIG W 0x08230172 sz0x3b：guildkey!=0 且当前角色==m_guildDB+0x17（master）。
bool CUser::IsGuildMaster()
{
    // ORIG 0x8230183-0x82301a6：`call;test %eax,%eax;je` + `cmp %edx,%eax;jne` 双跳
    // 共享 mov $0 尾岛。4.4 对"call 结果寄存器直比"恒物化 setne/sete（P197④），
    // 嵌套 if / `==0` / goto 形均反产 sete；只有把短路 && 直接作为 bool 返回值
    // （return a && b）时，两个条件分支才直落 0/1 常量岛 ⇒ 与 ORIG 全等。
    return get_charac_guildkey() && (getCurCharacNo() == m_guildDB.m_masterNo);
}

// ORIG T 0x0866c414 sz0x55：m_guildDB+0x2e 起 5 个 int 副会长表。
bool CUser::IsSubGuildMaster()
{
    // ORIG 0x866c424-0x866c446：表按 pack(1) 的 `int m_subMasterData[5]`（+0x2e 非对齐）
    // 建模后，4.4 与 4.1 同形折成 `add $0x23410,%edx; mov 0xe(%eax,%edx,4),%ebx`
    // （成员基址 0x8d040=4*0x23410 折进 scaled 变址、余 0xe 作 disp；probe s3 实证）；
    // 之前把头里建成 char[0x14]+reinterpret_cast ⇒ 基址 0x8d04e 不可 4 整除，4.4 退化成
    // base+shl 双步。技巧 22 register bool 物化 cmp;sete;test;je 命中。
    for (int i = 0; i < 5; ++i)
    {
        // b28d-solo-CUser AE: ORIG `cmp; sete %al; test %al,%al; je` 无槽；具名 hit 落 -0x9
        // 并把 i 槽推 -0xc→-0x10 ⇒ 内联条件。
        // b28d-solo-CUser：getCurCharacNo 已 unsigned（CUserCharacInfo.h:701 Main P211 实验）
        //   而 m_subMasterData 为 int ⇒ 异型多一条 `mov %eax,%ebx` 中转；ORIG 0x0866c414
        //   单段直落 ebx（同型）⇒ 调用点收窄回 int。
        if (m_guildDB.m_subMasterData[i] == (int)getCurCharacNo())
            return true;
    }
    return false;
}

// ---------------- U1 批次第二段（deleteSpecific 域 / 杂项发送域）----------------

// 定义于 CDataManagerScripts.cpp（ORIG 0x867cf8d T）
int _checkTimeoutItem(const Inven_Item& item);

// ORIG T 0x0867cf0c sz0x81：找 pair.first==itemIdx，返回 second；未找到 -1。
int CUser::_isMatchedItem(int itemIdx,
                          const std::vector<std::pair<int, int> >& list)
{
    // ORIG 0x867cf0c：prologue 单槽 it=-0xc（begin 缓存），循环 cond 无 end 重构
    //（技巧 25 具名缓存；声明序 begin 后占近槽）。
    std::vector<std::pair<int, int> >::const_iterator it = list.begin();
    std::vector<std::pair<int, int> >::const_iterator itend = list.end();
    for (; it != itend; ++it)
    {
        if (it->first == itemIdx)
            return it->second;
    }
    return -1;
}

// ORIG T 0x0867d0be sz0x330：[begin,end] 槽位超时/匹配/无效物品删除；
// 无效物品入 mailDel，最后邮件补偿。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d0be | _ZN5CUser24_deleteSpecificItemInvenE10INVEN_TYPERKSt6vectorISt4pairIiiESaIS3_EERS5_ii
void CUser::_deleteSpecificItemInven(INVEN_TYPE type,
                                     const std::vector<std::pair<int, int> >& list,
                                     std::vector<std::pair<int, int> >& out,
                                     int begin, int end)
{
    CInventory* inven = getCurCharacInvenW();
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = begin; slot <= end; ++slot)
    {
        Inven_Item item = inven->GetInvenSlot(type, slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        int matched = -1;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                mailDel.push_back(std::make_pair(item.m_addInfo, item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            enableSaveInven();
            if (matched == 0)
                inven->delete_item(type, slot, item.m_addInfo2,
                                   (eItemDelReason)0x13, true);
            else
                inven->delete_item(type, slot, item.m_addInfo2 - matched,
                                   (eItemDelReason)0x13, true);

            if (type == (INVEN_TYPE)0)
                out.push_back(std::make_pair(3, slot));   /*推断: ENUM_ITEMSPACE(3)*/
            else if (type == (INVEN_TYPE)1)
                out.push_back(std::make_pair(0, slot));
        }
    }

    RewardItem2DeleteInvalidItem("Inven", mailDel);
}

// ORIG T 0x0867d3ee sz0x2ae：同型逻辑作用于 CCargo 槽位 0..0x97。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d3ee | _ZN5CUser24_deleteSpecificItemCargoERKSt6vectorISt4pairIiiESaIS2_EERS4_
void CUser::_deleteSpecificItemCargo(
    const std::vector<std::pair<int, int> >& list,
    std::vector<std::pair<int, int> >& out)
{
    CCargo* cargo = getCurCharacCargoW();
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = 0; slot < 0x98; ++slot)
    {
        Inven_Item item = cargo->get_cargo_slot(slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        int matched = -1;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                mailDel.push_back(std::make_pair(item.m_addInfo, item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            if (matched == 0)
                cargo->delete_item(slot, item.m_addInfo2, (eItemDelReason)0x13);
            else
                cargo->delete_item(slot, item.m_addInfo2 - matched,
                                   (eItemDelReason)0x13);
            out.push_back(std::make_pair(2, slot));
        }
    }

    RewardItem2DeleteInvalidItem("Cargo", mailDel);
}

// ORIG T 0x0867d69c sz0x1d2：GetCharacExpandData(6) 兑换列表遍历删除，
// 超时或命中即 erase + alter() 标脏。m_redeemList 私有，按 ORIG 布局经 +0x08 访问。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d69c | _ZN5CUser25_deleteSpecificRedeemItemERKSt6vectorISt4pairIiiESaIS2_EE
void CUser::_deleteSpecificRedeemItem(
    const std::vector<std::pair<int, int> >& list)
{
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)6);
    if (redeem == 0)
        return;

    typedef std::list<Redeem_Item::Inven_Item_Expand_RedeemInfo> RedeemList;
    RedeemList& items = redeem->m_redeemList;   // 权威成员 +0x08

    unsigned int count = 0;
    RedeemList::iterator it = items.begin();
    while (it != items.end() && count <= 0x14)
    {
        Inven_Item item;
        memcpy(&item, &*it, sizeof(Inven_Item));   // RedeemInfo 前 0x3d 字节即 Inven_Item
        if (item.m_addInfo != 0)
        {
            bool remove =
                _checkTimeoutItem(item) ||
                (_isMatchedItem(item.m_addInfo, list) != -1);
            if (remove)
            {
                items.erase(it++);
                redeem->alter();
            }
            else
            {
                ++it;
            }
        }
        else
        {
            ++it;   /*推断*/  // ORIG 空项仅跳过不 erase
        }
        ++count;
    }
}

// ORIG T 0x0867d8a4 sz0x2c1：账号金库（须 IsExistAccountCargo），槽位数=GetCapacity()。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867d8a4 | _ZN5CUser31_deleteSpecificItemAccountCargoERKSt6vectorISt4pairIiiESaIS2_EERS4_
void CUser::_deleteSpecificItemAccountCargo(
    const std::vector<std::pair<int, int> >& list,
    std::vector<std::pair<int, int> >& out)
{
    if (!IsExistAccountCargo())
        return;

    CAccountCargo* cargo = GetAccountCargo();
    std::vector<std::pair<int, int> > mailDel;

    for (int slot = 0; slot < cargo->GetCapacity(); ++slot)
    {
        Inven_Item item = cargo->GetSlot(slot);
        if (item.m_addInfo == 0)
            continue;

        bool del = false;
        int matched = -1;
        if (_checkTimeoutItem(item))
        {
            del = true;
            matched = 0;
        }
        else
        {
            matched = _isMatchedItem(item.m_addInfo, list);
            if (matched != -1)
            {
                if (item.m_addInfo2 > matched)
                    del = true;
            }
            else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                         item.m_addInfo))
            {
                mailDel.push_back(std::make_pair(item.m_addInfo, item.m_addInfo2));
                del = true;
            }
        }

        if (del)
        {
            if (matched == 0)
                cargo->DeleteItem(slot, item.m_addInfo2);
            else
                cargo->DeleteItem(slot, item.m_addInfo2 - matched);
            out.push_back(std::make_pair(0xc, slot));
        }
    }

    RewardItem2DeleteInvalidItem("AccountCargo", mailDel);
}

// ORIG T 0x0867db66 sz0x39：转发给 GetCharacExpandData(0xe)（CTitleBook）。
void CUser::_deleteSpecificItemTitieBook(
    const std::vector<std::pair<int, int> >& list)
{
    CTitleBook* titleBook =
        (CTitleBook*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (titleBook != 0)
    {
        titleBook->deleteTimeoutItem(list);
    }
    return;
}

// ORIG T 0x086928c2 sz0x145：金币兑换。itemIdx<=0x28d291 → (idx-0x28d287)*10^6；
// 否则 idx*10^7-0x549a800。bReset 只做上限校验（err=10 返回 false）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x086928c2 | _ZN5CUser13DoGetGoldItemEiRSt6vectorISt4pairIiiESaIS2_EEbR10ENUM_ERROR
bool CUser::DoGetGoldItem(int itemIdx, std::vector<std::pair<int, int> >& out,
                          bool bReset, ENUM_ERROR& err)
{
    int amount;
    if (itemIdx <= 0x28d291)
        amount = (itemIdx - 0x28d287) * 1000000;
    else
        amount = itemIdx * 10000000 - 0x549a800;

    if (bReset)
    {
        unsigned int limit = G_CDataManager()->GetMoneyLimitPerLevel(
            get_charac_level(), get_acc_name());
        if ((unsigned int)getCurCharacMoney() + (unsigned int)amount > limit)
        {
            err = (ENUM_ERROR)0xa;
            return false;
        }
    }
    else
    {
        CInventory* inven = getCurCharacInvenW();
        inven->gain_money(amount, (eMoneyAddReason)0xe, true, 0);
        out.push_back(std::make_pair((int)ENUM_ITEMSPACE_0, 0));
    }
    return true;
}

// ORIG T 0x08692a34 sz0xc2：!bReset 时经 s_pIPGHelper 虚调用 vtable+0x0c/+0x2c；
// WongWork::IPG::CIPGHelper 仅最小建模（仅 dtor 虚函数），虚接口待补后恢复真实调用。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08692a34 | _ZN5CUser18DoGetCeraPointItemEjbR10ENUM_ERRORm
bool CUser::DoGetCeraPointItem(unsigned int money, bool bReset,
                               ENUM_ERROR& err, unsigned long itemIdx)
{
    // ORIG 0x8692a34：!bReset 时 IPG vtable+0xc = IPGInputPoint(this, money,
    // itemIdx==0x28ec3d?3:1, 0, 0)，随后 vtable+0x2c = IPGOutputConfirm(0,0)。
    (void)err;
    if (!bReset && GlobalData::s_pIPGHelper != 0)
    {
        GlobalData::s_pIPGHelper->IPGInputPoint(this, money,
                                                (char)(itemIdx == 0x28ec3d ? 3 : 1),
                                                0, 0);
        GlobalData::s_pIPGHelper->IPGOutputConfirm(0, 0);
    }
    return true;
}

// ORIG T 0x0868c3bc sz0xa2（返回 1 已删 / 0 事件空或非活跃）：
// OnTimeEvent(0x33) 活跃时删除奖励用户。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x0868c3bc | _ZN5CUser25delete_ontime_reward_listEv
bool CUser::delete_ontime_reward_list()   // b28c-L5 SIGFIX（P174）
{
    CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x33);
    // b28d-lane5：ORIG 0x0865a3xx 两失败路径**共享** trace 块（`cmpl $0,-0xc; je T` +
    // `call IsEventing; xor $1; test %al; je T`，T=trace 构造）⇒ 独立守卫 + goto
    // 共享块（P133；`||` 合并形反产 mov$1/mov$0 bool 物化岛）。
    if (ev == 0)
        goto TRACE;
    if (!ev->IsEventing(0))
        goto TRACE;
    ((COnTimeEvent*)ev)->delete_reward_user(this);
    return 1;
TRACE:
    {
        cMyTrace trace("bool CUser::delete_ontime_reward_list()", 0xa828, 5);
        trace("[ON_TIME_EVENT]delete_ontime_reward_list() error");
        return 0;
    }
}

// ORIG T 0x086924aa sz0x119：job==9 才发包；去重 combo 技能 → 包 0x1bd。
void CUser::sendComboSkillList()
{
    if (get_charac_job() != 9)
        return;
    ((SkillSlot*)getCurCharacSkillW())->eraseDuplicationComboSKill();
    PacketGuard packet;
    packet.put_header(0, 0x1bd);
    packet.put_byte(isComboSkillSet() ? 1 : 0);
    packet.put_byte(2);
    ((const SkillSlot*)getCurCharacSkillR())->getComboSkillSendInfo(packet);
    packet.finalize(true);
    Send(packet);
    setComboSkillSet(false);
}

// ORIG T 0x08656808 sz0xbb：发送当前金币（MakeItemPacket(1,1) → 0xe 包）。
void CUser::SendCoin()
{
    if (getCurCharacR() == 0)
        return;
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    // ORIG 0x865689e xor $0x1,%eax; 0x86568a3 jne cleanup：bool 返回取反 + 空 if（probe 实证 f1 形）
    if (!getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 1, packet))
    {
    }
    else
    {
        packet.finalize(true);
        Send(packet);
    }
}

// ORIG T 0x086504d8 sz0x11d：频道类型 !=7 时经 BigStreamPool 构造
// header(0x23)+UID 内部流，GetInBuffer<SIG_SAVE_INVENTORY> 后 _GetSaveInvenBase
// 填充、+0x08 字节置 1，投递 DB_Q。反编译见 CUser/SaveMoneyCoin.md。
bool CUser::SaveMoneyCoin()
{
    // P185 guard 作用域块形（ORIG 0x086504d8：guard@-0x14、put 后 `mov $1,%ebx`
    // 真值先于 dtor、epilogue 统一 movzbl %bl——旧平铺形 dtor 岛位置不同）。
    // ==7 走 sete 物化岛（技巧 22）；uid 跨 operator- 存活 %ebx。
    // ORIG 0x86504d8 **ebx 聚合单出口**：假支 `mov $0,%ebx; jmp END`、真支 put 后
    // `mov $1,%ebx`，END epilogue `movzbl %bl,%eax`；uid 跨 operator* 存活同用 %ebx
    // （时序不重叠：uid 在 put 前消费完）。
    register bool rc;   // b28d-solo-CUser：ORIG rc 驻 %ebx（mov $0/%1,%ebx + 尾 movzbl %bl）
    // b28d-solo-CUser：ORIG `sete %al; test %al,%al; je` 无 ch7 槽（P292 非 bool 物化）
    //   ⇒ 内联条件（probe_bool a3）。
    if (G_GameWorld()->GetChannelType() == 7)
    {
        rc = false;
        goto RCEND;
    }
    {
        CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("user.cpp", 0x18cc),
                           true);
        guard.operator*()->operator<<(0x23);
        register int uid;   // b28d-solo-CUser：ORIG `mov %eax,%ebx` 跨 operator* 保活
        uid = (int)GetUID();
        guard.operator*()->operator<<(uid);
        // ORIG 0x8650574-582：取 buffer 前先经一次 operator->（pt）call——源形
        // `guard->GetInBuffer<...>()`（智能指针循环解引用，pt 返回值作 this）。
        SIG_SAVE_INVENTORY* sig =
            ((CStreamGuard*)(void*)guard.operator->())
                ->GetInBuffer<SIG_SAVE_INVENTORY>();
        _GetSaveInvenBase(sig);
        sig->m_saveFlag = 1;            // ORIG movb $0x1,0x8(%eax)（+0x08 标志）
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        rc = true;
    }
RCEND:
    return rc;
}

// ORIG T 0x08690e36 sz0x82：事件邮件提醒 tooltip。空包 put_header(0,0x1ad) +
// finalize(true) 后直接 Send（docs/class_func_reports/CUser/SendEventMailAlaramTooltip.md）。
void CUser::SendEventMailAlaramTooltip()
{
    PacketGuard packet;
    packet.put_header(0, 0x1ad);
    packet.finalize(true);
    Send(packet);
}

// ORIG T 0x0869154a sz0x664：抽奖计数道具发放（事件币/道具/邮件补偿）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0869154a | _ZN5CUser20giveLotteryCountItemEib
void CUser::giveLotteryCountItem(int itemNo, bool match)
{
    if (getCurCharacR() == 0)
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0xb45f,
            "giveLotteryCountItem curchar = null id(%s) item_no(%u) match(%d)",
            NumberToString(get_acc_id(), 0), itemNo, match ? 1 : 0);
        return;
    }

    CStackableItem* item =
        (CStackableItem*)G_CDataManager()->find_item(itemNo);
    if (item == 0)
        return;
    if (!verifyStackableItemProcess((StackableAction::Type)0x36, item))
        return;

    std::vector<Inven_Item> rewards;
    const std::vector<int>& src = item->m_vec3a0;   // ORIG +0x3a0
    for (size_t i = 0; i + 1 < src.size(); i += 2)
    {
        Inven_Item it;                              // 默认构造后按 ORIG 填 8 字节
        *(int*)((char*)&it + 0) = src[i];
        *(int*)((char*)&it + 4) = src[i + 1];
        rewards.push_back(it);

        if (*(int*)&it == 1 && src[i + 1] >= 0)     // 事件币分支
        {
            int amount = src[i + 1];
            unsigned int now = getCurCharacInvenR()->GetEventCoin()
                             + (unsigned int)amount;
            getCurCharacInvenW()->SetEventCoin(now);
            m_historyLog.EventCoinAdd((int)now, amount, (eCoinAddReason)2);
            SaveMoneyCoin();
            SendCoin();
            continue;
        }

        G_CDataManager()->m_itemList->create_item(
            (ENUM_ITEM_CREATE_TYPE)2, it, src[i + 1]);

        bool give = (*(int*)&it != 0) || (it.get_add_info() != 0);
        if (!give)
            continue;

        int slot = getCurCharacInvenW()->insertItemIntoInventory(
            it, (eItemAddReason)0xd, true, true);
        if (slot < 0)
        {
            char msg[256];
            const char* str = g_scriptStringManager_.findString(
                4, "game_server_msg_104", (bool*)0);
            strncpy(msg, str ? str : "", 0xff);
            msg[0xff] = '\0';
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                getCurCharacName(), it, 0, (unsigned int)getCurCharacNo(),
                msg, (int)strlen(msg), 0, GetServerGroup(), false, false);
        }
        else
        {
            SendUpdateItem(eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        }
    }

    PacketGuard packet;
    packet.put_header(0, 0x1d0);
    packet.put_byte((int)rewards.size());
    for (size_t i = 0; i < rewards.size(); ++i)
    {
        packet.put_int(rewards[i].m_addInfo);     // +2
        packet.put_int(rewards[i].m_addInfo2);    // +7
    }
    packet.finalize(true);
    Send(packet);
}

// ORIG T 0x0868f472 sz0x158：i=0..min(6,m_field796c8) 发 0x1e 字节 MoneyLogPacket；
// 最后 resetMoneyLog()。对象布局为全局 MoneyLogPacket（PacketHeader 10B +
// 5 int @+0xa..+0x1d；ORIG 字段偏移实证），ctor 写 0x2723/0x1e 头并清零。
void CUser::SendMoenyLog()
{
    if (getCurCharacR() != 0)
    {
        for (int i = 0; i < 7 && i <= m_moneyLogIdx; ++i)
        {
            if (m_moneyLogs[i].m_money[0] == 0 &&
                m_moneyLogs[i].m_money[1] == 0)
                continue;
            MoneyLogPacket pkt;
            pkt.m_characNo = getCurCharacNo();
            pkt.m_money0   = m_moneyLogs[i].m_money[0];
            pkt.m_money1   = m_moneyLogs[i].m_money[1];
            pkt.m_accId    = get_acc_id();
            pkt.m_money2   = m_moneyLogs[i].m_money[2];
            // ORIG 0x868f556：&pkt 先求值存 %ebx（跨 GetServerGroup/GetServerProxy
            // 存活），proxy 调用结果不经具名槽，直接 SendPacket。
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(GetServerGroup())
                ->SendPacket((char*)&pkt, 0x1e);
        }
    }
    resetMoneyLog();
}

// ORIG T 0x0868152c sz0x1e0：PC 房 login 通知。填 stLogin（login[0]=3/acc 名/
// charac 名/ip/uid/accid）→ stLoginPacket 打包 → SendTcpPacket；随后
// ReadPacket 读回包（ORIG 以 pkt 自身为 this、reply 为 packet 源）。
// b28d-lane5 N封顶: P11 反向符号扩展; N82/P112 槽号分配 | ORIG 0x0868152c | _ZN5CUser23SendLoginToPCRoomServerEv
void CUser::SendLoginToPCRoomServer()
{
    char login[0x20e];
    char reply[0x20e];
    memset(login, 0, sizeof(login));
    // b28d-lane5：ORIG 0x0868xxx 每次字段访问重发 `lea -0x226(%ebp)`（P183 地址
    // 重物化铁律）⇒ 删除 lp 指针缓存，全表达式展开。
#define LP_LOGIN ((nexon_packet::stLogin*)login)
    LP_LOGIN->m_type = 3;
    strcpy(LP_LOGIN->m_account, get_acc_name());
    LP_LOGIN->m_accLen = (char)strlen(get_acc_name());
    strcpy(LP_LOGIN->m_characName, getCurCharacName());
    LP_LOGIN->m_charLen = (char)strlen(getCurCharacName());
    const int ip = get_public_ip_address();
    LP_LOGIN->m_ip = (unsigned int)ip;
    LP_LOGIN->m_canTry = 0;
    LP_LOGIN->m_userIdx = (unsigned int)GetUID();   // ORIG：直接写 stLogin+0x206（-0x20(%ebp)）
    LP_LOGIN->m_accId = (unsigned int)get_acc_id(); // ORIG：直接写 stLogin+0x20a（-0x1c(%ebp)）
    printf(
        "SEND nexon_packet::stLogin login:%d AccLen:%d Acc:%s CharLen:%d Char:%d "
        "Addr:%d canTry_:%d UserIdx:%d m_id:%d\n",
        LP_LOGIN->m_type, LP_LOGIN->m_accLen, LP_LOGIN->m_account, LP_LOGIN->m_charLen,
        (int)LP_LOGIN->m_characName,
        ip, LP_LOGIN->m_canTry, (int)LP_LOGIN->m_userIdx, (int)LP_LOGIN->m_accId);
#undef LP_LOGIN

    nexon_packet::stLoginPacket pkt(*(nexon_packet::stLogin*)login);
    int len = pkt.GetLength();
    GlobalData::s_pcroom_proxy->SendTcpPacket((char*)&pkt, len);
    memset(reply, 0, sizeof(reply));
    pkt.ReadPacket((const char*)reply, *(nexon_packet::stLogin*)reply);
}

// ORIG T 0x0868170c sz0x113：PC 房 logout 通知。stLoginPacket 打包 →
// GetLength → SendTcpPacket（nexon_packet 已建模，符号直调）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868170c | _ZN5CUser24SendLogoutToPCRoomServerEv
void CUser::SendLogoutToPCRoomServer()
{
    char login[0x210];
    // b28d-lane5：同 SendLoginToPCRoomServer——ORIG 0x086817xx 逐字段重发
    // `lea -0x226(%ebp)`（P183），删 lp 指针缓存。
#define LP_LOGOUT ((nexon_packet::stLogin*)login)
    LP_LOGOUT->m_type = 1;
    strcpy(LP_LOGOUT->m_account, get_acc_name());
    LP_LOGOUT->m_accLen = (char)strlen(get_acc_name());
    strcpy(LP_LOGOUT->m_account, get_acc_name());   // ORIG 重复一次（+0x049/+0x064 双 strcpy）
    LP_LOGOUT->m_accLen = (char)strlen(get_acc_name());
    strcpy(LP_LOGOUT->m_characName, getCurCharacName());
    LP_LOGOUT->m_charLen = (char)strlen(getCurCharacName());
    LP_LOGOUT->m_ip = (unsigned int)get_local_ip_address();
    LP_LOGOUT->m_canTry = 1;
#undef LP_LOGOUT

    nexon_packet::stLoginPacket pkt(*(nexon_packet::stLogin*)login);
    int len = pkt.GetLength();
    GlobalData::s_pcroom_proxy->SendTcpPacket((char*)&pkt, len);
}

// ORIG T 0x0864870e sz0xdc：LOGDB_Q(=4)：<<0xcc、<<-1、<<reason。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864870e | _ZN5CUser29SendClientCrashDownLogToLogDBEi
void CUser::SendClientCrashDownLogToLogDB(int reason)
{
    // b28d-lane5：ORIG 0x0864870e 无 Stream* 槽——Acquire 结果经 %eax 直传 ctor
    // （P0 右值内联；同 _UpdateUserInfo4Guild）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x8ac), true);
    guard.operator*()->operator<<(0xcc);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(reason);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ORIG T 0x0867fd0a sz0x3bb：MCAP 扣减率 → hades 计数 → checkMaxHackCntOver 处置
// → HackDetected 历史记录。全部依赖已建模为真实 C++ 符号（§5.3 去 asm 桥）：
// CMCAPManager::getPointPerHackType/incPoint/getPoint、CHades::AddHackTypeCnt、
// CHackAnalyzer::setUserP/getServerHackCnt/getServerHackAccumulatedCnt、
// CAutoPunishRuleHackTypeMgr::checkMaxHackCntOver、CUser::setHackUser、
// HackTypeInfo_t 真实 ctor（清零）、cUserHistoryLog::HackDetected。
// 结构 vs Ghidra C（onUpdateHackCount.md）逐段对齐，HackTypeInfo_t 字段语义
// （m_type1/m_type2→setHackUser a/c、m_value2/m_value3→type/param6）照抄。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0867fd0a | _ZN5CUser17onUpdateHackCountEN8WongWork13ENUM_HACKTYPEEijj
void CUser::onUpdateHackCount(WongWork::ENUM_HACKTYPE type, int count,
                              unsigned int a, unsigned int b)
{
    m_mcap.incPoint(
        WongWork::CMCAPManager::getPointPerHackType(type) * count);
    if (m_mcap.getPoint() > 1000)
        setStdDropRate(100 - (int)(m_mcap.getPoint() / 100));

    char ip[16];
    if (!isHackUserTimer())
    {
        getHades()->AddHackTypeCnt((unsigned long)type, (unsigned long)count,
                                   a, b);
        getHackAnalyzer()->setUserP(this);
        m_network.GetPeerIP2(ip, 0x10);

        // ORIG：HackTypeInfo_t::HackTypeInfo_t 真实 ctor 清零（非 memset）。
        WongWork::HackTypeInfo_t info;
        if (GlobalData::g_autoPunishRuleHackTypeMgr->checkMaxHackCntOver(
                type,
                (unsigned int)getHackAnalyzer()->getServerHackAccumulatedCnt(type),
                getHackAnalyzer(), info,
                m_lookOuts1, m_ipCounters, m_lookOuts2, ip))
        {
            setHackUser((unsigned int)info.m_type1,
                        (unsigned int)getHackAnalyzer()->
                            getServerHackAccumulatedCnt(
                                (WongWork::ENUM_HACKTYPE)info.m_type1),
                        (unsigned int)info.m_type2,
                        (unsigned int)getHackAnalyzer()->
                            getServerHackAccumulatedCnt(
                                (WongWork::ENUM_HACKTYPE)info.m_type2),
                        info.m_value2, info.m_value3);
        }
        if (info.m_type2 == 3)
        {
            WongWork::HackTypeInfo_t info2;
            if (GlobalData::g_autoPunishRuleHackTypeMgr->checkMaxHackCntOver(
                    type,
                    (unsigned int)getHackAnalyzer()->getServerHackAccumulatedCnt(type),
                    getHackAnalyzer(), info2,
                    m_lookOuts1, m_ipCounters, m_lookOuts2, ip))
            {
                setHackUser((unsigned int)info2.m_type1,
                            (unsigned int)getHackAnalyzer()->
                                getServerHackAccumulatedCnt(
                                    (WongWork::ENUM_HACKTYPE)info2.m_type1),
                            (unsigned int)info2.m_type2,
                            (unsigned int)getHackAnalyzer()->
                                getServerHackAccumulatedCnt(
                                    (WongWork::ENUM_HACKTYPE)info2.m_type2),
                            info2.m_value2, info2.m_value3);
            }
        }
    }

    m_historyLog.HackDetected(
        (unsigned int)type, (unsigned int)count,
        (unsigned int)getHackAnalyzer()->getServerHackCnt(type),
        (unsigned int)getHackAnalyzer()->getServerHackAccumulatedCnt(type),
        a, b);
}

// ORIG _ZN5CUser14setStdDropRateEi @ 0x0828715c (W)：
// *(int*)(this+0x79624) = rate，即 m_codeHackCheck(+0x795fc).m_field28。
void CUser::setStdDropRate(int rate)
{
    m_postChecksumData.m_field28 = rate;
}

// ============================================================================
// CUser 连续/冷却道具链簇（ORIG T，单函数还原见各 .md）
// ============================================================================

// ORIG _ZN5CUser20_processCoolTimeItemEP14CStackableItemjj @ 0x0865eb0c (T)
// cooltime 处理：CStackableItem+0x2d0(ms) 换算为秒累加当前秒，更新/插入
// CheckCoolTimeItem 所读的 cooltime 列表条目（GetCurCharacCoolTimeItemList）。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865eb0c | _ZN5CUser20_processCoolTimeItemEP14CStackableItemjj
void CUser::_processCoolTimeItem(CStackableItem* item, unsigned int itemIdx,
                                 unsigned int curSec)
{
    unsigned int expireSec =
        (unsigned int)((unsigned int)item->m_int2d0 / 1000) + curSec;
    std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemList();
    char found = 0;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        if ((*cool)[i].m_itemIdx == (int)itemIdx)
        {
            if ((*cool)[i].m_expireTime < expireSec)
                (*cool)[i].m_expireTime = expireSec;
            found = 1;
        }
    }
    if (!found)
    {
        ContinuousItemInfo info;
        info.m_itemIdx = (int)itemIdx;
        info.m_expireTime = expireSec;
        cool->push_back(info);
    }
    UpdateCoolTimeItemInfo(curSec);
}

// ORIG _ZN5CUser18_processEffectItemEP14CStackableItemjj @ 0x0865ec0e (T)
// effect 处理：CStackableItem+0x2e0 第 0 字节非 1 → 报错；
// 否则读 +0x2dc(ms) → 秒并 insert timer(0x76)，更新/插入 effect 列表。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0865ec0e | _ZN5CUser18_processEffectItemEP14CStackableItemjj
void CUser::_processEffectItem(CStackableItem* item, unsigned int itemIdx,
                               unsigned int curSec)
{
    // X2 字段形（判据②③：ORIG 0x0865ec1d `movzbl 0x2e0; xor $1; test al; je 消费者`
    // ——`!= 1` 直写 4.4 产 `cmp $1,%al`，uchar 绑 eax + ^=1 复现 xor 段）。
    unsigned char tag =
        (unsigned char)item->m_stat2dc.m_flagLo;
    tag ^= 1;
    if (! tag)
    {
        cMyTrace trace("void CUser::_processEffectItem(CStackableItem*, "
                       "unsigned int, unsigned int)", 0x391e, 0);
        trace("ERROR : Mismatch tag [stat change duration] and "
              "[effect maintenance]. There is no duration value.");
        return;
    }
    unsigned int durationSec = (unsigned int)item->m_stat2dc.sec / 1000;
    unsigned int expireSec = curSec + durationSec;
    CStackableItem::StatChangeInfo stat = item->GetStatChangeInfo();
    if (stat.m_flagLo != 0 || stat.m_flagHi != 0)
    {
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)0, GetUID(),
                                    (TIMER_MESSAGE)0x76, (int)durationSec,
                                    (int)itemIdx, (unsigned int)getCurCharacNo());
    }
    std::vector<ContinuousItemInfo>* effect =
        GetCurCharacEffectItemList();
    bool found = false;
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        if ((*effect)[i].m_itemIdx == (int)itemIdx)
        {
            if ((*effect)[i].m_expireTime < expireSec)
                (*effect)[i].m_expireTime = expireSec;
            found = true;
        }
    }
    if (! found)   // bool 值域聚合位，X2 免约束（判据①，ORIG 尾 testb %al 直判）
    {
        ContinuousItemInfo info;
        info.m_itemIdx = (int)itemIdx;
        info.m_expireTime = expireSec;
        effect->push_back(info);
    }
    UpdateEffectItemInfo(curSec);
}

// ORIG _ZN5CUser22_processContinuousItemEjP14CStackableItem @ 0x0865ea62 (T)
void CUser::_processContinuousItem(unsigned int itemIdx, CStackableItem* item)
{
    unsigned int curSec = (unsigned int)GlobalData::s_systemTime_.getCurSec();
    if (item != 0)
    {
        CStackableItem::CoolTimeOption coolOpt = item->GetCoolTimeOption();
        if (coolOpt.flag != 0)
            _processCoolTimeItem(item, itemIdx, curSec);
        CStackableItem::CoolTimeOption effOpt = item->GetEffectItemOption();
        if (effOpt.flag != 0)
            _processEffectItem(item, itemIdx, curSec);
    }
    _processSpeicalContinuousTimeItem(itemIdx);
}

// ORIG _ZN5CUser33_processSpeicalContinuousTimeItemEj @ 0x08659b60 (T)
// 特殊连续时间道具：查 item→is_stackable→IsExpAffect→GetExpAffectRatio>0
// 时 SetExpAffectItemEffect(true, ratio)。
// b28d-lane5 N封顶: N85 x87比较lowering | ORIG 0x08659b60 | _ZN5CUser33_processSpeicalContinuousTimeItemEj
void CUser::_processSpeicalContinuousTimeItem(unsigned int itemIdx)
{
    CItem* pItem = G_CDataManager()->find_item((int)itemIdx);
    if (pItem == 0)
        return;
    // ORIG 0x08659b60：is_stackable/IsExpAffect 均正向 `test %al,%al; je 共享尾
    // 出口`（无 xor/jne 反形）⇒ 源码正向嵌套 if（P59 反向）；ratio 槽 -0xc
    // fstps+flds 往返；x87 fucompp/fnstsw/sahf 段属 N85 跨版本豁免。
    if (pItem->is_stackable())
    {
        CStackableItem* stack = (CStackableItem*)pItem;
        if (stack->IsExpAffect())
        {
            float ratio = stack->GetExpAffectRatio();
            if (ratio > 0.0f)
                SetExpAffectItemEffect(true, ratio);
        }
    }
}

// ORIG _ZN5CUser17CheckCoolTimeItemEm @ 0x0865e994 (T)
// 当前角色 cooltime 列表中存在 itemIdx==param_1 且 expireTime>curSec 的条目。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0865e994 | _ZNK5CUser17CheckCoolTimeItemEm
bool CUser::CheckCoolTimeItem(unsigned long itemIdx) const
{
    if (getCurCharacR() == 0)
        return false;
    const std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemListR();
    unsigned int curSec = (unsigned int)GlobalData::s_systemTime_.getCurSec();
    for (std::vector<ContinuousItemInfo>::const_iterator it = cool->begin();
         it != cool->end(); ++it)
    {
        if ((unsigned long)it->m_itemIdx == itemIdx)
        {
            if (curSec < it->m_expireTime)
                return true;
            return false;
        }
    }
    return false;
}

// ORIG _ZN5CUser22UpdateCoolTimeItemInfoEj @ 0x08658b2a (T)
// 组 SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO（0x100 占位：characNo@+0x0 +
// count@+0x4 + [0x194]条目簇）投递队列 2。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08658b2a | _ZN5CUser22UpdateCoolTimeItemInfoEj
void CUser::UpdateCoolTimeItemInfo(unsigned int expireTime)
{
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x2cb8);
    CStreamGuard guard(s, true);
    guard.operator*()->operator<<((int)0x132);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO* sig = guard.operator->()->GetInBuffer<
        SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    memset(sig, 0, sizeof(SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO));
    sig->m_characNo = (unsigned int)getCurCharacNo();
    std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemList();
    if (cool->size() > 0x32) return;   // ORIG: 超过 0x32 条不发送
    unsigned int count = 0;
    for (unsigned int i = 0; i < cool->size(); ++i)
    {
        unsigned int idx = (unsigned int)(*cool)[i].m_itemIdx;
        unsigned int exp = (*cool)[i].m_expireTime;
        if (exp < expireTime || idx > 0x7ffffffe || exp > 2000000000u)
        {
            cool->erase(cool->begin() + i);
            --i;
            continue;
        }
        if (count < 0x32)
        {
            sig->m_info.m_entries[count].m_itemIdx = (*cool)[i].m_itemIdx;
            sig->m_info.m_entries[count].m_expireTime = (*cool)[i].m_expireTime;
            ++count;
        }
    }
    sig->m_info.m_count = (unsigned char)count;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ORIG _ZN5CUser20UpdateEffectItemInfoEj @ 0x08658d90 (T)
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08658d90 | _ZN5CUser20UpdateEffectItemInfoEj
void CUser::UpdateEffectItemInfo(unsigned int expireTime)
{
    Stream* s = GlobalData::s_stream_pool->Acquire("user.cpp", 0x2ceb);
    CStreamGuard guard(s, true);
    guard.operator*()->operator<<((int)0x137);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO* sig = guard.operator->()->GetInBuffer<
        SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO>();
    memset(sig, 0, sizeof(SIG_SAVE_AND_LOAD_CONTINUOUS_ITEM_INFO));
    sig->m_characNo = (unsigned int)getCurCharacNo();
    std::vector<ContinuousItemInfo>* effect =
        GetCurCharacEffectItemList();
    if (effect->size() > 0x32) return;   // ORIG: 超过 0x32 条不发送
    unsigned int count = 0;
    for (unsigned int i = 0; i < effect->size(); ++i)
    {
        unsigned int idx = (unsigned int)(*effect)[i].m_itemIdx;
        unsigned int exp = (*effect)[i].m_expireTime;
        if (exp < expireTime || idx > 0x7ffffffe || exp > 2000000000u)
        {
            effect->erase(effect->begin() + i);
            --i;
            continue;
        }
        if (count < 0x32)
        {
            sig->m_info.m_entries[count].m_itemIdx = (*effect)[i].m_itemIdx;
            sig->m_info.m_entries[count].m_expireTime = (*effect)[i].m_expireTime;
            ++count;
        }
    }
    sig->m_info.m_count = (unsigned char)count;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}

// ORIG _ZN5CUser32SendEventJarOutputItemStatisticsERKSt6vectorI10Inven_ItemSaIS1_EE
//   @ 0x0868a782 (T)
// vec 长==2 时：元素加信息(+2 addInfo)==0x28be7b 即罐子；另一元素为产出道具。
// 组 Packet_Event_UpgradeJar_Output_Statistic_GTS(0x106e/0x12) 经 statistic
// proxy(GET_ENUM_SERVER_GROUP_NONE) 下发。罐子 id 为 0x28be7b，剩余逻辑：
//   local_e = 罐子在 vec 中的下标（0 或 1）；m_itemCode = vec[1-local_e].addInfo；
//   m_itemCount = vec[local_e].+7（m_addInfo2 罐子数量）。
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x0868a782 | _ZN5CUser32SendEventJarOutputItemStatisticsERKSt6vectorI10Inven_ItemSaIS1_EE
void CUser::SendEventJarOutputItemStatistics(
    const std::vector<Inven_Item>& items)
{
    if ((int)items.size() != 2)
        return;
    unsigned short jarIdx = 0;
    if (items[0].m_addInfo != 0x28be7b)
    {
        if (items[1].m_addInfo != 0x28be7b)
            return;
        jarIdx = 1;
    }
    int itemCode = items[1 - (int)jarIdx].m_addInfo;
    int itemCount = items[(int)jarIdx].m_addInfo2;
    Packet_Event_UpgradeJar_Output_Statistic_GTS pkt;
    pkt.m_itemCode = itemCode;
    pkt.m_itemCount = itemCount;
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)0);
    if (proxy != 0)
        proxy->SendPacket((char*)&pkt, 0x12);
}

// ============================================================================
// ORIG 0x08692b84 CUser::SendAradEventItemLog(unsigned long, int)：
// "Arad_StatisticsEventItemList" 脚本 needStatistics(itemIndex) 命中时，
// 构造 16 字节 Sig{itemIndex,count,accId,characNo(-1)} 经内部流 0x20 投递
// LOGDB_Q。反编译见 docs/class_func_reports/CUser/SendAradEventItemLog.md。
// ============================================================================

// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08692b84 | _ZN5CUser20SendAradEventItemLogEmi
void CUser::SendAradEventItemLog(unsigned long itemIndex, int count)
{
    ARAD::Arad_DataManager* mgr = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
    ARAD::Arad_InterfaceGameScript* script =
        mgr->findGameScript("Arad_StatisticsEventItemList");
    if (script == 0 ||
        !static_cast<ARAD::Arad_StatisticsEventItemList*>(script)->needStatistics(itemIndex))
    {
        return;
    }

    arad::SigAradEventItemLog sig;
    sig.m_itemId = itemIndex;
    sig.m_itemCount = count;
    sig.m_id = get_acc_id();
    sig.m_characNo = get_charac_no(-1);

    // b28d-lane5：ORIG 0x0866xxx 无 Stream* 槽——Acquire 结果 %eax 直传 ctor（P0）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0xb9c7), true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, ARAD::K_ARAD_EVENT_ITEM_LOG, (int)GetUID());
    guard.operator->()->put_binary(&sig, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
}

// ============================================================================
// 任务 QP 发送链（RestoreUserQuestStubs：Dispatcher_Debug_Clear_Quest 依赖）
// ============================================================================

// ORIG T _ZN5CUser12sendCharacQpEv @0x0868ac24：
// GetCharacExpandData(0xb)（CQuestShop）非空 → shop->sendCharacQp(this, 0)。
void CUser::sendCharacQp()
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        shop->sendCharacQp(this, 0);
    }
    return;
}

// ORIG T _ZN5CUser13incQuestPointEiib @0x0868ac66：
// GetCharacExpandData(0xb) 非空 → old=getQp()、addQp(qp)；!send →
// shop->sendCharacQp(this,0)；m_historyLog.QPAdd(old, questIdx, qp)。
void CUser::incQuestPoint(int qp, int questIdx, bool send)
{
    CQuestShop* shop = (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    if (shop != 0)
    {
        int before = shop->getQp();
        shop->addQp(qp);
        if (!send)
        {
            shop->sendCharacQp(this, 0);
        }
        m_historyLog.QPAdd(before, questIdx, qp);
    }
    return;
}

// ============================================================================
// ORIG T 0x0864d90a：从 m_list711ec（+0x711ec list<int>）删除 state。
// ============================================================================
void CUser::RemoveRestrictedGoods(int state)
{
    // ORIG 0x0864d90a：b/end/it 三迭代器各落槽 -0x10/-0x14/-0x18——end() 仅
    // call 一次（具名 e 局部缓存，it != e 复用）；find 结果槽 sret -0x18。
    std::list<int>::iterator b = m_list711ec.begin();
    std::list<int>::iterator e = m_list711ec.end();
    std::list<int>::iterator it = std::find(b, e, state);
    if (it != e)
        m_list711ec.erase(it);
}

// ============================================================================
// ORIG T 0x0867ceae：全部角色 m_needCheckOverEquipItem（+0xe3b）置 1。
// ============================================================================
void CUser::SetNeedCheckOverEquipItem()
{
    // ORIG 0x08??：operator[] 结果存 -0xc 槽（i=-0x10 先声明远槽，技巧 21）：
    // `mov %eax,-0xc; cmpl $0,-0xc; je NEXT; mov -0xc,%eax; movb $1,0xe3b(%eax)`
    // ——单次 call + 具名引用中转（双 call 形反产 test;setne 物化+二次 sret）。
    for (unsigned int i = 0; i < m_characList.size(); ++i)
    {
        _Charac_info& r = m_characList[i];
        // P223（b28 BL9 实证形）：`if(c==e) goto L; body; L:;` 精确复现
        // ORIG `je 岛; body; jmp; nop` 空 then 占位岛。
        if (&r == 0)
            goto NEXT;
        r.m_needCheckOverEquipItem = 1;
    NEXT:;
    }
}
// ============================================================================
// LinkMisc5 批次（散点链接期未定义符号，第四阶）
// ============================================================================

// ---- CSecurityCard 状态 getter（ORIG W 0x0822ef0e/0x0822ef1a/0x0822ef26，
//      objdump 忠实转写；声明补于 CUser.h WongWork::CSecurityCard）----
bool WongWork::CSecurityCard::isActivate() const
{
    return m_activate2f;   // +0x2f 激活标志（bool 直返：ORIG movzbl 直返）
}

bool WongWork::CSecurityCard::isCertified() const
{
    return m_certified30;  // +0x30 认证标志（bool 直返：ORIG movzbl 直返）
}

int WongWork::CSecurityCard::getFailCnt()
{
    return m_failCnt34;         // +0x34 失败计数
}

// ============================================================================
// ORIG T 0x0868991c _ZN5CUser17isTradePunishTypeEi：
//   type==0 → m_tradePunishType(+0x79628)!=0；否则按位与命中。
// ============================================================================
bool CUser::isTradePunishType(int type)
{
    if (type == 0 && m_postChecksumData.m_tradePunishType != 0)
        return 1;
    // ORIG 0x8689949 je 尾岛（0x8689952 mov $0）：命中 fall `mov $1`（probe12 B 反向布局）
    if (type & m_postChecksumData.m_tradePunishType)
        return 1;
    return 0;
}

// ============================================================================
// ORIG T 0x086624f8 sz 0x30 _ZN5CUser15send_RedeemInfoEv：
//   GetCharacExpandData(6) 取 Redeem_Item::CRedeemItem 后 Send_RedeemList。
// ============================================================================
void CUser::send_RedeemInfo()
{
    Redeem_Item::CRedeemItem* redeem =
        (Redeem_Item::CRedeemItem*)GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)6);
    redeem->Send_RedeemList(this);
}

// ============================================================================
// ORIG T 0x08664ae8 sz 0x167 _ZN5CUser13calc_lev_statER15_Additioal_info：
//   基础块 add 后按当前等级循环叠加成长属性块（≤14 首转/≤49 二转/之后三转）。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08664ae8 | _ZN5CUser13calc_lev_statER15_Additioal_info
void CUser::calc_lev_stat(_Additioal_info& info)
{
    // ORIG：G_CDataManager() 在基础 add/get_growtype_stat×(3) 四处各自重调，
    // getCurCharFirstGrowType 重调两次——访问器结果禁缓存（战术 P183/P48）。
    info.add(*(const _Additioal_info*)
        &G_CDataManager()->m_characList->m_characs[get_charac_job()]);

    const void* statBase = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()].get_growtype_stat(0, 0);          // 无成长
    const void* statFirst = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()]
            .get_growtype_stat((signed char)getCurCharFirstGrowType(), 0);  // 一转
    int secondGrow = (signed char)getCurCharSecondGrowType();
    const void* statSecond = G_CDataManager()->m_characList
        ->m_characs[get_charac_job()].get_growtype_stat(
            (signed char)getCurCharFirstGrowType(), secondGrow);        // 二转

    for (int level = 1; get_charac_level() > level; ++level)
    {
        if (level <= 14)
            info.add(*(const _Additioal_info*)statBase);
        else if (level <= 49)
            info.add(*(const _Additioal_info*)statFirst);
        else
            info.add(*(const _Additioal_info*)statSecond);
    }
}

// ============================================================================
// ORIG T 0x0868ff04 sz 0x9c _ZN5CUser21CalcurateUserMaxLevelEv：
//   全角色 m_level 最大值 clamp 到 70 后 SetUserMaxLevel。
// ============================================================================
void CUser::CalcurateUserMaxLevel()
{
    int maxLevel = 0;
    // ORIG 0x867f??：`cmp -0xc(%ebp),%eax; setg`（signed 比较，i 为 int）+
    // operator[] 实参 idx 先载 %eax、this 走 %edx mov+add（技巧 15 失配序）。
    for (int i = 0; i < (int)m_characList.size(); ++i)
    {
        if ((int)m_characList[i].m_level > maxLevel)
            maxLevel = m_characList[i].m_level;
    }
    if (maxLevel > 70)
        maxLevel = 70;
    SetUserMaxLevel(maxLevel);
}

// ============================================================================
// ORIG T 0x0868e704 sz 0x2bb _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard：
//   管理等级/点数 + 雇佣兵列表逐个序列化（剩余时间、状态、名字、三状态字节）。
// ============================================================================
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0868e704 | _ZN5CUser22SetMercenaryInfoPacketER11PacketGuard
void CUser::SetMercenaryInfoPacket(PacketGuard& packet)
{
    bool leveledUp = CalcurateManageLevel();
    if (leveledUp)
        SendManageLevelUpNoti();

    packet.put_byte(GetManageLevel());
    packet.put_int(GetManagePoint());
    packet.put_byte((signed char)m_mercenaryMgr.m_characs.size());

    for (unsigned int i = 0; i < m_mercenaryMgr.m_characs.size(); ++i)
    {
        CHARAC_LOAD_MERCENARY& merc = m_mercenaryMgr.m_characs[i];

        // 剩余时间 = 到期秒 - 当前秒
        int remainSec =
            merc.m_field2c - GlobalData::s_systemTime_.getCurSec();

        packet.put_int(merc.m_characNo);

        int nameLen;
        if (&merc.m_name[0] == 0)
            nameLen = 0;
        else
            nameLen = strlen(merc.m_name);
        packet.put_int(nameLen);
        if (nameLen != 0)
            packet.put_str(merc.m_name, nameLen);

        if (remainSec > 0)
            packet.put_byte(1);
        else if (merc.m_field30 == 0xff)
            packet.put_byte(0);
        else
            packet.put_byte(2);

        packet.put_int(remainSec);
        packet.put_byte((signed char)merc.m_field30);
        packet.put_byte((signed char)merc.m_field31);
        packet.put_byte((signed char)merc.m_avatarBonus);
    }
}
// ============================================================================
// ORIG T 0x0868fec8 _ZN5CUser14SetUserMaxLevelEi：写 m_field8ebd8（0 → 1）。
// ============================================================================
void CUser::SetUserMaxLevel(int level)
{
    // ORIG 0x0868fec8：>85 clamp 到 85；>0 取 level；否则 1。
    if (level > 85)
        m_field8ebd8 = 85;
    // ORIG 0x868fee4 `jg 868fef5(m=level)`：假路 fall=m=1 ⇒ 源码用反向 `<=0`（技巧 17，4.4 对 if(<=0){1}else{v} 产 jg 前进跳）
    else if (level <= 0)
        m_field8ebd8 = 1;
    else
        m_field8ebd8 = level;
}

// ============================================================================
// ORIG T 0x0868fe2c _ZN5CUser14SetManageLevelEi：超上限取脚本 m_manageLevelMax。
// ============================================================================
// b28d-solo-CUser AE: ORIG 0x0868fe32-0x0868fe61 `cmp slot,%eax; setl %al; test %al,%al; je`
//   无 bool 槽（sub $0x8）；具名 less 落槽（-0x9）⇒ 删局部内联（probe_bool a6/a7 对照）。
//   条件/体各完整重估 G_CDataManager() 链（P183）。| _ZN5CUser14SetManageLevelEi
void CUser::SetManageLevel(int level)
{
    if (G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax < level)
        m_field8ebd0 = G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax;
    else
        m_field8ebd0 = level;
}
// ============================================================================
// ORIG T 0x0868fe72 _ZN5CUser14SetManagePointEi：点数封顶升级所需后写入。
// ============================================================================
void CUser::SetManagePoint(int point)
{
    // ORIG 0x868fe79-0x868fe9f：两次 G_CDataManager call（不折叠）；lvmax 先落
    // -0x10 槽再 `mov -0x10,%ebx`（跨 call callee-saved 持值，技巧 2/31），实参
    // `mov %ebx,0x4(%esp)` 先于 `mov %eax,(%esp)`。
    // b28d-solo-CUser N封顶: ORIG 0x868fe79 值落 -0x10 槽再 `mov -0x10,%ebx` 跨 call
    //   持值（push ebx/sub $0x24）；4.4 三形实测：具名→双槽(-0x14/-0x10)多中转、
    //   裸 register→直入 %ebx 免槽（无 push ebx）、语句表达式→值内联更差 ⇒ N7/N18a
    //   寄存器分配族（register 形为最近形 our=26 vs orig=27）。
    register unsigned int lvmax =
        G_CDataManager()->m_altarConfig.m_characManageScript->m_manageLevelMax;
    int require = G_CDataManager()->m_altarConfig.m_characManageScript
                      ->GetLevelUpRequirePoint(lvmax);
    if (point > require)
        point = require;
    m_field8ebd4 = point;
}

// ============================================================================
// ORIG T 0x0868ffa0 sz 0x86 _ZN5CUser20CalcurateManagePointEv：
//   Σ 各角色等级对应管理点数后 SetManagePoint。
// ============================================================================
void CUser::CalcurateManagePoint()
{
    // ORIG 0x0868ffa0：total=-0x10（先声明远槽）/i=-0xc（int，`cmp -0xc,%eax; setg`
    // 有符号比较）/level 无槽直落 %ebx（`movzwl 0x27,%eax; movswl %ax,%ebx` 跨
    // G_CDataManagerv 存活）。short→int 拓宽作为 GetPoint 实参**直传**（probe s3）；
    // 经 `register int asm("ebx")` 中间形反产 cwtl+mov。
    int total = 0;
    for (int i = 0; i < (int)m_characList.size(); ++i)
    {
        total += G_CDataManager()->m_altarConfig.m_characManageScript->GetPoint(
            m_characList[i].m_level);
    }
    SetManagePoint(total);
}

// ============================================================================
// ORIG T 0x08690026 sz 0x25 _ZN5CUser20CalcurateManageLevelEv：
//   重算点数、查等级、SetManageLevel；返回等级是否提升。
// ============================================================================
bool CUser::CalcurateManageLevel()
{
    CalcurateManagePoint();
    // ORIG 0x8690b??：newLevel 槽 -0x10；GetManageLevel() 结果**不落槽**直接
    // `cmp -0x10,%eax; setl;test;je`（真分支 movb $1,-0x9）；后 SetManageLevel；
    int newLevel = G_CDataManager()->m_altarConfig.m_characManageScript->GetLevel(
        GetManagePoint());
    bool up = false;
    if (GetManageLevel() < newLevel)
        up = true;
    SetManageLevel(newLevel);
    return up;
}

// ============================================================================
// ORIG T 0x08690b58 _ZN5CUser21SendManageLevelUpNotiEv：空包 header(0,0x19f)。
// ============================================================================
void CUser::SendManageLevelUpNoti()
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x19f);
    packet.finalize(true);
    Send(packet);
}

// ============================================================================
// CUser::ReturnToSelectCharacList @ 0x08686fee（ReturnToSelectCharacList.md）
//   返回角色选择列表：成员税上报 → 各管理器 CheckOut → 私商/突袭/专家/在线
//   团队离场 → 播放时间落库 → 复仇数据 → DB 日志/更新 → 登出缓存 → 离开
//   世界 → 重置角色/任务 → ARAD 周期数据清理 → 角色视图复位。
// ============================================================================
// 以下依赖符号均为 ORIG 真实符号；类/实现由对应域批次提供：
//   DB_UpdateCharacPlayTime::makeRequest(int,long)（DB 域）
//   DB_UpdateRevengeData::makeRequest(uint,uchar,uchar)（DB 域）
//   expert_job::CExpertJobMgr::OnLeaveUser(CUser*)（专家批次）
//   online_preliminary::COnlinePreliminaryTeamMgr::OnLeaveUser(CUser*)（在线批次）
//   WongWork::CHandlePremium::handleReturn2CharacView(CUser*)（premium 批次）
//   ARAD::Singleton<Arad_DataManager>::Get()（ARAD 域；SPECIAL）
// ============================================================================
// 注：以下依赖符号（DB_UpdateCharacPlayTime/DB_UpdateRevengeData、
// expert_job::CExpertJobMgr::OnLeaveUser、online_preliminary::
// COnlinePreliminaryTeamMgr::OnLeaveUser、WongWork::CHandlePremium::
// handleReturn2CharacView）均为 ORIG 真实符号，但对应权威类声明缺方法 /
// DB 请求类未建模，属外域缺口（BLOCKED，见报告）；其调用点在下方保留为
// 真实调用，由对应域批次补权威声明后即可编译。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x08686fee | _ZN5CUser24ReturnToSelectCharacListEb
void CUser::ReturnToSelectCharacList(bool param)
{
    UpdateCurCharacVisibleToCharacViewMemory();
    WorkPerFiveMin();

    int memberKey = get_charac_memberkey();
    if (memberKey != 0)
    {
        int payMoney = get_member_pay_tex_money_to_upper();
        int payFatigue = get_member_pay_tex_fatigue_to_upper();
        unsigned int characNo = getCurCharacNo();
        bool isUpper = is_connect_upper_member();
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetServerGroup());
        proxy->SendMemberPayTax(isUpper, characNo, payMoney, payFatigue);
        if (payMoney > 0)
            set_member_pay_tex_money_to_upper(0);
        if (payFatigue > 0)
            set_member_pay_tex_fatigue_to_upper(0);
    }

    CGameManager* gm = G_CGameManager();
    set_uppermember_exp_level(0);
    set_connect_upper_member(0);
    gm->CheckOutTrade(this);
    gm->CheckOutParty(this, false);
    gm->CheckOutPvp(this, false);
    gm->CheckOutWarRoom(this);
    gm->checkOutBlueMarble(this);

    private_store::CPrivateStoreMgr* psMgr = private_store::GetInstancePrivateStoreMgr();
    (*(void(**)(private_store::CPrivateStoreMgr*, CUser*))(*(void**)psMgr))(psMgr, this);

    	pvp_assault::CAssaultMgr* assaultMgr = pvp_assault::GetInstanceAssaultMgr();
	assaultMgr->OnLeaveAssaultPlace(this, true);
	GlobalData::s_ExpertJobMgr->OnLeaveUser(this);
	long playTick = GetCharacPlayTick();
	int characNo = get_charac_no(-1);
	DB_UpdateCharacPlayTime::makeRequest(characNo, playTick);
	GlobalData::s_onlinePreliminaryTeamMgr->OnLeaveUser(this);
	unsigned int curCharacNo = getCurCharacNo();
	unsigned char villageCnt = GetCurVillageAttackCount();
	unsigned char revengeCnt = GetCurRevengeDungeonCount();
	if (IsVillageAttackDBUpdate())
		DB_UpdateRevengeData::makeRequest(curCharacNo, villageCnt, revengeCnt);

    DBUpdateDBLogItem();
    ResetDBLogItem();
    UpdateData();
    LogoutCachedCharacter(1);

    GameWorld* gw = G_GameWorld();
    gw->leave_game_world(this);
    ResetCharacExpandData();

    {
        std::string name("Arad_EventPeriodDataManager");
        ARAD::Arad_DataManager* adm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        ARAD::Arad_EventPeriodDataManager* epdm =
            (ARAD::Arad_EventPeriodDataManager*)adm->findGameScript(name);
        epdm->delUserRewardData(this);
    }

    ResetCurCharac();
    resetSaveFlags();
    set_state((ch_state)2);
    doLinkCharacDisconnect();

    GameWorld* gw2 = G_GameWorld();
    gw2->InsertLoginUser(this);
    m_inventoryMemory.reset();
    m_quest.reset();

    long now = (long)time(0);
    getPad()->setCheckTime(now);
    	CerashopAddRestrict::Manager::ClearBuyRestrictItem(this);
	WongWork::CHandlePremium::handleReturn2CharacView(this);

    if (param)
    {
        PacketGuard pg;
        pg.put_header(1, 7);
        pg.put_byte(1);
        pg.finalize(true);
        Send(pg);
    }
}

// ============================================================================
// InventoryMemory::reset @ 0x0822d472（W 0x83）：四段 memset（0x4a58/0x1905/
// 0x39aa/0x2438，合计 0xc13f），末字节 +0xc13f 不清，逐段忠实转写。
void InventoryMemory::reset()
{
    memset(m_items1, 0, 0x4a58);
    memset(m_items2, 0, 0x1905);
    memset(m_items3, 0, 0x39aa);
    memset(m_items4, 0, ARAD::DATABASE::sourceCargoSize);
}

// ============================================================================
// Sanicova::CPad::setCheckTime @ 0x084ebe20（W 0xe 字节）：写 +0x28 m_checkTime28。
// ============================================================================
namespace Sanicova
{
void CPad::setCheckTime(long t)
{
    m_checkTime28 = t;
}
}

// ============================================================================
// b28d-lane5 N封顶: P168a/P248 setcc 物化方向; N82/P112 槽号分配 | ORIG 0x08651628 | _ZN5CUser40UpdateCurCharacVisibleToCharacViewMemoryEv
void CUser::UpdateCurCharacVisibleToCharacViewMemory()
{
    CInventory* inven = getCurCharacInvenW();
    if (inven == 0)
        return;

    for (int i = 0; i < 12; ++i)                                 // ORIG 0..0xb
    {
        CInventory::AvatarVisibleData& vis = inven->m_avatarVisible[i];  // b28d-lane5: Main 代落匿名 struct 包裹（N84）
        if (inven->m_invenItem[i].m_addInfo == 0)
        {
            vis.init();                                          // ORIG 0x0865171f 调 init
        }
        else
        {
            // ORIG：GetClearAvatar(i) 在 if 条件与 else 分支各自重调，结果禁缓存
            // （战术 P183/P48）。
            if (inven->GetClearAvatar(i) == 0)
                vis.m_field0 = inven->m_invenItem[i].m_addInfo;
            else
                vis.m_field0 = inven->GetClearAvatar(i);

            // ORIG key = item+0x07（m_addInfo2）；GetExpansionInfo 返回 {short,short}
            const WongWork::CAvatarItemMgr* mgr = inven->GetAvatarItemMgrR();
            void* info = const_cast<WongWork::CAvatarItemMgr*>(mgr)
                             ->GetExpansionInfo(inven->m_invenItem[i].m_addInfo2);
            if (info != 0)
            {
                const short* expansion = static_cast<const short*>(info);
                vis.m_field4 = expansion[0];
                vis.m_field6 = expansion[1];
        }
    }
            }



}
// ---- CPacketTimeChecker（ORIG W 0x8696d5c reset / 0x8696d74 check）----
// 定义置于本 TU（与 W7_UserOnlyC.cpp 中 CPacketVerifier::Init 调用者分离）：
// 4.4 unit-at-a-time 对 TU 内可见 callee 塌缩 outgoing 帧（sub $0x4），
// ORIG 4.1 恒发 sub $0x18（探针 /tmp/nl4p/p6.cpp vs p7.cpp 同族实证）。
void CPacketTimeChecker::reset()
{
    m_lastTime = 0;
    m_count = 0;
}

bool CPacketTimeChecker::check()
{
    ++m_count;
    long now = time(0);
    // P59 极性：ORIG jge→后置赋值块，fall-through 为节流判断块。
    if (now - m_lastTime < validSecond_)
    {
        if (m_count > validCount_)
            return false;
    }
    else
    {
        m_lastTime = now;
        m_count = 0;
    }
    return true;
}

// ============================================================================
// nexon_packet::stHeader::stHeader（ORIG 0x086941cc W，magic=0xaa; length=0;
// cmd=0）。定义原在 nexon_packet.cpp，迁入本 TU（stPacketC1 的 callee）——
// stPacketC1 所在 TU 不再含 callee 定义体 ⇒ 4.4 unit-at-a-time 不塌缩 outgoing
// 帧，保持 ORIG sub $0x18（NL4/NL3 unit-at-a-time 实证）。
// ============================================================================
nexon_packet::stHeader::stHeader()
    : m_magic(0xaa), m_length(0), m_cmd(0)
{
}
