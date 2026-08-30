// ============================================================================
// df_game_r 还原 —— ARAD 物品/奖励域实现（W6_ARAD_ItemBonus 批次独占 TU）。
//
// 符号清单（nm df_game_r 核实，全部本域产出）：
//   ARAD::CheckAccountItem(CUser*, uint, PurchaseLimitItem::Info&) T @0x08196ac8
//   ARAD::send_mail(ARAD_DATAS::AradMailInfo&)                     T @0x08196b03
//   ARAD::importCashShopPurcahseBonusItemList(char*, map&)         T @0x088b62ec
//   ARAD::ARAD_ITEM::arad_process_item(CUser*, Inven_Item const&)  T @0x081973fa
//   ARAD::ARAD_ITEM::arad_secret_shop_event_buff(CUser*, ulong)    T @0x08196dd3
//   ARAD::ARAD_ITEM::arad_secret_shop_event_item(CUser*, ulong, CStackableItem*) T @0x08196ed7
//   ARAD::CeraShopPurcahseCountBonus::{LoadScript 0x081abf14,
//     GiveBonusItem 0x081abf44, CheckBonusItem 0x081abff0,
//     _findBonusItem 0x081ac04c, _processBonusItem 0x081ac13e}     T
//   PurcahseBonus ctor / operator==、ARAD_DATAS MailInfo/ItemInfo ctor、
//   GetItemAttr 为 ORIG W 弱符号 → inline 定义于 Arad_ItemBonus.h /
//   ItemVendingMachine.h（与 ORIG 相同 TU 归属）。
// ============================================================================

#include <math.h>
#include <string.h>

#include <algorithm>
#include <utility>

#include "Arad_ItemBonus.h"

#include "Arad_DataManager.h"          // Singleton<Arad_DataManager>/getPurchaseLimitItem
#include "CMailBoxHelper.h"            // WongWork::CMailBoxHelper::ReqDBSendNewMail
#include "CDataManager.h"
#include "CCeraShop.h"
#include "CInventory.h"
#include "CItemList.h"
#include "CUserCharacInfo.h"
#include "GameTypes.h"                 // cMyTrace / Inven_Item 完整定义链
#include "GlobalData.h"
#include "Inven_Item.h"
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include "RDARScriptStringManager.h"
#include "GameWorld.h"                 // G_GameWorld / send_to_area

#include "DNFLexWrapper.h"             // ScanType / ScanInt
#include "NpcScript.h"                 // loadRDARScriptFile
#include "CUser.h"
#include "CSystemTime.h"

namespace {
const char kProcessPretty[] =
    "void ARAD::CeraShopPurcahseCountBonus::_processBonusItem(CUser*, int, "
    "const ARAD::PurcahseBonus&)";
const char kAradFuncFile[] = "localjapan/Arad_Function_Set.cpp";

// ---------------------------------------------------------------------------
// send_mail 写入目标：SIG_MAILBOX_SEND_NEW_MAIL（SigTypes.h 目前为
// m_data[0x168] pad）的字段化镜像。
// 【ABI 镜像】按 ORIG send_mail 反汇编写入偏移逐字段命名，pack(1)，
// sizeof=0x160 ≤ SigTypes.h 实际尺寸 0x168。字段语义见各注释。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct SigMailboxSendNewMailMirror
{
    char m_name[0x1d];        // +0x000 收件人名（strncpy 不保证 NUL）
    unsigned char m_pad1d;    // +0x01d
    int m_field1e;            // +0x01e ← mail.m_field10
    int m_field22;            // +0x022 ← mail.m_fieldC
    int m_field26;            // +0x026 ← mail.m_field4
    unsigned char m_field2a;  // +0x02a ← mail.item.m_flag
    int m_field2b;            // +0x02b ← mail.item.m_fieldC
    int m_field2f;            // +0x02f ← mail.item.m_field10
    unsigned char m_field33;  // +0x033 ← mail.item.GetItemAttr()
    unsigned short m_field34; // +0x034 ← mail.item.m_fieldA
    int m_field36;            // +0x036 ← mail.item.m_field10
    unsigned char m_pad3a[0xa];
    unsigned char m_field44;  // +0x044 置 0
    unsigned short m_field45; // +0x045 置 0
    unsigned char m_pad47[0xf];
    unsigned char m_field56;  // +0x056 置 0
    unsigned char m_pad57;
    unsigned char m_field58;  // +0x058 置 0
    unsigned short m_field59; // +0x059 ← mail.m_field14（short）
    char m_content[0x100];    // +0x05b memset 0 后 strncpy content
    unsigned int m_field15b;  // +0x15b ← mail.m_field8
    unsigned char m_field15f; // +0x15f ← mail.m_isSystemMail
};
#pragma pack(pop)

}  // namespace

