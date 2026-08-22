#include "Dispatchers.h"

#include "CDataManager.h"
#include "CDailyScheduleManager.h"
#include "CGameManager.h"
#include "CInventory.h"
#include "CPrivateStoreMgr.h"
#include "CStreamGuard.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"

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
        altar_pkt::ReqPause req;
        altar_pkt::ResPause res;
        read(packet, *(MSG_BASE*)&req);
        process(user, *(MSG_BASE*)&req, *(ParamBase*)&res);
        send_packet(user, *(CMDPacketStruct::STBaseResponse*)&res);
        return 0;
    }
    return 0x52c;
}
int Dispatcher_Pause::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    altar_pkt::ReqPause& req = *reinterpret_cast<altar_pkt::ReqPause*>(&msg);
    altar_pkt::ResPause& res = *reinterpret_cast<altar_pkt::ResPause*>(&param);
    res.m_nErrorCode = 0;  // check_error 恒 0（ORIG 0x8140b32）
    if (res.m_nErrorCode == 0)
    {
        res.m_bPause = req.m_data[0x0d];  // +0x08 = req+0x0d
        // ORIG Manager::pause @ 0x813105c
        procStagePause(getUserStageControl(user), req.m_data[0x0d] != 0);
    }
    return 0;
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
        altar_pkt::ReqBuyItem req;
        altar_pkt::ResBuyItem res;
        read(packet, *(MSG_BASE*)&req);
        process(user, *(MSG_BASE*)&req, *(ParamBase*)&res);
        send_packet(user, *(CMDPacketStruct::STBaseResponse*)&res);
        return 0;
    }
    return 0x380;
}
int Dispatcher_BuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    altar_pkt::ReqBuyItem& req = *reinterpret_cast<altar_pkt::ReqBuyItem*>(&msg);
    altar_pkt::ResBuyItem& res = *reinterpret_cast<altar_pkt::ResBuyItem*>(&param);
    // ORIG process_packet @ 0x813fc06：回显 + check_error + buyItem
    res.m_nBuyIndex = *(int*)&req.m_data[0x10];
    res.m_nShopType = *(int*)&req.m_data[0x14];
    res.m_nLevel = *(int*)&req.m_data[0x18];
    res.m_wFrom = *(unsigned short*)&req.m_data[0x1c];
    res.m_wTo = *(unsigned short*)&req.m_data[0x1e];
    res.m_nErrorCode = 0;  // check_error 恒 0
    if (res.m_nErrorCode == 0)
    {
        // [推断] ORIG buyItem @ 0x81319ae：需 CharacAdvanceAltarManager
        // （CUser+0x8df60，当前 CUser.h 为 pad[0xd4]）内部 AdvanceAltarData
        // 布局（+0x14 资金、getBuyItemMap）；未建模，暂置 error=3（数据缺失）。
        res.m_nErrorCode = 3;
        res.m_nTotalCost = 0;
    }
    return 0;
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
        altar_pkt::ReqSetSlot req;
        altar_pkt::ResSetSlot res;
        read(packet, *(MSG_BASE*)&req);
        process(user, *(MSG_BASE*)&req, *(ParamBase*)&res);
        send_packet(user, *(CMDPacketStruct::STBaseResponse*)&res);
        return 0;
    }
    return 0x3fc;
}
int Dispatcher_SetSlot::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    altar_pkt::ReqSetSlot& req = *reinterpret_cast<altar_pkt::ReqSetSlot*>(&msg);
    altar_pkt::ResSetSlot& res = *reinterpret_cast<altar_pkt::ResSetSlot*>(&param);
    res.m_bSet = req.m_data[0x14];       // +0x0c = req+0x14
    res.m_field08 = *(int*)&req.m_data[0x10];  // +0x08 = req+0x10
    memcpy(res.m_slot, &req.m_data[0x18], 12); // +0x10 ← req+0x18
    res.m_nErrorCode = 0;                // check_error 恒 0
    if (res.m_nErrorCode == 0)
    {
        int slotType = *(int*)&req.m_data[0x18];
        if (slotType == 1 || slotType == 2)
        {
            // [推断] ORIG setSlot @ 0x8131b58 依赖未建模 AdvanceAltarData /
            // AdvanceAltarShopParameter::getBuyUpgradeDataVec，暂置 error=3。
            res.m_nErrorCode = 3;
        }
        else
        {
            res.m_nErrorCode = 2;
        }
    }
    return 0;
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
        altar_pkt::ReqResetStar req;
        altar_pkt::ResResetStar res;
        read(packet, *(MSG_BASE*)&req);
        process(user, *(MSG_BASE*)&req, *(ParamBase*)&res);
        send_packet(user, *(CMDPacketStruct::STBaseResponse*)&res);
        return 0;
    }
    return 0x5c0;
}
int Dispatcher_ResetStar::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    altar_pkt::ReqResetStar& req = *reinterpret_cast<altar_pkt::ReqResetStar*>(&msg);
    altar_pkt::ResResetStar& res = *reinterpret_cast<altar_pkt::ResResetStar*>(&param);
    res.m_nErrorCode = 0;  // check_error 恒 0
    if (res.m_nErrorCode == 0)
    {
        // [推断] ORIG resetStar @ CharacAdvanceAltarManager 依赖未建模
        // AdvanceAltarData（+0xcc 更新标志/ridable 数据），暂置 error=3。
        res.m_nErrorCode = 3;
    }
    return 0;
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
        altar_pkt::ReqSummonUnit req;
        altar_pkt::ResSummonUnit res;
        read(packet, *(MSG_BASE*)&req);
        process(user, *(MSG_BASE*)&req, *(ParamBase*)&res);
        send_packet(user, *(CMDPacketStruct::STBaseResponse*)&res);
        return 0;
    }
    return 0x496;
}
int Dispatcher_SummonUnit::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    altar_pkt::ReqSummonUnit& req = *reinterpret_cast<altar_pkt::ReqSummonUnit*>(&msg);
    altar_pkt::ResSummonUnit& res = *reinterpret_cast<altar_pkt::ResSummonUnit*>(&param);
    res.m_bySlotType = req.m_data[0x0d];   // +0x08 = req+0x0d
    res.m_nUnitId = *(int*)&req.m_data[0x10];  // +0x0c = req+0x10
    res.m_nErrorCode = 0;                  // check_error 恒 0
    if (res.m_nErrorCode == 0)
    {
        advancealtar::StageControl* stage = getUserStageControl(user);
        char* cd = getCharacData(stage);
        if (cd != 0)
        {
            if (*(int*)&req.m_data[0x14] == characDataGetGageLevel(cd))
            {
                res.m_nErrorCode =
                    characDataSummonSlotUnit(cd, req.m_data[0x0d], *(int*)&req.m_data[0x10]);
                res.m_nGage = characDataGetGage(cd);   // +0x10
            }
            else
            {
                res.m_nErrorCode = 3;
            }
        }
        else
        {
            res.m_nErrorCode = 3;
        }
    }
    return 0;
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
        altar_pkt::ReqUpgradeGage req;
        altar_pkt::ResUpgradeGage res;
        read(packet, *(MSG_BASE*)&req);
        process(user, *(MSG_BASE*)&req, *(ParamBase*)&res);
        send_packet(user, *(CMDPacketStruct::STBaseResponse*)&res);
        return 0;
    }
    return 0x446;
}
int Dispatcher_UpgradeGage::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    altar_pkt::ReqUpgradeGage& req = *reinterpret_cast<altar_pkt::ReqUpgradeGage*>(&msg);
    altar_pkt::ResUpgradeGage& res = *reinterpret_cast<altar_pkt::ResUpgradeGage*>(&param);
    res.m_nErrorCode = 0;  // check_error 恒 0
    if (res.m_nErrorCode == 0)
    {
        advancealtar::StageControl* stage = getUserStageControl(user);
        char* cd = getCharacData(stage);
        if (cd != 0)
        {
            res.m_nGageLevelBefore = characDataGetGageLevel(cd);  // +0x08
            res.m_nErrorCode = characDataUpgreadGage(cd);          // +0x04
            res.m_nGageLevelAfter = characDataGetGageLevel(cd);    // +0x0c
            res.m_nGage = characDataGetGage(cd);                   // +0x10
        }
        else
        {
            res.m_nErrorCode = 3;
        }
    }
    return 0;
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
        altar_pkt::ReqExchangeSlot req;
        altar_pkt::ResExchangeSlot res;
        read(packet, *(MSG_BASE*)&req);
        process(user, *(MSG_BASE*)&req, *(ParamBase*)&res);
        send_packet(user, *(CMDPacketStruct::STBaseResponse*)&res);
        return 0;
    }
    return 0x4e8;
}
int Dispatcher_ExchangeSlot::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    altar_pkt::ReqExchangeSlot& req = *reinterpret_cast<altar_pkt::ReqExchangeSlot*>(&msg);
    altar_pkt::ResExchangeSlot& res = *reinterpret_cast<altar_pkt::ResExchangeSlot*>(&param);
    res.m_field08 = *(int*)&req.m_data[0x10];  // +0x08 = req+0x10
    res.m_nErrorCode = 0;                  // check_error 恒 0
    if (res.m_nErrorCode == 0)
    {
        // [推断] ORIG exchangeSlot @ 0x8131e36 依赖未建模 AdvanceAltarData，
        // 暂置 error=3；回显两个 _SetSlotData 仍按 ORIG 执行。
        res.m_nErrorCode = 3;
        memcpy(res.m_slot1, &req.m_data[0x18], 12);
        memcpy(res.m_slot2, &req.m_data[0x24], 12);
    }
    return 0;
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
        altar_pkt::ReqGetAchievementReward req;
        altar_pkt::ResGetAchievementReward res;
        read(packet, *(MSG_BASE*)&req);
        process(user, *(MSG_BASE*)&req, *(ParamBase*)&res);
        send_packet(user, *(CMDPacketStruct::STBaseResponse*)&res);
        return 0;
    }
    return 0x571;
}
int Dispatcher_GetAchievementReward::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG process_packet @ 0x8140da0：空实现（不读取请求、不写响应）。
    return 0;
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

}
Dispatcher_Redeem::Dispatcher_Redeem() {}
Dispatcher_Redeem::~Dispatcher_Redeem() {}
int Dispatcher_Redeem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Redeem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Redeem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseDye::Dispatcher_UseDye() {}
Dispatcher_UseDye::~Dispatcher_UseDye() {}
int Dispatcher_UseDye::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_UseDye::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseDye::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

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
int Dispatcher_Teleport::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Teleport::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Teleport::send(CUser* user, ParamBase& param) {}

