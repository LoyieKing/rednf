#include "Dispatchers.h"

// ---------------------------------------------------------------------------
// Dispatcher_UseVendingMachine::dispatch_sig —— 自动售货机（扫地机/道具售货机）
// 购买流程还原。
//
// 依据 docs/class_func_reports/Dispatcher_UseVendingMachine/（ORIG
// dispatch_sig @ 0x0821c2e6，约 1009 指令；连同 6 个辅助 _get/_put/_notify 方法）。
//
// 说明（`IMPL` = 本文件重构决策，非 ORIG 事实）：
//   1. ORIG 的这些辅助方法是类成员（_ZNK28Dispatcher_UseVendingMachine…），
//      Dispatchers.h 类声明未含这些成员且被约定只读，故本文件以 TU 内
//      file-local 辅助函数实现，显式传 CMTRand* 与数据源上下文。
//   2. ORIG 使用 this->m_rand（CMTRand* @ +0x04，由 ctor 以 time 种子创建）。
//      Dispatchers.cpp 的 ctor 为空桩，不改；故 dispatch_sig 内新建本地 CMTRand
//      并按 ORIG ctor 语义以 time 播种，传给各辅助函数。
//   3. 外部依赖已由 producer 在权威头/TU 落地：CVendingMachineStatistic /
//      TimerVendingMachineItemNotify（VendingMachine.h）、CGlobalEffectManager
//      （CGlobalEffectManager.h）、CStackableItem::getGlobalEffectData、
//      user_creature::CCreatureItemConverter::SetEggItem（CInventory.h）。
//      本文件直调这些真实成员，不再定义本地替身。
//   4. 返回码层面的错误路径（ORIG 为 __cxa_throw ENUM_ERROR / LineFunc 断言）
//      在本文件以对应 return 值近似表达，语义与报告一致（见 game_verify_guide）。
// ---------------------------------------------------------------------------

#include <ctime>

#include "CDataManager.h"
#include "CInventory.h"
#include "CUserCharacInfo.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "Inven_Item.h"
#include "CUser.h"
#include "GlobalData.h"
// 权威 WongWork::CGenUniqueNo 见 CInventory.h（含 genIPGNo(ENUM_IPGNO_TYPE,uint,char*) @0x084b7750），
// 故不再 include CGenUniqueNo.h（其类签名不同且与 CInventory.h 冲突）。
#include "CCeraShop.h"
#include "PacketGuard.h"
#include "CSystemTime.h"
#include "PvPTypes.h"   // 权威 CMTRand 声明（PvPTypes.h:178；CMTRand.h 不存在，类定义于 CMTRand.cpp）
#include "VendingMachine.h"   // CVendingMachineStatistic / TimerVendingMachineItemNotify 权威声明
#include "CGlobalEffectManager.h"
#include "CUserProc.h"   // APSystem::CUserProc::ClearActionAndSendtoUser 权威声明