namespace ARAD_DATAS
{
}  // namespace ARAD_DATAS

namespace ARAD
{

// ---------------------------------------------------------------------------
// CheckAccountItem —— ORIG 0x08196ac8（docs/class_func_reports/ARAD/
// CheckAccountItem.md）：单例 PurchaseLimitItem 查询包装，命中→false，
// 未命中→true。
// ---------------------------------------------------------------------------
bool CheckAccountItem(CUser* user, unsigned int itemIndex,
                      class PurchaseLimitItem::Info& out)
{
    (void)user;  // ORIG 不使用 user 形参（Ghidra/汇编均无引用）
    Arad_DataManager* mgr = Singleton<Arad_DataManager>::Get();
    return !mgr->getPurchaseLimitItem()->find(itemIndex, out);
}

// ---------------------------------------------------------------------------
// importCashShopPurcahseBonusItemList —— ORIG 0x088b62ec
// （docs/class_func_reports/ARAD/importCashShopPurcahseBonusItemList.md）。
// 错误码口径与汇编一致；文件扫描正常结束后的返回值取最后一次 insert 结果
// （成功=1）【推断：ORIG 在 EOF 路径复用上轮 ebx，指令层面未再赋值】。
// ---------------------------------------------------------------------------
int importCashShopPurcahseBonusItemList(
    char* path,
    std::map<int, std::vector<PurcahseBonus> >& out)
{
    if (!loadRDARScriptFile("", path))
        return 10;

    typedef std::vector<PurcahseBonus> BonusVec;

    int result = 0;
    bool again = true;
    bool ok = false;
    std::string token;

    while (again && ScanType(token, true))
    {
        if (token != "[bonus]")
            continue;

        const int key = ScanInt(&ok);
        if (!ok)
            return 27;  // 0x1b：goods 行首列缺失

        PurcahseBonus bonus;
        BonusVec list;

        while (true)
        {
            bonus.m_flag = (unsigned char)ScanInt(&ok);
            if (!ok)
                break;                       // 直接进入查重/插入段

            bonus.m_field4 = ScanInt(&ok);
            if (!ok) { result = 35; break; } // 0x23：type/item id 缺失
            bonus.m_field8 = (short)ScanInt(&ok);
            if (!ok) { result = 38; break; } // 0x26：数量缺失

            if (std::find(list.begin(), list.end(), bonus) != list.end())
            {
                LogManager::logFormat(
                    1, "../localjapan/RDARScriptCeraShop.cpp",
                    __FUNCTION__, 0x2b,
                    "[PURCAHSE BONUS] invalid purcahse count (goods:%u, count:%d)",
                    key, (int)bonus.m_flag);
                result = 44;                 // 0x2c：重复门槛档
                break;
            }
            list.push_back(bonus);
        }

        if (result == 44 || result == 35 || result == 38)
            break;                           // esi=0 → 结束循环返回错误码

        std::map<int, BonusVec>::iterator it = out.find(key);
        if (it != out.end())
        {
            LogManager::logFormat(
                1, "../localjapan/RDARScriptCeraShop.cpp",
                __FUNCTION__, 0x36,
                "[PURCAHSE BONUS] duplicate purcahse item (goods:%d)", key);
            result = 55;                     // 0x37：key 重复
            break;
        }

        out.insert(std::make_pair(key, list));
        result = 1;                          // 本节解析成功
        again = true;                        // 继续扫描下一节
    }

    return result;
}

// ---------------------------------------------------------------------------
// CeraShopPurcahseCountBonus::LoadScript —— ORIG 0x081abf14
// （docs/class_func_reports/ARAD__CeraShopPurcahseCountBonus/LoadScript.md）：
// return import(...) < 1 即失败返回 true。
// ---------------------------------------------------------------------------
bool CeraShopPurcahseCountBonus::LoadScript()
{
    // ORIG 0x081abf2f setg：import(...) > 0 → false，否则 true。
    if (importCashShopPurcahseBonusItemList(
            "CashShop/PurcahseBonusItem.shp", m_bonusMap) > 0)
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// CheckBonusItem —— ORIG 0x081abff0：map.find(key) != end()。
// ---------------------------------------------------------------------------
bool CeraShopPurcahseCountBonus::CheckBonusItem(int key)
{
    return m_bonusMap.find(key) != m_bonusMap.end();
}

// ---------------------------------------------------------------------------
// GiveBonusItem —— ORIG 0x081abf44（GiveBonusItem.md）：
// keyCheck(goodsIdx) 命中后 find(key)->second 上做阈值匹配再发放；
// itemIdx 形参在 ORIG 中未使用，保持签名一致。
// ---------------------------------------------------------------------------
void CeraShopPurcahseCountBonus::GiveBonusItem(CUser* user,
                                               unsigned long itemIdx,
                                               int goodsIdx,
                                               unsigned short threshold)
{
    (void)itemIdx;
    if (!CheckBonusItem(goodsIdx))
        return;

    PurcahseBonus local;
    std::map<int, std::vector<PurcahseBonus> >::iterator it =
        m_bonusMap.find(goodsIdx);
    if (_findBonusItem(it->second, local, threshold))
        _processBonusItem(user, goodsIdx, local);
}

// ---------------------------------------------------------------------------
// _findBonusItem —— ORIG 0x081ac04c：遍历 list 取最后一个 m_flag <= threshold
// 的元素 memcpy 0xa 字节到 out；无命中返回 false。
// ---------------------------------------------------------------------------
bool CeraShopPurcahseCountBonus::_findBonusItem(
    const std::vector<PurcahseBonus>& list, PurcahseBonus& out,
    unsigned short threshold)
{
    const PurcahseBonus* best = 0;
    for (std::vector<PurcahseBonus>::const_iterator it = list.begin();
         it != list.end(); ++it)
    {
        if ((unsigned short)it->m_flag <= threshold)
            best = &*it;
    }
    if (best == 0)
        return false;

    memcpy(&out, best, 10);
    return true;
}

// ---------------------------------------------------------------------------
// _processBonusItem —— ORIG 0x081ac13e（_processBonusItem.md 全文转写）：
//   type==1 代币；type==5 里程券；其它按 itemIdx 发放物品。
// ---------------------------------------------------------------------------
void CeraShopPurcahseCountBonus::_processBonusItem(CUser* user, int goodsIdx,
                                                   const PurcahseBonus& bonus)
{
    if (user == 0)
        return;

    if (bonus.m_field4 == 1)
    {
        // 代币通道：PayCoin += amount → history log(5) → SaveMoneyCoin/SendCoin。
        const unsigned int oldPay =
            user->getCurCharacInvenR()->GetPayCoin();
        const unsigned short amount = (unsigned short)bonus.m_field8;
        user->getCurCharacInvenW()->SetPayCoin(oldPay + amount);

        const unsigned int curPay = user->getCurCharacInvenR()->GetPayCoin();
        user->m_historyLog.PayCoinAdd((int)curPay, amount,
                                      (eCoinAddReason)5);
        user->SaveMoneyCoin();
        user->SendCoin();

        cMyTrace trace(kProcessPretty, 0x45, 0);
        trace("[PURCAHSE BONUS] add coin (mid:%u, coin:%d)",
              user->get_acc_id(), (int)amount);
        return;
    }

    if (bonus.m_field4 == 5)
    {
        // 里程券通道：floor(amount/1000.0f * price) → AddMileage(reason 5)。
        CCeraShopGoods* goods = (CCeraShopGoods*)G_CDataManager()->FindGoods(goodsIdx);
        if (goods == 0)
        {
            cMyTrace trace(kProcessPretty, 0x5f, 0);
            trace("[PURCAHSE BONUS] goods is null (goods:%d)", goodsIdx);
            return;
        }
        const int price = goods->GetCeraPrice();
        const double mileageF = floor(
            (double)(((float)bonus.m_field8 / 1000.0f) * (float)price));
        const int mileage = (int)mileageF;
        user->AddMileage(mileage, (eCoinAddReason)5);   // ORIG reason 5
        user->SendCashData(false);

        cMyTrace trace(kProcessPretty, 0x67, 0);
        trace("[PURCAHSE BONUS] add mileage coupon (mid:%u, mileage:%d)",
              user->get_acc_id(), mileage);
        return;
    }

    // 物品通道：itemIdx 查表 → make_item → 补 idx/count → 入包。
    CItem* item = G_CDataManager()->find_item(bonus.m_field4);
    if (item == 0)
    {
        cMyTrace trace(kProcessPretty, 0x71, 0);
        trace("[PURCAHSE BONUS] item is null (item:%u)",
              (unsigned int)bonus.m_field4);
        return;
    }

    Inven_Item inv;
    item->make_item(inv);
    inv.m_addInfo = bonus.m_field4;              // +0x02
    inv.m_addInfo2 = bonus.m_field8;             // +0x07（零扩展）

    if (item->IsCreatureItem() || item->IsAvatarItem())
    {
        cMyTrace trace(kProcessPretty, 0x7c, 0);
        trace("[PURCAHSE BONUS] invalid item type (mid:%u, item:%u)",
              user->get_acc_id(), (unsigned int)bonus.m_field4);
        return;
    }

    const int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        inv, (eItemAddReason)0x1f, true, false);     // ORIG reason 0x1f
    if (slot == -1)
    {
        const char* msg =
            g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewMail(
            user, inv, 0, user->getCurCharacNo(), true, msg, strlen(msg));
    }
    else
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        user->SaveInventory();
    }