Dispatcher_UseJewel::Dispatcher_UseJewel() {}
Dispatcher_UseJewel::~Dispatcher_UseJewel() {}
int Dispatcher_UseJewel::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_DieMobAll::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DieMobAll::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DieMobAll::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SkillInit::Dispatcher_SkillInit() {}
Dispatcher_SkillInit::~Dispatcher_SkillInit() {}
int Dispatcher_SkillInit::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_SkillInit::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SkillInit::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SkillInit::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_BlueMarble::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_BlueMarble::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BlueMarble::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_BlueMarble::send(CUser* user, ParamBase& param) {}

Dispatcher_ChangeHost::Dispatcher_ChangeHost() {}
Dispatcher_ChangeHost::~Dispatcher_ChangeHost() {}
int Dispatcher_ChangeHost::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ChangeHost::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeHost::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ComboSkill::Dispatcher_ComboSkill() {}
Dispatcher_ComboSkill::~Dispatcher_ComboSkill() {}
int Dispatcher_ComboSkill::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ComboSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ComboSkill::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ComboSkill::send(CUser* user, ParamBase& param) {}

Dispatcher_GM_Command::Dispatcher_GM_Command() {}
Dispatcher_GM_Command::~Dispatcher_GM_Command() {}
int Dispatcher_GM_Command::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_GM_Command::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GM_Command::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GuildCargo::Dispatcher_GuildCargo() {}
Dispatcher_GuildCargo::~Dispatcher_GuildCargo() {}
int Dispatcher_GuildCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GuildCargo::send(CUser* user, ParamBase& param) {}

