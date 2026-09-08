// ============================================================================
// df_game_r 还原 —— Arad_IPGProcess / Arad_MileageProcess 实现。
// 归属批次：LinkE2.JpnExImpl（ItemVendingMachine::ProcessIPGUserDataJpnEx 链
// 接收敛）。逐报告/逐指令转写，报告目录 docs/class_func_reports/
// {Arad_IPGProcess,Arad_MileageProcess}/。
//
// 布局实证（objdump 0x817a2ba/0x817a2ce）：
//   ctor 仅 *(this) = ItemVendingMachine*；D1/D2 空体；无 vtable（非虚）。
//   ORIG 各方法把 *(this)（= vm，其 +0x00 为内嵌 WongWork::CCeraShop m_ceraShop，
//   偏移 0 地址等价）按 CCeraShop* 传给 CCeraShop::_process* 家族
//   —— ORIG 原样类型双关，此处 reinterpret_cast 保持一致。
//
// Inven_Item 装载：ORIG (citem->vtable+0x08)(citem,&it) = CItem::make_item(it)
//   （CItem.h 虚表槽 2），随后 it.m_addInfo(+2)=itemIdx、it.m_addInfo2(+7)=stack。
// ============================================================================

#include "Arad_IPGProcess.h"
#include "Arad_MileageProcess.h"

#include "CDataManager.h"    // G_CDataManager / find_item / FindGoods / getItemLimitEditionMgr
#include "CUser.h"           // CUser / cUserHistoryLog（+0x79700）/ CeraShopPaymentType
#include "CInventory.h"      // AddAvatarItem / use_money / MakeItemPacket / Inven_Item
#include "CItem.h"           // isCerashopPackageItem / getUsablePeriod / getExpirationDate
#include "CItemList.h"       // CCeraShopGoods::GetSubGroupIndex / get_rand_int
#include <math.h>     // floor（GiveMileage）
#include <stdlib.h>   // atoi
#include "CMailBoxHelper.h"  // ReqDBSendNewAvatarMailCashShop
#include "RDARScriptStringManager.h"  // findString / g_scriptStringManager_
#include "LogManager.h"
#include "ItemVendingMachine.h"  // BillingRecord
#include "Arad_DataManager.h"    // ARAD::Singleton<Arad_DataManager> / Arad_Mileage_Data
#include "CUserCharacInfo.h"     // getCurCharacNo（经 CUser 继承可见）

#include <stdlib.h>   // atoi
#include <string.h>   // strlen

// ---------------------------------------------------------------------------
// Arad_IPGProcess
// ---------------------------------------------------------------------------
// ORIG W 0x0817a2ba：push ebp; mov esp,ebp; mov 0xc(%ebp),%edx; mov %edx,(%eax); pop ebp; ret
Arad_IPGProcess::Arad_IPGProcess(ItemVendingMachine* vm) : m_vm(vm) {}

// ORIG W 0x0817a2c8：push ebp; mov esp,ebp; pop ebp; ret（空体）
Arad_IPGProcess::~Arad_IPGProcess() {}

// ---------------------------------------------------------------------------
// CashShopModeCoin —— ORIG T 0x0819c434（报告 CashShopModeCoin.md 全文转写）。
// ---------------------------------------------------------------------------
void Arad_IPGProcess::CashShopModeCoin(CUser* user, unsigned short param2,
                                       unsigned int param3, int param4, char buyFlag)
{
    if (buyFlag == 1)
    {
        // esp: shop / user / (uint)param2 / param3 —— mangled (CUser*, t, j, i)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCoinGift(user, param2, param3, param4);
    }
    else
    {
        // esp: shop / user / (uint)param2 / 1 —— mangled (CUser*, t, b)；bool 实参 = 1
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCoin(user, param2, true);
    }
}

// ---------------------------------------------------------------------------
// CashShopModeItem —— ORIG T 0x0819c4a0（报告 CashShopModeItem.md 全文转写）。
// ---------------------------------------------------------------------------
void Arad_IPGProcess::CashShopModeItem(CUser* user, unsigned long itemIdx,
                                       unsigned short stack, int count, long seq,
                                       std::vector<Inven_Item>& out,
                                       WongWork::CCeraShop::stCeraShopItemParam_t& param,
                                       int& restrictOut, unsigned int accId,
                                       int characNo, char buyFlag)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    Inven_Item localItem;
    item->make_item(localItem);
    localItem.m_addInfo = (int)itemIdx;    // ORIG -0x47 = item +2
    localItem.m_addInfo2 = (int)stack;     // ORIG -0x42 = item +7

    if (buyFlag == 1)
    {
        // mangled _processItemGift(CUser*, Inven_Item, j, i)：esp 0x48=accId、0x4c=characNo
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processItemGift(user, localItem, accId, characNo);
    }
    else
    {
        // mangled _processItem(CUser*, Inven_Item, i count, Ri restrict, l seq,
        //   b 0, b 1, Rvector, RN..param_t&)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processItem(
            user, localItem, count, restrictOut, seq, false, true, out, param);
    }
}