namespace
{
/* =========================================================================
 * 权威数据结构（ORIG 布局；reinterpret 到重建未建模结构）
 * ====================================================================== */

// stVendingMachineOutput_t —— 售货机给/赏物品的加权描述（0xc 字节）。
// ORIG：+0x00 int itemIdx；+0x04 uint rate；+0x08 uint count。
struct VendingOutput
{
    int         m_itemIdx;   // +0x00
    unsigned int m_rate;     // +0x04
    unsigned int m_count;    // +0x08
};

// stVendingMachineInfo_t —— 售货机某一栏的完整信息（头部 + EquipmentParameterInfo）。
// ORIG（dispatch_sig/_getGiveItem/_getBonusItem/_putItemIntoUser 逐字节）：
//   +0x00 int m_field0（=栏位 id，_getVendingMachineInfo 匹配用）
//   +0x04 int m_payType（0=金币 / 1=活动币 / 2=胜点 / else=物品）
//   +0x08 int m_payAmount
//   +0x0c int m_giveCount（_getGiveItem 权重范围-1）
//   +0x10 std::vector<VendingOutput> m_vecGive
//   +0x1c int m_bonusCount（_getBonusItem 权重范围-1）
//   +0x20 std::vector<VendingOutput> m_vecBonus
//   （其后为 EquipmentParameterInfo 其余字段，本流程不访问）
struct VendingInfo
{
    int                      m_field0;   // +0x00
    int                      m_payType;  // +0x04
    int                      m_payAmount;// +0x08
    int                      m_giveCount;// +0x0c
    std::vector<VendingOutput> m_vecGive; // +0x10
    int                      m_bonusCount;// +0x1c
    std::vector<VendingOutput> m_vecBonus; // +0x20
};

// stVendingMachine_t —— +0x00 int m_id；+0x04 vector<VendingInfo>。
// CDataManager.h 的 stVendingMachine_t 仅 char m_pad[0x10]，本视图 ABI 兼容。
struct VendingMachineView
{
    int                    m_id;    // +0x00
    std::vector<VendingInfo> m_vec; // +0x04
};

// _putItemIntoUser / dispatch_sig 共用输出结构（0x10 字节）。
// +0x00 ulong m_itemIdx；+0x04 uint m_count；+0x08 int m_result；
// +0x0c short m_slot；+0x0e byte m_success。
struct PutItemResult
{
    unsigned long  m_itemIdx;   // +0x00
    unsigned int   m_count;     // +0x04
    int            m_result;    // +0x08
    short          m_slot;      // +0x0c
    unsigned char  m_success;   // +0x0e
};

/* =========================================================================
 * 辅助：从售货机表按 id 查找机器；按栏位 id 查找栏信息。
 * ==================================================================== */

const VendingInfo* get_vending_machine_info(unsigned int machineId, unsigned int slotId)
{
    CDataManager* dm = G_CDataManager();
    if (dm == 0)
        return 0;
    const std::vector<stVendingMachine_t>& table = dm->m_vendingMachine;
    for (size_t i = 0; i < table.size(); ++i)
    {
        const VendingMachineView* vm = reinterpret_cast<const VendingMachineView*>(&table[i]);
        if (vm->m_id == static_cast<int>(machineId))
        {
            const std::vector<VendingInfo>& infos = vm->m_vec;
            for (size_t j = 0; j < infos.size(); ++j)
            {
                if (static_cast<unsigned int>(infos[j].m_field0) == slotId)
                    return &infos[j];
            }
            return 0;
        }
    }
    return 0;
}

/* =========================================================================
 * 辅助：按权重随机选一个给/赏物品，写入 out（Inven_Item）。
 * ==================================================================== */

// ORIG _getGiveItem：依 m_giveCount 权重随机选 m_vecGive 中一个输出。
static int pick_give_item(CMTRand* rng, const VendingInfo* info, Inven_Item& out)
{
    out.reset();                  // ORIG：Inven_Item::Inven_Item(param_1)
    out.m_addInfo = -1;           // 初始 idx = -1（“未选择”标记）
    int range = info->m_giveCount - 1;
    if (range < 0)
        return -1;
    unsigned long r = (unsigned long)range;
    int pick = rng->randInt(r);
    const std::vector<VendingOutput>& vec = info->m_vecGive;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (static_cast<unsigned int>(pick) < vec[i].m_rate)
        {
            out.m_addInfo = vec[i].m_itemIdx;
            out.m_addInfo2 = (int)vec[i].m_count;
            return vec[i].m_itemIdx;
        }
    }
    return -1;
}