Dispatcher_Join_Power::Dispatcher_Join_Power() {}
Dispatcher_Join_Power::~Dispatcher_Join_Power() {}
int Dispatcher_Join_Power::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Join_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Join_Power::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MotionHack::Dispatcher_MotionHack() {}
Dispatcher_MotionHack::~Dispatcher_MotionHack() {}
int Dispatcher_MotionHack::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MotionHack::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MoveToGate::Dispatcher_MoveToGate() {}
Dispatcher_MoveToGate::~Dispatcher_MoveToGate() {}
int Dispatcher_MoveToGate::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_MoveToGate::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MoveToGate::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PurifyItem::Dispatcher_PurifyItem() {}
Dispatcher_PurifyItem::~Dispatcher_PurifyItem() {}
int Dispatcher_PurifyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PurifyItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RedeemList::Dispatcher_RedeemList() {}
Dispatcher_RedeemList::~Dispatcher_RedeemList() {}
int Dispatcher_RedeemList::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RedeemList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RedeemList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SelectItem::Dispatcher_SelectItem() {}
Dispatcher_SelectItem::~Dispatcher_SelectItem() {}
int Dispatcher_SelectItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SelectItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SelectItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TraceError::Dispatcher_TraceError() {}
Dispatcher_TraceError::~Dispatcher_TraceError() {}
int Dispatcher_TraceError::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_TraceError::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TraceError::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TryEnchant::Dispatcher_TryEnchant() {}
Dispatcher_TryEnchant::~Dispatcher_TryEnchant() {}
int Dispatcher_TryEnchant::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_TryEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TryEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_VerifyGold::Dispatcher_VerifyGold() {}
Dispatcher_VerifyGold::~Dispatcher_VerifyGold() {}
int Dispatcher_VerifyGold::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_ModItemAttr::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ModItemAttr::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ModItemAttr::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RefundSkill::Dispatcher_RefundSkill() {}
Dispatcher_RefundSkill::~Dispatcher_RefundSkill() {}
int Dispatcher_RefundSkill::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_RefundSkill::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RefundSkill::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_RefundSkill::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_UpgradeItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UpgradeItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AgreeEnchant::Dispatcher_AgreeEnchant() {}
Dispatcher_AgreeEnchant::~Dispatcher_AgreeEnchant() {}
int Dispatcher_AgreeEnchant::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AgreeEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AgreeEnchant::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Back2Village::Dispatcher_Back2Village() {}
Dispatcher_Back2Village::~Dispatcher_Back2Village() {}
int Dispatcher_Back2Village::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Back2Village::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Back2Village::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BossDieCheck::Dispatcher_BossDieCheck() {}
Dispatcher_BossDieCheck::~Dispatcher_BossDieCheck() {}
int Dispatcher_BossDieCheck::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BossDieCheck::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_BossDieCheck::send(CUser* user, ParamBase& param) {}

Dispatcher_CodeCheckSum::Dispatcher_CodeCheckSum() {}
Dispatcher_CodeCheckSum::~Dispatcher_CodeCheckSum() {}
int Dispatcher_CodeCheckSum::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CodeCheckSum::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CodeCheckSum::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CollectItems::Dispatcher_CollectItems() {}
Dispatcher_CollectItems::~Dispatcher_CollectItems() {}
int Dispatcher_CollectItems::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CollectItems::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_CollectItems::send(CUser* user, ParamBase& param) {}

Dispatcher_DepositMoney::Dispatcher_DepositMoney() {}
Dispatcher_DepositMoney::~Dispatcher_DepositMoney() {}
int Dispatcher_DepositMoney::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DepositMoney::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DepositMoney::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EvadeAssault::Dispatcher_EvadeAssault() {}
Dispatcher_EvadeAssault::~Dispatcher_EvadeAssault() {}
int Dispatcher_EvadeAssault::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_InformNotice::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_InformNotice::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_InformNotice::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_InformNotice::send(CUser* user, ParamBase& param) {}

Dispatcher_JoinPowerWar::Dispatcher_JoinPowerWar() {}
Dispatcher_JoinPowerWar::~Dispatcher_JoinPowerWar() {}
int Dispatcher_JoinPowerWar::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_JoinPowerWar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_JoinPowerWar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_MailBox_Open::Dispatcher_MailBox_Open() {}
Dispatcher_MailBox_Open::~Dispatcher_MailBox_Open() {}
int Dispatcher_MailBox_Open::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_MailBox_Open::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MailBox_Open::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_MailBox_Open::send(CUser* user, ParamBase& param) {}

