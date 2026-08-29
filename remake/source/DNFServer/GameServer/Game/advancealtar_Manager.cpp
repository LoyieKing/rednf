// ============================================================================
// advancealtar_Manager.cpp —— advancealtar::Manager::{selectStage,
// procErrorSelectStage} 真实实现（Prod_GlobalAdv 2026-08-24）。
//
// 证据：docs/class_func_reports/advancealtar__Manager/{selectStage,
// procErrorSelectStage}.md（ORIG 汇编+反编译 C 全文已读）。
// 阻塞符号（声明就位、定义待批次）：StageControl::selectStage(0x0812fa7a)。
// ============================================================================
#include "advancealtar.h"     // advancealtar::Manager / StageDifficulty / ENUM_ERROR
#include "CGameManager.h"     // G_CGameManager / getAdvanceAltar / StageControl 完整声明
#include "PacketGuard.h"      // PacketGuard : InterfacePacketBuf（put_header/put_byte/put_int/finalize）
#include "GameTypes.h"        // cMyTrace
#include "CUser.h"            // CUser 完整类型（m_advanceAltarIdx 字段化写）
#include "advancealtar_ProcStage.h"  // ProcStage / Timer_StageControl / HistoryLog（本批新增叶子头）
#include "GameWorld.h"             // G_GameWorld / out_from_dungeon / send_all（leaveUser）
#include "CGuildServerProxy.h"     // CEventManager / CEventBase 完整类型（isOnEvent）
#include "GlobalData.h"            // GlobalData::s_event_manager（isOnEvent）
#include "CMDPacketStruct.h"         // _SetSlotData 完整定义（setSlot/exchangeSlot/resetSlot 按值访问）

namespace advancealtar

{

// ORIG namespace 内 static 常量（_ZN12advancealtarL<len><name>E，r 段；值实测 ORIG
// .rodata 0x08af2e40..0x08af2e90 / 0x08b6d3cc..）：gcc 4.4 对未使用也发射。
// ----------------------------------------------------------------------------
static const int kMaxSlotCount = 12;            // 0x08af2e70
static const int kPlayMinLevel = 50;            // 0x08af2e64
static const int kMaxStageCount = 50;           // 0x08af2e6c
static const int kUnitSlotCount = 6;            // 0x08af2e50
static const int kSkillSlotCount = 6;           // 0x08af2e54
static const int kMaxBuyItemCount = 50;         // 0x08af2e74
static const int kMaxRewardCount = 30;          // 0x08af2e78
static const int kDefaultTicketFree = 100;      // 0x08af2e4c
static const int kInvalidRewardType = 0;        // 0x08af2e68
static const int kMaxAdvanceAltarDbData = 1;    // 0x08af2e7c
static const int kMaxStrBuffer = 2048;          // 0x08b6d3d0
static const int kMaxZipBuffer = 2048;          // 0x08b6d3cc
static const char* const kDelimterItem = "|";   // 0x08b6d3d8 指针 → '|'(0x7c)
static const char* const kDelimterField = ",";  // 0x08b6d3dc 指针 → ','(0x2c)
static const int KGaugeWidth[4] = { 60, 90, 125, 170 };       // 0x08af2e80
static const unsigned char kUnitSlotNoLock[6] = { 0, 0, 0, 0, 0, 0 };   // 0x08af2e58
static const unsigned char kSkillSlotNoLock[6] = { 0, 0, 0, 0, 1, 1 };  // 0x08af2e5e（实测）

// ----------------------------------------------------------------------------
// ORIG T 0x0813090e _ZN12advancealtar7Manager11selectStageEP5CUseriNS_15StageDifficulty1TE
// user 判空 → 3；难度越界(<0||>2) → 8；getAdvanceAltar() 判空 → cMyTrace +
// "StaticPool Defence DungeonNULL" → 3；
//   StageControl::selectStage != 0 → control->destroy() 后透传错误码；
//   == 0 → setAdvanceAltarIndex(control->getIndex())（ORIG 经 CUser::setAdvanceAltarIndex
//   W 0x0813490e 写 m_advanceAltarIdx，此处字段化等价写）→ 返回 0。
// ----------------------------------------------------------------------------
int Manager::selectStage(CUser* user, int stageId, StageDifficulty::T diff)
{
    if (user == 0)
    {
        return 3;
    }
    if (diff < 0 || diff > 2)
    {
        return 8;
    }

    StageControl* control = G_CGameManager()->getAdvanceAltar();
    if (control == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x6ab, 5);
        trace("StaticPool Defence DungeonNULL");
        return 3;
    }

    const int err = control->selectStage(user, stageId, diff);
    if (err != 0)
    {
        control->destroy();
        return err;
    }
    user->m_advanceAltarIdx = static_cast<short>(control->getIndex());  // ORIG setAdvanceAltarIndex(+0x8d012)
    return 0;
}