// ---------------------------------------------------------------------------
// CashShopModeAvatar —— ORIG T 0x0819c2b4（报告 CashShopModeAvatar.md 全文转写）。
// ---------------------------------------------------------------------------
void Arad_IPGProcess::CashShopModeAvatar(CUser* user, unsigned long itemIdx,
                                         int a3, int a4, char c1, char c2,
                                         char const* str, int& out,
                                         unsigned int accId, int a10, char buyFlag)
{
    (void)a3;  // ORIG p3 在两分支均未使用（ebp 0x14 无读取）

    if (buyFlag == 1)
    {
        // mangled _processAvatarGift(CUser*, i, i, c, c, PKc, j, i)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processAvatarGift(
            user, (int)itemIdx, a4, c1, c2, str, accId, a10);
        return;
    }

    CInventory* inven = user->getCurCharacInvenW();
    int slot = inven->AddAvatarItem((int)itemIdx, a4, false, c1, c2, str,
                                    (eAvatarItemAddReason)0, false, 0);
    if (slot < 0)
    {
        const char* msg = g_scriptStringManager_.findString(4, "game_server_msg_104", (bool*)0);
        int len = (int)strlen(msg);
        int characNo = user->getCurCharacNo();
        WongWork::CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
            user, characNo, (int)itemIdx, a4, c1, c2, str, msg, len);
    }
    else
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1, slot);
    }
}

// ---------------------------------------------------------------------------
// CashShopModeCreature —— ORIG T 0x0819c670（报告 CashShopModeCreature.md 转写）。
// 期限槽实证：esp0x50=08110c60 getUsablePeriod、esp0x54=080f1306 getExpirationDate。
// ---------------------------------------------------------------------------
void Arad_IPGProcess::CashShopModeCreature(CUser* user, unsigned long itemIdx,
                                           unsigned short stack, int& outCodeA,
                                           int& outCodeB, unsigned int accId,
                                           int characNo, char buyFlag)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    Inven_Item localItem;
    item->make_item(localItem);
    localItem.m_addInfo = (int)itemIdx;
    localItem.m_addInfo2 = (int)stack;

    if (buyFlag == 1)
    {
        // mangled _processCreatureGift(CUser*, Inven_Item, j, i, i, i)：
        //   esp 0x48=accId、0x4c=characNo、0x50=usablePeriod、0x54=expirationDate
        int usablePeriod = item->getUsablePeriod();
        int expirationDate = item->getExpirationDate();
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCreatureGift(
            user, localItem, accId, characNo, usablePeriod, expirationDate);
    }
    else
    {
        // mangled _processCreature(CUser*, Inven_Item, Ri, Ri, b, i, i)
        int usablePeriod = item->getUsablePeriod();
        int expirationDate = item->getExpirationDate();
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCreature(
            user, localItem, outCodeA, outCodeB, true, usablePeriod, expirationDate);
    }
}