Dispatcher_MailBox_Send::Dispatcher_MailBox_Send() {}
Dispatcher_MailBox_Send::~Dispatcher_MailBox_Send() {}
int Dispatcher_MailBox_Send::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_MailBox_Send::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MailBox_Send::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_MailBox_Send::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_ScanBotByDll::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ScanBotByDll::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ScanBotByDll::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ScanBotByDrv::Dispatcher_ScanBotByDrv() {}
Dispatcher_ScanBotByDrv::~Dispatcher_ScanBotByDrv() {}
int Dispatcher_ScanBotByDrv::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ScanBotByDrv::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ScanBotByDrv::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Secede_Power::Dispatcher_Secede_Power() {}
Dispatcher_Secede_Power::~Dispatcher_Secede_Power() {}
int Dispatcher_Secede_Power::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Secede_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Secede_Power::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SoloTeleport::Dispatcher_SoloTeleport() {}
Dispatcher_SoloTeleport::~Dispatcher_SoloTeleport() {}
int Dispatcher_SoloTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SoloTeleport::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SoloTeleport::send(CUser* user, ParamBase& param) {}

Dispatcher_TitleBookGet::Dispatcher_TitleBookGet() {}
Dispatcher_TitleBookGet::~Dispatcher_TitleBookGet() {}
int Dispatcher_TitleBookGet::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TitleBookGet::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_TitleBookPut::Dispatcher_TitleBookPut() {}
Dispatcher_TitleBookPut::~Dispatcher_TitleBookPut() {}
int Dispatcher_TitleBookPut::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TitleBookPut::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeCargo::Dispatcher_UpgradeCargo() {}
Dispatcher_UpgradeCargo::~Dispatcher_UpgradeCargo() {}
int Dispatcher_UpgradeCargo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UpgradeCargo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeCargo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UseLimitCube::Dispatcher_UseLimitCube() {}
Dispatcher_UseLimitCube::~Dispatcher_UseLimitCube() {}
int Dispatcher_UseLimitCube::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseLimitCube::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_UseLimitCube::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_HatchCreature::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_HatchCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_HatchCreature::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
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
int Dispatcher_ObjectBringUp::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ObjectBringUp::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ObjectBringUp::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ObjectBringUp::send(CUser* user, ParamBase& param) {}

Dispatcher_Overflow_Info::Dispatcher_Overflow_Info() {}
Dispatcher_Overflow_Info::~Dispatcher_Overflow_Info() {}
int Dispatcher_Overflow_Info::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Overflow_Info::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Overflow_Info::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PartyTeleport::Dispatcher_PartyTeleport() {}
Dispatcher_PartyTeleport::~Dispatcher_PartyTeleport() {}
int Dispatcher_PartyTeleport::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_PartyTeleport::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PartyTeleport::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_PartyTeleport::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_UseRenameCard::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_UseRenameCard::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseRenameCard::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_WithdrawMoney::Dispatcher_WithdrawMoney() {}
Dispatcher_WithdrawMoney::~Dispatcher_WithdrawMoney() {}
int Dispatcher_WithdrawMoney::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_CallMemberList::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_CallMemberList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CallMemberList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CheckJoinGuild::Dispatcher_CheckJoinGuild() {}
Dispatcher_CheckJoinGuild::~Dispatcher_CheckJoinGuild() {}
int Dispatcher_CheckJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CheckJoinGuild::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_CompoundEmblem::Dispatcher_CompoundEmblem() {}
Dispatcher_CompoundEmblem::~Dispatcher_CompoundEmblem() {}
int Dispatcher_CompoundEmblem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_CompoundEmblem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CompoundEmblem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DisJointAvatar::Dispatcher_DisJointAvatar() {}
Dispatcher_DisJointAvatar::~Dispatcher_DisJointAvatar() {}
int Dispatcher_DisJointAvatar::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DisJointAvatar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DisJointAvatar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_DisguiseCancel::Dispatcher_DisguiseCancel() {}
Dispatcher_DisguiseCancel::~Dispatcher_DisguiseCancel() {}
int Dispatcher_DisguiseCancel::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_DisguiseCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DisguiseCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_HellPartyStart::Dispatcher_HellPartyStart() {}
Dispatcher_HellPartyStart::~Dispatcher_HellPartyStart() {}
int Dispatcher_HellPartyStart::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_HellPartyStart::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_HellPartyStart::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ItemDictionary::Dispatcher_ItemDictionary() {}
Dispatcher_ItemDictionary::~Dispatcher_ItemDictionary() {}
int Dispatcher_ItemDictionary::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_Mercenary_Info::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Mercenary_Info::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Mercenary_Info::send(CUser* user, ParamBase& param) {}

Dispatcher_P2P_Statistics::Dispatcher_P2P_Statistics() {}
Dispatcher_P2P_Statistics::~Dispatcher_P2P_Statistics() {}
int Dispatcher_P2P_Statistics::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_P2P_Statistics::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_P2P_Statistics::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PcRoomRentItem::Dispatcher_PcRoomRentItem() {}
Dispatcher_PcRoomRentItem::~Dispatcher_PcRoomRentItem() {}
int Dispatcher_PcRoomRentItem::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_PcRoomRentItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PcRoomRentItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_PcRoomRentItem::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_RenameCreature::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_RenameCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RenameCreature::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_RenameCreature::send(CUser* user, ParamBase& param) {}

Dispatcher_UseBoosterItem::Dispatcher_UseBoosterItem() {}
Dispatcher_UseBoosterItem::~Dispatcher_UseBoosterItem() {}
int Dispatcher_UseBoosterItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UseBoosterItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseBoosterItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UserHistoryLog::Dispatcher_UserHistoryLog() {}
Dispatcher_UserHistoryLog::~Dispatcher_UserHistoryLog() {}
int Dispatcher_UserHistoryLog::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UserHistoryLog::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BuyCeraShopItem::Dispatcher_BuyCeraShopItem() {}
Dispatcher_BuyCeraShopItem::~Dispatcher_BuyCeraShopItem() {}
int Dispatcher_BuyCeraShopItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_BuyCeraShopItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BuyCeraShopItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

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
int Dispatcher_CeraPackageOpen::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_CeraPackageOpen::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_CeraPackageOpen::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

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
int Dispatcher_DisguiseRequest::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_GoblinPadStatus::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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

