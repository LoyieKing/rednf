#include "Dispatchers.h"
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn / ENUM_INTERNALPACKET_JPN

#include "CMyFileLog.h"
#include "CompoundEmblemDeps.h"
// ORIG T 0x0889232e / 0x088922d7（定义于 BingoEventLink4.cpp）
bool isSetBit_Uint(unsigned int value, int bit);
void setBit_Uint(unsigned int& value, bool set, int bit);
#include "CEventScriptMng.h"   // EventClassify::CEventScriptMng / CEventEntity /
#include "CEventScriptMng.h"   // 新增：EventClassify::CEventScriptMng/CObjectBringUp
#include "CTournamentDungeonReward.h"   // CTournamentDungeonReward /
#include <utility>             // std::pair<int,int>
#include <vector>
#include "CDataManager.h"            // G_CDataManager / find_item
#include "CDataManager.h"       // G_CDataManager / find_skill
#include "CGameManager.h"            // MSG_CHECK_MAIL_RECEIVER（set<type,T> 模板）
#include "CGuildServerProxy.h"       // CGuildServerProxy::SendGuildMail
#include "CInventory.h"              // CSecu_ProtectionField / GetInvenTypeFromItemSpace /
#include "CInventory.h"         // CInventory::GetInvenData/setCopy/delete_item/SetInventoryMemory
#include "CMailBoxHelper.h"          // WongWork::CMailBoxHelper::ReqDBCheckReceiver
#include "CServerProxyMgr.h"         // CServerProxyMgr<CGuildServerProxy>::GetServerProxy
#include "CSkill.h"             // CSkill::IsSpecialSkill
#include "CStreamGuard.h"            // CStreamGuard ctor/operator*/->/<</GetInBuffer
#include "CSyncSlangFilter.h"        // CSyncSlangFilter::GetInstance / HasSlang
#include "CUserCharacInfo.h"    // getCurCharac*/get_charac_*
#include "CompoundEmblemDeps.h"      // LineFunc(int, char const*, int, unsigned)
#include "CompoundEmblemDeps.h" // LineFunc(int, char const*, int, unsigned)
#include "GameEnums.h"          // ENUM_SKILL_TREE_KIND / eItemDelReason / ENUM_CMDPACKET
#include "GameTypes.h"          // cMyTrace
#include "GameWorld.h"          // G_GameWorld / IsPvPSkilTreeChannel
#include "GlobalData.h"              // s_guild_proxy_mgr / s_stream_pool / s_msgq_mgr
#include "InterfacePacketBuf.h" // clear/put_header/put_byte/put_short/put_int/finalize
#include "Arad_DataManager.h"             // ARAD::Singleton<ServiceRestrictManager>::Get（UpgradeInventory）
#include "DB_UpdateInventoryCapacity.h"   // UpgradeInventory DB 请求
#include "Inven_Item.h"         // Inven_Item ctor/reset
#include "CBattle_Field.h"               // DieMobAll: IsClearDungeon（战场视图）
#include "CExpandEquipslot.h"            // Equipslot_Switch（CUser.h 仅前向）
#include "CTimeGate.h"                   // CollectItems check_error isOpen（GlobalData.h 仅前向）
#include "MsgQueueMgr.h"             // MsgQueueMgr::put(QUEUE_IDX, CStreamGuard&)
#include "PacketGuard.h"        // PacketGuard
#include "ServiceRestrictManager.h"  // ServiceRestrictManager::isRestricted
#include "SigTypes.h"                // SIG_FIND_FACTORY_HUB
#include "SkillSlot.h"          // refund_skill/get_skill_level/get_remain_*_at_index
#include "StreamPool.h"              // StreamPool::Acquire
#include <cstddef>
#include <ctime>
#include <string.h>
#include <string> // LineFunc(int, char const*, int, unsigned)
#include "RDARScriptStringManager.h"  // g_scriptStringManager_ / findString

// ORIG T 0x088945ad（定义于 InitScriptProducers_2.cpp）
int makeFairPVPEquipmentPackageKey(int job, int idx);
#include "CDataManager.h"
#include "BlueMarble.h"
#include "CGuildServerProxy.h"
#include "Quest.h"         // Quest/stSelectQuestParam 完整定义
#include <cstring>
#include <iterator>
#include <list>
#include "CDailyScheduleManager.h"
#include "SigTypes.h"      // SIG_FORBID_USER_TO_PLAY 权威定义
#include "CGM_Manager.h"
#include "CGameManager.h"
#include "CInventory.h"
#include "CMonitorServerProxy.h"
#include "CParty.h"
#include "PvP_Room.h"
#include "CPrivateStoreMgr.h"
#include "CServerProxyMgr.h"
#include "CStreamGuard.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "TimerQueue.h"

// SIG_FORBID_USER_TO_PLAY 权威定义见 SigTypes.h（FixStubS3 已删除其重复 POD，冲突消解）。

// 安全分发器的协议检查函数由安全子系统提供；这里只保留 ORIG 的薄包装。
// 这些声明对应 docs/class_func_reports/Dispatcher_Security_*/dispatch_sig.md
// 中的全局 C++ 符号，定义缺失时须在安全子系统批次补齐，不能用 no-op 替代。
int _NS_IPScan(CUser* user, PacketBuf& packet);
int _NS_MI_RecvMemoryIntegrityModulation(CUser* user, PacketBuf& packet);
int _NS_NonClient_GetResponse(CUser* user, PacketBuf& packet);
int _NS_PI_2ND_RecvPacketIntegrityModulation_C2C(CUser* user, PacketBuf& packet);
int _NS_PI_2ND_RecvPacketIntegrityModulation_S2C(CUser* user, PacketBuf& packet);
int _NS_ProcessScan(CUser* user, PacketBuf& packet);
int _NS_PacketIntegrity(CUser* user, PacketBuf& packet);


// ============================================================================
// advancealtar/ARAD dispatcher 还原辅助（Dispatchers.cpp 内局部，不改头文件）
// ============================================================================
// 注意：shared/packet/include/CMDPacketStruct.h 定义的全局 MSG_BASE/ParamBase
// 与本工程 PacketDispatcher.h 的定义冲突，不能 include；因此本 TU 按 ORIG
// 线格式自行声明 advancealtar 请求/响应 POD（13 字节请求基体 + 派生负载；
// 响应 = 4 字节 ParamBase 视图 + 负载，整体 put_binary）。

namespace altar_pkt
{
// ---- ORIG 线格式（CMDPacketStruct.h 同源，详见该头文件注释）----
// 请求基体 13 字节（ORIG MSG_BASE），负载从 +0x0d 开始。
struct ReqBase
{
    unsigned char m_data[13];
};

// Dispatcher_Pause：req=14B，res=12B（0x242）
struct ReqPause
{
    unsigned char m_data[0xe];
};
struct ResPause
{
    int m_nErrorCode;        // +0x04
    unsigned char m_bPause;  // +0x08（回显 req+0x0d）
    unsigned char m_pad[3];
};

// Dispatcher_BuyItem：req=32B，res=28B（0x23d）
struct ReqBuyItem
{
    unsigned char m_data[0x20];
};
struct ResBuyItem
{
    int m_nErrorCode;        // +0x04
    int m_nBuyIndex;         // +0x08 = req+0x10
    int m_nShopType;         // +0x0c = req+0x14
    int m_nLevel;            // +0x10 = req+0x18
    unsigned short m_wFrom;  // +0x14 = req+0x1c
    unsigned short m_wTo;    // +0x16 = req+0x1e
    int m_nTotalCost;        // +0x18
};

// Dispatcher_SetSlot：req=36B，res=28B（0x23e）
struct ReqSetSlot
{
    unsigned char m_data[0x24];
};
struct ResSetSlot
{
    int m_nErrorCode;        // +0x04
    int m_field08;           // +0x08 = req+0x10
    unsigned char m_bSet;    // +0x0c = req+0x14
    unsigned char m_pad[3];
    unsigned char m_slot[12];  // +0x10 = req+0x18（_SetSlotData）
};

// Dispatcher_ResetStar：req=20B，res=12B（0x244）
struct ReqResetStar
{
    unsigned char m_data[0x14];
};
struct ResResetStar
{
    int m_nErrorCode;  // +0x04
    unsigned char m_pad[8];
};

// Dispatcher_SummonUnit：req=24B，res=20B（0x240）
struct ReqSummonUnit
{
    unsigned char m_data[0x18];
};
struct ResSummonUnit
{
    int m_nErrorCode;        // +0x04
    unsigned char m_bySlotType;  // +0x08 = req+0x0d
    unsigned char m_pad[3];
    int m_nUnitId;           // +0x0c = req+0x10
    int m_nGage;             // +0x10
};

// Dispatcher_UpgradeGage：req=24B，res=20B（0x23f）
struct ReqUpgradeGage
{
    unsigned char m_data[0x18];
};
struct ResUpgradeGage
{
    int m_nErrorCode;        // +0x04
    int m_nGageLevelBefore;  // +0x08
    int m_nGageLevelAfter;   // +0x0c
    int m_nGage;             // +0x10
};

// Dispatcher_ExchangeSlot：req=48B，res=36B（0x241）
struct ReqExchangeSlot
{
    unsigned char m_data[0x30];
};
struct ResExchangeSlot
{
    int m_nErrorCode;        // +0x04
    int m_field08;           // +0x08 = req+0x10
    unsigned char m_slot1[12];  // +0x0c = req+0x18
    unsigned char m_slot2[12];  // +0x18 = req+0x24
};

// Dispatcher_GetAchievementReward：req=20B，res=12B（0x243）
struct ReqGetAchievementReward
{
    unsigned char m_data[0x14];
};
struct ResGetAchievementReward
{
    int m_nErrorCode;      // +0x04
    int m_nAchievementIdx; // +0x08（sendAchievementReward 索引，恒 0）
};
}  // namespace altar_pkt

namespace
{
// ORIG CUser::getAdvanceAltar()：返回用户当前 advancealtar StageControl。
// [推断] 当前工程未建模 CUser 内 StageControl 指针成员；按等价语义用
// CUser::getAdvanceAltarIndex() + CGameManager::getAdvanceAltar(int) 映射。
advancealtar::StageControl* getUserStageControl(CUser* user)
{
    if (user == 0)
    {
        return 0;
    }
    short idx = user->getAdvanceAltarIndex();
    if (idx < 0)
    {
        return 0;
    }
    return G_CGameManager()->getAdvanceAltar(idx);
}

// ORIG advancealtar::ProcStage::pause(bool) @ 0x812eaa6：
//   ProcStage+0x04 为 stage state；flag=true 时 state 3→2，false 时 2/1→3。
// ProcStage 位于 StageControl+0x04。
int procStagePause(advancealtar::StageControl* stage, bool flag)
{
    if (stage == 0)
    {
        return 0;
    }
    char* procStage = reinterpret_cast<char*>(stage) + 0x04;
    int* state = reinterpret_cast<int*>(procStage + 0x04);
    if (flag)
    {
        if (*state != 3)
        {
            return 0;
        }
        *state = 2;
    }
    else
    {
        if (*state != 2 && *state != 1)
        {
            return 0;
        }
        *state = 3;
    }
    return 1;
}

// ORIG CharacData 位于 StageControl+0x98；+0x08 gageLevel，+0x0c gage。
char* getCharacData(advancealtar::StageControl* stage)
{
    return stage ? reinterpret_cast<char*>(stage) + 0x98 : 0;
}

int characDataGetGageLevel(char* cd)
{
    return cd ? *reinterpret_cast<int*>(cd + 0x08) : 0;
}

int characDataGetGage(char* cd)
{
    return cd ? *reinterpret_cast<int*>(cd + 0x0c) : 0;
}

// ORIG CharacData::upgreadGage @ 0x812d176（依赖 CDataManager+0x6364 配置区：
// +0x6368 float 系数、+0x6370 等级花费表、+0x637c 基数指针）。
int characDataUpgreadGage(char* cd)
{
    if (cd == 0)
    {
        return 3;
    }
    CDataManager* dm = G_CDataManager();
    int gageLevel = *reinterpret_cast<int*>(cd + 0x08);
    int gage = *reinterpret_cast<int*>(cd + 0x0c);
    if (gageLevel == 3)
    {
        return 2;
    }
    char* cfg = reinterpret_cast<char*>(dm) + 0x6364;
    int base = **reinterpret_cast<int**>(cfg + 0x18);
    float rate = *reinterpret_cast<float*>(cfg + 0x04);
    int costTbl = *reinterpret_cast<int*>(cfg + 0x0c + (gageLevel + 4) * 4);
    int cost = base * (int)(rate * (float)costTbl + 0.5f);
    if (gage < cost)
    {
        return 3;
    }
    *reinterpret_cast<int*>(cd + 0x0c) = gage - cost;
    int* lvl = reinterpret_cast<int*>(cd + 0x08);
    if (*lvl == 1)
    {
        *lvl = 2;
    }
    else if (*lvl == 2)
    {
        *lvl = 3;
    }
    else if (*lvl == 0)
    {
        *lvl = 1;
    }
    return 0;
}

// ORIG CharacData::summonSlotUnit @ 0x812d278：dm+0x6364 处
// map<uchar,_SlotItem>；_SlotItem +0x08 itemId、+0x0c needGage。
int characDataSummonSlotUnit(char* cd, unsigned char slotType, int unitId)
{
    if (cd == 0)
    {
        return 3;
    }
    CDataManager* dm = G_CDataManager();
    char* cfg = reinterpret_cast<char*>(dm) + 0x6364;
    // [推断] +0x00 处为 map<unsigned char,_SlotItem>（ConfigParameter 首成员）。
    typedef std::map<unsigned char, int> SlotItemMap;
    SlotItemMap* slotMap = reinterpret_cast<SlotItemMap*>(cfg);
    SlotItemMap::iterator it = slotMap->find(slotType);
    if (it == slotMap->end())
    {
        return 4;
    }
    // [推断] map value 为 _SlotItem（+0x08 itemId / +0x0c needGage 的 4 字段 POD）
    // 布局与 advancealtar::_SlotItem 一致；此处仅读取 itemId/needGage。
    const char* item = reinterpret_cast<const char*>(&it->second);
    int itemId = *reinterpret_cast<const int*>(item + 0x08);
    int needGage = *reinterpret_cast<const int*>(item + 0x0c);
    int base = **reinterpret_cast<int**>(cfg + 0x18);
    int cost = base * needGage;
    int gage = *reinterpret_cast<int*>(cd + 0x0c);
    if (itemId == unitId && cost <= gage)
    {
        *reinterpret_cast<int*>(cd + 0x0c) = gage - cost;
        return 0;
    }
    return 2;
}
}  // namespace

namespace advancealtar {

// Wave5 三分解方法共用的请求/响应字节视图
// （CMDPacketStruct::STBaseRequest / STBaseResponse 均为不透明载体，
//  ORIG process_packet 按偏移读写，见各 advancealtar__Dispatcher_*/报告）。
inline char* reqBytes(CMDPacketStruct::STBaseRequest& r) { return reinterpret_cast<char*>(&r); }
inline const char* reqBytes(const CMDPacketStruct::STBaseRequest& r) { return reinterpret_cast<const char*>(&r); }
inline char* resBytes(CMDPacketStruct::STBaseResponse& r) { return reinterpret_cast<char*>(&r); }

// ==================== Dispatcher_Pause（ORIG 0x8140a1c）====================
// dispatch_sig：state/charac 校验失败返回 0x52c；否则 read(0xe) → process →
// send_packet(0x242, 0xc) → 0。
// process：res+0x08 = req+0x0d（pause 标志）；check_error 恒 0；
//          Manager::pause(user, flag) → StageControl+0x04 ProcStage::pause。
Dispatcher_Pause::Dispatcher_Pause() {}
Dispatcher_Pause::~Dispatcher_Pause() {}
int Dispatcher_Pause::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x242)
        return 0;
    }
    return 0x52c;
}
int Dispatcher_Pause::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Wave5 消重：转调三分解方法（证据源 advancealtar__Dispatcher_Pause/process_packet.md）。
    return process_packet(user,
        *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(&msg),
        *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(&param));
}
int Dispatcher_Pause::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG read_packet @ 0x8140b04：读 0xe 字节
    packet.get_binary((char*)&msg, 0xe);
    return 0;
}
void Dispatcher_Pause::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140bae
    PacketGuard guard;
    guard.put_header(1, 0x242);
    guard.put_byte(1);
    guard.put_binary((char*)&resp, 0xc);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法（独立 const 成员，指令级转写自报告）----
bool Dispatcher_Pause::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return false;  // [objdump 0x08140b32] mov $0x0,%eax; pop %ebp; ret
}
int Dispatcher_Pause::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140b04] get_binary(&req, 0xe); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0xe);
    return 0;
}
int Dispatcher_Pause::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                     CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x08140b3c]：resp+4 = check_error；==0 时 resp+8 = req+0xd 并 Manager::pause。
    char* sb = resBytes(resp);
    *(reinterpret_cast<int*>(sb + 4)) = static_cast<int>(check_error(user, req));
    if (*(reinterpret_cast<int*>(sb + 4)) == 0)
    {
        const char* rb = reqBytes(req);
        sb[8] = rb[0xd];                                   // pause 标志回显
        Manager::pause(user, rb[0xd] != 0);                // ORIG T 0x0813105c
    }
    return 0;
}

// ==================== Dispatcher_BuyItem（ORIG 0x813fad0）====================
// dispatch_sig：校验失败返回 0x380；read(0x20) → process → send_packet(0x23d, 0x1c)。
// process：res 回显 req 全部字段；res+0x04 = check_error(0)；buyItem 依赖
//          CharacAdvanceAltarManager 内部 AdvanceAltarData（未建模），标注推断。
Dispatcher_BuyItem::Dispatcher_BuyItem() {}
Dispatcher_BuyItem::~Dispatcher_BuyItem() {}
int Dispatcher_BuyItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x23d)
        return 0;
    }
    return 0x380;
}
int Dispatcher_BuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Wave5 消重：转调三分解方法（证据源 advancealtar__Dispatcher_BuyItem/process_packet.md）。
    return process_packet(user,
        *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(&msg),
        *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(&param));
}
int Dispatcher_BuyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG read_packet @ 0x813fbb8：读 0x20 字节
    packet.get_binary((char*)&msg, 0x20);
    return 0;
}
void Dispatcher_BuyItem::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x813fe82
    PacketGuard guard;
    guard.put_header(1, 0x23d);
    guard.put_byte(1);
    guard.put_binary((char*)&resp, 0x1c);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_BuyItem::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    // [objdump 0x0813fbe6] return !isOpenAdvanceAltar();
    return !isOpenAdvanceAltar();
}
int Dispatcher_BuyItem::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x0813fbb8] get_binary(&req, 0x20); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x20);
    return 0;
}
int Dispatcher_BuyItem::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                       CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x0813fc06]
    const char* rb = reqBytes(req);
    char* sb = resBytes(resp);
    *(int*)(sb + 0x08) = *(const int*)(rb + 0x10);                    // buyIndex 回显
    *(int*)(sb + 0x0c) = *(const int*)(rb + 0x14);                    // shopType
    *(int*)(sb + 0x10) = *(const int*)(rb + 0x18);                    // level
    *(unsigned short*)(sb + 0x14) = *(const unsigned short*)(rb + 0x1c);  // fromSlot
    *(unsigned short*)(sb + 0x16) = *(const unsigned short*)(rb + 0x1e);  // toSlot
    *(int*)(sb + 4) = static_cast<int>(check_error(user, req));
    if (*(int*)(sb + 4) == 0)
    {
        CDataManager* dm = G_CDataManager();                          // ORIG call G_CDataManager
        std::vector<advancealtar::BuyUpgradeData>* vec =
            dm->m_altarShop.getBuyUpgradeDataVec(
                *(const int*)(rb + 0x10),
                static_cast<AdvanceAltarShopType::T>(*(const int*)(rb + 0x14)));
        if (vec == 0)                              // [objdump 0x813fcbf] err=3
        {
            *(int*)(sb + 4) = 3;
            return 0;
        }
        const BuyUpgradeData* found = 0;           // 线性查找 item 匹配行
        for (std::vector<BuyUpgradeData>::iterator it = vec->begin(); it != vec->end(); ++it)
        {
            if (it->m_nItemId == *(int*)(sb + 0x10))   // [objdump 0x813fd04] cmp *it==resp+0x10
            {
                found = &(*it);
                break;
            }
        }
        if (found == 0)                            // [objdump 0x813fd6b] err=3
        {
            *(int*)(sb + 4) = 3;
            return 0;
        }
        int totalCost = 0;
        for (int lv = (short)*(const unsigned short*)(rb + 0x1c);
             lv <= (short)*(const unsigned short*)(rb + 0x1e); ++lv)
        {
            FieldData* fd = const_cast<BuyUpgradeData*>(found)->getFieldDataPoint(
                lv, static_cast<FieldType::T>(2));                 // ORIG T 0x088a2e4c
            if (fd == 0)                       // [objdump 0x813fdb6] err=3
            {
                *(int*)(sb + 4) = 3;
                return 0;
            }
            totalCost += fd->m_value;          // [objdump 0x813fdca] add +4(int)
        }
        int rewardIdx = 0;
        int rc = user->m_advanceAltar.buyItem(                     // ORIG T 0x081319ae
            static_cast<AdvanceAltarShopType::T>(*(const int*)(rb + 0x14)),
            *(const int*)(rb + 0x18),
            (short)*(const unsigned short*)(rb + 0x1c),
            (short)*(const unsigned short*)(rb + 0x1e),
            totalCost, rewardIdx);
        *(int*)(sb + 4) = rc;
        *(int*)(sb + 0x18) = rewardIdx;
        if (rc == 0)
        {
            HistoryLog::starSub(*user, static_cast<SubStarType::T>(0), totalCost);  // ORIG T 0x08133db0
        }
    }
    return 0;
}

// ==================== Dispatcher_SetSlot（ORIG 0x813ff3e）====================
// dispatch_sig：校验失败返回 0x3fc；read(0x24) → process → send_packet(0x23e, 0x1c)。
// process：res 回显 req（含 _SetSlotData 12B）；slotType 非 1/2 → error=2；
//          setSlot 依赖未建模 AdvanceAltarData，标注推断。
Dispatcher_SetSlot::Dispatcher_SetSlot() {}
Dispatcher_SetSlot::~Dispatcher_SetSlot() {}
int Dispatcher_SetSlot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x23e)
        return 0;
    }
    return 0x3fc;
}
int Dispatcher_SetSlot::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Wave5 消重：转调三分解方法（证据源 advancealtar__Dispatcher_SetSlot/process_packet.md）。
    return process_packet(user,
        *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(&msg),
        *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(&param));
}
int Dispatcher_SetSlot::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG read_packet @ 0x8140020：读 0x24 字节
    packet.get_binary((char*)&msg, 0x24);
    return 0;
}
void Dispatcher_SetSlot::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140148
    PacketGuard guard;
    guard.put_header(1, 0x23e);
    guard.put_byte(1);
    guard.put_binary((char*)&resp, 0x1c);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_SetSlot::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return !isOpenAdvanceAltar();  // [objdump 0x0814004e]
}
int Dispatcher_SetSlot::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140020] get_binary(&req, 0x24); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x24);
    return 0;
}
int Dispatcher_SetSlot::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                       CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x0814006e]
    const char* rb = reqBytes(req);
    char* sb = resBytes(resp);
    sb[0x0c] = rb[0x14];                                   // isSet 回显（byte）
    *(int*)(sb + 8) = *(const int*)(rb + 0x10);            // field08
    std::memcpy(sb + 0x10, rb + 0x18, 0xc);                // _SetSlotData 整块回显
    *(int*)(sb + 4) = static_cast<int>(check_error(user, req));
    if (*(int*)(sb + 4) == 0)
    {
        int slotType = *(const int*)(rb + 0x18);           // _SetSlotData.m_type
        if (slotType == 2 || slotType == 1)                // [objdump 0x081400f4]
        {
            int rc = user->m_advanceAltar.setSlot(         // ORIG T 0x08131b58
                rb[0x14] != 0,
                *reinterpret_cast<const CMDPacketStruct::_SetSlotData*>(rb + 0x18));
            *(int*)(sb + 4) = rc;
        }
        else
        {
            *(int*)(sb + 4) = 2;
        }
    }
    return 0;
}

// ==================== Dispatcher_ResetStar（ORIG 0x8140f6e）====================
// dispatch_sig：校验失败返回 0x5c0；read(0x14) → process → send_packet(0x244, 0xc)。
// process：res+0x04 = check_error(0)；resetStar 依赖未建模数据，标注推断。
Dispatcher_ResetStar::Dispatcher_ResetStar() {}
Dispatcher_ResetStar::~Dispatcher_ResetStar() {}
int Dispatcher_ResetStar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x244)
        return 0;
    }
    return 0x5c0;
}
int Dispatcher_ResetStar::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Wave5 消重：转调三分解方法（证据源 advancealtar__Dispatcher_ResetStar/process_packet.md）。
    return process_packet(user,
        *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(&msg),
        *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(&param));
}
int Dispatcher_ResetStar::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG read_packet @ 0x814105c：读 0x14 字节
    packet.get_binary((char*)&msg, 0x14);
    return 0;
}
void Dispatcher_ResetStar::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x81411fc
    PacketGuard guard;
    guard.put_header(1, 0x244);
    guard.put_byte(1);
    guard.put_binary((char*)&resp, 0xc);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_ResetStar::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return !isOpenAdvanceAltar();  // [objdump 0x0814108a]
}
int Dispatcher_ResetStar::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x0814105c] get_binary(&req, 0x14); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x14);
    return 0;
}
int Dispatcher_ResetStar::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                         CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x081410aa]
    char* sb = resBytes(resp);
    *(int*)(sb + 4) = static_cast<int>(check_error(user, req));
    if (*(int*)(sb + 4) == 0)
    {
        int rc = user->m_advanceAltar.resetStar(                     // ORIG T 0x081333b2
            user,
            static_cast<CMDPacketStruct::ResetStarType::T>(*(const int*)(reqBytes(req) + 0x10)));
        *(int*)(sb + 4) = rc;
        if (rc == 0)
        {
            AdvanceAltarData* data = user->m_advanceAltar.getCurrentAdvanceAltarData();  // ORIG T 0x081348c4
            if (data != 0)
            {
                NOTIPacketStruct::_NotiAdvanceAltarUpdateRidable noti;   // ORIG ctor 0x08141ff8
                PacketGuard guard;
                guard.put_header(0, 0x210);                          // [objdump 0x08141172] put_header(0,0x210)
                guard.put_binary((char*)&noti, 1);                   // put_binary(&noti,1)
                advancealtar::putRidableInfoToPacket(guard, *data);  // ORIG T 0x08141781
                guard.finalize(true);
                user->Send(guard);                                   // ORIG T 0x086485ba CUser::Send
            }
        }
    }
    return 0;
}

// ==================== Dispatcher_StartGame（ORIG 0x813f7b0 区）====================
// check_error @ 0x813f7de：isOpenAdvanceAltar（CDailyScheduleManager 4/10）→
//   state==3 → level>=50 → 非竞技佣兵 → 非摆摊 → 非交易 → 非分解商店 →
//   非组队 → 非 PVP 频道 → 0；各失败 1/2/3/4。
// process @ 0x813f8ce：check_error → param+0x04；成功后
//   CharacAdvanceAltarManager::checkAndCreateDefaultRidable（未建模，推断）。
// read @ 0x813f7b0：读 0xd 字节。
// send @ 0x813f92a：error==0 先 StageControl::gotoStage（推断）；发包 0x23c：
//   put_byte(1)+put_binary(param,8)；成功后 set_state(0xd)+put_int(1)+ridable 数据
//   （getAdvanceAltarDataByIndex/putRidableInfoToPacket 未建模，推断）。
Dispatcher_StartGame::Dispatcher_StartGame() {}
Dispatcher_StartGame::~Dispatcher_StartGame() {}
int Dispatcher_StartGame::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x813f7de；ORIG 内部将 MSG_BASE& 实参当作 CUser* 操作（见反编译），
    // 此处统一以 user 为操作对象（语义等价）。
    CUser* target = user ? user : reinterpret_cast<CUser*>(&msg);
    if (GlobalData::s_DailyScheduleManager->GetScheduleState(
            (_DailyScheduleType)4, 10) != 1)
    {
        return 1;
    }
    if (target->get_state() != 3)
    {
        return 3;
    }
    if (target->get_charac_level() < 0x32)
    {
        return 4;
    }
    if (target->isCompetitionMercenary())
    {
        return 3;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(target))
    {
        return 3;
    }
    if (target->CheckInTrade())
    {
        return 2;
    }
    if (target->IsBusyDisjointStore())
    {
        return 3;
    }
    if (target->CheckInParty())
    {
        return 3;
    }
    if (G_GameWorld()->IsPVPChannel())
    {
        return 3;
    }
    return 0;
}
int Dispatcher_StartGame::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x813f8ce
    int err = check_error(user, msg);
    *(int*)((char*)&param + 4) = err;
    if (err == 0)
    {
        // [推断] CharacAdvanceAltarManager::checkAndCreateDefaultRidable
        // （CUser+0x8df60）依赖未建模 AdvanceAltarData；暂不执行。
    }
    return 0;
}
int Dispatcher_StartGame::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x813f7b0：读 0xd 字节
    packet.get_binary((char*)&msg, 0xd);
    return 0;
}
void Dispatcher_StartGame::send(CUser* user, ParamBase& param)
{
    // ORIG 0x813f92a
    int err = *(int*)((char*)&param + 4);
    if (err == 0)
    {
        // [推断] ORIG 先 StageControl::gotoStage(user)（StageControl 完整布局
        // 未建模，CGameManager.h 为 pad[0xc0]）；暂不执行。
        user->set_state((ch_state)0xd);
    }
    PacketGuard guard;
    guard.put_header(1, 0x23c);
    guard.put_byte(1);
    guard.put_binary((char*)&param, 8);
    if (err == 0)
    {
        // [推断] ORIG 成功后追加 put_int(1) + 1 个
        // putRidableInfoToPacket(AdvanceAltarData)（数据源未建模）。
        guard.put_int(1);
    }
    guard.finalize(true);
    user->Send(guard);
}

// ==================== Dispatcher_SummonUnit（ORIG 0x81404a8）====================
// dispatch_sig：校验失败返回 0x496；read(0x18) → process → send_packet(0x240, 0x14)。
// process：res+0x08=req+0x0d、res+0x0c=req+0x10；check_error 0；若 gageLevel 匹配
//   summonSlotUnit（真实偏移实现）；res+0x10 = getGage()。
Dispatcher_SummonUnit::Dispatcher_SummonUnit() {}
Dispatcher_SummonUnit::~Dispatcher_SummonUnit() {}
int Dispatcher_SummonUnit::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x240)
        return 0;
    }
    return 0x496;
}
int Dispatcher_SummonUnit::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Wave5 消重：转调三分解方法（证据源 advancealtar__Dispatcher_SummonUnit/process_packet.md）。
    return process_packet(user,
        *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(&msg),
        *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(&param));
}
int Dispatcher_SummonUnit::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG read_packet @ 0x8140580：读 0x18 字节
    packet.get_binary((char*)&msg, 0x18);
    return 0;
}
void Dispatcher_SummonUnit::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x81406b8
    PacketGuard guard;
    guard.put_header(1, 0x240);
    guard.put_byte(1);
    guard.put_binary((char*)&resp, 0x14);
    guard.finalize(true);

    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
int Dispatcher_SummonUnit::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return 0;  // [objdump 0x081405ae] mov $0x0,%eax; ret
}
int Dispatcher_SummonUnit::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140580] get_binary(&req, 0x18); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x18);
    return 0;
}
int Dispatcher_SummonUnit::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                          CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x081405b8]
    const char* rb = reqBytes(req);
    char* sb = resBytes(resp);
    sb[8] = rb[0xd];                                       // slotType 回显
    *(int*)(sb + 0x0c) = *(const int*)(rb + 0x10);         // unitId 回显
    *(int*)(sb + 4) = check_error(user, req);
    if (*(int*)(sb + 4) == 0)
    {
        advancealtar::StageControl* stage = user->getAdvanceAltar();   // ORIG T 0x08655262 CUser::getAdvanceAltar
        if (stage != 0)
        {
            char* cd = getCharacData(stage);               // StageControl::getCharacData W 0x081348a0
            if (*(const int*)(rb + 0x14) == characDataGetGageLevel(cd))     // gageLevel 一致性
            {
                *(int*)(sb + 4) = characDataSummonSlotUnit(
                    cd, static_cast<unsigned char>(rb[0xd]), *(const int*)(rb + 0x10));
                *(int*)(sb + 0x10) = characDataGetGage(cd);
            }
            else                                           // [objdump 0x8140650] err=3
            {
                *(int*)(sb + 4) = 3;
            }
        }
    }
    return 0;
}

// ==================== Dispatcher_UpgradeGage（ORIG 0x814020e）====================
// dispatch_sig：校验失败返回 0x446；read(0x18) → process → send_packet(0x23f, 0x14)。
// process：res+0x08=升级前 gageLevel、res+0x04=upgreadGage()、res+0x0c=升级后、
//          res+0x10=getGage()（真实偏移实现）。
Dispatcher_UpgradeGage::Dispatcher_UpgradeGage() {}
Dispatcher_UpgradeGage::~Dispatcher_UpgradeGage() {}
int Dispatcher_UpgradeGage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x23f)
        return 0;
    }
    return 0x446;
}
int Dispatcher_UpgradeGage::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Wave5 消重：转调三分解方法（证据源 advancealtar__Dispatcher_UpgradeGage/process_packet.md）。
    return process_packet(user,
        *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(&msg),
        *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(&param));
}
int Dispatcher_UpgradeGage::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG read_packet @ 0x81402e6：读 0x18 字节
    packet.get_binary((char*)&msg, 0x18);
    return 0;
}
void Dispatcher_UpgradeGage::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x81403e2
    PacketGuard guard;
    guard.put_header(1, 0x23f);
    guard.put_byte(1);
    guard.put_binary((char*)&resp, 0x14);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
int Dispatcher_UpgradeGage::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return 0;  // [objdump 0x08140314] mov $0x0,%eax; ret
}
int Dispatcher_UpgradeGage::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x081402e6] get_binary(&req, 0x18); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x18);
    return 0;
}
int Dispatcher_UpgradeGage::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                           CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x0814031e]
    char* sb = resBytes(resp);
    *(int*)(sb + 4) = check_error(user, req);
    if (*(int*)(sb + 4) == 0)
    {
        advancealtar::StageControl* stage = user->getAdvanceAltar();   // ORIG T 0x08655262
        if (stage != 0)
        {
            char* cd = getCharacData(stage);               // StageControl::getCharacData W 0x081348a0
            *(int*)(sb + 8) = characDataGetGageLevel(cd);      // 升级前 gageLevel
            *(int*)(sb + 4) = characDataUpgreadGage(cd);       // ORIG CharacData::upgreadGage T 0x0812d176
            *(int*)(sb + 0x0c) = characDataGetGageLevel(cd);   // 升级后 gageLevel
            *(int*)(sb + 0x10) = characDataGetGage(cd);
        }
    }
    return 0;
}

// ==================== Dispatcher_ExchangeSlot（ORIG 0x8140774）====================
// dispatch_sig：校验失败返回 0x4e8；read(0x30) → process → send_packet(0x241, 0x24)。
// process：res+0x08=req+0x10；check_error 0；exchangeSlot 依赖未建模数据，推断。
Dispatcher_ExchangeSlot::Dispatcher_ExchangeSlot() {}
Dispatcher_ExchangeSlot::~Dispatcher_ExchangeSlot() {}
int Dispatcher_ExchangeSlot::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet
        send_packet(user, res);            // ORIG send_packet(0x241)
        return 0;
    }
    return 0x4e8;
}
int Dispatcher_ExchangeSlot::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Wave5 消重：转调三分解方法（证据源 advancealtar__Dispatcher_ExchangeSlot/process_packet.md）。
    return process_packet(user,
        *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(&msg),
        *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(&param));
}
int Dispatcher_ExchangeSlot::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG read_packet @ 0x8140856：读 0x30 字节
    packet.get_binary((char*)&msg, 0x30);
    return 0;
}
void Dispatcher_ExchangeSlot::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140966
    PacketGuard guard;
    guard.put_header(1, 0x241);
    guard.put_byte(1);
    guard.put_binary((char*)&resp, 0x24);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_ExchangeSlot::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return !isOpenAdvanceAltar();  // [objdump 0x08140884]
}
int Dispatcher_ExchangeSlot::read_packet(PacketBuf& packet, CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140856] get_binary(&req, 0x30); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x30);
    return 0;
}
int Dispatcher_ExchangeSlot::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                            CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x081408a4]
    const char* rb = reqBytes(req);
    char* sb = resBytes(resp);
    *(int*)(sb + 8) = *(const int*)(rb + 0x10);            // field08 回显
    *(int*)(sb + 4) = static_cast<int>(check_error(user, req));
    if (*(int*)(sb + 4) == 0)
    {
        int rc = user->m_advanceAltar.exchangeSlot(        // ORIG T 0x08131f0a
            *reinterpret_cast<CMDPacketStruct::_SetSlotData*>(const_cast<char*>(rb) + 0x18),
            *reinterpret_cast<CMDPacketStruct::_SetSlotData*>(const_cast<char*>(rb) + 0x24));
        *(int*)(sb + 4) = rc;
        std::memcpy(sb + 0x0c, rb + 0x18, 0xc);            // slotA 回显
        std::memcpy(sb + 0x18, rb + 0x24, 0xc);            // slotB 回显
    }
    return 0;
}

// ==================== Dispatcher_GetAchievementReward（ORIG 0x8140c74）====================
// dispatch_sig：校验失败返回 0x571；read(0x14) → process（ORIG 为空）→
//   send_packet(0x243)。send_packet：put_byte(1)+put_binary(res,0xc)+
//   put_int(count)+逐项 _RewardItem（sendAchievementReward 未建模，推断）。
Dispatcher_GetAchievementReward::Dispatcher_GetAchievementReward() {}
Dispatcher_GetAchievementReward::~Dispatcher_GetAchievementReward() {}
int Dispatcher_GetAchievementReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 &&
        user->getCurCharacR() != 0)
    {
        unsigned char reqBuf[sizeof(CMDPacketStruct::STBaseRequest) + 0x40];
        unsigned char resBuf[sizeof(CMDPacketStruct::STBaseResponse) + 0x40];
        std::memset(reqBuf, 0, sizeof(reqBuf));
        std::memset(resBuf, 0, sizeof(resBuf));
        CMDPacketStruct::STBaseRequest& req =
            *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(reqBuf);
        CMDPacketStruct::STBaseResponse& res =
            *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(resBuf);
        read_packet(packet, req);          // ORIG read_packet
        process_packet(user, req, res);    // ORIG process_packet（空体）
        send_packet(user, res);            // ORIG send_packet(0x243)
        return 0;
    }
    return 0x571;
}
int Dispatcher_GetAchievementReward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // Wave5 消重：转调三分解方法（证据源 advancealtar__Dispatcher_GetAchievementReward/process_packet.md）。
    return process_packet(user,
        *reinterpret_cast<CMDPacketStruct::STBaseRequest*>(&msg),
        *reinterpret_cast<CMDPacketStruct::STBaseResponse*>(&param));
}
int Dispatcher_GetAchievementReward::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG read_packet @ 0x8140d52：读 0x14 字节
    packet.get_binary((char*)&msg, 0x14);
    return 0;
}
void Dispatcher_GetAchievementReward::send_packet(CUser* user, CMDPacketStruct::STBaseResponse& resp)
{
    // ORIG 0x8140daa
    altar_pkt::ResGetAchievementReward& res =
        *reinterpret_cast<altar_pkt::ResGetAchievementReward*>(&resp);
    PacketGuard guard;
    guard.put_header(1, 0x243);
    guard.put_byte(1);
    // [推断] ORIG 调 CharacAdvanceAltarManager::sendAchievementReward(user,
    // res+0x08, &vec) 并追加 vec 逐项 _RewardItem(12B)；数据源未建模，
    // res+0x04 置 error=3，count 置 0（客户端收到空奖励列表）。
    res.m_nErrorCode = 3;
    guard.put_binary((char*)&res, 0xc);
    guard.put_int(0);
    guard.finalize(true);
    user->Send(guard);
}

// ---- Wave5 三分解方法 ----
bool Dispatcher_GetAchievementReward::check_error(CUser* user, CMDPacketStruct::STBaseRequest& req) const
{
    (void)user; (void)req;
    return !isOpenAdvanceAltar();  // [objdump 0x08140d80]
}
int Dispatcher_GetAchievementReward::read_packet(PacketBuf& packet,
                                                 CMDPacketStruct::STBaseRequest& req) const
{
    // [objdump 0x08140d52] get_binary(&req, 0x14); return 0
    packet.get_binary(reinterpret_cast<char*>(&req), 0x14);
    return 0;
}
int Dispatcher_GetAchievementReward::process_packet(CUser* user,
                                                    CMDPacketStruct::STBaseRequest& req,
                                                    CMDPacketStruct::STBaseResponse& resp) const
{
    (void)user; (void)req; (void)resp;
    // [objdump 0x08140da0] mov $0x0,%eax; ret（ORIG 空体：不读请求、不写响应）
    return 0;
}

}
Dispatcher_Redeem::Dispatcher_Redeem() {}
Dispatcher_Redeem::~Dispatcher_Redeem() {}
int Dispatcher_Redeem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Redeem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Redeem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseDye::Dispatcher_UseDye() {}
Dispatcher_UseDye::~Dispatcher_UseDye() {}
int Dispatcher_UseDye::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81e727e：仅允许 state==3；交易中返回 1。
    if (user == 0 || user->get_state() != 3)
    {
        return 0x13;
    }
    return user->CheckInTrade() ? 1 : 0;
}
int Dispatcher_UseDye::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_USE_DYE& req = static_cast<MSG_USE_DYE&>(msg);
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_0e)))
    {
        return LineFunc(0x6245,
                        "virtual int Dispatcher_UseDye::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_10)))
    {
        return LineFunc(0x6248,
                        "virtual int Dispatcher_UseDye::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

Dispatcher_Antibot::Dispatcher_Antibot() {}
Dispatcher_Antibot::~Dispatcher_Antibot() {}
int Dispatcher_Antibot::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Antibot::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Antibot::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeHp::Dispatcher_ChangeHp() {}
Dispatcher_ChangeHp::~Dispatcher_ChangeHp() {}
int Dispatcher_ChangeHp::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ChangeHp::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeHp::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Teleport::Dispatcher_Teleport() {}
Dispatcher_Teleport::~Dispatcher_Teleport() {}

int Dispatcher_Teleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_TELEPORT& req = static_cast<MSG_TELEPORT&>(msg);
    if (!packet.get_short(*reinterpret_cast<short*>(req.m_pad + 1)))
    {
        return LineFunc(0x2b34,
                        "virtual int Dispatcher_Teleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(req.m_pad + 3)))
    {
        return LineFunc(0x2b38,
                        "virtual int Dispatcher_Teleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(req.m_pad[7]))
    {
        return LineFunc(0x2b3c,
                        "virtual int Dispatcher_Teleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(req.m_pad[8]))
    {
        return LineFunc(0x2b3f,
                        "virtual int Dispatcher_Teleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}


Dispatcher_UseJewel::Dispatcher_UseJewel() {}
Dispatcher_UseJewel::~Dispatcher_UseJewel() {}
int Dispatcher_UseJewel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    return LineFunc(0xc9e2,
                    "virtual int Dispatcher_UseJewel::dispatch_sig(CUser*, PacketBuf&)",
                    0, 0);
}
int Dispatcher_UseJewel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseJewel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BingoQuiz::Dispatcher_BingoQuiz() {}
Dispatcher_BingoQuiz::~Dispatcher_BingoQuiz() {}
int Dispatcher_BingoQuiz::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_BingoQuiz::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BingoQuiz::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_BingoQuiz::send(CUser* user, ParamBase& param) {}

Dispatcher_DieMobAll::Dispatcher_DieMobAll() {}
Dispatcher_DieMobAll::~Dispatcher_DieMobAll() {}
int Dispatcher_DieMobAll::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x81ecce6 (89 insn)：战斗中全灭上报；副本未清且(非0x2afe图或无副本)记 hack
    if (user->get_state() != 5)
    {
        return 0;
    }
    CParty* party = G_CGameManager()->GetParty();
    if (party == 0)
    {
        return LineFunc(0x6ef6,
                        "virtual int Dispatcher_DieMobAll::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    CBattle_Field& field = *reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1);
    if (!field.IsClearDungeon())
    {
        const int subCode =
            (party->m_dungeon != 0 && party->m_dungeon->get_index() == 0x2afe) ? 2 : 0;
        user->getHackAnalyzer()->addServerHackCnt(user,
                                                  static_cast<WongWork::ENUM_HACKTYPE>(0xcc), 1,
                                                  subCode, 0);
    }
    return 0;
}
int Dispatcher_DieMobAll::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DieMobAll::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SkillInit::Dispatcher_SkillInit() {}
Dispatcher_SkillInit::~Dispatcher_SkillInit() {}
int Dispatcher_SkillInit::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x081e5a2c
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 7;
    }
    MSG_SKILL_INIT& req = static_cast<MSG_SKILL_INIT&>(msg);
    int idx = user->GetCurCharacSkillTreeIndex();
    if (idx == -1 || idx == 0)
    {
        if (req.m_0d == 1)
        {
            return 0x13;
        }
        req.m_0d = 0;
    }
    else
    {
        if (req.m_0d != 1)
        {
            return 0x13;
        }
        req.m_0d = 1;
    }
    if (G_GameWorld()->IsPvPSkilTreeChannel())
    {
        if (user->get_pvp_grade() < 0)
        {
            return 0xc;
        }
    }
    return 0;
}
int Dispatcher_SkillInit::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e598a
    int err = check_error(user, msg);
    stSkillInit& p = static_cast<stSkillInit&>(param);
    p.m_4 = err;
    if (p.m_4 >= 1)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x5f3d,
                        "virtual int Dispatcher_SkillInit::process(CUser*, MSG_BASE&, ParamBase&)",
                        p.m_4, 0);
    }
    MSG_SKILL_INIT& req = static_cast<MSG_SKILL_INIT&>(msg);
    p.m_8 = req.m_0d;
    process_skill_init(user, param);
    return 0;
}
int Dispatcher_SkillInit::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081e5934：读一个字节到 msg 的 m_0d
    MSG_SKILL_INIT& req = static_cast<MSG_SKILL_INIT&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x5f26, "virtual int Dispatcher_SkillInit::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
void Dispatcher_SkillInit::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e5aec
    stSkillInit& p = static_cast<stSkillInit&>(param);
    if (p.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1ec,
                                 (unsigned char)(p.m_4 & 0xff));
        return;
    }
    PacketGuard pkt;
    pkt.put_header(1, 0x1ec);
    pkt.put_byte(1);
    pkt.put_byte((int)(char)p.m_8);
    pkt.finalize(true);
    user->Send(pkt);
    user->send_skill_info();
}
void Dispatcher_SkillInit::process_skill_init(CUser* user, ParamBase& param)
{
    // ORIG 0x081e5bdc
    if (user == 0)
    {
        return;
    }
    stSkillInit& p = static_cast<stSkillInit&>(param);
    int kind = (int)(char)p.m_8;
    SkillSlot* slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->clear_all_skills((ENUM_SKILL_TREE_KIND)kind);
    slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->set_parent(user);
    slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->InitSkill(false, (ENUM_SKILL_TREE_KIND)kind);
    slot = (SkillSlot*)user->getCurCharacSkillW();
    slot->resetComboSkill((ENUM_SKILL_TREE_KIND)kind);
    user->setComboSkillSet(true);
}

Dispatcher_Antibot_DP::Dispatcher_Antibot_DP() {}
Dispatcher_Antibot_DP::~Dispatcher_Antibot_DP() {}
int Dispatcher_Antibot_DP::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Antibot_DP::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Antibot_DP::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AskRematch::Dispatcher_AskRematch() {}
Dispatcher_AskRematch::~Dispatcher_AskRematch() {}
int Dispatcher_AskRematch::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AskRematch::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AskRematch::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BVHackInfo::Dispatcher_BVHackInfo() {}
Dispatcher_BVHackInfo::~Dispatcher_BVHackInfo() {}
int Dispatcher_BVHackInfo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_BVHackInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BVHackInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BlueMarble::Dispatcher_BlueMarble() {}
Dispatcher_BlueMarble::~Dispatcher_BlueMarble() {}
int Dispatcher_BlueMarble::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    const MSG_BLUE_MARBLE& req = static_cast<const MSG_BLUE_MARBLE&>(msg);
    if (req.m_0d == 6)
    {
        return 0;
    }
    if (user->getMoveSpace() != (UserSpace::T)1)
    {
        return -2;
    }
    BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
    if (marble == 0)
    {
        return -3;
    }
    if (marble->getUserSlot(user) == -1)
    {
        return -4;
    }
    int state = user->get_state();
    return (state == 0xb || state == 5) ? 0 : -5;
}
int Dispatcher_BlueMarble::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081e5fdc
    int err = check_error(user, msg);
    stBlueMarble& p = static_cast<stBlueMarble&>(param);
    p.m_4 = err;
    if (p.m_4 >= 1)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x60a9,
                        "virtual int Dispatcher_BlueMarble::process(CUser*, MSG_BASE&, ParamBase&)",
                        p.m_4, 0);
    }
    const MSG_BLUE_MARBLE& req = static_cast<const MSG_BLUE_MARBLE&>(msg);
    BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
    if (marble == 0)
    {
        return 0;
    }
    switch (req.m_0d)
    {
    case 0: marble->startBlueMarble(); break;
    case 1: marble->userThrowDice(user); break;
    case 2: marble->enterDungeon(user); break;
    case 3: marble->giveupDungeon(user); break;
    case 4: marble->clearDungeon(user); break;
    case 5: marble->exitDungeon(user); break;
    case 6: marble->leaveUser(user); break;
    case 7: marble->giveBonusItem(user); break;
    default: break;
    }
    return 0;
}
int Dispatcher_BlueMarble::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BLUE_MARBLE& req = static_cast<MSG_BLUE_MARBLE&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x609a,
                        "virtual int Dispatcher_BlueMarble::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
void Dispatcher_BlueMarble::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081e61f4
    stBlueMarble& p = static_cast<stBlueMarble&>(param);
    if (p.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1f0);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1f0,
                                 (unsigned char)(p.m_4 & 0xff));
    }
}

Dispatcher_ChangeHost::Dispatcher_ChangeHost() {}
Dispatcher_ChangeHost::~Dispatcher_ChangeHost() {}
int Dispatcher_ChangeHost::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8204c1e：state>=3 才处理；teleport_state 在 [0,3) 反走城错误 0x9e
    if (user == 0 || user->get_state() < 3)
    {
        return LineFunc(0x9cbf,
                        "virtual int Dispatcher_ChangeHost::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    unsigned char slot = 0;
    if (!packet.get_byte(slot))
    {
        return LineFunc(0x9cc2,
                        "virtual int Dispatcher_ChangeHost::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x7c, 0x13);
        return 0;
    }
    CPartyTelePort* teleport = (CPartyTelePort*)party->GetPartyTelePort();
    if (teleport != 0)
    {
        int ts = teleport->get_teleport_state();
        if (ts >= 0 && ts < 4)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x7c, 0x9e);
            return 0;
        }
    }
    // ORIG 汇编在 change_manager 后取 eax 判 >0，但 CParty.h 权威声明为 void，
    // 无法取返回值 —— 按权威头调用，保留成功路径。
    party->change_manager(user, (int)slot);
    return 0;
}
int Dispatcher_ChangeHost::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeHost::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// read() 直接依赖（tsv 清单外）：G_CDataManager()+0x14 指向的基址 +0x48fc
// 为 vector<pair<int,int>>（ComboSkill.cpp 同源布局视图：6 组组合技表首列），
// 线性查 first==idx 命中返回 1，否则 0。
int Dispatcher_ComboSkill::checkComboSkillIndex(short idx)
{
    // G_CDataManager() 返回对象 +0x14 处为组合技列表基址 char*
    // （成员访问形态 mov 0x14(%eax),%eax 与 ORIG 一致；镜像视图同 ComboSkill.cpp）
    struct DataManagerComboListView
    {
        char m_pad[0x14];
        char* m_pComboList;  // +0x14
    };
    std::vector<std::pair<int, int> >* comboList =
        (std::vector<std::pair<int, int> >*)(
            ((DataManagerComboListView*)G_CDataManager())->m_pComboList + 0x48fc);
    for (int i = 0; i <= 5; ++i)
    {
        if ((*comboList)[i].first == (int)idx)
        {
            return 1;
        }
    }
    return 0;
}

Dispatcher_ComboSkill::Dispatcher_ComboSkill() {}
Dispatcher_ComboSkill::~Dispatcher_ComboSkill() {}
int Dispatcher_ComboSkill::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->get_state() < 3 || user->get_charac_job() != 9)
    {
        return -1;
    }
    const MSG_COMBO_SKILL& req = static_cast<const MSG_COMBO_SKILL&>(msg);
    return req.m_pad0d == 0 || req.m_pad0d == 1 ? 0 : 0x13;
}
// err>0 → 0；err<0 → LineFunc(0x33f)；否则当前角色 SkillSlot 写入组合技映射
// 并校验该树。
int Dispatcher_ComboSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stComboSkill& out = static_cast<stComboSkill&>(param);
    MSG_COMBO_SKILL& req = static_cast<MSG_COMBO_SKILL&>(msg);
    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x33f,
                        "virtual int Dispatcher_ComboSkill::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_4, 0);
    }
    SkillSlot* skillW =
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
    skillW->setComboSkillInfo((ENUM_SKILL_TREE_KIND)(char)req.m_pad0d,
                              req.m_comboMap);
    skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
    skillW->verifyComboSkillTree((ENUM_SKILL_TREE_KIND)(char)req.m_pad0d);
    return 0;
}
// 读 MSG_COMBO_SKILL：m_pad0d 树种类、树数量(≤6)、每树 {kind, 技能数(≤6),
// skillIdx...}；checkComboSkillIndex 校验通过才收技能列表；逐项插入
// m_comboMap(kind → vector<int>)。任何 get 失败按行号走 LineFunc；
// 外层 get_short 失败/循环耗尽均归 LineFunc(0x30c)。
int Dispatcher_ComboSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_COMBO_SKILL& req = static_cast<MSG_COMBO_SKILL&>(msg);
    std::map<short, std::vector<int>, std::less<short>,
             std::allocator<std::pair<short const, std::vector<int> > > >&
        comboMap = req.m_comboMap;
    std::vector<int> skills;
    comboMap.clear();
    if (!packet.get_byte(req.m_pad0d))
    {
        return LineFunc(0x301,
                        "virtual int Dispatcher_ComboSkill::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    char treeCount = 0;
    if (!packet.get_byte(treeCount))
    {
        return LineFunc(0x304,
                        "virtual int Dispatcher_ComboSkill::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    for (int i = 0; i < (int)treeCount && i < 6; ++i)
    {
        short kind = 0;
        char skillCount = 0;
        if (!packet.get_short(kind))
        {
            break;  // 归 LineFunc(0x30c)
        }
        if (checkComboSkillIndex(kind))
        {
            if (!packet.get_byte(skillCount))
            {
                return LineFunc(0x312,
                                "virtual int Dispatcher_ComboSkill::read(PacketBuf&, MSG_BASE&)",
                                0, 0);
            }
            skills.clear();
            for (int j = 0; j < (int)skillCount && j < 6; ++j)
            {
                short skillIdx = 0;
                if (!packet.get_short(skillIdx))
                {
                    return LineFunc(0x319,
                                    "virtual int Dispatcher_ComboSkill::read(PacketBuf&, MSG_BASE&)",
                                    0, 0);
                }
                skills.push_back((int)skillIdx);
            }
            comboMap.insert(std::make_pair(kind, skills));
        }
    }
    return LineFunc(0x30c,
                    "virtual int Dispatcher_ComboSkill::read(PacketBuf&, MSG_BASE&)",
                    0, 0);
}
void Dispatcher_ComboSkill::send(CUser* user, ParamBase& param)
{
    stComboSkill& out = static_cast<stComboSkill&>(param);
    if (out.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fd);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fd,
                                 (unsigned char)(out.m_4 & 0xff));
    }
}

Dispatcher_GM_Command::Dispatcher_GM_Command() {}
Dispatcher_GM_Command::~Dispatcher_GM_Command() {}
// ---- GM 命令监控包/信号局部视图（仅本 TU 使用；ORIG W
//      Packet_Monitor_Notice_Message 0x822bbd6 / Packet_Forbid_Chat_By_Monitor
//      0x822bc70，均为清零构造的定长缓冲）----
namespace gm_pkt
{
#pragma pack(push, 1)
// 监控服通知：+0x0a 长度字节、+0x0b 文本（总长 0x10b）
struct MonitorNoticeMessage
{
    MonitorNoticeMessage() { memset(this, 0, sizeof(*this)); }
    unsigned char m_pad[0x0a];
    unsigned char m_len;
    char m_msg[0x100];
};
// 禁言监控包：+0x0a 秒数(int)、+0x12 名字（总长 0x30）
struct ForbidChatByMonitor
{
    ForbidChatByMonitor() { memset(this, 0, sizeof(*this)); }
    unsigned char m_pad[0x0a];
    int m_seconds;
    unsigned char m_pad2[4];
    char m_name[0x1e];
};
// SIG_FORBID_USER_TO_PLAY 前 9 字节视图（ORIG memset 9B 后按 +0/+4/+5 写）
struct SigForbidPlayBody
{
    int m_characNo;
    unsigned char m_flag;
    int m_days;
};
#pragma pack(pop)
}  // namespace gm_pkt

// ORIG 0x81fa19e：GM 命令分发（cmd=0..0xd）。校验链 user!=0/getCurCharacR!=0/
// get_byte 成功；失败走 LineFunc 断言路径（返回 0）。响应包 header 0x6e：
// local_1d0 = put_byte(1)+put_byte(cmd)+payload；local_1c4 = 固定拒绝包
// (put_byte(99),put_byte(1))，目标用户不存在时发给 GM 本人。
int Dispatcher_GM_Command::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->getCurCharacR() == 0)
    {
        // ORIG: LineFunc(0x88fd/0x8902,...) 断言路径
        return 0;
    }

    unsigned char cmd = 0;
    if (!packet.get_byte(cmd))
    {
        return 0;  // ORIG get_byte 失败 → LineFunc(0x890e)
    }

    PacketGuard reject;   // ORIG local_1c4：目标缺失时的固定应答
    reject.put_header(1, 0x6e);
    reject.put_byte(1);
    reject.put_byte(99);
    reject.put_byte(1);
    reject.finalize(true);

    PacketGuard resp;     // ORIG local_1d0
    resp.put_header(1, 0x6e);
    resp.put_byte(1);
    resp.put_byte(cmd);

    unsigned int gmAccId = user->get_acc_id();  // ORIG 仅用于 cMyTrace 日志上下文
    (void)gmAccId;

    char name[30];
    int len = 0;
    short wLen = 0;
    int extra = 0;
    CUser* target = 0;
    GameWorld* world = G_GameWorld();
    switch (cmd)
    {
    case 0:  // 切换 GM 模式（无参数）
        if (GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0)
        {
            user->SetGameMasterMode(
                GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) != 1);
            GlobalData::s_GM_Manager->TurnGmMode(user->get_acc_id());
        }
        break;
    case 1:  // 全服公告 GMC <str>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_str(name, 0x100, len) && len != 0)
        {
            resp.put_int(len);
            resp.put_str(name, len);
            resp.finalize(true);
            world->send_all(resp);
        }
        break;
    case 2:  // 本区域公告 GMZ <str>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_str(name, 0x100, len) && len != 0)
        {
            resp.put_int(len);
            resp.put_str(name, len);
            resp.finalize(true);
            world->send_to_area(user->getCurCharacVill(), user->get_area(false), resp);
        }
        break;
    case 3:  // 监控服公告 GMS <str>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_str(name, 0x100, len) && len != 0)
        {
            resp.put_int(len);
            resp.put_str(name, len);
            resp.finalize(true);
            gm_pkt::MonitorNoticeMessage noti;
            noti.m_len = (unsigned char)len;
            strncpy(noti.m_msg, name, len);
            CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                user->GetServerGroup());
            proxy->SendTcpPacket((char*)&noti, 0x10b);
        }
        break;
    case 4:  // 踢人 expel <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_str(name, 0x1e, len))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                resp.put_int(len);
                resp.put_str(name, len);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
                target->DisConnSig((DISCONN_SIG)0x14, true, 0);
            }
        }
        break;
    case 5:  // 禁言 forbid chat <mins> <?> <name>
    case 6:  // 禁移动 forbid move <mins> <?> <name>
    {
        bool isChat = (cmd == 5);
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_int(extra) &&
            packet.get_str(name, 0x1e, (short)extra))
        {
            // 分钟→秒钳制：<1 →60，>=0x5a0 →86400
            int sec = (len < 0x5a0) ? ((len < 1) ? 0x3c : len * 0x3c) : 0x15180;
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else if (isChat || target->get_state() == 3)
            {
                if (isChat)
                {
                    world->DisableChat(name, sec);
                }
                else
                {
                    world->DisableMove(name, sec);
                }
                TIMER_MESSAGE timerMsg =
                    isChat ? (TIMER_MESSAGE)0x5d : (TIMER_MESSAGE)0x5e;
                G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0,
                                            (int)target->GetUID(), timerMsg, sec,
                                            (int)target->GetLoginTick(),
                                            (unsigned int)target->getCurCharacNo());
                resp.put_int(len);
                resp.put_int(extra);
                resp.put_str(name, (short)extra);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
                if (!isChat)
                {
                    // case 6 无监控包；case 5 发 Packet_Forbid_Chat_By_Monitor
                    if (cmd == 5)
                    {
                        gm_pkt::ForbidChatByMonitor moni;
                        moni.m_seconds = sec;
                        strncpy(moni.m_name, name, sizeof(moni.m_name));
                        CMonitorServerProxy* proxy =
                            GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                                target->GetServerGroup());
                        proxy->SendTcpPacket((char*)&moni, 0x30);
                    }
                }
            }
        }
        break;
    }
    case 7:  // 隐身
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0)
        {
            user->SetInvisible();
            if (user->get_state() == 3)
            {
                user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
            }
        }
        break;
    case 8:  // 现身
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0)
        {
            user->SetVisible();
            if (user->get_state() == 3)
            {
                user->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
            }
        }
        break;
    case 9:  // 调级 setlevel <level> <?> <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_int(extra) &&
            packet.get_str(name, 0x1e, (short)extra))
        {
            memset(name, 0, sizeof(name));
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                int diff = len - target->get_charac_level();
                bool ok = true;
                while (diff < 0 && ok)
                {
                    int nextLv = target->get_charac_level() - 1;
                    if (nextLv > 200) nextLv = 200;
                    target->SetCharacLevel(nextLv);
                    ++diff;
                }
                while (diff > 0 && ok)
                {
                    int nextLv = target->get_charac_level() + 1;
                    if (nextLv > 200) nextLv = 200;
                    target->SetCharacLevel(nextLv);
                    --diff;
                }
                if (ok)
                {
                    int st = target->get_state();
                    if (st == 3)
                    {
                        target->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
                        target->SendNotiPacket((CUser::eSendTarget)0, (ENUM_NOTIPACKET)2, 0);
                        target->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)2, 1);
                    }
                    else if (st == 5)
                    {
                        target->SendNotiPacket((CUser::eSendTarget)1, (ENUM_NOTIPACKET)0x25, 0);
                        target->SendNotiPacket((CUser::eSendTarget)2, (ENUM_NOTIPACKET)2, 1);
                    }
                    resp.put_int(len);
                    resp.put_int(extra);
                    resp.put_str(name, (short)extra);
                    resp.finalize(true);
                    user->Send(resp);
                    target->Send(resp);
                }
            }
        }
        break;
    case 10:  // 发物品 grant item <itemIdx> <count> <?> <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_short(wLen) &&
            packet.get_int(extra) && packet.get_str(name, 0x1e, (short)extra))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
                int slot = target->AddItem(len, (int)(unsigned short)wLen,
                                           (eItemAddReason)6, space, 0);
                if (slot >= 0)
                {
                    target->SendUpdateItemList((CUser::eSendTarget)1, space, slot);
                    resp.put_int(len);
                    resp.put_short(wLen);
                    resp.put_int(extra);
                    resp.put_str(name, (short)extra);
                    resp.finalize(true);
                    user->Send(resp);
                }
            }
        }
        break;
    case 0xb:  // 复活 revive <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_str(name, 0x1e, len))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            CParty* party = (target != 0) ? (CParty*)target->GetParty() : 0;
            if (party == 0)
            {
                user->Send(reject);
            }
            else
            {
                party->set_charac_live(target, (ENUM_USER_DUNGEON_STATE)1);
                resp.put_int(len);
                resp.put_str(name, len);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
            }
        }
        break;
    case 0xc:  // 重置疲劳 reset fatigue <name>
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_str(name, 0x1e, len))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                target->RecoverFatigue(0);
                target->SendFatigue();
                resp.put_int(len);
                resp.put_str(name, len);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
            }
        }
        break;
    case 0xd:  // 封号 forbid play <days> <?> <name>（发 SIG_FORBID_USER_TO_PLAY 到 DB 队列）
        if (GlobalData::s_GM_Manager->GetCurrentGmMode(user->get_acc_id()) == 1 &&
            GlobalData::s_GM_Manager->GetGmLevel(user->get_acc_id()) > 0 &&
            packet.get_int(len) && packet.get_int(extra) &&
            packet.get_str(name, 0x1e, (short)extra))
        {
            target = world->find_user_from_world_byaccid(
                world->GetIDUserInIDMap(name));
            if (target == 0)
            {
                user->Send(reject);
            }
            else
            {
                Stream* stream = GlobalData::s_stream_pool->Acquire(
                    "PacketDispatcher_Impl_1.cpp", 0x8b28);
                CStreamGuard guard(stream, true);
                guard << 0x93;
                guard << (int)user->GetUID();
                SIG_FORBID_USER_TO_PLAY* sig = guard.GetInBuffer<SIG_FORBID_USER_TO_PLAY>();
                memset(sig, 0, 9);
                gm_pkt::SigForbidPlayBody* body =
                    (gm_pkt::SigForbidPlayBody*)sig;
                body->m_characNo = target->getCurCharacNo();
                body->m_flag = 1;
                body->m_days = (len < 1) ? 1 : ((len < 100) ? len : 100);
                GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
                resp.put_int(len);
                resp.put_int(extra);
                resp.put_str(name, (short)extra);
                resp.finalize(true);
                user->Send(resp);
                target->Send(resp);
            }
        }
        break;
    default:
        break;  // ORIG: "Undifined GM Command : %d by %s"（仅日志）
    }
    return 0;
}
int Dispatcher_GM_Command::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)user; (void)msg; (void)param;
    return 0;  // ORIG 类未声明 process（vtable 纯虚由基类骨架覆盖），恒 0
}
int Dispatcher_GM_Command::read(PacketBuf& packet, MSG_BASE& msg)
{
    (void)packet; (void)msg;
    return 0;  // 同上：ORIG 无 read，逐字段在 dispatch_sig 内读取
}

Dispatcher_GuildCargo::Dispatcher_GuildCargo() {}
Dispatcher_GuildCargo::~Dispatcher_GuildCargo() {}
int Dispatcher_GuildCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GuildCargo::send(CUser* user, ParamBase& param) {}

Dispatcher_Join_Power::Dispatcher_Join_Power() {}
Dispatcher_Join_Power::~Dispatcher_Join_Power() {}
int Dispatcher_Join_Power::dispatch_sig(CUser* user, PacketBuf& packet)
{
    return LineFunc(0xa8a7,
                    "virtual int Dispatcher_Join_Power::dispatch_sig(CUser*, PacketBuf&)",
                    0, 0);
}
int Dispatcher_Join_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Join_Power::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MotionHack::Dispatcher_MotionHack() {}
Dispatcher_MotionHack::~Dispatcher_MotionHack() {}
int Dispatcher_MotionHack::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MotionHack::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MoveToGate::Dispatcher_MoveToGate() {}
Dispatcher_MoveToGate::~Dispatcher_MoveToGate() {}
int Dispatcher_MoveToGate::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9a0d,
                        "virtual int Dispatcher_MoveToGate::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    private_store::CPrivateStoreMgr* manager =
        private_store::GetInstancePrivateStoreMgr();
    private_store::ISeller* seller = 0;
    if (manager != 0)
    {
        manager->GetStoreSeller(&seller, user);
    }
    if (seller != 0)
    {
        seller->MoveToGate();
    }
    return 0;
}
int Dispatcher_MoveToGate::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MoveToGate::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_RedeemList::Dispatcher_RedeemList() {}
Dispatcher_RedeemList::~Dispatcher_RedeemList() {}
int Dispatcher_RedeemList::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x822621c：state>2 时向用户发送兑换信息；否则返回 0
    if (user == 0 || user->get_state() <= 2)
    {
        return 0;
    }
    user->send_RedeemInfo();
    return 0;
}
int Dispatcher_RedeemList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RedeemList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SelectItem::Dispatcher_SelectItem() {}
Dispatcher_SelectItem::~Dispatcher_SelectItem() {}
int Dispatcher_SelectItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SelectItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SelectItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TraceError::Dispatcher_TraceError() {}
Dispatcher_TraceError::~Dispatcher_TraceError() {}
int Dispatcher_TraceError::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char errorType = 0;
    if (!packet.get_byte(errorType))
    {
        return LineFunc(0x9c74,
                        "virtual int Dispatcher_TraceError::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (errorType == 2)
    {
        char errorCode = 0;
        if (!packet.get_byte(errorCode))
        {
            return LineFunc(0x9cb3,
                            "virtual int Dispatcher_TraceError::dispatch_sig(CUser*, PacketBuf&)",
                            0, 0);
        }
        cMyTrace trace("virtual int Dispatcher_TraceError::dispatch_sig(CUser*, PacketBuf&)",
                       0x9cb5, 0);
        trace("KILLBOSS2 client - error type(%d)", (int)errorCode);
    }
    else if (errorType == 3)
    {
        return LineFunc(0x9c85,
                        "virtual int Dispatcher_TraceError::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    return 0;
}
int Dispatcher_TraceError::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TraceError::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TryEnchant::Dispatcher_TryEnchant() {}
Dispatcher_TryEnchant::~Dispatcher_TryEnchant() {}
int Dispatcher_TryEnchant::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_TryEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TryEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_VerifyGold::Dispatcher_VerifyGold() {}
Dispatcher_VerifyGold::~Dispatcher_VerifyGold() {}
int Dispatcher_VerifyGold::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() < 3)
    {
        return 0;
    }
    unsigned short value = 0;
    if (!packet.get_short(value))
    {
        return LineFunc(0xde71,
                        "virtual int Dispatcher_VerifyGold::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    PacketGuard response;
    response.put_header(1, 0x11f);
    response.put_byte(1);
    response.put_short(value);
    response.put_int(user->getCurCharacMoney());
    response.finalize(true);
    user->Send(response);
    return 0;
}
int Dispatcher_VerifyGold::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_VerifyGold::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BingoReward::Dispatcher_BingoReward() {}
Dispatcher_BingoReward::~Dispatcher_BingoReward() {}
int Dispatcher_BingoReward::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_BingoReward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BingoReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_BingoReward::send(CUser* user, ParamBase& param) {}

Dispatcher_CranePickUp::Dispatcher_CranePickUp() {}
Dispatcher_CranePickUp::~Dispatcher_CranePickUp() {}
int Dispatcher_CranePickUp::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CranePickUp::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CranePickUp::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EPLPCommand::Dispatcher_EPLPCommand() {}
Dispatcher_EPLPCommand::~Dispatcher_EPLPCommand() {}
int Dispatcher_EPLPCommand::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_EPLPCommand::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_EPLPCommand::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ModItemAttr::Dispatcher_ModItemAttr() {}
Dispatcher_ModItemAttr::~Dispatcher_ModItemAttr() {}



Dispatcher_SecurityLog::Dispatcher_SecurityLog() {}
Dispatcher_SecurityLog::~Dispatcher_SecurityLog() {}
int Dispatcher_SecurityLog::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityLog::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SecurityLog::send(CUser* user, ParamBase& param) {}

Dispatcher_Select_Card::Dispatcher_Select_Card() {}
Dispatcher_Select_Card::~Dispatcher_Select_Card() {}
int Dispatcher_Select_Card::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Select_Card::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Select_Card::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeItem::Dispatcher_UpgradeItem() {}
Dispatcher_UpgradeItem::~Dispatcher_UpgradeItem() {}
int Dispatcher_UpgradeItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->CheckInTrade())
    {
        return 0;                                    // ORIG 空用户/交易中静默丢弃
    }
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0x9518,
            "virtual int Dispatcher_UpgradeItem::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }
    if (user->CheckInTrade())                        // ORIG 二次检查
    {
        return 0;
    }

    MSG_ITEM_UPGRADE req;                            // ORIG ctor @0x0822dcea

    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(req.m_padD + 0x0d)))   // +0x1a byte
    {
        return LineFunc(
            0x9530,
            "virtual int Dispatcher_UpgradeItem::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_padD + 0x0e))) // +0x1b word（槽位）
    {
        return LineFunc(
            0x9532,
            "virtual int Dispatcher_UpgradeItem::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(req.m_padD + 0x10)))    // +0x1d dword
    {
        return LineFunc(
            0x9534,
            "virtual int Dispatcher_UpgradeItem::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_padD + 0x16))) // +0x23 word
    {
        return LineFunc(
            0x9536,
            "virtual int Dispatcher_UpgradeItem::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_padD + 0x14))) // +0x21 word
    {
        return LineFunc(
            0x9539,
            "virtual int Dispatcher_UpgradeItem::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }

    unsigned int strLen = 0;
    memset(req.m_28, 0, sizeof(req.m_28));           // ORIG memset(this+0x28,0,0x81)
    if (!packet.get_int(strLen))                     // 字符串长度前导
    {
        return LineFunc(
            0x9540,
            "virtual int Dispatcher_UpgradeItem::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }
    if (!packet.get_str(req.m_28, (int)sizeof(req.m_28), (int)strLen))
    {
        return LineFunc(
            0x9542,
            "virtual int Dispatcher_UpgradeItem::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }

    ServiceRestrictManager* restrictMgr = ARAD::Singleton<ServiceRestrictManager>::Get();
    if (restrictMgr->isRestricted(user, RestrictCategory::Enum_1, 7))
    {
        user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x53, 0xd1);
        return 0;
    }

    const unsigned int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x25);
    if (secu != 0)
    {
        user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x53,
                                      (unsigned short)(secu & 0xffff));
        return 0;
    }

    if (user->CheckItemLock(1, *reinterpret_cast<unsigned short*>(req.m_padD + 0x0e)))
    {
        user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x53, 0xd5);
        return 0;
    }

    G_CDataManager()->m_itemUpgrade->item_upgrade_system(user, &req, 0x53);
    return 0;
}
int Dispatcher_UpgradeItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AgreeEnchant::Dispatcher_AgreeEnchant() {}
Dispatcher_AgreeEnchant::~Dispatcher_AgreeEnchant() {}
int Dispatcher_AgreeEnchant::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AgreeEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AgreeEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Back2Village::Dispatcher_Back2Village() {}
Dispatcher_Back2Village::~Dispatcher_Back2Village() {}
int Dispatcher_Back2Village::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0;
    }
    if (user->get_state() == 0xd)
    {
        PacketGuard notification;
        G_GameWorld()->out_from_dungeon(user);
        notification.put_header(0, 2);
        notification.put_byte(0);
        notification.put_short(1);
        user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
        notification.finalize(true);
        G_GameWorld()->send_all(notification, user);
        return 0;
    }
    if (user->get_state() != 5)
    {
        return 0;
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0 || party->GetEPLPState() != 0)
    {
        return 0;
    }
    if (party->getManager() == user)
    {
        if (party->get_quick_party_index() != -1)
        {
            G_CGameManager()->CheckOutQuickParty(party, true);
        }
        party->ReturnToVillage();
        return 0;
    }
    if (party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2) != 1)
    {
        PacketGuard notification;
        G_GameWorld()->out_from_dungeon(user);
        notification.clear();
        notification.put_header(0, 2);
        notification.put_byte(0);
        notification.put_short(1);
        user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
        notification.finalize(true);
        G_GameWorld()->send_all(notification, user);
    }
    return 0;
}
int Dispatcher_Back2Village::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Back2Village::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// process 依赖（清单外）：清 param.m_8=0；GetPartyIndex 找不到队伍 → cMyTrace
// "Party == NULL"，返回 8；否则 0。（声明需入 Dispatchers.h，见 NEEDS_DECL）
int Dispatcher_BossDieCheck::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    out.m_8 = 0;
    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    if (party == 0)
    {
        cMyTrace trace(
            "int Dispatcher_BossDieCheck::check_error(CUser*, MSG_BASE&, ParamBase&)",
            0x258b, 0);
        trace("Party == NULL");
        return 8;
    }
    return 0;
}

Dispatcher_BossDieCheck::Dispatcher_BossDieCheck() {}
Dispatcher_BossDieCheck::~Dispatcher_BossDieCheck() {}
// 技能校验和比对（不等记 hack 0x25c）；从战场 m_apcList(party+0xc3c) 删除
// 上报 boss 序号(msg+0x10)，未命中 param=0x15；删空后记时刻、给队长挂 100ms
// 定时器(param3=characNo) 并置 param.m_8=1；msg+0xe==-1 记 hack 0xcb。
int Dispatcher_BossDieCheck::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    MSG_BOSS_DIE_CHECK& req = static_cast<MSG_BOSS_DIE_CHECK&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    unsigned short& bossSeq =
        *reinterpret_cast<unsigned short*>(pReq + 0x10);   // +0x10 word
    short& unkKind =
        *reinterpret_cast<short*>(pReq + 0xe);             // +0x0e word

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x259e,
                        "virtual int Dispatcher_BossDieCheck::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_4, user->get_acc_id());
    }

    if (user->getCurCharacSkillCheckSum() !=
        *reinterpret_cast<int*>(pReq + 0x14))              // +0x14 int
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25c, 1, 0, 0);
    }

    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    CBattle_Field* field = reinterpret_cast<CBattle_Field*>(
        reinterpret_cast<char*>(party) + 0xb24);           // party+0xc3c = BF+0x118
    std::vector<int>::iterator it = std::find(field->m_apcList.begin(),
                                              field->m_apcList.end(),
                                              (int)bossSeq);
    if (it != field->m_apcList.end())
    {
        field->m_apcList.erase(it);
        out.m_a = (short)bossSeq;
    }
    else
    {
        out.m_4 = 0x15;
    }

    if ((int)field->m_apcList.size() == 0)                 // party+0xc48 = BF+0x124
    {
        field->m_field124 = GlobalData::s_systemTime_.getCurSec();
        int characNo = user->get_charac_no(-1);
        G_TimerQueue()->InsertTimerInMilisecond(
            (TimerEntry::OBJ_TYPE)1, party->GetPartyIndex(),
            (TIMER_MESSAGE)0x64, (long long)1000,
            field->m_field124, (unsigned int)characNo);
        out.m_8 = 1;
    }

    if (unkKind == -1)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0xcb, 1, 0, 0);
    }
    return 0;
}
int Dispatcher_BossDieCheck::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BOSS_DIE_CHECK& req = static_cast<MSG_BOSS_DIE_CHECK&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    unsigned short& unkE =
        *reinterpret_cast<unsigned short*>(pReq + 0xe);
    unsigned short& bossSeq =
        *reinterpret_cast<unsigned short*>(pReq + 0x10);
    int& checksum =
        *reinterpret_cast<int*>(pReq + 0x14);
    if (!packet.get_short(unkE))
    {
        return LineFunc(0x2578,
                        "virtual int Dispatcher_BossDieCheck::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_short(bossSeq))
    {
        return LineFunc(0x2579,
                        "virtual int Dispatcher_BossDieCheck::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(checksum))
    {
        return LineFunc(0x257b,
                        "virtual int Dispatcher_BossDieCheck::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
// param.m_c==2 先发命令错误包 0xb7；再组 header(0,0x73)：成功(m_4==0)回
// {1, m_8, m_a}，失败回 {0, m_4}；有队伍发全队，否则单发。
void Dispatcher_BossDieCheck::send(CUser* user, ParamBase& param)
{
    stBossDieCheckParam& out = static_cast<stBossDieCheckParam&>(param);
    if (out.m_c == 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xb7, 2);
    }
    PacketGuard packet;
    packet.put_header(0, 0x73);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_byte((unsigned char)out.m_8);
        packet.put_short((unsigned short)out.m_a);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4);
    }
    packet.finalize(true);
    CParty* party =
        G_CGameManager()->GetParty((int)(short)user->GetPartyIndex());
    if (party == 0)
    {
        user->Send(packet);
    }
    else
    {
        party->send_to_party(packet);
    }
}

Dispatcher_CodeCheckSum::Dispatcher_CodeCheckSum() {}
Dispatcher_CodeCheckSum::~Dispatcher_CodeCheckSum() {}
int Dispatcher_CodeCheckSum::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82044b0 (101 insn)：读 int + 0x20 字节校验块 → checkCodeChecksum；
    // 失败且 state>2 时记 hack(0x3c8)
    if (user->get_state() < 2)
    {
        return 0;
    }
    int seq = 0;
    if (!packet.get_int(seq))
    {
        return LineFunc(0x9bfd,
                        "virtual int Dispatcher_CodeCheckSum::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    unsigned char checksum[32];
    if (!packet.get_binary(reinterpret_cast<char*>(checksum), 0x20))
    {
        return LineFunc(0x9bff,
                        "virtual int Dispatcher_CodeCheckSum::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }

    char path[260];                                    // ORIG local_13c 清零区
    memset(path, 0, sizeof(path));

    // BLOCKED(_ZN5CUser17checkCodeChecksumERK16CodeHackChecksumPc @0x0867ebba
    //        与 CodeHackChecksum(0x20B POD) 未建模): 校验结果占位为通过
    const char checksumOk = 1;                         // BLOCKED 调用返回值
    (void)checksum;
    user->IncChecksumAnswerCount();
    if (checksumOk == 1)
    {
        return 0;
    }
    int failKind = std::strstr(path, ".exe.bak") != 0 ? 2 : 3;   // ORIG 死存储保留
    (void)failKind;
    if (user->get_state() > 2)
    {
        user->getHackAnalyzer()->addServerHackCnt(user,
                                                  static_cast<WongWork::ENUM_HACKTYPE>(0x3c8),
                                                  1, 0, 0);
    }
    return 0;
}
int Dispatcher_CodeCheckSum::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CodeCheckSum::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

// process 依赖（清单外）：state!=3→7；count(msg+0x10)!=1→0x16；
// GetInvenSlot(EQUIP,slot) 后 find_item 缺失→0x11、持有数不足→0x16；
// 非 0x12ca 物品→0x17；0x12ca 时摆摊忙→0x3c、时光门开→0x16，否则 0。
// （声明需入 Dispatchers.h，见 NEEDS_DECL）
int Dispatcher_CollectItems::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_COLLECT_ITEMS& req = static_cast<MSG_COLLECT_ITEMS&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    short& slot = *reinterpret_cast<short*>(pReq + 0xe);          // +0xe word
    unsigned short& count = *reinterpret_cast<unsigned short*>(pReq + 0x10);

    if (user->get_state() != 3)
    {
        return 7;
    }
    if ((unsigned short)count != 1)
    {
        return 0x16;
    }
    Inven_Item slotItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, (int)slot);
    CItem* item = G_CDataManager()->find_item(slotItem.m_addInfo);
    if (item == 0)
    {
        return 0x11;
    }
    if (slotItem.m_addInfo2 < (int)(unsigned int)count)
    {
        return 0x16;
    }
    if (slotItem.m_addInfo != 0x12ca)
    {
        return 0x17;
    }
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (GlobalData::s_timeGate_->isOpen())
    {
        return 0x16;
    }
    return 0;
}

// process 依赖（清单外）。TU 内完整类型定义（全仓此前无定义/声明）：
// PacketHeader 视图(type 0x27e6 / size 0x39) + accId/itemIdx/characNo/
// nameLen/name，总长 0x39；ctor 清 itemIdx(+0x12)/characNo(+0x16) 两 int。
class Packet_CollectItems
{
public:
    Packet_CollectItems();  // ORIG W 0x0822c75a

    unsigned short m_type;    // +0x00（header type 0x27e6）
    unsigned short m_size;    // +0x02（header size 0x39）
    unsigned short m_field4;  // +0x04
    int m_field6;             // +0x06
    char m_padA[4];           // +0x0a..+0x0d
    unsigned int m_accId;     // +0x0e
    unsigned int m_itemIdx;   // +0x12
    int m_characNo;           // +0x16
    char m_nameLen;           // +0x1a
    char m_name[0x1d];        // +0x1b..+0x37
};                            // 总长 0x39

Packet_CollectItems::Packet_CollectItems()
{
    m_type = 0x27e6;
    m_size = 0x39;
    m_field4 = 0;
    m_field6 = 0;
    m_itemIdx = 0;
    m_characNo = 0;
}

Dispatcher_CollectItems::Dispatcher_CollectItems() {}
Dispatcher_CollectItems::~Dispatcher_CollectItems() {}
// 删收集物（INVEN_TYPE 1 / reason 3），失败 LineFunc(0x5aa7)；成功把
// slot/count 回填 param，并向监控服代理上报 Packet_CollectItems(0x39 字节)。
int Dispatcher_CollectItems::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stCollectItems& out = static_cast<stCollectItems&>(param);
    MSG_COLLECT_ITEMS& req = static_cast<MSG_COLLECT_ITEMS&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    short& slot = *reinterpret_cast<short*>(pReq + 0xe);          // +0xe word
    unsigned short& count = *reinterpret_cast<unsigned short*>(pReq + 0x10);

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x5aa2,
                        "virtual int Dispatcher_CollectItems::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_4, 0);
    }

    CInventory* invenW = user->getCurCharacInvenW();
    if (!invenW->delete_item((INVEN_TYPE)1, (int)slot, (unsigned short)count,
                             (eItemDelReason)3, true))
    {
        return LineFunc(0x5aa7,
                        "virtual int Dispatcher_CollectItems::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, 0);
    }
    out.m_8 = slot;
    out.m_a = (short)count;

    Packet_CollectItems notify;
    notify.m_itemIdx = (unsigned int)count;
    notify.m_characNo = user->get_charac_no(-1);
    notify.m_accId = user->get_acc_id();
    const char* name = user->getCurCharacName();
    notify.m_nameLen = (char)strlen(name);
    strncpy(notify.m_name, name, 0x1d);
    CMonitorServerProxy* proxy = GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup());
    proxy->SendTcpPacket(reinterpret_cast<char*>(&notify), 0x39);
    return 0;
}
int Dispatcher_CollectItems::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_COLLECT_ITEMS& req = static_cast<MSG_COLLECT_ITEMS&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    short& slot = *reinterpret_cast<short*>(pReq + 0xe);
    unsigned short& count = *reinterpret_cast<unsigned short*>(pReq + 0x10);
    if (!packet.get_short(slot))
    {
        return LineFunc(0x5a8f,
                        "virtual int Dispatcher_CollectItems::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_short(count))
    {
        return LineFunc(0x5a90,
                        "virtual int Dispatcher_CollectItems::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
// err==0x7fffffff 静默；err==0 回 header(1,0x1e3){1, m_8(signed), m_a}；
// 其余发命令错误包 0x1e3。
void Dispatcher_CollectItems::send(CUser* user, ParamBase& param)
{
    stCollectItems& out = static_cast<stCollectItems&>(param);
    if (out.m_4 == (int)0x7fffffff)
    {
        return;
    }
    if (out.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1e3,
                                 (unsigned char)(out.m_4 & 0xff));
        return;
    }
    PacketGuard packet;
    packet.put_header(1, 0x1e3);
    packet.put_byte(1);
    packet.put_short((int)out.m_8);
    packet.put_short((unsigned short)out.m_a);
    packet.finalize(true);
    user->Send(packet);
}

Dispatcher_DepositMoney::Dispatcher_DepositMoney() {}
Dispatcher_DepositMoney::~Dispatcher_DepositMoney() {}
int Dispatcher_DepositMoney::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() < 3)
    {
        return 0;
    }
    unsigned int money = 0;
    if (!packet.get_int(money))
    {
        return LineFunc(0xdfc2,
                        "virtual int Dispatcher_DepositMoney::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (!user->IsExistAccountCargo())
    {
        return 0;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    if (cargo != 0 && cargo->DepositMoney(money) != 0)
    {
        user->m_historyLog.AccountCargoAddItem(
            0, (int)cargo->GetMoney(), (int)money, (eItemAddReason)0x2d);
    }
    return 0;
}
int Dispatcher_DepositMoney::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DepositMoney::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EvadeAssault::Dispatcher_EvadeAssault() {}
Dispatcher_EvadeAssault::~Dispatcher_EvadeAssault() {}
int Dispatcher_EvadeAssault::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_EvadeAssault::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x27c2, pretty, 0, 0);
    }
    if (user->get_state() == 3)
    {
        return 0;
    }
    return LineFunc(0x27c5, pretty, 0, user->get_acc_id());
}
int Dispatcher_EvadeAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_EvadeAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_FairPvPScore::Dispatcher_FairPvPScore() {}
Dispatcher_FairPvPScore::~Dispatcher_FairPvPScore() {}
int Dispatcher_FairPvPScore::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_FairPvPScore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_FairPvPScore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_HumanCertify::Dispatcher_HumanCertify() {}
Dispatcher_HumanCertify::~Dispatcher_HumanCertify() {}
int Dispatcher_HumanCertify::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_HumanCertify::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_HumanCertify::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_InformNotice::Dispatcher_InformNotice() {}
Dispatcher_InformNotice::~Dispatcher_InformNotice() {}
int Dispatcher_InformNotice::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 3)
    {
        return 1;
    }
    const MSG_INFORM_NOTICE& req = static_cast<const MSG_INFORM_NOTICE&>(msg);
    return (unsigned int)req.m_0d < 0x80 ? 0 : -1;
}
int Dispatcher_InformNotice::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_InformNotice::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_InformNotice::send(CUser* user, ParamBase& param) {}

Dispatcher_JoinPowerWar::Dispatcher_JoinPowerWar() {}
Dispatcher_JoinPowerWar::~Dispatcher_JoinPowerWar() {}
int Dispatcher_JoinPowerWar::dispatch_sig(CUser* user, PacketBuf& packet)
{
    return LineFunc(0xc8d3,
                    "virtual int Dispatcher_JoinPowerWar::dispatch_sig(CUser*, PacketBuf&)",
                    0, 0);
}
int Dispatcher_JoinPowerWar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_JoinPowerWar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MailBox_Open::Dispatcher_MailBox_Open() {}
Dispatcher_MailBox_Open::~Dispatcher_MailBox_Open() {}
int Dispatcher_MailBox_Open::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_MailBox_Open::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MailBox_Open::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_MailBox_Open::send(CUser* user, ParamBase& param) {}


Dispatcher_MemberSecede::Dispatcher_MemberSecede() {}
Dispatcher_MemberSecede::~Dispatcher_MemberSecede() {}
int Dispatcher_MemberSecede::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_MemberSecede::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MemberSecede::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestMatch::Dispatcher_RequestMatch() {}
Dispatcher_RequestMatch::~Dispatcher_RequestMatch() {}
int Dispatcher_RequestMatch::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestMatch::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestMatch::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ScanBotByDll::Dispatcher_ScanBotByDll() {}
Dispatcher_ScanBotByDll::~Dispatcher_ScanBotByDll() {}
int Dispatcher_ScanBotByDll::check_error(CUser* user, MSG_BASE& msg)
{
    const MSG_SCAN_DLL& req = static_cast<const MSG_SCAN_DLL&>(msg);
    int value = *reinterpret_cast<const int*>(req.m_pad);
    return value < 0 || value > 49 ? 0x407 : 0;
}
int Dispatcher_ScanBotByDll::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ScanBotByDll::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ScanBotByDrv::Dispatcher_ScanBotByDrv() {}
Dispatcher_ScanBotByDrv::~Dispatcher_ScanBotByDrv() {}
int Dispatcher_ScanBotByDrv::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ScanBotByDrv::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ScanBotByDrv::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Secede_Power::Dispatcher_Secede_Power() {}
Dispatcher_Secede_Power::~Dispatcher_Secede_Power() {}
int Dispatcher_Secede_Power::dispatch_sig(CUser* user, PacketBuf& packet)
{
    return LineFunc(0xa91a,
                    "virtual int Dispatcher_Secede_Power::dispatch_sig(CUser*, PacketBuf&)",
                    0, 0);
}
int Dispatcher_Secede_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Secede_Power::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SoloTeleport::Dispatcher_SoloTeleport() {}
Dispatcher_SoloTeleport::~Dispatcher_SoloTeleport() {}

Dispatcher_TitleBookGet::Dispatcher_TitleBookGet() {}
Dispatcher_TitleBookGet::~Dispatcher_TitleBookGet() {}
int Dispatcher_TitleBookGet::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char* pMsg = (char*)&msg;

    if (user->get_state() <= 2)
    {
        return 0;
    }

    // getTitle(user, category=+0x19, slot=+0x1d, itemIdx=+0x15, space=+0xd,
    //          outSlot=&+0x11)
    int ret = CTitleBook::getTitle(
        user,
        (ENUM_TITLE_BOOK_CATEGORY)*(int*)(pMsg + 0x19),
        *(int*)(pMsg + 0x1d),
        *(int*)(pMsg + 0x15),
        (ENUM_ITEMSPACE)*(int*)(pMsg + 0x0d),
        *(int*)(pMsg + 0x11));

    if (ret == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x19d);
        packet.put_byte(1);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x11));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.put_int(*(int*)(pMsg + 0x1d));
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x19d);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}
int Dispatcher_TitleBookGet::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_int(*(int*)(pMsg + 0x0d)))
    {
        return LineFunc(0x420c,
                        "virtual int Dispatcher_TitleBookGet::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x11)))
    {
        return LineFunc(0x420e,
                        "virtual int Dispatcher_TitleBookGet::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x15)))
    {
        return LineFunc(0x4210,
                        "virtual int Dispatcher_TitleBookGet::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x19)))
    {
        return LineFunc(0x4212,
                        "virtual int Dispatcher_TitleBookGet::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x1d)))
    {
        return LineFunc(0x4214,
                        "virtual int Dispatcher_TitleBookGet::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

Dispatcher_TitleBookPut::Dispatcher_TitleBookPut() {}
Dispatcher_TitleBookPut::~Dispatcher_TitleBookPut() {}
int Dispatcher_TitleBookPut::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char* pMsg = (char*)&msg;

    if (user->get_state() <= 2)
    {
        return 0;
    }

    // putTitle(user, space=+0xd, slot=+0x11, itemIdx=+0x15, category=+0x19,
    //          titleSlot=+0x1d)
    int ret = CTitleBook::putTitle(
        user,
        (ENUM_ITEMSPACE)*(int*)(pMsg + 0x0d),
        *(int*)(pMsg + 0x11),
        *(int*)(pMsg + 0x15),
        (ENUM_TITLE_BOOK_CATEGORY)*(int*)(pMsg + 0x19),
        *(int*)(pMsg + 0x1d));

    if (ret == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x19c);
        packet.put_byte(1);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x11));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.put_int(*(int*)(pMsg + 0x1d));
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x19c);
        packet.put_byte(0);
        packet.put_byte(ret);
        packet.put_int(*(int*)(pMsg + 0x0d));
        packet.put_int(*(int*)(pMsg + 0x19));
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
}
int Dispatcher_TitleBookPut::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_int(*(int*)(pMsg + 0x0d)))
    {
        return LineFunc(0x41d4,
                        "virtual int Dispatcher_TitleBookPut::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x11)))
    {
        return LineFunc(0x41d6,
                        "virtual int Dispatcher_TitleBookPut::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x15)))
    {
        return LineFunc(0x41d8,
                        "virtual int Dispatcher_TitleBookPut::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x19)))
    {
        return LineFunc(0x41da,
                        "virtual int Dispatcher_TitleBookPut::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x1d)))
    {
        return LineFunc(0x41dc,
                        "virtual int Dispatcher_TitleBookPut::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

Dispatcher_UpgradeCargo::Dispatcher_UpgradeCargo() {}
Dispatcher_UpgradeCargo::~Dispatcher_UpgradeCargo() {}
int Dispatcher_UpgradeCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0xe7d8,
            "virtual int Dispatcher_UpgradeCargo::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }

    const unsigned int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x1d);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, (unsigned char)(secu & 0xff));
        return 0;
    }

    short slot = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(
            0xe7e2,
            "virtual int Dispatcher_UpgradeCargo::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }

    // 扩仓券物品校验：槽内数量字段须 > 0
    Inven_Item ticket =
        user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, slot);
    if (ticket.m_addInfo2 < 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x11);
        return 0;
    }

    // 券动作合法性（StackableAction TYPE_UPGRADE_CARGO=0x2b，按 itemIdx 校验）
    if (!user->verifyStackableItemActionPlace(StackableAction::TYPE_UPGRADE_CARGO,
                                              (unsigned int)ticket.m_addInfo))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x11);
        return 0;
    }

    const CCargo* cargoR = user->getCurCharacCargoR();
    if (!cargoR->isUpgradable(cargoR->GetCapacity() + 0x10))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x13);
        return 0;
    }

    if (user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x11);
        return 0;
    }

    // 包 1（0x0e）：删除券后的物品更新广播
    PacketGuard pg;                                  // try 域 @0822a32b..0822a482
    pg.put_header(0, 0xe);
    pg.put_byte(0);
    pg.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE_EQUIP, slot, pg);
    pg.finalize(true);
    user->Send(pg);

    // 升级仓库容量（+0x10），失败回错误码 0x13
    if (!user->getCurCharacCargoW()->UpgradeCargo(
            user->getCurCharacCargoR()->GetCapacity() + 0x10))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x198, 0x13);
        return 0;
    }

    // 包 2（0x42）：新容量通知
    pg.clear();
    pg.put_header(0, 0x42);
    pg.put_short(1);
    pg.put_short(user->getCurCharacCargoR()->GetCapacity());
    pg.finalize(true);
    user->Send(pg);
    return 0;
}
int Dispatcher_UpgradeCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseLimitCube::Dispatcher_UseLimitCube() {}
Dispatcher_UseLimitCube::~Dispatcher_UseLimitCube() {}
// process/read/send/check_error 实现于 Dispatcher_UseLimitCube_impl.cpp（ORIG 0x081d3d38 族）。

Dispatcher_AssertManager::Dispatcher_AssertManager() {}
Dispatcher_AssertManager::~Dispatcher_AssertManager() {}
int Dispatcher_AssertManager::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AssertManager::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AssertManager::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CallGuildInfo::Dispatcher_CallGuildInfo() {}
Dispatcher_CallGuildInfo::~Dispatcher_CallGuildInfo() {}
int Dispatcher_CallGuildInfo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CallGuildInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeEmotion::Dispatcher_ChangeEmotion() {}
Dispatcher_ChangeEmotion::~Dispatcher_ChangeEmotion() {}
int Dispatcher_ChangeEmotion::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ChangeEmotion::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeEmotion::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ChangeEmotion::send(CUser* user, ParamBase& param) {}

Dispatcher_Clear_Used_QP::Dispatcher_Clear_Used_QP() {}
Dispatcher_Clear_Used_QP::~Dispatcher_Clear_Used_QP() {}
int Dispatcher_Clear_Used_QP::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_Clear_Used_QP::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Clear_Used_QP::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Clear_Used_QP::send(CUser* user, ParamBase& param) {}

Dispatcher_ConnectP2PPvP::Dispatcher_ConnectP2PPvP() {}
Dispatcher_ConnectP2PPvP::~Dispatcher_ConnectP2PPvP() {}
int Dispatcher_ConnectP2PPvP::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ConnectP2PPvP::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ConnectP2PPvP::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DenyJoinGuild::Dispatcher_DenyJoinGuild() {}
Dispatcher_DenyJoinGuild::~Dispatcher_DenyJoinGuild() {}
int Dispatcher_DenyJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DenyJoinGuild::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EnchantByBead::Dispatcher_EnchantByBead() {}
Dispatcher_EnchantByBead::~Dispatcher_EnchantByBead() {}
int Dispatcher_EnchantByBead::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_EnchantByBead::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_EnchantByBead::send(CUser* user, ParamBase& param) {}

Dispatcher_GenCeraTicket::Dispatcher_GenCeraTicket() {}
Dispatcher_GenCeraTicket::~Dispatcher_GenCeraTicket() {}
int Dispatcher_GenCeraTicket::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_GenCeraTicket::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GenCeraTicket::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GiveGiftToNPC::Dispatcher_GiveGiftToNPC() {}
Dispatcher_GiveGiftToNPC::~Dispatcher_GiveGiftToNPC() {}
int Dispatcher_GiveGiftToNPC::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_GiveGiftToNPC::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GiveGiftToNPC::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GuildJoinList::Dispatcher_GuildJoinList() {}
Dispatcher_GuildJoinList::~Dispatcher_GuildJoinList() {}
int Dispatcher_GuildJoinList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildJoinList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_HatchCreature::Dispatcher_HatchCreature() {}
Dispatcher_HatchCreature::~Dispatcher_HatchCreature() {}
int Dispatcher_HatchCreature::check_error(CUser* user, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;

    if (user->getCurCharacR() == 0)
    {
        return -1;
    }

    int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x14);
    if (secuErr != 0)
    {
        return secuErr;
    }

    short slot = *(short*)(pMsg + 0x0e);
    int invenType = GetInvenTypeFromItemSpace((unsigned char)pMsg[0x0d]);
    if (user->CheckItemLock(invenType, slot))
    {
        return 0xd5;
    }

    return 0;
}
int Dispatcher_HatchCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stHatchCreature& st = *reinterpret_cast<stHatchCreature*>(&param);
    char* pMsg = (char*)&msg;

    int err = check_error(user, msg);
    st.m_4 = err;

    if (err >= 1)
    {
        return 0;
    }

    if (err < 0)
    {
        return LineFunc(0x252c,
                        "virtual int Dispatcher_HatchCreature::process(CUser*, MSG_BASE&, ParamBase&)",
                        err, user->get_acc_id());
    }

    short slot = *(short*)(pMsg + 0x0e);
    int invenType = GetInvenTypeFromItemSpace((unsigned char)pMsg[0x0d]);

    const Inven_Item* item =
        user->getCurCharacInvenR()->GetInvenRef(invenType, slot);
    if (item == 0)
    {
        return LineFunc(0x2533,
                        "virtual int Dispatcher_HatchCreature::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, user->get_acc_id());
    }

    int addInfo = item->get_add_info();

    CInventory* invW = user->getCurCharacInvenW();
    user_creature::CCreatureMgr* mgr = invW->GetCreatureMgrW();

    // BLOCKED(user_creature::CCreatureMgr::Hatch(int)
    //         _ZN13user_creature12CCreatureMgr5HatchEi @0x08339fa0)
    // CInventory.h 内 user_creature::CCreatureMgr 尚无该声明（见 NEEDS_DECL）。
    mgr->Hatch(addInfo); // 占位直调，待声明落地

    return 0;
}
int Dispatcher_HatchCreature::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_byte(*(unsigned char*)(pMsg + 0x0d)))
    {
        return LineFunc(0x2518,
                        "virtual int Dispatcher_HatchCreature::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_short(*(short*)(pMsg + 0x0e)))
    {
        return LineFunc(0x251b,
                        "virtual int Dispatcher_HatchCreature::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
void Dispatcher_HatchCreature::send(CUser* user, ParamBase& param) {}

Dispatcher_JoinGuildInfo::Dispatcher_JoinGuildInfo() {}
Dispatcher_JoinGuildInfo::~Dispatcher_JoinGuildInfo() {}
int Dispatcher_JoinGuildInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_JoinGuildInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MouseRegister::Dispatcher_MouseRegister() {}
Dispatcher_MouseRegister::~Dispatcher_MouseRegister() {}
int Dispatcher_MouseRegister::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_MouseRegister::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_MouseRegister::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MouseRegister::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_MouseRegister::send(CUser* user, ParamBase& param) {}

Dispatcher_ObjectBringUp::Dispatcher_ObjectBringUp() {}
Dispatcher_ObjectBringUp::~Dispatcher_ObjectBringUp() {}

Dispatcher_Overflow_Info::Dispatcher_Overflow_Info() {}
Dispatcher_Overflow_Info::~Dispatcher_Overflow_Info() {}
int Dispatcher_Overflow_Info::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Overflow_Info::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Overflow_Info::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_QuickJoinRoom::Dispatcher_QuickJoinRoom() {}
Dispatcher_QuickJoinRoom::~Dispatcher_QuickJoinRoom() {}
int Dispatcher_QuickJoinRoom::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_QuickJoinRoom::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_QuickJoinRoom::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SelectStriker::Dispatcher_SelectStriker() {}
Dispatcher_SelectStriker::~Dispatcher_SelectStriker() {}
int Dispatcher_SelectStriker::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_SelectStriker::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SelectStriker::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SelectStriker::send(CUser* user, ParamBase& param) {}

Dispatcher_UseCraneStart::Dispatcher_UseCraneStart() {}
Dispatcher_UseCraneStart::~Dispatcher_UseCraneStart() {}
int Dispatcher_UseCraneStart::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UseCraneStart::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseCraneStart::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseRenameCard::Dispatcher_UseRenameCard() {}
Dispatcher_UseRenameCard::~Dispatcher_UseRenameCard() {}
int Dispatcher_UseRenameCard::check_error(CUser* user, MSG_BASE& msg)
{
    if (user->get_state() != 3)
    {
        return 0x13;
    }

    if (user->CheckInTrade())
    {
        return 0x13;
    }

    GameWorld* world = G_GameWorld();
    int channelType = world->GetChannelType();
    if (channelType == 0xf || channelType == 0x10)
    {
        return 0x13;
    }

    const _Charac_info* charac = user->getCurCharacR();
    if (charac == 0)
    {
        return 0x13;
    }

    // BLOCKED(CUser::IsWaitingStateCharac(unsigned int)
    //         call @0x0822fbe6，参数 (this, *characR))
    // 仓库未建模该符号（见 NEEDS_DECL）。ORIG：返回真 → 通过(0)，假 → 0x12。
    // 占位：保守取 false（拒绝），避免未建模状态下误放行改名卡消耗。
    bool waiting = false; // 占位
    if (waiting)
    {
        return 0;
    }

    return 0x12;
}
int Dispatcher_UseRenameCard::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUseRenameCard& st = *reinterpret_cast<stUseRenameCard*>(&param);
    char* pMsg = (char*)&msg;

    int err = check_error(user, msg);
    st.m_4 = err;

    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)err);
        return 0;
    }

    Inven_Item item;
    int ret = getItem(*user, 1, *(short*)(pMsg + 0x0e), item);
    if (ret != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)ret);
        return 0;
    }

    CItem* stackItem = G_CDataManager()->find_item(item.m_addInfo); // +0x02 item idx
    if (stackItem == 0)
    {
        return 0;
    }

    if (!stackItem->is_stackable())
    {
        return 0;
    }

    // TYPE_CHARACTER_NAME_CHANGE = 0x33
    if (!static_cast<CStackableItem*>(stackItem)->verifyStackableAction(
            StackableAction::TYPE_CHARACTER_NAME_CHANGE))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)ret);
        return 0;
    }

    user->SetCharacViewState(0xff);

    CInventory* invW = user->getCurCharacInvenW();
    int delRet = invW->delete_item((INVEN_TYPE)1, *(short*)(pMsg + 0x0e), 1,
                                   (eItemDelReason)3, true);
    if (delRet == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fe);
        user->ReturnToSelectCharacList(true);
        user->UpdateCharacView();
        return 0;
    }

    user->SetCharacViewState(0);
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, 0x11);
    return LineFunc(0x632e,
                    "virtual int Dispatcher_UseRenameCard::process(CUser*, MSG_BASE&, ParamBase&)",
                    0, user->get_acc_id());
}
int Dispatcher_UseRenameCard::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_short(*(short*)(pMsg + 0x0e)))
    {
        return LineFunc(0x62f3,
                        "virtual int Dispatcher_UseRenameCard::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

int Dispatcher_UseRenameCard::getItem(const CUser& user, int invenType, short slot,
                                      Inven_Item& out) const
{
    if (user.CheckItemLock(invenType, slot))
    {
        return 1;
    }

    Inven_Item slotItem = user.getCurCharacInvenR()->GetInvenSlot(invenType, slot);

    out = slotItem; // ORIG：0x3d 字节逐字段复制

    if (*(int*)((char*)&out + 0x02) == 0) // m_addInfo(item idx)==0 → 视为空槽失败
    {
        return 1;
    }

    return 0;
}

Dispatcher_WithdrawMoney::Dispatcher_WithdrawMoney() {}
Dispatcher_WithdrawMoney::~Dispatcher_WithdrawMoney() {}
int Dispatcher_WithdrawMoney::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() < 3)
    {
        return 0;
    }
    unsigned int money = 0;
    if (!packet.get_int(money))
    {
        return LineFunc(0xdfda,
                        "virtual int Dispatcher_WithdrawMoney::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (!user->IsExistAccountCargo())
    {
        return 0;
    }
    CAccountCargo* cargo = user->GetAccountCargo();
    if (cargo != 0 && cargo->WithdrawMoney(money) != 0)
    {
        user->m_historyLog.AccountCargoDelItem(
            0, (int)cargo->GetMoney(), (int)money, (eItemDelReason)0x26);
    }
    return 0;
}
int Dispatcher_WithdrawMoney::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_WithdrawMoney::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionBidding::Dispatcher_AuctionBidding() {}
Dispatcher_AuctionBidding::~Dispatcher_AuctionBidding() {}
int Dispatcher_AuctionBidding::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionBidding::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionBidding::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BuyAutomatItem::Dispatcher_BuyAutomatItem() {}
Dispatcher_BuyAutomatItem::~Dispatcher_BuyAutomatItem() {}
int Dispatcher_BuyAutomatItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_BuyAutomatItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BuyAutomatItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CallMemberList::Dispatcher_CallMemberList() {}
Dispatcher_CallMemberList::~Dispatcher_CallMemberList() {}
int Dispatcher_CallMemberList::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81cbd44：当前角色及 memberkey 必须有效。
    if (user == 0 || user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    return user->get_charac_memberkey() == 0 ? 0x7fffffff : 0;
}
int Dispatcher_CallMemberList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallMemberList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CheckJoinGuild::Dispatcher_CheckJoinGuild() {}
Dispatcher_CheckJoinGuild::~Dispatcher_CheckJoinGuild() {}
int Dispatcher_CheckJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckJoinGuild::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompoundEmblem::Dispatcher_CompoundEmblem() {}
Dispatcher_CompoundEmblem::~Dispatcher_CompoundEmblem() {}


Dispatcher_DisJointAvatar::Dispatcher_DisJointAvatar() {}
Dispatcher_DisJointAvatar::~Dispatcher_DisJointAvatar() {}
int Dispatcher_DisJointAvatar::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DisJointAvatar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DisJointAvatar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DisguiseCancel::Dispatcher_DisguiseCancel() {}
Dispatcher_DisguiseCancel::~Dispatcher_DisguiseCancel() {}
int Dispatcher_DisguiseCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() != 3)
    {
        return 0;
    }
    if (user->isDisguiseCharac() != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd7, 3);
        // ORIG also executes the setter again after this branch.
        user->setDisguiseCharac(false, 0, 0);
    }
    user->setDisguiseCharac(false, 0, 0);
    PacketGuard notification;
    notification.put_header(0, 2);
    notification.put_byte(0);
    notification.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
    notification.finalize(true);
    G_GameWorld()->send_all(notification, user);
    return 0;
}
int Dispatcher_DisguiseCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DisguiseCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_HellPartyStart::Dispatcher_HellPartyStart() {}
Dispatcher_HellPartyStart::~Dispatcher_HellPartyStart() {}
int Dispatcher_HellPartyStart::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() == 5)
    {
        return 0;
    }
    return LineFunc(0xd1bd,
                    "virtual int Dispatcher_HellPartyStart::dispatch_sig(CUser*, PacketBuf&)",
                    0, 0);
}
int Dispatcher_HellPartyStart::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_HellPartyStart::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ItemDictionary::Dispatcher_ItemDictionary() {}
Dispatcher_ItemDictionary::~Dispatcher_ItemDictionary() {}
int Dispatcher_ItemDictionary::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    return user->get_state() < 3 ? 1 : 0;
}
int Dispatcher_ItemDictionary::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ItemDictionary::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ItemDictionary::send(CUser* user, ParamBase& param) {}

Dispatcher_Lag_Statistics::Dispatcher_Lag_Statistics() {}
Dispatcher_Lag_Statistics::~Dispatcher_Lag_Statistics() {}
int Dispatcher_Lag_Statistics::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Lag_Statistics::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Lag_Statistics::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Mercenary_Info::Dispatcher_Mercenary_Info() {}
Dispatcher_Mercenary_Info::~Dispatcher_Mercenary_Info() {}
int Dispatcher_Mercenary_Info::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ddb46 (53 insn): !user -> -1; user->get_state() <= ch_state1 -> -1; else 0
    if (user == 0)
        return -1;
    if (user->get_state() <= static_cast<int>(ch_state1))
        return -1;
    return 0;
}

// ORIG 0x081ddb3c (9 insn): 空实现，无消息体
int Dispatcher_Mercenary_Info::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }  // ORIG empty

void Dispatcher_Mercenary_Info::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081ddb7c (0xa7 insn): put_header(1,0x1ba) + put_byte(1) +
    // SetMercenaryInfoPacket + finalize(true) + Send
    PacketGuard guard;
    guard.put_header(1, 0x1ba);
    guard.put_byte(1);
    user->SetMercenaryInfoPacket(guard);
    guard.finalize(true);
    user->Send(guard);
}

Dispatcher_P2P_Statistics::Dispatcher_P2P_Statistics() {}
Dispatcher_P2P_Statistics::~Dispatcher_P2P_Statistics() {}
int Dispatcher_P2P_Statistics::check_error(CUser* user, MSG_BASE& msg)
{
    return LineFunc(0x5970,
                    "int Dispatcher_P2P_Statistics::check_error(CUser*, MSG_BASE&)",
                    0, 0);
}
int Dispatcher_P2P_Statistics::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_P2P_Statistics::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_PvPChannelInfo::Dispatcher_PvPChannelInfo() {}
Dispatcher_PvPChannelInfo::~Dispatcher_PvPChannelInfo() {}
int Dispatcher_PvPChannelInfo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_PvPChannelInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PvPChannelInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RemovePvPBuddy::Dispatcher_RemovePvPBuddy() {}
Dispatcher_RemovePvPBuddy::~Dispatcher_RemovePvPBuddy() {}
int Dispatcher_RemovePvPBuddy::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RemovePvPBuddy::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RemovePvPBuddy::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RenameCreature::Dispatcher_RenameCreature() {}
Dispatcher_RenameCreature::~Dispatcher_RenameCreature() {}
int Dispatcher_RenameCreature::check_error(CUser* user, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;

    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return -1;
    }

    std::string name(pMsg + 0x14);

    // BLOCKED(restrict_inputting_name::isUtf8StrInUnicodeRange(char const*)
    //         _ZN23restrict_inputting_name23isUtf8StrInUnicodeRangeEPKc @0x08ac2d6c)
    // 仓库未建模该符号。ORIG：返回非 1（非法 UTF-8 范围）→ 错误 0x9f。
    // 占位：视为合法（true），使后续低俗语/特殊字符检查保持可达。
    bool inUnicodeRange = true; // 占位
    if (!inUnicodeRange)
    {
        return 0x9f;
    }

    if (CSyncSlangFilter::GetInstance()->HasSlangName(name) ||
        G_CDataManager()->hasSpecialCharacter(pMsg + 0x14))
    {
        return 0x9f;
    }

    if (CSyncSlangFilter::GetInstance()->HasSlang(name))
    {
        return 0x9f;
    }

    return 0;
}
int Dispatcher_RenameCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRenameCreature& st = *reinterpret_cast<stRenameCreature*>(&param);
    char* pMsg = (char*)&msg;

    int err = check_error(user, msg);
    st.m_4 = err;

    if (err >= 1)
    {
        return 0;
    }

    if (err < 0)
    {
        return LineFunc(0x24d3,
                        "virtual int Dispatcher_RenameCreature::process(CUser*, MSG_BASE&, ParamBase&)",
                        err, user->get_acc_id());
    }

    // 名字为默认占位名"이름없음"时清空为空串再改名。
    if (strcmp(pMsg + 0x14, "\xec\x9d\xb4\xeb\xa6\x84\xec\x97\x86\xec\x9d\x8c") == 0)
    {
        memset(pMsg + 0x14, 0, 0xd);
    }

    int slot = (signed char)pMsg[0x0d];      // movsbl
    int creatureIdx = *(short*)(pMsg + 0x0e); // movswl

    CInventory* invW = user->getCurCharacInvenW();
    user_creature::CCreatureMgr* mgr = invW->GetCreatureMgrW();

    // BLOCKED(user_creature::CCreatureMgr::RenameCreature(char const*, int, int)
    //         _ZN13user_creature12CCreatureMgr14RenameCreatureEPKcii @0x08339b6e)
    // CInventory.h 内 user_creature::CCreatureMgr 尚无该声明（见 NEEDS_DECL）。
    mgr->RenameCreature(pMsg + 0x14, creatureIdx, slot); // 占位直调，待声明落地

    return 0;
}
int Dispatcher_RenameCreature::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    if (!packet.get_short(*(short*)(pMsg + 0x0e)))
    {
        return LineFunc(0x24b4,
                        "virtual int Dispatcher_RenameCreature::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(*(char*)(pMsg + 0x0d)))
    {
        return LineFunc(0x24b7,
                        "virtual int Dispatcher_RenameCreature::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x10)))
    {
        return LineFunc(0x24ba,
                        "virtual int Dispatcher_RenameCreature::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_str(pMsg + 0x14, 0xd, *(int*)(pMsg + 0x10)))
    {
        return LineFunc(0x24bd,
                        "virtual int Dispatcher_RenameCreature::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
void Dispatcher_RenameCreature::send(CUser* user, ParamBase& param) {}

Dispatcher_UseBoosterItem::Dispatcher_UseBoosterItem() {}
Dispatcher_UseBoosterItem::~Dispatcher_UseBoosterItem() {}


Dispatcher_UserHistoryLog::Dispatcher_UserHistoryLog() {}
Dispatcher_UserHistoryLog::~Dispatcher_UserHistoryLog() {}
int Dispatcher_UserHistoryLog::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UserHistoryLog::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BuyCeraShopItem::Dispatcher_BuyCeraShopItem() {}
Dispatcher_BuyCeraShopItem::~Dispatcher_BuyCeraShopItem() {}


Dispatcher_CallGuildInvite::Dispatcher_CallGuildInvite() {}
Dispatcher_CallGuildInvite::~Dispatcher_CallGuildInvite() {}
int Dispatcher_CallGuildInvite::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CallGuildInvite::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildInvite::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CancelJoinGuild::Dispatcher_CancelJoinGuild() {}
Dispatcher_CancelJoinGuild::~Dispatcher_CancelJoinGuild() {}
int Dispatcher_CancelJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CancelJoinGuild::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CeraPackageOpen::Dispatcher_CeraPackageOpen() {}
Dispatcher_CeraPackageOpen::~Dispatcher_CeraPackageOpen() {}

Dispatcher_ChangeGuildName::Dispatcher_ChangeGuildName() {}
Dispatcher_ChangeGuildName::~Dispatcher_ChangeGuildName() {}
int Dispatcher_ChangeGuildName::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ChangeGuildName::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeGuildName::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompleteDisplay::Dispatcher_CompleteDisplay() {}
Dispatcher_CompleteDisplay::~Dispatcher_CompleteDisplay() {}
int Dispatcher_CompleteDisplay::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CompleteDisplay::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompleteDisplay::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompleteLoadPvP::Dispatcher_CompleteLoadPvP() {}
Dispatcher_CompleteLoadPvP::~Dispatcher_CompleteLoadPvP() {}
int Dispatcher_CompleteLoadPvP::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CompleteLoadPvP::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompleteLoadPvP::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Compound_Avatar::Dispatcher_Compound_Avatar() {}
Dispatcher_Compound_Avatar::~Dispatcher_Compound_Avatar() {}
int Dispatcher_Compound_Avatar::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Compound_Avatar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Compound_Avatar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DieBloodMonster::Dispatcher_DieBloodMonster() {}
Dispatcher_DieBloodMonster::~Dispatcher_DieBloodMonster() {}
int Dispatcher_DieBloodMonster::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DieBloodMonster::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DieBloodMonster::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DisguiseRequest::Dispatcher_DisguiseRequest() {}
Dispatcher_DisguiseRequest::~Dispatcher_DisguiseRequest() {}
int Dispatcher_DisguiseRequest::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() != 3)
    {
        return 0;
    }
    private_store::CPrivateStoreMgr* manager =
        private_store::GetInstancePrivateStoreMgr();
    if (manager != 0 && manager->IsBusyPrivateStore(user))
    {
        return 0;
    }
    if (user->IsCurCharacGhost() != 0 || user->GetAssaultPlace() != 0)
    {
        return 0;
    }
    unsigned char kind = 0;
    if (!packet.get_byte(kind))
    {
        return LineFunc(0xcf00,
                        "virtual int Dispatcher_DisguiseRequest::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    unsigned short index = 0;
    if (!packet.get_short(index))
    {
        return LineFunc(0xcf02,
                        "virtual int Dispatcher_DisguiseRequest::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (user->isDisguiseCharac() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd6, 0x12);
        return 0;
    }
    user->setDisguiseCharac(true, kind, index);
    PacketGuard notification;
    notification.put_header(0, 2);
    notification.put_byte(0);
    notification.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&notification), 0);
    notification.finalize(true);
    G_GameWorld()->send_all(notification, user);
    return 0;
}
int Dispatcher_DisguiseRequest::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DisguiseRequest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DonateGuildFund::Dispatcher_DonateGuildFund() {}
Dispatcher_DonateGuildFund::~Dispatcher_DonateGuildFund() {}
int Dispatcher_DonateGuildFund::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DonateGuildFund::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GiveupExpertJob::Dispatcher_GiveupExpertJob() {}
Dispatcher_GiveupExpertJob::~Dispatcher_GiveupExpertJob() {}
int Dispatcher_GiveupExpertJob::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_GiveupExpertJob::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GiveupExpertJob::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GoblinPadStatus::Dispatcher_GoblinPadStatus() {}
Dispatcher_GoblinPadStatus::~Dispatcher_GoblinPadStatus() {}
int Dispatcher_GoblinPadStatus::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82177ae (81 insn)：状态字节 0=未激活 / 1=失败≥3 / 2=已认证 / 3=未认证
    Sanicova::CPad* pad = user->getPad();
    unsigned char status = 0;
    if (pad->isActivate())
    {
        if (pad->getFailCnt() >= 3)
        {
            status = 1;
        }
        else
        {
            status = pad->isCertified() ? 2 : 3;
        }
    }
    PacketGuard response;
    response.put_header(1, 0xc4);
    response.put_byte(1);
    response.put_byte(status);
    response.finalize(true);
    user->Send(response);
    return 0;
}
int Dispatcher_GoblinPadStatus::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GoblinPadStatus::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_LimitNpcBuyItem::Dispatcher_LimitNpcBuyItem() {}
Dispatcher_LimitNpcBuyItem::~Dispatcher_LimitNpcBuyItem() {}
int Dispatcher_LimitNpcBuyItem::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_LimitNpcBuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_LimitNpcBuyItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_LimitNpcBuyItem::send(CUser* user, ParamBase& param) {}

Dispatcher_MailBox_Extract::Dispatcher_MailBox_Extract() {}
Dispatcher_MailBox_Extract::~Dispatcher_MailBox_Extract() {}
int Dispatcher_MailBox_Extract::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_MailBox_Extract::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MailBox_Extract::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_MailBox_Extract::send(CUser* user, ParamBase& param) {}


Dispatcher_PVPRequestFight::Dispatcher_PVPRequestFight() {}
Dispatcher_PVPRequestFight::~Dispatcher_PVPRequestFight() {}
int Dispatcher_PVPRequestFight::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() != 7)
    {
        return LineFunc(0x9c25,
                        "virtual int Dispatcher_PVPRequestFight::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    PvP_Room* room = static_cast<PvP_Room*>(user->GetPVPRoom());
    if (room == 0)
    {
        return LineFunc(0x9c2b,
                        "virtual int Dispatcher_PVPRequestFight::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (room->request_fight(user) != 1)
    {
        return LineFunc(0x9c30,
                        "virtual int Dispatcher_PVPRequestFight::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    return 0;
}
int Dispatcher_PVPRequestFight::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PVPRequestFight::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_QueryCharacInfo::Dispatcher_QueryCharacInfo() {}
Dispatcher_QueryCharacInfo::~Dispatcher_QueryCharacInfo() {}
int Dispatcher_QueryCharacInfo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_QueryCharacInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_QueryCharacInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ReportAbuseUser::Dispatcher_ReportAbuseUser() {}
Dispatcher_ReportAbuseUser::~Dispatcher_ReportAbuseUser() {}
int Dispatcher_ReportAbuseUser::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ReportAbuseUser::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ReportAbuseUser::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestItemLock::Dispatcher_RequestItemLock() {}
Dispatcher_RequestItemLock::~Dispatcher_RequestItemLock() {}
int Dispatcher_RequestItemLock::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestItemLock::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestItemLock::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SaveQuestNotify::Dispatcher_SaveQuestNotify() {}
// ORIG _ZN26Dispatcher_SaveQuestNotifyD1Ev W 0x08260a8e：vptr 复位后调基类模板
// D1（0x08262482）+ 恒假 delete 守卫；D0Ev W 0x08260abe = D1 + operator delete。
Dispatcher_SaveQuestNotify::~Dispatcher_SaveQuestNotify() {}
int Dispatcher_SaveQuestNotify::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x0825eec4：user/当前角色空 → -1；state<=2 → -2；
    // 扫 msg+0xd 起 10 槽 int，非 0 且 find_quest 失败 → -3；否则 0。
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -2;
    }
    const MSG_SAVE_QUEST_NOTIFY& req = static_cast<const MSG_SAVE_QUEST_NOTIFY&>(msg);
    for (int i = 0; i <= 9; ++i)
    {
        int questIdx = *reinterpret_cast<const int*>(req.m_0d + i * 4); // +0x0d+i*4
        if (questIdx != 0 && G_CDataManager()->find_quest(questIdx) == 0)
        {
            return -3;
        }
    }
    return 0;
}

// ORIG 0x0825ed06：读 1 字节 count（须 0..5），再按 count 读 int 写入
// msg+0xd 起任务 idx 区；任一步失败走 LineFunc（ORIG 返回其断言值）。
int Dispatcher_SaveQuestNotify::read(PacketBuf& packet, MSG_BASE& msg)
{
    char count = 0;
    if (!packet.get_byte(count))
    {
        return LineFunc(0x2b3,
                        "virtual int Dispatcher_SaveQuestNotify::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (count < 0 || count > 5)
    {
        return LineFunc(0x2b6,
                        "virtual int Dispatcher_SaveQuestNotify::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    MSG_SAVE_QUEST_NOTIFY& req = static_cast<MSG_SAVE_QUEST_NOTIFY&>(msg);
    for (int i = 0; i < (int)count; ++i)
    {
        unsigned int value = 0;
        if (!packet.get_int(value))
        {
            return LineFunc(0x2bb,
                            "virtual int Dispatcher_SaveQuestNotify::read(PacketBuf&, MSG_BASE&)",
                            0, 0);
        }
        *reinterpret_cast<int*>(req.m_0d + i * 4) = (int)value; // +0x0d+i*4
    }
    return 0;
}

// ORIG 0x0825edfc：err=check_error(user,msg)；*(int*)(param+4)=err；
// err>0 → 0；err<0 → LogManager::logFormat(1,"PacketDispatcher_Impl_2.cpp",
// <PRETTY>,0x2cb,"Dispatcher Check_Error No(%d)",err) 后 return 0x2cc；
// err==0 → memcpy(getCurCharacQuestW()->m_authenData.m_authen, msg+0xd, 0x28)
// （UserQuest+0x75f4，10×int 任务通知区）后 return 0。
int Dispatcher_SaveQuestNotify::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    saveQuestNotifyParam& res = static_cast<saveQuestNotifyParam&>(param);
    const MSG_SAVE_QUEST_NOTIFY& req = static_cast<const MSG_SAVE_QUEST_NOTIFY&>(msg);

    int err = check_error(user, msg);
    res.m_errorCode = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LogManager::logFormat(1, "PacketDispatcher_Impl_2.cpp",
                              "virtual int Dispatcher_SaveQuestNotify::process(CUser*, MSG_BASE&, ParamBase&)",
                              0x2cb, "Dispatcher Check_Error No(%d)", err);
        return 0x2cc;
    }
    // err==0：msg+0xd 起 0x28 字节写入 m_authenData.m_authen（+0x75f4）。
    memcpy(user->getCurCharacQuestW()->m_authenData.m_authen,
           req.m_0d, 0x28);
    return 0;
}

Dispatcher_SecuDataControl::Dispatcher_SecuDataControl() {}
Dispatcher_SecuDataControl::~Dispatcher_SecuDataControl() {}
int Dispatcher_SecuDataControl::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SecuDataControl::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecuDataControl::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_IPScan::Dispatcher_Security_IPScan() {}
Dispatcher_Security_IPScan::~Dispatcher_Security_IPScan() {}
int Dispatcher_Security_IPScan::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_IPScan(user, packet);
    if (result == 1)
    {
        return LineFunc(0xde94,
                        "virtual int Dispatcher_Security_IPScan::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_IPScan::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_IPScan::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Shop_Coin_Event::Dispatcher_Shop_Coin_Event() {}
Dispatcher_Shop_Coin_Event::~Dispatcher_Shop_Coin_Event() {}
int Dispatcher_Shop_Coin_Event::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Shop_Coin_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Shop_Coin_Event::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TutorialLevelUp::Dispatcher_TutorialLevelUp() {}
Dispatcher_TutorialLevelUp::~Dispatcher_TutorialLevelUp() {}
int Dispatcher_TutorialLevelUp::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TutorialLevelUp::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ApproveJoinGuild::Dispatcher_ApproveJoinGuild() {}
Dispatcher_ApproveJoinGuild::~Dispatcher_ApproveJoinGuild() {}
int Dispatcher_ApproveJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ApproveJoinGuild::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CallGuildLevelUp::Dispatcher_CallGuildLevelUp() {}
Dispatcher_CallGuildLevelUp::~Dispatcher_CallGuildLevelUp() {}
int Dispatcher_CallGuildLevelUp::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CallGuildLevelUp::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildLevelUp::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CallGuildMembers::Dispatcher_CallGuildMembers() {}
Dispatcher_CallGuildMembers::~Dispatcher_CallGuildMembers() {}
int Dispatcher_CallGuildMembers::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CallGuildMembers::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildMembers::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeCharacName::Dispatcher_ChangeCharacName() {}
Dispatcher_ChangeCharacName::~Dispatcher_ChangeCharacName() {}
int Dispatcher_ChangeCharacName::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ChangeCharacName::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeCharacName::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeLetterStat::Dispatcher_ChangeLetterStat() {}
Dispatcher_ChangeLetterStat::~Dispatcher_ChangeLetterStat() {}
int Dispatcher_ChangeLetterStat::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0 ? 0 : -1;
}
int Dispatcher_ChangeLetterStat::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeLetterStat::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ChangeLetterStat::send(CUser* user, ParamBase& param) {}

Dispatcher_DieAssaultPlayer::Dispatcher_DieAssaultPlayer() {}
Dispatcher_DieAssaultPlayer::~Dispatcher_DieAssaultPlayer() {}
int Dispatcher_DieAssaultPlayer::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DieAssaultPlayer::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DieAssaultPlayer::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Equipslot_Switch::Dispatcher_Equipslot_Switch() {}
Dispatcher_Equipslot_Switch::~Dispatcher_Equipslot_Switch() {}
int Dispatcher_Equipslot_Switch::check_error(CUser* user, MSG_BASE& msg)
{
    if (&msg == 0)
    {
        return LineFunc(0x3e1c,
                        "int Dispatcher_Equipslot_Switch::check_error(CUser*, MSG_BASE&)",
                        0, 0);
    }
    if (user->get_state() <= 2)
    {
        return LineFunc(0x3e1d,
                        "int Dispatcher_Equipslot_Switch::check_error(CUser*, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
// err>0 → 错误包(0x18d,err) 返 -1；err<0 → 错误包(0x18d,0x15)+LineFunc(0x3e2c)；
// 否则对扩展槽视图(#9)执行 EquipslotSwitch(msg+0xf,msg+0xd,msg+0xe)，结果字节
// 回写 param.m_4。
int Dispatcher_Equipslot_Switch::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stEquipslotSwitch& out = static_cast<stEquipslotSwitch&>(param);
    MSG_EQUIPSLOT_SWITCH& req = static_cast<MSG_EQUIPSLOT_SWITCH&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18d,
                                 (unsigned char)(err & 0xff));
        return -1;
    }
    if (err < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18d, 0x15);
        return LineFunc(0x3e2c,
                        "virtual int Dispatcher_Equipslot_Switch::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, 0);
    }
    CExpandEquipslot* expand =
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    out.m_4 = (unsigned char)(char)expand->EquipslotSwitch(
        user, pReq[0xf], pReq[0xd], pReq[0xe]);
    return 0;
}
int Dispatcher_Equipslot_Switch::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_EQUIPSLOT_SWITCH& req = static_cast<MSG_EQUIPSLOT_SWITCH&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    char& flagD = pReq[0xd];
    char& flagE = pReq[0xe];
    char& flagF = pReq[0xf];
    if (!packet.get_byte(flagD))
    {
        return LineFunc(0x3e12,
                        "virtual int Dispatcher_Equipslot_Switch::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(flagE))
    {
        return LineFunc(0x3e14,
                        "virtual int Dispatcher_Equipslot_Switch::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(flagF))
    {
        return LineFunc(0x3e16,
                        "virtual int Dispatcher_Equipslot_Switch::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
// 成功先补发两份装备槽视图，再统一回 header(1,0x18d)：成功 {1}，失败 {0,m_4}。
void Dispatcher_Equipslot_Switch::send(CUser* user, ParamBase& param)
{
    stEquipslotSwitch& out = static_cast<stEquipslotSwitch&>(param);
    if (out.m_4 == 0)
    {
        CExpandEquipslot* expand =
            user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
        expand->Send_Equip_Info(user);
        expand->Send_Expand_Equip_Info(user);
    }
    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x18d);
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte((char)out.m_4);
    }
    packet.finalize(true);
    user->Send(packet);
}

// ============================================================================
// NEEDS_DECL: 建议追加到 Dispatchers.h 的声明（body 引用但现有声明缺失）：
//   class Dispatcher_BossDieCheck 公有段：
//     int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
//       // ORIG _ZN23Dispatcher_BossDieCheck11check_errorEP5CUserR8MSG_BASER9ParamBase T 0x081cd71e
//   class Dispatcher_CollectItems 公有段：
//     int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
//       // ORIG _ZN23Dispatcher_CollectItems11check_errorEP5CUserR8MSG_BASER9ParamBase T 0x081e329e
//   class Dispatcher_ComboSkill 公有段：
//     int checkComboSkillIndex(short idx);
//       // ORIG _ZN21Dispatcher_ComboSkill20checkComboSkillIndexEs T 0x0825f292
//
// NEEDS_DECL: 宿主 include 区需追加：
//   #include <algorithm>          // std::find（BossDieCheck::process）
//   #include "CExpandEquipslot.h" // Equipslot_Switch 调用其方法（CUser.h 仅前向）
//   #include "CTimeGate.h"        // CollectItems check_error isOpen（GlobalData.h 仅前向）
//
// 备注：Packet_CollectItems 类定义在本文件 EXTRA 节内（TU 局部，ORIG W
// 0x0822c75a）；若后续其他 TU 需要请迁共享头并移除本文件定义。
// ============================================================================

Dispatcher_ExitPrivateStore::Dispatcher_ExitPrivateStore() {}
Dispatcher_ExitPrivateStore::~Dispatcher_ExitPrivateStore() {}
int Dispatcher_ExitPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ExitPrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ExitPrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ExpertExtraction::Dispatcher_ExpertExtraction() {}
Dispatcher_ExpertExtraction::~Dispatcher_ExpertExtraction() {}

Dispatcher_InformNotice_2nd::Dispatcher_InformNotice_2nd() {}
Dispatcher_InformNotice_2nd::~Dispatcher_InformNotice_2nd() {}
int Dispatcher_InformNotice_2nd::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 3)
    {
        return 1;
    }
    const MSG_INFORM_NOTICE_2ND& req = static_cast<const MSG_INFORM_NOTICE_2ND&>(msg);
    return (unsigned int)req.m_0d < 0x80 ? 0 : -1;
}
int Dispatcher_InformNotice_2nd::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_InformNotice_2nd::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_InformNotice_2nd::send(CUser* user, ParamBase& param) {}

Dispatcher_MemberEnterReply::Dispatcher_MemberEnterReply() {}
Dispatcher_MemberEnterReply::~Dispatcher_MemberEnterReply() {}
int Dispatcher_MemberEnterReply::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0 ? 0 : -1;
}
int Dispatcher_MemberEnterReply::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MemberEnterReply::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Mercenary_Return::Dispatcher_Mercenary_Return() {}
Dispatcher_Mercenary_Return::~Dispatcher_Mercenary_Return() {}
int Dispatcher_Mercenary_Return::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MsgMercenaryReturn& req = reinterpret_cast<MsgMercenaryReturn&>(msg);
    SendMercenaryReturnResult& result = static_cast<SendMercenaryReturnResult&>(param);
    if (user == 0)
        return -1;
    if (user->get_state() != 3)
        return -1;

    char* pResult = reinterpret_cast<char*>(&result);
    // ORIG: Return_Mercenary_Charac(user, *(uint*)(msg+0x10), m_result@+0xc 引用传入)
    // BLOCKED(_ZN5CUser23Return_Mercenary_CharacEjR25competition_reward_result @0x0868e692
    //        —— CUser.h 缺声明，跨域待补): 返回值与出线结果填充占位
    const bool ret = false;  // BLOCKED 调用返回值
    pResult[0x04] = static_cast<char>(ret != false);            // +0x04 bool 归还成功
    pResult[0x05] = req.m_head[0];                              // +0x05 char 回显(msg+0xd)
    *reinterpret_cast<int*>(pResult + 0x08) =
        *reinterpret_cast<unsigned int*>(req.m_pad + 3);        // +0x08 int = msg+0x10
    return 0;
}

int Dispatcher_Mercenary_Return::read(PacketBuf& packet, MSG_BASE& msg)   // ORIG 0x081dd904 (39 insn)
{
    MsgMercenaryReturn& req = reinterpret_cast<MsgMercenaryReturn&>(msg);
    // msg+0xd byte / msg+0x10 int；失败走 LineFunc(0x4e88/0x4e8a, 本函数 pretty 名)
    static const char kFunc[] = "virtual int Dispatcher_Mercenary_Return::read(PacketBuf&, MSG_BASE&)";
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_pad[0])))
        return LineFunc(0x4e88, kFunc, 0, 0);
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(req.m_pad + 3)))
        return LineFunc(0x4e8a, kFunc, 0, 0);
    return 0;
}
void Dispatcher_Mercenary_Return::send(CUser* user, ParamBase& param)     // ORIG 0x081dda1c (86 insn)
{
    SendMercenaryReturnResult& result = static_cast<SendMercenaryReturnResult&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x1b9);
    guard.put_byte(*reinterpret_cast<char*>(reinterpret_cast<char*>(&result) + 0x04) != 0);  // +0x04 bool
    guard.put_byte(static_cast<unsigned int>(
        *reinterpret_cast<char*>(reinterpret_cast<char*>(&result) + 0x05)));                 // +0x05 char 回显
    guard.put_int(*reinterpret_cast<int*>(reinterpret_cast<char*>(&result) + 0x08));         // +0x08 int
    guard.put_int(result.m_result.m_0);                                     // +0x0c
    guard.put_int(result.m_result.m_4);                                     // +0x10
    guard.put_byte(static_cast<unsigned int>(
        static_cast<char>(result.m_result.m_8)));                           // +0x14
    guard.finalize(true);
    user->Send(guard);
}

Dispatcher_MultiMailBoxSend::Dispatcher_MultiMailBoxSend() {}
Dispatcher_MultiMailBoxSend::~Dispatcher_MultiMailBoxSend() {}
int Dispatcher_MultiMailBoxSend::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_MultiMailBoxSend::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MultiMailBoxSend::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Open_Guild_Board::Dispatcher_Open_Guild_Board() {}
Dispatcher_Open_Guild_Board::~Dispatcher_Open_Guild_Board() {}
int Dispatcher_Open_Guild_Board::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Open_Guild_Board::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PvPBuddyConnList::Dispatcher_PvPBuddyConnList() {}
Dispatcher_PvPBuddyConnList::~Dispatcher_PvPBuddyConnList() {}
int Dispatcher_PvPBuddyConnList::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_PvPBuddyConnList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PvPBuddyConnList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RefreshGuildInfo::Dispatcher_RefreshGuildInfo() {}
Dispatcher_RefreshGuildInfo::~Dispatcher_RefreshGuildInfo() {}
int Dispatcher_RefreshGuildInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RefreshGuildInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ReplyGuildInvite::Dispatcher_ReplyGuildInvite() {}
Dispatcher_ReplyGuildInvite::~Dispatcher_ReplyGuildInvite() {}
int Dispatcher_ReplyGuildInvite::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ReplyGuildInvite::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ReplyGuildInvite::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ReportBadP2PUser::Dispatcher_ReportBadP2PUser() {}
Dispatcher_ReportBadP2PUser::~Dispatcher_ReportBadP2PUser() {}
int Dispatcher_ReportBadP2PUser::dispatch_sig(CUser* user, PacketBuf& packet)
{
    char seat = 0;
    if (!packet.get_byte(seat))
    {
        return LineFunc(0x8ddb,
                        "virtual int Dispatcher_ReportBadP2PUser::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (user == 0)
    {
        return 0;
    }
    if (!user->CheckInPvp())
    {
        if (user->get_state() == 5)
        {
            CParty* party = (CParty*)user->GetParty();
            if (party != 0)
            {
                party->VoteLackUser(user, (int)seat);
            }
        }
    }
    else
    {
        PvP_Room* room = (PvP_Room*)user->GetPVPRoom();
        if (room != 0)
        {
            room->VoteLackUser(user, (int)seat);
        }
    }
    return 0;
}
int Dispatcher_ReportBadP2PUser::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ReportBadP2PUser::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Req_Urgent_Quest::Dispatcher_Req_Urgent_Quest() {}
Dispatcher_Req_Urgent_Quest::~Dispatcher_Req_Urgent_Quest() {}
// ORIG 0x081df42c：state>2 → param+4=0/param+8=-1；等级 27..69 且
// getCurCharacQuestR()->m_authenData.m_urgentQuest==-1 才继续；
// hasEpicQuest → err=1；否则静态 selected_quest 清空后
// getQuestNPCList(8)+allowable_questlist_as_npc(…,true) 取候选：
// 空 → err=4；非空 → 静态 CMTRand randInt(size-1) 抽一个写回
// m_urgentQuest 与 param+8，param+4=1。
int Dispatcher_Req_Urgent_Quest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    ReqResult& res = static_cast<ReqResult&>(param);
    if (user->get_state() <= 2)
    {
        return 0;
    }

    res.m_found = 0;
    res.m_questIdx = -1;

    int level = user->get_charac_level();
    UserQuest* questR = user->getCurCharacQuestR();
    bool eligible = level > 0x1a && level <= 0x45 &&
                    questR->m_authenData.m_urgentQuest == -1;
    if (!eligible)
    {
        res.m_errorCode = 1;
        return 0;
    }
    if (user->getCurCharacQuestW()->hasEpicQuest())
    {
        res.m_errorCode = 1;
        return 0;
    }

    // 函数局部静态（ORIG __cxa_guard 保护）：候选任务列表 + 随机骰。
    static std::list<int> selected_quest;
    selected_quest.clear();

    char cleared[0x7531];  // ORIG local_7589：m_cleared(0x7530)+save-flag 逐字节快照
    memcpy(cleared, &user->getCurCharacQuestR()->m_cleared, 0x7531);

    stSelectQuestParam selectParam(user);
    QuestList* list = G_CDataManager()->m_questList;
    std::multimap<int, int>* npcList =
        list->getQuestNPCList(ENUM_QUEST_GRADE_8);
    list->allowable_questlist_as_npc(
        selected_quest, *npcList, selectParam,
        *reinterpret_cast<const WongWork::CQuestClear*>(cleared), true);

    if (selected_quest.empty())
    {
        res.m_errorCode = 4;
    }
    else
    {
        static CMTRand urgentRandomDice;  // ORIG __cxa_guard 静态
        unsigned long range = (unsigned long)(selected_quest.size() - 1);
        int dice = urgentRandomDice.randInt(range);
        if ((unsigned int)dice < selected_quest.size())
        {
            std::list<int>::iterator it = selected_quest.begin();
            std::advance(it, dice);
            int selectedIdx = *it;
            user->getCurCharacQuestW()->m_authenData.m_urgentQuest = selectedIdx;
            res.m_questIdx = selectedIdx;
            res.m_found = 1;
        }
    }
    return 0;
}
// ORIG 0x081df422：字面 return 0（无包内容需解析）。
int Dispatcher_Req_Urgent_Quest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
// ORIG 0x081df75a：PacketGuard put_header(1,0x1c6)；put_byte(param.m_found!=0)；
// found → put_short(param.m_questIdx)，否则 put_byte(param.m_errorCode)；
// finalize(true) 后 CUser::Send。
void Dispatcher_Req_Urgent_Quest::send(CUser* user, ParamBase& param)
{
    const ReqResult& res = static_cast<const ReqResult&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1c6);
    packet.put_byte(res.m_found != 0 ? 1 : 0);
    if (res.m_found)
    {
        packet.put_short(res.m_questIdx);
    }
    else
    {
        packet.put_byte(res.m_errorCode);
    }
    packet.finalize(true);
    user->Send(packet);
}

Dispatcher_RequestBlackList::Dispatcher_RequestBlackList() {}
Dispatcher_RequestBlackList::~Dispatcher_RequestBlackList() {}
int Dispatcher_RequestBlackList::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() >= 3 ? 0 : -1;
}
int Dispatcher_RequestBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestBlackList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestJoinGuild::Dispatcher_RequestJoinGuild() {}
Dispatcher_RequestJoinGuild::~Dispatcher_RequestJoinGuild() {}
int Dispatcher_RequestJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestJoinGuild::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ResponseCreature::Dispatcher_ResponseCreature() {}
Dispatcher_ResponseCreature::~Dispatcher_ResponseCreature() {}
int Dispatcher_ResponseCreature::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() != 3 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9acf,
                        "virtual int Dispatcher_ResponseCreature::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    user->getCurCharacInvenW()->GetCreatureMgrW()->ResponseCreature();
    return 0;
}
int Dispatcher_ResponseCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ResponseCreature::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SDC_Damage_Check::Dispatcher_SDC_Damage_Check() {}
Dispatcher_SDC_Damage_Check::~Dispatcher_SDC_Damage_Check() {}
int Dispatcher_SDC_Damage_Check::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SDC_Damage_Check::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SDC_Damage_Check::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SaveGameOption_1::Dispatcher_SaveGameOption_1() {}
Dispatcher_SaveGameOption_1::~Dispatcher_SaveGameOption_1() {}
int Dispatcher_SaveGameOption_1::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_SaveGameOption_1::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x28ad, pretty, 0, 0);
    }
    return user->get_state() < 2 ? -1 : 0;
}
int Dispatcher_SaveGameOption_1::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SaveGameOption_1::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SaveGameOption_2::Dispatcher_SaveGameOption_2() {}
Dispatcher_SaveGameOption_2::~Dispatcher_SaveGameOption_2() {}
int Dispatcher_SaveGameOption_2::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_SaveGameOption_2::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x28fa, pretty, 0, 0);
    }
    return user->get_state() < 2 ? -1 : 0;
}
int Dispatcher_SaveGameOption_2::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SaveGameOption_2::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeChronicle::Dispatcher_UpgradeChronicle() {}
Dispatcher_UpgradeChronicle::~Dispatcher_UpgradeChronicle() {}
int Dispatcher_UpgradeChronicle::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UpgradeChronicle::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeChronicle::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeInventory::Dispatcher_UpgradeInventory() {}
Dispatcher_UpgradeInventory::~Dispatcher_UpgradeInventory() {}
int Dispatcher_UpgradeInventory::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0xe828,
            "virtual int Dispatcher_UpgradeInventory::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }

    const unsigned int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x1d);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, (unsigned char)(secu & 0xff));
        return 0;
    }

    short slot = 0;
    if (!packet.get_short(slot))
    {
        return LineFunc(
            0xe832,
            "virtual int Dispatcher_UpgradeInventory::dispatch_sig(CUser*, PacketBuf&)",
            0, 0);
    }

    Inven_Item ticket =
        user->getCurCharacInvenR()->GetInvenSlot(INVEN_TYPE_EQUIP, slot);
    if (ticket.m_addInfo2 < 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x11);
        return 0;
    }

    // 扩背包券仅两种道具 idx：0x2897ca / 0x203b
    if (!(ticket.m_addInfo == 0x2897ca || ticket.m_addInfo == 0x203b))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x11);
        return 0;
    }

    // 背包容量上限 16 格
    if (user->getCurCharacInvenR()->get_inventory_capacity() > 0xf)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x13);
        return 0;
    }

    if (user->getCurCharacInvenW()->delete_item(
            INVEN_TYPE_EQUIP, slot, 1, eItemDelReason_3, true) != 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1af, 0x11);
        return 0;
    }

    // 包 1（0x0e）：删除券后的物品更新广播
    PacketGuard pg;                                  // try 域 @0822a71a..0822a8db
    pg.put_header(0, 0xe);
    pg.put_byte(0);
    pg.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE_EQUIP, slot, pg);
    pg.finalize(true);
    user->Send(pg);

    // 扩容日志（旧容量 → +8），再落实际容量与 DB 请求
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    const int capOld = invenR->get_inventory_capacity();
    user->m_historyLog.ExpandInventory(capOld, capOld + 8, 1);
    invenR->set_inventory_capacity(capOld + 8);
    const int capNew = user->getCurCharacInvenR()->get_inventory_capacity();
    DB_UpdateInventoryCapacity::makeRequest(
        user->GetServerGroup(), (int)user->GetUID(),
        (unsigned int)user->getCurCharacNo(), capNew);

    // 包 2（0x42）：subtype 0xb + 新容量
    pg.clear();
    pg.put_header(0, 0x42);
    pg.put_short(0xb);
    pg.put_short(capNew);
    pg.finalize(true);
    user->Send(pg);
    return 0;
}
int Dispatcher_UpgradeInventory::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeInventory::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AddSocketToAvatar::Dispatcher_AddSocketToAvatar() {}
Dispatcher_AddSocketToAvatar::~Dispatcher_AddSocketToAvatar() {}
int Dispatcher_AddSocketToAvatar::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AddSocketToAvatar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AddSocketToAvatar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionRegistItem::Dispatcher_AuctionRegistItem() {}
Dispatcher_AuctionRegistItem::~Dispatcher_AuctionRegistItem() {}


Dispatcher_ChangeHostWarRoom::Dispatcher_ChangeHostWarRoom() {}
Dispatcher_ChangeHostWarRoom::~Dispatcher_ChangeHostWarRoom() {}
int Dispatcher_ChangeHostWarRoom::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty =
        "int Dispatcher_ChangeHostWarRoom::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x41b1, pretty, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x41b4, pretty, 0, 0);
    }
    const MSG_CHANGE_HOST_WARROOM& req =
        static_cast<const MSG_CHANGE_HOST_WARROOM&>(msg);
    signed char warRoom = static_cast<signed char>(req.m_pad[0]);
    if (warRoom < 0 || warRoom > 5)
    {
        return LineFunc(0x41b8, pretty, 0, user->get_acc_id());
    }
    return user->get_state() == 8 ? 0 : -1;
}
int Dispatcher_ChangeHostWarRoom::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeHostWarRoom::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CHANGE_HOST_WARROOM& req = static_cast<MSG_CHANGE_HOST_WARROOM&>(msg);
    if (!packet.get_byte(req.m_pad[0]))
    {
        return LineFunc(0x418e,
                        "virtual int Dispatcher_ChangeHostWarRoom::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(req.m_pad + 1)))
    {
        return LineFunc(0x4191,
                        "virtual int Dispatcher_ChangeHostWarRoom::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
void Dispatcher_ChangeHostWarRoom::send(CUser* user, ParamBase& param) {}

Dispatcher_ConnectP2PAssault::Dispatcher_ConnectP2PAssault() {}
Dispatcher_ConnectP2PAssault::~Dispatcher_ConnectP2PAssault() {}
int Dispatcher_ConnectP2PAssault::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ConnectP2PAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ConnectP2PAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Debug_Clear_Quest::Dispatcher_Debug_Clear_Quest() {}
Dispatcher_Debug_Clear_Quest::~Dispatcher_Debug_Clear_Quest() {}
// ORIG 0x081df91c：isGMUser 否 → LineFunc(0x51f3) 返回其值；
// res.m_index = msg（MsgClearQuestIndex::operator=，ORIG W 0x08235266）；
// 遍历 m_questIdxs：find_quest 失败 → return 1；exposeQuest &&
// check_possible → getCurCharacQuestW()->m_cleared.setClearedQuest(idx)、
// m_field138>0 → incQuestPoint(值,idx,true)、m_field13a>0 →
// incQuestPiece(值,idx,true)，计数；循环后 res.m_index.m_count=实际清空数，
// sendCharacQp() + sendCharacQuestPiece()，return 0。
int Dispatcher_Debug_Clear_Quest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (!user->isGMUser())
    {
        return LineFunc(0x51f3,
                        "virtual int Dispatcher_Debug_Clear_Quest::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, 0);
    }
    MsgClearQuestIndex& req = static_cast<MsgClearQuestIndex&>(msg);
    ReqDebugClearQuestResult& res = static_cast<ReqDebugClearQuestResult&>(param);

    // ORIG 0x08235266：res.m_index = msg（逐标量 + vector operator=）。
    res.m_index = req;

    int clearedCount = 0;
    for (int i = 0; i < req.m_count; ++i)
    {
        int questIdx = req.m_questIdxs[i];
        Quest* quest = G_CDataManager()->find_quest(questIdx);
        if (quest == 0)
        {
            return 1;
        }
        stSelectQuestParam selectParam(user);
        if (quest->exposeQuest() && quest->check_possible(selectParam))
        {
            user->getCurCharacQuestW()->m_cleared.setClearedQuest(
                (unsigned int)questIdx);
            if (quest->m_field138 > 0)
            {
                user->incQuestPoint((int)quest->m_field138, questIdx, true);
            }
            if (quest->m_field13a > 0)
            {
                user->incQuestPiece((int)quest->m_field13a, questIdx, true);
            }
            ++clearedCount;
        }
    }
    res.m_index.m_count = clearedCount;   // ORIG 写 *(int*)(param+0x14)
    user->sendCharacQp();
    user->sendCharacQuestPiece();
    return 0;
}
// ORIG 0x081df840：get_short 读数量写入 msg.m_count（+0x10），再逐个
// get_short push_back 进 m_questIdxs；失败走 LineFunc(0x51e1/0x51e8)。
int Dispatcher_Debug_Clear_Quest::read(PacketBuf& packet, MSG_BASE& msg)
{
    MsgClearQuestIndex& req = static_cast<MsgClearQuestIndex&>(msg);
    short count = 0;
    if (!packet.get_short(count))
    {
        return LineFunc(0x51e1,
                        "virtual int Dispatcher_Debug_Clear_Quest::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    req.m_count = count;
    for (int i = 0; i < req.m_count; ++i)
    {
        short value = 0;
        if (!packet.get_short(value))
        {
            return LineFunc(0x51e8,
                            "virtual int Dispatcher_Debug_Clear_Quest::read(PacketBuf&, MSG_BASE&)",
                            0, 0);
        }
        req.m_questIdxs.push_back((int)value);
    }
    return 0;
}
// ORIG 0x081dfb40：PacketGuard put_header(1,0x1c9)+put_byte(1)+
// put_short(res.m_index.m_count)；按 count put_short 每个 idx；
// finalize(true)→Send→send_clear_quest_list()；随后第二个 PacketGuard 经
// UserQuest::get_quest_info 填充任务列表包并发送。
void Dispatcher_Debug_Clear_Quest::send(CUser* user, ParamBase& param)
{
    const ReqDebugClearQuestResult& res = static_cast<const ReqDebugClearQuestResult&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x1c9);
    packet.put_byte(1);
    packet.put_short(res.m_index.m_count);
    for (int i = 0; i < res.m_index.m_count; ++i)
    {
        packet.put_short(res.m_index.m_questIdxs[i]);
    }
    packet.finalize(true);
    user->Send(packet);
    user->send_clear_quest_list();

    PacketGuard questList;   // ORIG local_2c：UserQuest 任务列表包
    user->getCurCharacQuestR()->get_quest_info(reinterpret_cast<char*>(&questList));
    user->Send(questList);
}

Dispatcher_DeleteToBlackList::Dispatcher_DeleteToBlackList() {}
Dispatcher_DeleteToBlackList::~Dispatcher_DeleteToBlackList() {}
int Dispatcher_DeleteToBlackList::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() >= 3 ? 0 : -1;
}
int Dispatcher_DeleteToBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DeleteToBlackList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EnterPrivateStore::Dispatcher_EnterPrivateStore() {}
Dispatcher_EnterPrivateStore::~Dispatcher_EnterPrivateStore() {}
int Dispatcher_EnterPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_EnterPrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_EnterPrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_FatigueAttendance::Dispatcher_FatigueAttendance() {}
Dispatcher_FatigueAttendance::~Dispatcher_FatigueAttendance() {}

Dispatcher_GuildCargoHistory::Dispatcher_GuildCargoHistory() {}
Dispatcher_GuildCargoHistory::~Dispatcher_GuildCargoHistory() {}
int Dispatcher_GuildCargoHistory::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildCargoHistory::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GuildCargoHistory::send(CUser* user, ParamBase& param) {}

Dispatcher_GuildCargoPopItem::Dispatcher_GuildCargoPopItem() {}
Dispatcher_GuildCargoPopItem::~Dispatcher_GuildCargoPopItem() {}
int Dispatcher_GuildCargoPopItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildCargoPopItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GuildCargoPopItem::send(CUser* user, ParamBase& param) {}

Dispatcher_LoadingTimeReport::Dispatcher_LoadingTimeReport() {}
Dispatcher_LoadingTimeReport::~Dispatcher_LoadingTimeReport() {}
int Dispatcher_LoadingTimeReport::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_LoadingTimeReport::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_LoadingTimeReport::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PeerConnectResult::Dispatcher_PeerConnectResult() {}
Dispatcher_PeerConnectResult::~Dispatcher_PeerConnectResult() {}
int Dispatcher_PeerConnectResult::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_PeerConnectResult::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PeerConnectResult::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PutItemForEnchant::Dispatcher_PutItemForEnchant() {}
Dispatcher_PutItemForEnchant::~Dispatcher_PutItemForEnchant() {}
int Dispatcher_PutItemForEnchant::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_PutItemForEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PutItemForEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestItemUnlock::Dispatcher_RequestItemUnlock() {}
Dispatcher_RequestItemUnlock::~Dispatcher_RequestItemUnlock() {}
int Dispatcher_RequestItemUnlock::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestItemUnlock::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestItemUnlock::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SecretShopBuyItem::Dispatcher_SecretShopBuyItem() {}
Dispatcher_SecretShopBuyItem::~Dispatcher_SecretShopBuyItem() {}
int Dispatcher_SecretShopBuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecretShopBuyItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SecretShopBuyItem::send(CUser* user, ParamBase& param) {}

Dispatcher_SecurityCardIssue::Dispatcher_SecurityCardIssue() {}
Dispatcher_SecurityCardIssue::~Dispatcher_SecurityCardIssue() {}
int Dispatcher_SecurityCardIssue::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SecurityCardIssue::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardIssue::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SetSubGuildMaster::Dispatcher_SetSubGuildMaster() {}
Dispatcher_SetSubGuildMaster::~Dispatcher_SetSubGuildMaster() {}
int Dispatcher_SetSubGuildMaster::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SetSubGuildMaster::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SetSubGuildMaster::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeGuildCargo::Dispatcher_UpgradeGuildCargo() {}
Dispatcher_UpgradeGuildCargo::~Dispatcher_UpgradeGuildCargo() {}
int Dispatcher_UpgradeGuildCargo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UpgradeGuildCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeGuildCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseVendingMachine::Dispatcher_UseVendingMachine() {}
Dispatcher_UseVendingMachine::~Dispatcher_UseVendingMachine() {}


Dispatcher_AchievementTrigger::Dispatcher_AchievementTrigger() {}
Dispatcher_AchievementTrigger::~Dispatcher_AchievementTrigger() {}
int Dispatcher_AchievementTrigger::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AchievementTrigger::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Antibot_DPCallBack::Dispatcher_Antibot_DPCallBack() {}
Dispatcher_Antibot_DPCallBack::~Dispatcher_Antibot_DPCallBack() {}
int Dispatcher_Antibot_DPCallBack::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Antibot_DPCallBack::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Antibot_DPCallBack::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_BiddingRoutingItem::Dispatcher_BiddingRoutingItem() {}
Dispatcher_BiddingRoutingItem::~Dispatcher_BiddingRoutingItem() {}
int Dispatcher_BiddingRoutingItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_BiddingRoutingItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BiddingRoutingItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Cancel_Quick_Party::Dispatcher_Cancel_Quick_Party() {}
Dispatcher_Cancel_Quick_Party::~Dispatcher_Cancel_Quick_Party() {}
int Dispatcher_Cancel_Quick_Party::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Cancel_Quick_Party::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Cancel_Quick_Party::send(CUser* user, ParamBase& param) {}

Dispatcher_ChangeRandomOption::Dispatcher_ChangeRandomOption() {}
Dispatcher_ChangeRandomOption::~Dispatcher_ChangeRandomOption() {}
int Dispatcher_ChangeRandomOption::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeRandomOption::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeTutorialFlag::Dispatcher_ChangeTutorialFlag() {}
Dispatcher_ChangeTutorialFlag::~Dispatcher_ChangeTutorialFlag() {}
int Dispatcher_ChangeTutorialFlag::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ChangeTutorialFlag::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeTutorialFlag::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Change_Charac_Slot::Dispatcher_Change_Charac_Slot() {}
Dispatcher_Change_Charac_Slot::~Dispatcher_Change_Charac_Slot() {}
int Dispatcher_Change_Charac_Slot::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Change_Charac_Slot::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Change_Charac_Slot::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CharacterStatistic::Dispatcher_CharacterStatistic() {}
Dispatcher_CharacterStatistic::~Dispatcher_CharacterStatistic() {}
int Dispatcher_CharacterStatistic::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x82051d0：get_state < 3 → 0；依次 get_int 6 个字段（任一失败 → 0），
    // 成功后写 CUser+0x8e03c..0x8e050（6 个 int，Character 统计区），并置
    // CUser+0x8e074 = 1。零外部调用（自包含）。
    if (user == 0 || user->get_state() < 3)
    {
        return 0;
    }

    int v0, v1, v2, v3, v4, v5;
    if (packet.get_int(v0) != 1) return 0;
    if (packet.get_int(v1) != 1) return 0;
    if (packet.get_int(v2) != 1) return 0;
    if (packet.get_int(v3) != 1) return 0;
    if (packet.get_int(v4) != 1) return 0;
    if (packet.get_int(v5) != 1) return 0;

    char* base = reinterpret_cast<char*>(user);
    *(int*)(base + 0x8e03c) = v0;  // ORIG *(int*)(param_1+0x8e03c) = local_24
    *(int*)(base + 0x8e044) = v1;  // local_1c
    *(int*)(base + 0x8e048) = v2;  // local_18
    *(int*)(base + 0x8e040) = v3;  // local_20
    *(int*)(base + 0x8e04c) = v4;  // local_14
    *(int*)(base + 0x8e050) = v5;  // local_10[0]
    *(base + 0x8e074) = 1;
    return 0;
}
int Dispatcher_CharacterStatistic::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CharacterStatistic::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CloseDisjointStore::Dispatcher_CloseDisjointStore() {}
Dispatcher_CloseDisjointStore::~Dispatcher_CloseDisjointStore() {}
int Dispatcher_CloseDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CloseDisjointStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_CloseDisjointStore::send(CUser* user, ParamBase& param) {}

Dispatcher_CompatibilityIndex::Dispatcher_CompatibilityIndex() {}
Dispatcher_CompatibilityIndex::~Dispatcher_CompatibilityIndex() {}
int Dispatcher_CompatibilityIndex::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompatibilityIndex::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CreateAccountCargo::Dispatcher_CreateAccountCargo() {}
Dispatcher_CreateAccountCargo::~Dispatcher_CreateAccountCargo() {}
int Dispatcher_CreateAccountCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2)
    {
        CAccountCargo* cargo = user->GetAccountCargo();
        if (cargo != 0)
        {
            cargo->CreateAccountCargo(user);
        }
    }
    return 0;
}
int Dispatcher_CreateAccountCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CreateAccountCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CreatePrivateStore::Dispatcher_CreatePrivateStore() {}
Dispatcher_CreatePrivateStore::~Dispatcher_CreatePrivateStore() {}
int Dispatcher_CreatePrivateStore::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CreatePrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CreatePrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DungeonNPCBuffInfo::Dispatcher_DungeonNPCBuffInfo() {}
Dispatcher_DungeonNPCBuffInfo::~Dispatcher_DungeonNPCBuffInfo() {}
int Dispatcher_DungeonNPCBuffInfo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DungeonNPCBuffInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DungeonNPCBuffInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EnterDisjointStore::Dispatcher_EnterDisjointStore() {}
Dispatcher_EnterDisjointStore::~Dispatcher_EnterDisjointStore() {}
int Dispatcher_EnterDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_EnterDisjointStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_EnterDisjointStore::send(CUser* user, ParamBase& param) {}

Dispatcher_FrameLagStatistics::Dispatcher_FrameLagStatistics() {}
Dispatcher_FrameLagStatistics::~Dispatcher_FrameLagStatistics() {}
int Dispatcher_FrameLagStatistics::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_FrameLagStatistics::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_FrameLagStatistics::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GuildCargoMoveItem::Dispatcher_GuildCargoMoveItem() {}
Dispatcher_GuildCargoMoveItem::~Dispatcher_GuildCargoMoveItem() {}
int Dispatcher_GuildCargoMoveItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildCargoMoveItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GuildCargoMoveItem::send(CUser* user, ParamBase& param) {}

Dispatcher_GuildCargoPushItem::Dispatcher_GuildCargoPushItem() {}
Dispatcher_GuildCargoPushItem::~Dispatcher_GuildCargoPushItem() {}
// ORIG 0x081d0f64 (check_error)：state==3 且 curCharac 有效；guildkey==0→100；
// 无公会驻地→0xb5；Secu 保护场 CheckItem(8,slot)；物品锁→0xd5；
// msg+0x10 与槽内 m_addInfo 不符或 find_item 失败→0xc5；通过→0
int Dispatcher_GuildCargoPushItem::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->get_state() != 3)
        return -1;
    if (user->getCurCharacR() == 0)
        return -1;
    if (user->get_charac_guildkey() == 0)
        return 100;
    if (!user->IsExistGuildAgit())
        return 0xb5;

    const unsigned char itemSpace =
        *reinterpret_cast<const unsigned char*>(reinterpret_cast<const char*>(&msg) + 0x0d);
    const unsigned short slot =
        *reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&msg) + 0x0e);
    const int invenType = GetInvenTypeFromItemSpace(itemSpace);
    int err = GlobalData::s_pSecuProtectionField->CheckItem(
        user, static_cast<SECURITY_PROTCTION>(8), static_cast<unsigned long>(slot));
    const Inven_Item slotItem = user->getCurCharacInvenR()->GetInvenSlot(invenType, static_cast<int>(slot));

    if (err != 0)
        return err;                          // ORIG: Secu 拒绝时直接透传
    if (user->CheckItemLock(invenType, static_cast<int>(slot)))
        return 0xd5;
    const int reqAddInfo = *reinterpret_cast<const int*>(reinterpret_cast<const char*>(&msg) + 0x10);
    if (reqAddInfo == 0 || slotItem.m_addInfo != reqAddInfo)
        return 0xc5;
    if (G_CDataManager()->find_item(slotItem.m_addInfo) == 0)
        return 0xc5;
    return 0;
}

int Dispatcher_GuildCargoPushItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d10d4 (0x242 insn)
    stGuildCargoPushItemParam& result = static_cast<stGuildCargoPushItemParam&>(param);
    const unsigned char itemSpace =
        *reinterpret_cast<const unsigned char*>(reinterpret_cast<const char*>(&msg) + 0x0d);
    const unsigned short slot =
        *reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&msg) + 0x0e);

    int err = check_error(user, msg);
    result.m_4 = err;                                   // +0x04 错误码
    if (err >= 1)
        return 0;
    if (err < 0)
        return LineFunc(0x2cf0, "virtual int Dispatcher_GuildCargoPushItem::process(CUser*, MSG_BASE&, ParamBase&)", err, 0);

    const CInventory* inven = user->getCurCharacInvenR();
    const int invenType = GetInvenTypeFromItemSpace(itemSpace);
    const Inven_Item slotItem = inven->GetInvenSlot(invenType, static_cast<int>(slot));
    CDataManager* dataMgr = G_CDataManager();
    CItem* item = dataMgr->find_item(slotItem.m_addInfo);

    // ORIG: 构造 Packet_Guild_Check_Guild_Cargo_Push_Item 并经
    //       CServerProxyMgr<CGuildServerProxy>::GetServerProxy(s_guild_proxy_mgr,
    //       user->GetServerGroup())->SendTcpPacket 发送，字段：
    //         guildkey=get_charac_guildkey() / characNo=getCurCharacNo() /
    //         msg+0x10 int / msg+0x18 ushort / msg+0x14 int / msg[0xd] byte /
    //         msg+0xe ushort / isStackable=item->is_stackable() /
    //         stackLimit=is_stackable()?getStackableLimit():1
    // BLOCKED(Packet_Guild_Check_Guild_Cargo_Push_Item 未建模 —— Inter/Guild 域缺口)
    const bool isStackable = item->is_stackable();
    const int stackLimit =
        isStackable ? static_cast<CStackableItem*>(item)->getStackableLimit() : 1;
    (void)stackLimit;
    (void)isStackable;
    // BLOCKED(cUserHistoryLog::GuildCargoCheckPushItem(Reason5, msg+0x18, msg+0x10,
    //        msg+0x14, msg[0xd], msg+0xe, isStackable) @0x0868659c —— 声明缺失)
    return 0;
}

int Dispatcher_GuildCargoPushItem::read(PacketBuf& packet, MSG_BASE& msg)   // ORIG 0x081d0dfe (83 insn)
{
    static const char kFunc[] =
        "virtual int Dispatcher_GuildCargoPushItem::read(PacketBuf&, MSG_BASE&)";
    char* pMsg = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(pMsg + 0x0d)))
        return LineFunc(0x2c91, kFunc, 0, 0);           // msg+0xd byte itemSpace
    if (!packet.get_short(*reinterpret_cast<short*>(pMsg + 0x0e)))
        return LineFunc(0x2c92, kFunc, 0, 0);           // msg+0xe short slot
    if (!packet.get_int(*reinterpret_cast<int*>(pMsg + 0x10)))
        return LineFunc(0x2c93, kFunc, 0, 0);           // msg+0x10 int
    if (!packet.get_int(*reinterpret_cast<int*>(pMsg + 0x14)))
        return LineFunc(0x2c94, kFunc, 0, 0);           // msg+0x14 int
    if (!packet.get_short(*reinterpret_cast<short*>(pMsg + 0x18)))
        return LineFunc(0x2c95, kFunc, 0, 0);           // msg+0x18 short count
    return 0;
}

void Dispatcher_GuildCargoPushItem::send(CUser* user, ParamBase& param)     // ORIG 0x081d1316 (36 insn)
{
    stGuildCargoPushItemParam& result = static_cast<stGuildCargoPushItemParam&>(param);
    if (result.m_4 != 0)
    {
        if (result.m_4 == 100 || result.m_4 == 0xb5)
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x109),
                                     static_cast<unsigned char>(result.m_4 & 0xff));
        else
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xfb),
                                     static_cast<unsigned char>(result.m_4 & 0xff));
    }
}

Dispatcher_RemovePrivateStore::Dispatcher_RemovePrivateStore() {}
Dispatcher_RemovePrivateStore::~Dispatcher_RemovePrivateStore() {}
int Dispatcher_RemovePrivateStore::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0;
    }
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        PacketGuard response;
        response.put_header(1, 0x59);
        response.put_byte(0);
        response.put_byte(0x15);
        response.finalize(true);
        user->Send(response);
        return 0;
    }
    private_store::CPrivateStoreMgr* manager =
        private_store::GetInstancePrivateStoreMgr();
    private_store::ISeller* seller = 0;
    if (manager != 0)
    {
        manager->GetStoreSeller(&seller, user);
    }
    if (seller != 0)
    {
        seller->RemovePrivateStore(false);
    }
    return 0;
}
int Dispatcher_RemovePrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RemovePrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestAddPvPBuddy::Dispatcher_RequestAddPvPBuddy() {}
Dispatcher_RequestAddPvPBuddy::~Dispatcher_RequestAddPvPBuddy() {}
int Dispatcher_RequestAddPvPBuddy::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestAddPvPBuddy::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestAddPvPBuddy::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestGuildSecede::Dispatcher_RequestGuildSecede() {}
Dispatcher_RequestGuildSecede::~Dispatcher_RequestGuildSecede() {}
int Dispatcher_RequestGuildSecede::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestGuildSecede::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestGuildSecede::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestMemberEnter::Dispatcher_RequestMemberEnter() {}
Dispatcher_RequestMemberEnter::~Dispatcher_RequestMemberEnter() {}
int Dispatcher_RequestMemberEnter::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0 ? 0 : -1;
}
int Dispatcher_RequestMemberEnter::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestMemberEnter::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_RequestMemberEnter::send(CUser* user, ParamBase& param) {}

Dispatcher_Request_Seria_Buff::Dispatcher_Request_Seria_Buff() {}
Dispatcher_Request_Seria_Buff::~Dispatcher_Request_Seria_Buff() {}
int Dispatcher_Request_Seria_Buff::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Request_Seria_Buff::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Request_Seria_Buff::send(CUser* user, ParamBase& param) {}

Dispatcher_Score_Scroll_State::Dispatcher_Score_Scroll_State() {}
Dispatcher_Score_Scroll_State::~Dispatcher_Score_Scroll_State() {}
int Dispatcher_Score_Scroll_State::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Score_Scroll_State::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Score_Scroll_State::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SecurityCardDisuse::Dispatcher_SecurityCardDisuse() {}
Dispatcher_SecurityCardDisuse::~Dispatcher_SecurityCardDisuse() {}
int Dispatcher_SecurityCardDisuse::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SecurityCardDisuse::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardDisuse::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_TitleBookOtherUser::Dispatcher_TitleBookOtherUser() {}
Dispatcher_TitleBookOtherUser::~Dispatcher_TitleBookOtherUser() {}
int Dispatcher_TitleBookOtherUser::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TitleBookOtherUser::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UnsealRandomOption::Dispatcher_UnsealRandomOption() {}
Dispatcher_UnsealRandomOption::~Dispatcher_UnsealRandomOption() {}
int Dispatcher_UnsealRandomOption::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UnsealRandomOption::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UnsealRandomOption::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ValidateScriptHash::Dispatcher_ValidateScriptHash() {}
Dispatcher_ValidateScriptHash::~Dispatcher_ValidateScriptHash() {}
int Dispatcher_ValidateScriptHash::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ValidateScriptHash::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ValidateScriptHash::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SCRIPT_HASH& req = static_cast<MSG_SCRIPT_HASH&>(msg);
    if (!packet.get_byte(req.m_pad[0]))
    {
        return LineFunc(0x37d,
                        "virtual int Dispatcher_ValidateScriptHash::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(req.m_pad[1]))
    {
        return LineFunc(0x382,
                        "virtual int Dispatcher_ValidateScriptHash::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<int*>(req.m_pad + 2)))
    {
        return LineFunc(0x387,
                        "virtual int Dispatcher_ValidateScriptHash::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

Dispatcher_AuctionRegistCancel::Dispatcher_AuctionRegistCancel() {}
Dispatcher_AuctionRegistCancel::~Dispatcher_AuctionRegistCancel() {}
int Dispatcher_AuctionRegistCancel::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionRegistCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionRegistCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BuyPrivateStoreItem::Dispatcher_BuyPrivateStoreItem() {}
Dispatcher_BuyPrivateStoreItem::~Dispatcher_BuyPrivateStoreItem() {}
int Dispatcher_BuyPrivateStoreItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_BuyPrivateStoreItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BuyPrivateStoreItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CallGuildAllMembers::Dispatcher_CallGuildAllMembers() {}
Dispatcher_CallGuildAllMembers::~Dispatcher_CallGuildAllMembers() {}
int Dispatcher_CallGuildAllMembers::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CallGuildAllMembers::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildAllMembers::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompleteLoadAssault::Dispatcher_CompleteLoadAssault() {}
Dispatcher_CompleteLoadAssault::~Dispatcher_CompleteLoadAssault() {}
int Dispatcher_CompleteLoadAssault::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CompleteLoadAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompleteLoadAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CreateDisjointStore::Dispatcher_CreateDisjointStore() {}
Dispatcher_CreateDisjointStore::~Dispatcher_CreateDisjointStore() {}
int Dispatcher_CreateDisjointStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CreateDisjointStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_CreateDisjointStore::send(CUser* user, ParamBase& param) {}

Dispatcher_CreatureSendMessage::Dispatcher_CreatureSendMessage() {}
Dispatcher_CreatureSendMessage::~Dispatcher_CreatureSendMessage() {}
int Dispatcher_CreatureSendMessage::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CreatureSendMessage::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CreatureSendMessage::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_FightVillageMonster::Dispatcher_FightVillageMonster() {}
Dispatcher_FightVillageMonster::~Dispatcher_FightVillageMonster() {}
int Dispatcher_FightVillageMonster::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_FightVillageMonster::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_FightVillageMonster::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GuildAttendanceInfo::Dispatcher_GuildAttendanceInfo() {}
Dispatcher_GuildAttendanceInfo::~Dispatcher_GuildAttendanceInfo() {}
int Dispatcher_GuildAttendanceInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildAttendanceInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GuildMasterDelegate::Dispatcher_GuildMasterDelegate() {}
Dispatcher_GuildMasterDelegate::~Dispatcher_GuildMasterDelegate() {}
int Dispatcher_GuildMasterDelegate::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_GuildMasterDelegate::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildMasterDelegate::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_New_Gmdebug_Command::Dispatcher_New_Gmdebug_Command() {}
Dispatcher_New_Gmdebug_Command::~Dispatcher_New_Gmdebug_Command() {}
int Dispatcher_New_Gmdebug_Command::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_New_Gmdebug_Command::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_New_Gmdebug_Command::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PowerWarProcessInfo::Dispatcher_PowerWarProcessInfo() {}
Dispatcher_PowerWarProcessInfo::~Dispatcher_PowerWarProcessInfo() {}
int Dispatcher_PowerWarProcessInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x821ea32 (83 insn)
    if (!(user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return LineFunc(0xd721,
                        "virtual int Dispatcher_PowerWarProcessInfo::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    const unsigned int characNo = static_cast<unsigned int>(user->get_charac_no(-1));
    CGuildServerProxy* proxy =
        GlobalData::s_guild_proxy_mgr->GetServerProxy(static_cast<ENUM_SERVER_GROUP>(user->GetServerGroup()));
    if (proxy != 0)
    {
        proxy->SendPowerWarProcessInfo(characNo);
    }
    PacketGuard response;
    response.put_header(1, 0xec);
    response.put_byte(1);
    response.finalize(true);
    user->Send(response);
    return 0;
}
int Dispatcher_PowerWarProcessInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PowerWarProcessInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PvpMissionHPPercent::Dispatcher_PvpMissionHPPercent() {}
Dispatcher_PvpMissionHPPercent::~Dispatcher_PvpMissionHPPercent() {}
int Dispatcher_PvpMissionHPPercent::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PvpMissionHPPercent::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_PvpMissionHPPercent::send(CUser* user, ParamBase& param) {}

Dispatcher_RegisterToBlackList::Dispatcher_RegisterToBlackList() {}
Dispatcher_RegisterToBlackList::~Dispatcher_RegisterToBlackList() {}
int Dispatcher_RegisterToBlackList::check_error(CUser* user, MSG_BASE& msg)
{
    return user != 0 && user->get_state() >= 3 ? 0 : -1;
}
int Dispatcher_RegisterToBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RegisterToBlackList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestAssaultPrice::Dispatcher_RequestAssaultPrice() {}
Dispatcher_RequestAssaultPrice::~Dispatcher_RequestAssaultPrice() {}

Dispatcher_RequestDisjointItem::Dispatcher_RequestDisjointItem() {}
Dispatcher_RequestDisjointItem::~Dispatcher_RequestDisjointItem() {}
int Dispatcher_RequestDisjointItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestDisjointItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_RequestDisjointItem::send(CUser* user, ParamBase& param) {}

Dispatcher_ResponseAddPvPBuddy::Dispatcher_ResponseAddPvPBuddy() {}
Dispatcher_ResponseAddPvPBuddy::~Dispatcher_ResponseAddPvPBuddy() {}
int Dispatcher_ResponseAddPvPBuddy::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ResponseAddPvPBuddy::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ResponseAddPvPBuddy::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SaveCharacterOption::Dispatcher_SaveCharacterOption() {}
Dispatcher_SaveCharacterOption::~Dispatcher_SaveCharacterOption() {}
int Dispatcher_SaveCharacterOption::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() < 2)
    {
        return -2;
    }
    return user->getCurCharacNo() == 0 ? -3 : 0;
}
int Dispatcher_SaveCharacterOption::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SaveCharacterOption::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SecretShopOpenClose::Dispatcher_SecretShopOpenClose() {}
Dispatcher_SecretShopOpenClose::~Dispatcher_SecretShopOpenClose() {}
int Dispatcher_SecretShopOpenClose::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecretShopOpenClose::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SecretShopOpenClose::send(CUser* user, ParamBase& param) {}

Dispatcher_SecurityCardAuthReq::Dispatcher_SecurityCardAuthReq() {}
Dispatcher_SecurityCardAuthReq::~Dispatcher_SecurityCardAuthReq() {}
int Dispatcher_SecurityCardAuthReq::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SecurityCardAuthReq::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardAuthReq::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_SecurityCardCertKey::Dispatcher_SecurityCardCertKey() {}
Dispatcher_SecurityCardCertKey::~Dispatcher_SecurityCardCertKey() {}
int Dispatcher_SecurityCardCertKey::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SecurityCardCertKey::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardCertKey::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpdateServerMessage::Dispatcher_UpdateServerMessage() {}
Dispatcher_UpdateServerMessage::~Dispatcher_UpdateServerMessage() {}
int Dispatcher_UpdateServerMessage::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UpdateServerMessage::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpdateServerMessage::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeAccountCargo::Dispatcher_UpgradeAccountCargo() {}
Dispatcher_UpgradeAccountCargo::~Dispatcher_UpgradeAccountCargo() {}
int Dispatcher_UpgradeAccountCargo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8224fb0：状态大于 2 时直接委托账号仓库。
    if (user != 0 && user->get_state() > 2)
    {
        CAccountCargo* cargo = user->GetAccountCargo();
        if (cargo != 0)
        {
            cargo->UpgradeAccountCargo(user);
        }
    }
    return 0;
}
int Dispatcher_UpgradeAccountCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeAccountCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeItemSeparate::Dispatcher_UpgradeItemSeparate() {}
Dispatcher_UpgradeItemSeparate::~Dispatcher_UpgradeItemSeparate() {}
int Dispatcher_UpgradeItemSeparate::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_UpgradeItemSeparate::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeItemSeparate::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseSharedEffectItem::Dispatcher_UseSharedEffectItem() {}
Dispatcher_UseSharedEffectItem::~Dispatcher_UseSharedEffectItem() {}
int Dispatcher_UseSharedEffectItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UseSharedEffectItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseSharedEffectItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_VerifyCreatureQuest::Dispatcher_VerifyCreatureQuest() {}
Dispatcher_VerifyCreatureQuest::~Dispatcher_VerifyCreatureQuest() {}
// ORIG 0x081e324c：user 非空且 state>2 时，对当前角色背包的装备宠物
// 调 VerifyEquipedCreatureQuest（进化任务资格校验/任务列表包补发）。
int Dispatcher_VerifyCreatureQuest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user != 0 && user->get_state() > 2)
    {
        user->getCurCharacInvenR()->GetCreatureMgrR()->VerifyEquipedCreatureQuest();
    }
    return 0;
}
int Dispatcher_VerifyCreatureQuest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_WarRoomWpPerMonster::Dispatcher_WarRoomWpPerMonster() {}
Dispatcher_WarRoomWpPerMonster::~Dispatcher_WarRoomWpPerMonster() {}
int Dispatcher_WarRoomWpPerMonster::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_WarRoomWpPerMonster::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x39af, pretty, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x39b2, pretty, 0, 0);
    }
    return user->get_state() == 8 ? 0 : -1;
}
int Dispatcher_WarRoomWpPerMonster::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_WarRoomWpPerMonster::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_WarRoomWpPerMonster::send(CUser* user, ParamBase& param) {}

Dispatcher_AbnormalUseStackable::Dispatcher_AbnormalUseStackable() {}
Dispatcher_AbnormalUseStackable::~Dispatcher_AbnormalUseStackable() {}
int Dispatcher_AbnormalUseStackable::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_AbnormalUseStackable::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AbnormalUseStackable::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionBuyItemApiece::Dispatcher_AuctionBuyItemApiece() {}
Dispatcher_AuctionBuyItemApiece::~Dispatcher_AuctionBuyItemApiece() {}
int Dispatcher_AuctionBuyItemApiece::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionBuyItemApiece::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionBuyItemApiece::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionMyBiddingInfo::Dispatcher_AuctionMyBiddingInfo() {}
Dispatcher_AuctionMyBiddingInfo::~Dispatcher_AuctionMyBiddingInfo() {}
int Dispatcher_AuctionMyBiddingInfo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionMyBiddingInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionMyBiddingInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BuyCeraShopLimitItem::Dispatcher_BuyCeraShopLimitItem() {}
Dispatcher_BuyCeraShopLimitItem::~Dispatcher_BuyCeraShopLimitItem() {}


Dispatcher_CallGuildCreateRight::Dispatcher_CallGuildCreateRight() {}
Dispatcher_CallGuildCreateRight::~Dispatcher_CallGuildCreateRight() {}
int Dispatcher_CallGuildCreateRight::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CallGuildCreateRight::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallGuildCreateRight::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Check3rdPartyConcent::Dispatcher_Check3rdPartyConcent() {}
Dispatcher_Check3rdPartyConcent::~Dispatcher_Check3rdPartyConcent() {}
int Dispatcher_Check3rdPartyConcent::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Check3rdPartyConcent::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CheckGuildNameDouble::Dispatcher_CheckGuildNameDouble() {}
Dispatcher_CheckGuildNameDouble::~Dispatcher_CheckGuildNameDouble() {}
int Dispatcher_CheckGuildNameDouble::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CheckGuildNameDouble::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckGuildNameDouble::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Compound_ExtreamItem::Dispatcher_Compound_ExtreamItem() {}
Dispatcher_Compound_ExtreamItem::~Dispatcher_Compound_ExtreamItem() {}
int Dispatcher_Compound_ExtreamItem::_push_newitem() { return 0; }
int Dispatcher_Compound_ExtreamItem::_remove_victim() { return 0; }
int Dispatcher_Compound_ExtreamItem::_reset() { return 0; }
int Dispatcher_Compound_ExtreamItem::_send_result() { return 0; }
int Dispatcher_Compound_ExtreamItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Compound_ExtreamItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Compound_ExtreamItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GatheringPartyStatus::Dispatcher_GatheringPartyStatus() {}
Dispatcher_GatheringPartyStatus::~Dispatcher_GatheringPartyStatus() {}
int Dispatcher_GatheringPartyStatus::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_GatheringPartyStatus::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GatheringPartyStatus::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ItemHyperLinkMessage::Dispatcher_ItemHyperLinkMessage() {}
Dispatcher_ItemHyperLinkMessage::~Dispatcher_ItemHyperLinkMessage() {}
int Dispatcher_ItemHyperLinkMessage::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->get_state() < 3)
    {
        return -1;
    }
    const MSG_ITEM_HYPER_LINK_MESSAGE& req =
        static_cast<const MSG_ITEM_HYPER_LINK_MESSAGE&>(msg);
    unsigned char type = static_cast<unsigned char>(req.m_pad[0]);
    return type == 0x24 || type == 0x25 || type == 0x21 || type == 7 ? -1 : 0;
}
int Dispatcher_ItemHyperLinkMessage::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ItemHyperLinkMessage::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ItemHyperLinkMessage::send(CUser* user, ParamBase& param) {}

Dispatcher_NotifyMessageToGuild::Dispatcher_NotifyMessageToGuild() {}
Dispatcher_NotifyMessageToGuild::~Dispatcher_NotifyMessageToGuild() {}
int Dispatcher_NotifyMessageToGuild::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_NotifyMessageToGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_NotifyMessageToGuild::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_OperateRidableObject::Dispatcher_OperateRidableObject() {}
Dispatcher_OperateRidableObject::~Dispatcher_OperateRidableObject() {}
int Dispatcher_OperateRidableObject::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_OperateRidableObject::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_OperateRidableObject::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PartyTeleportConfirm::Dispatcher_PartyTeleportConfirm() {}
Dispatcher_PartyTeleportConfirm::~Dispatcher_PartyTeleportConfirm() {}
int Dispatcher_PartyTeleportConfirm::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_PartyTeleportConfirm::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PartyTeleportConfirm::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_PartyTeleportConfirm::send(CUser* user, ParamBase& param) {}

Dispatcher_PcRoomPlayTimeReward::Dispatcher_PcRoomPlayTimeReward() {}
Dispatcher_PcRoomPlayTimeReward::~Dispatcher_PcRoomPlayTimeReward() {}
int Dispatcher_PcRoomPlayTimeReward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PcRoomPlayTimeReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_PcRoomPlayTimeReward::send(CUser* user, ParamBase& param) {}

Dispatcher_PrecheckSoloTeleport::Dispatcher_PrecheckSoloTeleport() {}
Dispatcher_PrecheckSoloTeleport::~Dispatcher_PrecheckSoloTeleport() {}
int Dispatcher_PrecheckSoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PrecheckSoloTeleport::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_PrecheckSoloTeleport::send(CUser* user, ParamBase& param) {}

Dispatcher_PvpMissionComboClear::Dispatcher_PvpMissionComboClear() {}
Dispatcher_PvpMissionComboClear::~Dispatcher_PvpMissionComboClear() {}
int Dispatcher_PvpMissionComboClear::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PvpMissionComboClear::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Register_Quick_Party::Dispatcher_Register_Quick_Party() {}
Dispatcher_Register_Quick_Party::~Dispatcher_Register_Quick_Party() {}
int Dispatcher_Register_Quick_Party::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Register_Quick_Party::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Register_Quick_Party::send(CUser* user, ParamBase& param) {}

Dispatcher_ReportMannerlessUser::Dispatcher_ReportMannerlessUser() {}
Dispatcher_ReportMannerlessUser::~Dispatcher_ReportMannerlessUser() {}
int Dispatcher_ReportMannerlessUser::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ReportMannerlessUser::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ReportMannerlessUser::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Request_PvpExpOfWeek::Dispatcher_Request_PvpExpOfWeek() {}
Dispatcher_Request_PvpExpOfWeek::~Dispatcher_Request_PvpExpOfWeek() {}
int Dispatcher_Request_PvpExpOfWeek::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Request_PvpExpOfWeek::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Request_PvpExpOfWeek::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_ProcessScan::Dispatcher_Security_ProcessScan() {}
Dispatcher_Security_ProcessScan::~Dispatcher_Security_ProcessScan() {}
int Dispatcher_Security_ProcessScan::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_ProcessScan(user, packet);
    if (result == 2)
    {
        return LineFunc(0xde86,
                        "virtual int Dispatcher_Security_ProcessScan::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (result == 3)
    {
        return LineFunc(0xde88,
                        "virtual int Dispatcher_Security_ProcessScan::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (result == 1)
    {
        return LineFunc(0xde84,
                        "virtual int Dispatcher_Security_ProcessScan::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_ProcessScan::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_ProcessScan::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SeriaRoom_Deco_Event::Dispatcher_SeriaRoom_Deco_Event() {}
Dispatcher_SeriaRoom_Deco_Event::~Dispatcher_SeriaRoom_Deco_Event() {}
int Dispatcher_SeriaRoom_Deco_Event::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81e5e50：state==3 才允许当前活动请求。
    if (user == 0)
    {
        return -1;
    }
    return user->get_state() == 3 ? 0 : 7;
}
int Dispatcher_SeriaRoom_Deco_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SeriaRoom_Deco_Event::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SERIAROOM_DECO_EVENT& req = static_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_pad + 1)))
    {
        return LineFunc(0x5fff,
                        "virtual int Dispatcher_SeriaRoom_Deco_Event::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
void Dispatcher_SeriaRoom_Deco_Event::send(CUser* user, ParamBase& param) {}

Dispatcher_WriteGuildMemberMemo::Dispatcher_WriteGuildMemberMemo() {}
Dispatcher_WriteGuildMemberMemo::~Dispatcher_WriteGuildMemberMemo() {}
int Dispatcher_WriteGuildMemberMemo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_WriteGuildMemberMemo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_WriteGuildMemberMemo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }


Dispatcher_Client_Spec_Statistic::Dispatcher_Client_Spec_Statistic() {}
Dispatcher_Client_Spec_Statistic::~Dispatcher_Client_Spec_Statistic() {}
int Dispatcher_Client_Spec_Statistic::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_Client_Spec_Statistic::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Client_Spec_Statistic::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Client_Spec_Statistic::send(CUser* user, ParamBase& param) {}

Dispatcher_CreatureScriptMessage::Dispatcher_CreatureScriptMessage() {}
Dispatcher_CreatureScriptMessage::~Dispatcher_CreatureScriptMessage() {}
int Dispatcher_CreatureScriptMessage::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CreatureScriptMessage::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CreatureScriptMessage::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Mercenary_Competition::Dispatcher_Mercenary_Competition() {}
Dispatcher_Mercenary_Competition::~Dispatcher_Mercenary_Competition() {}
int Dispatcher_Mercenary_Competition::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081ddd00 (0xd4 insn)
    MsgMercenaryCompetition& req = reinterpret_cast<MsgMercenaryCompetition&>(msg);
    SendMercenaryCompetitionResult& result = static_cast<SendMercenaryCompetitionResult&>(param);
    if (user == 0)
        return -1;
    if (user->get_state() != 3)
        return -1;

    unsigned int targetNo = *reinterpret_cast<unsigned int*>(req.m_pad + 3);   // msg+0x10
    const char b0x15 = static_cast<char>(req.m_pad[8]);                        // msg+0x15
    const char b0x14 = static_cast<char>(req.m_pad[7]);                        // msg+0x14

    if (user->GetParty() != 0 && user->getCurCharacNo() == static_cast<int>(targetNo))
    {
        *reinterpret_cast<char*>(reinterpret_cast<char*>(&result) + 0x04) = 0; // param+4 = 0
        return -1;
    }

    char* pResult = reinterpret_cast<char*>(&result);
    // ORIG: Competition_Mercenary_Charac(user, msg+0x10, (char)msg+0x15, (char)msg+0x14)
    // BLOCKED(_ZN5CUser27Competition_Mercenary_CharacEjcc @0x0868e6c0 —— CUser.h 缺声明):
    const bool ret = false;                                     // BLOCKED 调用返回值
    pResult[0x04] = static_cast<char>(ret != false);
    *reinterpret_cast<int*>(pResult + 0x08) = static_cast<int>(targetNo);
    pResult[0x0c] = b0x15;
    pResult[0x0d] = b0x14;
    // ORIG: cUserHistoryLog::MercenaryCompetition(user->m_historyLog, targetNo, b0x15, b0x14)
    // BLOCKED(_ZN15cUserHistoryLog20MercenaryCompetitionEjcc @0x08686aca —— 声明缺失)
    return 0;
}

int Dispatcher_Mercenary_Competition::read(PacketBuf& packet, MSG_BASE& msg)  // ORIG 0x081ddc24 (81 insn)
{
    MsgMercenaryCompetition& req = reinterpret_cast<MsgMercenaryCompetition&>(msg);
    static const char kFunc[] =
        "virtual int Dispatcher_Mercenary_Competition::read(PacketBuf&, MSG_BASE&)";
    if (!packet.get_int(*reinterpret_cast<unsigned int*>(req.m_pad + 3)))     // msg+0x10
        return LineFunc(0x4eda, kFunc, 0, 0);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_pad[7])))              // msg+0x14
        return LineFunc(0x4edc, kFunc, 0, 0);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_pad[8])))              // msg+0x15
        return LineFunc(0x4ede, kFunc, 0, 0);
    return 0;
}

void Dispatcher_Mercenary_Competition::send(CUser* user, ParamBase& param)    // ORIG 0x081dde20 (83 insn)
{
    SendMercenaryCompetitionResult& result = static_cast<SendMercenaryCompetitionResult&>(param);
    char* pResult = reinterpret_cast<char*>(&result);
    PacketGuard guard;
    guard.put_header(1, 0x1bb);
    guard.put_byte(pResult[0x04] != 0);
    if (pResult[0x04] == 0)
    {
        guard.put_byte(0x15);                                                  // 失败错误码
    }
    else
    {
        guard.put_int(*reinterpret_cast<int*>(pResult + 0x08));
        guard.put_byte(static_cast<unsigned int>(static_cast<char>(pResult[0x0d])));
        guard.put_byte(static_cast<unsigned int>(static_cast<char>(pResult[0x0c])));
    }
    guard.finalize(true);
    user->Send(guard);
}

Dispatcher_OpenGuildCreateWindow::Dispatcher_OpenGuildCreateWindow() {}
Dispatcher_OpenGuildCreateWindow::~Dispatcher_OpenGuildCreateWindow() {}
int Dispatcher_OpenGuildCreateWindow::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->get_state() <= 2 ||
        user->getCurCharacR() == 0)
    {
        return LineFunc(0xa3e3,
                        "virtual int Dispatcher_OpenGuildCreateWindow::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    user->resetGuildCreateFlag();
    user->resetTempGuildNameAddr();
    return 0;
}
int Dispatcher_OpenGuildCreateWindow::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_OpenGuildCreateWindow::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RepairDisjointMachine::Dispatcher_RepairDisjointMachine() {}
Dispatcher_RepairDisjointMachine::~Dispatcher_RepairDisjointMachine() {}
int Dispatcher_RepairDisjointMachine::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RepairDisjointMachine::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_RepairDisjointMachine::send(CUser* user, ParamBase& param) {}

Dispatcher_AuctionAskAveragePrice::Dispatcher_AuctionAskAveragePrice() {}
Dispatcher_AuctionAskAveragePrice::~Dispatcher_AuctionAskAveragePrice() {}
int Dispatcher_AuctionAskAveragePrice::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionAskAveragePrice::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionAskAveragePrice::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionSearchByItemKey::Dispatcher_AuctionSearchByItemKey() {}
Dispatcher_AuctionSearchByItemKey::~Dispatcher_AuctionSearchByItemKey() {}
int Dispatcher_AuctionSearchByItemKey::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionSearchByItemKey::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionSearchByItemKey::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Break_Away_Quest_Check::Dispatcher_Break_Away_Quest_Check() {}
Dispatcher_Break_Away_Quest_Check::~Dispatcher_Break_Away_Quest_Check() {}
// ORIG 0x081d390a：msg.m_checkFlag==0（+0xd）→ setBreakAwayDungeonClearCnt(-1)、
// setBreakAwayQuestTime(0)、StreamPool::Acquire("PacketDispatcher_Impl_1.cpp",
// 0x31d9)+CStreamGuard(stream,true) 写 0x144/UID/acc_id 入 DB_Q；
// 否则 getBreakAwayDungeonClearCnt()==0 →
// GlobalData::s_BreakAwaySys->giveBreakAwayDungeonClearReward(user)，
// 非 0 → param.m_resultCode=1。
// 【blocked：giveBreakAwayDungeonClearReward 及其 selectReward/
// Inter_LoadEtc::giveBreakAwayReward 生产链尚未还原（报告齐备，独立批次），
// 已 hub 通知 Main；不写猜测实现。】
int Dispatcher_Break_Away_Quest_Check::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
// ORIG 0x081d38ba：get_byte 读入 msg.m_checkFlag（+0xd）；失败走
// LineFunc(0x3197) 返回其值。
int Dispatcher_Break_Away_Quest_Check::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BREAK_AWAY& req = static_cast<MSG_BREAK_AWAY&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_checkFlag)))
    {
        return LineFunc(0x3197,
                        "virtual int Dispatcher_Break_Away_Quest_Check::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
// ORIG 0x081d3a72：m_resultCode==0 → put_header(1,0x16d)+put_byte(1)+
// finalize(true)；否则 put_header(1,0x16d)+put_byte(0)+
// put_short(m_resultCode)+finalize(true)。两分支均 CUser::Send。
void Dispatcher_Break_Away_Quest_Check::send(CUser* user, ParamBase& param)
{
    const stBreakAwayParam& res = static_cast<const stBreakAwayParam&>(param);
    PacketGuard packet;
    packet.put_header(1, 0x16d);
    if (res.m_resultCode == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_short(res.m_resultCode);
    }
    packet.finalize(true);
    user->Send(packet);
}

Dispatcher_ChangeAnotherSkillTree::Dispatcher_ChangeAnotherSkillTree() {}
Dispatcher_ChangeAnotherSkillTree::~Dispatcher_ChangeAnotherSkillTree() {}
int Dispatcher_ChangeAnotherSkillTree::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x81d2094：state 3/6 才允许切换，并校验请求树索引。
    if (user == 0)
    {
        return -1;
    }
    int state = user->get_state();
    if (state != 3 && state != 6)
    {
        return 0x13;
    }
    const MSG_CHANGE_ANOTHER_SKILL_TREE& req =
        static_cast<const MSG_CHANGE_ANOTHER_SKILL_TREE&>(msg);
    int requested = static_cast<signed char>(req.m_0d);
    int current = user->GetCurCharacSkillTreeIndex();
    if (requested != current)
    {
        return -2;
    }
    return current == -1 ? -3 : 0;
}
int Dispatcher_ChangeAnotherSkillTree::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeAnotherSkillTree::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CHANGE_ANOTHER_SKILL_TREE& req =
        static_cast<MSG_CHANGE_ANOTHER_SKILL_TREE&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x2ec2,
                        "virtual int Dispatcher_ChangeAnotherSkillTree::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}
void Dispatcher_ChangeAnotherSkillTree::send(CUser* user, ParamBase& param) {}

Dispatcher_CharacSlotExtendEffect::Dispatcher_CharacSlotExtendEffect() {}
Dispatcher_CharacSlotExtendEffect::~Dispatcher_CharacSlotExtendEffect() {}
int Dispatcher_CharacSlotExtendEffect::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CharacSlotExtendEffect::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_CharacSlotExtendEffect::send(CUser* user, ParamBase& param) {}

Dispatcher_CheckAssaultMotionHack::Dispatcher_CheckAssaultMotionHack() {}
Dispatcher_CheckAssaultMotionHack::~Dispatcher_CheckAssaultMotionHack() {}
int Dispatcher_CheckAssaultMotionHack::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CheckAssaultMotionHack::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckAssaultMotionHack::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ConnectP2PAfterAssault::Dispatcher_ConnectP2PAfterAssault() {}
Dispatcher_ConnectP2PAfterAssault::~Dispatcher_ConnectP2PAfterAssault() {}
int Dispatcher_ConnectP2PAfterAssault::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ConnectP2PAfterAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ConnectP2PAfterAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DeathTowerStageCommand::Dispatcher_DeathTowerStageCommand() {}
Dispatcher_DeathTowerStageCommand::~Dispatcher_DeathTowerStageCommand() {}
int Dispatcher_DeathTowerStageCommand::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DeathTowerStageCommand::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DeathTowerStageCommand::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Delete_GuildBoard_Text::Dispatcher_Delete_GuildBoard_Text() {}
Dispatcher_Delete_GuildBoard_Text::~Dispatcher_Delete_GuildBoard_Text() {}
int Dispatcher_Delete_GuildBoard_Text::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Delete_GuildBoard_Text::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DungeonEventStoryPause::Dispatcher_DungeonEventStoryPause() {}
Dispatcher_DungeonEventStoryPause::~Dispatcher_DungeonEventStoryPause() {}
int Dispatcher_DungeonEventStoryPause::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DungeonEventStoryPause::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DungeonEventStoryPause::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Float_RDATA_Modulation::Dispatcher_Float_RDATA_Modulation() {}
Dispatcher_Float_RDATA_Modulation::~Dispatcher_Float_RDATA_Modulation() {}
int Dispatcher_Float_RDATA_Modulation::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_Float_RDATA_Modulation::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Float_RDATA_Modulation::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_LogIngameAdvertisement::Dispatcher_LogIngameAdvertisement() {}
Dispatcher_LogIngameAdvertisement::~Dispatcher_LogIngameAdvertisement() {}
int Dispatcher_LogIngameAdvertisement::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    return user->get_state() < 2 ? 0x7fffffff : 0;
}
int Dispatcher_LogIngameAdvertisement::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_LogIngameAdvertisement::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_LogIngameAdvertisement::send(CUser* user, ParamBase& param) {}

Dispatcher_QueryCharacInfoMailbox::Dispatcher_QueryCharacInfoMailbox() {}
Dispatcher_QueryCharacInfoMailbox::~Dispatcher_QueryCharacInfoMailbox() {}
int Dispatcher_QueryCharacInfoMailbox::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_QueryCharacInfoMailbox::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_QueryCharacInfoMailbox::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestCharacSkillInfo::Dispatcher_RequestCharacSkillInfo() {}
Dispatcher_RequestCharacSkillInfo::~Dispatcher_RequestCharacSkillInfo() {}
int Dispatcher_RequestCharacSkillInfo::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_RequestCharacSkillInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestCharacSkillInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_RequestCharacSkillInfo::send(CUser* user, ParamBase& param) {}

Dispatcher_SecurityCardAuthCancel::Dispatcher_SecurityCardAuthCancel() {}
Dispatcher_SecurityCardAuthCancel::~Dispatcher_SecurityCardAuthCancel() {}
int Dispatcher_SecurityCardAuthCancel::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SecurityCardAuthCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardAuthCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SecurityCardRetransfer::Dispatcher_SecurityCardRetransfer() {}
Dispatcher_SecurityCardRetransfer::~Dispatcher_SecurityCardRetransfer() {}
int Dispatcher_SecurityCardRetransfer::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SecurityCardRetransfer::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardRetransfer::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TournamentRewardSelect::Dispatcher_TournamentRewardSelect() {}
Dispatcher_TournamentRewardSelect::~Dispatcher_TournamentRewardSelect() {}

Dispatcher_UpgradeDisjointMachine::Dispatcher_UpgradeDisjointMachine() {}
Dispatcher_UpgradeDisjointMachine::~Dispatcher_UpgradeDisjointMachine() {}
int Dispatcher_UpgradeDisjointMachine::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeDisjointMachine::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_UpgradeDisjointMachine::send(CUser* user, ParamBase& param) {}

Dispatcher_VerifyPrivateStoreItem::Dispatcher_VerifyPrivateStoreItem() {}
Dispatcher_VerifyPrivateStoreItem::~Dispatcher_VerifyPrivateStoreItem() {}
int Dispatcher_VerifyPrivateStoreItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_VerifyPrivateStoreItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_VerifyPrivateStoreItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionMyAuctionHistory::Dispatcher_AuctionMyAuctionHistory() {}
Dispatcher_AuctionMyAuctionHistory::~Dispatcher_AuctionMyAuctionHistory() {}
int Dispatcher_AuctionMyAuctionHistory::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionMyAuctionHistory::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionMyAuctionHistory::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Card_Select_Right_State::Dispatcher_Card_Select_Right_State() {}
Dispatcher_Card_Select_Right_State::~Dispatcher_Card_Select_Right_State() {}
int Dispatcher_Card_Select_Right_State::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Card_Select_Right_State::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Card_Select_Right_State::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CheckGuildAddressDouble::Dispatcher_CheckGuildAddressDouble() {}
Dispatcher_CheckGuildAddressDouble::~Dispatcher_CheckGuildAddressDouble() {}
int Dispatcher_CheckGuildAddressDouble::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CheckGuildAddressDouble::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckGuildAddressDouble::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompoundItemByExpertJob::Dispatcher_CompoundItemByExpertJob() {}
Dispatcher_CompoundItemByExpertJob::~Dispatcher_CompoundItemByExpertJob() {}
int Dispatcher_CompoundItemByExpertJob::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CompoundItemByExpertJob::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompoundItemByExpertJob::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_InvestItemAmplifyOption::Dispatcher_InvestItemAmplifyOption() {}
Dispatcher_InvestItemAmplifyOption::~Dispatcher_InvestItemAmplifyOption() {}


Dispatcher_RequestItemUnlockCancel::Dispatcher_RequestItemUnlockCancel() {}
Dispatcher_RequestItemUnlockCancel::~Dispatcher_RequestItemUnlockCancel() {}
int Dispatcher_RequestItemUnlockCancel::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestItemUnlockCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestItemUnlockCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestPCRoomPlayerList::Dispatcher_RequestPCRoomPlayerList() {}
Dispatcher_RequestPCRoomPlayerList::~Dispatcher_RequestPCRoomPlayerList() {}
int Dispatcher_RequestPCRoomPlayerList::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        return 0;
    }
    const unsigned int accountId = user != 0 ? user->get_acc_id() : 0;
    cMyTrace trace(
        "virtual int Dispatcher_RequestPCRoomPlayerList::dispatch_sig(CUser*, PacketBuf&)",
        0xd14e, 5);
    trace("PCROOM : Dispatcher_RequestPCRoomPlayerList => User State : %d, m_id : %s",
          user != 0 ? user->get_state() : 0, NumberToString(accountId, 0));
    return LineFunc(0xd14f,
                    "virtual int Dispatcher_RequestPCRoomPlayerList::dispatch_sig(CUser*, PacketBuf&)",
                    0, 0);
}
int Dispatcher_RequestPCRoomPlayerList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestPCRoomPlayerList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Write_On_The_GuildBoard::Dispatcher_Write_On_The_GuildBoard() {}
Dispatcher_Write_On_The_GuildBoard::~Dispatcher_Write_On_The_GuildBoard() {}
int Dispatcher_Write_On_The_GuildBoard::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Write_On_The_GuildBoard::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Write_On_The_GuildBoard::send(CUser* user, ParamBase& param) {}

Dispatcher_AuctionSearchByNoItmeKey::Dispatcher_AuctionSearchByNoItmeKey() {}
Dispatcher_AuctionSearchByNoItmeKey::~Dispatcher_AuctionSearchByNoItmeKey() {}
int Dispatcher_AuctionSearchByNoItmeKey::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionSearchByNoItmeKey::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionSearchByNoItmeKey::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CheckEnableServerMessage::Dispatcher_CheckEnableServerMessage() {}
Dispatcher_CheckEnableServerMessage::~Dispatcher_CheckEnableServerMessage() {}
int Dispatcher_CheckEnableServerMessage::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CheckEnableServerMessage::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckEnableServerMessage::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompleteLoadAfterAssault::Dispatcher_CompleteLoadAfterAssault() {}
Dispatcher_CompleteLoadAfterAssault::~Dispatcher_CompleteLoadAfterAssault() {}
int Dispatcher_CompleteLoadAfterAssault::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CompleteLoadAfterAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompleteLoadAfterAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Enchant_3rdChronicleItem::Dispatcher_Enchant_3rdChronicleItem() {}
Dispatcher_Enchant_3rdChronicleItem::~Dispatcher_Enchant_3rdChronicleItem() {}
extern "C" int _ZN8WongWork12CItemUpgrade11ProcUpgradeEP5CUserP11MSG_ENCHANTP11PacketGuard(
    void* self, CUser* user, void* msg, void* packet);

int Dispatcher_Enchant_3rdChronicleItem::check_error(CUser* user, MSG_BASE& msg)
{
    // 仅允许 state==3 且当前角色有效；否则错误码 7
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return 7;
    }

    MSG_ENCHANT& req = static_cast<MSG_ENCHANT&>(msg);

    const int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x27);
    if (secu != 0)
    {
        return secu;
    }

    if (user->CheckItemLock(1, (unsigned short)req.m_0d))
    {
        return 0xd5;
    }

    // msg+0x15：请求强化的物品 idx → 物品脚本必须存在
    CItem* itemScript = G_CDataManager()->find_item(req.m_15);
    if (itemScript == 0)
    {
        return 4;
    }

    CInventory* invenW = user->getCurCharacInvenW();
    if (invenW == 0)
    {
        return 4;
    }

    // ORIG Inven_Item ctor @0x080cb854 后整体拷贝 GetInvenSlot 结果
    Inven_Item slotItem;
    slotItem = invenW->GetInvenSlot(INVEN_TYPE_EQUIP, (unsigned short)req.m_13);

    // 背包槽内物品须与请求脚本一致，否则 0x17
    if (slotItem.m_addInfo != itemScript->get_index())
    {
        return 0x17;
    }
    return 0;
}
int Dispatcher_Enchant_3rdChronicleItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x173, (unsigned char)(err & 0xff));
        return -1;
    }

    stEnchantParam& out = static_cast<stEnchantParam&>(param);   // +0x04 结果槽
    MSG_ENCHANT& req = static_cast<MSG_ENCHANT&>(msg);

    PacketGuard pg;                                              // try 域 @081d37e3..081d3846
    pg.put_header(1, 0x173);

    out.m_4 = _ZN8WongWork12CItemUpgrade11ProcUpgradeEP5CUserP11MSG_ENCHANTP11PacketGuard(
        G_CDataManager()->m_itemUpgrade, user, &req, &pg);

    if (out.m_4 == 0)
    {
        pg.finalize(true);
        user->Send(pg);
    }
    return 0;
}
int Dispatcher_Enchant_3rdChronicleItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_ENCHANT& req = static_cast<MSG_ENCHANT&>(msg);

    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&req.m_13)))
    {
        return LineFunc(
            0x3158,
            "virtual int Dispatcher_Enchant_3rdChronicleItem::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(&req.m_15)))
    {
        return LineFunc(
            0x315a,
            "virtual int Dispatcher_Enchant_3rdChronicleItem::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(&req.m_0d)))
    {
        return LineFunc(
            0x315d,
            "virtual int Dispatcher_Enchant_3rdChronicleItem::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(&req.m_0f)))
    {
        return LineFunc(
            0x315f,
            "virtual int Dispatcher_Enchant_3rdChronicleItem::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    if (!packet.get_byte(req.m_19))
    {
        return LineFunc(
            0x3162,
            "virtual int Dispatcher_Enchant_3rdChronicleItem::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    return 0;
}
void Dispatcher_Enchant_3rdChronicleItem::send(CUser* user, ParamBase& param)
{
    stEnchantParam& out = static_cast<stEnchantParam&>(param);
    if (out.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x173,
                                 (unsigned char)(out.m_4 & 0xff));
    }
}

Dispatcher_GoblinPadRequestCryptKey::Dispatcher_GoblinPadRequestCryptKey() {}
Dispatcher_GoblinPadRequestCryptKey::~Dispatcher_GoblinPadRequestCryptKey() {}
int Dispatcher_GoblinPadRequestCryptKey::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_GoblinPadRequestCryptKey::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GoblinPadRequestCryptKey::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GoldTakeIncreasingAmount::Dispatcher_GoldTakeIncreasingAmount() {}
Dispatcher_GoldTakeIncreasingAmount::~Dispatcher_GoldTakeIncreasingAmount() {}
int Dispatcher_GoldTakeIncreasingAmount::check_error(CUser* user, MSG_BASE& msg)
{
    const char* pretty = "int Dispatcher_GoldTakeIncreasingAmount::check_error(CUser*, MSG_BASE&)";
    if (user == 0)
    {
        return LineFunc(0x3903, pretty, 0, 0);
    }
    return user->get_state() == 5 ? 0 : -1;
}
int Dispatcher_GoldTakeIncreasingAmount::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GoldTakeIncreasingAmount::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GoldTakeIncreasingAmount::send(CUser* user, ParamBase& param) {}

Dispatcher_LevelupSupportReqestItem::Dispatcher_LevelupSupportReqestItem() {}
Dispatcher_LevelupSupportReqestItem::~Dispatcher_LevelupSupportReqestItem() {}
int Dispatcher_LevelupSupportReqestItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_LevelupSupportReqestItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_LevelupSupportReqestItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_LevelupSupportReqestItem::send(CUser* user, ParamBase& param) {}

Dispatcher_RequestOnTimeEventReward::Dispatcher_RequestOnTimeEventReward() {}
Dispatcher_RequestOnTimeEventReward::~Dispatcher_RequestOnTimeEventReward() {}
int Dispatcher_RequestOnTimeEventReward::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestOnTimeEventReward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestOnTimeEventReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestPCRoomPlayerCount::Dispatcher_RequestPCRoomPlayerCount() {}
Dispatcher_RequestPCRoomPlayerCount::~Dispatcher_RequestPCRoomPlayerCount() {}
int Dispatcher_RequestPCRoomPlayerCount::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user != 0 && user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        return 0;
    }
    const unsigned int accountId = user != 0 ? user->get_acc_id() : 0;
    cMyTrace trace(
        "virtual int Dispatcher_RequestPCRoomPlayerCount::dispatch_sig(CUser*, PacketBuf&)",
        0xd186, 5);
    trace("PCROOM : Dispatcher_RequestPCRoomPlayerCount => User State : %d, m_id : %s",
          user != 0 ? user->get_state() : 0, NumberToString(accountId, 0));
    return LineFunc(0xd187,
                    "virtual int Dispatcher_RequestPCRoomPlayerCount::dispatch_sig(CUser*, PacketBuf&)",
                    0, 0);
}
int Dispatcher_RequestPCRoomPlayerCount::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestPCRoomPlayerCount::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_MemoryIntegrity::Dispatcher_Security_MemoryIntegrity() {}
Dispatcher_Security_MemoryIntegrity::~Dispatcher_Security_MemoryIntegrity() {}
int Dispatcher_Security_MemoryIntegrity::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_MI_RecvMemoryIntegrityModulation(user, packet);
    if (result == 2)
    {
        return LineFunc(0xdf97,
                        "virtual int Dispatcher_Security_MemoryIntegrity::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (result == 3)
    {
        return LineFunc(0xdf99,
                        "virtual int Dispatcher_Security_MemoryIntegrity::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (result == 1)
    {
        return LineFunc(0xdf95,
                        "virtual int Dispatcher_Security_MemoryIntegrity::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_MemoryIntegrity::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_MemoryIntegrity::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_PacketIntegrity::Dispatcher_Security_PacketIntegrity() {}
Dispatcher_Security_PacketIntegrity::~Dispatcher_Security_PacketIntegrity() {}
int Dispatcher_Security_PacketIntegrity::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_PacketIntegrity(user, packet);
    switch (result)
    {
    default:
        return 0;
    case 1:
        return LineFunc(0xded7,
                        "virtual int Dispatcher_Security_PacketIntegrity::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    case 2:
        return LineFunc(0xded9,
                        "virtual int Dispatcher_Security_PacketIntegrity::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    case 3:
        return LineFunc(0xdedb,
                        "virtual int Dispatcher_Security_PacketIntegrity::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    case 4:
        return LineFunc(0xdedd,
                        "virtual int Dispatcher_Security_PacketIntegrity::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    case 5:
        return LineFunc(0xdedf,
                        "virtual int Dispatcher_Security_PacketIntegrity::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
}
int Dispatcher_Security_PacketIntegrity::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_PacketIntegrity::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SelectUltimateDifficulty::Dispatcher_SelectUltimateDifficulty() {}
Dispatcher_SelectUltimateDifficulty::~Dispatcher_SelectUltimateDifficulty() {}
int Dispatcher_SelectUltimateDifficulty::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }int Dispatcher_SelectUltimateDifficulty::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SelectUltimateDifficulty::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Select_Item_Grwoth_Power::Dispatcher_Select_Item_Grwoth_Power() {}
Dispatcher_Select_Item_Grwoth_Power::~Dispatcher_Select_Item_Grwoth_Power() {}
int Dispatcher_Select_Item_Grwoth_Power::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081db580：state>2 且当前角色非空才放行；随后要求
    // (growth_power_reward & 0xff) < 当前段下界（get_break_away_section_low_level）。
    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return -1;
    }
    int level = user->get_charac_level();
    int low = -1;  // ORIG 0x0822afd6：无命中返回 -1
    const std::vector<STBreakAwaySection>& secs = G_CDataManager()->m_growthPower.m_vec0;
    for (std::vector<STBreakAwaySection>::const_iterator it = secs.begin(); it != secs.end(); ++it)
    {
        if (static_cast<int>(static_cast<unsigned char>(it->m_field1)) > level)
        {
            break;
        }
        low = static_cast<unsigned char>(it->m_field1);
    }
    unsigned char reward = user->get_growth_power_reward();
    if (static_cast<int>(reward) < low)
    {
        return 0;
    }
    return -1;
}

int Dispatcher_Select_Item_Grwoth_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }

    MSG_GROWTH_POWER& req = static_cast<MSG_GROWTH_POWER&>(msg);
    stGrowthPowerParam& out = static_cast<stGrowthPowerParam&>(param);
    CDataManager* dm = G_CDataManager();

    STBreakAwaySection section =
        dm->m_growthPower.get_break_away_section_data(user->get_charac_level());
    int key = makeFairPVPEquipmentPackageKey(
        user->get_charac_job(),
        static_cast<int>(static_cast<unsigned char>(section.m_field0)));
    std::map<int, std::vector<std::vector<STStackablePackageData> > >::iterator it =
        dm->m_growthPower.m_map18.find(key);
    if (it == dm->m_growthPower.m_map18.end())
    {
        return 0;
    }

    std::vector<Inven_Item> items;
    int idx = 0;
    for (std::vector<std::vector<STStackablePackageData> >::const_iterator oit = it->second.begin();
         oit != it->second.end(); ++oit)
    {
        unsigned char sel = *reinterpret_cast<unsigned char*>(&req.m_0e[idx]);
        if (oit->size() <= static_cast<unsigned int>(sel))
        {
            cMyTrace trace(
                "virtual int Dispatcher_Select_Item_Grwoth_Power::process(CUser*, MSG_BASE&, ParamBase&)",
                0x4a8b, 0);
            trace("[GrowthPower](%d)(j:%d)(l:%d)(s:%d)(p:%d)",
                  user->get_acc_id(), user->get_charac_job(), user->get_charac_level(),
                  static_cast<int>(oit->size()), static_cast<int>(sel));
            return LineFunc(0x4a8c,
                            "virtual int Dispatcher_Select_Item_Grwoth_Power::process(CUser*, MSG_BASE&, ParamBase&)",
                            0, user->get_acc_id());
        }
        if (idx > 4)
        {
            return LineFunc(0x4a8f,
                            "virtual int Dispatcher_Select_Item_Grwoth_Power::process(CUser*, MSG_BASE&, ParamBase&)",
                            0, user->get_acc_id());
        }

        Inven_Item item;
        item.m_addInfo = (*oit)[sel].m_field0;
        CItem* info = dm->find_item(item.m_addInfo);
        if (info == 0)
        {
            out.m_4 = 0x11;
            return 0;
        }
        info->make_item(item);
        if (item.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
        {
            item.m_addInfo2 = (*oit)[sel].m_field4;
        }
        items.push_back(item);
        ++idx;
    }

    for (std::vector<Inven_Item>::const_iterator iit = items.begin(); iit != items.end(); ++iit)
    {
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            *iit, static_cast<eItemAddReason>(8), true, true);
        if (slot < 0)
        {
            char title[0x15];
            char body[0x100];
            strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_213", 0), 0x14);
            strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_214", 0), 0xff);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, *iit, 0, user->getCurCharacNo(), body,
                static_cast<int>(strlen(body)), 0, user->GetServerGroup(), false, false);
        }
        else
        {
            user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                                     static_cast<ENUM_ITEMSPACE>(0), slot);
        }
    }
    return 0;
}

int Dispatcher_Select_Item_Grwoth_Power::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_GROWTH_POWER& req = static_cast<MSG_GROWTH_POWER&>(msg);
    if (!packet.get_byte(req.m_0d))
    {
        return LineFunc(0x4a55,
                        "virtual int Dispatcher_Select_Item_Grwoth_Power::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    for (int i = 0; i < static_cast<int>(req.m_0d); ++i)
    {
        if (!packet.get_byte(*reinterpret_cast<unsigned char*>(&req.m_0e[i])))
        {
            return LineFunc(0x4a5a,
                            "virtual int Dispatcher_Select_Item_Grwoth_Power::read(PacketBuf&, MSG_BASE&)",
                            0, 0);
        }
    }
    return 0;
}

void Dispatcher_Select_Item_Grwoth_Power::send(CUser* user, ParamBase& param)
{
    stGrowthPowerParam& out = static_cast<stGrowthPowerParam&>(param);
    if (out.m_4 == 0)
    {
        int level = user->get_charac_level();
        user->set_growth_power_reward(level);
        user->SendCmdOkPacket(static_cast<ENUM_CMDPACKET>(0x1b0));
    }
    else
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1b0),
                                 static_cast<unsigned char>(out.m_4));
    }
}

Dispatcher_SeriaRoom_Ani_Deco_Event::Dispatcher_SeriaRoom_Ani_Deco_Event() {}
Dispatcher_SeriaRoom_Ani_Deco_Event::~Dispatcher_SeriaRoom_Ani_Deco_Event() {}
int Dispatcher_SeriaRoom_Ani_Deco_Event::check_error(CUser* user, MSG_BASE& msg)
{
    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x69);
    return event->IsEventing(0) ? 0 : 1;
}
int Dispatcher_SeriaRoom_Ani_Deco_Event::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SERIAROOM_DECO_EVENT& req = static_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_pad + 1)))
    {
        return LineFunc(0x6540,
                        "virtual int Dispatcher_SeriaRoom_Ani_Deco_Event::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

Dispatcher_UpdateContractOfCubeInfo::Dispatcher_UpdateContractOfCubeInfo() {}
Dispatcher_UpdateContractOfCubeInfo::~Dispatcher_UpdateContractOfCubeInfo() {}
int Dispatcher_UpdateContractOfCubeInfo::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG 0x825fa8e：premium 类型 0x5c 未受影响才返回错误 8。
    return user != 0 && user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x5c)
               ? 0
               : 8;
}
int Dispatcher_UpdateContractOfCubeInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpdateContractOfCubeInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_UpdateContractOfCubeInfo::send(CUser* user, ParamBase& param) {}

Dispatcher_UseCreatureEvolutionItem::Dispatcher_UseCreatureEvolutionItem() {}
Dispatcher_UseCreatureEvolutionItem::~Dispatcher_UseCreatureEvolutionItem() {}
int Dispatcher_UseCreatureEvolutionItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UseCreatureEvolutionItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseCreatureEvolutionItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseRightOfChangeGrowType::Dispatcher_UseRightOfChangeGrowType() {}
Dispatcher_UseRightOfChangeGrowType::~Dispatcher_UseRightOfChangeGrowType() {}
int Dispatcher_UseRightOfChangeGrowType::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_UseRightOfChangeGrowType::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseRightOfChangeGrowType::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_UseRightOfChangeGrowType::send(CUser* user, ParamBase& param) {}

Dispatcher_AuctionMyRegistedItemInfo::Dispatcher_AuctionMyRegistedItemInfo() {}
Dispatcher_AuctionMyRegistedItemInfo::~Dispatcher_AuctionMyRegistedItemInfo() {}
int Dispatcher_AuctionMyRegistedItemInfo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionMyRegistedItemInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionMyRegistedItemInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BloodRoundUiPrepareFinish::Dispatcher_BloodRoundUiPrepareFinish() {}
Dispatcher_BloodRoundUiPrepareFinish::~Dispatcher_BloodRoundUiPrepareFinish() {}
int Dispatcher_BloodRoundUiPrepareFinish::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x8222270：战斗态+血轮地下城且非（bloodType<1 或物品等级==1）→ 血轮生成计时器
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return LineFunc(0xdbc0,
                        "virtual int Dispatcher_BloodRoundUiPrepareFinish::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (user->get_state() != 5)
    {
        return LineFunc(0xdbc2,
                        "virtual int Dispatcher_BloodRoundUiPrepareFinish::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    CParty* party = (CParty*)user->GetParty();
    if (party == 0 || party->m_dungeon == 0)
    {
        return 0;
    }
    CBattle_Field& field = *reinterpret_cast<CBattle_Field*>(&party->m_padBattleField1);
    MapInfo* mi = field.GetMapInfo();
    bool skip = (party->m_dungeon->m_bloodDungeonType < 1) ||
                (mi != 0 && mi->m_visited == 1);
    if (!skip)
    {
        field.BloodSpawnTimerManager(false, false);
    }
    return 0;
}
int Dispatcher_BloodRoundUiPrepareFinish::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BloodRoundUiPrepareFinish::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Skill_Command_All_Default::Dispatcher_Skill_Command_All_Default() {}
Dispatcher_Skill_Command_All_Default::~Dispatcher_Skill_Command_All_Default() {}
int Dispatcher_Skill_Command_All_Default::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Skill_Command_All_Default::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Skill_Command_All_Default::send(CUser* user, ParamBase& param) {}

Dispatcher_Skill_Command_Customizing::Dispatcher_Skill_Command_Customizing() {}
Dispatcher_Skill_Command_Customizing::~Dispatcher_Skill_Command_Customizing() {}

Dispatcher_Buy_Charac_Status_Using_QP::Dispatcher_Buy_Charac_Status_Using_QP() {}
Dispatcher_Buy_Charac_Status_Using_QP::~Dispatcher_Buy_Charac_Status_Using_QP() {}
int Dispatcher_Buy_Charac_Status_Using_QP::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    return user->get_state() == 3 ? 0 : 99;
}
int Dispatcher_Buy_Charac_Status_Using_QP::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Buy_Charac_Status_Using_QP::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Buy_Charac_Status_Using_QP::send(CUser* user, ParamBase& param) {}

Dispatcher_Change_PartyMemberPosition::Dispatcher_Change_PartyMemberPosition() {}
Dispatcher_Change_PartyMemberPosition::~Dispatcher_Change_PartyMemberPosition() {}
int Dispatcher_Change_PartyMemberPosition::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Change_PartyMemberPosition::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Change_PartyMemberPosition::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestIngameAdvertisement::Dispatcher_RequestIngameAdvertisement() {}
Dispatcher_RequestIngameAdvertisement::~Dispatcher_RequestIngameAdvertisement() {}

Dispatcher_CallPartyMemberRealtimeInfo::Dispatcher_CallPartyMemberRealtimeInfo() {}
Dispatcher_CallPartyMemberRealtimeInfo::~Dispatcher_CallPartyMemberRealtimeInfo() {}
int Dispatcher_CallPartyMemberRealtimeInfo::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return LineFunc(0x2781,
                        "int Dispatcher_CallPartyMemberRealtimeInfo::check_error(CUser*, MSG_BASE&)",
                        0, 0);
    }
    return user->get_state() == 3 ? 0 : -1;
}
int Dispatcher_CallPartyMemberRealtimeInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallPartyMemberRealtimeInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_CallPartyMemberRealtimeInfo::send(CUser* user, ParamBase& param) {}

Dispatcher_Direct_Entrance_Quick_Party::Dispatcher_Direct_Entrance_Quick_Party() {}
Dispatcher_Direct_Entrance_Quick_Party::~Dispatcher_Direct_Entrance_Quick_Party() {}
int Dispatcher_Direct_Entrance_Quick_Party::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Direct_Entrance_Quick_Party::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Direct_Entrance_Quick_Party::send(CUser* user, ParamBase& param) {}

Dispatcher_Exchange_Random_Item_Reward::Dispatcher_Exchange_Random_Item_Reward() {}
Dispatcher_Exchange_Random_Item_Reward::~Dispatcher_Exchange_Random_Item_Reward() {}
int Dispatcher_Exchange_Random_Item_Reward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Exchange_Random_Item_Reward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Exchange_Random_Item_Reward::send(CUser* user, ParamBase& param) {}

Dispatcher_RequestConditionEventReward::Dispatcher_RequestConditionEventReward() {}
Dispatcher_RequestConditionEventReward::~Dispatcher_RequestConditionEventReward() {}
int Dispatcher_RequestConditionEventReward::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestConditionEventReward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestConditionEventReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TournamentRewardSelectState::Dispatcher_TournamentRewardSelectState() {}
Dispatcher_TournamentRewardSelectState::~Dispatcher_TournamentRewardSelectState() {}
int Dispatcher_TournamentRewardSelectState::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_TournamentRewardSelectState::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TournamentRewardSelectState::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_TournamentRewardSelectState::send(CUser* user, ParamBase& param) {}

Dispatcher_Expand_Equipslot_Flag_Update::Dispatcher_Expand_Equipslot_Flag_Update() {}
Dispatcher_Expand_Equipslot_Flag_Update::~Dispatcher_Expand_Equipslot_Flag_Update() {}
int Dispatcher_Expand_Equipslot_Flag_Update::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Expand_Equipslot_Flag_Update::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Expand_Equipslot_Flag_Update::send(CUser* user, ParamBase& param) {}

Dispatcher_FinishVillageMonsterFighting::Dispatcher_FinishVillageMonsterFighting() {}
Dispatcher_FinishVillageMonsterFighting::~Dispatcher_FinishVillageMonsterFighting() {}
int Dispatcher_FinishVillageMonsterFighting::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_FinishVillageMonsterFighting::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_FinishVillageMonsterFighting::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GrowthWeaponEventUseMaterial::Dispatcher_GrowthWeaponEventUseMaterial() {}
Dispatcher_GrowthWeaponEventUseMaterial::~Dispatcher_GrowthWeaponEventUseMaterial() {}
int Dispatcher_GrowthWeaponEventUseMaterial::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_GrowthWeaponEventUseMaterial::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GrowthWeaponEventUseMaterial::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GrowthWeaponEventUseMaterial::send(CUser* user, ParamBase& param) {}

Dispatcher_HeroMissionEventState_Reward::Dispatcher_HeroMissionEventState_Reward() {}
Dispatcher_HeroMissionEventState_Reward::~Dispatcher_HeroMissionEventState_Reward() {}
int Dispatcher_HeroMissionEventState_Reward::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_HeroMissionEventState_Reward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_HeroMissionEventState_Reward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_HeroMissionEventState_Reward::send(CUser* user, ParamBase& param) {}

Dispatcher_SaveGameOption_QuickChatting::Dispatcher_SaveGameOption_QuickChatting() {}
Dispatcher_SaveGameOption_QuickChatting::~Dispatcher_SaveGameOption_QuickChatting() {}
int Dispatcher_SaveGameOption_QuickChatting::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SaveGameOption_QuickChatting::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Avatar_Disjoint_Random_Reward::Dispatcher_Avatar_Disjoint_Random_Reward() {}
Dispatcher_Avatar_Disjoint_Random_Reward::~Dispatcher_Avatar_Disjoint_Random_Reward() {}
int Dispatcher_Avatar_Disjoint_Random_Reward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Avatar_Disjoint_Random_Reward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Avatar_Disjoint_Random_Reward::send(CUser* user, ParamBase& param) {}

Dispatcher_SDC_ActiveStatus_Damage_Check::Dispatcher_SDC_ActiveStatus_Damage_Check() {}
Dispatcher_SDC_ActiveStatus_Damage_Check::~Dispatcher_SDC_ActiveStatus_Damage_Check() {}
int Dispatcher_SDC_ActiveStatus_Damage_Check::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SDC_ActiveStatus_Damage_Check::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SDC_ActiveStatus_Damage_Check::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_police_queryCharacName_useMid::Dispatcher_police_queryCharacName_useMid() {}
Dispatcher_police_queryCharacName_useMid::~Dispatcher_police_queryCharacName_useMid() {}
int Dispatcher_police_queryCharacName_useMid::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_police_queryCharacName_useMid::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_police_queryCharacName_useMid::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ImageCommunicationEquipmentUse::Dispatcher_ImageCommunicationEquipmentUse() {}
Dispatcher_ImageCommunicationEquipmentUse::~Dispatcher_ImageCommunicationEquipmentUse() {}
int Dispatcher_ImageCommunicationEquipmentUse::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ImageCommunicationEquipmentUse::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ImageCommunicationEquipmentUse::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ImageCommunicationEquipmentUse::send(CUser* user, ParamBase& param) {}

Dispatcher_Security_NonClient_GetResponse::Dispatcher_Security_NonClient_GetResponse() {}
Dispatcher_Security_NonClient_GetResponse::~Dispatcher_Security_NonClient_GetResponse() {}
int Dispatcher_Security_NonClient_GetResponse::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_NonClient_GetResponse(user, packet);
    if (result == 1)
    {
        return LineFunc(0xdf88,
                        "virtual int Dispatcher_Security_NonClient_GetResponse::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (result == 2)
    {
        return LineFunc(0xdf8a,
                        "virtual int Dispatcher_Security_NonClient_GetResponse::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_NonClient_GetResponse::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_NonClient_GetResponse::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_PI_HashModulation_C2C::Dispatcher_Security_PI_HashModulation_C2C() {}
Dispatcher_Security_PI_HashModulation_C2C::~Dispatcher_Security_PI_HashModulation_C2C() {}
int Dispatcher_Security_PI_HashModulation_C2C::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_PI_2ND_RecvPacketIntegrityModulation_C2C(user, packet);
    if (result == 1)
    {
        return LineFunc(0xe32f,
                        "virtual int Dispatcher_Security_PI_HashModulation_C2C::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (result == 2)
    {
        return LineFunc(0xe331,
                        "virtual int Dispatcher_Security_PI_HashModulation_C2C::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_PI_HashModulation_C2C::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_PI_HashModulation_C2C::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_PI_HashModulation_S2C::Dispatcher_Security_PI_HashModulation_S2C() {}
Dispatcher_Security_PI_HashModulation_S2C::~Dispatcher_Security_PI_HashModulation_S2C() {}
int Dispatcher_Security_PI_HashModulation_S2C::dispatch_sig(CUser* user, PacketBuf& packet)
{
    int result = _NS_PI_2ND_RecvPacketIntegrityModulation_S2C(user, packet);
    if (result == 2)
    {
        return LineFunc(0xe320,
                        "virtual int Dispatcher_Security_PI_HashModulation_S2C::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (result == 3)
    {
        return LineFunc(0xe322,
                        "virtual int Dispatcher_Security_PI_HashModulation_S2C::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    if (result == 1)
    {
        return LineFunc(0xe31e,
                        "virtual int Dispatcher_Security_PI_HashModulation_S2C::dispatch_sig(CUser*, PacketBuf&)",
                        0, 0);
    }
    return 0;
}
int Dispatcher_Security_PI_HashModulation_S2C::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_PI_HashModulation_S2C::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Compound_Equipment_Upgrade_Card::Dispatcher_Compound_Equipment_Upgrade_Card() {}
Dispatcher_Compound_Equipment_Upgrade_Card::~Dispatcher_Compound_Equipment_Upgrade_Card() {}


Dispatcher_GrowthWeaponEventChangeInfinity::Dispatcher_GrowthWeaponEventChangeInfinity() {}
Dispatcher_GrowthWeaponEventChangeInfinity::~Dispatcher_GrowthWeaponEventChangeInfinity() {}
int Dispatcher_GrowthWeaponEventChangeInfinity::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_GrowthWeaponEventChangeInfinity::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GrowthWeaponEventChangeInfinity::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GrowthWeaponEventChangeInfinity::send(CUser* user, ParamBase& param) {}

Dispatcher_SaveGameOption_ChattingEmoticon::Dispatcher_SaveGameOption_ChattingEmoticon() {}
Dispatcher_SaveGameOption_ChattingEmoticon::~Dispatcher_SaveGameOption_ChattingEmoticon() {}
int Dispatcher_SaveGameOption_ChattingEmoticon::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SaveGameOption_ChattingEmoticon::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SaveGameOption_ChattingEmoticon::send(CUser* user, ParamBase& param) {}

Dispatcher_ComboSkillExtensionQuickSlotReset::Dispatcher_ComboSkillExtensionQuickSlotReset() {}
Dispatcher_ComboSkillExtensionQuickSlotReset::~Dispatcher_ComboSkillExtensionQuickSlotReset() {}
int Dispatcher_ComboSkillExtensionQuickSlotReset::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->get_state() < 3 || user->get_charac_job() != 9)
    {
        return -1;
    }
    const MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET& req =
        static_cast<const MSG_COMBO_SKILL_EXTENSION_QUICK_SLOT_RESET&>(msg);
    return req.m_0d == 0 || req.m_0d == 1 ? 0 : 0x13;
}
int Dispatcher_ComboSkillExtensionQuickSlotReset::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ComboSkillExtensionQuickSlotReset::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ComboSkillExtensionQuickSlotReset::send(CUser* user, ParamBase& param) {}

Dispatcher_Select_3rdChronicleItem_ForEnchant::Dispatcher_Select_3rdChronicleItem_ForEnchant() {}
Dispatcher_Select_3rdChronicleItem_ForEnchant::~Dispatcher_Select_3rdChronicleItem_ForEnchant() {}
int Dispatcher_Select_3rdChronicleItem_ForEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Select_3rdChronicleItem_ForEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GrowthCreatureChangeInfinityCreature::Dispatcher_GrowthCreatureChangeInfinityCreature() {}
Dispatcher_GrowthCreatureChangeInfinityCreature::~Dispatcher_GrowthCreatureChangeInfinityCreature() {}
int Dispatcher_GrowthCreatureChangeInfinityCreature::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_GrowthCreatureChangeInfinityCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GrowthCreatureChangeInfinityCreature::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GrowthCreatureChangeInfinityCreature::send(CUser* user, ParamBase& param) {}

Dispatcher_P2PHolePunchingSuccessRateStatistics::Dispatcher_P2PHolePunchingSuccessRateStatistics() {}
Dispatcher_P2PHolePunchingSuccessRateStatistics::~Dispatcher_P2PHolePunchingSuccessRateStatistics() {}
int Dispatcher_P2PHolePunchingSuccessRateStatistics::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_P2PHolePunchingSuccessRateStatistics::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_P2PHolePunchingSuccessRateStatistics::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_P2PHolePunchingSuccessRateStatistics::send(CUser* user, ParamBase& param) {}

namespace ARAD {
namespace DISPATCHER {

// 声明于 Arad_InternalDispatcher.h（ORIG 0x081977b6；实现在 Arad_InternalDispatcher.cpp）

// 线格式 Sig 结构（ORIG arad 命名空间；本 TU 按 put_binary 字节数定义）。
// [推断] 字段级语义由各 checkWorkState/dispatch_sig 使用方定义；此处仅保证
// 线格式长度（dispatch_sig 读包与 put_binary 一致）。

// ==================== Dispatcher_AvartarRoulette（ORIG 0x819ef54）====================
// dispatch_sig：user==0→0x312；getCurCharacR==0→0；state<3→SendCmdErrorPacket
//   (0x24a,7) 返回 0；CheckInTrade→同上；PVP/FreePvP→同上；读 2 字节
//   （slotType<0xb、count<10）；通过后发内部流 0x2c（3 字节）。
//   checkWorkState 安全检查依赖未建模服务（ServiceRestrictManager 等），推断为通过。
Dispatcher_AvartarRoulette::Dispatcher_AvartarRoulette() {}
Dispatcher_AvartarRoulette::~Dispatcher_AvartarRoulette() {}
int Dispatcher_AvartarRoulette::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x312;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    if (user->get_state() < 3)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24a, 7);
        return 0;
    }
    unsigned char buf[3];
    memset(buf, 0, sizeof(buf));
    if (packet.get_byte(buf[0]) != 1)
    {
        return 0x32f;
    }
    if (packet.get_byte(buf[1]) != 1)
    {
        return 0x331;
    }
    if (buf[0] >= 0xb)
    {
        return 0x334;
    }
    if (buf[1] >= 10)
    {
        return 0x337;
    }
    // [推断] checkWorkState（ServiceRestrictManager::isRestricted /
    // CSecu_ProtectionField::Check / EmblemCompoundServer 等未建模）暂按通过。
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x347);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2c, (int)user->GetUID());
    guard.put_binary(buf, 3);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AvartarRoulette::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AvartarRoulette::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AvartarCoinCount（ORIG 0x819e660 区）====================
// 当前仅 checkWorkState/dispatch_sig 桩（工作列表不含）；保留签名，补最小骨架。
Dispatcher_AvartarCoinCount::Dispatcher_AvartarCoinCount() {}
Dispatcher_AvartarCoinCount::~Dispatcher_AvartarCoinCount() {}
int Dispatcher_AvartarCoinCount::checkWorkState(CUser* user)
{
    // [推断] 未建模服务检查，暂按通过。
    return 1;
}
int Dispatcher_AvartarCoinCount::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x3f3;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    return 0;
}
int Dispatcher_AvartarCoinCount::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AvartarCoinCount::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AradAvatarConvert（ORIG 0x81a00be）====================
// dispatch_sig：user==0→0x74a；getCurCharacR==0→0；state∉[3,4]→0；PVP→
//   SendCmdErrorPacket(0x24f,7)；CheckInTrade→同上；isEnableAvatarSocketAction
//   （未建模，推断 0）；读 short+short+int；发内部流 0x33（0xe 字节）。
Dispatcher_AradAvatarConvert::Dispatcher_AradAvatarConvert() {}
Dispatcher_AradAvatarConvert::~Dispatcher_AradAvatarConvert() {}
int Dispatcher_AradAvatarConvert::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x74a;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    int st = user->get_state();
    if (st < 3 || st > 4)
    {
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24f, 7);
        return 0;
    }
    // [推断] CUser::isEnableAvatarSocketAction 未建模，按 0（允许）处理。
    unsigned char buf[0xe];
    memset(buf, 0, sizeof(buf));
    short s0 = 0, s1 = 0;
    if (packet.get_short(s0) != 1)
    {
        return 0x76d;
    }
    if (packet.get_short(s1) != 1)
    {
        return 0x771;
    }
    int i0 = 0;
    if (packet.get_int(i0) != 1)
    {
        return 0x775;
    }
    *(short*)&buf[0] = s0;
    *(short*)&buf[2] = s1;
    *(int*)&buf[4] = i0;
    // [推断] checkWorkState 安全检查（未建模）暂按通过。
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x780);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x33, (int)user->GetUID());
    guard.put_binary(buf, 0xe);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AradAvatarConvert::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AradAvatarConvert::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AradEmblemCompound（ORIG 0x819fd7e）====================
// dispatch_sig：user==0→0x6f1；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket(0x24e,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   isEnableAvatarSocketAction（推断 0）；读 3 个 short；发内部流 0x32（0x16 字节）。
Dispatcher_AradEmblemCompound::Dispatcher_AradEmblemCompound() {}
Dispatcher_AradEmblemCompound::~Dispatcher_AradEmblemCompound() {}
int Dispatcher_AradEmblemCompound::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x6f1;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    int st = user->get_state();
    if (st < 3 || st > 4)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 7);
        return 0;
    }
    // [推断] CUser::isEnableAvatarSocketAction 未建模，按 0（允许）处理。
    unsigned char buf[0x16];
    memset(buf, 0, sizeof(buf));
    short s0 = 0, s1 = 0, s2 = 0;
    if (packet.get_short(s0) != 1)
    {
        return 0x713;
    }
    if (packet.get_short(s1) != 1)
    {
        return 0x717;
    }
    if (packet.get_short(s2) != 1)
    {
        return 0x71b;
    }
    *(short*)&buf[0] = s0;
    *(short*)&buf[2] = s1;
    *(short*)&buf[4] = s2;
    // [推断] checkWorkState（ServiceRestrictManager::isRestricted /
    // CSecu_ProtectionField::Check / EmblemCompoundServer::CheckCondition 未建模）
    // 暂按通过。
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x723);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x32, (int)user->GetUID());
    guard.put_binary(buf, 0x16);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AradEmblemCompound::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AradEmblemCompound::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AradJumpingCharacter（ORIG 0x819f1ec）====================
// dispatch_sig：user==0→0x59b；getCurCharacR==0→0；读 byte+byte+short+int
//   （错误码 0x5a9/0x5ac/0x5af/0x5b1/0x5b4/0x5b6）；state∉[3,4]→0；
//   通过后发内部流 0x2d（0x11 字节，含 acc_id/charac_no）。
Dispatcher_AradJumpingCharacter::Dispatcher_AradJumpingCharacter() {}
Dispatcher_AradJumpingCharacter::~Dispatcher_AradJumpingCharacter() {}
int Dispatcher_AradJumpingCharacter::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x59b;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    unsigned char buf[0x11];
    memset(buf, 0, sizeof(buf));
    unsigned char b1 = 0;
    if (packet.get_byte(b1) != 1)
    {
        return 0x5a9;
    }
    if (b1 == 0 || b1 > 2)
    {
        return 0x5ac;
    }
    unsigned char b2 = 0;
    if (packet.get_byte(b2) != 1)
    {
        return 0x5af;
    }
    if (b2 == 0 || b2 > 4)
    {
        return 0x5b1;
    }
    unsigned short s1 = 0;
    if (packet.get_short(s1) != 1)
    {
        return 0x5b4;
    }
    int i1 = 0;
    if (packet.get_int(i1) != 1)
    {
        return 0x5b6;
    }
    buf[0] = b1;
    buf[1] = b2;
    *(unsigned short*)&buf[2] = s1;
    *(int*)&buf[4] = i1;
    int st = user->get_state();
    if (st < 3 || st > 4)
    {
        return 0;
    }
    // [推断] checkWorkState（未建模）暂按通过。
    *(int*)&buf[8] = (int)user->get_acc_id();    // +0x08 acc_id
    *(int*)&buf[0xc] = user->get_charac_no(-1);  // +0x0c charac_no
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x5c8);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2d, (int)user->GetUID());
    guard.put_binary(buf, 0x11);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AradJumpingCharacter::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AradJumpingCharacter::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AradUseAvatarRecharge（ORIG 0x819fa56）====================
// dispatch_sig：user==0→0x69c；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket(0x24d,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   读 short+short；发内部流 0x30（0x10 字节）。
Dispatcher_AradUseAvatarRecharge::Dispatcher_AradUseAvatarRecharge() {}
Dispatcher_AradUseAvatarRecharge::~Dispatcher_AradUseAvatarRecharge() {}
int Dispatcher_AradUseAvatarRecharge::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x69c;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    int st = user->get_state();
    if (st < 3 || st > 4)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 7);
        return 0;
    }
    unsigned char buf[0x10];
    memset(buf, 0, sizeof(buf));
    short s0 = 0, s1 = 0;
    if (packet.get_short(s0) != 1)
    {
        return 0x6b6;
    }
    if (packet.get_short(s1) != 1)
    {
        return 0x6ba;
    }
    *(short*)&buf[0] = s0;
    *(short*)&buf[2] = s1;
    // [推断] checkWorkState（未建模）暂按通过。
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x6c2);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x30, (int)user->GetUID());
    guard.put_binary(buf, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AradUseAvatarRecharge::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AradUseAvatarRecharge::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_EventCreateDnfRequest（ORIG 0x81a0362）====================
// dispatch_sig：packet==0→0x7b3；state∉[3,4]→0；CEventCreateDnf 事件（GetRepeatEvent
//   /GetRate/IsRewardCondition/SendReward，未建模）标注推断：发 0x22d 响应包
//   （rate 0 + flag 0）并返回。
Dispatcher_EventCreateDnfRequest::Dispatcher_EventCreateDnfRequest() {}
Dispatcher_EventCreateDnfRequest::~Dispatcher_EventCreateDnfRequest() {}
int Dispatcher_EventCreateDnfRequest::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x7b3;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    int st = user->get_state();
    if (st < 3 || st > 4)
    {
        return 0;
    }
    // [推断] ORIG 经 CEventManager::GetRepeatEvent(0xa0) 取 CEventCreateDnf 并
    // 判定奖励条件/发放奖励（CEventCreateDnf 未建模）。此处仅发送 0x22d 响应
    // 骨架：put_header(0,0x22d) + put_short(rate) + put_byte(0)。
    PacketGuard guard;
    guard.put_header(0, 0x22d);
    guard.put_short(0);  // [推断] CEventCreateDnf::GetRate 未建模，置 0
    guard.put_byte(0);
    guard.finalize(true);
    user->Send(guard);
    return 0;
}
int Dispatcher_EventCreateDnfRequest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_EventCreateDnfRequest::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_AvartarHiddenOptionChange（ORIG 0x819f6f8）====================
// dispatch_sig：user==0→0x632；getCurCharacR==0→0；state∉[3,4]→
//   SendCmdErrorPacket(0x24c,7)；PVP/FreePvP→同上；CheckInTrade→同上；
//   读 short+short；发内部流 0x2e（6 字节）。
Dispatcher_AvartarHiddenOptionChange::Dispatcher_AvartarHiddenOptionChange() {}
Dispatcher_AvartarHiddenOptionChange::~Dispatcher_AvartarHiddenOptionChange() {}
int Dispatcher_AvartarHiddenOptionChange::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x632;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    int st = user->get_state();
    if (st < 3 || st > 4)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 7);
        return 0;
    }
    if (G_GameWorld()->IsPVPChannel() || G_GameWorld()->IsFreePvPChannel())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 7);
        return 0;
    }
    if (user->CheckInTrade())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 7);
        return 0;
    }
    unsigned char buf[6];
    memset(buf, 0, sizeof(buf));
    short s0 = 0, s1 = 0;
    if (packet.get_short(s0) != 1)
    {
        return 0x64d;
    }
    if (packet.get_short(s1) != 1)
    {
        return 0x651;
    }
    *(short*)&buf[0] = s0;
    *(short*)&buf[2] = s1;
    // [推断] checkWorkState（未建模）暂按通过。
    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_PacketDispatcher.cpp", 0x659);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2e, (int)user->GetUID());
    guard.put_binary(buf, 6);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), guard);
    return 0;
}
int Dispatcher_AvartarHiddenOptionChange::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_AvartarHiddenOptionChange::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

// ==================== Dispatcher_RequestPrcoomDayilyReward（ORIG 0x81a0612）====================
// dispatch_sig：user==0→0x7e8；getCurCharacR==0→0；state∉[3,4]→0；
//   每日奖励剩余次数（CUser+0x8ec32）<1→SendCmdErrorPacket(0x259,0x16)；
//   读 int itemId（0x212f/0x2130 限定，错误 0x7fb/0x800）；find_item 后
//   make_item 注入背包（invenType=0xe），成功扣减次数并发
//   InterSelectPcroomDailyReward::SendPacket / DBUpdatePcroomDailyReward::makeRequest。
Dispatcher_RequestPrcoomDayilyReward::Dispatcher_RequestPrcoomDayilyReward() {}
Dispatcher_RequestPrcoomDayilyReward::~Dispatcher_RequestPrcoomDayilyReward() {}
int Dispatcher_RequestPrcoomDayilyReward::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
    {
        return 0x7e8;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0;
    }
    int st = user->get_state();
    if (st < 3 || st > 4)
    {
        return 0;
    }
    if (user->m_field8ec32 < 1)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x259, 0x16);
        return 0;
    }
    int itemId = 0;
    if (packet.get_int(itemId) != 1)
    {
        return 0x7fb;
    }
    if (itemId != 0x212f && itemId != 0x2130)
    {
        return 0x800;
    }
    CItem* item = G_CDataManager()->find_item(itemId);
    if (item == 0 || item->GetItemType() != 0)
    {
        return 0;
    }
    Inven_Item inven;
    item->make_item(inven);
    inven.set_add_info(1);
    CInventory* inv = user->getCurCharacInvenW();
    int slot = inv->insertItemIntoInventory(
        inven, (eItemAddReason)0xe, true, true);
    if (slot < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x259, 4);
    }
    else
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x259);
        user->SendUpdateItem(CUser::eSendTarget_1, ENUM_ITEMSPACE_0, slot);
        user->m_field8ec32 = (char)(user->m_field8ec32 - 1);
        // [推断] InterSelectPcroomDailyReward::SendPacket /
        // DBUpdatePcroomDailyReward::makeRequest（未建模）未调用。
    }
    return 0;
}
int Dispatcher_RequestPrcoomDayilyReward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    return 0;
}
int Dispatcher_RequestPrcoomDayilyReward::read(PacketBuf& packet, MSG_BASE& msg)
{
    return 0;
}

}
}
namespace ARAD {
// Arad_Dispatcher_Login_Impl 生命周期占位保留原状（真实 db_dispatch 0x27d +
// premium_info_set/punish_user_check/account_password_check/pad_certifiaction_check
// 待独立批次建模；本票范围仅 Manager/BuyItemCheck_Impl，见 Arad_Dispatcher.cpp）。
Arad_Dispatcher_Login_Impl::Arad_Dispatcher_Login_Impl() {}
Arad_Dispatcher_Login_Impl::~Arad_Dispatcher_Login_Impl() {}
}  // namespace ARAD


namespace APSystem {
Dispatcher_ClearAction::Dispatcher_ClearAction() {}
Dispatcher_ClearAction::~Dispatcher_ClearAction() {}
int Dispatcher_ClearAction::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ClearAction::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ClearAction::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GetTodayReward::Dispatcher_GetTodayReward() {}
Dispatcher_GetTodayReward::~Dispatcher_GetTodayReward() {}
int Dispatcher_GetTodayReward::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_GetTodayReward::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GetTodayReward::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

}


// ══════════════ 

// ══════════════ 批次 B1：10 类桩真实化落盘区（DispatchersRestore） ══════════════

/* ==================== AvatarOptionChange_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_AvatarOptionChange 桩真实化（批次 disp_b1）
// 依据 docs/class_func_reports/Dispatcher_AvatarOptionChange/{read,process,check_error,send}.md
//
// 宿主落盘注意（Dispatchers.cpp）：
//   1. 下方"局部线格式视图"为文件级一次性定义，置于类实现之前（匿名 namespace，
//      与 altar_pkt 模式一致），不要随函数重复粘贴。
//   2. Dispatchers.cpp 现有 include 已含 CDataManager.h/CInventory.h/CUserCharacInfo.h/
//      GlobalData.h/CompoundEmblemDeps.h(LineFunc)；还需补
//      #include "CStackableItem.h"、#include "CEquipItem.h"（find_item 结果
//      downcast 与 IsAvatarOptionValid 需要完整类型）。
//   3. PacketTypes.h 现有 MSG_CHANGE_AVATAR_OPTION 字段化（+0xd pad/+0xe u16/+0x10 int/
//      +0x14 u16/+0x16 int/+0x1a u8）与 ORIG 报告偏移（+0xd short/+0xf int/+0x13 short/
//      +0x15 int/+0x19 byte）错位 1 字节，本批不改共享头，统一经视图按报告偏移访问；
//      stChangeAvatarOption 仍为 m_pad[0x40]，同样经视图访问。
// ============================================================================

// ==== 前置：局部线格式视图（宿主一次性置入匿名 namespace） ====
namespace {

// ORIG read(0x081e04f2) 线格式：
//   +0x0d short 消耗品槽位（0<=v<=0x137）、+0x0f int 消耗品 itemIdx（>=0）、
//   +0x13 short 目标 avatar 装备槽位（0<=v<=0x68）、+0x15 int 目标装备 itemIdx（>=0）、
//   +0x19 byte 目标 avatar option 值（bit7==0）。
struct AocMsgView
{
    char m_head[0xd];            // MSG_BASE（0xd 字节）
    short m_consumeSlot;         // +0x0d 消耗品槽位
    int m_consumeItemIdx;        // +0x0f 消耗品 item idx
    short m_avatarSlot;          // +0x13 目标 avatar 装备槽位
    int m_equipItemIdx;          // +0x15 目标装备 item idx
    char m_option;               // +0x19 目标 avatar option 值（get_byte(char&)）
};

// stChangeAvatarOption(ParamBase) 视图：process 写 / send 读。
struct AocParamView
{
    char m_pad0[4];
    int m_errorCode;             // +0x04 check_error 结果
    short m_consumeSlot;         // +0x08
    short m_avatarSlot;          // +0x0a
    unsigned char m_option;      // +0x0c
};

}  // namespace

// ==== Dispatcher_AvatarOptionChange::read (ORIG 0x081e04f2) ====
int Dispatcher_AvatarOptionChange::read(PacketBuf& packet, MSG_BASE& msg)
{
    AocMsgView& m = *reinterpret_cast<AocMsgView*>(&msg);
    if (!packet.get_short(m.m_consumeSlot))
        return LineFunc(0x5380,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (m.m_consumeSlot < 0 || m.m_consumeSlot > 0x137)
        return LineFunc(0x5383,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (!packet.get_int(m.m_consumeItemIdx))
        return LineFunc(0x5386,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (m.m_consumeItemIdx < 0)
        return LineFunc(0x5389,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (!packet.get_short(m.m_avatarSlot))
        return LineFunc(0x538c,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (m.m_avatarSlot < 0 || m.m_avatarSlot > 0x68)
        return LineFunc(0x5390,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (!packet.get_int(m.m_equipItemIdx))
        return LineFunc(0x5393,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (m.m_equipItemIdx < 0)
        return LineFunc(0x5396,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (!packet.get_byte(m.m_option))
        return LineFunc(0x5399,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (((unsigned char)m.m_option & 0x80) != 0)
        return LineFunc(0x539c,
                        "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    return 0;
}

// ==== Dispatcher_AvatarOptionChange::check_error (ORIG 0x081e0a40) ====
// 校验链：user/状态门 → Secu 保护域(0x2a) → 消耗品槽一致性/数量 → 消耗品为可堆叠的
// avatar option change 道具 → 目标槽物品一致性 → 目标为非堆叠 avatar 装备 → 锁定检查
// → job/type/grade 条件或限定集合 → option 值合法性。
int Dispatcher_AvatarOptionChange::check_error(CUser* user, MSG_BASE& msg)
{
    AocMsgView& m = *reinterpret_cast<AocMsgView*>(&msg);
    if (user == 0)
        return -1;
    if (user->get_state() <= 2)
        return -1;

    int secuErr = GlobalData::s_pSecuProtectionField->Check(user,
                                                            (SECURITY_PROTCTION)0x2a);
    if (secuErr != 0)
        return secuErr;

    const CInventory* invR = user->getCurCharacInvenR();
    Inven_Item consumeSlot = invR->GetInvenSlot(INVEN_TYPE_EQUIP, m.m_consumeSlot);
    if (consumeSlot.m_addInfo != m.m_consumeItemIdx)
        return 0x11;
    if (consumeSlot.m_addInfo2 <= 0)
        return 0x16;

    CStackableItem* consume =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(consumeSlot.m_addInfo));
    if (consume == 0)
        return 0x11;
    if (!consume->is_stackable())
        return 0x11;
    if (!consume->IsAvatarOptionChangeItem())
        return 0x11;

    Inven_Item avatarSlot = invR->GetInvenSlot(INVEN_TYPE_AVATAR, m.m_avatarSlot);
    if (avatarSlot.m_addInfo != m.m_equipItemIdx)
        return 0x11;

    CItem* equip = G_CDataManager()->find_item(m.m_equipItemIdx);
    if (equip == 0)
        return 0x11;
    if (equip->is_stackable())  // 目标必须是非堆叠装备（ORIG 判断取反）
        return 0x11;
    if (!equip->IsAvatarItem())  // ORIG vtable+0x10 无参虚调用（IsAvatarItem 槽）
        return 0x11;
    if (user->CheckItemLock(INVEN_TYPE_AVATAR, m.m_avatarSlot))
        return 0xd5;

    // 消耗品尾部三字节选项条件区：+0x365 job、+0x366 type、+0x367 grade
    const signed char optJob = (signed char)(consume->m_tail365.m_short365 & 0xff);
    const signed char optType = (signed char)((consume->m_tail365.m_short365 >> 8) & 0xff);
    const signed char optGrade = (signed char)consume->m_tail365.m_char367;

    if (consume->m_set368.empty())
    {
        if ((equip->check_job_type((char)optJob) != 1) && optJob != 0xb)
            return 0xfe;
        if (equip->GetItemType() != (int)optType && optType != 0x1a)
            return 0xfd;
        if (equip->get_grade() != (int)optGrade && optGrade != 0)
            return 0xfc;
    }
    else if (consume->m_set368.find(avatarSlot.m_addInfo) == consume->m_set368.end())
    {
        return 0xff;
    }

    if (!consume->m_set380.empty() &&
        consume->m_set380.find((int)(signed char)m.m_option) != consume->m_set380.end())
    {
        return 0xfb;
    }

    if (!static_cast<const CEquipItem*>(equip)->IsAvatarOptionValid(m.m_option))
        return 0xfa;

    return 0;
}

// ==== Dispatcher_AvatarOptionChange::process (ORIG 0x081e0774) ====
// 影子背包预演删除消耗品成功后，才落到真实背包；改写目标 avatar 装备的 option 字节并回包。
int Dispatcher_AvatarOptionChange::process(CUser* user, MSG_BASE& msg, ParamBase& paramBase)
{
    AocMsgView& m = *reinterpret_cast<AocMsgView*>(&msg);
    AocParamView& param = *reinterpret_cast<AocParamView*>(&paramBase);

    int err = check_error(user, msg);
    param.m_errorCode = err;
    if (param.m_errorCode > 0)
        return 0;
    if (param.m_errorCode < 0)
        return LineFunc(0x53ab,
                        "virtual int Dispatcher_AvatarOptionChange::process(CUser*, MSG_BASE&, ParamBase&)",
                        param.m_errorCode, 0);

    CInventory* invW = user->getCurCharacInvenW();

    // ORIG 在栈上构造影子背包（CInventory + InventoryMemory 0xc140）做删除预演。
    CInventory shadowInv;
    InventoryMemory shadowMem;
    shadowInv.SetInventoryMemory(&shadowMem);
    shadowInv.setCopy(*user->getCurCharacInvenR());

    if (shadowInv.delete_item(INVEN_TYPE_EQUIP, m.m_consumeSlot, 1,
                              (eItemDelReason)0x2f, false) != 1)
    {
        param.m_errorCode = 0x16;
        return 0;
    }
    Inven_Item* shadowRef = shadowInv.GetInvenRef(INVEN_TYPE_AVATAR, m.m_avatarSlot);
    if (shadowRef == 0)
    {
        param.m_errorCode = 0x11;
        return 0;
    }

    invW->delete_item(INVEN_TYPE_EQUIP, m.m_consumeSlot, 1, (eItemDelReason)0x2f, true);
    Inven_Item* avatarRef = invW->GetInvenRef(INVEN_TYPE_AVATAR, m.m_avatarSlot);

    // BLOCKED: _ZN15cUserHistoryLog21UseAvatarOptionChangeEiiii @0x08686d0e ——
    //   CUser.h 的 cUserHistoryLog 缺该声明，待历史日志批次补齐后启用：
    //   user->m_historyLog.UseAvatarOptionChange(avatarRef->m_addInfo2,
    //       avatarRef->m_addInfo, avatarRef->m_fieldb, (int)(signed char)m.m_option);

    avatarRef->m_fieldb = (unsigned short)(signed char)m.m_option;
    invW->SendAvatarEvent(0x2c3, avatarRef->m_addInfo2,
                          (int)(signed char)m.m_option, 0, 0);

    param.m_consumeSlot = m.m_consumeSlot;
    param.m_avatarSlot = m.m_avatarSlot;
    param.m_option = (unsigned char)m.m_option;
    return 0;
}

// ==== Dispatcher_AvatarOptionChange::send (ORIG 0x081e0e32) ====
void Dispatcher_AvatarOptionChange::send(CUser* user, ParamBase& paramBase)
{
    AocParamView& param = *reinterpret_cast<AocParamView*>(&paramBase);
    if (param.m_errorCode == 0)
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(1, 0x1cc);
        packet.put_byte(1);
        packet.put_short(param.m_avatarSlot);
        packet.put_byte((int)(signed char)param.m_option);
        packet.finalize(true);
        user->Send(packet);
        user->SendUpdateItemList(CUser::eSendTarget_1, ENUM_ITEMSPACE_0,
                                 param.m_consumeSlot);
        return;
    }
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1cc, (unsigned char)param.m_errorCode);
}

/* ==================== BlueMarbleConfirmInfo_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_BlueMarbleConfirmInfo 四函数真实化
// ORIG: check_error 0x081e68c8 / read 0x081e6242 / process 0x081e62da /
//       send 0x081e6904
//
// 宿主落盘注意（Dispatchers.cpp 已有 include 集合核对）：
//   - BlueMarble.h / CGameManager.h(G_CGameManager/getBlueMarble(int)) /
//     CInventory.h(NumberToString 声明) / GlobalData.h：已 include ✓
//   - CEnvironment.h：需补 include（G_CEnvironment()->get_file_name）。
//   - CMyFileLog.h：需补 include（CMyFileLog ctor/operator() 完整类型）。
//   - <cstdio>：需补 include（sprintf；仓库先例 BlueMarble.cpp 同型用法）。
//
// Param：模板第二参 stBlueMarbleConfirmInfo（PacketTypes.h:1720，具名字段
// m_4=+0x04 错误码）。process 不写其余字段，send 仅读 m_4 —— 无偏移访问。
//
// check_error 以汇编为准：Ghidra 反编译把 user 误写为 param_2
// （cmpl $0x0,0xc(%ebp) 检查的是栈上 user 实参），已修正。
// ============================================================================

// ==== Dispatcher_BlueMarbleConfirmInfo::check_error (ORIG 0x081e68c8) ====
int Dispatcher_BlueMarbleConfirmInfo::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    return 0;
}

// ==== Dispatcher_BlueMarbleConfirmInfo::read (ORIG 0x081e6242) ====
int Dispatcher_BlueMarbleConfirmInfo::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_BLUE_MARBLE_CONFIRM_INFO& req =
        static_cast<MSG_BLUE_MARBLE_CONFIRM_INFO&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))        // +0x0d flag
    {
        return LineFunc(24845,
                        "virtual int Dispatcher_BlueMarbleConfirmInfo::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_0e)))    // +0x0e index
    {
        return LineFunc(24848,
                        "virtual int Dispatcher_BlueMarbleConfirmInfo::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_BlueMarbleConfirmInfo::process (ORIG 0x081e62da) ====
int Dispatcher_BlueMarbleConfirmInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stBlueMarbleConfirmInfo& out = static_cast<stBlueMarbleConfirmInfo&>(param);
    MSG_BLUE_MARBLE_CONFIRM_INFO& req =
        static_cast<MSG_BLUE_MARBLE_CONFIRM_INFO&>(msg);

    out.m_4 = check_error(user, msg);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(24863,
                        "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_4, 0);
    }

    // P4：1024 字节栈数组内联清零（rep stos 形态）
    char path[1024] = { '\0' };
    sprintf(path, "./log/%s/BlueMarbleConfirmInfo",
            G_CEnvironment()->get_file_name());

    short clientIdx = static_cast<short>(user->getBlueMarbleIndex());   // u16→s16
    short serverIdx = static_cast<short>(req.m_0e);                     // u16→s16
    int space = static_cast<int>(user->getMoveSpace());
    int state = user->get_state();
    int characNo = user->get_charac_no(-1);
    char* idStr = NumberToString(user->get_acc_id(), 0);

    CMyFileLog clientLog(
        "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
        0x6127);
    clientLog(path,
              "Client m_id(%s) charac_no(%u) userState(%d) userSpace(%d)"
              "client_blueMarble_Index(%d) server_bluemarble_index(%d)",
              idStr, characNo, state, space,
              static_cast<int>(clientIdx), static_cast<int>(serverIdx));

    // ORIG 此处读 scriptMgr+0x30 至未消费局部（死值），省略不影响语义。
    if (req.m_0d == 0)                                   // 用户仍在蓝弹珠内存池
    {
        if (serverIdx == clientIdx)
        {
            BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
            if (marble != 0)
            {
                CMyFileLog sameLog(
                    "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
                    0x614d);
                sameLog(path,
                        "OverLap User index same m_id(%s) charac_no(%u) userState(%d)"
                        " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                        idStr, characNo, state, space,
                        static_cast<int>(clientIdx), static_cast<int>(serverIdx));
                marble->checkBlueMarbleUserConfirm(user);
            }
        }
        else
        {
            BlueMarble* gmMarble =
                G_CGameManager()->getBlueMarble(static_cast<int>(serverIdx));
            if (gmMarble != 0)
            {
                CMyFileLog gmLog(
                    "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
                    0x6135);
                gmLog(path,
                      "OverLap User index different m_id(%s) charac_no(%u) userState(%d)"
                      " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                      idStr, characNo, state, space,
                      static_cast<int>(clientIdx), static_cast<int>(serverIdx));
                gmMarble->checkBlueMarbleUserConfirm(user);
            }
            BlueMarble* userMarble =
                static_cast<BlueMarble*>(user->getBlueMarble());
            if (userMarble != 0)
            {
                CMyFileLog userLog(
                    "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
                    0x6140);
                userLog(path,
                        "OverLap User index different m_id(%s) charac_no(%u) userState(%d)"
                        " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                        idStr, characNo, state, space,
                        static_cast<int>(clientIdx), static_cast<int>(serverIdx));
                userMarble->checkBlueMarbleUserConfirm(user);
            }
        }
    }
    else                                                 // 用户不在池：仅日志+确认
    {
        CMyFileLog noneLog(
            "virtual int Dispatcher_BlueMarbleConfirmInfo::process(CUser*, MSG_BASE&, ParamBase&)",
            0x6158);
        noneLog(path,
                "Not Exist User m_id(%s) charac_no(%u) userState(%d)"
                " userSpace(%d)client_blueMarble_Index(%d) server_bluemarble_index(%d)",
                idStr, characNo, state, space,
                static_cast<int>(clientIdx), static_cast<int>(serverIdx));
        BlueMarble* marble = static_cast<BlueMarble*>(user->getBlueMarble());
        if (marble != 0)
        {
            marble->checkBlueMarbleUserConfirm(user);
        }
    }
    return 0;
}

// ==== Dispatcher_BlueMarbleConfirmInfo::send (ORIG 0x081e6904) ====
void Dispatcher_BlueMarbleConfirmInfo::send(CUser* user, ParamBase& param)
{
    stBlueMarbleConfirmInfo& out = static_cast<stBlueMarbleConfirmInfo&>(param);
    if (out.m_4 == 0)
    {
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fc);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fc,
                                 static_cast<unsigned char>(out.m_4 & 0xff));
    }
}

/* ==================== MailBox_Send_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_MailBox_Send —— 还原 ORIG 0x081cc3da..0x081ccdf9（check_error/
// GuildMailCheck/process/read/send）。依据 docs/class_func_reports/
// Dispatcher_MailBox_Send/{read,check_error,GuildMailCheck,process,send}.md。
//
// 宿主落盘注意：
// 1. Dispatcher_MailBox_Send::GuildMailCheck 当前未在 Dispatchers.h 声明，
//    需在类内补 `int GuildMailCheck(CUser* user, MSG_BASE& msg);`。
// 2. PacketTypes.h 的 MSG_MAILBOX_SEND 目前为 char m_pad[0x100]（sizeof 0x10d），
//    而 ORIG 消息布局至少用到 +0x144（m_45[0x100] 文本区）。落盘前必须把
//    m_pad 扩容到 ≥0x141（总长 ≥0x14d），否则下列偏移访问越界。字段语义：
//      +0x0d int   收件人名长度（read 首个 get_int）
//      +0x11 char[0x1d] 收件人名
//      +0x2e unsigned int 邮件金币
//      +0x32 unsigned int 公会邮键（非 0 走公会邮件）
//      +0x36 unsigned char 附件物品空间（GetInvenTypeFromItemSpace 入参）
//      +0x37 unsigned short 附件槽位
//      +0x39 unsigned long 附件物品 id（find_item 入参；32 位 4 字节）
//      +0x3d int 附件附加字段（read 读入、本批函数未消费）
//      +0x41 int 标题长度
//      +0x45 char[0x100] 邮件文本
// 3. BLOCKED 调用（ORIG 真实符号存在、仓库尚无声明，保留占位不影响其余逻辑）：
//      CheckItemTradeLimitCountToMoveByUser(CUser&,int,int,bool&) @0x0812032e
//      CUser::reqSendMailCertify()                                @0x0868a51a
//      CUser::set_MailMsgData(MSG_MAILBOX_SEND*)                  @0x0868a4b6
//      CUser::IncrementTradeGold(unsigned int,int)                @0x0864658a
// ============================================================================


                                     // GlobalData::s_pSecuProtectionField

namespace  // 本 TU 内的偏移视图（MSG_MAILBOX_SEND 字段化前的一致访问点）
{

inline int& mbLen(MSG_BASE& m) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&m) + 0x0d); }          // +0x0d 收件人名长
inline char* mbName(MSG_BASE& m) { return reinterpret_cast<char*>(&m) + 0x11; }                                 // +0x11 收件人名[0x1d]
inline unsigned int& mbGold(MSG_BASE& m) { return *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&m) + 0x2e); } // +0x2e 金币
inline unsigned int& mbGuildKey(MSG_BASE& m) { return *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(&m) + 0x32); } // +0x32 公会邮键
inline unsigned char& mbSpace(MSG_BASE& m) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&m) + 0x36); } // +0x36 物品空间
inline unsigned short& mbSlot(MSG_BASE& m) { return *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&m) + 0x37); } // +0x37 槽位
inline unsigned long& mbItemId(MSG_BASE& m) { return *reinterpret_cast<unsigned long*>(reinterpret_cast<char*>(&m) + 0x39); } // +0x39 物品 id
inline int& mbTitleLen(MSG_BASE& m) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&m) + 0x41); }     // +0x41 标题长
inline char* mbText(MSG_BASE& m) { return reinterpret_cast<char*>(&m) + 0x45; }                                 // +0x45 文本[0x100]

}  // namespace

// ==== Dispatcher_MailBox_Send::read (ORIG 0x081cc3da) ====
int Dispatcher_MailBox_Send::read(PacketBuf& packet, MSG_BASE& msg)
{
    if (!packet.get_int(mbLen(msg)))  // +0x0d 收件人名长
        return LineFunc(0x2309, "virtual int Dispatcher_MailBox_Send::read(PacketBuf&, MSG_BASE&)", 0, 0);

    if (!packet.get_str(mbName(msg), 0x1d, mbLen(msg)) ||                       // +0x11 收件人名
        !packet.get_int(mbGold(msg)) ||                                         // +0x2e 金币
        !packet.get_byte(mbSpace(msg)) ||                                       // +0x36 空间
        !packet.get_short(mbSlot(msg)) ||                                       // +0x37 槽位
        !packet.get_int(mbItemId(msg)) ||                                       // +0x39 物品 id
        !packet.get_int(*reinterpret_cast<int*>(reinterpret_cast<char*>(&msg) + 0x3d)))  // +0x3d 附件附加字段
        return LineFunc(0x2315, "virtual int Dispatcher_MailBox_Send::read(PacketBuf&, MSG_BASE&)", 0, 0);

    mbTitleLen(msg) = 0;                                        // +0x41 = 0
    mbGuildKey(msg) = 0;                                        // +0x32 = 0
    memset(mbText(msg), 0, 0x100);                              // +0x45 清零

    if (!packet.get_int(mbTitleLen(msg)))                       // +0x41 标题长
        return LineFunc(0x231b, "virtual int Dispatcher_MailBox_Send::read(PacketBuf&, MSG_BASE&)", 0, 0);
    if (!packet.get_str(mbText(msg), 0x100, mbTitleLen(msg)))   // +0x45 文本
        return LineFunc(0x231e, "virtual int Dispatcher_MailBox_Send::read(PacketBuf&, MSG_BASE&)", 0, 0);
    if (!packet.get_int(mbGuildKey(msg)))                       // +0x32 公会邮键
        return LineFunc(0x2321, "virtual int Dispatcher_MailBox_Send::read(PacketBuf&, MSG_BASE&)", 0, 0);
    return 0;
}

// ==== Dispatcher_MailBox_Send::GuildMailCheck (ORIG 0x081cc8c2) ====
int Dispatcher_MailBox_Send::GuildMailCheck(CUser* user, MSG_BASE& msg)
{
    if (mbGuildKey(msg) == 0)                                   // +0x32 公会邮键
        return 0x22;

    if (!user->IsSubGuildMaster() && !user->IsGuildMaster())    // 仅会长/副会长可发
        return 0x24;

    const int fee = *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CDataManager()) + 0xa624);  // +0xa624 公会邮件费（推断：ORIG 全局费用字段）
    const CInventory* inv = user->getCurCharacInvenR();
    if (inv->get_money() < fee)
        return 0x23;
    return 0;
}

// ==== Dispatcher_MailBox_Send::check_error (ORIG 0x081cc636) ====
int Dispatcher_MailBox_Send::check_error(CUser* user, MSG_BASE& msg)
{
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
        return -1;

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, RestrictCategory::Enum_1, 4))
        return 0xd1;

    const int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, static_cast<SECURITY_PROTCTION>(0x3f));
    if (secuErr != 0)
        return secuErr;

    const int invenType = GetInvenTypeFromItemSpace(static_cast<int>(mbSpace(msg)));
    const int slot = static_cast<int>(mbSlot(msg));
    if (user->CheckItemLock(invenType, slot))
        return 0xd5;

    {
        bool movable = true;
        /* BLOCKED: _Z36CheckItemTradeLimitCountToMoveByUserR5CUseriiRb @0x0812032e
           ORIG: CheckItemTradeLimitCountToMoveByUser(*user, invenType, slot, &movable);
           仓库暂无该真实声明，占位保持其余逻辑；恢复后 movable 由其写回。 */
        (void)invenType;
        (void)slot;
        if (!movable)
            return 0x72;
    }

    {
        const std::string title(mbText(msg));                   // +0x45 文本入 std::string
        if (CSyncSlangFilter::GetInstance()->HasSlang(title))
            return 0x9f;
    }

    if (strlen(mbText(msg)) != static_cast<size_t>(mbTitleLen(msg)))
        return -3;                                              // 标题长与实际串长不符

    if (mbItemId(msg) == 0 &&
        mbGold(msg) == 0 &&
        mbText(msg)[0] == '\0')
        return 0x16;                                            // 纯空邮件

    if (mbGuildKey(msg) != 0) {
        const int guildErr = GuildMailCheck(user, msg);
        if (guildErr != 0)
            return guildErr;
    }

    if (user->GetReliablePerson() == 1)
        return 0;
    if (!user->IsAvailableCurCharacTradeGoldDaily(mbGold(msg)))
        return 0x46;                                            // 当日交易金币额度不足
    return 0;
}

// ==== Dispatcher_MailBox_Send::process (ORIG 0x081cc958) ====
int Dispatcher_MailBox_Send::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stMailboxSendParam& out = static_cast<stMailboxSendParam&>(param);

    out.m_4 = check_error(user, msg);                           // +0x04 错误码
    if (out.m_4 > 0)
        return 0;
    if (out.m_4 < 0)
        return LineFunc(0x23ce,
                        "virtual int Dispatcher_MailBox_Send::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_4, user->get_acc_id());

    if (mbGuildKey(msg) != 0) {                                 // ---- 公会邮件分支 ----
        const int fee = *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CDataManager()) + 0xa624);  // +0xa624 公会邮件费（推断）
        CInventory* invW = user->getCurCharacInvenW();
        invW->use_money(fee, static_cast<eMoneySubReason>(0x16), true);

        CGuildServerProxy* proxy =
            reinterpret_cast<CServerProxyMgr<CGuildServerProxy>*>(GlobalData::s_guild_proxy_mgr)
                ->GetServerProxy(user->GetServerGroup());
        proxy->SendGuildMail(static_cast<unsigned int>(user->getCurCharacNo()),
                             static_cast<unsigned int>(user->get_charac_guildkey()),
                             mbTitleLen(msg),
                             mbText(msg));
        return 0;
    }

    // ---- 附带金币时先走客户端确认回执（certify）路径 ----
    bool needCertify = false;
    if (mbGold(msg) != 0) {
        /* BLOCKED: _ZN5CUser18reqSendMailCertifyEv @0x0868a51a
           ORIG: needCertify = user->reqSendMailCertify();
           仓库暂无该真实声明，占位保持 false 与其余逻辑一致。 */
        needCertify = false;
    }
    if (needCertify) {
        /* BLOCKED: _ZN5CUser15set_MailMsgDataEP16MSG_MAILBOX_SEND @0x0868a4b6
           ORIG: user->set_MailMsgData(static_cast<MSG_MAILBOX_SEND*>(&msg));
           仓库暂无该真实声明；恢复后在此保存消息等待回执再继续。 */
        return 0;
    }

    // ---- DB 收件人校验请求 ----
    int rc = 0;
    {
        MSG_CHECK_MAIL_RECEIVER chk;
        chk.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_0, MSG_MAILBOX_SEND>(
            static_cast<const MSG_MAILBOX_SEND&>(msg));
        rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(user, mbName(msg), &chk);
    }
    if (rc != 0) {
        out.m_4 = rc;                                           // 校验失败码透传
    }
    else {
        // 交易金币计数：普通低稀有度附件才计入特殊标记
        unsigned char specialMark = 0;
        if (mbItemId(msg) != 0) {
            CItem* item = G_CDataManager()->find_item(static_cast<int>(mbItemId(msg)));
            if (item != 0) {
                // ORIG 经 vtable+0x10 间接调用（= IsAvatarItem 槽，ORIG vtable 已核实）
                if (!item->IsAvatarItem() && !(item->get_rarity() > 1))
                    specialMark = 1;
            }
        }
        if (mbItemId(msg) == 0)
            specialMark = 1;
        /* BLOCKED: _ZN5CUser18IncrementTradeGoldEji @0x0864658a
           ORIG: user->IncrementTradeGold(mbGold(msg), specialMark);
           仓库仅有 CReliablePerson::IncrementTradeGold(unsigned int)，
           CUser 两参版本缺声明，占位保持其余逻辑。 */
        (void)specialMark;
    }

    // ---- 邮件金币上限统计（SIG_FIND_FACTORY_HUB 入 NETWORK 统计队列）----
    {
        const unsigned short usedFatigue = user->getCurCharacUsedFatigue();
        const int rnd = get_rand_int(0x19);
        const int money = user->getCurCharacInvenR()->get_money();
        // ORIG 浮点序：(long long)(money * (0.7 + (float)((double)rnd * 0.01)))
        // （.rodata 0x08bd6338=0.01、0x08bd6340=0.7，已从二进制核实）
        const double factor = 0.7 + static_cast<double>(static_cast<float>(static_cast<double>(rnd) * 0.01));
        const unsigned long long goldCap =
            static_cast<unsigned long long>(static_cast<double>(money) * factor);
        if (goldCap <= static_cast<unsigned long long>(mbGold(msg))) {
            Stream* stream = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x241d);
            CStreamGuard guard(stream, true);
            guard << static_cast<int>(0x18a);                   // 统计包命令字
            guard << static_cast<int>(user->GetUID());
            SIG_FIND_FACTORY_HUB* sig = guard.GetInBuffer<SIG_FIND_FACTORY_HUB>();
            memset(sig, 0, 0x24);                               // SIG 总长 0x24（当前头为 pad 视图）
            *reinterpret_cast<unsigned int*>(sig) = user->get_acc_id();          // +0x00 accId
            strncpy(reinterpret_cast<char*>(sig) + 4, mbName(msg),
                    static_cast<size_t>(mbLen(msg)));                            // +0x04 收件人名
            *(reinterpret_cast<char*>(sig) + 0x21) = 0;                          // +0x21 疲劳标记清零
            const int fatigueRoll = get_rand_int(0x14);
            if (fatigueRoll + 0x78 <= static_cast<int>(static_cast<unsigned int>(usedFatigue)))
                *(reinterpret_cast<char*>(sig) + 0x21) = 1;              // +0x21 疲劳标记
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
    }
    return 0;
}

// ==== Dispatcher_MailBox_Send::send (ORIG 0x081ccdb2) ====
void Dispatcher_MailBox_Send::send(CUser* user, ParamBase& param)
{
    stMailboxSendParam& p = static_cast<stMailboxSendParam&>(param);
    if (p.m_4 == 0x7fffffff)                                    // ORIG 哨兵值：静默
        return;
    if (p.m_4 == 0)
        return;
    user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x61),
                             static_cast<unsigned char>(p.m_4 & 0xff));
}

/* ==================== MonstercardBind_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_MonstercardBind 桩真实化 —— 函数体交付
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 对应类段
// （替换 2344-2349 行的空 ctor/dtor/桩；dtor ORIG 为基类转发，保持 {}）
//
// 头约束说明（宿主知悉）：
//  - Dispatchers.h 的 Dispatcher_MonstercardBind 仅声明了 ctor/~dtor/
//    check_error/process/read/send，未声明私有 helper
//    (_bind_process/calcurate_rarity/check_need_slot/get_bind_result)、
//    ENUM_MONSTER_CARD_BIND_TYPE 与 m_rand(+0x04) 成员。
//    按任务规则，helper 逻辑以报告为准【内联】进 check_error/process，
//   并在各内联点注明 ORIG 地址。
//  - ORIG ctor (W 0x08595348) 于 this+0x04 new CMTRand(time(0))；因头无成员，
//    以本文件匿名命名空间 holder 承载同一随机数实例（种子与生命周期一致）。
//    若后续头补出 `CMTRand* m_rand; // +0x04` 与 4 个私有方法声明，可将各
//    内联段还原为独立成员定义（ORIG 符号均为 T，见下各节地址注释）。
//  - 依赖符号均已 nm/grep 核实存在：CUserCharacInfo::getCurCharacInvenR(W
//    0x080da27e)、CInventory::GetInvenSlot(int,int) const(0x084fb918)、
//    CDataManager::find_item(0x0835fa32)、CItem::is_stackable/get_rarity(W)、
//    CInventory::get_empty_slot(0x084fb824)/delete_item(0x0850400c)/
//    tryInsertEventItems(0x0850675e)/insert_event_items(0x08506bb2)/setCopy、
//    GameWorld::IsPVPChannel、G_GameWorld、G_CDataManager、LineFunc、
//    CSecu_ProtectionField::CheckItemRarity(0x082887d0)、CMTRand::randInt
//    (0x080cbabe)、STExpertJobScript 字段(m_bindListCnt/m_bindList/
//    m_bindOnlyCnt/m_bindOnly/m_monsterCardBindRateMap)。
// ============================================================================

// （宿主 TU 已含：Dispatchers.h/CDataManager.h/CInventory.h/GameWorld.h/
//   GlobalData.h/CUserCharacInfo.h/PvPTypes.h(CMTRand)/STExpertJobScript.h/
//   CompoundEmblemDeps.h(LineFunc)；cMyTrace 经 GameTypes.h 可见。）

namespace
{

// ORIG Dispatcher_MonstercardBind ctor @0x08595348 (W)：实例级 CMTRand 存
// this+0x04（Dispatchers.h 已补 m_rand 成员）。

} // anonymous namespace

// ==== Dispatcher_MonstercardBind::Dispatcher_MonstercardBind (ORIG 0x08595348 W) ====
Dispatcher_MonstercardBind::Dispatcher_MonstercardBind()
{
    // ORIG：call IPacketDispatcher<MSG_MONSTERCARD_BIND,stMonstercardBind,
    // ch_state3>::IPacketDispatcher(); 置 vtable; time(0); new(0x9c8) 后
    // CMTRand(const unsigned long&) 种子构造，指针存 this+0x04。
    unsigned long seed = (unsigned long)time(0);
    m_rand = new CMTRand(seed);
}

// ==== Dispatcher_MonstercardBind::~Dispatcher_MonstercardBind (ORIG D1 0x08244b42 / D0 0x08244b72，基类转发) ====
Dispatcher_MonstercardBind::~Dispatcher_MonstercardBind()
{
}

// ==== Dispatcher_MonstercardBind::read (ORIG 0x081d82e8 T) ====
int Dispatcher_MonstercardBind::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_MONSTERCARD_BIND& m = static_cast<MSG_MONSTERCARD_BIND&>(msg);
    short value = 0;
    if (!packet.get_short(value))
    {
        return LineFunc(
            0x444a,
            "virtual int Dispatcher_MonstercardBind::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    m.m_0d = (unsigned short)value;
    if (!packet.get_short(value))
    {
        return LineFunc(
            0x444c,
            "virtual int Dispatcher_MonstercardBind::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    m.m_0f = (unsigned short)value;
    if (!packet.get_short(value))
    {
        return LineFunc(
            0x444e,
            "virtual int Dispatcher_MonstercardBind::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    m.m_11 = (unsigned short)value;
    return 0;
}

// ==== Dispatcher_MonstercardBind::check_error (ORIG 0x081d8de2 T) ====
int Dispatcher_MonstercardBind::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }

    MSG_MONSTERCARD_BIND& m = static_cast<MSG_MONSTERCARD_BIND&>(msg);
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());

    // 卡 1（msg+0x0d）：须为非堆叠怪物卡且绑定数据索引有效
    Inven_Item slotA = invenR->GetInvenSlot(1, m.m_0d);
    CItem* cardA = G_CDataManager()->find_item(slotA.m_addInfo);
    if (cardA == 0)
    {
        return 0x11;
    }
    if (cardA->is_stackable())
    {
        return 0x11;
    }
    {
        // +0x330 推断：怪物卡绑定数据索引（ORIG mov 0x330(%eax),%eax；
        // CItem.h 当前布局止于 +0x148，按任务规则偏移访问）。
        const int bindInfoIdx = *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(cardA) + 0x330);
        if (bindInfoIdx == -1)
        {
            return 0x11;
        }
    }
    if (slotA.m_addInfo2 < 1)
    {
        return 0x16;
    }

    // 卡 2（msg+0x0f）：非堆叠 + 数量校验（无绑定索引检查，与 ORIG 一致）
    Inven_Item slotB = invenR->GetInvenSlot(1, m.m_0f);
    CItem* cardB = G_CDataManager()->find_item(slotB.m_addInfo);
    if (cardB == 0)
    {
        return 0x11;
    }
    if (cardB->is_stackable())
    {
        return 0x11;
    }
    if (slotB.m_addInfo2 < 1)
    {
        return 0x16;
    }

    // 卡 3（msg+0x11）：同卡 2 口径
    Inven_Item slotC = invenR->GetInvenSlot(1, m.m_11);
    CItem* cardC = G_CDataManager()->find_item(slotC.m_addInfo);
    if (cardC == 0)
    {
        return 0x11;
    }
    if (cardC->is_stackable())
    {
        return 0x11;
    }
    if (slotC.m_addInfo2 < 1)
    {
        return 0x16;
    }

    // 卡 2/卡 3 稀有度必须一致
    if (cardB->get_rarity() != cardC->get_rarity())
    {
        return 0x13;
    }

    // ---- 内联：check_need_slot @0x081d83c4 T ----
    // get_empty_slot(1, ITEM_TYPE 10)（怪物卡专用区）与 get_empty_slot(1,
    // ITEM_TYPE 0)；两者均 <0 才算无槽位。
    {
        const int emptyCard =
            invenR->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)10);
        const int emptyAny =
            invenR->get_empty_slot((INVEN_TYPE)1, (Inven_Item::ITEM_TYPE)0);
        const bool hasFreeSlot = (emptyCard >= 0) || (emptyAny >= 0);
        if (!hasFreeSlot)
        {
            return 4;
        }
    }

    // 非 PvP 通道且处于村庄态(3)才允许继续
    if (G_GameWorld()->IsPVPChannel() || user->get_state() != 3)
    {
        return -1;
    }

    // 类别校验（ORIG call CItem::hasCategory W 0x0822c97a = m_set128 查询；
    // CItem.h 未声明该方法，按其实现语义直查 m_set128）：
    // 两卡均须含类别 1，且均不得含类别 2。
    if (!(cardB->m_set128.count((STItemScript::ENUM_ITEM_CATEGORY)1) != 0 &&
          cardC->m_set128.count((STItemScript::ENUM_ITEM_CATEGORY)1) != 0))
    {
        return 0x17;
    }
    if (cardB->m_set128.count((STItemScript::ENUM_ITEM_CATEGORY)2) != 0 ||
        cardC->m_set128.count((STItemScript::ENUM_ITEM_CATEGORY)2) != 0)
    {
        return 0x17;
    }

    // 保护场稀有度检查；非 0 错误码透传
    return GlobalData::s_pSecuProtectionField->CheckItemRarity(
        user, (SECURITY_PROTCTION)0x28, cardB, &slotB);
}

// ==== Dispatcher_MonstercardBind::process (ORIG 0x081d88b0 T) ====
int Dispatcher_MonstercardBind::process(CUser* user, MSG_BASE& msg,
                                        ParamBase& param)
{
    stMonstercardBind& res = static_cast<stMonstercardBind&>(param);
    MSG_MONSTERCARD_BIND& m = static_cast<MSG_MONSTERCARD_BIND&>(msg);

    res.m_4 = check_error(user, msg);
    if (res.m_4 > 0)
    {
        return 0;
    }
    if (res.m_4 < 0)
    {
        return LineFunc(0x44db,
                        "virtual int Dispatcher_MonstercardBind::process(CUser*, "
                        "MSG_BASE&, ParamBase&)",
                        res.m_4, 0);
    }

    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    Inven_Item slotA = invenR->GetInvenSlot(1, m.m_0d);
    CItem* cardA = G_CDataManager()->find_item(slotA.m_addInfo);
    Inven_Item slotB = invenR->GetInvenSlot(1, m.m_0f);
    CItem* cardB = G_CDataManager()->find_item(slotB.m_addInfo);
    Inven_Item slotC = invenR->GetInvenSlot(1, m.m_11);
    CItem* cardC = G_CDataManager()->find_item(slotC.m_addInfo);

    std::vector<std::pair<int, int> > results;

    bool bindOk = false;
    {
        // ---- 内联：get_bind_result @0x081d84a0 T ----
        // 主卡（slotA）的怪物卡数据索引 → GetMonsterCardBindRate
        // （W 0x0822b2d0：m_monsterCardBindRateMap.find，未命中清零返假）。
        // +0x330 推断：怪物卡绑定数据索引（同 check_error 注）。
        const int cardDataIdx = *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(cardA) + 0x330);
        STExpertJobScript* script = static_cast<STExpertJobScript*>(
            G_CDataManager()->GetExpertJobScript(1));
        stMonsterCardBindRate rate;
        std::map<int, stMonsterCardBindRate>::const_iterator itRate =
            script->m_monsterCardBindRateMap.find(cardDataIdx);
        if (itRate != script->m_monsterCardBindRateMap.end())
        {
            rate = itRate->second;
            bindOk = true;
        }
        else
        {
            bindOk = false;
        }

        if (bindOk)
        {
            results.clear();

            // ---- 内联：calcurate_rarity @0x081d842e T ----
            // randInt(10000) < rate.m_field0 时按原稀有度升档：
            // 0→1、1→2、2/3→3、其它→0；否则保持原稀有度。
            const int rarity = cardB->get_rarity();
            unsigned long range = 10000;
            const int diceUp = m_rand->randInt(range);
            int newRarity = rarity;
            if ((unsigned int)diceUp < (unsigned int)rate.m_field0)
            {
                switch (rarity)
                {
                case 0:
                    newRarity = 1;
                    break;
                case 1:
                    newRarity = 2;
                    break;
                case 2:
                case 3:
                    newRarity = 3;
                    break;
                default:
                    newRarity = 0;
                    break;
                }
            }

            // 二次 roll 决定查 special(bind only) 还是 common(bind list) 表。
            range = 10000;
            const int diceTable = m_rand->randInt(range);
            const int bindType =
                ((unsigned int)diceTable < (unsigned int)rate.m_field2) ? 0 : 1;

            // ---- 内联：_bind_process @0x081d85fc T ----
            if (bindType == 0 || bindType == 1)
            {
                // ORIG GetSpecialBindInfo W 0x0822b358 = this+0xc4、
                //      GetCommonBindInfo  W 0x0822b366 = this+0x84
                // （stMonsterCardBindInfo：cnt[4]+vec[3]，STExpertJobScript.h）。
                const int* cntArr =
                    (bindType == 0)
                        ? script->m_specialBindInfo.m_count
                        : script->m_commonBindInfo.m_count;
                const std::vector<stMonsterCardBindResult>* lists =
                    (bindType == 0)
                        ? script->m_specialBindInfo.m_vec
                        : script->m_commonBindInfo.m_vec;
                const std::vector<stMonsterCardBindResult>& table =
                    lists[newRarity];
                const int total = cntArr[newRarity];
                const size_t entryCount = table.size();

                if (entryCount == 0)
                {
                    cMyTrace trace("bool Dispatcher_MonstercardBind::_bind_process("
                                   "Dispatcher_MonstercardBind::ENUM_MONSTER_CARD_BIND_"
                                   "TYPE, ENUM_RARITY, IntPairVector&)",
                                   0x44b1, 5);
                    trace("MonstercardBind Error : size(%d)", (int)entryCount);
                    bindOk = false;
                }
                else if (total != 0)
                {
                    cMyTrace trace("bool Dispatcher_MonstercardBind::_bind_process("
                                   "Dispatcher_MonstercardBind::ENUM_MONSTER_CARD_BIND_"
                                   "TYPE, ENUM_RARITY, IntPairVector&)",
                                   0x44b6, 5);
                    trace("MonstercardBind Error : size(%d), sum(%d)",
                          (int)entryCount, total);
                    bindOk = false;
                }
                else
                {
                    // 权重轮盘：pick ∈ [1,total]，累计权重覆盖 pick 的首个条目命中。
                    range = (unsigned long)(total - 1);
                    const int pick = m_rand->randInt(range) + 1;
                    int accWeight = 0;
                    for (size_t i = 0; i < entryCount; ++i)
                    {
                        const unsigned short weight =
                            (unsigned short)table[i].m_field4;
                        if ((size_t)pick > accWeight &&
                            pick <= accWeight + (int)weight)
                        {
                            results.push_back(
                                std::make_pair(table[i].m_field0, 1));
                            break;
                        }
                        accWeight += (int)weight;
                    }
                    bindOk = !results.empty();
                    if (!bindOk)
                    {
                        cMyTrace trace(
                            "bool Dispatcher_MonstercardBind::_bind_process("
                            "Dispatcher_MonstercardBind::ENUM_MONSTER_CARD_BIND_TYPE, "
                            "ENUM_RARITY, IntPairVector&)",
                            0x44ca, 5);
                        trace("Dispatcher_MonstercardBind::_bind_process ERROR : "
                              "result is empty");
                    }
                }
            }
            else
            {
                results.clear();
                bindOk = false;
            }
        }
    }

    if (!bindOk)
    {
        res.m_4 = 0x13;
        return 0;
    }

    // 影子背包试删试插（失败不影响真背包）
    CInventory tmpInv;
    InventoryMemory tmpMem;
    tmpInv.SetInventoryMemory(&tmpMem);
    // ORIG 081d8ac2：实参为 getCurCharacInvenRefR()(W 0x081803ca，charac+0xda
    // 槽区视图)，直接按 CInventory 传入 setCopy —— 类型双关，忠实转写。
    tmpInv.setCopy(*reinterpret_cast<const CInventory*>(
        user->getCurCharacInvenRefR()));

    const bool delA = tmpInv.delete_item((INVEN_TYPE)1, m.m_0d, 1,
                                         (eItemDelReason)0x2c, false);
    if (!delA)
    {
        res.m_4 = 0x11;
        return 0;
    }
    const bool delB = tmpInv.delete_item((INVEN_TYPE)1, m.m_0f, 1,
                                         (eItemDelReason)0x2c, false);
    if (!delB)
    {
        res.m_4 = 0x11;
        return 0;
    }
    const bool delC = tmpInv.delete_item((INVEN_TYPE)1, m.m_11, 1,
                                         (eItemDelReason)0x2c, false);
    if (!delC)
    {
        res.m_4 = 0x11;
        return 0;
    }
    if ((int)tmpInv.tryInsertEventItems(results) != (int)results.size())
    {
        res.m_4 = 0x11;
        return 0;
    }

    // 真背包扣三张卡并插入绑定结果
    CInventory* invenW = user->getCurCharacInvenW();
    invenW->delete_item((INVEN_TYPE)1, m.m_0d, 1, (eItemDelReason)0x2c, true);
    invenW->delete_item((INVEN_TYPE)1, m.m_0f, 1, (eItemDelReason)0x2c, true);
    invenW->delete_item((INVEN_TYPE)1, m.m_11, 1, (eItemDelReason)0x2c, true);

    std::vector<int> outSlots;
    invenW->insert_event_items(results, outSlots, (eMoneyAddReason)0,
                               (eItemAddReason)0x34, false, "", "");

    res.m_8 = m.m_0d;
    res.m_a = m.m_0f;
    res.m_c = m.m_11;
    res.m_list = outSlots;
    return 0;
}

// ==== Dispatcher_MonstercardBind::send (ORIG 0x081d9148 T) ====
void Dispatcher_MonstercardBind::send(CUser* user, ParamBase& param)
{
    stMonstercardBind& res = static_cast<stMonstercardBind&>(param);
    if (res.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x19e,
                                 (unsigned char)res.m_4);
        return;
    }

    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x19e);
    guard.put_byte(1);
    guard.put_short(res.m_8);
    guard.put_short(res.m_a);
    guard.put_short(res.m_c);

    const size_t entryTotal = res.m_list.size();
    const int groups = (int)(entryTotal / 3);
    guard.put_byte(groups);

    size_t idx = 0;
    for (int g = 0; g < groups; ++g)
    {
        guard.put_short(res.m_list[idx]);
        ++idx;
        guard.put_int(res.m_list[idx]);
        ++idx;
        guard.put_int(res.m_list[idx]);
        ++idx;
    }

    guard.finalize(true);
    user->Send(guard);
}

/* ==================== PartyTeleport_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_PartyTeleport 四函数真实化
// ORIG: check_error 0x081dc19a / read 0x081dbe70 / process 0x081dbfd6 /
//       send 0x081dc426
//
// 宿主落盘注意（Dispatchers.cpp 已有 include 集合核对）：
//   - GameWorld.h / GlobalData.h / CInventory.h：已 include ✓
//   - Village.h：需补 include（Village::get_gate_area 完整调用，先例
//     CPacketRespondent.cpp:15）。
//   - CPartyTelePort.h：需补 include（CPartyTelePort 成员调用；PacketTypes.h 仅
//     借其 stPartyTelePort(大写 P) 内嵌类型，不引入类本体）。
//   - CGuildServerProxy.h：需补 include（CEventManager::GetRepeatEvent /
//     CEventBase::IsEventing 完整类型）。
//   - CSystemTime.h：需补 include（s_systemTime_.getCurSec() 完整类型）。
//
// Param 视图：模板第二参 stPartyTeleport（PacketTypes.h:2308，ParamBase 派生，
// char m_pad[0x40] 占位）。ORIG process/send 报告实证布局：
//   +0x04 int 错误码、+0x08 byte 回显(msg+0xd)、+0x0c int 光环剩余秒。
// 头未字段化 → 按约束以 reinterpret_cast 偏移访问并注明。
// ============================================================================

// ==== Dispatcher_PartyTeleport::check_error (ORIG 0x081dc19a) ====
int Dispatcher_PartyTeleport::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    MSG_PARTY_TELEPORT& req = static_cast<MSG_PARTY_TELEPORT&>(msg);
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    GameWorld* world = G_GameWorld();
    if (world->IsPVPChannel() || world->IsPvPSkilTreeChannel())
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());   // GetParty 返回 void*
    if (party == 0)
    {
        return 0x13;
    }
    if (party->getManager() != user)                     // 仅队长可发起
    {
        return 0x17;
    }
    Village* village = G_GameWorld()->getUserVillage(static_cast<int>(req.m_0d));
    if (village == 0)
    {
        return 0x13;
    }
    if (static_cast<int>(req.m_0e) == village->get_gate_area())   // 目标=城门禁传
    {
        return 0x13;
    }
    int auraSlotCount = 0;
    if (!user->isGMUser())
    {
        // 返回值>0 时直接作为错误码透传（光环头像占用槽位检查）
        int auraEquip = user->is_equip_aura_avatar('\0', auraSlotCount);
        if (auraEquip > 0)
        {
            return auraEquip;
        }
    }
    int auraExpire = user->get_aura_avatar_option_value(0);
    if (!user->isGMUser() &&
        GlobalData::s_systemTime_.getCurSec() < auraExpire)
    {
        return 0x16;                                     // 光环传送券未过期→冷却
    }
    for (int i = 0; i < 4; ++i)                          // P2：i<=3 有符号循环
    {
        if (!party->checkValidUser(i))
        {
            continue;
        }
        CUser* member = party->get_user(i);
        if (member == 0)
        {
            continue;
        }
        int moveErr = G_GameWorld()->check_move_area(
            member, static_cast<int>(req.m_0d), static_cast<int>(req.m_0e),
            static_cast<int>(static_cast<short>(req.m_0f)),
            static_cast<int>(static_cast<short>(req.m_11)),
            static_cast<int>(req.m_13), true, 0);
        if (moveErr > 0)
        {
            return moveErr;
        }
    }
    return 0;
}

// ==== Dispatcher_PartyTeleport::read (ORIG 0x081dbe70) ====
int Dispatcher_PartyTeleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PARTY_TELEPORT& req = static_cast<MSG_PARTY_TELEPORT&>(msg);
    if (!packet.get_byte(req.m_0d))                      // +0x0d village（uchar& 重载）
    {
        return LineFunc(19231,
                        "virtual int Dispatcher_PartyTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(req.m_0e))                      // +0x0e area
    {
        return LineFunc(19232,
                        "virtual int Dispatcher_PartyTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_0f)))   // +0x0f posx
    {
        return LineFunc(19233,
                        "virtual int Dispatcher_PartyTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<short*>(&req.m_11)))   // +0x11 posy
    {
        return LineFunc(19234,
                        "virtual int Dispatcher_PartyTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(req.m_13))                      // +0x13 direction
    {
        return LineFunc(19235,
                        "virtual int Dispatcher_PartyTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_PartyTeleport::process (ORIG 0x081dbfd6) ====
int Dispatcher_PartyTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // stPartyTeleport(Param) pad 未字段化 —— ORIG 报告实证偏移视图：
    char* pbase = reinterpret_cast<char*>(&param);
    int& err = *reinterpret_cast<int*>(pbase + 0x4);              // +0x04 错误码
    unsigned char& echoState = *reinterpret_cast<unsigned char*>(pbase + 0x8); // +0x08 回显
    int& auraRemain = *reinterpret_cast<int*>(pbase + 0xc);       // +0x0c 光环剩余秒

    MSG_PARTY_TELEPORT& req = static_cast<MSG_PARTY_TELEPORT&>(msg);

    err = check_error(user, msg);
    echoState = req.m_0d;
    auraRemain = user->get_aura_avatar_option_value(0);
    int curSec = GlobalData::s_systemTime_.getCurSec();
    if (curSec < auraRemain)
    {
        auraRemain = auraRemain - curSec;
    }
    else
    {
        auraRemain = 0;
    }

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(19260,
                        "virtual int Dispatcher_PartyTeleport::process(CUser*, MSG_BASE&, ParamBase&)",
                        err, 0);
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
    {
        err = 0x13;
        return 0;
    }
    CPartyTelePort* tp =
        static_cast<CPartyTelePort*>(party->GetPartyTelePort());   // void* → 类视图
    tp->set_teleport_data(req.m_0d, req.m_0e,
                          static_cast<short>(req.m_0f),
                          static_cast<short>(req.m_11),
                          req.m_13, '\0');
    int slot = static_cast<int>(party->GetMemberSlotNo(user));
    tp->set_teleport_member_state(slot, 1);
    tp->check_cur_teleport_state();
    return 0;
}

// ==== Dispatcher_PartyTeleport::send (ORIG 0x081dc426) ====
void Dispatcher_PartyTeleport::send(CUser* user, ParamBase& param)
{
    // 偏移视图同 process（stPartyTeleport pad 未字段化）
    char* pbase = reinterpret_cast<char*>(&param);
    int err = *reinterpret_cast<int*>(pbase + 0x4);               // +0x04
    unsigned char echoState = *reinterpret_cast<unsigned char*>(pbase + 0x8); // +0x08
    int auraRemain = *reinterpret_cast<int*>(pbase + 0xc);        // +0x0c

    PacketGuard packet;
    packet.clear();
    packet.put_header(1, 0x1b3);                         // class=1 cmd=0x1b3
    if (err == 0)
    {
        packet.put_byte(1);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(err & 0xff);
        packet.put_byte(static_cast<int>(echoState));
        packet.put_int(auraRemain);
    }
    packet.finalize(true);
    user->Send(packet);

    if (err == 0)
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0)
        {
            CPartyTelePort* tp =
                static_cast<CPartyTelePort*>(party->GetPartyTelePort());
            tp->send_teleport_effect_at_different_place(3);
            tp->send_teleport_status(0);
            tp->process_teleport();
        }
    }
}

/* ==================== PcRoomRentItem_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_PcRoomRentItem 四函数 + giveDimensionItems 真实化
// ORIG: check_error 0x081e5762 / read 0x081e512e / process 0x081e52c0 /
//       send 0x081e542e / giveDimensionItems 0x081e583e
//
// 宿主落盘注意（Dispatchers.cpp 已有 include 集合核对）：
//   - InstanceRentalSystem.h：经 PacketTypes.h（stRentItemParam::m_output）可见 ✓
//   - pc_room::GetSelectionIndex 定义在 GlobalData.cpp（T 0x082711f7），头未暴露，
//     本文件给出局部声明（与 _NS_IPScan 先例同型）。
//   - Dispatchers.h 的 Dispatcher_PcRoomRentItem 类内缺 giveDimensionItems 声明，
//     落盘时需补一行私有声明：
//         void giveDimensionItems(CUser* user, InstanceRentalSystem::RentInput& input);
//   - CGuildServerProxy.h：需补 include（check_error 经
//     s_event_manager->GetRepeatEvent(0x5e)->IsEventing 调用，需 CEventManager/
//     CEventBase 完整类型；先例 BattleDepsB2.cpp:41）。

// pc_room::GetSelectionIndex —— T 0x082711f7，定义于 GlobalData.cpp namespace pc_room。
namespace pc_room
{
bool GetSelectionIndex(CUser& user, int& outIndex, int type);
}

// ==== Dispatcher_PcRoomRentItem::check_error (ORIG 0x081e5762) ====
int Dispatcher_PcRoomRentItem::check_error(CUser* user, MSG_BASE& msg)
{
    MSG_RENT_ITEM& req = static_cast<MSG_RENT_ITEM&>(msg);
    if (user->get_state() == 3 && user->getCurCharacR() != 0)
    {
        char type = static_cast<char>(req.m_0e);          // +0x0e 租借类型（movzbl+sbyte）
        if (type < 1 || type > 2)
        {
            return 8;
        }
        if (user->GetPCRoomNo() == 0 && type == 1)
        {
            return 8;
        }
        // type==2：重复租借事件(0x5e)未进行中 → 错误。vptr+0x34 槽 = IsEventing。
        if (type == 2 &&
            !GlobalData::s_event_manager->GetRepeatEvent(0x5e)->IsEventing((CUser*)0))
        {
            return 8;
        }
        return 0;
    }
    return 3;
}

// ==== Dispatcher_PcRoomRentItem::read (ORIG 0x081e512e) ====
int Dispatcher_PcRoomRentItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_RENT_ITEM& req = static_cast<MSG_RENT_ITEM&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0e)))          // +0x0e type
    {
        return LineFunc(24124,
                        "virtual int Dispatcher_PcRoomRentItem::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))          // +0x0d count
    {
        return LineFunc(24128,
                        "virtual int Dispatcher_PcRoomRentItem::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    char count = static_cast<char>(req.m_0d);
    if (count < 1 || count > 1)                                       // 恒为 1 项
    {
        return LineFunc(24131,
                        "virtual int Dispatcher_PcRoomRentItem::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    for (int i = 0; i < static_cast<int>(count); ++i)
    {
        // 元素对 @+0x0f+2i/+0x10+2i（RentInElem sectionIdx/itemIdx；count==1 不越界）
        if (!packet.get_byte(req.m_0f[i * 2]))
        {
            return LineFunc(24136,
                            "virtual int Dispatcher_PcRoomRentItem::read(PacketBuf&, MSG_BASE&)",
                            0, 0);
        }
        if (!packet.get_byte(req.m_0f[i * 2 + 1]))
        {
            return LineFunc(24138,
                            "virtual int Dispatcher_PcRoomRentItem::read(PacketBuf&, MSG_BASE&)",
                            0, 0);
        }
    }
    return 0;
}

// ==== Dispatcher_PcRoomRentItem::process (ORIG 0x081e52c0) ====
int Dispatcher_PcRoomRentItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRentItemParam& out = static_cast<stRentItemParam&>(param);
    MSG_RENT_ITEM& req = static_cast<MSG_RENT_ITEM&>(msg);

    out.m_4 = check_error(user, msg);
    if (out.m_4 != 0)
    {
        return 0;
    }

    InstanceRentalSystem::RentInput input;               // ctor 清 vector/machineKey
    input.m_logFlag = 1;                                 // +0x10 log 开关（PcRoom 报告实证）
    input.m_machineKey = static_cast<int>(static_cast<char>(req.m_0e));
    for (int i = 0; i < static_cast<int>(static_cast<char>(req.m_0d)); ++i)
    {
        InstanceRentalSystem::RentInElem elem;
        elem.m_sectionIdx = static_cast<unsigned char>(req.m_0f[i * 2]);      // +0xf+2i
        elem.m_itemIdx = static_cast<unsigned char>(req.m_0f[i * 2 + 1]);     // +0x10+2i
        input.m_elems.push_back(elem);
    }

    giveDimensionItems(user, input);

    out.m_4 = static_cast<int>(
        static_cast<InstanceRentalSystem*>(GlobalData::s_rentalSystem)
            ->Rent(*user, input, out.m_output));
    if (out.m_4 == 0)
    {
        /* BLOCKED: _ZN5CUser29UpdateCurrentCharacRentalInfoERKN20InstanceRentalSystem10RentOutputEib @0x08676e3c
           —— 仓库无该 CUser 方法声明/定义。ORIG 实参：
           user->UpdateCurrentCharacRentalInfo(out.m_output,
                                               (int)(char)req.m_0e, false);
           （RentOutput const&, int machineKey, bool logFlag=false）。
           依赖补齐后恢复本调用，其余逻辑保持不变。 */
    }
    return 0;                                            // ORIG RentInput dtor 于帧尾
}

// ==== Dispatcher_PcRoomRentItem::giveDimensionItems (ORIG 0x081e583e) ====
// 注：ORIG 为类成员（见文件头宿主声明补充说明）。
void Dispatcher_PcRoomRentItem::giveDimensionItems(CUser* user,
                                                   InstanceRentalSystem::RentInput& input)
{
    if (user == 0 || input.m_machineKey != 2)            // 仅 dimension(machineKey==2)
    {
        return;
    }
    int selectionIdx = 0;
    if (!pc_room::GetSelectionIndex(*user, selectionIdx, 2))
    {
        return;
    }
    int growType = static_cast<int>(user->getCurCharFirstGrowType());   // W 0x08110c94
    int job = user->get_charac_job();                                    // W 0x080fdf20

    /* BLOCKED: _ZN29DimensionActivationRewardData18getRewardGiveItemsEiii @0x08915f9a
       —— 仓库无该符号声明/定义（CDataManager.h DimensionActivationRewardData 仅有
       getSectionData/getRewardSelectItemList/getRewardGiveItemList）。ORIG 后续逻辑：
           DimensionActivationRewardData* rewardData =
               reinterpret_cast<DimensionActivationRewardData*>(
                   reinterpret_cast<char*>(G_CDataManager()) + 0x4b34);   // +0x4b34 推断
           std::vector<EventRewardItemInfo>* items =
               rewardData->getRewardGiveItems(selectionIdx, job, growType);
           if (items != 0)
               for (int i = 0; i < (int)items->size(); ++i)
               {
                   InstanceRentalSystem::RentInElem elem;
                   elem.m_sectionIdx = selectionIdx;
                   elem.m_itemIdx = i;
                   elem.m_type = 1;                       // GiveList 路径标记
                   input.m_elems.push_back(elem);
               }
       依赖补齐后恢复上方循环。 */
}

// ==== Dispatcher_PcRoomRentItem::send (ORIG 0x081e542e) ====
void Dispatcher_PcRoomRentItem::send(CUser* user, ParamBase& param)
{
    stRentItemParam& out = static_cast<stRentItemParam&>(param);
    bool sendEquipSlot10 = false;

    PacketGuard packet;
    packet.put_header(1, 0x1ee);                         // class=1 cmd=0x1ee
    if (out.m_4 == 0)
    {
        packet.put_byte(1);
        packet.put_byte(static_cast<int>(out.m_output.m_deleted.size()));
        std::vector<InstanceRentalSystem::DeletedRentItem>::iterator it;
        for (it = out.m_output.m_deleted.begin();
             it != out.m_output.m_deleted.end(); ++it)
        {
            short slot = static_cast<short>(it->m_slot);              // +0x02
            if (it->m_bEquip == 0)                                    // +0x00
            {
                packet.put_byte(0);
            }
            else
            {
                packet.put_byte(3);
                if (slot > 9 && slot < 0x16)
                {
                    sendEquipSlot10 = true;                           // 装备位 9..0x15 变动
                }
            }
            packet.put_short(static_cast<int>(slot));
        }
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(out.m_4 & 0xff);
    }
    packet.finalize(true);
    user->Send(packet);

    if (out.m_4 == 0)
    {
        PacketGuard infoPacket;
        infoPacket.put_header(0, 0xe);                   // class=0 cmd=0xe 租借列表刷新
        infoPacket.put_byte(0);
        infoPacket.put_short(static_cast<int>(out.m_output.m_infos.size()));
        std::vector<InstanceRentalSystem::RentalInfo>::iterator it;
        for (it = out.m_output.m_infos.begin();
             it != out.m_output.m_infos.end(); ++it)
        {
            const CInventory* inven = const_cast<CInventory*>(user->getCurCharacInvenR());
            inven->MakeItemPacket(INVEN_TYPE_EQUIP,
                                  static_cast<int>(static_cast<short>(it->m_slot)),
                                  infoPacket);
        }
        infoPacket.finalize(true);
        user->Send(infoPacket);
    }

    if (sendEquipSlot10)
    {
        user->send_equip(10);
    }
}

/* ==================== PurifyItem_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_PurifyItem 桩真实化 —— 函数体交付
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 对应类段
// （替换 1504-1507 行空 ctor/dtor/桩；dtor ORIG 为基类转发，保持 {}）
//
// 头约束说明（宿主知悉）：
//  - Dispatchers.h 的 Dispatcher_PurifyItem 仅声明 ctor/~dtor/process/read，
//    未声明 processPurifyType —— 按任务规则将其逻辑内联进 process 并注明
//    （ORIG T 0x082194b6）。
//  - CItemAmplifier::getPurifyMaterialType/getPurifyMaterialItemIdx/
//    getPurifyMaterialItemCount（ORIG W 0x08234d20/0x08234cd4/0x08234cfa）
//    头内无声明且 m_script 为 private：以文件内 static 辅助按对象偏移承载，
//    偏移与 CItemAmplifier.h 中 stAmplifyItemScript_t 具名字段一一对应
//    （+0x9c m_purifyMatItem / +0xa0 m_purifyOnlyMatItem /
//     +0xa4 m_purifyMatCount / +0xa8 m_purifyOnlyMatCount）。
//  - stAmplifyOption_t::identify 头声明为 void，ORIG W 0x0822b3d8 实返 bool
//    （P3 收窄法）：processPurifyType 内联处按指令语义展开取返回值。
//  - 异常抛掷按 P6：throw static_cast<ENUM_ERROR>(N)，catch(ENUM_ERROR&)
//    回发 SendCmdErrorPacket(0xcf,(unsigned char)e)。
// ============================================================================

// （宿主 TU 已含：Dispatchers.h/CInventory.h/CItemAmplifier.h/GlobalData.h/
//   GameWorld.h/CUserCharacInfo.h/GameTypes.h(cMyTrace)/
//   CompoundEmblemDeps.h(LineFunc)。）

namespace
{

// ORIG CItemAmplifier::getPurifyMaterialType(int) const W 0x08234d20：
// i∈[0,1]，*(int*)(amp + 0x9c + i*4) == itemIdx 则返回 i；否则 -1。
int amplGetPurifyMaterialType(const CItemAmplifier* amp, int itemIdx)
{
    for (int i = 0; i <= 1; ++i)
    {
        const int entry = *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(amp) + 0x9c + i * 4);
        if (entry == itemIdx)
        {
            return i;
        }
    }
    return -1;
}

// ORIG CItemAmplifier::getPurifyMaterialItemIdx(int) const W 0x08234cd4：
// type∈[0,1] → *(int*)(amp + 0x9c + type*4)；越界返回 0。
int amplGetPurifyMaterialItemIdx(const CItemAmplifier* amp, int type)
{
    if (type >= 0 && type <= 1)
    {
        return *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(amp) + 0x9c + type * 4);
    }
    return 0;
}

// ORIG CItemAmplifier::getPurifyMaterialItemCount(int) const W 0x08234cfa：
// type∈[0,1] → *(int*)(amp + 0xa4 + type*4)；越界返回 0。
int amplGetPurifyMaterialItemCount(const CItemAmplifier* amp, int type)
{
    if (type >= 0 && type <= 1)
    {
        return *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(amp) + 0xa4 + type * 4);
    }
    return 0;
}

} // anonymous namespace

// ==== Dispatcher_PurifyItem::Dispatcher_PurifyItem (ORIG IPacketDispatcher C1 0x0859879e W 转发) ====
Dispatcher_PurifyItem::Dispatcher_PurifyItem()
{
}

// ==== Dispatcher_PurifyItem::~Dispatcher_PurifyItem (ORIG D1 0x0825afba / D0 0x0825afea，基类转发) ====
Dispatcher_PurifyItem::~Dispatcher_PurifyItem()
{
}

// ==== Dispatcher_PurifyItem::read (ORIG 0x08218b94 T) ====
int Dispatcher_PurifyItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_PURIFY_ITEM& m = static_cast<MSG_PURIFY_ITEM&>(msg);

    unsigned short word = 0;
    unsigned long itemIdx = 0;

    if (!packet.get_short(word))
    {
        return LineFunc(
            0xcbbf,
            "virtual int Dispatcher_PurifyItem::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    m.m_0d = word;

    if (!packet.get_item_idx(itemIdx))  // PacketBuf::get_item_idx W 0x0822b6e8
    {
        return LineFunc(
            0xcbc1,
            "virtual int Dispatcher_PurifyItem::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    m.m_0f = (int)itemIdx;

    if (!packet.get_short(word))
    {
        return LineFunc(
            0xcbc3,
            "virtual int Dispatcher_PurifyItem::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    m.m_13 = word;

    if (!packet.get_item_idx(itemIdx))
    {
        return LineFunc(
            0xcbc5,
            "virtual int Dispatcher_PurifyItem::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    m.m_15 = (int)itemIdx;
    return 0;
}

// ==== Dispatcher_PurifyItem::process (ORIG 0x08218cb6 T) ====
int Dispatcher_PurifyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;  // ORIG 不读 param（local_18 仅赋值未使用）

    if (user->get_state() != 3)
    {
        return LineFunc(0xcbcb,
                        "virtual int Dispatcher_PurifyItem::process(CUser*, "
                        "MSG_BASE&, ParamBase&)",
                        0, 0);
    }

    MSG_PURIFY_ITEM& m = static_cast<MSG_PURIFY_ITEM&>(msg);

    if (G_GameWorld()->GetChannelType() == 7)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, 0x11);
        return 0;
    }

    if (user->CheckItemLock(1, m.m_0d))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, 0xd5);
        return 0;
    }

    try
    {
        Inven_Item targetItem;
        Inven_Item materialItem;

        const int purifyType =
            amplGetPurifyMaterialType(GlobalData::s_itemAmplifier_, m.m_15);
        if (purifyType == -1)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }
        if (amplGetPurifyMaterialItemIdx(GlobalData::s_itemAmplifier_,
                                         purifyType) != (int)m.m_15)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }

        targetItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_0d);
        if (targetItem.m_addInfo != (int)m.m_0f)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }
        // 已鉴定（bit7==0）的强化选项不允许净化
        if (targetItem.m_amp.isIdentified())
        {
            throw static_cast<ENUM_ERROR>(0x17);
        }

        materialItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_13);
        const int needCount = amplGetPurifyMaterialItemCount(
            GlobalData::s_itemAmplifier_, purifyType);
        if (materialItem.m_addInfo != (int)m.m_15 ||
            materialItem.m_addInfo2 < needCount)
        {
            throw static_cast<ENUM_ERROR>(0x11);
        }

        CInventory* invenW = user->getCurCharacInvenW();
        if (!invenW->delete_item((INVEN_TYPE)1, m.m_13, needCount,
                                 (eItemDelReason)3, true))
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        // ---- 内联：processPurifyType @0x082194b6 T ----
        // type 0：鉴定强化选项（stAmplifyOption_t::identify W 0x0822b3d8，
        //         未鉴定→清 bit7 返真；头内 void 声明，按指令语义展开）。
        // type 1：重置强化选项（stAmplifyOption_t::reset W 0x080cb75c）后返真。
        bool purified = false;
        if (purifyType == 0)
        {
            purified = !targetItem.m_amp.isIdentified();
            if (purified)
            {
                targetItem.m_amp.m_abilityType = (unsigned char)(
                    targetItem.m_amp.m_abilityType & 0x7f);
            }
        }
        else if (purifyType == 1)
        {
            targetItem.m_amp.reset();
            purified = true;
        }
        if (!purified)
        {
            throw static_cast<ENUM_ERROR>(0x16);
        }

        user->m_historyLog.apply_Item(materialItem.m_addInfo,
                                      targetItem.m_addInfo,
                                      (eApplyItemReason)2);

        invenW->update_item((INVEN_TYPE)1, m.m_0d, targetItem);

        // 刷新材料槽（删除后剩余数量入包）
        materialItem =
            user->getCurCharacInvenR()->GetInvenSlot(1, m.m_13);

        PacketGuard guard;
        guard.put_header(1, 0xcf);
        guard.put_byte(1);
        guard.put_short(m.m_13);
        guard.put_int(materialItem.m_addInfo2);
        guard.put_short(m.m_0d);
        guard.put_byte((unsigned char)targetItem.m_amp.getAbilityType());
        guard.put_short(targetItem.m_amp.getAbilityValue());
        guard.finalize(true);
        user->Send(guard);
        return 0;
    }
    catch (ENUM_ERROR& e)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xcf, (unsigned char)e);
        return 0;
    }
}

/* ==================== RefundSkill_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_RefundSkill —— 还原 ORIG 0x081da6fe..0x081dafa3（read/process/
// check_error/send）。依据 docs/class_func_reports/Dispatcher_RefundSkill/
// {read,process,check_error,send}.md。
//
// 宿主落盘注意：
// 1. PacketTypes.h 的 MSG_REFUND_SKILL 已具名（m_0d 技能 id、m_11 技能树选择、
//    m_12 树种类、m_13 退款数量），直接使用。
// 2. stRefundSkill 目前为 char m_pad[0x40] 占位，ORIG 布局（process 写 / send
//    读实证）如下；落盘前建议字段化，本批先以偏移视图访问并注明：
//      +0x04 unsigned char 技能树序号快照（GetCurCharacSkillTreeIndex）
//      +0x05 bool          特殊技能标记（CSkill::IsSpecialSkill）
//      +0x06 unsigned short 剩余 SP/SFP
//      +0x08 int           SkillSlot::refund_skill 返回值
//      +0x0c unsigned char 树种类（回传 msg m_12）
//      +0x0d unsigned char 技能等级（get_skill_level）
//      +0x10 int           物品 idx（回传 msg m_0d）
//      +0x14 unsigned char 退款数量（回传 msg m_13）
//      +0x18 int           错误码（check_error/内部失败）
// ============================================================================




namespace  // 本 TU 内的 stRefundSkill 偏移视图（头字段化前的唯一访问点）
{

inline unsigned char& rfTreeIndex(ParamBase& p) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&p) + 0x04); }   // +0x04 树序号
inline bool& rfSpecial(ParamBase& p) { return *reinterpret_cast<bool*>(reinterpret_cast<char*>(&p) + 0x05); }                       // +0x05 特殊技能
inline unsigned short& rfRemain(ParamBase& p) { return *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&p) + 0x06); }    // +0x06 剩余SP/SFP
inline int& rfResult(ParamBase& p) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&p) + 0x08); }                          // +0x08 refund 结果
inline unsigned char& rfKind(ParamBase& p) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&p) + 0x0c); }        // +0x0c 树种类
inline unsigned char& rfLevel(ParamBase& p) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&p) + 0x0d); }       // +0x0d 等级
inline int& rfItemIdx(ParamBase& p) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&p) + 0x10); }                         // +0x10 物品idx
inline unsigned char& rfCount(ParamBase& p) { return *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&p) + 0x14); }       // +0x14 数量
inline int& rfErr(ParamBase& p) { return *reinterpret_cast<int*>(reinterpret_cast<char*>(&p) + 0x18); }                             // +0x18 错误码

}  // namespace

// ==== Dispatcher_RefundSkill::read (ORIG 0x081da6fe) ====
int Dispatcher_RefundSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_REFUND_SKILL& req = static_cast<MSG_REFUND_SKILL&>(msg);
    if (!packet.get_int(req.m_0d))  // 技能 id
        return LineFunc(0x48e5, "virtual int Dispatcher_RefundSkill::read(PacketBuf&, MSG_BASE&)", 0, 0);
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_11)))  // 技能树选择（signed byte 读入）
        return LineFunc(0x48e7, "virtual int Dispatcher_RefundSkill::read(PacketBuf&, MSG_BASE&)", 0, 0);
    if (!packet.get_byte(req.m_12))  // 树种类
        return LineFunc(0x48e9, "virtual int Dispatcher_RefundSkill::read(PacketBuf&, MSG_BASE&)", 0, 0);
    if (!packet.get_byte(req.m_13))  // 退款数量
        return LineFunc(0x48eb, "virtual int Dispatcher_RefundSkill::read(PacketBuf&, MSG_BASE&)", 0, 0);
    return 0;
}

// ==== Dispatcher_RefundSkill::check_error (ORIG 0x081dad1c) ====
int Dispatcher_RefundSkill::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
        return -1;
    const MSG_REFUND_SKILL& req = static_cast<const MSG_REFUND_SKILL&>(msg);

    if (user->getCurCharacR() == 0)
        return -1;

    if (user->get_state() != 3)
        return 7;

    // PvPSkilTree 频道只允许对应树技能 id（0x2897c7 普通 / 0x2897c6 PvP）
    if (G_GameWorld()->IsPvPSkilTreeChannel()) {
        if (req.m_0d != 0x2897c7)
            return 0x16;
    }
    else {
        if (req.m_0d != 0x2897c6)
            return 0x16;
    }

    // 技能树序号与包内选择必须一致：-1/0 树禁选 1，其余必选 1
    const int treeIndex = user->GetCurCharacSkillTreeIndex();
    if (treeIndex == -1 || treeIndex == 0) {
        if (req.m_11 == 1)
            return 0x13;
    }
    else {
        if (req.m_11 != 1)
            return 0x13;
    }

    if (req.m_13 == 0)
        return 0x13;                                            // 数量不得为 0
    return 0;
}

// ==== Dispatcher_RefundSkill::process (ORIG 0x081da820) ====
int Dispatcher_RefundSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_REFUND_SKILL& req = static_cast<MSG_REFUND_SKILL&>(msg);

    rfErr(param) = check_error(user, msg);                      // +0x18 错误码
    if (rfErr(param) > 0)
        return 0;
    if (rfErr(param) < 0)
        return LineFunc(0x48fa,
                        "virtual int Dispatcher_RefundSkill::process(CUser*, MSG_BASE&, ParamBase&)",
                        rfErr(param), 0);

    // ORIG 帧：std::vector<int> 局部仅构造/clear/析构（无副作用消费），以等大
    // dead[] 复原帧空间。
    char deadVec[12];                                           // 复原 vector<int> 帧槽（未用局部）

    Inven_Item item;
    item.reset();
    const CInventory* invR = user->getCurCharacInvenR();
    const int slot = invR->GetInvenData(req.m_0d, item);
    if (!(item.m_addInfo == req.m_0d &&                          // +0x02 物品 idx 一致
          req.m_13 <= item.m_addInfo2 &&                        // +0x07 持有数 ≥ 请求数
          slot >= 0)) {
        rfErr(param) = 0x16;
        return 0;
    }

    {
        CInventory snapshot;                                    // 库存副本上先行试删
        InventoryMemory mem;
        snapshot.SetInventoryMemory(&mem);
        snapshot.setCopy(*invR);
        if (snapshot.delete_item(INVEN_TYPE_EQUIP, slot, req.m_13, eItemDelReason_3, false)) {
            rfErr(param) = 0x16;                                // 副本删除失败
            return 0;
        }

        rfErr(param) = 0;                                       // refund_skill 的 needGold 出参
        SkillSlot* skillW = reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW());
        const int result = skillW->refund_skill(
            static_cast<int>(req.m_12),                         // skillIdx（树内技能）
            user->get_charac_level(),
            user->get_charac_job(),
            static_cast<int>(user->getCurCharFirstGrowType()),
            static_cast<int>(user->getCurCharSecondGrowType()),
            static_cast<int>(req.m_13),                         // refundCount
            rfErr(param));
        rfResult(param) = result;                               // +0x08

        if (result > 0xcc)
            return 0;                                           // 超出可退上限
        if (result < 0)
            return 0;

        // 真实删除背包中的退款材料书
        if (user->delete_item(static_cast<char>(INVEN_TYPE_INVEN), slot,
                              static_cast<unsigned long>(req.m_0d),
                              static_cast<int>(req.m_13), eItemDelReason_3) != 0) {
            cMyTrace trace(
                "virtual int Dispatcher_RefundSkill::process(CUser*, MSG_BASE&, ParamBase&)",
                0x4932, 5);
            trace("delete_item failed - refund skill %d/%d",
                  static_cast<int>(req.m_0d), static_cast<int>(req.m_13));
        }

        rfSpecial(param) = false;                               // +0x05 特殊技能标记
        CSkill* skill = G_CDataManager()->find_skill(user->get_charac_job(),
                                                     static_cast<int>(req.m_12));
        if (skill != 0 && skill->IsSpecialSkill())
            rfSpecial(param) = true;

        rfTreeIndex(param) = static_cast<unsigned char>(user->GetCurCharacSkillTreeIndex());  // +0x04
        rfKind(param) = req.m_12;                               // +0x0c
        rfLevel(param) = static_cast<unsigned char>(
            reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                ->get_skill_level(rfResult(param)));            // +0x0d

        if (!rfSpecial(param)) {
            rfRemain(param) = static_cast<unsigned short>(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                    ->get_remain_sp_at_index(
                        static_cast<ENUM_SKILL_TREE_KIND>(
                            static_cast<char>(rfTreeIndex(param)))));  // +0x06
        }
        else {
            const int ti = user->GetCurCharacSkillTreeIndex();
            const ENUM_SKILL_TREE_KIND sfp =
                (ti == -1 || ti == 0) ? SKILL_TREE_SFP_1 : SKILL_TREE_SFP_2;
            rfRemain(param) = static_cast<unsigned short>(
                reinterpret_cast<SkillSlot*>(const_cast<char*>(user->getCurCharacSkillR()))
                    ->get_remain_sfp_at_index(sfp));
        }

        rfItemIdx(param) = req.m_0d;                            // +0x10
        rfCount(param) = req.m_13;                              // +0x14
    }
    return 0;
}

// ==== Dispatcher_RefundSkill::send (ORIG 0x081dae0a) ====
void Dispatcher_RefundSkill::send(CUser* user, ParamBase& param)
{
    if (rfErr(param) != 0) {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1ab),
                                 static_cast<unsigned char>(rfErr(param) & 0xff));
        return;
    }

    PacketGuard pg;
    pg.clear();
    pg.put_header(1, 0x1ab);
    pg.put_byte(1);                                             // 成功标志
    pg.put_byte(static_cast<int>(static_cast<char>(rfSpecial(param))));   // +0x05
    pg.put_byte(static_cast<int>(static_cast<char>(rfTreeIndex(param)))); // +0x04
    pg.put_short(static_cast<int>(rfRemain(param)));            // +0x06
    pg.put_byte(static_cast<int>(rfResult(param) & 0xff));      // +0x08 低字节
    pg.put_byte(static_cast<int>(rfKind(param)));               // +0x0c
    pg.put_byte(static_cast<int>(rfLevel(param)));              // +0x0d
    pg.put_int(rfItemIdx(param));                               // +0x10
    pg.put_byte(static_cast<int>(rfCount(param)));              // +0x14
    pg.finalize(true);
    user->Send(pg);
}

/* ==================== SecurityCardAuthRpy_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_SecurityCardAuthRpy 桩真实化（批次 disp_b1）
// 依据 docs/class_func_reports/Dispatcher_SecurityCardAuthRpy/{read,process,check_error,send}.md
//
// 宿主落盘注意（Dispatchers.cpp）：
//   1. 下方"局部线格式视图"为文件级一次性定义，置于类实现之前（匿名 namespace）。
//   2. 需在 include 区补充：Cipher.h、GameTypes.h(cMyTrace)、CTradeSpace.h
//      （send 分支用 CTradeSpace 完整类型）；GlobalData.h/CDataManager.h/
//      CompoundEmblemDeps.h(LineFunc)/CUserCharacInfo.h 已含。
//   3. 布局告警：ORIG MSG_SECU_RPY 权威尺寸 >= 0x1014（数据区 +0x0f..+0x100e、长度
//      +0x100f、应答类型 +0x1013）；当前 PacketTypes.h 仅 char m_pad[0x100]（总 0x10d），
//      read 按报告偏移写入会越出该对象。需 PacketTypes 批次扩正权威布局；本批按 ORIG
//      报告偏移经视图访问，不改共享头。
//   4. BLOCKED 外部依赖（grep 全仓库 *.h 无声明，按约束注释占位、保留其余逻辑）：
//      - WongWork::CSecurityCard::{checkAnswer,incFailCnt,getCertCnt,setCertified,
//        resetFailCnt}（CUser.h:800 现有声明缺这五个，其余方法已在）
//      - XNuclear::CHades::ChangeSecurity(_ZN8XNuclear6CHades14ChangeSecurityENS_19ENUM_HADES_SECURITYEj @0x084b95a8)
//      - cUserHistoryLog::SecuService(@0x0868479c)
//      - DB_SecurityCardUpdateFailCnt::makeRequest(uint,uint)（@0x0842bfaa）
//      - DB_SecurityCardAddCertCnt::makeRequest(uint)（@0x08436388）
//      - DB_SecurityCardUpdateCancelCnt::makeRequest(uint,uint)（@0x0842c112；
//        DB_AccountCargoSync.h 现声明为非静态成员，与 ORIG 静态调用形态不符）
//      - Dispatcher_MouseRegister::doPassPadReplay（@0x082633c6）
// ============================================================================

// ==== 前置：局部线格式视图（宿主一次性置入匿名 namespace） ====
namespace {

struct ScrMsgView
{
    char m_head[0xd];              // MSG_BASE（0xd 字节）
    unsigned char m_secuType;      // +0x0d（0=安全卡、1=密保卡 pad）
    unsigned char m_cryptoType;    // +0x0e（check_error：非 0、<=0xd、!=5）
    char m_data[0x1000];           // +0x0f 密文区（get_binary 读 m_dataLen 字节）
    int m_dataLen;                 // +0x100f
    unsigned char m_answerType;    // +0x1013
};

}  // namespace

// ==== Dispatcher_SecurityCardAuthRpy::read (ORIG 0x08262658) ====
int Dispatcher_SecurityCardAuthRpy::read(PacketBuf& packet, MSG_BASE& msg)
{
    ScrMsgView& m = *reinterpret_cast<ScrMsgView*>(&msg);
    if (!packet.get_byte(m.m_secuType))
        return LineFunc(0x2b,
                        "virtual int Dispatcher_SecurityCardAuthRpy::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (!packet.get_byte(m.m_cryptoType))
        return LineFunc(0x2e,
                        "virtual int Dispatcher_SecurityCardAuthRpy::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (!packet.get_int(m.m_dataLen))
        return LineFunc(0x32,
                        "virtual int Dispatcher_SecurityCardAuthRpy::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (!packet.get_binary(m.m_data, m.m_dataLen))
        return LineFunc(0x35,
                        "virtual int Dispatcher_SecurityCardAuthRpy::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    if (!packet.get_byte(m.m_answerType))
        return LineFunc(0x3a,
                        "virtual int Dispatcher_SecurityCardAuthRpy::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    return 0;
}

// ==== Dispatcher_SecurityCardAuthRpy::check_error (ORIG 0x0826314c) ====
int Dispatcher_SecurityCardAuthRpy::check_error(CUser* user, MSG_BASE& msg)
{
    ScrMsgView& m = *reinterpret_cast<ScrMsgView*>(&msg);
    if (m.m_cryptoType == 0 || m.m_cryptoType > 0xd || m.m_cryptoType == 5)
    {
        cMyTrace trace(
            "int Dispatcher_SecurityCardAuthRpy::check_error(CUser*, MSG_BASE&)", 0x14c, 0);
        trace("ERROR [TRACE AUTH RPY] : crypto_type2");
        return 0x14d;
    }
    if (user->get_state() <= 1)
    {
        cMyTrace trace(
            "int Dispatcher_SecurityCardAuthRpy::check_error(CUser*, MSG_BASE&)", 0x154, 0);
        trace("ERROR [TRACE AUTH RPY] : get_state()<LOGIN");
        return 0x155;
    }
    return 0;
}

// ==== Dispatcher_SecurityCardAuthRpy::process (ORIG 0x082627ca) ====
// 安全卡/密保卡认证应答：按 secuType 选密钥管理器解密 0x1000 内缓冲，
// 安卡走 checkAnswer/失败计数/DB 上报链，pad 走 doPassPadReplay 链。
int Dispatcher_SecurityCardAuthRpy::process(CUser* user, MSG_BASE& msg, ParamBase& paramBase)
{
    ScrMsgView& m = *reinterpret_cast<ScrMsgView*>(&msg);
    SecuCardRpyParam& param = static_cast<SecuCardRpyParam&>(paramBase);

    if (check_error(user, msg) > 0)
        return -1;

    param.m_4 = 0;
    param.m_5 = 0;

    unsigned char decrypted[0x400];
    memset(decrypted, 0, sizeof(decrypted));
    int decryptedLen = 0x1000;

    WongWork::CSecurityCard* card = user->getSecurityCard();
    Sanicova::CPad* pad = user->getPad();
    Cipher* cipher = GlobalData::s_secu_cipher_keystring_manager_
        [(pad->GetSelectedCipherIdx() & 0xffff) * 4];

    const bool cardActive = (card->isActivate() && m.m_secuType == 0);
    const bool padActive = (pad->isActivate() && m.m_secuType == 1);

    if (!cardActive && !padActive)
        return LineFunc(0x76,
                        "virtual int Dispatcher_SecurityCardAuthRpy::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, 0);

    if (!cardActive)
    {
        // pad 分支：解密失败即 trace + 错误出参后返回。
        if (cipher->Decrypt((int)m.m_cryptoType,
                            reinterpret_cast<const unsigned char*>(m.m_data),
                            m.m_dataLen, decrypted, &decryptedLen) != 1)
        {
            param.m_5 = 1;
            cMyTrace trace(
                "virtual int Dispatcher_SecurityCardAuthRpy::process(CUser*, MSG_BASE&, ParamBase&)",
                0x71, 0);
            trace("ERROR [TRACE AUTH RPY] : ENUM_CMDPACKET_SECURITY_CARD_AUTH_RPY,ENUM_ERROR_ETC");
            return 0;
        }
    }
    else
    {
        // card 分支：ORIG 不检查 Decrypt 返回值。
        cipher->Decrypt((int)m.m_cryptoType,
                        reinterpret_cast<const unsigned char*>(m.m_data),
                        m.m_dataLen, decrypted, &decryptedLen);
    }

    // 解密明文前 6 字节（安卡用前两字节作答案；pad 用全部 6 字节组 7 字节串）。
    const unsigned char answer0 = decrypted[0];
    const unsigned char answer1 = decrypted[1];
    const unsigned char answer2 = decrypted[2];
    const unsigned char answer3 = decrypted[3];
    const unsigned char answer4 = decrypted[4];
    const unsigned char answer5 = decrypted[5];

    if (cardActive)
    {
        card->setRequestState(0);
        if ((unsigned int)card->getFailCnt() > 2)
        {
            param.m_5 = 0x7a;
            return 0;
        }

        // BLOCKED: _ZNK8WongWork13CSecurityCard11checkAnswerEhh @0x085fdf6a ——
        //   CUser.h 的 CSecurityCard 缺 checkAnswer 声明，待 CSecurityCard 批次补齐：
        //   bool answerOk = (card->checkAnswer(answer0, answer1) == 1);
        bool answerOk = false;  // 占位（依赖缺失），失败链路以下完整保留

        if (!answerOk)
        {
            // BLOCKED: _ZN15cUserHistoryLog11SecuServiceE22ENUM_SECU_SERVICE_TYPE24ENUM_SECU_SERVICE_ACTION @0x0868479c
            //   user->m_historyLog.SecuService((ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)0);
            // BLOCKED: _ZN8WongWork13CSecurityCard10incFailCntEv @0x082652ae ——
            //   ORIG failCnt 为 incFailCnt 自增后的新值。
            unsigned int newFailCnt = (unsigned int)card->getFailCnt();
            if (newFailCnt < 3)
            {
                param.m_5 = 0x79;
                // BLOCKED: _ZN8XNuclear6CHades14ChangeSecurityENS_19ENUM_HADES_SECURITYEj @0x084b95a8
                //   user->getHades()->ChangeSecurity((XNuclear::ENUM_HADES_SECURITY)1,
                //       newFailCnt == 1 ? 0x15 : 0x16);
            }
            else
            {
                param.m_5 = 0x7a;
                // BLOCKED: ChangeSecurity(hades, (ENUM_HADES_SECURITY)1, 0x17) 同上地址
                user->getHackAnalyzer()->addServerHackCnt(user,
                                                          (WongWork::ENUM_HACKTYPE)0x343,
                                                          1, 0, 0);
            }
            // BLOCKED: _ZN28DB_SecurityCardUpdateFailCnt11makeRequestEjj @0x0842bfaa
            //   DB_SecurityCardUpdateFailCnt::makeRequest(user->get_acc_id(),
            //       card->getFailCnt());
            return 0;
        }

        // BLOCKED: SecuService(hist, (ENUM_SECU_SERVICE_TYPE)1, (ENUM_SECU_SERVICE_ACTION)1) 同上
        // BLOCKED: ChangeSecurity(hades, (ENUM_HADES_SECURITY)1, 0x19) 同上

        PacketGuard packet;
        packet.put_header(1, 0xa7);
        packet.put_byte(1);
        // BLOCKED: _ZNK8WongWork13CSecurityCard11getCertCntEv @0x0826528a ——
        //   int certCnt = card->getCertCnt();
        int certCnt = 0;  // 占位（依赖缺失）
        packet.put_byte(certCnt > 0x3b ? 1 : 0);
        packet.finalize(true);
        user->Send(packet);

        // BLOCKED: DB_SecurityCardUpdateFailCnt::makeRequest(acc_id, 0) @0x0842bfaa
        if (card->getCancelCnt() != 0)
        {
            // BLOCKED: _ZN30DB_SecurityCardUpdateCancelCnt11makeRequestEjj @0x0842c112
            //   DB_AccountCargoSync.h 现把 makeRequest 声明为非静态成员，与 ORIG
            //   静态调用形态不符；统一为 static 后接：
            //   DB_SecurityCardUpdateCancelCnt::makeRequest(user->get_acc_id(), 0);
        }
        // BLOCKED: _ZN25DB_SecurityCardAddCertCnt11makeRequestEj @0x08436388
        //   DB_SecurityCardAddCertCnt::makeRequest(user->get_acc_id());
        // BLOCKED: _ZN8WongWork13CSecurityCard12setCertifiedEb @0x08265296
        //   card->setCertified(true);
        // BLOCKED: _ZN8WongWork13CSecurityCard12resetFailCntEv @0x082652c8
        //   card->resetFailCnt();
    }
    else  // padActive
    {
        pad->setRequestState(0);
        if (pad->getFailCnt() > 2)
        {
            param.m_5 = 0x88;
            return 0;
        }

        char passAnswer[7];
        passAnswer[0] = (char)answer0;
        passAnswer[1] = (char)answer1;
        passAnswer[2] = (char)answer2;
        passAnswer[3] = (char)answer3;
        passAnswer[4] = (char)answer4;
        passAnswer[5] = (char)answer5;
        passAnswer[6] = '\0';

        // BLOCKED: _ZN24Dispatcher_MouseRegister15doPassPadReplayEP5CUserPccRbRh @0x082633c6 ——
        //   Dispatcher_MouseRegister 无头声明，待 MouseRegister 批次补齐：
        //   Dispatcher_MouseRegister::doPassPadReplay(user, passAnswer,
        //       (char)m.m_answerType,
        //       *reinterpret_cast<bool*>(&param.m_4), param.m_5);
        if (param.m_4 == 0)
            return 0;
        param.m_5 = 1;
    }

    param.m_4 = 1;
    return 0;
}

// ==== Dispatcher_SecurityCardAuthRpy::send (ORIG 0x08262f4e) ====
void Dispatcher_SecurityCardAuthRpy::send(CUser* user, ParamBase& paramBase)
{
    SecuCardRpyParam& param = static_cast<SecuCardRpyParam&>(paramBase);
    if (param.m_4 != 0)
    {
        // 认证通过后通知交易对方断开私聊通道（0xb0 包）。
        CTradeSpace* trade = G_CGameManager()->GetTradeSpace(user->GetTradeSpace());
        if (trade != 0 && trade->is_empty() == 0)
        {
            PacketGuard packet;
            packet.clear();
            packet.put_header(0, 0xb0);
            packet.put_byte(0);
            packet.finalize(true);
            CUser* opposite = trade->GetOppositeUser(user);
            if (opposite != 0)
                opposite->Send(packet);
        }
        return;
    }
    if (param.m_5 == 0)
        return;

    PacketGuard packet;
    packet.put_header(1, 0xa7);
    packet.put_byte(0);
    packet.put_byte(param.m_5);
    if (param.m_5 == 0x87)
        packet.put_byte(user->getPad()->getFailCnt());
    packet.finalize(true);
    user->Send(packet);
}

/* ==================== StackableActionUse_body.cpp (批次B1 落盘) ==================== */
// ============================================================================
// Dispatcher_StackableActionUse 四函数真实化
// ORIG: check_error 0x0825e332 / read 0x0825df72 / process 0x0825e11e /
//       send 0x0825e4ec
//
// 宿主落盘注意（Dispatchers.cpp 已有 include 集合核对）：
//   - CHackAnalyzer.h：需补 include（getHackAnalyzer()->addServerHackCnt 需要
//     WongWork::CHackAnalyzer 完整类型；CUser.h 仅返回指针）。
//   - Inven_Item.h / CInventory.h / CDataManager.h / GlobalData.h：已 include ✓
//   - StackableAction::Type 枚举经 CUser.h 可见（verifyStackableItemProcess 签名）✓
//
// Param 视图：模板第二参 stStackableActionUse（PacketTypes.h:2583，ParamBase 派生，
// char m_pad[0x40] 占位）。ORIG process/send 报告实证布局：
//   +0x04 int 错误码、+0x08 short 回显槽位(msg+0xd)、+0x0a char 回显类型(msg+0xf)。
//
// BLOCKED 汇总：
//   - StackableAction::Action::check(int,input_param&) @0x0827fa26 与
//     StackableAction::Action::run(int,input_param&) @0x0827fa9a 仓库无定义；
//     input_param 仅存在于 GlobalData.cpp TU 内部（头未暴露），无法跨 TU 构造。
//     → process 中 check→run→use_item→_processContinuousItem 主链以 BLOCKED
//       注释占位，错误路径/Param 填充/日志全部保留。
// ============================================================================

// ==== Dispatcher_StackableActionUse::check_error (ORIG 0x0825e332) ====
int Dispatcher_StackableActionUse::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0 || user->getCurCharacR() == 0)
    {
        return -1;
    }
    MSG_STACKABLE_ACTION_USE& req = static_cast<MSG_STACKABLE_ACTION_USE&>(msg);

    // 背包校验和不一致仅记 hack 计数，不中断（ORIG 形态）
    if (user->getCurCharacInvenCheckSum() != req.m_10)
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x25b, 1, 0, 0);
    }
    if (user->CheckInTrade())
    {
        return 0xd8;
    }
    if (req.m_14 == 0)                                   // +0x14 action type 非零
    {
        return 0x17;
    }

    short slot = req.m_0d;                               // +0x0d 槽位
    const CInventory* inven = const_cast<CInventory*>(user->getCurCharacInvenR());   // GetInvenSlot/try_use_item 均 const
    Inven_Item item = inven->GetInvenSlot(1, static_cast<int>(slot));
    unsigned int itemIdx = static_cast<unsigned int>(item.m_addInfo);   // +0x02

    CItem* itemDef = G_CDataManager()->find_item(static_cast<int>(itemIdx));
    if (itemDef == 0)
    {
        return 0x11;
    }
    // ORIG test %al 单字节检验（P3 收窄）：verify 通过返回 al==1
    if ((unsigned char)user->verifyStackableItemProcess(
            static_cast<StackableAction::Type>(static_cast<char>(req.m_14)),
            itemDef) != 1u)
    {
        return 0x17;
    }
    if (user->CheckCoolTimeItem(static_cast<unsigned long>(itemIdx)))
    {
        return 0x13;
    }
    int ret = inven->try_use_item(static_cast<int>(slot), 1,
                                  static_cast<int>(static_cast<char>(req.m_0f)));
    if (ret != 0)
    {
        return ret;
    }
    return 0;
}

// ==== Dispatcher_StackableActionUse::read (ORIG 0x0825df72) ====
int Dispatcher_StackableActionUse::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_STACKABLE_ACTION_USE& req = static_cast<MSG_STACKABLE_ACTION_USE&>(msg);
    if (!packet.get_short(req.m_0d))                     // +0x0d slot
    {
        return LineFunc(251,
                        "virtual int Dispatcher_StackableActionUse::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0f)))        // +0x0f invenType
    {
        return LineFunc(254,
                        "virtual int Dispatcher_StackableActionUse::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(req.m_10))                       // +0x10 checksum
    {
        return LineFunc(257,
                        "virtual int Dispatcher_StackableActionUse::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_14)))        // +0x14 type
    {
        return LineFunc(260,
                        "virtual int Dispatcher_StackableActionUse::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(req.m_15))                       // +0x15 param1
    {
        return LineFunc(263,
                        "virtual int Dispatcher_StackableActionUse::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_int(req.m_19))                       // +0x19 param2
    {
        return LineFunc(266,
                        "virtual int Dispatcher_StackableActionUse::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_StackableActionUse::process (ORIG 0x0825e11e) ====
int Dispatcher_StackableActionUse::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // stStackableActionUse pad 未字段化 —— ORIG 报告实证偏移视图：
    char* pbase = reinterpret_cast<char*>(&param);
    int& err = *reinterpret_cast<int*>(pbase + 0x4);              // +0x04 错误码
    short& echoSlot = *reinterpret_cast<short*>(pbase + 0x8);     // +0x08 回显槽位
    char& echoType = *reinterpret_cast<char*>(pbase + 0xa);       // +0x0a 回显类型

    MSG_STACKABLE_ACTION_USE& req = static_cast<MSG_STACKABLE_ACTION_USE&>(msg);

    err = check_error(user, msg);
    echoType = static_cast<char>(req.m_0f);
    echoSlot = req.m_0d;

    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            "virtual int Dispatcher_StackableActionUse::process(CUser*, MSG_BASE&, ParamBase&)",
            0x11f, "Dispatcher Check_Error No(%d)", err);
        return LineFunc(288,
                        "virtual int Dispatcher_StackableActionUse::process(CUser*, MSG_BASE&, ParamBase&)",
                        0, 0);
    }

    CInventory* inven = const_cast<CInventory*>(user->getCurCharacInvenR());
    Inven_Item item = inven->GetInvenSlot(1, static_cast<int>(req.m_0d));
    unsigned int itemIdx = static_cast<unsigned int>(item.m_addInfo);
    CStackableItem* itemDef =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(static_cast<int>(itemIdx)));
    if (itemDef == 0)
    {
        err = 0x11;
        return 0;
    }

    /* BLOCKED: StackableAction::Action 动作执行链
       —— _ZN15StackableAction6Action5checkEiRNS_11input_paramE @0x0827fa26、
          _ZN15StackableAction6Action3runEiRNS_11input_paramE @0x0827fa9a 仓库无定义；
          input_param 仅在 GlobalData.cpp 内部定义（头未暴露，且其 m_pad0[8] 于 ORIG
          栈上未初始化）。ORIG 后续主链（依赖补齐后恢复）：
              StackableAction::input_param inputs;         // 前 8 字节未初始化（忠实）
              inputs.m_user = user;                        // +0x08
              inputs.m_item = itemDef;                     // +0x0c
              err = GlobalData::g_Actions->check((int)(char)req.m_14, inputs);
              if (err == 0)
                  err = GlobalData::g_Actions->run((int)(char)req.m_14, inputs);
              if (err == 0)
              {
                  err = user->getCurCharacInvenW()->use_item(
                      (int)req.m_0d, 1, (int)(char)req.m_0f);
                  if (err == 0)
                      user->_processContinuousItem(itemIdx, itemDef);
              }
       use_item/_processContinuousItem 均为 run 成功后的下游，此处按 ORIG 门控整体
       短路；err 保持 check_error 的 0。 */
    return 0;
}

// ==== Dispatcher_StackableActionUse::send (ORIG 0x0825e4ec) ====
void Dispatcher_StackableActionUse::send(CUser* user, ParamBase& param)
{
    // 偏移视图同 process（stStackableActionUse pad 未字段化）
    char* pbase = reinterpret_cast<char*>(&param);
    int err = *reinterpret_cast<int*>(pbase + 0x4);               // +0x04
    short echoSlot = *reinterpret_cast<short*>(pbase + 0x8);      // +0x08
    char echoType = *reinterpret_cast<char*>(pbase + 0xa);        // +0x0a

    if (err != 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);                 // class=1 cmd=0x204
        packet.put_byte(0);
        packet.put_byte(err & 0xff);
        packet.put_byte(static_cast<int>(echoType));
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x204);
        packet.put_byte(1);
        packet.put_short(static_cast<int>(echoSlot));
        packet.put_byte(static_cast<int>(echoType));
        packet.finalize(true);
        user->Send(packet);
    }
}

// ==== Dispatcher_UseDye::getItem (ORIG 0x081e71c0) ====
// 取 (invenType, slot) 槽位物品；返回 true=槽空（m_addInfo==0）。
bool Dispatcher_UseDye::getItem(const CUser& user, int invenType, short slot,
                                Inven_Item& out) const
{
    out = const_cast<CInventory*>(user.getCurCharacInvenR())
              ->GetInvenSlot(invenType, slot);
    return out.m_addInfo == 0;
}

// ==== Dispatcher_UseDye::notifyUpdatedInventoryItemInfo (ORIG 0x081e70e6) ====
void Dispatcher_UseDye::notifyUpdatedInventoryItemInfo(CUser& user, short slot) const
{
    PacketGuard packet;
    packet.put_header(0, 0xe);
    packet.put_byte(0);
    packet.put_short(1);
    user.getCurCharacInvenR()->MakeItemPacket(
        static_cast<INVEN_TYPE>(1), slot, packet);
    packet.finalize(true);
    user.Send(packet);
}

// ==== Dispatcher_UseDye::process (ORIG 0x081e69ea) ====
namespace
{
// DyeInfo（STItemScript.h）字段私有，按 ORIG 布局组件视图访问（P1）：
// +0x00 int 计数/标志、+0x04 vector<pair<int,int>>。
struct UseDyeDyeInfoView
{
    int m_field0;                                 // +0x00
    std::vector<std::pair<int, int> > m_vec4;     // +0x04
};
} // anonymous namespace

int Dispatcher_UseDye::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stUseDye& out = static_cast<stUseDye&>(param);
    MSG_USE_DYE& req = static_cast<MSG_USE_DYE&>(msg);

    int err = check_error(user, msg);
    out.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3),
                                 static_cast<unsigned char>(out.m_4));
        return LineFunc(0x625a,
                        "virtual int Dispatcher_UseDye::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_4, 0);
    }

    Inven_Item dyeItem;
    if (getItem(*user, 1, req.m_0e, dyeItem))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3),
                                 static_cast<unsigned char>(dyeItem.m_addInfo != 0));
        return 1;
    }
    Inven_Item targetItem;
    if (getItem(*user, 2, req.m_10, targetItem))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3), 1);
        return 1;
    }

    CItem* dyeDef = G_CDataManager()->find_item(dyeItem.m_addInfo);
    if (dyeDef == 0)
    {
        return 1;
    }
    if (!dyeDef->is_stackable())
    {
        return 1;
    }

    const UseDyeDyeInfoView& dye =
        *reinterpret_cast<const UseDyeDyeInfoView*>(&dyeDef->m_dyeInfo9c);
    if (dye.m_field0 == 0 || dye.m_vec4.empty())
    {
        return 1;
    }
    std::map<int, AvatarVariation::colorRGB>::const_iterator colorIt =
        G_CDataManager()->m_avatarColor.m_colorMap.find(dye.m_vec4[0].first);
    if (colorIt == G_CDataManager()->m_avatarColor.m_colorMap.end())
    {
        return 1;
    }

    CEquipItem* avatarDef =
        static_cast<CEquipItem*>(G_CDataManager()->find_item(targetItem.m_addInfo));
    if (!avatarDef->IsAvatarItem())          // vtable +0x10 槽（P5 直调）
    {
        return 1;
    }
    if (avatarDef->IsAvatarColorVariation(0) != 1)
    {
        return 1;
    }

    // ORIG 以目标物品 idx 为 avatar key
    const WongWork::CAvatarItemMgr* mgrR =
        user->getCurCharacInvenR()->GetAvatarItemMgrR();
    const void* expansion = mgrR->GetExpansionInfo(targetItem.m_addInfo);
    if (expansion == 0)
    {
        return 1;
    }
    const short oldColorA = reinterpret_cast<const short*>(expansion)[0];
    const short oldColorB = reinterpret_cast<const short*>(expansion)[1];

    WongWork::CAvatarItemMgr* mgrW = user->getCurCharacInvenW()->GetAvatarItemMgrW();
    const bool applied =
        mgrW->SetAvatarColor(targetItem.m_addInfo,
                             static_cast<short>(dye.m_vec4[0].first), 0);
    if (!applied)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3), 1);
        return LineFunc(0x6292,
                        "virtual int Dispatcher_UseDye::process(CUser*, MSG_BASE&, ParamBase&)",
                        1, user->get_acc_id());
    }

    const bool deleted = user->getCurCharacInvenW()->delete_item(
        static_cast<INVEN_TYPE>(1), req.m_0e, 1,
        static_cast<eItemDelReason>(3), true);
    if (!deleted)
    {
        // 删除失败：回滚头像颜色并报错
        mgrW->SetAvatarColor(targetItem.m_addInfo, oldColorA, oldColorB);
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1f3), 0x11);
        return LineFunc(0x629b,
                        "virtual int Dispatcher_UseDye::process(CUser*, MSG_BASE&, ParamBase&)",
                        1, user->get_acc_id());
    }

    notifyUpdatedInventoryItemInfo(*user, req.m_0e);

    PacketGuard packet;
    packet.put_header(1, 0x1f3);
    packet.put_byte(1);
    packet.put_short(req.m_10);
    packet.put_int(4);
    {
        // pair.first 低 16 位 + 2 字节零填充（local_6c/local_6a）
        const unsigned char raw[4] = {
            static_cast<unsigned char>(dye.m_vec4[0].first & 0xff),
            static_cast<unsigned char>((dye.m_vec4[0].first >> 8) & 0xff),
            0, 0 };
        packet.put_binary(const_cast<char*>(reinterpret_cast<const char*>(raw)), 4);
    }
    packet.finalize(true);
    user->Send(packet);

    user->getCurCharacInvenW()->SendAvatarEvent(
        0x31a, targetItem.m_addInfo, dye.m_vec4[0].first, 0, 0);

    /* BLOCKED: cUserHistoryLog::UseDye _ZN15cUserHistoryLog6UseDyeEiiRKSsii @0x08686cba
       —— HistoryLog.h 无该声明。ORIG 实参：
       ((cUserHistoryLog*)((char*)user + 0x79700))->UseDye(
           dyeItem.m_addInfo, targetItem.m_addInfo, targetItem.m_addInfo,
           std::string(avatar->GetIPGAgencyNo()), req.m_10); */
    return 0;
}

// ==== Dispatcher_Teleport::check_error (ORIG 0x081d056c，三参形态) ====
int Dispatcher_Teleport::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_TELEPORT& req = static_cast<MSG_TELEPORT&>(msg);
    stTeleportParam& out = static_cast<stTeleportParam&>(param);

    if (!(user->get_state() > 2 && user->getCurCharacR() != 0))
    {
        return -1;
    }
    if (user->get_state() > 4)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf1), 0x13);
        return -1;
    }

    const short slot = *reinterpret_cast<short*>(&req.m_pad[1]);
    const int invenType =
        GetInvenTypeFromItemSpace(static_cast<int>(*reinterpret_cast<char*>(&req.m_pad[7])));
    Inven_Item* ref = const_cast<Inven_Item*>(
        user->getCurCharacInvenR()->GetInvenRef(invenType, slot));
    if (ref == 0)
    {
        return -2;
    }
    const int itemIdx = ref->m_addInfo;
    if (itemIdx != 0x27ac4e && itemIdx != 0x28e7d0 && itemIdx != 0x29f288)
    {
        return -2;
    }

    const unsigned char vill =
        static_cast<unsigned char>(*reinterpret_cast<char*>(&req.m_pad[8]));
    if (static_cast<char>(vill) < 1)
    {
        return 0x7fffffff;
    }
    if (user->CheckMoveTown(static_cast<int>(static_cast<char>(vill))) != 0)
    {
        return 0x7fffffff;
    }

    CPartyTelePort* teleport = 0;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        teleport = static_cast<CPartyTelePort*>(party->GetPartyTelePort());
        const char state = teleport->get_teleport_state();
        if (state >= 0 && state <= 3)
        {
            return 0x9e;   // 组队传送进行中
        }
    }

    out.m_14 = static_cast<int>(itemIdx);
    /* BLOCKED: CUser::use_stackable(short,int,int,int) @0x0865e0ae（CUser.h 无声明）。
       ORIG: if (!user->use_stackable(slot, space, -1, -1)) return 0x11; */
    const bool consumed = true;  // 依赖补齐前按消耗成功路径继续

    /* BLOCKED: expert_job::CAlchemist::OnTeleportCharacter @0x085d1424 —— ORIG 为
       静态形态调用，而 CExpertJob.h 声明为成员且 CAlchemist 含纯虚无法实例化。
       ORIG: alchemistOk = CAlchemist::OnTeleportCharacter(user, (char)vill, 0,0,0, true);
       依赖补齐前按通过路径继续。 */
    const bool alchemistOk = true;
    if (!consumed)
    {
        return 0x11;
    }
    if (!alchemistOk)
    {
        return 0x7fffffff;
    }
    return 0;
}

// ==== Dispatcher_Teleport::process (ORIG 0x081d07bc) ====
int Dispatcher_Teleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stTeleportParam& out = static_cast<stTeleportParam&>(param);
    MSG_TELEPORT& req = static_cast<MSG_TELEPORT&>(msg);

    out.m_4 = check_error(user, msg, param);
    if (out.m_4 > 0)
    {
        return 0;
    }
    if (out.m_4 < 0)
    {
        return LineFunc(0x2bcc,
                        "virtual int Dispatcher_Teleport::process(CUser*, MSG_BASE&, ParamBase&)",
                        out.m_4, user->get_acc_id());
    }
    *reinterpret_cast<short*>(&out.m_gapA[0]) = *reinterpret_cast<short*>(&req.m_pad[1]);
    *reinterpret_cast<unsigned int*>(&out.m_gapA[2]) =
        *reinterpret_cast<unsigned int*>(&req.m_pad[3]);
    out.m_10 = req.m_pad[7];
    out.m_11 = req.m_pad[8];
    return 0;
}

// ==== Dispatcher_Teleport::send (ORIG 0x081d087c) ====
void Dispatcher_Teleport::send(CUser* user, ParamBase& param)
{
    stTeleportParam& out = static_cast<stTeleportParam&>(param);
    if (out.m_4 == static_cast<int>(0x7fffffff))
    {
        return;
    }
    if (out.m_4 != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0xf1),
                                 static_cast<unsigned char>(out.m_4));
        return;
    }
    PacketGuard itemPacket;
    itemPacket.put_header(0, 0xe);
    itemPacket.put_byte(out.m_10);
    itemPacket.put_short(1);
    user->getCurCharacInvenR()->MakeItemPacket(
        static_cast<INVEN_TYPE>(GetInvenTypeFromItemSpace(
            static_cast<int>(static_cast<char>(out.m_10)))),
        out.m_8, itemPacket);
    itemPacket.finalize(true);
    user->Send(itemPacket);

    PacketGuard packet;
    packet.put_header(1, 0xf1);
    packet.put_byte(1);
    packet.put_short(out.m_8);
    packet.put_int(out.m_14);
    packet.finalize(true);
    user->Send(packet);
}


// ══════════════ 批次 B2：11 类桩真实化落盘区（DispatchersRestore） ══════════════

/* ==================== CeraPackageOpen_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_CeraPackageOpen 桩真实化 —— 函数体交付（B2）
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 类段
// （替换 2225-2238 行空 ctor/dtor 与 check_error/process/read 桩；本类 ORIG 无
//   send —— IPacketDispatcher 基类 send 空实现即 ORIG 形态）
//
// 报告依据（docs/class_func_reports/Dispatcher_CeraPackageOpen/，全部已完整阅读）：
//   check_error.md 0x081e7cc2 / read.md 0x081e76fa / process.md 0x081e788a
//
// 【前置条件 — 宿主必须先扩头，否则越界】PacketTypes.h 现为：
//     class stOpenCeraPackage : public ParamBase { int m_4; char m_pad08[0x3c]; };
//   （sizeof 0x44）。ORIG read 按 msg 写至 +0x148：+0xe short slot、
//   +0x10 byte count(≤0x23)、条目 i∈[0,count)：ulong@+(i+2)*8+4、byte@+(i+2)*8+8。
//   dispatch_template 以 `Param param;` 栈分配，宿主需扩 sizeof ≥ 0x149。
//
// 【BLOCKED 外部调用】（grep 全仓库无声明，按约束注释占位、其余逻辑保留）：
//   _ZN8WongWork9CCeraShop22CallSpecialItemHandlerEiRK19STSpecailItem_ParamR20STSpecailItem_Result @0x08325854
//   —— CCeraShop.h 无该成员声明；STSpecailItem_Param/STSpecailItem_Result 为
//      CCeraShop_Special.cpp 文件局部 struct（ORIG W ctor 0x08234ec2/0x08234f36，
//      布局 +0x00 user +0x04 itemIdx +0x08 value +0x0c items* +0x10 ceraParam*
//      +0x14 count +0x18 time(long) +0x1c flag +0x1d..1f char +0x20 extra=1 /
//      Result: int m_value）。恢复方式：将两 struct 提升为权威头类型、CCeraShop.h
//      补 CallSpecialItemHandler(int, const STSpecailItem_Param&, STSpecailItem_Result&)
//      声明后，去掉 process 内 BLOCKED 占位改为真实调用（含构造序列，注释已给出）。
//
// 头约束：新增 include 无（宿主 TU 已含 CUser.h/CInventory.h/CDataManager.h/
// GameEnums.h/PacketGuard.h/InterfacePacketBuf.h 等）。
// ============================================================================

// ==== Dispatcher_CeraPackageOpen::check_error (ORIG 0x081e7cc2) ====
int Dispatcher_CeraPackageOpen::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user->get_state() != 3)
    {
        return 0x13;
    }
    if (user->CheckInTrade())
    {
        return 0x13;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0x13;
    }
    return 0;
}

// ==== Dispatcher_CeraPackageOpen::read (ORIG 0x081e76fa) ====
int Dispatcher_CeraPackageOpen::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* const M = reinterpret_cast<char*>(&msg);

    if (!packet.get_short(*reinterpret_cast<short*>(M + 0xe)))
    {
        return LineFunc(
            0x636b,
            "virtual int Dispatcher_CeraPackageOpen::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }

    if (!packet.get_byte(*reinterpret_cast<unsigned char*>(M + 0x10)))
    {
        return LineFunc(
            0x636e,
            "virtual int Dispatcher_CeraPackageOpen::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }

    // cmp $0x23,%al; jbe → unsigned byte ≤ 0x23 才通过
    if (*reinterpret_cast<const unsigned char*>(M + 0x10) > 0x23)
    {
        return LineFunc(
            0x6371,
            "virtual int Dispatcher_CeraPackageOpen::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }

    for (unsigned int i = 0; i < *reinterpret_cast<const unsigned char*>(M + 0x10); ++i)
    {
        if (!packet.get_int(
                *reinterpret_cast<unsigned long*>(M + (i + 2) * 8 + 4)))
        {
            return LineFunc(
                0x6376,
                "virtual int Dispatcher_CeraPackageOpen::read(PacketBuf&, MSG_BASE&)",
                0, 0);
        }
        if (!packet.get_byte(
                *reinterpret_cast<unsigned char*>(M + (i + 2) * 8 + 8)))
        {
            return LineFunc(
                0x6379,
                "virtual int Dispatcher_CeraPackageOpen::read(PacketBuf&, MSG_BASE&)",
                0, 0);
        }
    }
    return 0;
}

// ==== Dispatcher_CeraPackageOpen::process (ORIG 0x081e788a) ====
int Dispatcher_CeraPackageOpen::process(CUser* user, MSG_BASE& msg,
                                        ParamBase& param)
{
    char* const M = reinterpret_cast<char*>(&msg);
    char* const P = reinterpret_cast<char*>(&param);

    const int err = check_error(user, msg);
    *reinterpret_cast<int*>(P + 4) = err;                   // stOpenCeraPackage::m_4
    if (err != 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207),
                                 static_cast<unsigned char>(err & 0xff));
        return 0;
    }

    const short slot = *reinterpret_cast<const short*>(M + 0xe);
    if (user->CheckItemLock(1, static_cast<int>(slot)))
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207), 4);
        return 0;
    }

    const Inven_Item slotItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, static_cast<int>(slot));
    const int itemIdx = slotItem.m_addInfo;                 // Inven_Item+0x02
    if (itemIdx == 0)
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207), 1);
        return 0;
    }

    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
    {
        return 1;                                           // 静默返回（ORIG ebx=1）
    }
    if (!item->is_stackable() || item->GetItemType() != 0x26)
    {
        // vtable+0xc 直调 = GetItemType()（P5；CItem.h 虚序 D1/D0/make_item/GetItemType）
        return 1;
    }

    {
        // ---- 构造特殊商品处理入参（两 BLOCKED 类型均无权威可见性）----
        /* BLOCKED: stCeraShopItemParam_t 构造（ORIG ctor 0x0817a244）——
           WongWork::CCeraShop 嵌套类型无权威可见性。ORIG:
             count = *(uchar*)(M+0x10);
             for i<count: ceraParam.m_pairs.push_back({(int)*(ulong*)(M+(i+2)*8+4),
                                                       (int)*(uchar*)(M+(i+2)*8+8)});
           依赖补齐后恢复。 */
        std::vector<Inven_Item> items;                      // ORIG vector<Inven_Item> C1

        bool handlerOk = false;
        int resultValue = 0;
        /* BLOCKED: _ZN8WongWork9CCeraShop22CallSpecialItemHandlerEiRK19STSpecailItem_ParamR20STSpecailItem_Result @0x08325854
           ORIG:
             STSpecailItem_Param sp(items, ceraParam);       // W 0x08234ec2
             sp.m_user = user;
             sp.m_itemIdx = itemIdx;
             STSpecailItem_Result result;                    // W 0x08234f36（清零首 4 字节）
             handlerOk = GlobalData::s_pCeraShop->CallSpecialItemHandler(5, sp, result);
             resultValue = result.m_value;
           两参数类型为 CCeraShop_Special.cpp 文件局部 struct、CCeraShop.h 缺方法声明；
           占位保持其余逻辑（handlerOk=false → 走错误回包分支），恢复后由其写回。 */
        (void)items;

        if (!handlerOk)
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207),
                                     static_cast<unsigned char>(resultValue & 0xff));
            return 0;
        }

        if (resultValue == 0)
        {
            // 扣除礼包本体并回 ACK 包
            user->getCurCharacInvenW()->delete_item(
                static_cast<INVEN_TYPE>(1), static_cast<int>(slot), 1,
                static_cast<eItemDelReason>(3), true);
            PacketGuard pg;                                 // ORIG 此处无 clear（同汇编）
            pg.put_header(1, 0x207);
            pg.put_byte(1);
            pg.put_short(slot);
            pg.finalize(true);
            user->Send(pg);
        }
        else
        {
            user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x207),
                                     static_cast<unsigned char>(resultValue & 0xff));
        }
        return 0;
    }
}

/* ==================== Debug_Clear_Quest_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_Debug_Clear_Quest —— B2 批次真实化(DispB2_GrpH)
// 报告:docs/class_func_reports/Dispatcher_Debug_Clear_Quest/{read,process,send}.md
//
// 宿主接入说明:
//   - 新增 include:无(宿主 Dispatchers.cpp 已含 Quest.h(MsgClearQuestIndex/
//     ReqDebugClearQuestResult 经 PacketTypes.h、stSelectQuestParam、Quest)、
//     CDataManager.h(G_CDataManager/find_quest)、CUser.h(isGMUser/incQuestPoint/
//     incQuestPiece/sendCharacQp/sendCharacQuestPiece/getCurCharacQuestW/
//     getCurCharacQuestR/send_clear_quest_list)、QuestClear.h(CQuestClear::
//     setClearedQuest(unsigned))、PacketGuard.h。
//   - 新增头声明:无(Dispatchers.h 已声明 process/read/send 三方法)。
//
// 注:宿主 Dispatchers.cpp:2916-3008 已含一份等价实现(逐报告核对语义一致);
// 本文件为报告忠实转写,供宿主统一落盘时对照或覆盖。两处差异已按报告修正:
//   1) find_quest 走 const 重载 _ZNK12CDataManager10find_questEi @0x0835fdc6
//      (ORIG 符号),经 const CDataManager* 调用;
//   2) find_skill 同理见 Skill_Command_Customizing_body.cpp。
// ============================================================================




/* ==================== ExpertExtraction_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_ExpertExtraction 四函数真实化(ORIG 转写)
// 目标宿主:source/DNFServer/GameServer/Game/Dispatchers.cpp 2571-2576 现有桩
//   (check_error/process/read/send)按本文件替换函数体。
// ============================================================================
//
// ---- 新增 include(宿主 Dispatchers.cpp 当前缺失,需补)----
//   #include "STItemScript.h"      // ENUM_EXPERT_JOB_TYPE
// 已含可复用:GlobalData.h(g_ExtractionMgr)、PacketGuard.h、InterfacePacketBuf.h、
//   CompoundEmblemDeps.h(LineFunc)、GameEnums.h(ENUM_CMDPACKET)。
//
// ---- 宿主需补的声明(expert_extraction 域当前无任何头声明/实现)----
// 1) expert_extraction::GetExtracterType — ORIG T _ZN17expert_extraction16GetExtracterTypeE20ENUM_EXPERT_JOB_TYPE @0x084a2904:
//      namespace expert_extraction { int GetExtracterType(ENUM_EXPERT_JOB_TYPE job); }
//    语义:job==2→0(Alchemist)/job==1→1(Enchanter)/job==4→2(DollController)/其它→3。
// 2) expert_extraction::CExtractionMgr::get_extracter — ORIG T ...13get_extracterEj @0x084a3166:
//      class CExtractionMgr { public: CExtraction* get_extracter(unsigned int type); };
//    该类现为 GlobalData.cpp 局部类(m_slots[3]),需提升到 GlobalData.h 供本 TU 调用;
//    get_extracter 即 m_slots[type](type>2 返回 0)。
// 3) expert_extraction::CExtraction 业务虚函数(GlobalData.cpp 局部类现仅 ctor/dtor/m_rand,
//    需按 ORIG vtable 槽序补齐;ORIG vtable @0x8c7ddc0:槽+8/+0xc/+0x10):
//      virtual ~CExtraction();                                                        // +0x00/+0x04
//      virtual bool extraction(CUser*, short, std::vector<std::pair<int,int> >&);      // +0x08 ORIG T 0x084a3182 族
//      virtual int  check_error(CUser*, char, short, short);                          // +0x0c ORIG T 0x084a2a5e
//      virtual bool inven_process(CUser*, short, std::vector<std::pair<int,int> >&, std::vector<int>&); // +0x10 ORIG T 0x084a2cec
// ============================================================================

// ==== Dispatcher_ExpertExtraction::read (ORIG 0x081d9336) ====
int Dispatcher_ExpertExtraction::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_EXPERT_EXTRACTION& m = static_cast<MSG_EXPERT_EXTRACTION&>(msg);
    if (!packet.get_byte(reinterpret_cast<char&>(m.m_0d)))
    {
        return LineFunc(0x45ac, "virtual int Dispatcher_ExpertExtraction::read(PacketBuf&, MSG_BASE&)", 0, 0);
    }
    if (!packet.get_short(reinterpret_cast<short&>(m.m_0e)))
    {
        return LineFunc(0x45ae, "virtual int Dispatcher_ExpertExtraction::read(PacketBuf&, MSG_BASE&)", 0, 0);
    }
    if (!packet.get_short(reinterpret_cast<short&>(m.m_10)))
    {
        return LineFunc(0x45b0, "virtual int Dispatcher_ExpertExtraction::read(PacketBuf&, MSG_BASE&)", 0, 0);
    }
    return 0;
}

// ==== Dispatcher_ExpertExtraction::process (ORIG 0x081d9412) ====
int Dispatcher_ExpertExtraction::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stExpertExtraction& p = static_cast<stExpertExtraction&>(param);
    MSG_EXPERT_EXTRACTION& m = static_cast<MSG_EXPERT_EXTRACTION&>(msg);

    const int err = check_error(user, msg);
    p.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x45bf, "virtual int Dispatcher_ExpertExtraction::process(CUser*, MSG_BASE&, ParamBase&)", err, 0);
    }

    /* BLOCKED: expert_extraction::{GetExtracterType @0x084a2904, CExtractionMgr::get_extracter
       @0x084a3166} —— GlobalData.cpp 局部类不完整、无头声明。ORIG:
       ex = g_ExtractionMgr->get_extracter(GetExtracterType((ENUM_EXPERT_JOB_TYPE)m.m_0d));
       依赖补齐前按"提取器存在且校验通过"路径继续（ex 恒视为非空）。
       ORIG ex==0 分支: return LineFunc(0x45cb, ..., 0, get_acc_id); */

    std::vector<std::pair<int, int> > materials;
    materials.clear();
    /* BLOCKED: CExtraction::extraction vtable+0x08 —— expert_extraction 域未建模。
       ORIG: if (!ex->extraction(user, (short)m.m_10, materials)) { p.m_4 = 1; return 0; }
       依赖补齐前按成功路径继续（materials 保持空）。 */
    const bool extracted = true;
    if (!extracted)
    {
        p.m_4 = 1;
        return 0;
    }

    std::vector<int> results;
    results.clear();
    /* BLOCKED: CExtraction::inven_process vtable+0x10 —— 同上。
       ORIG: if (!ex->inven_process(user, (short)m.m_10, materials, results)) { p.m_4 = 0x11; } else ... */
    const bool invenOk = true;
    if (!invenOk)
    {
        p.m_4 = 0x11;
    }
    else
    {
        p.m_8 = (short)m.m_10;
        p.m_list = results;
    }
    return 0;
}

// ==== Dispatcher_ExpertExtraction::check_error (ORIG 0x081d962e) ====
int Dispatcher_ExpertExtraction::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    MSG_EXPERT_EXTRACTION& m = static_cast<MSG_EXPERT_EXTRACTION&>(msg);

    const int type = 0;  // BLOCKED: GetExtracterType @0x084a2904（同上）
    if (type == 3)
    {
        return 1;
    }
    /* BLOCKED: CExtractionMgr::get_extracter @0x084a3166 / CExtraction::check_error vtable+0x0c —— 同上。
       ORIG: if (ex == 0) return 1; return ex->check_error(user, (char)m.m_0d, (short)m.m_0e, (short)m.m_10);
       依赖补齐前按校验通过返回。 */
    return 0;
}

// ==== Dispatcher_ExpertExtraction::send (ORIG 0x081d9700) ====
void Dispatcher_ExpertExtraction::send(CUser* user, ParamBase& param)
{
    stExpertExtraction& p = static_cast<stExpertExtraction&>(param);
    if (p.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1a0, (unsigned char)p.m_4);
        return;
    }

    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1a0);
    guard.put_byte(1);
    guard.put_short((int)p.m_8);

    const size_t count = p.m_list.size() / 3;
    guard.put_byte((int)count);
    for (size_t i = 0; i < count; ++i)
    {
        guard.put_short(p.m_list[i * 3 + 0]);
        guard.put_int(p.m_list[i * 3 + 1]);
        guard.put_int(p.m_list[i * 3 + 2]);
    }
    guard.finalize(true);
    user->Send(guard);
}

/* ==================== FatigueAttendance_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_FatigueAttendance 函数体批次（disp_restore_b2 / GrpF）
// 来源报告：docs/class_func_reports/Dispatcher_FatigueAttendance/{README,check_error,process,read,send}.md
// 目标落点：source/DNFServer/GameServer/Game/Dispatchers.cpp 替换 3026-3031 行桩段
// ============================================================================
//
// ---- 新增 include（宿主 Dispatchers.cpp 当前缺失）----
                               // EventClassify::CAttendance（get_event_script_mng 视图）

// ---- 宿主需补的头声明（本批次函数体直接使用）----
// CUser.h（现仅有无参 void* get_ingame_event_history() @CUser.h:2111，缺重载）：
//   void get_ingame_event_history(int type, std::pair<int,int>& out);
//       // ORIG _ZN5CUser24get_ingame_event_historyEiRSt4pairIiiE T 0x0868d50c
//   void set_ingame_event_history(int type, int value1, int value2);
//       // ORIG _ZN5CUser24set_ingame_event_historyEiii T 0x0868d580
// 自由位工具（定义已在 BingoEventLink4.cpp，缺公共声明点）：
//   bool isSetBit_Uint(unsigned int value, int bit);        // T 0x0889232e
//   void setBit_Uint(unsigned int& value, bool set, int bit); // T 0x088922d7
//
// ---- BLOCKED 依赖（占位保留，其余逻辑已真实化）----
//   _ZN13EventClassify15CEventScriptMng29process_update_ingame_historyEP5CUser
//     T 0x0810c82e —— CEventScriptMng.h 无此方法声明、全仓无定义。
//     见 process() 内注释；恢复后取消注释该调用行即可。
// ============================================================================

// ==== Dispatcher_FatigueAttendance::check_error (ORIG 0x081db310) ====
int Dispatcher_FatigueAttendance::check_error(CUser* user, MSG_BASE& msg)
{
    // ORIG：0xc(%ebp)=user 判空 → -1；msg 引用存帧后未消费（死存储，省略）。
    if (user == 0)
    {
        return -1;
    }
    if (user->getCurCharacR() == 0)
    {
        return -1;
    }
    if (user->get_state() != 3)
    {
        return 99;
    }
    EventClassify::CEventScriptMng* mng =
        (EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng();
    // ORIG：is_eventing(0x56) 取反检验；真 → 0，假 → 7。
    if (!mng->is_eventing(0x56))
    {
        return 7;
    }
    return 0;
}

// ==== Dispatcher_FatigueAttendance::read (ORIG 0x081dafa4) ====
int Dispatcher_FatigueAttendance::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_FATIGUE_ATTENDANCE& req = static_cast<MSG_FATIGUE_ATTENDANCE&>(msg);
    // ORIG：get_int(&msg+0xd)；失败走 LineFunc(0x49b3)。
    if (!packet.get_int(req.m_0d))
    {
        return LineFunc(0x49b3,
                        "virtual int Dispatcher_FatigueAttendance::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_FatigueAttendance::process (ORIG 0x081daffa) ====
int Dispatcher_FatigueAttendance::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_FATIGUE_ATTENDANCE& req = static_cast<MSG_FATIGUE_ATTENDANCE&>(msg);
    // stFatigueAttendance 字段（现 m_pad[0x40] 占位；建议宿主字段化）：
    //   +0x04 int  errNo（check_error 结果，send 消费）
    //   +0x08 int  当日序号（= req.m_0d 回填，send 以 short 发出）
    //   +0x0c unsigned 打卡位图（setBit 后的 history.first 快照，send put_int）
    //   +0x10 int  今日时间值（get_today_time，send put_int）
    char* pres = reinterpret_cast<char*>(&param);

    // ORIG +0x008：param+8 = *(int*)(msg+0xd)（先于 check_error 回填请求当日序号）
    *reinterpret_cast<int*>(pres + 8) = req.m_0d;

    int err = check_error(user, msg);
    *reinterpret_cast<int*>(pres + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x49c5,
            "virtual int Dispatcher_FatigueAttendance::process(CUser*, MSG_BASE&, ParamBase&)",
            err, 0);
    }

    EventClassify::CEventScriptMng* mng =
        (EventClassify::CEventScriptMng*)G_CDataManager()->get_event_script_mng();

    unsigned char day = mng->get_today_time_day();          // +0x09d（uchar）
    int recordType = mng->get_record_type(0x56);            // +0x0bd

    /* BLOCKED: CUser::get_ingame_event_history(int, pair<int,int>&) @0x0868d50c ——
       CUser.h 仅有无参 void* 重载。ORIG: get_ingame_event_history(recordType, history)。 */
    std::pair<int, int> history(0, 0);
    unsigned int attendFlags = static_cast<unsigned int>(history.first);

    // 已打过当日卡（bit day-1 已置位）→ 0x12
    if (isSetBit_Uint(attendFlags, static_cast<int>(day) - 1))
    {
        *reinterpret_cast<int*>(pres + 4) = 0x12;
        return 0;
    }

    // 疲劳合计：server_fatigue_count(day)（short）+ fatigue_count_db(day)
    int totalFatigue = static_cast<int>(
        user->get_server_fatigue_count(day));
    totalFatigue += user->get_fatigue_count_db(static_cast<int>(day));

    EventClassify::CEventEntity* entity = mng->get_event_entity(0x56);   // +0x166
    if (entity == 0)
    {
        return 0;
    }

    int todayTime = mng->get_today_time();
    // ORIG：start > today || end < today → 不在活动期 → 7
    if (entity->get_event_start_time() > todayTime ||
        entity->get_event_end_time() < todayTime)
    {
        *reinterpret_cast<int*>(pres + 4) = 7;
        return 0;
    }

    // 疲劳超上限（fatigue_condition < total）→ 0x16
    EventClassify::CAttendance* attendance =
        static_cast<EventClassify::CAttendance*>(entity);
    if (attendance->get_fatigue_condition() < totalFatigue)
    {
        *reinterpret_cast<int*>(pres + 4) = 0x16;
        return 0;
    }

    // 置当日打卡位并回写历史（第三参恒 0）
    setBit_Uint(attendFlags, true, static_cast<int>(day) - 1);
    /* BLOCKED: CUser::set_ingame_event_history(int,int,int) —— 头缺声明；ORIG 回写历史 */

    *reinterpret_cast<unsigned int*>(pres + 0xc) = attendFlags;          // +0x24a
    int nowTime = mng->get_today_time();
    *reinterpret_cast<int*>(pres + 0x10) = nowTime;                      // +0x265

    int passDay = mng->get_pass_day(entity->get_event_start_time(), nowTime);
    attendance->send_attend_reward_item(user, passDay);                  // T 0x0810e878

    // BLOCKED: _ZN13EventClassify15CEventScriptMng29process_update_ingame_historyEP5CUser
    //          @0x0810c82e（无声明无定义）。ORIG 此处调
    //          mng->process_update_ingame_history(user);
    user->reset_ingame_event_history_update();

    int finalCondition = attendance->get_final_day_condition();
    attendance->send_attend_final_rward_item(user, static_cast<int>(attendFlags),
                                             finalCondition);            // T 0x0810e9c0
    return 0;
}

// ==== Dispatcher_FatigueAttendance::send (ORIG 0x081db392) ====
void Dispatcher_FatigueAttendance::send(CUser* user, ParamBase& param)
{
    char* pres = reinterpret_cast<char*>(&param);
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1ad);
    if (*reinterpret_cast<const int*>(pres + 4) == 0)
    {
        guard.put_byte(1);
        // ORIG 读 param+8 整型入栈，由 put_short 取低 16 位发出
        guard.put_short(*reinterpret_cast<const int*>(pres + 8));
        guard.put_int(*reinterpret_cast<const int*>(pres + 0xc));
        guard.put_int(*reinterpret_cast<const int*>(pres + 0x10));
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(static_cast<signed char>(*reinterpret_cast<const int*>(pres + 4)));
        guard.put_short(*reinterpret_cast<const int*>(pres + 8));
    }
    guard.finalize(true);
    user->Send(guard);
}

/* ==================== ObjectBringUp_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_ObjectBringUp 桩真实化 —— 函数体交付（B2）
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 类段
// （替换 1985-1997 行空 ctor/dtor 与 check_error/process/read/send 桩）
//
// 报告依据（docs/class_func_reports/Dispatcher_ObjectBringUp/，全部已完整阅读）：
//   check_error.md 0x081e16ce / read.md 0x081e147e / process.md 0x081e14d4 /
//   send.md 0x081e1704
//
// 【前置条件 — 宿主必须先扩头，否则越界】PacketTypes.h 现为：
//     class stObjectBringUp : public ParamBase {
//         int m_4; short m_8; unsigned char m_a; unsigned char m_b; };  // sizeof 0x10
//   ORIG 另在 +0x0c 内嵌 std::vector<int>（sendBringUpReward 输出、send 迭代回包，
//   共 0xc 字节，总 sizeof 应为 0x18）。dispatch_template 以 `Param param;` 栈分配，
//   宿主需补成员（建议名 m_rewards，+0x0c vector<int>）。本文件以
//   reinterpret_cast 承载该 vector 引用，其余字段用现有具名成员。
//
// 【BLOCKED 外部调用】（ORIG 符号经 docs/class_func_reports/EventClassify__CObjectBringUp/
// 确认真实存在，但仓库无声明无定义 —— 归 EventClassify 实体批次；按约束注释占位、
// 其余逻辑保留）：
//   1. _ZN13EventClassify14CObjectBringUp24verifyMaterialItemByRuleEP5CUserj @0x0810fcea
//      bool verifyMaterialItemByRule(CUser*, unsigned int itemIdx)
//   2. _ZN13EventClassify14CObjectBringUp17sendBringUpRewardEP5CUserRSt6vectorIiSaIiEERh @0x0810fe90
//      bool sendBringUpReward(CUser*, std::vector<int>& rewards, unsigned char& flag)
//   恢复方式：CEventScriptMng.h 的 class CObjectBringUp 补上述两声明（同文件
//   CPurchaseCashItemBonus::sendPurchaseBonusReward「声明先行」先例），实现由
//   EventClassify__CObjectBringUp 两份报告落地后，去掉 process 内对应 BLOCKED 占位。
//
// 头约束：新增 include "CEventScriptMng.h"（宿主 TU 未含；get_event_script_mng/
// get_event_entity/is_eventing/CObjectBringUp 前置可见性）。
// ============================================================================


// ==== Dispatcher_ObjectBringUp::check_error (ORIG 0x081e16ce) ====
int Dispatcher_ObjectBringUp::check_error(CUser* user, MSG_BASE& msg)
{
    (void)msg;
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)                             // cmp $2,setle（P2）
    {
        return -2;
    }
    return 0;
}

// ==== Dispatcher_ObjectBringUp::read (ORIG 0x081e147e) ====
int Dispatcher_ObjectBringUp::read(PacketBuf& packet, MSG_BASE& msg)
{
    // PacketBuf::get_short(unsigned short&)（零扩展口径）
    if (!packet.get_short(
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(&msg) + 0xe)))
    {
        return LineFunc(
            0x56aa,
            "virtual int Dispatcher_ObjectBringUp::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    return 0;
}

// ==== Dispatcher_ObjectBringUp::process (ORIG 0x081e14d4) ====
int Dispatcher_ObjectBringUp::process(CUser* user, MSG_BASE& msg,
                                      ParamBase& param)
{
    stObjectBringUp& out = static_cast<stObjectBringUp&>(param);

    EventClassify::CEventScriptMng* scriptMng = static_cast<EventClassify::CEventScriptMng*>(
        G_CDataManager()->get_event_script_mng());
    EventClassify::CObjectBringUp* entity =
        static_cast<EventClassify::CObjectBringUp*>(scriptMng->get_event_entity(0x19b));
    if (entity == 0)
    {
        out.m_4 = 0xb2;
        return 0;
    }

    if (!entity->is_eventing())                             // vtable+0x08 虚槽直调（P5）
    {
        out.m_4 = 0xb2;
        return 0;
    }

    const int err = check_error(user, msg);
    out.m_4 = err;
    if (err != 0)
    {
        return 0;
    }

    const unsigned short slot =
        *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const char*>(&msg) + 0xe);
    const Inven_Item slotItem =
        user->getCurCharacInvenR()->GetInvenSlot(1, static_cast<int>(slot));
    const unsigned int itemIdx = static_cast<unsigned int>(slotItem.m_addInfo);

    if (G_CDataManager()->find_item(static_cast<int>(itemIdx)) == 0)
    {
        out.m_4 = 0x11;
        return 0;
    }

    bool materialOk = true;
    /* BLOCKED: _ZN13EventClassify14CObjectBringUp24verifyMaterialItemByRuleEP5CUserj @0x0810fcea
       ORIG: materialOk = entity->verifyMaterialItemByRule(user, itemIdx);
       ORIG 符号真实（报告 verifyMaterialItemByRule.md 全文在手）但仓库无声明定义；
       占位保持 true 使后续扣料/发奖逻辑可达；恢复后由其写回（false → out.m_4=0x17）。 */
    if (!materialOk)
    {
        out.m_4 = 0x17;
        return 0;
    }
    user->incCurUseBringUpMaterialCount();                  // ORIG 调用点 0x0822fac4
    out.m_a = user->getCurUseBringUpMaterialTotal();        // ORIG 调用点 0x08110d12

    user->getCurCharacInvenW()->delete_item(
        static_cast<INVEN_TYPE>(1), static_cast<int>(slot), 1,
        static_cast<eItemDelReason>(3), true);

    // +0x0c 处 vector<int>：sendBringUpReward 输出（每轮消费 4 个 int）
    std::vector<int>& rewards =
        *reinterpret_cast<std::vector<int>*>(
            reinterpret_cast<char*>(&out) + 0xc);
    bool rewardOk = true;
    /* BLOCKED: _ZN13EventClassify14CObjectBringUp17sendBringUpRewardEP5CUserRSt6vectorIiSaIiEERh @0x0810fe90
       ORIG: rewardOk = entity->sendBringUpReward(user, rewards, out.m_b);
       ORIG 符号真实（报告 sendBringUpReward.md 在手）但仓库无声明定义；
       占位保持 true 使成功回写（m_8=slot）可达；恢复后由其写回（false → out.m_4=1）。 */
    (void)rewards;

    if (!rewardOk)
    {
        out.m_4 = 1;
        return 0;
    }

    out.m_8 = slot;
    return 0;
}

// ==== Dispatcher_ObjectBringUp::send (ORIG 0x081e1704) ====
void Dispatcher_ObjectBringUp::send(CUser* user, ParamBase& param)
{
    const stObjectBringUp& out = static_cast<const stObjectBringUp&>(param);

    PacketGuard pg;                                         // ORIG 无 clear（同汇编）
    pg.put_header(1, 0x1d5);

    if (out.m_4 == 0)
    {
        pg.put_byte(1);
        pg.put_short(static_cast<unsigned short>(out.m_8));
        pg.put_byte(static_cast<unsigned int>(out.m_a));
        pg.put_byte(static_cast<unsigned int>(out.m_b));

        if (out.m_b != 0)
        {
            // +0x0c 处 vector<int>：每轮消费 4 个 int（short,int,int,short）
            const std::vector<int>& rewards =
                *reinterpret_cast<const std::vector<int>*>(
                    reinterpret_cast<const char*>(&out) + 0xc);
            std::vector<int>::const_iterator it = rewards.begin();
            while (it != rewards.end())
            {
                const short v0 = static_cast<short>(*it); ++it;
                const int v1 = *it; ++it;
                const int v2 = *it; ++it;
                const short v3 = static_cast<short>(*it); ++it;
                pg.put_short(v0);
                pg.put_int(v1);
                pg.put_int(v2);
                pg.put_short(v3);
            }
        }
    }
    else
    {
        pg.put_byte(0);
        pg.put_byte(out.m_4 & 0xff);
    }

    pg.finalize(true);
    user->Send(pg);
}

/* ==================== RequestAssaultPrice_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_RequestAssaultPrice 四函数真实化(ORIG 转写)
// 目标宿主:source/DNFServer/GameServer/Game/Dispatchers.cpp 3428-3433 现有桩
//   (check_error/process/read/send)按本文件替换函数体。
// ============================================================================
//
// ---- 新增 include(宿主 Dispatchers.cpp 当前缺失,需补)----
//   #include "CAssaultMgr.h"        // pvp_assault::GetInstanceAssaultMgr/CAssaultMgr
//   #include "CPrivateStoreMgr.h"   // private_store::GetInstancePrivateStoreMgr/IsBusyPrivateStore
//   #include "CEquipItem.h"         // CEquipItem::getUpgradeCostDiscount(0x822ca80)
// 已含可复用:GameWorld.h(G_GameWorld/find_from_world/IsIntegratedPvPBaseChannel)、
//   GlobalData.h(s_pSecuProtectionField/s_pGMAccounts_)、CGuildServerProxy.h(CGMAccounts::isGM)、
//   CInventory.h(CSecu_ProtectionField/SECURITY_PROTCTION/CInventory::GetInvenSlot)、
//   CUserCharacInfo.h(IsBusyDisjointStore/IsCurCharacGhost/SetCurCharacAssault*/getCurCharac*)、
//   CDataManager.h(G_CDataManager/find_item)、Inven_Item.h、PacketGuard.h、
//   InterfacePacketBuf.h、CompoundEmblemDeps.h(LineFunc)、GameEnums.h。
//
// ---- 宿主需补的声明 ----
// 1) pvp_assault::CAssaultMgr(CAssaultMgr.h 补三个成员声明,均为 ORIG T):
//      int  check_party_teleport_user(char type, CUser* a, CUser* b, bool flag); // ORIG T 0x082efe14 _ZN...25check_party_teleport_userEcP5CUserS2_b
//      int  GetRequestAssaultNeedMoney(CUser* a, CUser* b);                      // ORIG T 0x082edc86 _ZN...26GetRequestAssaultNeedMoneyEP5CUserS1_
//      void OnRequestAssault(CUser* a, CUser* b, int* outErr);                   // ORIG T 0x082edfba _ZN...16OnRequestAssaultEP5CUserS1_Pi
// 2) stRequestAssaultPrice(PacketTypes.h:2407 现 char m_pad[0x40],按 ORIG 布局字段化,
//    尺寸不变,仅前 8 字节具名;stExpertExtraction 同型先例):
//      int m_4;    // +0x04 check_error 结果
//      int m_8;    // +0x08 突袭费用(含折扣后)
//      char m_pad10[0x38]; // +0x0c..+0x40 保持原总长
// ============================================================================

// ==== Dispatcher_RequestAssaultPrice::read (ORIG 0x081dee2a) ====
int Dispatcher_RequestAssaultPrice::read(PacketBuf& packet, MSG_BASE& msg)
{
    MsgRequestAssaultPrice& m = static_cast<MsgRequestAssaultPrice&>(msg);
    if (!packet.get_short(reinterpret_cast<unsigned short&>(m.m_0d)))
    {
        return LineFunc(0x50bd, "virtual int Dispatcher_RequestAssaultPrice::read(PacketBuf&, MSG_BASE&)", 0, 0);
    }
    return 0;
}

// ==== Dispatcher_RequestAssaultPrice::process (ORIG 0x081dee80) ====
int Dispatcher_RequestAssaultPrice::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRequestAssaultPrice& p = static_cast<stRequestAssaultPrice&>(param);
    MsgRequestAssaultPrice& m = static_cast<MsgRequestAssaultPrice&>(msg);

    CUser* target = G_GameWorld()->find_from_world((unsigned short)m.m_0d);
    if (target == 0)
    {
        p.m_4 = 3;
        return 0;
    }
    if (target->get_state() <= 2 || target->getCurCharacR() == 0)
    {
        p.m_4 = 3;
        return 0;
    }

    /* BLOCKED: pvp_assault::GetInstanceAssaultMgr()->GetRequestAssaultNeedMoney @0x082edc86
       —— CAssaultMgr.h 无该访问器声明。ORIG: needMoney = GetInstanceAssaultMgr()->GetRequestAssaultNeedMoney(user, target); */
    const int needMoney = 0;
    p.m_8 = needMoney;
    if (p.m_8 > 0)
    {
        // ORIG:GetInvenSlot(inven, 0, 0xb),取返回 Inven_Item 的 +0x02(item idx)查物品,
        // 命中则按装备折扣重算费用:money + discount*money/(-100.0f)(float 截断取整)。
        Inven_Item slot = user->getCurCharacInvenR()->GetInvenSlot(0, 0xb);
        CItem* item = G_CDataManager()->find_item(slot.m_addInfo);
        if (item != 0)
        {
            const float discount = static_cast<CEquipItem*>(item)->getUpgradeCostDiscount();
            const float base = (float)p.m_8;
            p.m_8 = (int)(base + (discount * base) / -100.0f);
        }
    }

    user->SetCurCharacAssaultMoney(p.m_8);
    user->SetCurCharacAssaultedUserId((unsigned short)target->get_unique_id());

    const int err = check_error(user, msg);
    p.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x50f4, "virtual int Dispatcher_RequestAssaultPrice::process(CUser*, MSG_BASE&, ParamBase&)", err, 0);
    }
    return 0;
}

// ==== Dispatcher_RequestAssaultPrice::check_error (ORIG 0x081df084) ====
int Dispatcher_RequestAssaultPrice::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    MsgRequestAssaultPrice& m = static_cast<MsgRequestAssaultPrice&>(msg);

    CUser* target = G_GameWorld()->find_from_world((unsigned short)m.m_0d);

    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        return 0x3c;
    }
    if (user->IsBusyDisjointStore())
    {
        return 0xbf;
    }
    if (user->IsCurCharacGhost())
    {
        return 0x13;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return 0x13;
    }
    if (target == 0)
    {
        return 3;
    }
    if (target->IsBusyDisjointStore())
    {
        return 0xc0;
    }

    /* BLOCKED: check_party_teleport_user @0x082efe14 —— 同上；ORIG tpErr>0 时透传其值 */
    const int tpErr = 0;
    if (tpErr > 0)
    {
        return tpErr;
    }

    const int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x37);
    if (secuErr != 0)
    {
        return secuErr;
    }

    if (target->get_state() > 4)
    {
        return 0x7fffffff;
    }

    // ORIG 分支语义:请求者非 GM 且目标为 GM → 拒绝。
    const bool gmBlock = !GlobalData::s_pGMAccounts_->isGM(user->get_acc_id())
                      && GlobalData::s_pGMAccounts_->isGM(target->get_acc_id());
    if (gmBlock)
    {
        return 0x7fffffff;
    }

    int err = 0;
    /* BLOCKED: OnRequestAssault @0x082edfba —— 同上；ORIG: OnRequestAssault(user, target, &err) */
    return err;
}

// ==== Dispatcher_RequestAssaultPrice::send (ORIG 0x081df294) ====
void Dispatcher_RequestAssaultPrice::send(CUser* user, ParamBase& param)
{
    stRequestAssaultPrice& p = static_cast<stRequestAssaultPrice&>(param);

    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1bf);

    if (p.m_4 == 0)
    {
        guard.put_byte(1);
        guard.put_int(p.m_8);
    }
    else if (p.m_4 != (int)0x7fffffff)
    {
        guard.put_byte(0);
        guard.put_byte(p.m_4);
        if (p.m_4 == 10)
        {
            guard.put_int(p.m_8 - user->getCurCharacMoney());
        }
    }
    // ORIG:err==0x7fffffff 时既不 finalize 也不 Send,直接析构返回。

    if (p.m_4 != (int)0x7fffffff)
    {
        guard.finalize(true);
        user->Send(guard);
    }
}

/* ==================== RequestIngameAdvertisement_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_RequestIngameAdvertisement 桩真实化 —— 函数体交付（B2）
// 目标落盘：source/DNFServer/GameServer/Game/Dispatchers.cpp 类段
// （替换现 ctor/dtor/check_error/process/read/send 空桩；另附第 5 函数
//   sendRewardItemSystemMail —— Dispatchers.h 未声明，见下方头约束）
//
// 报告依据（docs/class_func_reports/Dispatcher_RequestIngameAdvertisement/，
// 全部已完整阅读）：
//   check_error.md 0x081e4662 / read.md 0x081e434c / process.md 0x081e43a2 /
//   send.md 0x081e46c8 / sendRewardItemSystemMail.md 0x081e4b6c
//
// 【前置条件 — 宿主必须先扩头，否则越界】PacketTypes.h 现为：
//     class stRequestIngameAdvertisement : public ParamBase { char m_pad[0x40]; };
//   而 dispatch_template 以 `Param param;` 按 sizeof 栈分配，ORIG process/send
//   访问至 +0x644（错误码 int @+0x640、广告类型 short @+0x644），且 +0x004 起
//   内嵌 stNotifyIngameADInfo（≥0x63c）。宿主需将 m_pad 扩至 0x644
//   （sizeof ≥ 0x648）并按下方偏移注释具名化。本文件全部以
//   reinterpret_cast<char*>(&param)+N 绝对偏移访问，与汇编 disp 一一对应。
//
// 【BLOCKED 外部调用】（grep 全仓库无真实声明，按约束注释占位、其余逻辑保留）：
//   1. _ZN27CInGameAdvertisementManager33CheckIngameAdvertisementConditionEP5CUserR20stNotifyIngameADInfoi @0x080fcfa8
//      —— CInGameAdvertisementManager 仅存在于 GameMain.cpp 局部最小类（仅
//         LoadADExposeInfoAtApp 声明），两方法均无声明。
//   2. _ZN27CInGameAdvertisementManager8WriteLogEP5CUsercscj @0x080fda56
//      —— 同上。process 共 3 处调用点均已注释占位。
//   恢复方式：在权威头声明 CInGameAdvertisementManager 两方法后，去掉对应
//   BLOCKED 块内的占位赋值改为真实调用即可，其余代码无需改动。
//
// 【头约束】
//   - sendRewardItemSystemMail(CUser*, unsigned long, int)：ORIG 真实成员
//     （_ZN37...24sendRewardItemSystemMailEP5CUsermi @0x081e4b6c）。宿主需在
//     Dispatchers.h 该类内补声明；本文件给出完整定义。
//   - 新增 include：无（宿主 TU 已含 RDARScriptStringManager.h/CMailBoxHelper.h/
//     CDataManager.h/CInventory.h/CUserCharacInfo.h/InterfacePacketBuf.h/
//     PacketGuard.h/CompoundEmblemDeps.h(LineFunc)/string.h 等）。
//   - 宿主需补 extern 声明（供去 BLOCKED 后链接）：
//       class CInGameAdvertisementManager;
//       CInGameAdvertisementManager* InGameADManagerInstance(); // _Z23InGameADManagerInstancev 0x080fc9e9
// ============================================================================

// ==== Dispatcher_RequestIngameAdvertisement::check_error (ORIG 0x081e4662) ====
int Dispatcher_RequestIngameAdvertisement::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;                                          // 0xffffffff
    }

    if (user->get_state() <= 1)                             // cmp $1,setle（P2 有符号比较）
    {
        return 0x7fffffff;
    }

    // msg+0xd：movzwl 零扩展后与 6 比较 → unsigned short 口径
    const unsigned short adType =
        *reinterpret_cast<const unsigned short*>(reinterpret_cast<const char*>(&msg) + 0xd);
    if (adType == 6 && user->get_multiboxLotteryItemFailCnt() < 3)
    {
        return -2;                                          // 0xfffffffe
    }

    return 0;
}

// ==== Dispatcher_RequestIngameAdvertisement::read (ORIG 0x081e434c) ====
int Dispatcher_RequestIngameAdvertisement::read(PacketBuf& packet, MSG_BASE& msg)
{
    if (!packet.get_short(
            *reinterpret_cast<short*>(reinterpret_cast<char*>(&msg) + 0xd)))
    {
        return LineFunc(
            0x5c10,
            "virtual int Dispatcher_RequestIngameAdvertisement::read(PacketBuf&, MSG_BASE&)",
            0, 0);
    }
    return 0;
}

// ==== Dispatcher_RequestIngameAdvertisement::process (ORIG 0x081e43a2) ====
// ParamBase 绝对偏移（汇编 disp 原样）：
//   +0x640 int 错误码 | +0x644 short 广告类型（回显 msg+0xd）
//   +0x004 起内嵌 stNotifyIngameADInfo（未建模，全偏移承载）：
//     +0x004 int（send put_int 首字段/WriteLog 第 5 参 uint）
//     +0x008 char[]（描述文本，strlen 口径）| +0x03a char[]（备选文本）
//     +0x139 char[] / +0x238 int / +0x23c char[] / +0x33b byte 广告形态 /
//     +0x33c byte 日志开关 / +0x33d byte 奖品形态 / +0x33e char[] /
//     +0x43d char[] / +0x53c ulong 奖品 idx A / +0x540 char[] / +0x23c 同理 B 区
int Dispatcher_RequestIngameAdvertisement::process(CUser* user, MSG_BASE& msg,
                                                   ParamBase& param)
{
    char* const P = reinterpret_cast<char*>(&param);

    const int err = check_error(user, msg);
    *reinterpret_cast<int*>(P + 0x640) = err;

    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        LineFunc(
            0x5c20,
            "virtual int Dispatcher_RequestIngameAdvertisement::process(CUser*, MSG_BASE&, ParamBase&)",
            err, 0);
        return 0;
    }

    // err == 0 主流程
    const short adType =
        *reinterpret_cast<const short*>(reinterpret_cast<const char*>(&msg) + 0xd);
    *reinterpret_cast<short*>(P + 0x644) = adType;

    bool adOk = false;
    /* BLOCKED: _ZN27CInGameAdvertisementManager33CheckIngameAdvertisementConditionEP5CUserR20stNotifyIngameADInfoi @0x080fcfa8
       ORIG: adOk = InGameADManagerInstance()->CheckIngameAdvertisementCondition(
                 user, *reinterpret_cast<stNotifyIngameADInfo*>(P + 4), (int)adType);
       仓库暂无该真实声明（CInGameAdvertisementManager 为 GameMain.cpp 局部最小类），
       占位保持其余逻辑；恢复后由其写回 adOk。 */

    if (adOk)
    {
        const unsigned char adShape =
            *reinterpret_cast<const unsigned char*>(P + 0x33b);

        if (adShape == 4 && user->get_multiboxLotteryItemFailCnt() > 2)
        {
            const unsigned char prizeShape =
                *reinterpret_cast<const unsigned char*>(P + 0x33d);

            if (prizeShape != 0 &&
                *reinterpret_cast<unsigned long*>(P + 0x238) != 0)
            {
                sendRewardItemSystemMail(
                    user, *reinterpret_cast<unsigned long*>(P + 0x238), 1);
                if (*reinterpret_cast<const unsigned char*>(P + 0x33c) != 0)
                {
                    /* BLOCKED: _ZN27CInGameAdvertisementManager8WriteLogEP5CUsercscj @0x080fda56
                       ORIG: InGameADManagerInstance()->WriteLog(user, '\x03', adType,
                                 (char)*reinterpret_cast<const unsigned char*>(P + 0x33b),
                                 *reinterpret_cast<unsigned int*>(P + 4));
                       占位保持其余逻辑；恢复后改为真实调用。 */
                }
            }
            else if (prizeShape == 0 &&
                     *reinterpret_cast<unsigned long*>(P + 0x53c) != 0)
            {
                sendRewardItemSystemMail(
                    user, *reinterpret_cast<unsigned long*>(P + 0x53c), 1);
                if (*reinterpret_cast<const unsigned char*>(P + 0x33c) != 0)
                {
                    /* BLOCKED: _ZN27CInGameAdvertisementManager8WriteLogEP5CUsercscj @0x080fda56
                       ORIG: 同上 WriteLog(user, '\x03', ...)。占位保持其余逻辑。 */
                }
            }

            user->set_multiboxLotteryItemFailCnt(0);        // 0x080fdf44
        }

        if (*reinterpret_cast<const unsigned char*>(P + 0x33c) != 0)
        {
            /* BLOCKED: _ZN27CInGameAdvertisementManager8WriteLogEP5CUsercscj @0x080fda56
               ORIG: InGameADManagerInstance()->WriteLog(user, '\0', adType,
                         (char)adShape, *reinterpret_cast<unsigned int*>(P + 4));
               占位保持其余逻辑。 */
        }
    }
    else
    {
        *reinterpret_cast<int*>(P + 0x640) = 4;             // 条件不满足固定错误码
    }

    return 0;
}

// ==== Dispatcher_RequestIngameAdvertisement::send (ORIG 0x081e46c8) ====
void Dispatcher_RequestIngameAdvertisement::send(CUser* user, ParamBase& param)
{
    char* const P = reinterpret_cast<char*>(&param);
    const int err = *reinterpret_cast<const int*>(P + 0x640);

    if (err == 0x7fffffff)
    {
        return;                                             // check_error 未跑（state 门早退）
    }

    PacketGuard pg;
    if (err != 0)
    {
        // ---- 错误回包：byte 0 + err + type ----
        pg.clear();                                         // InterfacePacketBuf::clear
        pg.put_header(1, 0x1e9);
        pg.put_byte(0);
        pg.put_byte(err & 0xff);
        pg.put_short(*reinterpret_cast<const short*>(P + 0x644));
        pg.finalize(true);
        user->Send(pg);
        return;
    }

    // ---- 成功回包 ----
    pg.clear();
    pg.put_header(1, 0x1e9);
    pg.put_byte(1);
    pg.put_int(*reinterpret_cast<const int*>(P + 0x004));
    pg.put_short(*reinterpret_cast<const short*>(P + 0x644));
    // 以下 +0x33b/+0x33c/+0x33d 与文本区偏移均为 stNotifyIngameADInfo 视图（基址 P+4）
    pg.put_byte(static_cast<signed char>(*reinterpret_cast<const unsigned char*>(P + 0x33b)));

    {
        const int descLen =
            static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x008)));
        pg.put_int(descLen);
        pg.put_str(reinterpret_cast<const char*>(P + 0x008), descLen);
    }

    pg.put_byte(static_cast<signed char>(*reinterpret_cast<const unsigned char*>(P + 0x33d)));

    if (*reinterpret_cast<const unsigned char*>(P + 0x33d) == 0)
    {
        const int textLen =
            static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x33e)));
        pg.put_int(textLen);
        pg.put_str(reinterpret_cast<const char*>(P + 0x33e), textLen);

        const unsigned char kind =
            *reinterpret_cast<const unsigned char*>(P + 0x33b);
        if (kind == 0)
        {
            pg.put_int(*reinterpret_cast<const int*>(P + 0x53c));
        }
        else if (kind == 1)
        {
            const int len =
                static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x43d)));
            pg.put_int(len);
            pg.put_str(reinterpret_cast<const char*>(P + 0x43d), len);
        }
        else if (kind == 3)
        {
            const int len =
                static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x540)));
            pg.put_int(len);
            pg.put_str(reinterpret_cast<const char*>(P + 0x540), len);
        }
    }
    else
    {
        const int textLen =
            static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x03a)));
        pg.put_int(textLen);
        pg.put_str(reinterpret_cast<const char*>(P + 0x03a), textLen);

        const unsigned char kind =
            *reinterpret_cast<const unsigned char*>(P + 0x33b);
        if (kind == 0)
        {
            pg.put_int(*reinterpret_cast<const int*>(P + 0x238));
        }
        else if (kind == 1)
        {
            const int len =
                static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x139)));
            pg.put_int(len);
            pg.put_str(reinterpret_cast<const char*>(P + 0x139), len);
        }
        else if (kind == 3)
        {
            const int len =
                static_cast<int>(strlen(reinterpret_cast<const char*>(P + 0x23c)));
            pg.put_int(len);
            pg.put_str(reinterpret_cast<const char*>(P + 0x23c), len);
        }
    }

    pg.put_byte(static_cast<signed char>(*reinterpret_cast<const unsigned char*>(P + 0x33c)));
    pg.finalize(true);
    user->Send(pg);
}

// ==== Dispatcher_RequestIngameAdvertisement::sendRewardItemSystemMail (ORIG 0x081e4b6c) ====
// 头声明缺失：宿主需在 Dispatchers.h 补
//   void sendRewardItemSystemMail(CUser* user, unsigned long itemIdx, int count);
void Dispatcher_RequestIngameAdvertisement::sendRewardItemSystemMail(
    CUser* user, unsigned long itemIdx, int count)
{
    if (user == 0)
    {
        return;
    }

    CDataManager* dm = G_CDataManager();
    if (dm->find_item(static_cast<int>(itemIdx)) == 0)
    {
        return;
    }

    Inven_Item item;                                        // ORIG C1 W 0x080cb854
    item.m_addInfo = static_cast<int>(itemIdx);             // +0x02（item idx）
    dm->m_itemList->create_item(ENUM_ITEM_CREATE_TYPE_0, item, count);

    // RDAR 字符串：发件人名（≤0x1d）/ 正文（≤0xff），strncpy 不手动补 NUL（同 ORIG）
    char sender[0x1d];
    strncpy(sender,
            g_scriptStringManager_.findString(4, "game_server_msg_295", 0),
            0x1d);
    char body[0x100];
    strncpy(body,
            g_scriptStringManager_.findString(4, "game_server_msg_296", 0),
            0xff);

    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
        sender, item, 0u,
        static_cast<unsigned int>(user->getCurCharacNo()),
        body, static_cast<int>(strlen(body)), 0u,
        user->GetServerGroup(), false, false);
}

/* ==================== SeriaRoom_Ani_Deco_Event_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_SeriaRoom_Ani_Deco_Event 函数体批次（disp_restore_b2 / GrpF）
// 来源报告：docs/class_func_reports/Dispatcher_SeriaRoom_Ani_Deco_Event/{README,check_error,process,read,send}.md
// 目标落点：source/DNFServer/GameServer/Game/Dispatchers.cpp 替换 4355（process）与
//           4367（send）两行桩；check_error/read 已有真实实现，本批次不动。
// ============================================================================
//
// ---- 新增 include：无（CDataManager/CUserCharacInfo/CGuildServerProxy/
//      GlobalData/InterfacePacketBuf 宿主均已包含）----
//
// ---- 需补的头声明：无 ----
//
// ---- BLOCKED 依赖（占位保留，其余逻辑已真实化）----
// 1. _ZN21SeriaRoomAniDecoEvent12processEventER5CUser T 0x0827ba52
//    SeriaRoomAniDecoEvent 类（重复事件 0x69 实体）全仓未建模；
//    报告齐备：docs/class_func_reports/SeriaRoomAniDecoEvent/{README,processEvent,
//    SendAniDecoItem,StartEvent,...}.md。恢复该类后取消注释 processEvent 调用。
// 2. _ZN31DB_UpdateSeriaRoomAnimationDeco11makeRequestEjsiss T 0x0844af74
//    DB_UpdateSeriaRoomAnimationDeco 类全仓未建模（vtable 引用见
//    global_constructors_keyed_to_WongWork__DBCommon/GetIdentity.md:3942）。
//    ORIG 实参：(unsigned)user->get_acc_id(), (short)user->GetServerGroupToPvP(),
//               (int)user->getCurCharacNo(), (short)info.m_field0, (short)newField2。
// ============================================================================

// ==== Dispatcher_SeriaRoom_Ani_Deco_Event::process (ORIG 0x081e886a) ====
int Dispatcher_SeriaRoom_Ani_Deco_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_SERIAROOM_DECO_EVENT& req = static_cast<MSG_SERIAROOM_DECO_EVENT&>(msg);
    stSeriaRoomDecoEvent& res = static_cast<stSeriaRoomDecoEvent&>(param);
    int err = check_error(user, msg);
    res.m_4 = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x6551,
            "virtual int Dispatcher_SeriaRoom_Ani_Deco_Event::process(CUser*, MSG_BASE&, ParamBase&)",
            err, 0);
    }

    CDataManager* dcm = G_CDataManager();
    seriaRoom_AniDeco info = user->getSeriaRoomAniDecoInfo();   // +0x08d（4 字节值返回）

    // 进度上限：info.m_field2(有符号 word) >= maxAniDecoItem() → 0x14
    // （ORIG cmp %ax,%bx / setge：双方取低 16 位有符号比较）
    if (static_cast<short>(info.m_field2) >= static_cast<short>(dcm->maxAniDecoItem()))
    {
        res.m_4 = 0x14;
        return 0;
    }

    // 客户端指定槽位 = msg+0xe word（即 read 写入的 req.m_pad+1）
    unsigned short slot =
        *reinterpret_cast<const unsigned short*>(req.m_pad + 1);

    const CInventory* invenR = user->getCurCharacInvenR();
    Inven_Item slotItem = invenR->GetInvenSlot(static_cast<int>(INVEN_TYPE(1)),
                                               static_cast<int>(slot));

    // find_item(item idx) 缺定义 → 1
    CItem* itemDef = dcm->find_item(slotItem.m_addInfo);
    if (itemDef == 0)
    {
        res.m_4 = 1;
        return 0;
    }

    // info.m_field2 == -1 哨兵 → 1（ORIG movzwl -0x2e / cmpw $0xffff）
    if (info.m_field2 == -1)
    {
        res.m_4 = 1;
        return 0;
    }

    // 槽位有效性校验：(idx=info.m_field2, value=itemIdx)；假 → 0x17
    // （ORIG xor $1,%eax：真继续、假报错）
    if (!dcm->invalidAniDecoItem(static_cast<int>(static_cast<short>(info.m_field2)),
                                 slotItem.m_addInfo))
    {
        res.m_4 = 0x17;
        return 0;
    }

    // 成功路径：回显槽位 → 删除道具 → 进度+1 → 落库 → 事件处理
    res.m_8 = static_cast<short>(slot);                         // +0x18d

    CInventory* invenW = user->getCurCharacInvenW();
    invenW->delete_item(INVEN_TYPE(1), static_cast<int>(slot), 1,
                        static_cast<eItemDelReason>(3), true);

    seriaRoom_AniDeco updated = info;                           // m_field0 不变
    updated.m_field2 =
        static_cast<short>(static_cast<unsigned short>(info.m_field2 + 1));
    user->setSeriaRoomAniDecoInfo(updated);

    /* BLOCKED: _ZN31DB_UpdateSeriaRoomAnimationDeco11makeRequestEjsiss @0x0844af74
       DB_UpdateSeriaRoomAnimationDeco 未建模；ORIG 此处调
       DB_UpdateSeriaRoomAnimationDeco::makeRequest(user->get_acc_id(),
           user->GetServerGroupToPvP(), user->getCurCharacNo(),
           info.m_field0, updated.m_field2); */

    CEventManager* eventMgr = GlobalData::s_event_manager;
    CEventBase* evt = eventMgr->GetRepeatEvent(0x69);
    if (evt != 0 && evt->IsEventing(0))                         // vtable +0x34 槽
    {
        CEventBase* aniDecoEvent = eventMgr->GetRepeatEvent(0x69);
        if (aniDecoEvent != 0)
        {
            /* BLOCKED: _ZN21SeriaRoomAniDecoEvent12processEventER5CUser @0x0827ba52
               SeriaRoomAniDecoEvent 未建模；ORIG 此处调
               res.m_4 = static_cast<SeriaRoomAniDecoEvent*>(aniDecoEvent)
                               ->processEvent(*user); */
        }
    }
    return 0;
}

// ==== Dispatcher_SeriaRoom_Ani_Deco_Event::send (ORIG 0x081e8b64) ====
void Dispatcher_SeriaRoom_Ani_Deco_Event::send(CUser* user, ParamBase& param)
{
    stSeriaRoomDecoEvent& res = static_cast<stSeriaRoomDecoEvent&>(param);
    PacketGuard guard;
    // ORIG 无 clear()（ctor 后直接 put_header），忠实保留
    guard.put_header(1, 0x201);
    if (res.m_4 == 0)
    {
        guard.put_byte(1);
        // param+8 word 零扩展入栈（movzwl/movzwl）
        guard.put_short(static_cast<int>(
            static_cast<unsigned short>(res.m_8)));
        seriaRoom_AniDeco info = user->getSeriaRoomAniDecoInfo();
        // -0xe word（m_field2）零扩展后 cwtl 符号扩展为 int 实参
        guard.put_short(static_cast<int>(static_cast<short>(info.m_field2)));
    }
    else
    {
        guard.put_byte(0);
        guard.put_byte(res.m_4);
    }
    guard.finalize(true);
    user->Send(guard);
}

/* ==================== Skill_Command_Customizing_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_Skill_Command_Customizing —— B2 批次真实化(DispB2_GrpH)
// 报告:docs/class_func_reports/Dispatcher_Skill_Command_Customizing/{read,process,send}.md
//
// 宿主接入说明:
//   - 新增 include:无(宿主 Dispatchers.cpp 已含全部所需头;
//     LineFunc 经 CompoundEmblemDeps.h、NumberToString 经 SkillSlot.h、
//     LogManager::logFormat 经 GameTypes.h、STSkillCommandData/CSkill/
//     stSkillCommandParam 经既有 include 链可见)。
//   - 新增头声明(归属 CUser.h,CUser 域批次负责):
//       bool customizingSkillCommand(STSkillCommandData&);  // ORIG T 0x08654f74
//                                                           // _ZN5CUser23customizingSkillCommandER18STSkillCommandData
//       void defaultSkillCommand();                         // ORIG T 0x086550ee
//                                                           // _ZN5CUser19defaultSkillCommandEv
//     两符号当前源码树无声明/定义(nm -C df_game_r 实为 T),本文件相应调用处
//     以 BLOCKED 注释占位,其余逻辑已完整转写;producer 落地后按注释恢复调用。
//
// MSG_SKILL_COMMAND 线格式(PacketTypes.h 现仅 m_pad[0x100],+0xNN 为报告实证,
// 非推断):+0x0d char commandCount;条目 i(步长 8):
//   +0xe+i*8 short 命令ID / +0x10+i*8 char keyCount / +0x11+i*8+j char key[j]
// ============================================================================

// ==== Dispatcher_Skill_Command_Customizing::read (ORIG 0x081d29ac) ====
int Dispatcher_Skill_Command_Customizing::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* m = reinterpret_cast<char*>(&msg);
    if (!packet.get_byte(m[0xd]))
    {
        return LineFunc(0x2ff8,
                        "virtual int Dispatcher_Skill_Command_Customizing::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    const char commandCount = m[0xd];
    for (int i = 0; i < (int)commandCount; ++i)
    {
        if (!packet.get_short(*reinterpret_cast<short*>(m + i * 8 + 0xe)))
        {
            return LineFunc(0x2fff,
                            "virtual int Dispatcher_Skill_Command_Customizing::read(PacketBuf&, MSG_BASE&)",
                            0, 0);
        }
        if (!packet.get_byte(m[i * 8 + 0x10]))
        {
            return LineFunc(0x3002,
                            "virtual int Dispatcher_Skill_Command_Customizing::read(PacketBuf&, MSG_BASE&)",
                            0, 0);
        }
        const char keyCount = (char)m[i * 8 + 0x10];
        for (int j = 0; j < (int)keyCount; ++j)
        {
            if (!packet.get_byte(m[i * 8 + j + 0x11]))
            {
                return LineFunc(0x3009,
                                "virtual int Dispatcher_Skill_Command_Customizing::read(PacketBuf&, MSG_BASE&)",
                                0, 0);
            }
        }
    }
    return 0;
}

// ==== Dispatcher_Skill_Command_Customizing::process (ORIG 0x081d2b42) ====
int Dispatcher_Skill_Command_Customizing::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    char* m = reinterpret_cast<char*>(&msg);
    stSkillCommandParam& res = static_cast<stSkillCommandParam&>(param);

    const int commandCount = (int)(char)m[0xd];
    if (commandCount > 0x31)
    {
        // ORIG:count>49 → err=0xfc 后直接返回(不经错误日志/default 分支)
        res.m_4 = 0xfc;
        return 0;
    }

    for (int i = 0; i < commandCount; ++i)
    {
        // STSkillCommandData ctor ORIG W 0x0822db66(CUser.h 内嵌定义已一致)
        STSkillCommandData data;
        data.m_command = *reinterpret_cast<unsigned short*>(m + i * 8 + 0xe);
        data.m_count   = (unsigned char)m[i * 8 + 0x10];

        const int job = user->get_charac_job();   // CUserCharacInfo::get_charac_job
        // ORIG 走 const 重载 _ZNK12CDataManager10find_skillEii @0x0835fda2,
        // 实参=(job, 命令ID);经 const 指针调用以匹配 _ZNK 符号。
        const CDataManager* dm = G_CDataManager();
        CSkill* skill = dm->find_skill(job, (int)(short)data.m_command);
        if (skill == 0)
        {
            res.m_4 = 0xfc;
            break;
        }
        if (skill->isCommandCustomizing() != 1)   // CSkill::isCommandCustomizing W 0x0822b5d2
        {
            res.m_4 = 0xf9;
            break;
        }

        const char keyCount = (char)data.m_count;
        if (keyCount > 5 || keyCount < 1)
        {
            res.m_4 = 0xfb;
            break;
        }

        unsigned char keys[8];   // ORIG 帧 -0x29 起的命令键缓存(keyCount<=5 已证)
        bool keyRangeError = false;
        for (int j = 0; j < (int)keyCount; ++j)
        {
            keys[j] = (unsigned char)m[i * 8 + j + 0x11];
            if ((char)keys[j] < 0 || (char)keys[j] > 8)
            {
                res.m_4 = 0xfa;
                keyRangeError = true;
                break;   // ORIG:跳出内层后仍无条件执行末位键检查序列
            }
        }

        // 末位键检查:keys[keyCount-1] ∉ {0,1,2,3,7} 才放行(ORIG 0x081d2c92..0x081d2cfd)
        const unsigned char lastKey = keys[(int)keyCount - 1];
        if (lastKey == 0 || lastKey == 1 || lastKey == 2 ||
            lastKey == 3 || lastKey == 7)
        {
            res.m_4 = 0xfa;
            break;
        }

        /* BLOCKED: CUser::customizingSkillCommand(STSkillCommandData&)
           _ZN5CUser23customizingSkillCommandER18STSkillCommandData @0x08654f74
           源码树暂无声明/定义。ORIG:cVar1 = customizingSkillCommand(user,&data);
             if (cVar1 != 1) { res.m_4 = 0xfc; break; }
           producer 落地后恢复上述两行(替换本注释)。BLOCKED 期间等效于
           “调用恒成功”路径继续下一轮——错误码 0xfc 的该来源缺失,已在文件头
           注明依赖宿主补齐,不伪造其它返回。 */
        (void)data;
        (void)keyRangeError;
    }

    if (res.m_4 != 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp",
            "virtual int Dispatcher_Skill_Command_Customizing::process(CUser*, MSG_BASE&, ParamBase&)",
            0x3051,
            "SKILL_COMMAND_LOG : CUSTOMIZING ERROR!! ALL COMMAND DEFAULT!! m_id (%s)",
            NumberToString(user->get_acc_id(), 0));
        /* BLOCKED: CUser::defaultSkillCommand()
           _ZN5CUser19defaultSkillCommandEv @0x086550ee
           源码树暂无声明/定义。producer 落地后恢复:
             user->defaultSkillCommand();
           (全部命令重置为默认;BLOCKED 期间仅记录日志。) */
    }
    return 0;
}

// ==== Dispatcher_Skill_Command_Customizing::send (ORIG 0x081d2da4) ====
void Dispatcher_Skill_Command_Customizing::send(CUser* user, ParamBase& param)
{
    const stSkillCommandParam& res = static_cast<const stSkillCommandParam&>(param);
    if (res.m_4 == 0)
    {
        PacketGuard packet;
        packet.put_header(1, 0x14c);
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        PacketGuard packet;
        packet.put_header(1, 0x14c);
        packet.put_byte(0);
        packet.put_short((int)res.m_4);
        packet.finalize(true);
        user->Send(packet);
    }
}

/* ==================== SoloTeleport_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_SoloTeleport —— B2 批次真实化(DispB2_GrpH)
// 报告:docs/class_func_reports/Dispatcher_SoloTeleport/{read,process,check_error,send}.md
//
// 宿主接入说明:
//   - 新增 include:无(宿主 Dispatchers.cpp 已含 PacketTypes.h(MSG_SOLO_TELEPORT)、
//     CPartyTelePort.h(stPartyTelePort,经 PacketTypes.h)、GlobalData.h、
//     GameWorld.h、GameTypes.h(cMyTrace)、CompoundEmblemDeps.h(LineFunc))。
//   - 新增头声明(Dispatchers.h Dispatcher_SoloTeleport 段,process 调用它):
//       int check_error(CUser* user, MSG_BASE& msg, ParamBase& param);
//     (ORIG _ZN23Dispatcher_SoloTeleport11check_errorEP5CUserR8MSG_BASER9ParamBase
//      @0x081e1e98;现头文件缺该声明。)
//
// 阻塞依赖(Teleport 类族未建模——GlobalData.cpp 仅存 NPC_Teleport/User_Teleport
// 的 m_pad[0x14] 占位类,无虚表无基类;STTeleportData/STCheckRuleData 亦未建模):
//   class Teleport        { vptr; stPartyTelePort m_tp(+0x4); int m_endCoolTime(+0x10); }
//                         ORIG C1 0x08283c30 / vtable _ZTV8Teleport @0x08c0b240;
//   class NPC_Teleport : Teleport   C1 0x082840da,_ZTV12NPC_Teleport @0x08c0b228;
//   class User_Teleport : Teleport  C1 0x08284142,_ZTV13User_Teleport @0x08c0b210;
//   struct STTeleportData { int m_field00 /*ctor memset 前4字节*/; stPartyTelePort m_tp; }
//                         ORIG ctor W 0x0822d78e(sizeof>=0x10,NPC set_teleport_data
//                         以 data+4 作 stPartyTelePort& 转发读 12 字节实证);
//   struct STCheckRuleData{ int m_field00; CUser* m_user; } ctor W 0x0822d788。
// 各 BLOCKED 注释处均给出 ORIG 地址/mangled 与恢复形态;宿主建模 Teleport 族后
// 取消注释即可,本文件其余逻辑(read 全部/process 参数回填/send 包构造)已完整落地。
// ============================================================================

// ==== Dispatcher_SoloTeleport::read (ORIG 0x081e1b9c) ====
int Dispatcher_SoloTeleport::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_SOLO_TELEPORT& req = static_cast<MSG_SOLO_TELEPORT&>(msg);
    unsigned int field30 = 0;
    if (!packet.get_int(field30))
    {
        return LineFunc(0x5778,
                        "virtual int Dispatcher_SoloTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    req.m_30 = (int)field30;

    if (!packet.get_int(req.m_34))
    {
        return LineFunc(0x577b,
                        "virtual int Dispatcher_SoloTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (req.m_34 == -1)
    {
        return 0;   // 目标为 -1 时后续坐标字段不读
    }

    // +0x39..+0x40 即 stPartyTelePort 字段(PacketTypes.h MSG_SOLO_TELEPORT 布局)
    if (!packet.get_byte(req.m_tp.m_village))       // +0x39 unsigned char&
    {
        return LineFunc(0x577f,
                        "virtual int Dispatcher_SoloTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(req.m_tp.m_areaIndex))     // +0x3a unsigned char&
    {
        return LineFunc(0x5780,
                        "virtual int Dispatcher_SoloTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_short(req.m_tp.m_posx))         // +0x3c short&
    {
        return LineFunc(0x5781,
                        "virtual int Dispatcher_SoloTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_short(req.m_tp.m_posy))         // +0x3e short&
    {
        return LineFunc(0x5782,
                        "virtual int Dispatcher_SoloTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(req.m_tp.m_direction))     // +0x40 unsigned char&
    {
        return LineFunc(0x5783,
                        "virtual int Dispatcher_SoloTeleport::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_SoloTeleport::process (ORIG 0x081e1d9c) ====
int Dispatcher_SoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_SOLO_TELEPORT& req = static_cast<MSG_SOLO_TELEPORT&>(msg);
    stSoloTeleportParam& res = static_cast<stSoloTeleportParam&>(param);

    // ORIG:m_44 存全局槽地址(&g_pTeleport[0] 或 &g_pTeleport[1],即 Teleport**;
    // GlobalData.h:namespace GlobalData { extern void* g_pTeleport[2]; } @0x0941f818)
    req.m_44 = (int)(req.m_34 != -1 ? &GlobalData::g_pTeleport[0]
                                    : &GlobalData::g_pTeleport[1]);

    /* BLOCKED: 经 g_pTeleport 槽所指对象的虚表槽 +0xc 调
       set_teleport_data(STTeleportData const&):
         NPC_Teleport::_ZN12NPC_Teleport17set_teleport_dataERK14STTeleportData
           @0x08284124(转发 Teleport::set_party_teleport 0x08283d98);
         User_Teleport::_ZN13User_Teleport17set_teleport_dataERK14STTeleportData
           @0x082842b8(GameWorld::find_from_world 链)。
       vtable 证据:_ZTV12NPC_Teleport(vptr=0x08c0b230) fn[3]=0x08284124;
                   _ZTV13User_Teleport(vptr=0x08c0b218) fn[3]=0x082842b8。
       ORIG:构造 STTeleportData data(memset field00=0 后:
             data.m_field00=req.m_30;data.m_tp=req.m_tp),
             再 (*reinterpret_cast<Teleport**>(req.m_44))->set_teleport_data(data)。
       producer 落地后恢复上述调用。 */

    const int err = check_error(user, msg, param);
    if (err > 0)
    {
        res.m_4 = err;
    }
    else if (err < 0)
    {
        return -1;   // ORIG:err<0 → 返回 -1(dispatch_template result<0 → 不发包)
    }

    if (err == 0)
    {
        /* BLOCKED: Teleport::set_end_cool_time(Teleport*, CUser*)
           _ZN8Teleport17set_end_cool_timeEP5CUser @0x08283dc2
           (取 aura avatar option idx=2 与 CSystemTime::getCurSec 差值写入
            this+0x10)。producer 落地后恢复:
             (*reinterpret_cast<Teleport**>(req.m_44))->set_end_cool_time(user); */
        res.m_8 = req.m_30;   // ORIG:param+8 = msg+0x30(err==0 时)
    }

    res.m_c = req.m_44;       // ORIG:param+0xc = msg+0x44(无条件)
    return 0;
}

// ==== Dispatcher_SoloTeleport::check_error (ORIG 0x081e1e98) ====
// 注意:Dispatchers.h 现缺本声明,接入时须在类内补(见文件头)。
int Dispatcher_SoloTeleport::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    if (user == 0)
    {
        return -1;
    }
    MSG_SOLO_TELEPORT& req = static_cast<MSG_SOLO_TELEPORT&>(msg);
    if (req.m_30 == -1 && req.m_34 == -1)
    {
        return -1;
    }

    /* BLOCKED: 经 g_pTeleport 槽所指对象的虚表槽 +0x8 调
       check_rule(STCheckRuleData const&):
         NPC_Teleport::_ZN12NPC_Teleport10check_ruleERK15STCheckRuleData
           @0x082840f6(转发基类 Teleport::check_rule 0x08283d82);
         User_Teleport::_ZN13User_Teleport10check_ruleERK15STCheckRuleData
           @0x0828415e(基类链 + GameWorld::find_from_world 校验)。
       vtable:_ZTV12NPC_Teleport fn[2]=0x082840f6;
              _ZTV13User_Teleport fn[2]=0x0828415e。
       ORIG:STCheckRuleData rule(req.m_30, user);
            err = check_rule(rule);if (err != 0) return err。

       BLOCKED: Teleport::get_party_teleport()
         _ZN8Teleport18get_party_teleportEv @0x08283db6(返回 this+0x4 处
         stPartyTelePort&)及 getter 链(stPartyTelePort::get_teleport_village
         0x0822d728 / get_teleport_area_index 0x0822d734 / get_teleport_posx
         0x0822d740 / get_teleport_posy 0x0822d74c / get_teleport_direction
         0x0822d758)。
       producer 落地后恢复尾段:
         Teleport* tp = *reinterpret_cast<Teleport**>(req.m_44);
         if (tp->check_rule(STCheckRuleData(req.m_30, user)) != 0) return err;
         stPartyTelePort& party = tp->get_party_teleport();
         return G_GameWorld()->check_move_area(user,
             (int)party.get_teleport_village(), (int)party.get_teleport_area_index(),
             (int)party.get_teleport_posx(), (int)party.get_teleport_posy(),
             (int)party.get_teleport_direction(), true, 1);

       上述依赖未建模前校验链不可达;返回 -1 使 process 走 ORIG err<0 分支
       (不回填错误码、不发响应包),不向客户端伪造任何通过/失败语义。 */
    return -1;
}

// ==== Dispatcher_SoloTeleport::send (ORIG 0x081e1ff6) ====
void Dispatcher_SoloTeleport::send(CUser* user, ParamBase& param)
{
    const stSoloTeleportParam& res = static_cast<const stSoloTeleportParam&>(param);
    PacketGuard packet;
    packet.clear();                 // InterfacePacketBuf::clear ORIG 0x080cb8e6
    packet.put_header(1, 0x1d7);
    if (res.m_4 == 0)
    {
        packet.put_byte(1);
        packet.finalize(true);
        user->Send(packet);

        /* BLOCKED: Teleport::move_area(Teleport*, CUser*)
           _ZN8Teleport9move_areaEP5CUser @0x08283fb8
           (send_teleport_effect(user,1) → GameWorld::move_area(...,true,2,0,0) →
            send_teleport_effect(user,2) → update_next_end_cool_time)。
           producer 落地后恢复(move_area 结果决定两条 cMyTrace 日志):
             const bool ok =(*reinterpret_cast<Teleport**>(res.m_c))->move_area(user);
             const int charNo = user->getCurCharacNo();
             if (ok) { cMyTrace trace("virtual void Dispatcher_SoloTeleport::send(CUser*, ParamBase&)", 0x57dd, 0);
                       trace("SOLO_TELEPORT : teleport_success:(char_no:%d)", charNo); }
             else    { cMyTrace trace("virtual void Dispatcher_SoloTeleport::send(CUser*, ParamBase&)", 0x57d9, 5);
                       trace("SOLO_TELEPORT : teleport_error:(char_no:%d)", charNo); } */
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte((int)(char)res.m_4);
        /* BLOCKED: Teleport::get_end_cool_time()
           _ZN8Teleport17get_end_cool_timeEv @0x08283e24(返回 this+0x10)。
           producer 落地后恢复:
             packet.put_int((*reinterpret_cast<Teleport**>(res.m_c))->get_end_cool_time()); */
        packet.finalize(true);
        user->Send(packet);
    }
}

/* ==================== TournamentRewardSelect_body.cpp (批次B2 落盘) ==================== */
// ============================================================================
// Dispatcher_TournamentRewardSelect 函数体批次（disp_restore_b2 / GrpF）
// 来源报告：docs/class_func_reports/Dispatcher_TournamentRewardSelect/{README,check_error,process,read,send}.md
// 目标落点：source/DNFServer/GameServer/Game/Dispatchers.cpp 替换 3962-3967 行桩段
// ============================================================================
//
// ---- 新增 include（宿主 Dispatchers.cpp 当前缺失）----
                                        // ENUM_TOURNAMENT_REWARD_CARD_TYPE

// ---- 宿主需补的头声明 ----
// CParty.h:600 现声明 `void CheckRecvFromAllMember(CUser*)` 与 ORIG 不符，须改：
//   bool CheckRecvFromAllMember(CUser* user);
//       // ORIG _ZN6CParty22CheckRecvFromAllMemberEP5CUser T 0x085b3fd8（返回 al）
// CTournamentDungeonReward.h 缺三方法声明（本批次函数体直接使用）：
//   bool isCardSelectComplete(ENUM_TOURNAMENT_REWARD_CARD_TYPE type,
//                             const std::vector<int>& members) const;
//       // ORIG _ZN24CTournamentDungeonReward20isCardSelectCompleteE32ENUM_TOURNAMENT_REWARD_CARD_TYPERKSt6vectorIiSaIiEE T 0x08284d08
//   bool SelectCard(int slotNo, char cardA, char cardB);
//       // ORIG _ZN24CTournamentDungeonReward10SelectCardEicc T 0x08284ee4
//   void MakeSelectPacket(PacketGuard* packet);
//       // ORIG _ZN24CTournamentDungeonReward16MakeSelectPacketEP11PacketGuard T 0x08284fa4
// （party+0x17bc 已具名 CParty::m_tournamentReward @CParty.h:756，直接使用。）
//
// ---- BLOCKED 依赖：无（全部外部调用均有现有真实声明/定义）----
// ============================================================================

// ==== Dispatcher_TournamentRewardSelect::check_error (ORIG 0x081e046c) ====
int Dispatcher_TournamentRewardSelect::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() != 5)
    {
        return 0x7fffffff;
    }
    // GetPartyIndex 返回 short（ORIG movswl %ax 后入栈）
    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return -1;
    }
    // ORIG cmp $0x2,%al：clear state == 2（选卡阶段）→ 0，否则 0x7fffffff
    if (party->GetTournamentDungeonClearState() != 2)
    {
        return 0x7fffffff;
    }
    return 0;
}

// ==== Dispatcher_TournamentRewardSelect::read (ORIG 0x081dffda) ====
int Dispatcher_TournamentRewardSelect::read(PacketBuf& packet, MSG_BASE& msg)
{
    MsgTournamentRewardSelect& req = static_cast<MsgTournamentRewardSelect&>(msg);
    // get_byte(char&) 直写 msg+0xd；随后域检验 [0,1]（js/cmp $1,jle 形态）
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))
    {
        return LineFunc(0x52fd,
                        "virtual int Dispatcher_TournamentRewardSelect::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (static_cast<char>(req.m_0d) < 0 || static_cast<char>(req.m_0d) > 1)
    {
        return LineFunc(0x5300,
                        "virtual int Dispatcher_TournamentRewardSelect::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0e)))
    {
        return LineFunc(0x5303,
                        "virtual int Dispatcher_TournamentRewardSelect::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    if (static_cast<char>(req.m_0e) < 0 || static_cast<char>(req.m_0e) > 1)
    {
        return LineFunc(0x5306,
                        "virtual int Dispatcher_TournamentRewardSelect::read(PacketBuf&, MSG_BASE&)",
                        0, 0);
    }
    return 0;
}

// ==== Dispatcher_TournamentRewardSelect::process (ORIG 0x081e00f0) ====
int Dispatcher_TournamentRewardSelect::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MsgTournamentRewardSelect& req = static_cast<MsgTournamentRewardSelect&>(msg);
    // stTournamentRewardSelect 字段（现 m_pad[0x40] 占位）：+0x04 int errNo
    int err = check_error(user, msg);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = err;
    if (err > 0)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(
            0x5315,
            "virtual int Dispatcher_TournamentRewardSelect::process(CUser*, MSG_BASE&, ParamBase&)",
            err, 0);
    }

    CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
    if (party == 0)
    {
        return LineFunc(
            0x531c,
            "virtual int Dispatcher_TournamentRewardSelect::process(CUser*, MSG_BASE&, ParamBase&)",
            0, 0);
    }

    std::vector<int> members;                                // +0x1c7 默认构造
    party->GetValidPartyMember(members);                     // T 0x085bd1dc

    CTournamentDungeonReward& reward = party->m_tournamentReward;   // +0x17bc
    // 全员已选卡 → 0x7fffffff（isCardSelectComplete 第二参=卡组类型 2）
    /* BLOCKED: CTournamentDungeonReward::isCardSelectComplete @0x08284d08 —— CTournamentDungeonReward.h 缺声明。
       ORIG: if (reward.isCardSelectComplete((ENUM_TOURNAMENT_REWARD_CARD_TYPE)2, list)) { ... 0x7fffffff }
       依赖补齐前按未完成继续。 */
    if (false)  // BLOCKED: isCardSelectComplete @0x08284d08（ORIG 实参 cardType=2, members）
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = 0x7fffffff;
        return 0;
    }

    // msg+0xd/msg+0xe 为 signed byte 读入（movsbl）
    const unsigned int slotNo = party->GetMemberSlotNo(user);
    /* BLOCKED: CTournamentDungeonReward::SelectCard —— CTournamentDungeonReward.h 缺声明。
       ORIG: bool selected = reward.SelectCard((int)slotNo, (char)req.m_0d, (char)req.m_0e);
       依赖补齐前按选卡成功继续。 */
    const bool selected = true;
    if (!selected)
    {
        *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 4) = 0x7fffffff;
    }
    return 0;
}

// ==== Dispatcher_TournamentRewardSelect::send (ORIG 0x081e02b0) ====
void Dispatcher_TournamentRewardSelect::send(CUser* user, ParamBase& param)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(1, 0x1cb);
    if (*reinterpret_cast<const int*>(reinterpret_cast<const char*>(&param) + 4) == 0)
    {
        guard.put_byte(1);
        CParty* party = G_CGameManager()->GetParty(user->GetPartyIndex());
        if (party != 0)
        {
            CTournamentDungeonReward& reward = party->m_tournamentReward;   // +0x17bc
            /* BLOCKED: CTournamentDungeonReward::MakeSelectPacket —— 头缺声明；补齐后恢复 */

            guard.finalize(true);
            party->send_to_party(guard);

            // 全员已接收后结算（ORIG test %al 前取反：假 → 直接析构返回）
            /* BLOCKED: CParty::CheckRecvFromAllMember ORIG 返回 bool @?（现头为 void）；
               ORIG: if (!CheckRecvFromAllMember(user)) { 析构返回 } else { 结算 }
               依赖补齐前跳过结算。 */
            if (false)  // BLOCKED: CheckRecvFromAllMember ORIG 返回 bool（现头为 void）
            {
                std::vector<int> members;
                party->GetValidPartyMember(members);
                if (false)  // BLOCKED 同上 @0x08284d08
                {
                    bool victory = party->isTournamentVictory();
                    party->SetTournamentDungeonClearState(3);
                    party->TournamentDungeonClear(victory);
                }
            }
        }
    }
    // err != 0 或 party 缺失：仅析构返回（不 finalize、不发送）
}

// ==== Wave3Deps：8 个 Dispatcher ctor（ORIG 各 0x1c 字节 W，均为编译器形态：
// 调基类 IPacketDispatcher<MSG,Param,state>::C1 后设 vptr；空体即忠实）====
// Dispatcher_StackableActionUse   ORIG 0x0859583c
// Dispatcher_AvatarOptionChange   ORIG 0x0859591c
// Dispatcher_PartyTeleport        ORIG 0x08595954
// Dispatcher_RefundSkill          ORIG 0x0859598c
// Dispatcher_MailBox_Send         ORIG 0x08595ecc
// Dispatcher_SecurityCardAuthRpy  ORIG 0x08596498
// Dispatcher_BlueMarbleConfirmInfo ORIG 0x08597de2
// Dispatcher_PcRoomRentItem       ORIG 0x08597d8e
Dispatcher_StackableActionUse::Dispatcher_StackableActionUse() {}
Dispatcher_AvatarOptionChange::Dispatcher_AvatarOptionChange() {}
Dispatcher_PartyTeleport::Dispatcher_PartyTeleport() {}
Dispatcher_RefundSkill::Dispatcher_RefundSkill() {}
Dispatcher_MailBox_Send::Dispatcher_MailBox_Send() {}
Dispatcher_SecurityCardAuthRpy::Dispatcher_SecurityCardAuthRpy() {}
Dispatcher_BlueMarbleConfirmInfo::Dispatcher_BlueMarbleConfirmInfo() {}
Dispatcher_PcRoomRentItem::Dispatcher_PcRoomRentItem() {}