// ORIG _getBonusItem：依 m_bonusCount 权重随机选 m_vecBonus 中一个输出。
// 额外前置：GlobalData::s_event_manager->GetRepeatEvent(0x23) 的事件开启（vtable+0x34(0)）
//   且 bonus 表非空才给赏。
static int pick_bonus_item(CMTRand* rng, const VendingInfo* info, Inven_Item& out)
{
    out.reset();                  // ORIG：Inven_Item::Inven_Item(param_1)
    out.m_addInfo = -1;
    // ORIG：GetRepeatEvent(0x23) → 事件对象 vtable[+0x34] 未建模（仅 ret 或检查开启位）。
    // 该事件状态接口未在共享头落地，按报告以「事件开启」为默认真值（推断），
    // 仅依赖 bonus 表非空。若后续权威事件检查落地，替换为真实调用。
    bool eventOn = true;
    if ((!eventOn) || info->m_vecBonus.empty())
        return -1;
    int range = info->m_bonusCount - 1;
    if (range < 0)
        return -1;
    unsigned long r = (unsigned long)range;
    int pick = rng->randInt(r);
    const std::vector<VendingOutput>& vec = info->m_vecBonus;
    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (static_cast<unsigned int>(pick) < vec[i].m_rate)
        {
            out.m_addInfo = vec[i].m_itemIdx;
            out.m_addInfo2 = (int)vec[i].m_count;
            return vec[i].m_itemIdx;
        }
    }
    return -1;
}

/* =========================================================================
 * 辅助：把物品/货币/胜点交给用户，填充 result。
 * ==================================================================== */