Dispatcher_MonstercardBind::Dispatcher_MonstercardBind() {}
Dispatcher_MonstercardBind::~Dispatcher_MonstercardBind() {}
int Dispatcher_MonstercardBind::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_MonstercardBind::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MonstercardBind::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_MonstercardBind::send(CUser* user, ParamBase& param) {}

Dispatcher_PVPRequestFight::Dispatcher_PVPRequestFight() {}
Dispatcher_PVPRequestFight::~Dispatcher_PVPRequestFight() {}
int Dispatcher_PVPRequestFight::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
Dispatcher_SaveQuestNotify::~Dispatcher_SaveQuestNotify() {}
int Dispatcher_SaveQuestNotify::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_SaveQuestNotify::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SaveQuestNotify::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SecuDataControl::Dispatcher_SecuDataControl() {}
Dispatcher_SecuDataControl::~Dispatcher_SecuDataControl() {}
int Dispatcher_SecuDataControl::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SecuDataControl::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecuDataControl::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_IPScan::Dispatcher_Security_IPScan() {}
Dispatcher_Security_IPScan::~Dispatcher_Security_IPScan() {}
int Dispatcher_Security_IPScan::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_ChangeLetterStat::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_Equipslot_Switch::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_Equipslot_Switch::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Equipslot_Switch::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Equipslot_Switch::send(CUser* user, ParamBase& param) {}

Dispatcher_ExitPrivateStore::Dispatcher_ExitPrivateStore() {}
Dispatcher_ExitPrivateStore::~Dispatcher_ExitPrivateStore() {}
int Dispatcher_ExitPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ExitPrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ExitPrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ExpertExtraction::Dispatcher_ExpertExtraction() {}
Dispatcher_ExpertExtraction::~Dispatcher_ExpertExtraction() {}
int Dispatcher_ExpertExtraction::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ExpertExtraction::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ExpertExtraction::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ExpertExtraction::send(CUser* user, ParamBase& param) {}

Dispatcher_InformNotice_2nd::Dispatcher_InformNotice_2nd() {}
Dispatcher_InformNotice_2nd::~Dispatcher_InformNotice_2nd() {}
int Dispatcher_InformNotice_2nd::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_InformNotice_2nd::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_InformNotice_2nd::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_InformNotice_2nd::send(CUser* user, ParamBase& param) {}

Dispatcher_MemberEnterReply::Dispatcher_MemberEnterReply() {}
Dispatcher_MemberEnterReply::~Dispatcher_MemberEnterReply() {}
int Dispatcher_MemberEnterReply::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_MemberEnterReply::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_MemberEnterReply::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Mercenary_Return::Dispatcher_Mercenary_Return() {}
Dispatcher_Mercenary_Return::~Dispatcher_Mercenary_Return() {}
int Dispatcher_Mercenary_Return::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Mercenary_Return::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Mercenary_Return::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_ReportBadP2PUser::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ReportBadP2PUser::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ReportBadP2PUser::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Req_Urgent_Quest::Dispatcher_Req_Urgent_Quest() {}
Dispatcher_Req_Urgent_Quest::~Dispatcher_Req_Urgent_Quest() {}
int Dispatcher_Req_Urgent_Quest::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Req_Urgent_Quest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Req_Urgent_Quest::send(CUser* user, ParamBase& param) {}

Dispatcher_RequestBlackList::Dispatcher_RequestBlackList() {}
Dispatcher_RequestBlackList::~Dispatcher_RequestBlackList() {}
int Dispatcher_RequestBlackList::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_RequestBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestBlackList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestJoinGuild::Dispatcher_RequestJoinGuild() {}
Dispatcher_RequestJoinGuild::~Dispatcher_RequestJoinGuild() {}
int Dispatcher_RequestJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestJoinGuild::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ResponseCreature::Dispatcher_ResponseCreature() {}
Dispatcher_ResponseCreature::~Dispatcher_ResponseCreature() {}
int Dispatcher_ResponseCreature::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ResponseCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ResponseCreature::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SDC_Damage_Check::Dispatcher_SDC_Damage_Check() {}
Dispatcher_SDC_Damage_Check::~Dispatcher_SDC_Damage_Check() {}
int Dispatcher_SDC_Damage_Check::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SDC_Damage_Check::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SDC_Damage_Check::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SaveGameOption_1::Dispatcher_SaveGameOption_1() {}
Dispatcher_SaveGameOption_1::~Dispatcher_SaveGameOption_1() {}
int Dispatcher_SaveGameOption_1::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_SaveGameOption_1::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SaveGameOption_1::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SaveGameOption_2::Dispatcher_SaveGameOption_2() {}
Dispatcher_SaveGameOption_2::~Dispatcher_SaveGameOption_2() {}
int Dispatcher_SaveGameOption_2::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_SaveGameOption_2::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SaveGameOption_2::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeChronicle::Dispatcher_UpgradeChronicle() {}
Dispatcher_UpgradeChronicle::~Dispatcher_UpgradeChronicle() {}
int Dispatcher_UpgradeChronicle::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UpgradeChronicle::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeChronicle::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_UpgradeInventory::Dispatcher_UpgradeInventory() {}
Dispatcher_UpgradeInventory::~Dispatcher_UpgradeInventory() {}
int Dispatcher_UpgradeInventory::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UpgradeInventory::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UpgradeInventory::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AddSocketToAvatar::Dispatcher_AddSocketToAvatar() {}
Dispatcher_AddSocketToAvatar::~Dispatcher_AddSocketToAvatar() {}
int Dispatcher_AddSocketToAvatar::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AddSocketToAvatar::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AddSocketToAvatar::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AuctionRegistItem::Dispatcher_AuctionRegistItem() {}
Dispatcher_AuctionRegistItem::~Dispatcher_AuctionRegistItem() {}
int Dispatcher_AuctionRegistItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_AuctionRegistItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AuctionRegistItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_ChangeHostWarRoom::Dispatcher_ChangeHostWarRoom() {}
Dispatcher_ChangeHostWarRoom::~Dispatcher_ChangeHostWarRoom() {}
int Dispatcher_ChangeHostWarRoom::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ChangeHostWarRoom::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeHostWarRoom::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_ChangeHostWarRoom::send(CUser* user, ParamBase& param) {}

