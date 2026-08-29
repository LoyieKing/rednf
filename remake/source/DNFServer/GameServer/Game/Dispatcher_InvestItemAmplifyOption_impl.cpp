#include "Dispatchers.h"

#include "CDataManager.h"      // G_CDataManager(), WongWork::CItemUpgrade, m_itemUpgrade (+0x20)
#include "CInventory.h"        // getCurCharacInvenR/W, INVEN_TYPE, GetInvenSlot/delete_item/update_item
#include "CItemAmplifier.h"    // CItemAmplifier, stInvestAmplifyOption_t, ENUM_ITEM_AMPLIFY_INVEST_TYPE, stAmplifyOption_t
#include "CItem.h"             // CItem, IsEnableWorld(ENUM_WORLD_TYPE) + IsEnableWorld(const CItem*)
#include "GlobalData.h"        // GlobalData::s_itemAmplifier_
#include "GameWorld.h"         // G_GameWorld(), IsPVPChannel(), IsPvPSkilTreeChannel()
#include "CUserCharacInfo.h"   // get_charac_level(), getCurCharacInvenR/W()（CUser is-a CUserCharacInfo）
#include "Inven_Item.h"        // Inven_Item（0x3d, pack1）
#include "CUser.h"             // CUser::get_state()/CheckItemLock/Send/SendCmdErrorPacket, m_historyLog

// ORIG 自由函数 LineFunc(int, const char*, int, unsigned int) @0x085908d7 T；
// 定义由 CompoundEmblemDeps.cpp 提供（见 CompoundEmblemDeps.h）。
extern int LineFunc(int, const char*, int, unsigned int);