// ----------------------------------------------------------------------------
// ORIG T 0x081309f0 _ZN12advancealtar7Manager20procErrorSelectStageEP5CUser10ENUM_ERROR
// user 判空或 err==0 直接返回；否则 PacketGuard: put_header(1,0x10) /
// put_byte(0) / put_int(err) / finalize(true) → CUser::Send。
// ----------------------------------------------------------------------------
void Manager::procErrorSelectStage(CUser* user, ENUM_ERROR err)
{
    if (user == 0 || err == 0)
    {
        return;
    }

    PacketGuard guard;
    guard.put_header(1, 0x10);
    guard.put_byte(0);
    guard.put_int(static_cast<int>(err));
    guard.finalize(true);
    user->Send(guard);
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130ab2 _ZN12advancealtar7Manager9dieCharacEP5CUser
// user 判空 / getAdvanceAltar() 判空直接返回；仅 isRewardState() 时处理：
//   stageType==1 → clearStageAndReward() + setState(5) +
//                  RegistTimerClearReward(getIndex(), END_N1)
//   其它       → setState(6) + RegistTimerEPLP(getIndex(), END_N1) +
//                  HistoryLog::dungeonClearInfo(user, control, false)
// [未建模批次] ProcStage::clearStageAndReward(0x0812e3f6) 依赖
//   CharacAdvanceAltarManager::clearStage/addAchievementReward/rewardClearStage/
//   sendClearStage/starAdd 等约 2.2k 行报告子树，本批跳过该调用（同
//   StageControl::leaveUser 对 HistoryLog::leaveDungeon 的既有处理）。
// ----------------------------------------------------------------------------
void Manager::dieCharac(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }

    ProcStage* procStage = control->getProcStage();
    if (!procStage->isRewardState())
    {
        return;
    }

    if (procStage->getStageType() == 1)
    {
        // ORIG procStage->clearStageAndReward();
        procStage->setState(ProcStage::State::S_N5);
        Timer_StageControl::RegistTimerClearReward(control->getIndex(),
                                                   StageEndType::END_N1);
    }
    else
    {
        procStage->setState(ProcStage::State::S_N6);
        Timer_StageControl::RegistTimerEPLP(control->getIndex(),
                                            StageEndType::END_N1);
        HistoryLog::dungeonClearInfo(*user, *control, false);
    }
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130eb2 _ZN12advancealtar7Manager10finishLoadEP5CUser
// getAdvanceAltar() 判空 → 0；isAvailableTciket() 为假 → 1（原样返回）；
// 否则 setState(1) 并回包 put_header(0,0x1e)/put_int(0)/finalize(true) → Send；
// 恒返回 1。
// ----------------------------------------------------------------------------
int Manager::finishLoad(CUser* user)
{
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return 0;
    }
    if (!user->m_advanceAltar.isAvailableTciket())
    {
        return 1;
    }

    control->getProcStage()->setState(ProcStage::State::S_N1);

    PacketGuard guard;
    guard.put_header(0, 0x1e);
    guard.put_int(0);
    guard.finalize(true);
    user->Send(guard);
    return 1;
}

// ----------------------------------------------------------------------------
// ORIG T 0x08130fa8 _ZN12advancealtar7Manager10giveUpGameEP5CUser
// user/getAdvanceAltar() 判空直接返回；setState(6) +
// RegistTimerEPLP(getIndex(), END_N2) + dungeonClearInfo(user, control, false)。
// ----------------------------------------------------------------------------
void Manager::giveUpGame(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }

    control->getProcStage()->setState(ProcStage::State::S_N6);
    Timer_StageControl::RegistTimerEPLP(control->getIndex(),
                                        StageEndType::END_N2);
    HistoryLog::dungeonClearInfo(*user, *control, false);
}


// ============================ OnlyOrig_Altar.AltarMgr 批（2026-08-27）=========================
// 新增符号（ORIG 报告全文转写；证据路径见各函数注记）：
//   Manager::{C1,C2,D1,D2,eplp,pause,dieMob,leaveUser}、isOnEvent、
//   CharacAdvanceAltarManager::{gmSetTicket,decTicket,addRewardData,
//     getClearStageSummary,getCurrentAdvanceAltarData,setUnlockBuyItem,
//     setDefaultBuyItem,buyItem}、
//   AdvanceAltarData::{getSlotMap,getBuyItemMap,set}。
// ============================================================================

// ---- ORIG T/W 0x08130902 / 0x08130908（docs/class_func_reports/advancealtar__Manager/
//      Manager.md、~Manager.md）：push ebp/mov esp,ebp/pop ebp/ret —— 全空体。----
Manager::Manager() {}
Manager::~Manager() {}

// ---- ORIG T 0x0813101e（advancealtar__Manager/eplp.md 全文）----
void Manager::eplp(CUser* user, EplpType::T type)
{
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }
    if (static_cast<int>(type) > 2)   // ORIG: add $0; cmp $2 ja —— 无符号比较
    {
        return;
    }
    control->leaveUser();
}

// ---- ORIG T 0x0813105c（advancealtar__Manager/pause.md 全文）。ORIG 返回
//      undefined4（ProcStage::pause 结果）；协调口径 void，返回值丢弃。----
void Manager::pause(CUser* user, bool flag)
{
    if (user == 0)
    {
        return;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return;
    }
    ProcStage* procStage = control->getProcStage();
    procStage->pause(flag ? true : false);
}

// ---- ORIG T 0x08130bb8（advancealtar__Manager/dieMob.md 全文 + 汇编逐块核对）----
int Manager::dieMob(CUser* user, unsigned short mobSeq, unsigned short param,
                    const MSG_MONSTER_DIE& msg)
{
    if (user == 0)
    {
        return 0;
    }
    StageControl* control = user->getAdvanceAltar();
    if (control == 0)
    {
        return 1;
    }
    ProcStage* procStage = control->getProcStage();

    bool killed = procStage->dieMob(mobSeq, param, msg);
    if (killed)
    {
        PacketGuard packet;
        packet.put_header(0, 0x26);
        packet.put_short(mobSeq);
        packet.put_byte(0);           // ×5 个 put_byte(0)（ORIG +0x66..+0x108）
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_byte(0);
        packet.finalize(true);
        user->Send(packet);
    }

    StageEndType::T endType = StageEndType::END_N1;
    if (!procStage->isEndStage(endType))
    {
        return 1;
    }
    if (!procStage->isRewardState())
    {
        return 1;
    }

    if (endType == StageEndType::END_N0)
    {   // ORIG local_18==0：全清死亡分支
        if (procStage->getStageType() == 1)
        {
            procStage->setState(ProcStage::State::S_N5);
            procStage->clearStageAndReward();   // [跨片] clearStage 链定义归 AltarProc 批
            Timer_StageControl::RegistTimerClearReward(control->getIndex(), endType);
            // HistoryLog::dungeonClearInfo(true) 分支 ORIG 无（该段属 endType==3）
        }
        else
        {
            procStage->setState(ProcStage::State::S_N6);
            Timer_StageControl::RegistTimerEPLP(control->getIndex(), endType);
            HistoryLog::dungeonClearInfo(*user, *control, false);
        }
    }
    else if (endType == StageEndType::END_N3)
    {   // ORIG local_18==3：全阶段完成
        if (procStage->endProcAllKillMonster())
        {
            procStage->setState(ProcStage::State::S_N5);
            Timer_StageControl::RegistTimerClearReward(control->getIndex(), endType);
            HistoryLog::dungeonClearInfo(*user, *control, true);
        }
        else
        {
            control->sendMapInfo();
        }
    }
    return 1;
}