Dispatcher_ConnectP2PAssault::Dispatcher_ConnectP2PAssault() {}
Dispatcher_ConnectP2PAssault::~Dispatcher_ConnectP2PAssault() {}
int Dispatcher_ConnectP2PAssault::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_ConnectP2PAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ConnectP2PAssault::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Debug_Clear_Quest::Dispatcher_Debug_Clear_Quest() {}
Dispatcher_Debug_Clear_Quest::~Dispatcher_Debug_Clear_Quest() {}
int Dispatcher_Debug_Clear_Quest::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Debug_Clear_Quest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Debug_Clear_Quest::send(CUser* user, ParamBase& param) {}

Dispatcher_DeleteToBlackList::Dispatcher_DeleteToBlackList() {}
Dispatcher_DeleteToBlackList::~Dispatcher_DeleteToBlackList() {}
int Dispatcher_DeleteToBlackList::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_DeleteToBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_DeleteToBlackList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_EnterPrivateStore::Dispatcher_EnterPrivateStore() {}
Dispatcher_EnterPrivateStore::~Dispatcher_EnterPrivateStore() {}
int Dispatcher_EnterPrivateStore::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_EnterPrivateStore::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_EnterPrivateStore::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_FatigueAttendance::Dispatcher_FatigueAttendance() {}
Dispatcher_FatigueAttendance::~Dispatcher_FatigueAttendance() {}
int Dispatcher_FatigueAttendance::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_FatigueAttendance::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_FatigueAttendance::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_FatigueAttendance::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_UseVendingMachine::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_UseVendingMachine::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_UseVendingMachine::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AchievementTrigger::Dispatcher_AchievementTrigger() {}
Dispatcher_AchievementTrigger::~Dispatcher_AchievementTrigger() {}
int Dispatcher_AchievementTrigger::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AchievementTrigger::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Antibot_DPCallBack::Dispatcher_Antibot_DPCallBack() {}
Dispatcher_Antibot_DPCallBack::~Dispatcher_Antibot_DPCallBack() {}
int Dispatcher_Antibot_DPCallBack::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Antibot_DPCallBack::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Antibot_DPCallBack::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_AvatarOptionChange::Dispatcher_AvatarOptionChange() {}
Dispatcher_AvatarOptionChange::~Dispatcher_AvatarOptionChange() {}
int Dispatcher_AvatarOptionChange::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_AvatarOptionChange::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_AvatarOptionChange::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_AvatarOptionChange::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_CreateAccountCargo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_GuildCargoPushItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GuildCargoPushItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_GuildCargoPushItem::send(CUser* user, ParamBase& param) {}

Dispatcher_RemovePrivateStore::Dispatcher_RemovePrivateStore() {}
Dispatcher_RemovePrivateStore::~Dispatcher_RemovePrivateStore() {}
int Dispatcher_RemovePrivateStore::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_RequestMemberEnter::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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

Dispatcher_StackableActionUse::Dispatcher_StackableActionUse() {}
Dispatcher_StackableActionUse::~Dispatcher_StackableActionUse() {}
int Dispatcher_StackableActionUse::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_StackableActionUse::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_StackableActionUse::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_StackableActionUse::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_ValidateScriptHash::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

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
int Dispatcher_PowerWarProcessInfo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_PowerWarProcessInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PowerWarProcessInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_PvpMissionHPPercent::Dispatcher_PvpMissionHPPercent() {}
Dispatcher_PvpMissionHPPercent::~Dispatcher_PvpMissionHPPercent() {}
int Dispatcher_PvpMissionHPPercent::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_PvpMissionHPPercent::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_PvpMissionHPPercent::send(CUser* user, ParamBase& param) {}

Dispatcher_RegisterToBlackList::Dispatcher_RegisterToBlackList() {}
Dispatcher_RegisterToBlackList::~Dispatcher_RegisterToBlackList() {}
int Dispatcher_RegisterToBlackList::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_RegisterToBlackList::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RegisterToBlackList::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestAssaultPrice::Dispatcher_RequestAssaultPrice() {}
Dispatcher_RequestAssaultPrice::~Dispatcher_RequestAssaultPrice() {}
int Dispatcher_RequestAssaultPrice::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_RequestAssaultPrice::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestAssaultPrice::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_RequestAssaultPrice::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_SaveCharacterOption::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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

Dispatcher_SecurityCardAuthRpy::Dispatcher_SecurityCardAuthRpy() {}
Dispatcher_SecurityCardAuthRpy::~Dispatcher_SecurityCardAuthRpy() {}
int Dispatcher_SecurityCardAuthRpy::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_SecurityCardAuthRpy::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SecurityCardAuthRpy::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SecurityCardAuthRpy::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_UpgradeAccountCargo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_VerifyCreatureQuest::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_VerifyCreatureQuest::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_WarRoomWpPerMonster::Dispatcher_WarRoomWpPerMonster() {}
Dispatcher_WarRoomWpPerMonster::~Dispatcher_WarRoomWpPerMonster() {}
int Dispatcher_WarRoomWpPerMonster::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_BuyCeraShopLimitItem::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_BuyCeraShopLimitItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BuyCeraShopLimitItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

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
int Dispatcher_ItemHyperLinkMessage::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_Security_ProcessScan::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Security_ProcessScan::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_ProcessScan::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SeriaRoom_Deco_Event::Dispatcher_SeriaRoom_Deco_Event() {}
Dispatcher_SeriaRoom_Deco_Event::~Dispatcher_SeriaRoom_Deco_Event() {}
int Dispatcher_SeriaRoom_Deco_Event::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_SeriaRoom_Deco_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SeriaRoom_Deco_Event::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SeriaRoom_Deco_Event::send(CUser* user, ParamBase& param) {}