    cMyTrace trace(kProcessPretty, 0x96, 0);
    trace("[PURCAHSE BONUS] add item. (mid:%u, item:%u, count:%d)",
          user->get_acc_id(), (unsigned int)bonus.m_field4,
          (int)(unsigned short)bonus.m_field8);
}

// ---------------------------------------------------------------------------
// CheckAccountItem 依赖的 PurchaseLimitItem 查询说明见头注释。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// send_mail —— ORIG 0x08196b03（send_mail.md 全文转写）：邮件流打包入 DB 队列。
// ---------------------------------------------------------------------------
int send_mail(ARAD_DATAS::AradMailInfo& info)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire(kAradFuncFile, 0x3a);
    CStreamGuard guard(stream, true);

    guard.operator*()->operator<<(0x80);   // 流头 type（ORIG 同槽位 int<<）
    guard.operator*()->operator<<(-1);

    SigMailboxSendNewMailMirror* sig =
        reinterpret_cast<SigMailboxSendNewMailMirror*>(
            guard->GetInBuffer<SIG_MAILBOX_SEND_NEW_MAIL>());

    sig->m_field22 = info.m_fieldC;
    sig->m_field1e = info.m_field10;
    sig->m_field15f = info.m_isSystemMail;
    sig->m_field15b = info.m_field8;
    sig->m_field26 = info.m_field4;
    sig->m_field59 = (unsigned short)info.m_field14;
    memset(sig->m_content, 0, sizeof(sig->m_content));
    strncpy(sig->m_name, info.m_name, 0x1d);
    if (info.m_content != 0)
        strncpy(sig->m_content, info.m_content, strlen(info.m_content));

    sig->m_field2a = info.m_item.m_flag;
    sig->m_field2b = info.m_item.m_fieldC;
    sig->m_field2f = info.m_item.m_field4;
    sig->m_field34 = info.m_item.m_fieldA;
    sig->m_field33 = info.m_item.GetItemAttr();
    sig->m_field36 = info.m_item.m_field10;

    sig->m_field44 = 0;
    sig->m_field45 = 0;
    sig->m_field58 = 0;
    sig->m_field56 = 0;

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    return 1;
}

// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_secret_shop_event_buff —— ORIG 0x08196dd3（报告全文转写）。
// ---------------------------------------------------------------------------
void ARAD_ITEM::arad_secret_shop_event_buff(CUser* user, unsigned long param)
{
    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0x100);
    pg.put_byte(1);
    pg.put_int(param);
    pg.put_int(0x708);
    pg.finalize(true);
    user->Send(pg);

    const int area = user->get_area(false);
    const int vill = user->getCurCharacVill();
    G_GameWorld()->send_to_area(vill, area, pg);
}

// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_secret_shop_event_item —— ORIG 0x08196ed7（报告全文转写）。
// ---------------------------------------------------------------------------
void ARAD_ITEM::arad_secret_shop_event_item(CUser* user, unsigned long param,
                                            CStackableItem* stackable)
{
    typedef std::vector<CRabbitBox> RabbitVec;
    RabbitVec& rabbits = stackable->m_arad148.m_vec20;   // +0x148+0x20（AradItemInfo 子对象）

    if (rabbits.empty())
        return;
    if (stackable->get_index() == 0x28be4b)
        return;

    if (stackable->get_index() == 0x28be6a)
    {
        // 纯事件广播分支（不发放物品）。
        PacketGuard pg;
        pg.clear();
        pg.put_header(0, 0x100);
        pg.put_byte(1);
        pg.put_int(param);
        pg.put_int(0x708);
        pg.finalize(true);
        user->Send(pg);

        const int area = user->get_area(false);
        const int vill = user->getCurCharacVill();
        G_GameWorld()->send_to_area(vill, area, pg);
        return;
    }

    int lastSlot = -1;
    for (RabbitVec::iterator it = rabbits.begin(); it != rabbits.end(); ++it)
    {
        CRabbitBox box = *it;                          // ORIG copy ctor
        const int rabbitIdx = box.getItemIndex();
        CItem* item = G_CDataManager()->find_item(rabbitIdx);
        if (item == 0)
            continue;

        Inven_Item inv;
        item->make_item(inv);
        inv.m_addInfo = rabbitIdx;                     // +0x02
        inv.m_addInfo2 = box.getItemCount();           // +0x07
        lastSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
            inv, (eItemAddReason)0xd, true, false);   // ORIG reason 0xd
        if (lastSlot == -1)
        {
            WongWork::CMailBoxHelper::ReqDBSendNewMail(
                user, inv, 0, user->getCurCharacNo(), true, 0, 0);
        }
        else if (!user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0,
                                           lastSlot))
        {
            return;                                    // LAB_0819735a 提前结束
        }
    }

    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0x100);
    pg.put_byte(1);
    pg.put_int(param);
    pg.put_int(0x708);
    pg.finalize(true);

    const int area = user->get_area(false);
    const int vill = user->getCurCharacVill();
    G_GameWorld()->send_to_area(vill, area, pg);
}

// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_process_item —— ORIG 0x081973fa（arad_process_item.md 转写）。
// ---------------------------------------------------------------------------
bool ARAD_ITEM::arad_process_item(CUser* user, Inven_Item const& item)
{
    if (user == 0 || item.m_addInfo == 0)
        return false;

    int restrictCode = 0;

    // ORIG 构造序：items vector → sysTime → GetRestrictCode。
    std::vector<Inven_Item> items;
    const int secTime = GlobalData::s_systemTime_.getCurSec();  // getCurSec()
    const int restrictSrc = G_CDataManager()->getItemList()->GetRestrictCode(
        item.m_addInfo);

    WongWork::CCeraShop::stCeraShopItemParam_t param;

    // ORIG 第 3 参为 GetRestrictCode 的返回值、第 4 参为输出 code 引用
    // 【ABI 口径按 callsite 汇编槽位直转】；curSec 以 long 低半传递（高半 0，
    //【推断：ORIG 仅赋值低半栈槽】）。
    const bool ok = GlobalData::s_pCeraShop->ProcessSpecialItem(
        user, item.m_addInfo, restrictSrc, restrictCode, secTime, true,
        items, param, '\0', '\0', false, 1);
    if (ok)
    {
        LogManager::logFormat(
            1, kAradFuncFile,
            __PRETTY_FUNCTION__,
            0x187, "Error process Item (charac:%u, item:%u, count:%d)",
            (unsigned int)user->getCurCharacNo(), (unsigned int)item.m_addInfo,
            (unsigned int)item.m_addInfo2);
        return false;
    }

    if (restrictCode == -1)
    {
        const char* msg =
            g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewMail(
            user, item, 0, user->getCurCharacNo(), true, msg, strlen(msg));
    }
    else
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, restrictCode);
        user->SaveInventory();
    }
    return true;
}

}  // namespace ARAD