// ---- ORIG T 0x081310b2（advancealtar__Manager/leaveUser.md 全文）----
void Manager::leaveUser(CUser* user)
{
    if (user == 0)
    {
        return;
    }
    GameWorld* world = G_GameWorld();
    world->out_from_dungeon(user);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info(reinterpret_cast<char*>(&packet), '\0');
    packet.finalize(true);
    world = G_GameWorld();
    world->send_all(packet, user);
}


// ---- ORIG T 0x0813375c（advancealtar__CharacAdvanceAltarManager/gmSetTicket.md）----
void CharacAdvanceAltarManager::gmSetTicket(int ticket)
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return;
    }
    cur->m_ticket = ticket;                       // ORIG *(cur+4)=param
    cur->m_update = 1;                            // ORIG *(cur+0xcc)=1
}

// ---- ORIG T 0x081313b6（decTicket.md）：先扣 m_ticket，不足扣 m_freeTicket；
// 任一成功置 update=1 并返 1（头口径 char）。----
char CharacAdvanceAltarManager::decTicket()
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return 0;
    }
    bool dec = false;
    if (cur->m_ticket >= 1)
    {
        --cur->m_ticket;
        dec = true;
    }
    else if (cur->m_freeTicket > 0)
    {
        --cur->m_freeTicket;
        dec = true;
    }
    if (dec)
    {
        cur->m_update = 1;
    }
    return dec ? 1 : 0;
}

// ---- ORIG T 0x08132abe（addRewardData.md）：rewards map 查重后两端各插一份
//      （_Reward{field0=rewardId,byte4=0}），置 m_update=1；返回 1/0。----
int CharacAdvanceAltarManager::addRewardData(int rewardId, std::map<int, _Reward>& out)
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return 0;
    }
    if (cur->m_rewards.find(rewardId) != cur->m_rewards.end())
    {
        return 0;
    }
    _Reward entry;                    // ORIG _Reward ctor W memset 5
    entry.m_field0 = rewardId;
    entry.m_byte4 = 0;
    cur->m_rewards.insert(std::make_pair(rewardId, entry));
    cur->m_update = 1;
    out.insert(std::make_pair(rewardId, entry));
    return 1;
}

// ---- ORIG T 0x081318e2（getClearStageSummary.md）：按 _Stage.m_field0
//      （difficulty 计数）分组累计 out[0..2]。----
void CharacAdvanceAltarManager::getClearStageSummary(int out[3]) const
{
    const AdvanceAltarData* data = getCurrentAdvanceAltarData();
    if (data == 0)
    {
        return;
    }
    for (std::map<int, _Stage>::const_iterator it = data->m_stages.begin();
         it != data->m_stages.end(); ++it)
    {
        const int diff = it->second.m_field0;          // ORIG iter+8 → value.f0
        if (diff == 1)
        {
            ++out[1];
            ++out[0];
        }
        else if (diff == 2)
        {
            ++out[2];
            ++out[1];
            ++out[0];
        }
        else if (diff == 0)
        {
            ++out[0];
        }
    }
}





// ==================== CharacAdvanceAltarManager 补遗 ====================
// ---- ORIG T 0x08132d20（setUnlockBuyItem.md 全文）：遍历商店升级表匹配 stageIdx；
//      minLevel=getMinLevelFieldDataPoint(FT_LEVEL)(缺省回退 level 0→1)；价格点=
//      getFieldDataPoint(minLevel, FT_PRICE)；命中则构建 _BuyItem 入对应 shop 表，
//      需要占槽时置入空槽【BLOCKED 子调用：setUnlockItemToEmptySlot 依赖 _Slot
//      in-map 布局口径】；尾部统一置 update=1。----
void CharacAdvanceAltarManager::setUnlockBuyItem(AdvanceAltarShopType::T shopType,
                                                 int stageIdx,
                                                 std::vector<_BuyItem>* out)
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return;
    }
    std::vector<BuyUpgradeData>* upgrades =
        G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(cur->m_ridableId, shopType);
    std::map<int, _BuyItem>* items = cur->getBuyItemMap(shopType);
    if (upgrades == 0 || items == 0)
    {
        return;
    }
    for (std::vector<BuyUpgradeData>::iterator it = upgrades->begin();
         it != upgrades->end(); ++it)
    {
        if (it->m_field4 != stageIdx)
        {
            continue;
        }
        int minLevel = 0;
        FieldData* minPoint = it->getMinLevelFieldDataPoint(FieldType::FT_LEVEL, minLevel);
        FieldData* pricePoint = it->getFieldDataPoint(minLevel, FieldType::FT_PRICE);
        if (minPoint == 0 || pricePoint == 0)
        {
            continue;
        }
        _BuyItem entry;                    // ORIG _BuyItem ctor W memset 12
        entry.m_field0 = it->m_nItemId;
        entry.m_field4 = shopType;
        entry.m_short8 = static_cast<short>(minLevel);
        entry.m_short10 = static_cast<short>(pricePoint->m_value);
        items->insert(std::make_pair(entry.m_field0, entry));
        if (minLevel >= 1 && it->m_field8 != 0)   // ORIG 双条件后才占空槽
        {
            setUnlockItemToEmptySlot(shopType, entry.m_field0);
        }
        if (out != 0)
        {
            out->push_back(entry);
        }
    }
    cur->m_update = 1;
}