Dispatcher_WriteGuildMemberMemo::Dispatcher_WriteGuildMemberMemo() {}
Dispatcher_WriteGuildMemberMemo::~Dispatcher_WriteGuildMemberMemo() {}
int Dispatcher_WriteGuildMemberMemo::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_WriteGuildMemberMemo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_WriteGuildMemberMemo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_BlueMarbleConfirmInfo::Dispatcher_BlueMarbleConfirmInfo() {}
Dispatcher_BlueMarbleConfirmInfo::~Dispatcher_BlueMarbleConfirmInfo() {}
int Dispatcher_BlueMarbleConfirmInfo::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_BlueMarbleConfirmInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BlueMarbleConfirmInfo::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_BlueMarbleConfirmInfo::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_Mercenary_Competition::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Mercenary_Competition::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Mercenary_Competition::send(CUser* user, ParamBase& param) {}

Dispatcher_OpenGuildCreateWindow::Dispatcher_OpenGuildCreateWindow() {}
Dispatcher_OpenGuildCreateWindow::~Dispatcher_OpenGuildCreateWindow() {}
int Dispatcher_OpenGuildCreateWindow::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_Break_Away_Quest_Check::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Break_Away_Quest_Check::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Break_Away_Quest_Check::send(CUser* user, ParamBase& param) {}

Dispatcher_ChangeAnotherSkillTree::Dispatcher_ChangeAnotherSkillTree() {}
Dispatcher_ChangeAnotherSkillTree::~Dispatcher_ChangeAnotherSkillTree() {}
int Dispatcher_ChangeAnotherSkillTree::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_ChangeAnotherSkillTree::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_ChangeAnotherSkillTree::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
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
int Dispatcher_LogIngameAdvertisement::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_TournamentRewardSelect::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_TournamentRewardSelect::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_TournamentRewardSelect::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_TournamentRewardSelect::send(CUser* user, ParamBase& param) {}

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
int Dispatcher_InvestItemAmplifyOption::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_InvestItemAmplifyOption::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_InvestItemAmplifyOption::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestItemUnlockCancel::Dispatcher_RequestItemUnlockCancel() {}
Dispatcher_RequestItemUnlockCancel::~Dispatcher_RequestItemUnlockCancel() {}
int Dispatcher_RequestItemUnlockCancel::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestItemUnlockCancel::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestItemUnlockCancel::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_RequestPCRoomPlayerList::Dispatcher_RequestPCRoomPlayerList() {}
Dispatcher_RequestPCRoomPlayerList::~Dispatcher_RequestPCRoomPlayerList() {}
int Dispatcher_RequestPCRoomPlayerList::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
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
int Dispatcher_Enchant_3rdChronicleItem::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_Enchant_3rdChronicleItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Enchant_3rdChronicleItem::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Enchant_3rdChronicleItem::send(CUser* user, ParamBase& param) {}

Dispatcher_GoblinPadRequestCryptKey::Dispatcher_GoblinPadRequestCryptKey() {}
Dispatcher_GoblinPadRequestCryptKey::~Dispatcher_GoblinPadRequestCryptKey() {}
int Dispatcher_GoblinPadRequestCryptKey::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_GoblinPadRequestCryptKey::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_GoblinPadRequestCryptKey::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_GoldTakeIncreasingAmount::Dispatcher_GoldTakeIncreasingAmount() {}
Dispatcher_GoldTakeIncreasingAmount::~Dispatcher_GoldTakeIncreasingAmount() {}
int Dispatcher_GoldTakeIncreasingAmount::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_RequestPCRoomPlayerCount::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_RequestPCRoomPlayerCount::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestPCRoomPlayerCount::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_MemoryIntegrity::Dispatcher_Security_MemoryIntegrity() {}
Dispatcher_Security_MemoryIntegrity::~Dispatcher_Security_MemoryIntegrity() {}
int Dispatcher_Security_MemoryIntegrity::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Security_MemoryIntegrity::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_MemoryIntegrity::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_PacketIntegrity::Dispatcher_Security_PacketIntegrity() {}
Dispatcher_Security_PacketIntegrity::~Dispatcher_Security_PacketIntegrity() {}
int Dispatcher_Security_PacketIntegrity::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Security_PacketIntegrity::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_PacketIntegrity::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_SelectUltimateDifficulty::Dispatcher_SelectUltimateDifficulty() {}
Dispatcher_SelectUltimateDifficulty::~Dispatcher_SelectUltimateDifficulty() {}
int Dispatcher_SelectUltimateDifficulty::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_SelectUltimateDifficulty::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SelectUltimateDifficulty::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Select_Item_Grwoth_Power::Dispatcher_Select_Item_Grwoth_Power() {}
Dispatcher_Select_Item_Grwoth_Power::~Dispatcher_Select_Item_Grwoth_Power() {}
int Dispatcher_Select_Item_Grwoth_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Select_Item_Grwoth_Power::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Select_Item_Grwoth_Power::send(CUser* user, ParamBase& param) {}

Dispatcher_SeriaRoom_Ani_Deco_Event::Dispatcher_SeriaRoom_Ani_Deco_Event() {}
Dispatcher_SeriaRoom_Ani_Deco_Event::~Dispatcher_SeriaRoom_Ani_Deco_Event() {}
int Dispatcher_SeriaRoom_Ani_Deco_Event::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_SeriaRoom_Ani_Deco_Event::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_SeriaRoom_Ani_Deco_Event::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_SeriaRoom_Ani_Deco_Event::send(CUser* user, ParamBase& param) {}