static void put_item_into_user(CUser* user, unsigned long itemIdx, unsigned int count,
                               PutItemResult& result)
{
    const int PAY_MONEY     = 0;   // 金币
    const int PAY_EVENTCOIN = 1;   // 活动币
    const int PAY_WINPOINT  = 2;   // 胜点
    // （其它值 = 给实物物品，按 itemIdx）

    result.m_itemIdx = itemIdx;
    result.m_count   = count;
    result.m_result  = 0;
    result.m_slot    = -1;
    result.m_success = 1;

    if (result.m_itemIdx == 0xffffffffUL)
        return;

    short slot = -1;

    CUserCharacInfo* chi = static_cast<CUserCharacInfo*>(user);

    if (itemIdx == PAY_MONEY)
    {
        CInventory* invW = chi->getCurCharacInvenW();
        invW->gain_money((int)count, (eMoneyAddReason)0x15, 1, 0);
        slot = 0;
    }
    else if (itemIdx == PAY_EVENTCOIN)
    {
        CInventory* invW = chi->getCurCharacInvenW();
        int cur = invW->GetEventCoin();
        invW->SetEventCoin((unsigned int)(cur + (int)count));
        CInventory* invR = const_cast<CInventory*>(chi->getCurCharacInvenR());
        unsigned int newCoin = invR->GetEventCoin();
        // ORIG: cUserHistoryLog::EventCoinAdd((cUserHistoryLog*)(user+0x79700),coin,amount,6)
        user->m_historyLog.EventCoinAdd((int)newCoin, (int)count, (eCoinAddReason)6);
        slot = 1;
    }
    else if (itemIdx == PAY_WINPOINT)
    {
        // ORIG _ZN5CUser12gainWinPointEi12eWPAddReason @0x0864fd2c（CUser 域已补声明+实现）。
        user->gainWinPoint((int)count, (eWPAddReason)3);
        slot = 2;
    }
    else
    {
        // 实物物品
        CDataManager* dm = G_CDataManager();
        CStackableItem* item = static_cast<CStackableItem*>(dm->find_item((int)itemIdx));
        if (item == 0)
        {
            result.m_success = 0;
            return;
        }

        Inven_Item outItem;
        CDataManager* dm2 = G_CDataManager();
        dm2->getItemList()->create_item((ENUM_ITEM_CREATE_TYPE)0, outItem, (int)count);

        int itemType = item->GetItemType();
        if (itemType == 0x17)
        {
            // 需要全局效果：按 m_vec280（vector<stGlobalEffect_t>）逐项注册。
            // ORIG：registNewEffect(m_field0, m_field4, characName, 5, m_field8)。
            const std::vector<stGlobalEffect_t>& eff = item->getGlobalEffectData();
            const char* characName = chi->getCurCharacName();
            for (size_t e = 0; e < eff.size(); ++e)
            {
                GlobalData::s_globalEffectManager_->registNewEffect(
                    (unsigned int)eff[e].m_field0, eff[e].m_field4, characName, 5,
                    (long)eff[e].m_field8);
            }
            slot = -1;
        }
        else if (item->IsAvatarItem())
        {
            unsigned int characNo = chi->getCurCharacNo();
            char ipgBuf[0x18];
            WongWork::CGenUniqueNo::genIPGNo((WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)4,
                                             characNo, ipgBuf);
            char* ipg = ipgBuf;
            int usablePeriod = item->getUsablePeriod();
            CInventory* invW = chi->getCurCharacInvenW();
            slot = (short)invW->AddAvatarItem((int)itemIdx, usablePeriod, /*b*/0, /*c*/0,
                                              /*d*/0, ipg, (eAvatarItemAddReason)0xe,
                                              /*e*/0, /*f*/(unsigned short)0);
            result.m_result = 1;
        }
        else if (item->IsCreatureItem() && item->GetItemType() == 0x16)
        {
            // 生物（蛋/宠物），先 SetEggItem 再 insert
            if (outItem.m_field1 == 5 /* ITEM_TYPE_CREATURE_EQUIP */)
            {
                bool ok = user_creature::CCreatureItemConverter::SetEggItem(&outItem);
                if (ok)
                {
                    CInventory* invW = chi->getCurCharacInvenW();
                    slot = (short)invW->insertItemIntoInventory(outItem,
                                                                (eItemAddReason)0x23, 1, 0);
                    if (slot >= 0)
                    {
                        int expire = item->getExpirationDate();
                        int usable = item->getUsablePeriod();
                        CInventory* invW2 = chi->getCurCharacInvenW();
                        invW2->GetCreatureMgrW()->InsertCreatureItem(&outItem, (int)slot,
                                                                     0x23, usable, expire);
                    }
                }
                result.m_result = 7;
            }
        }
        else
        {
            // 普通实物
            // ORIG：item 非 0x10（stackable 普通物）时先查 Cera 特殊物品处理；
            //   报告路径依赖 CCeraShop 特殊物品与受限物品逻辑。此处按报告保留
            //   "非特殊 → 直接 insert" 的真实分支（restrictCode 路径待 producer）。
            bool special = false;
            bool skipInsert = false;
            if (item->GetItemType() != 0x10)
            {
                int dummy = 0;
                skipInsert = GlobalData::s_pCeraShop->IsSpecialItem((int)itemIdx,
                                                                    dummy, dummy, special);
                // [TODO] producer 落地后：ProcessSpecialItem / SaveFeaturedIdx /
                //   SetRestrictedGoods / SendRestrictedGoods 阶段按报告补齐。
            }
            if ((!skipInsert) && (!special))
            {
                CInventory* invW = chi->getCurCharacInvenW();
                slot = (short)invW->insertItemIntoInventory(outItem,
                                                            (eItemAddReason)0x23, 1, 0);
                if (slot == -1)
                    result.m_success = 0;
            }
        }
    }

    result.m_slot = slot;
    if (result.m_success != 0)
    {
        CVendingMachineStatistic* st = GetInstanceVendingMachineStatistic();
        if (st)
            st->addOutputItem(result.m_itemIdx, result.m_count);
    }
}

/* =========================================================================
 * 辅助：给用户发"物品已放入"计时通知（若该物品尚未出现在用户拥有的列表）。
 * ==================================================================== */
static void notify_item_info(const VendingInfo* info, CUser* user,
                             const PutItemResult& result)
{
    // ORIG：在 GlobalData::s_globalEffectManager_ 的效果列表中查找 (result.m_itemIdx,
    //   result.m_count) pair；若不存在则 registTimer。因 stGlobalEffect_t 表未建模，
    //   ORIG std::find 语义按「表为空 → 总注册」近似（推断）。
    unsigned int uid = user->GetUID();
    TimerVendingMachineItemNotify::registTimer((int)uid, result.m_itemIdx,
                                               result.m_count, 5);
}

} // namespace