// ---- ORIG T 0x08132cb8（setDefaultBuyItem.md）：清对应 shop 表后全量重放。----
void CharacAdvanceAltarManager::setDefaultBuyItem(AdvanceAltarShopType::T type)
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return;
    }
    std::map<int, _BuyItem>* items = cur->getBuyItemMap(type);
    if (items == 0)
    {
        return;
    }
    items->clear();
    setUnlockBuyItem(type, 0, 0);
}

// ---- ORIG T 0x08132d20 后续：本函数自上一提交块起沿用同一文件命名空间；
// 以下为第二收口批次（slot 族全真转写）。迭代器偏移按协调者裁定采用
// pair<const u8,_Slot>* 视图：it+0=键(槽号)、it+1=&second(m_byte0)、
// it+2=&m_int1、it+6=&m_byte5、it+7=&m_int6 —— 与现头 sizeof(_Slot)==0xa 自洽。----

// ---- ORIG T 0x08131b58（setSlot.md 全文）----
int CharacAdvanceAltarManager::setSlot(bool isSet, const CMDPacketStruct::_SetSlotData& data)
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return 2;
    }
    AdvanceAltarShopType::T shopType;
    if (data.m_type == 1)
    {
        shopType = AdvanceAltarShopType::T_0;
    }
    else if (data.m_type == 2)
    {
        shopType = AdvanceAltarShopType::T_1;
    }
    else
    {
        return 2;
    }
    if (static_cast<unsigned char>(data.m_field4) > 5)
    {
        return 2;
    }

    std::vector<BuyUpgradeData>* upgrades =
        G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(cur->m_ridableId, shopType);
    if (upgrades == 0)
    {
        return 3;
    }
    const BuyUpgradeData* chosen = 0;
    for (std::vector<BuyUpgradeData>::const_iterator it = upgrades->begin();
         it != upgrades->end(); ++it)
    {
        if (it->m_nItemId == data.m_field8)
        {
            chosen = &(*it);
            break;
        }
    }
    if (chosen == 0)
    {
        return 3;
    }
    if (chosen->m_field8 == 0)          // “is settable slot”未开放
    {
        return 3;
    }
    if (!isSet)
    {
        return resetSlot(*cur, data);   // ORIG 尾 jmp 进入 resetSlot 共用出口
    }

    std::map<int, _BuyItem>* items = cur->getBuyItemMap(shopType);
    if (items == 0)
    {
        return 3;
    }
    if (items->find(data.m_field8) != items->end())
    {
        return 3;                        // 已购买过
    }

    std::map<unsigned char, _Slot>* slots =
        cur->getSlotMap(static_cast<AdvanceAltarSlotType::T>(data.m_type));
    if (slots == 0)
    {
        return 3;
    }
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        if (it->second.m_int6 == data.m_field8)
        {
            return 4;                    // 同一物品已在其它槽装备
        }
    }
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        _Slot& slot = it->second;
        if (slot.m_byte5 == static_cast<unsigned char>(data.m_field4) &&
            slot.m_int1 == data.m_type &&
            slot.m_byte0 != 1)           // 未锁定
        {
            slot.m_int6 = data.m_field8;              // ORIG it+7 ← data+8
            cur->m_update = 1;
            return 0;
        }
    }
    return 2;
}

// ---- ORIG T 0x08131f0a（exchangeSlot.md 全文）----
int CharacAdvanceAltarManager::exchangeSlot(CMDPacketStruct::_SetSlotData& slotA,
                                            CMDPacketStruct::_SetSlotData& slotB)
{
    if (slotA.m_type != slotB.m_type)
    {
        return 2;
    }
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return 3;
    }
    std::map<unsigned char, _Slot>* slots =
        cur->getSlotMap(static_cast<AdvanceAltarSlotType::T>(slotA.m_type));
    if (slots == 0)
    {
        return 4;
    }

    std::map<unsigned char, _Slot>::iterator foundA = slots->end();
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        const _Slot& slot = it->second;
        if (slot.m_byte5 == static_cast<unsigned char>(slotA.m_field4) &&
            slot.m_int1 == slotA.m_type &&
            slot.m_int6 == slotA.m_field8)
        {
            foundA = it;
            break;
        }
    }
    std::map<unsigned char, _Slot>::iterator foundB = slots->end();
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        const _Slot& slot = it->second;
        if (slot.m_byte5 == static_cast<unsigned char>(slotB.m_field4) &&
            slot.m_int1 == slotB.m_type &&
            slot.m_int6 == slotB.m_field8)
        {
            foundB = it;
            break;
        }
    }
    if (foundA == slots->end() || foundB == slots->end())
    {
        return 3;
    }
    if (foundA->second.m_byte0 != 0 || foundB->second.m_byte0 != 0)
    {
        return 3;                         // 任一处于锁定态不可交换
    }

    const int tmp = foundA->second.m_int6;
    foundA->second.m_int6 = foundB->second.m_int6;
    foundB->second.m_int6 = tmp;
    cur->m_update = 1;
    return 0;
}