// ---------------------------------------------------------------------------
// CashShopModeVendingItem —— ORIG T 0x0819c862（报告 CashShopModeVendingItem.md）。
// 非特殊商品：insertItemIntoInventory 直插背包（失败转邮件补发）；
// 特殊商品：ProcessSpecialItem 通道。
// ---------------------------------------------------------------------------
void Arad_IPGProcess::CashShopModeVendingItem(
    CUser* user, unsigned long itemIdx, unsigned short stack, int count,
    long seq, std::vector<Inven_Item>& out,
    WongWork::CCeraShop::stCeraShopItemParam_t& param, int& restrictOut,
    int characNo, char buyFlag)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    Inven_Item localItem;
    item->make_item(localItem);
    localItem.m_addInfo = (int)itemIdx;   // ORIG -0x58 = item +2
    localItem.m_addInfo2 = (int)stack;    // ORIG -0x53 = item +7

    int outType = 0;
    int outValue = 0;
    bool special = false;
    WongWork::CCeraShop* ceraShop = reinterpret_cast<WongWork::CCeraShop*>(m_vm);
    if (!ceraShop->IsSpecialItem((int)itemIdx, outType, outValue, special))
    {
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            localItem, (eItemAddReason)0x1e, true, false);
        if (slot != -1)
        {
            user->SendUpdateItemList(CUser::eSendTarget_1,
                                     (ENUM_ITEMSPACE)0, slot);
            user->SaveInventory();
        }
        else
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewMailCashShop(
                user, localItem, 0, (unsigned int)user->getCurCharacNo(), true, msg,
                (int)strlen(msg));
            slot = 0;
        }
    }
    else
    {
        const bool result = ceraShop->ProcessSpecialItem(
            user, (int)itemIdx, count, restrictOut, seq, true, out, param,
            0, 0, false, 1);
        if (!result)
        {
            LogManager::logFormat(1, "localjapan/Arad_IPGProcess.cpp",
                                  __PRETTY_FUNCTION__, 0x1a2,
                                  "ProcessSpecialItem failed");
        }
    }
}

// ---------------------------------------------------------------------------
// Arad_MileageProcess
// ---------------------------------------------------------------------------
// ORIG W 0x0817a2ce：同 Arad_IPGProcess ctor 形态。
Arad_MileageProcess::Arad_MileageProcess(ItemVendingMachine* vm) : m_vm(vm) {}

// ORIG W 0x0817a2dc：空体。
Arad_MileageProcess::~Arad_MileageProcess() {}

// ---------------------------------------------------------------------------
// BuySuccess —— ORIG T 0x0819e220（报告 BuySuccess.md 全文转写）。
// ---------------------------------------------------------------------------
void Arad_MileageProcess::BuySuccess(CUser* user, int a2, int a3, int a4, int a5,
                                     int a6, int a7, std::vector<Inven_Item>& items)
{
    user->UseMileage(a5);

    const int kResultCode = 0xb;
    const char kGiftFlag = 0;
    PacketGuard packet;
    packet.put_header(1, 0x43);
    packet.put_byte(1);
    packet.put_byte((int)kGiftFlag);
    packet.put_int(kResultCode);
    packet.put_int(a2);
    packet.put_int(a3);
    packet.put_int(a6);
    packet.put_int(a4);
    packet.put_short((int)items.size());
    for (std::vector<Inven_Item>::const_iterator it = items.begin(); it != items.end(); ++it)
    {
        packet.put_int(it->m_addInfo);    // ORIG put_int(it+2)
        packet.put_int(it->m_addInfo2);   // ORIG put_int(it+7)
    }
    if (a6 == a7)
    {
        user->SendCashData(false);
        PacketGuard itemPacket;
        itemPacket.put_header(0, 0xe);
        itemPacket.put_byte(0);
        itemPacket.put_short(2);
        const CInventory* inven = user->getCurCharacInvenR();
        inven->MakeItemPacket((INVEN_TYPE)1, 0, itemPacket);
        inven->MakeItemPacket((INVEN_TYPE)1, 2, itemPacket);
        itemPacket.finalize(true);
        user->Send(itemPacket);
    }
    packet.finalize(true);
    user->Send(packet);
}

// ---------------------------------------------------------------------------
// GiveMileage —— ORIG T 0x0819dc9c（报告 GiveMileage.md 全文转写）。
// 汇编算术：fild rate; fild amount; flds [1000.0f @0x8ba7330]; fdivrp; fmulp;
//   fstpl; call floor; fistp（fldcw 0xc 截断模式）→ (int)floor(...)。
// ---------------------------------------------------------------------------
void Arad_MileageProcess::GiveMileage(CUser* user, int ipgNo, int amount)
{
    if (user == 0)
        return;

    ARAD::Arad_DataManager* mgr = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
    int rate = mgr->getAradMileageData()->find(ipgNo);
    if (rate < 0 || rate > 1000)
    {
        ARAD::Arad_DataManager* mgr2 = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        rate = mgr2->getAradMileageData()->default_value();
    }

    double value = (double)((float)rate / 1000.0f * (float)amount);
    // ORIG fstp 后 call floor（floor 后再截断转换；对正数与 (int) 一致，
    // 负数分支 floor 语义保持）。
    double floored = floor(value);
    user->AddMileage((int)floored, (eCoinAddReason)3);
}