/* =========================================================================
 * dispatch_sig —— 主入口。
 * ==================================================================== */
int Dispatcher_UseVendingMachine::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0)
        return 0;
    if (user->get_state() != 3)
        return 0;
    if (user->CheckInTrade())
        return 0;

    unsigned int machineId = 0;
    unsigned int slotId    = 0;
    short        pos       = -1;

    if (!packet.get_int(machineId))
        return 0;   // ORIG LineFunc(0xd029) 断言路径
    if (!packet.get_int(slotId))
        return 0;   // ORIG LineFunc(0xd02a) 断言路径
    if (!packet.get_short(pos))
        return 0;   // ORIG LineFunc(0xd02b) 断言路径

    const VendingInfo* info = get_vending_machine_info(machineId, slotId);
    if (info == 0)
        return 0;   // ORIG LineFunc(0xd02f/0xd033) 断言路径

    // 随机源：ORIG ctor 以 time 种子构造 this->m_rand。
    CMTRand rng;
    rng.seed((unsigned long)time(0));

    // 给/赏物品数组（两槽：槽 0 给物品、槽 1 赏物品）。
    Inven_Item give[2];
    give[0].reset();
    give[1].reset();
    give[0].m_addInfo = -1;
    give[1].m_addInfo = -1;
    for (int slot = 0; slot < 2; ++slot)
    {
        if (slot == 0)
        {
            // 选给物品；若非特殊有效则重选（最多 2 次，仍-1 则放弃该槽）
            for (int attempt = 0; attempt < 3; ++attempt)
            {
                Inven_Item picked;
                pick_give_item(&rng, info, picked);
                give[slot] = picked;
                if (give[slot].m_addInfo == -1)
                    break;
                CItem* git = G_CDataManager()->find_item(give[slot].m_addInfo);
                if (git != 0 && git->GetItemType() == 0x17)
                    continue;   // 特殊物品，重选
                break;          // 普通物品即采用
            }
        }
        else
        {
            // 赏物品
            Inven_Item picked;
            pick_bonus_item(&rng, info, picked);
            give[slot] = picked;
        }
    }

    // 空位检查：多背包空间与货架空位（ORIG 逐背包区段 check_empty_count 1..10）。
    bool invFull = true;
    CUserCharacInfo* chi = static_cast<CUserCharacInfo*>(user);
    {
        const int zones[7] = { 1, 2, 3, 10, 9, 5, 6 };
        for (int z = 0; z < 7; ++z)
        {
            CInventory* inv = chi->getCurCharacInvenW();
            if (inv->check_empty_count((Inven_Item::ITEM_TYPE)zones[z], 2) != 1)
            {
                invFull = false;
                break;
            }
        }
        if (invFull)
        {
            CInventory* inv = chi->getCurCharacInvenW();
            if (inv->GetRemainCapacity((INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8) > 1)
                invFull = false;
        }
    }
    if (invFull)
        return 0;   // ORIG throw ENUM_ERROR(4)：背包满

    // 扣款
    int remainingMoney = 0;
    if (info->m_payType == 0)
    {
        CInventory* invW = chi->getCurCharacInvenW();
        if (invW->use_money(info->m_payAmount, (eMoneySubReason)0x1c, 1) != 1)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        remainingMoney = invW->get_money();
    }
    else if (info->m_payType == 1)
    {
        const CInventory* invR = chi->getCurCharacInvenR();
        unsigned int total = invR->getTotalCoin();
        if (total < (unsigned int)info->m_payAmount)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        for (int i = 0; i < info->m_payAmount; ++i)
        {
            CInventory* invW = chi->getCurCharacInvenW();
            invW->UseCoin((eCoinSubReason)6);
        }
        const CInventory* invR2 = chi->getCurCharacInvenR();
        remainingMoney = (int)invR2->getTotalCoin();
    }
    else if (info->m_payType == 2)
    {
        unsigned int wp = chi->GetWinPoint();
        if (wp < (unsigned int)info->m_payAmount)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        user->useWinPoint(info->m_payAmount, (eWPSubReason)4);
        remainingMoney = chi->GetWinPoint();
    }
    else
    {
        // 以指定栏物品（pos 指向的）支付：payType 即所需支付物品 idx。
        const CInventory* invR = chi->getCurCharacInvenR();
        Inven_Item payItem = invR->GetInvenSlot((INVEN_TYPE)1, pos);
        if (payItem.m_addInfo != info->m_payType)
            return 0;   // ORIG throw ENUM_ERROR(0x11)
        if (payItem.m_fieldd < info->m_payAmount)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        CInventory* invW = chi->getCurCharacInvenW();
        if (invW->delete_item((INVEN_TYPE)1, (int)pos, info->m_payAmount,
                              (eItemDelReason)0x1c, 1) != 1)
            return 0;   // ORIG throw ENUM_ERROR(0x16)
        const CInventory* invR2 = chi->getCurCharacInvenR();
        remainingMoney = invR2->GetInvenSlot((INVEN_TYPE)1, pos).m_fieldd;
    }

    CVendingMachineStatistic* vst = GetInstanceVendingMachineStatistic();
    if (vst)
        vst->incUseCount();

    // 输出
    PutItemResult result[2];
    for (int i = 0; i < 2; ++i)
        put_item_into_user(user, give[i].m_addInfo, give[i].m_addInfo2, result[i]);

    for (int i = 0; i < 2; ++i)
    {
        if (result[i].m_itemIdx != 0xffffffffUL && result[i].m_count != 0)
            notify_item_info(info, user, result[i]);
    }

    // 回包：header 0xda
    PacketGuard resp;
    resp.put_header(1, 0xda);
    resp.put_byte(1);
    resp.put_short(pos);
    resp.put_int(remainingMoney);
    for (int i = 0; i < 2; ++i)
    {
        resp.put_item_idx(result[i].m_itemIdx);
        resp.put_int((int)result[i].m_count);
    }
    int idx = resp.get_index();
    int cnt = 0;
    resp.put_short(0);
    for (int i = 0; i < 2; ++i)
    {
        if (result[i].m_itemIdx != 0xffffffffUL && result[i].m_slot != -1 &&
            result[i].m_success != 0)
        {
            CInventory* inv = const_cast<CInventory*>(chi->getCurCharacInvenR());
            inv->MakeItemPacket((INVEN_TYPE)result[i].m_slot, (int)result[i].m_slot, resp);
            ++cnt;
        }
    }
    resp.put_short(idx, cnt);
    resp.finalize(true);
    user->Send(resp);
    APSystem::_ActionGroupIndex grp;
    grp.m_value = 0x27;
    APSystem::CUserProc::ClearActionAndSendtoUser(user, grp, 0, (ENUM_PACKETCLASS)0);
    return 0;
}

// ============================================================================
// ORIG 0x08233dbc W _ZN28Dispatcher_UseVendingMachine7processEP5CUserR8MSG_BASER9ParamBase
//   objdump 逐指令核实：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
//   —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0821c2e6，本文件上方)
//   及 6 个 _get/_put/_notify 辅助方法。忠实转写空返回，勿臆造。
int Dispatcher_UseVendingMachine::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                          ParamBase& /*param*/)
{
    return 0;
}

// ORIG 0x08233db2 W _ZN28Dispatcher_UseVendingMachine4readER9PacketBufR8MSG_BASE
//   objdump 核实同为纯空 ret0（0x08233db2..0x08233dbc）；ORIG 的包读取在
//   dispatch_sig 内完成（get_int/get_int/get_short 直读 PacketBuf）。
int Dispatcher_UseVendingMachine::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;
}