// ---- ORIG T 0x08132bfa（resetSlot.md 全文）：按槽号查找；找不到或锁定返 2；
// 命中清空槽数据并置 update=1 返 0。----
int CharacAdvanceAltarManager::resetSlot(advancealtar::AdvanceAltarData& cur,
                                         const CMDPacketStruct::_SetSlotData& data)
{
    std::map<unsigned char, _Slot>* slots =
        cur.getSlotMap(static_cast<AdvanceAltarSlotType::T>(data.m_type));
    if (slots == 0)
    {
        return 3;
    }
    std::map<unsigned char, _Slot>::iterator it =
        slots->find(static_cast<unsigned char>(data.m_field4));
    if (it == slots->end())
    {
        return 2;
    }
    if (it->second.m_byte0 == 1)
    {
        return 2;                          // 锁定槽
    }
    it->second.m_int6 = 0;
    cur.m_update = 1;
    return 0;
}

// ---- ORIG T 0x08132234（setUnlockItemToEmptySlot.md 全文）----
void CharacAdvanceAltarManager::setUnlockItemToEmptySlot(AdvanceAltarShopType::T shopType,
                                                         int itemIdx)
{
    AdvanceAltarSlotType::T slotType;
    if (shopType == AdvanceAltarShopType::T_0)
    {
        slotType = AdvanceAltarSlotType::SLOT_N1;
    }
    else if (shopType == AdvanceAltarShopType::T_1)
    {
        slotType = AdvanceAltarSlotType::SLOT_N2;
    }
    else
    {
        return;
    }
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return;
    }
    std::map<unsigned char, _Slot>* slots = cur->getSlotMap(slotType);
    if (slots == 0)
    {
        return;
    }
    for (std::map<unsigned char, _Slot>::iterator it = slots->begin();
         it != slots->end(); ++it)
    {
        _Slot& slot = it->second;
        if (slot.m_byte0 == 1)             // ORIG 加锁位/已占用任一不满足即跳过
        {
            continue;
        }
        if (slot.m_int6 != 0)
        {
            continue;
        }
        slot.m_int6 = itemIdx;
        cur->m_update = 1;
        return;
    }
}

// ---- ORIG W 0x08132f8e（setDefaultSlot.md 全文）：unit/skill 两张槽表各扫 0..5；
// 命中则清空槽数据；缺失则以 kUnitSlotNoLock/kSkillSlotNoLock[k] 为模板补插，
// 类型标记分别为 SLOT_N1/SLOT_N2，槽内槽号=k 写入 m_byte5。----
void CharacAdvanceAltarManager::setDefaultSlot()
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return;
    }
    static const unsigned char* kUnit = kUnitSlotNoLock;    // ORIG .rodata 0x08af2e58（6×00，实测）
    static const unsigned char* kSkill = kSkillSlotNoLock;  // ORIG .rodata 0x08af2e5e（实测 00 00 00 00 01 01）

    for (int k = 0; k <= 5; ++k)
    {
        std::map<unsigned char, _Slot>& slots = cur->m_slots1;
        std::map<unsigned char, _Slot>::iterator it = slots.find(
            static_cast<unsigned char>(k));
        if (it != slots.end())
        {
            it->second.m_int6 = 0;
            continue;
        }
        _Slot slot;                                  // ORIG _Slot ctor memset
        slot.m_byte0 = kUnit[k];
        slot.m_int1 = 1;                             // 槽类型标记（SLOT_N1）
        slot.m_byte5 = static_cast<unsigned char>(k);
        slot.m_int6 = 0;
        slots.insert(std::make_pair(static_cast<unsigned char>(k), slot));
    }
    for (int k = 0; k <= 5; ++k)
    {
        std::map<unsigned char, _Slot>& slots = cur->m_slots2;
        std::map<unsigned char, _Slot>::iterator it = slots.find(
            static_cast<unsigned char>(k));
        if (it != slots.end())
        {
            it->second.m_int6 = 0;
            continue;
        }
        _Slot slot;
        slot.m_byte0 = kSkill[k];
        slot.m_int1 = 2;                             // 槽类型标记（SLOT_N2）
        slot.m_byte5 = static_cast<unsigned char>(k);
        slot.m_int6 = 0;
        slots.insert(std::make_pair(static_cast<unsigned char>(k), slot));
    }
}

// ---- ORIG T 0x081333b2（resetStar.md 全文）：非 0 类型→返 2；0 型扣重置费
// （费用表按 m_resetCount 查，miss 回退 getMaxStarResetGoldFee）→ 不足返 3 →
// use_money(fee,0x32,1) 失败返 3 → SendUpdateItemList(1,0,0) → ++m_resetCount
// → setDefaultSlot + 三商店 default → 遍历 stages（m_field4!=-1）三商店重放
// unlock → 星池差值入账 + HistoryLog::starAdd。----
int CharacAdvanceAltarManager::resetStar(CUser* user, CMDPacketStruct::ResetStarType::T type)
{
    if (user == 0 || getCurrentAdvanceAltarData() == 0)
    {
        return 2;
    }
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (type != static_cast<CMDPacketStruct::ResetStarType::T>(0))
    {
        return 2;
    }

    int fee = 0;
    std::map<int, _StarRestFeeByGold>::iterator feeIt =
        G_CDataManager()->m_altarShop.m_starRestMap.find(
            static_cast<int>(cur->m_resetCount));
    if (feeIt != G_CDataManager()->m_altarShop.m_starRestMap.end())
    {
        fee = feeIt->second.m_field0;                       // pair 视图 *(feeIter+4)=second.m_field0
    }
    else
    {
        fee = G_CDataManager()->m_altarShop.getMaxStarResetGoldFee();
    }

    const CInventory* invenR = user->getCurCharacInvenR();
    if (invenR->get_money() < fee)
    {
        return 3;
    }
    CInventory* invenW = user->getCurCharacInvenW();
    if (!invenW->use_money(fee, static_cast<eMoneySubReason>(0x32), true))
    {
        return 3;
    }
    user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_ITEMSPACE>(0), 0);

    ++cur->m_resetCount;                                    // ORIG short 自增
    setDefaultSlot();
    setDefaultBuyItem(AdvanceAltarShopType::T_0);
    setDefaultBuyItem(AdvanceAltarShopType::T_1);
    setDefaultBuyItem(AdvanceAltarShopType::T_2);

    for (std::map<int, _Stage>::const_iterator it = cur->m_stages.begin();
         it != cur->m_stages.end(); ++it)
    {
        if (it->second.m_field4 == -1)
        {
            continue;
        }
        setUnlockBuyItem(AdvanceAltarShopType::T_0, it->second.m_field4, 0);
        setUnlockBuyItem(AdvanceAltarShopType::T_1, it->second.m_field4, 0);
        setUnlockBuyItem(AdvanceAltarShopType::T_2, it->second.m_field4, 0);
    }

    const int delta = (cur->m_curStar + cur->m_starPoolBase) - cur->m_starPoint;
    cur->m_starPoint = cur->m_curStar + cur->m_starPoolBase;
    cur->m_update = 1;

    advancealtar::AddStarType::T logType = advancealtar::AddStarType::ADD_BY_RESET;
    if (delta > 0)
    {
        HistoryLog::starAdd(*user, logType, 0, delta);
    }
    return 0;
}