// ---------------------------------------------------------------------------
// BuyFail —— ORIG T 0x0819e50e（报告 BuyFail.md 全文转写）。
// ---------------------------------------------------------------------------
void Arad_MileageProcess::BuyFail(CUser* user, short errCode, int a3, int a4, int a5)
{
    PacketGuard packet;
    // b28c-28：两值必须是** PacketGuard ctor 之后**初始化的非 const 局部——
    // ORIG 0x0819e50e+0x24 在 put_header 前才发 `movl $0xb,-0x10(%ebp)` /
    // `movb $0x0,-0x9(%ebp)`，且两处实参从槽装载
    // （`movsbl -0x9,%eax; mov %eax,0x4(%esp)`、`mov -0x10,%eax; mov %eax,0x4(%esp)`）；
    // const 形被前端折叠成 `movl $imm,0x4(%esp)`（少 2 条）。
    int resultCode = 0xb;        // ORIG -0x10（int 槽）
    char giftFlag = 0;           // ORIG -0x9（signed char 槽 ⇒ movsbl）
    packet.put_header(1, 0x43);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_short((int)errCode);
    packet.put_byte((int)giftFlag);
    packet.put_int(resultCode);
    packet.put_int(a3);
    packet.put_int(a4);
    packet.put_int(a5);
    packet.put_int(-1);
    packet.finalize(true);
    user->Send(packet);
}

// ---------------------------------------------------------------------------
// BuyMileageItem —— ORIG T 0x0819dd4e（报告 BuyMileageItem.md 全文转写）。
// ---------------------------------------------------------------------------
int Arad_MileageProcess::BuyMileageItem(CUser* user, int param2, unsigned long itemIdx,
                                        unsigned short stack, int count, long seq,
                                        int param7, char c1, char c2, int price,
                                        int a11, int a12, char const* str,
                                        WongWork::CCeraShop::stCeraShopItemParam_t& param)
{
    (void)param7;  // ORIG p7（ebp 0x24）在本函数无消费【转写实证】

    CCeraShopGoods* goods = reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(param2));
    if (goods == 0)
        return 0x15;

    Inven_Item localItem;
    CItem* item = 0;

    int subGroup = goods->GetSubGroupIndex();
    if (subGroup != 1)
    {
        item = G_CDataManager()->find_item((int)itemIdx);
        if (item == 0)
            return 0x15;
        item->make_item(localItem);
        localItem.m_addInfo = (int)itemIdx;
        localItem.m_addInfo2 = (int)stack;
    }

    if (user->GetMileage() < price)
        return 0x3ed;

    int outCodeA = 0;  // local_30（creature 结果）
    int outCodeB = 0;  // local_34（错误码）
    std::vector<Inven_Item> items;

    subGroup = goods->GetSubGroupIndex();
    switch (subGroup)
    {
    case 0:
    {
        CInventory* inven = user->getCurCharacInvenW();
        int slot = inven->AddAvatarItem((int)itemIdx, param7, false, c1, c2, str,
                                        (eAvatarItemAddReason)0, false, 0);
        if (slot < 0)
        {
            const char* msg = g_scriptStringManager_.findString(4, "game_server_msg_104", (bool*)0);
            int len = (int)strlen(msg);
            int characNo = user->getCurCharacNo();
            WongWork::CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
                user, characNo, (int)itemIdx, param7, c1, c2, str, msg, len);
        }
        else
        {
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1, slot);
        }
        break;
    }
    case 1:
        // esp: shop / user / (uint)stack / 1（同 CashShopModeCoin 非 gift 分支）
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCoin(user, stack, true);
        break;
    case 2:
    case 7:
    case 9:
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processItem(
            user, localItem, count, outCodeB, seq, false, true, items, param);
        break;
    case 3:
    {
        int usablePeriod = item->getUsablePeriod();
        int expirationDate = item->getExpirationDate();
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCreature(
            user, localItem, outCodeA, outCodeB, true, usablePeriod, expirationDate);
        break;
    }
    default:
        break;
    }

    if (outCodeB == 0)
    {
        BuySuccess(user, param2, (int)stack, outCodeA, price, a11, a12, items);
        // BillingRecord(vm, user, param2, subGroup, price, str, "mileage item", 0, 0ull, 0)
        //   —— ORIG esp0x18..0x28 = "mileage item"/0/0/0（0x819e1b5..0x4e3）。
        m_vm->BillingRecord(user, param2, subGroup, price, str, "mileage item", 0, 0ULL, 0);
    }

    return outCodeB;
}