Dispatcher_UpdateContractOfCubeInfo::Dispatcher_UpdateContractOfCubeInfo() {}
Dispatcher_UpdateContractOfCubeInfo::~Dispatcher_UpdateContractOfCubeInfo() {}
int Dispatcher_UpdateContractOfCubeInfo::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_BloodRoundUiPrepareFinish::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_BloodRoundUiPrepareFinish::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_BloodRoundUiPrepareFinish::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Skill_Command_All_Default::Dispatcher_Skill_Command_All_Default() {}
Dispatcher_Skill_Command_All_Default::~Dispatcher_Skill_Command_All_Default() {}
int Dispatcher_Skill_Command_All_Default::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Skill_Command_All_Default::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Skill_Command_All_Default::send(CUser* user, ParamBase& param) {}

Dispatcher_Skill_Command_Customizing::Dispatcher_Skill_Command_Customizing() {}
Dispatcher_Skill_Command_Customizing::~Dispatcher_Skill_Command_Customizing() {}
int Dispatcher_Skill_Command_Customizing::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Skill_Command_Customizing::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_Skill_Command_Customizing::send(CUser* user, ParamBase& param) {}

Dispatcher_Buy_Charac_Status_Using_QP::Dispatcher_Buy_Charac_Status_Using_QP() {}
Dispatcher_Buy_Charac_Status_Using_QP::~Dispatcher_Buy_Charac_Status_Using_QP() {}
int Dispatcher_Buy_Charac_Status_Using_QP::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_RequestIngameAdvertisement::check_error(CUser* user, MSG_BASE& msg) { return 0; }
int Dispatcher_RequestIngameAdvertisement::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_RequestIngameAdvertisement::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }
void Dispatcher_RequestIngameAdvertisement::send(CUser* user, ParamBase& param) {}

Dispatcher_CallPartyMemberRealtimeInfo::Dispatcher_CallPartyMemberRealtimeInfo() {}
Dispatcher_CallPartyMemberRealtimeInfo::~Dispatcher_CallPartyMemberRealtimeInfo() {}
int Dispatcher_CallPartyMemberRealtimeInfo::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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
int Dispatcher_Security_NonClient_GetResponse::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Security_NonClient_GetResponse::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_NonClient_GetResponse::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_PI_HashModulation_C2C::Dispatcher_Security_PI_HashModulation_C2C() {}
Dispatcher_Security_PI_HashModulation_C2C::~Dispatcher_Security_PI_HashModulation_C2C() {}
int Dispatcher_Security_PI_HashModulation_C2C::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Security_PI_HashModulation_C2C::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_PI_HashModulation_C2C::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Security_PI_HashModulation_S2C::Dispatcher_Security_PI_HashModulation_S2C() {}
Dispatcher_Security_PI_HashModulation_S2C::~Dispatcher_Security_PI_HashModulation_S2C() {}
int Dispatcher_Security_PI_HashModulation_S2C::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Security_PI_HashModulation_S2C::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Security_PI_HashModulation_S2C::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

Dispatcher_Compound_Equipment_Upgrade_Card::Dispatcher_Compound_Equipment_Upgrade_Card() {}
Dispatcher_Compound_Equipment_Upgrade_Card::~Dispatcher_Compound_Equipment_Upgrade_Card() {}
int Dispatcher_Compound_Equipment_Upgrade_Card::dispatch_sig(CUser* user, PacketBuf& packet) { return 0; }
int Dispatcher_Compound_Equipment_Upgrade_Card::process(CUser* user, MSG_BASE& msg, ParamBase& param) { return 0; }
int Dispatcher_Compound_Equipment_Upgrade_Card::read(PacketBuf& packet, MSG_BASE& msg) { return 0; }

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
int Dispatcher_ComboSkillExtensionQuickSlotReset::check_error(CUser* user, MSG_BASE& msg) { return 0; }
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

// 定义于 AvatarRechargeServer.cpp（ORIG 0x081977b6）
void make_internal_stream_jpn(CStreamGuard& guard, int type, int param);

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
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, 0x2c, (int)user->GetUID());
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
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, 0x33, (int)user->GetUID());
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
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, 0x32, (int)user->GetUID());
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
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, 0x2d, (int)user->GetUID());
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
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, 0x30, (int)user->GetUID());
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
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, 0x2e, (int)user->GetUID());
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
Arad_Dispatcher_Manager::Arad_Dispatcher_Manager() {}
Arad_Dispatcher_Manager::~Arad_Dispatcher_Manager() {}
int Arad_Dispatcher_Manager::open()
{
    // ORIG 0x818fe38：new Arad_Dispatcher_BuyItemCheck_Impl() → 注册到
    // GlobalInstance<Arad_Dispatcher_Manager>::inst()。
    // [推断] 当前 Arad_Dispatcher_Manager 无注册容器/方法声明（Dispatchers.h
    // 仅 open），注册步骤暂以创建对象代替；register_dispatcher 语义待还原。
    // [推断] Arad_Dispatcher_BuyItemCheck_Impl 当前为抽象占位（handler1/
    // handler2 纯虚未实现，Dispatchers.h 3522），无法实例化；注册容器未建模。
    return 0;
}

Arad_Dispatcher_Login_Impl::Arad_Dispatcher_Login_Impl() {}
Arad_Dispatcher_Login_Impl::~Arad_Dispatcher_Login_Impl() {}

Arad_Dispatcher_BuyItemCheck_Impl::Arad_Dispatcher_BuyItemCheck_Impl() {}
Arad_Dispatcher_BuyItemCheck_Impl::~Arad_Dispatcher_BuyItemCheck_Impl() {}

}
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