// ---- ORIG T 0x08133720（gmSetStar.md）：当前星=v、星池点=基数+v、update=1。----
void CharacAdvanceAltarManager::gmSetStar(int star)
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return;
    }
    cur->m_curStar = star;
    cur->m_starPoint = cur->m_starPoolBase + star;
    cur->m_update = 1;
}

// ---- ORIG T 0x08131426（checkAndCreateDefaultRidable.md 全文）----
void CharacAdvanceAltarManager::checkAndCreateDefaultRidable()
{
    bool skipInit = false;
    AdvanceAltarData* target = 0;
    for (int i = 0; i <= 0; ++i)
    {
        AdvanceAltarData* data =
            reinterpret_cast<AdvanceAltarData*>(reinterpret_cast<char*>(this) + 4 + i * 0xd0);
        if (data->m_ridableId != RidableId::RIDEABLE)
        {
            continue;
        }
        const bool allEmpty = data->m_buyItems1.empty() &&
                              data->m_buyItems2.empty() &&
                              data->m_buyItems3.empty();
        if (allEmpty)
        {
            target = data;               // ORIG local_14 = this+i*0xd0+4
            break;
        }
        skipInit = true;                 // 已有内容不重置
    }
    if (skipInit)
    {
        return;
    }
    if (target == 0)
    {
        target = &m_data[0];            // ORIG 回退 this+4
    }
    target->m_update = 1;
    target->m_ridableId = RidableId::RIDEABLE;
    setCurrentAdvanceAltarData(RidableId::RIDEABLE);
    setDefaultSlot();
    setDefaultStage();
    setDefaultBuyItem(AdvanceAltarShopType::T_0);
    setDefaultBuyItem(AdvanceAltarShopType::T_1);
    setDefaultBuyItem(AdvanceAltarShopType::T_2);
    target->m_ticket = G_CDataManager()->m_altarConfig.m_0;   // ORIG +0x6364 配置票数
}

// ---- ORIG T 0x081319ae（buyItem.md 全文）----
int CharacAdvanceAltarManager::buyItem(AdvanceAltarShopType::T shopType, int level,
                                       short fromSlot, short toSlot, int totalCost,
                                       int& outReward)
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return 3;
    }
    if (cur->m_starPoint < totalCost)
    {
        return 2;
    }
    std::map<int, _BuyItem>* items = cur->getBuyItemMap(shopType);
    if (items == 0)
    {
        return 3;
    }

    std::map<int, _BuyItem>::iterator it = items->find(level);
    if (it != items->end())
    {
        it->second.m_short8 = toSlot;                          // ORIG +0xc ← param_5 截断
        it->second.m_short10 =
            static_cast<short>(it->second.m_short10 + totalCost);   // +0xe 累加
    }
    else
    {
        _BuyItem entry;                  // ORIG _BuyItem ctor W memset 12
        entry.m_field0 = shopType;       // 序按 asm：param_2→f0 / param_3→f4 /
        entry.m_field4 = level;          //   toSlot→short8 / totalCost→shortA；
        entry.m_short8 = toSlot;         //   map 键=level（make_pair int& &param_3）
        entry.m_short10 = static_cast<short>(totalCost);
        items->insert(std::make_pair(entry.m_field4, entry));
    }

    cur->m_starPoint -= totalCost;                              // ORIG +0x14 扣减
    cur->m_update = 1;
    outReward = cur->m_starPoint;
    HistoryLog::buyItem(shopType, level, fromSlot, toSlot, totalCost);
    return 0;
}

// ---- ORIG T 0x0813234e（sendAchievementReward.md 全文）：奖励领奖入口。----
int CharacAdvanceAltarManager::sendAchievementReward(
    CUser* user, int achievementId,
    const std::vector<_RewardItem>** outItems)
{
    if (user == 0 || getCurrentAdvanceAltarData() == 0)
    {
        return 2;
    }
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    std::map<int, _Reward>& rewards = cur->m_rewards;
    std::map<int, _Reward>::iterator it = rewards.find(achievementId);
    if (it == rewards.end())
    {
        return 2;
    }
    if (it->second.m_byte4 != 0)         // 已领取标记（_Reward+4）
    {
        return 3;
    }
    AchievementRewardData* reward =
        G_CDataManager()->m_altarReward.getAchievementRewardData(achievementId);
    if (reward == 0)
    {
        return 3;
    }

    std::vector<_RewardItem> normalItems;      // ORIG local_34/local_40 双缓冲分流
    std::vector<_RewardItem> eventItems;
    for (std::vector<_RewardItem>::const_iterator rit = reward->m_rewardItems.begin();
         rit != reward->m_rewardItems.end(); ++rit)
    {
        const bool onEvent = advancealtar::isOnEvent();
        if (onEvent && rit->m_type != 0)
        {
            eventItems.push_back(*rit);
        }
        else
        {
            normalItems.push_back(*rit);
        }
    }
    const char* postalTitle = reward->m_postalTitle.c_str();     // +0x20
    const char* postalLetter = reward->m_postalLetter.c_str();   // +0x24
    if (!normalItems.empty())
    {
        ProcStage::sendMailRewardItem(user, normalItems, postalTitle, postalLetter);
    }
    if (!eventItems.empty())
    {
        ProcStage::sendMailRewardItem(user, eventItems,
                                      "chn_game_server_msg_86", postalLetter);
    }
    *outItems = &reward->m_rewardItems;
    it->second.m_byte4 = 1;                                     // 置已领取
    cur->m_update = 1;
    return 0;
}