// ORIG 0x082194f4（约 883 指令）：强化投资（增幅强化投点）。
// 恢复链路：解析装备槽/材料槽/强化类型 → 校验 → 消耗材料 → CItemAmplifier 强化 +
// CItemUpgrade 随机强化（investType==2）→ 写回装备槽 → 回复包 header 0xd0。
// 所有校验/解析失败走 LineFunc 断言路径，并返回 LineFunc 的返回值（ORIG 0x08219534 起）。
// 业务校验失败按 ORIG 抛 ENUM_ERROR（整型码）；第 5 项之后随机强化失败走
// SendCmdErrorPacket(0xd0, code) 后返回 0。
int Dispatcher_InvestItemAmplifyOption::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG ±0x012：get_state() != 3 → return LineFunc(0xcc82, ...)
    if (user->get_state() != 3)
    {
        return LineFunc(0xcc82, __PRETTY_FUNCTION__, 0, 0);
    }

    // ---- 解析 packet ----
    unsigned char investType = 0;         // local_35[0]（加固类型 byte）
    unsigned short equipSlot = 0;         // local_38
    unsigned long equipItemIdx = 0;       // local_3c（get_item_idx，8 字节）
    unsigned short materialSlot = 0;      // local_3e
    unsigned long materialItemIdx = 0;    // local_44（get_item_idx）

    if (!packet.get_byte(investType)) return LineFunc(0xcc89, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(equipSlot)) return LineFunc(0xcc8a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_item_idx(equipItemIdx)) return LineFunc(0xcc8b, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(materialSlot)) return LineFunc(0xcc8c, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_item_idx(materialItemIdx)) return LineFunc(0xcc8d, __PRETTY_FUNCTION__, 0, 0);

    char strBuf[0x81] = { 0 };            // local_15f（随机强化名字串）
    unsigned int strLen = 0;              // local_48
    unsigned char count = 0;              // local_49（额外尝试次数 / 强化量）

    if (!packet.get_byte(count)) return LineFunc(0xcc97, __PRETTY_FUNCTION__, 0, 0);
    if (count > 4) return LineFunc(0xcc99, __PRETTY_FUNCTION__, 0, 0);
    if (investType == 2)                                   // 随机强化带名字串
    {
        if (!packet.get_int(strLen)) return LineFunc(0xcca0, __PRETTY_FUNCTION__, 0, 0);
        if (!packet.get_str(strBuf, 0x81, (int)strLen)) return LineFunc(0xcca1, __PRETTY_FUNCTION__, 0, 0);
    }
    if (investType > 2) return LineFunc(0xcca6, __PRETTY_FUNCTION__, 0, 0);

    // ---- 读装备/材料项 ----
    Inven_Item equipInven;                 // local_a1
    Inven_Item materialInven;              // local_de
    stInvestAmplifyOption_t investOpt;     // local_54（+0x00 m_amount / +0x04 m_value，均清 0）
    int upgradeRet = 0;                    // local_58（随机强化的 int& 输出）

    CItem* equipItem = G_CDataManager()->find_item((int)equipItemIdx);  // local_30

    if (user->CheckItemLock(1, (int)equipSlot) != 0) throw (ENUM_ERROR)0xd5;  // 装备被锁
    if (equipItem == 0) throw (ENUM_ERROR)0x11;
    equipInven = user->getCurCharacInvenR()->GetInvenSlot(1, (int)equipSlot);
    if ((int)equipInven.m_addInfo != (int)equipItemIdx) throw (ENUM_ERROR)0x11;  // 槽内 item idx 不符

    materialInven = user->getCurCharacInvenR()->GetInvenSlot(1, (int)materialSlot);
    if ((int)materialInven.m_addInfo != (int)materialItemIdx) throw (ENUM_ERROR)0x11;

    int charLevel = user->get_charac_level();   // CUser is-a CUserCharacInfo
    if (GlobalData::s_itemAmplifier_->checkInvestableItem(equipItem, equipInven, charLevel) != true)
        throw (ENUM_ERROR)8;                    // 装备不可投资

    CItem* materialItem = G_CDataManager()->find_item((int)materialItemIdx);  // local_2c
    if (materialItem == 0) throw (ENUM_ERROR)0x11;
    if (!equipItem->IsEnableWorld(materialItem)) throw (ENUM_ERROR)8;    // EnableWorld(equip, material)
    if (!materialItem->IsEnableWorld(equipItem)) throw (ENUM_ERROR)8;    // 双向世界启用校验

    // ---- 材料信息 ----
    if (GlobalData::s_itemAmplifier_->getInvestMaterialInfo(
            (ENUM_ITEM_AMPLIFY_INVEST_TYPE)investType, materialItemIdx, investOpt) != 1)
        throw (ENUM_ERROR)0x11;                 // 找不到投资信息

    if (investOpt.m_amount == 5 && count == 0) throw (ENUM_ERROR)0x11;   // 需 count 但未给
    if (count != 0)                             // 给定 count：仅允许 m_amount==5 的表项
    {
        if (investOpt.m_amount != 5) throw (ENUM_ERROR)0x11;
        investOpt.m_amount = (int)count;
    }

    unsigned int investAmount = (unsigned int)investOpt.m_amount;   // local_54
    // m_addInfo2（材料槽 +0x07）< m_value（最低需求）→ 材料不足
    if (investAmount == 0 || materialInven.m_addInfo2 < investOpt.m_value)
        throw (ENUM_ERROR)0x11;

    // 投资量 == 装备当前强化能力类型请求同族 → 拒绝
    if (investAmount == ((unsigned char)equipInven.m_amp.getAbilityType() & 0xff))
        throw (ENUM_ERROR)0x17;

    // PvP / PvP 技能树频道拒绝
    GameWorld* gw = G_GameWorld();
    bool isPvP = (gw->IsPVPChannel() || gw->IsPvPSkilTreeChannel());
    if (isPvP) throw (ENUM_ERROR)0x13;

    // 启用该强化投资
    int enableRet = GlobalData::s_itemAmplifier_->enableInvestAmplifyOption(
        equipInven, (ENUM_ITEM_AMPLIFY_INVEST_TYPE)investType);
    if (enableRet != 0) throw (ENUM_ERROR)enableRet;    // ORIG throw local_28

    // 随机强化（investType==2）前置：enableAmplifyRandomUpgrade 填充 upgradeRet
    int ret2 = 0;                                       // local_28 / iVar4
    if (investType == 2)
    {
        ret2 = G_CDataManager()->m_itemUpgrade->enableAmplifyRandomUpgrade(
            user, equipInven, materialInven, upgradeRet);
        if (ret2 == 0x15) throw (ENUM_ERROR)0xcb;
        if (ret2 != 0) throw (ENUM_ERROR)ret2;
    }

    // 消耗材料（count = investOpt.m_value，即材料消耗量）
    if (user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, (int)materialSlot, investOpt.m_value, eItemDelReason_3, true) != 1)
        throw (ENUM_ERROR)0x16;

    // 历史日志（material serial, equip serial, reason=2）
    user->m_historyLog.apply_Item(materialInven.m_addInfo2, equipInven.m_addInfo2, (eApplyItemReason)2);

    // 强化投资
    GlobalData::s_itemAmplifier_->investAmplifyOption(equipItem, equipInven, investOpt);

    // 随机强化（investType==2）后段：用 enableAmplifyRandomUpgrade 的 output
    if (investType == 2)
    {
        int rc = G_CDataManager()->m_itemUpgrade->ProcAmplifyRandomUpgrade(
            user, equipInven, materialInven, upgradeRet, strBuf);
        if (rc != 0)
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xd0, (unsigned char)(rc & 0xff));
            return 0;
        }
    }

    // 写回装备槽
    user->getCurCharacInvenW()->update_item((INVEN_TYPE)1, (int)equipSlot, equipInven);

    // 重读材料槽（回复包用最新状态）
    materialInven = user->getCurCharacInvenR()->GetInvenSlot(1, (int)materialSlot);

    // ---- 回复包 header 0xd0 ----
    PacketGuard resp;
    resp.put_header(1, 0xd0);
    resp.put_byte(1);
    resp.put_byte((int)investType);
    resp.put_short((int)materialSlot);
    resp.put_int(materialInven.m_addInfo2);    // 材料槽最新状态
    resp.put_short((int)equipSlot);
    resp.put_byte((unsigned char)(equipInven.m_amp.getAbilityType() & 0xff));
    resp.put_short((unsigned short)(equipInven.m_amp.getAbilityValue() & 0xffff));
    if (investType == 2)
    {
        resp.put_byte((int)equipInven.GetUpgrade());
    }
    resp.finalize(true);
    user->Send(resp);
    return 0;
}

// ORIG 弱符号（约 5 指令，vtable 占位）：返回 0 与 ORIG identical，勿改。
int Dispatcher_InvestItemAmplifyOption::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)user; (void)msg; (void)param;
    return 0;
}
int Dispatcher_InvestItemAmplifyOption::read(PacketBuf& packet, MSG_BASE& msg)
{
    (void)packet; (void)msg;
    return 0;
}