// ==================== 终态收口批（OnlyOrig_Altar.AltarMgr，2026-08-27）====================
// ---- ORIG T 0x081316e8（getAdvanceAltarDataByIndex.md；任务文本 0x8133194 为
//      笔误，nm -C 权威地址 0x081316e8）：idx 负或超界→0，否则 this+4+i*0xd0。----
AdvanceAltarData* CharacAdvanceAltarManager::getAdvanceAltarDataByIndex(int idx)
{
    if (idx < 0 || idx > 0)
    {
        return 0;
    }
    return reinterpret_cast<AdvanceAltarData*>(
        reinterpret_cast<char*>(this) + 4 + idx * sizeof(AdvanceAltarData));
}

// ---- ORIG T 0x08131254（setCurrentAdvanceAltarData.md 全文）----
bool CharacAdvanceAltarManager::setCurrentAdvanceAltarData(RidableId::T ridableId)
{
    if (!isValidRidableId(ridableId))
    {
        return false;
    }
    for (int i = 0; i <= 0; ++i)
    {
        AdvanceAltarData* data =
            reinterpret_cast<AdvanceAltarData*>(reinterpret_cast<char*>(this) + 4 + i * sizeof(AdvanceAltarData));
        if (data->m_ridableId == ridableId)
        {
            m_field0 = reinterpret_cast<int>(data);   // ORIG *(int*)this = &data[i]
            return true;
        }
    }
    m_field0 = 0;
    return false;
}

// ---- ORIG T 0x08132f72（setDefaultStage.md）：setDefaultStage ⇒ setNextStage(0)。----
void CharacAdvanceAltarManager::setDefaultStage()
{
    setNextStage(0);
}

// ---- ORIG T 0x08131616（CharacAdvanceAltarManager/set.md）：单实例分派，
//      db.m_db(+4) → m_data.set。----
void CharacAdvanceAltarManager::set(const _CharacAdvanceAltarDbData& db)
{
    for (int i = 0; i < 1; ++i)
    {
        AdvanceAltarData* data =
            reinterpret_cast<AdvanceAltarData*>(reinterpret_cast<char*>(this) + 4 + i * sizeof(AdvanceAltarData));
        data->set(*reinterpret_cast<const _AdvanceAltarDbData*>(
            reinterpret_cast<const char*>(&db) + 4 + i * sizeof(_AdvanceAltarDbData)));
    }
}

// ---- ORIG T 0x08132638（getSlotItem.md 全文）。报告内 `local_78[0] =
// *(_SlotItem*)(slotIterElem+6)` 的 pair 基址记法按 ProcStage.h _SlotItem
// 写入点口径还原：itemId=等级点 FT_INDEX 的 FieldData::m_value、needGage=
// CItem::getAdvanceAltarNeedGage；键=槽表 first（槽号）。局部 tmp _SlotItem
// 构造自然发射 ORIG W 0x081347e2 的 C1/C2 弱符号。----
void CharacAdvanceAltarManager::getSlotItem(
    std::map<unsigned char, _SlotItem>& out) const
{
    const AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return;
    }
    const std::map<unsigned char, _Slot>& slots = cur->m_slots1;      // ORIG base+0x84
    for (std::map<unsigned char, _Slot>::const_iterator sit = slots.begin();
         sit != slots.end(); ++sit)
    {
        std::map<int, _BuyItem>::const_iterator bit =
            cur->m_buyItems1.find(static_cast<int>(sit->first));       // 槽号查购买表
        if (bit == cur->m_buyItems1.end())
        {
            continue;
        }
        std::vector<BuyUpgradeData>* upgrades =
            G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(cur->m_ridableId,
                                                               AdvanceAltarShopType::T_0);
        if (upgrades == 0)
        {
            continue;
        }
        BuyUpgradeData* chosen = 0;
        for (std::vector<BuyUpgradeData>::iterator uit = upgrades->begin();
             uit != upgrades->end(); ++uit)
        {
            if (uit->m_field4 == bit->second.m_field4)
            {
                chosen = &(*uit);                     // ORIG 升级表匹配 *(buyIter+8)
                break;
            }
        }
        if (chosen == 0)
        {
            continue;
        }
        const int level = bit->second.m_short8;                        // ORIG +0xc 截断
        FieldData* point = chosen->getFieldDataPoint(level, FieldType::FT_INDEX);
        if (point == 0)
        {
            continue;
        }
        const CItem* item = G_CDataManager()->find_item(point->m_value);
        if (item == 0)
        {
            continue;
        }
        _SlotItem slotItem;                                            // ORIG ctor memset(0xc)
        slotItem.m_itemId = point->m_value;
        slotItem.m_needGage = item->getAdvanceAltarNeedGage();
        out.insert(std::make_pair(sit->first, slotItem));
    }
}

// ---- ORIG W 0x081348c4（objdump thunk：mov (%eax),%eax）——const 版定义，
//      头内唯一声明（非 const 版已删，避免调用方产出错误 mangled）。----
AdvanceAltarData* CharacAdvanceAltarManager::getCurrentAdvanceAltarData() const
{
    // ORIG W thunk mov (%eax),%eax；NK 成员但返回非 const 指针（调用方原地改写），
    // C++ 下需显式去除 const 视图（与 ORIG 语义一致）。
    return m_field0 != 0 ? const_cast<AdvanceAltarData*>(&m_data[0])
                         : static_cast<AdvanceAltarData*>(0);
}

// ---- ORIG 内部只读表 _ZN12advancealtarL11kRewardStarE（rodata 0xc，
//      nm 多副本同值）：按难度 0/1/2 → 通关奖励星 {3,2,1}。ORIG 每个使用 TU
//      各持一份 internal 副本，此处以 static 复刻（无 ODR 冲突）。----
static const int kRewardStar[3] = {3, 2, 1};

// ---- ORIG T 0x0813175a _ZN12advancealtar25CharacAdvanceAltarManager10clearStageEi
//      NS_15StageDifficulty1TENS_9StageType1TEiRiRbS6_（clearStage.md 全文；
//      2026-08-27 LfAltarStage 批）。----
bool CharacAdvanceAltarManager::clearStage(int stageIdx, StageDifficulty::T diff,
                                           StageType::T type, int clearRound,
                                           int& starDelta, bool& unlockBuy,
                                           bool& bOut)
{
    unlockBuy = false;   // ORIG 首指令 *param_7=0（其余引用路径不改写）
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();   // *(int*)this 判空
    if (cur == 0)
    {
        return false;
    }
    if (!G_CDataManager()->m_stageMapList.isExist(stageIdx))
    {
        return false;
    }
    std::map<int, _Stage>::iterator it = cur->m_stages.find(stageIdx);
    if (it == cur->m_stages.end())
    {
        return false;
    }
    if (type == StageType::STAGE_SURVIVAL)                  // 生存关：只刷新最高轮数
    {
        if (static_cast<int>(cur->m_maxClearRound) < clearRound)   // ORIG sext(u16) 比较
        {
            cur->m_maxClearRound = static_cast<short>(clearRound);
            bOut = true;
            cur->m_update = 1;
        }
        return true;
    }
    if (it->second.m_field4 == -1)     // 首次记录 → 解锁商店购买项
    {
        unlockBuy = true;
    }
    if (it->second.m_field4 < static_cast<int>(diff))    // 未达该难度 → 晋级发星
    {
        it->second.m_field4 =
            StageDifficulty::getNextDifficulty(
                static_cast<StageDifficulty::T>(it->second.m_field4));
        starDelta = kRewardStar[diff];
        cur->m_curStar += starDelta;      // +0x0c
        cur->m_starPoint += starDelta;    // +0x14
        cur->m_update = 1;
    }
    return true;
}

// ---- ORIG T 0x081331d4 _ZN12advancealtar25CharacAdvanceAltarManager12setNextStageEi
//      （setNextStage.md 全文；2026-08-27 LfAltarStage 批）。stageIdx==0 走首幕
//      占位；否则按 StageMapList 链推进，下一幕未入 map 时写入占位记录并按链
//      尾标志递归。----
void CharacAdvanceAltarManager::setNextStage(int stageIdx)
{
    AdvanceAltarData* cur = getCurrentAdvanceAltarData();
    if (cur == 0)
    {
        return;
    }
    if (stageIdx == 0)   // 开场：插入 (firstStageId, {firstStageId, -1}) 占位
    {
        StageMapList& stageMapList = G_CDataManager()->m_stageMapList;
        _Stage stage;
        stage.m_field0 = stageMapList.getFirstStageIndex();
        stage.m_field4 = -1;
        cur->m_stages.insert(std::make_pair(stage.m_field0, stage));
        return;
    }
    bool chainFlags = false;
    const int nextStage =
        G_CDataManager()->m_stageMapList.getNextStage(stageIdx, chainFlags);
    if (nextStage == 0)
    {
        return;
    }
    std::map<int, _Stage>::iterator it = cur->m_stages.find(nextStage);
    if (it != cur->m_stages.end())
    {
        return;   // 已有该幕记录则不再推进
    }
    cur->m_field20 = 1;   // ORIG *(char*)(cur+0x20)=1（语义【推断】）
    _Stage stage;
    stage.m_field0 = nextStage;
    stage.m_field4 = -1;
    cur->m_stages.insert(std::make_pair(nextStage, stage));
    cur->m_update = 1;
    if (chainFlags)
    {
        setNextStage(nextStage);
    }
}
}  // namespace advancealtar
// ============================================================================
// StageMapList::{isExist,getNextStage,getFirstStageIndex} —— LfAltarStage 批
// 定义归本 TU；声明在 CDataManager.h::StageMapList。
// ============================================================================

// ---- ORIG T 0x08364df6 _ZNK12StageMapList7isExistEi ----
bool StageMapList::isExist(int stageIdx) const
{
    return m_map.find(stageIdx) != m_map.end();
}

// ---- ORIG W 0x081347c0 _ZNK12StageMapList18getFirstStageIndexEv（inline thunk）----
int StageMapList::getFirstStageIndex() const
{
    return m_firstStageId;
}

// ---- ORIG T 0x08364e58 _ZNK12StageMapList12getNextStageEiRb ----
int StageMapList::getNextStage(int stageIdx, bool& outFlags) const
{
    std::map<int, _StageMapSummary>::const_iterator it = m_map.find(stageIdx);
    if (it == m_map.end())
    {
        return 0;
    }
    std::map<int, _StageMapSummary>::const_iterator next =
        m_map.find(it->second.m_nextId);
    if (next != m_map.end())
    {
        outFlags = next->second.m_flags != 0;
    }
    return it->second.m_nextId;
}

