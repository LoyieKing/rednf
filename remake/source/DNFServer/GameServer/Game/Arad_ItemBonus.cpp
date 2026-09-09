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
    int m_field15b;           // +0x15b ← mail.m_field8（同型直存，ORIG 0x08196c1a）
    // ORIG 0x08196c2b `movzbl (%eax),%eax; movzbl %al,%edx; mov %edx,0x15f(%eax)`
    // ⇒ 目标为 4 字节（uchar→uint 双段零扩展后 32 位 store）。
    unsigned int m_field15f;  // +0x15f ← mail.m_isSystemMail
    unsigned char m_pad163[5];// +0x163（凑 SIG_MAILBOX_SEND_NEW_MAIL 0x168）
};
#pragma pack(pop)

static_assert(sizeof(SigMailboxSendNewMailMirror) == 0x168,
              "SIG_MAILBOX_SEND_NEW_MAIL mirror size");

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
    // P0：ORIG 无 mgr 局部（Get→getPurchaseLimitItem→find 寄存器直传）；
    // 尾部 choose 岛（test;je;mov$0;jmp;mov$1）＝ P56 if-return 形。
    if (Singleton<Arad_DataManager>::Get()->getPurchaseLimitItem()->find(itemIndex, out))
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// importCashShopPurcahseBonusItemList —— ORIG 0x088b62ec
// （docs/class_func_reports/ARAD/importCashShopPurcahseBonusItemList.md）。
// 错误码口径与汇编一致；EOF 路径 ORIG `mov $0,%ebx`（+0x34c）⇒ 正常结束返回 0。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: ORIG 无 result/again 局部（P0：错误码经 %ebx 直返、
// 循环条件仅 ScanType；%esi 为 EH 清理用的返回标志）；`int key = 0;` 先于
// bonus/list 构造、`key = ScanInt(&ok);` 独立赋值（ORIG +0x76/+0x93）。
int importCashShopPurcahseBonusItemList(
    char* path,
    std::map<int, std::vector<PurcahseBonus> >& out)
{
    if (!loadRDARScriptFile("", path))
        return 10;

    typedef std::vector<PurcahseBonus> BonusVec;

    std::string token;
    bool ok = false;

    // ORIG +0x41 条件头在循环顶（入口 `jmp +0x41`、body 尾 `jmp +0x41`），
    // token 判等为 `if (token == "[bonus]") { body }`（`_ZSteq`，je 落 +0x40 nop）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[bonus]")
        {
            int key = 0;
            PurcahseBonus bonus;
            BonusVec list;

            key = ScanInt(&ok);
            if (!ok)
                return 27;  // 0x1b：goods 行首列缺失

            while (true)
            {
                bonus.m_flag = (unsigned char)ScanInt(&ok);
                if (!ok)
                    break;                   // 直接进入查重/插入段

                bonus.m_field4 = ScanInt(&ok);
                if (!ok) return 35;          // 0x23：type/item id 缺失
                bonus.m_field8 = (short)ScanInt(&ok);
                if (!ok) return 38;          // 0x26：数量缺失

                // 具名 found 强制 LHS（find）先求值，RHS end() 在 find 后（ORIG 序）。
                std::vector<PurcahseBonus>::iterator found =
                    std::find(list.begin(), list.end(), bonus);
                if (found != list.end())
                {
                    LogManager::logFormat(
                        1, "../localjapan/RDARScriptCeraShop.cpp",
                        __FUNCTION__, 0x2b,
                        "[PURCAHSE BONUS] invalid purcahse count "
                        "(goods:%u, count:%d)",
                        key, (int)bonus.m_flag);
                    return 44;               // 0x2c：重复门槛档
                }
                list.push_back(bonus);
            }

            std::map<int, BonusVec>::iterator it = out.find(key);
            if (it != out.end())
            {
                LogManager::logFormat(
                    1, "../localjapan/RDARScriptCeraShop.cpp",
                    __FUNCTION__, 0x36,
                    "[PURCAHSE BONUS] duplicate purcahse item (goods:%d)", key);
                return 55;                   // 0x37：key 重复
            }

            out.insert(std::make_pair(key, list));
        }
    }

    return 0;
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
    // ORIG 求值序：find 先、end 后（左侧表达式先物化）+ P56 choose 岛形。
    std::map<int, std::vector<PurcahseBonus> >::iterator it = m_bonusMap.find(key);
    if (it == m_bonusMap.end())
        return false;
    return true;
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
    // ORIG 0x081abf8x：`call _findBonusItem; xor $0x1,%eax; test; jne 尾`＝
    // !boolcall() 负形早退（P168a①），正向 if 落 je 形。
    if (!_findBonusItem(it->second, local, threshold))
        return;
    _processBonusItem(user, goodsIdx, local);
}

// ---------------------------------------------------------------------------
// _findBonusItem —— ORIG 0x081ac04c：遍历 list 取最后一个 m_flag <= threshold
// 的元素 memcpy 0xa 字节到 out；无命中返回 false。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: ORIG best 为**迭代器**（best=list.end()，槽 -0x18），
// it 槽 -0x14；命中 `best=it`（无 operator*），尾判 `best==list.end()`（-0xc）后
// `out=*best`（结构赋值 ⇒ 0xa 字节 memcpy + 自拷贝守卫，ORIG 0x081ac0d3..0x081ac136）。
bool CeraShopPurcahseCountBonus::_findBonusItem(
    const std::vector<PurcahseBonus>& list, PurcahseBonus& out,
    unsigned short threshold)
{
    std::vector<PurcahseBonus>::const_iterator it = list.begin();
    std::vector<PurcahseBonus>::const_iterator best = list.end();
    for (; it != list.end(); ++it)
    {
        if ((unsigned short)it->m_flag <= threshold)
            best = it;
    }
    if (best == list.end())
        return false;

    out = *best;
    return true;
}

// ---------------------------------------------------------------------------
// _processBonusItem —— ORIG 0x081ac13e（_processBonusItem.md 全文转写）：
//   type==1 代币；type==5 里程券；其它按 itemIdx 发放物品。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: verdict=IDENTICAL_AE 332=332，p167 42=42 CLEAR。
// 关键旋钮：switch(m_field4) 三路 cmp/je/jmp（P17/P46/P251）；P0 无 oldPay/
// amount/curPay/mileageF 具名局部（各使用点重算，x87 序 price*(amount/1000.0f)）；
// P212 全头部声明复现槽深序（goods -0x30→msg -0x1c）；P221 cMyTrace 全表达式
// 临时链翻 sibling 槽方向；P92 尾日志读 inv 字段；P315/P254 m_field4 改 unsigned
// 翻 find_item 实参两段装载。ORIG 0x081ac13e。
void CeraShopPurcahseCountBonus::_processBonusItem(CUser* user, int goodsIdx,
                                                   const PurcahseBonus& bonus)
{
    // P212 全头部声明：按 ORIG 槽深序（goods -0x30 → msg -0x1c）声明。
    CCeraShopGoods* goods;
    int price;
    int mileage;
    CItem* item;
    int slot;
    const char* msg;

    if (user == 0)
        return;

    // P17/P46/P251：ORIG `cmp $1;je;cmp $5;je;jmp`（值全程 %eax 无槽）⇒ switch。
    switch (bonus.m_field4)
    {
    case 1:
        // 代币通道：P0——ORIG 无 oldPay/amount/curPay 具名局部（各使用点重算
        // `(unsigned short)bonus.m_field8`，值经 %ebx 跨 receiver call 保活）。
        user->getCurCharacInvenW()->SetPayCoin(
            user->getCurCharacInvenR()->GetPayCoin() +
            (unsigned short)bonus.m_field8);

        user->m_historyLog.PayCoinAdd(user->getCurCharacInvenR()->GetPayCoin(),
                                      (unsigned short)bonus.m_field8,
                                      (eCoinAddReason)5);
        user->SaveMoneyCoin();
        user->SendCoin();

        cMyTrace(kProcessPretty, 0x45, 0)(
            "[PURCAHSE BONUS] add coin (mid:%u, coin:%d)",
            user->get_acc_id(), (unsigned short)bonus.m_field8);
        return;

    case 5:
        // 里程券通道：floor(amount/1000.0f * price) → AddMileage(reason 5)。
        goods = (CCeraShopGoods*)G_CDataManager()->FindGoods(goodsIdx);
        if (goods == 0)
        {
            cMyTrace(kProcessPretty, 0x5f, 0)(
                "[PURCAHSE BONUS] goods is null (goods:%d)", goodsIdx);
            return;
        }
        price = goods->GetCeraPrice();
        // ORIG x87 序：fild price → fild amount → flds 1000 → fdivrp → fmulp
        // ⇒ 原码 `price * ((float)(unsigned short)m_field8 / 1000.0f)`。
        mileage = (int)floor(
            (float)price *
            ((float)(unsigned short)bonus.m_field8 / 1000.0f));
        user->AddMileage(mileage, (eCoinAddReason)5);   // ORIG reason 5
        user->SendCashData(false);

        cMyTrace(kProcessPretty, 0x67, 0)(
            "[PURCAHSE BONUS] add mileage coupon (mid:%u, mileage:%d)",
            user->get_acc_id(), mileage);
        return;

    default:
        break;
    }

    // 物品通道：itemIdx 查表 → make_item → 补 idx/count → 入包。
    item = G_CDataManager()->find_item(bonus.m_field4);
    if (item == 0)
    {
        cMyTrace(kProcessPretty, 0x71, 0)(
            "[PURCAHSE BONUS] item is null (item:%u)",
            bonus.m_field4);
        return;
    }

    Inven_Item inv;
    item->make_item(inv);
    inv.m_addInfo = bonus.m_field4;              // +0x02
    inv.m_addInfo2 = (unsigned short)bonus.m_field8;  // +0x07（ORIG 零扩展）

    if (item->IsCreatureItem() || item->IsAvatarItem())
    {
        cMyTrace(kProcessPretty, 0x7c, 0)(
            "[PURCAHSE BONUS] invalid item type (mid:%u, item:%u)",
            user->get_acc_id(), bonus.m_field4);
        return;
    }

    slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        inv, (eItemAddReason)0x1f, true, false);     // ORIG reason 0x1f
    // P47/P222：ORIG `cmpl $-1,slot; je <mail 外置块>`，update 直落。
    if (slot != -1)
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
        user->SaveInventory();
    }
    else
    {
        msg = g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
        WongWork::CMailBoxHelper::ReqDBSendNewMail(
            user, inv, 0, user->getCurCharacNo(), true, msg, strlen(msg));
    }

    // P92：日志实参从 inv 载荷字段读取（ORIG -0xcb/-0xc6）。
    cMyTrace(kProcessPretty, 0x96, 0)(
        "[PURCAHSE BONUS] add item. (mid:%u, item:%u, count:%d)",
        user->get_acc_id(), inv.m_addInfo, inv.m_addInfo2);
}

// ---------------------------------------------------------------------------
// CheckAccountItem 依赖的 PurchaseLimitItem 查询说明见头注释。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// send_mail —— ORIG 0x08196b03（send_mail.md 全文转写）：邮件流打包入 DB 队列。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: 依 ORIG 0x08196b03 段逐条复形——P259 Acquire 内联进
// CStreamGuard ctor（无 stream 具名局部）；item 指针具名局部（-0x10）；strlen 结果
// 具名 len（-0xc）再入 strncpy 实参；m_field15f 4 字节 store。
int send_mail(ARAD_DATAS::AradMailInfo& info)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire(kAradFuncFile, 0x3a), true);

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
    {
        const int len = (int)strlen(info.m_content);
        strncpy(sig->m_content, info.m_content, len);
    }

    // ORIG +0x155 处才 `lea 0x20(%info); mov %eax,-0x10` ⇒ 指针局部声明在物品段前。
    ARAD_DATAS::AradMailItemInfo* mailItem = &info.m_item;
    sig->m_field2a = mailItem->m_flag;
    sig->m_field2b = mailItem->m_fieldC;
    sig->m_field2f = mailItem->m_field4;
    sig->m_field34 = mailItem->m_fieldA;
    sig->m_field33 = mailItem->GetItemAttr();
    sig->m_field36 = mailItem->m_field10;

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
// b28d-solo-Arad_ItemBonus: P0/P52 — ORIG 无 area/vill 具名局部（get_area 结果
// 留 %ebx、getCurCharacVill 留 %esi 跨 G_GameWorld() 保活），实参内联单语句。
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

    G_GameWorld()->send_to_area(user->getCurCharacVill(), user->get_area(false),
                                pg);
}

// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_secret_shop_event_item —— ORIG 0x08196ed7（报告全文转写）。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: P0 — ORIG 无 rabbits 具名引用（每使用点重算
// `stackable->m_arad148.m_vec20`）；box/it 均在循环外默认构造（+0x15a/+0x165），
// 循环内 `box = *it;`；lastSlot 分支 ORIG `setne;test;je <mail>`（P225 形）。
void ARAD_ITEM::arad_secret_shop_event_item(CUser* user, unsigned long param,
                                            CStackableItem* stackable)
{
    typedef std::vector<CRabbitBox> RabbitVec;

    if (stackable->m_arad148.m_vec20.empty())
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

        G_GameWorld()->send_to_area(user->getCurCharacVill(),
                                    user->get_area(false), pg);
        return;
    }

    int lastSlot = -1;
    // P74/P50：box 与 it 均默认构造（ORIG +0x15a/+0x165），循环内赋值。
    CRabbitBox box;
    RabbitVec::iterator it;
    it = stackable->m_arad148.m_vec20.begin();
    while (it != stackable->m_arad148.m_vec20.end())
    {
        box = *it;
        // P0/P48：ORIG 无 rabbitIdx 局部，getItemIndex() 在使用点各调一次
        // （+0x1bc find_item 实参 / +0x1f5 inv.m_addInfo）。
        CItem* item = G_CDataManager()->find_item(box.getItemIndex());
        if (item != 0)
        {
            Inven_Item inv;
            // ORIG 序：+0x1e7 C1Ev → +0x1ef/+0x203 两字段写 → +0x20b make_item。
            inv.m_addInfo = box.getItemIndex();            // +0x02
            inv.m_addInfo2 = box.getItemCount();           // +0x07
            item->make_item(inv);
            lastSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
                inv, (eItemAddReason)0xd, true, false);   // ORIG reason 0xd
            // P225：ORIG `cmpl $-1;setne;test;je <mail>`（update 直落、mail 外置）
            // ⇒ while+goto 形；label 置于 ++it 前（ORIG +0x348 nop/+0x349 ++it）。
            while (lastSlot != -1)
            {
                if (!user->SendUpdateItemList(CUser::eSendTarget_1,
                                              (ENUM_ITEMSPACE)0, lastSlot))
                    return;                            // LAB_0819735a 提前结束
                goto slot_done;
            }
            WongWork::CMailBoxHelper::ReqDBSendNewMail(
                user, inv, 0, user->getCurCharacNo(), true, 0, 0);
        }
    slot_done:
        ++it;
    }

    PacketGuard pg;
    pg.clear();
    pg.put_header(0, 0x100);
    pg.put_byte(1);
    pg.put_int(param);
    pg.put_int(0x708);
    pg.finalize(true);

    G_GameWorld()->send_to_area(user->getCurCharacVill(),
                                user->get_area(false), pg);
}

// ---------------------------------------------------------------------------
// ARAD_ITEM::arad_process_item —— ORIG 0x081973fa（arad_process_item.md 转写）。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: 依 ORIG 0x0819741c..0x081974de 两枚 int 局部——
// restrictCode=-1（-0x18：尾判 ==-1 与 SendUpdateItemList 实参来源）与 code=0
// （-0x1c：&code 作 ProcessSpecialItem 第 4 参出参）；ORIG 未把 code 写回
// restrictCode（原码如此，邮件路径恒走）。P168a①：`if (!call())` 直产
// call;xor$1;test;je 且 then 块内联、log 段外置（ORIG +0x194）。
bool ARAD_ITEM::arad_process_item(CUser* user, Inven_Item const& item)
{
    if (user == 0 || item.m_addInfo == 0)
        return false;

    int restrictCode = -1;
    int code = 0;

    // ORIG 构造序：items vector → sysTime → GetRestrictCode。
    std::vector<Inven_Item> items;
    const int secTime = GlobalData::s_systemTime_.getCurSec();  // getCurSec()
    // P167：ORIG 直接读 G_CDataManager()+0xc（m_itemList）调 GetRestrictCode，
    // 不经 getItemList()（后者 -O0 未内联反产外联 call）
    const int restrictSrc = G_CDataManager()->m_itemList->GetRestrictCode(
        item.m_addInfo);

    WongWork::CCeraShop::stCeraShopItemParam_t param;

    // ORIG 第 3 参为 GetRestrictCode 的返回值、第 4 参为 code 出参引用；
    // curSec 以 long 低半传递（ORIG 仅赋值低半栈槽）。
    if (!GlobalData::s_pCeraShop->ProcessSpecialItem(
            user, item.m_addInfo, restrictSrc, code, secTime, true,
            items, param, '\0', '\0', false, 1))
    {
        // P47/P222：ORIG `cmp $-1; je <mail 外置块>`，update 直落 —— 故取反极性。
        if (restrictCode != -1)
        {
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0,
                                     restrictCode);
            user->SaveInventory();
        }
        else
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewMail(
                user, item, 0, user->getCurCharacNo(), true, msg, strlen(msg));
        }
    }
    else
    {
        LogManager::logFormat(
            1, kAradFuncFile, __PRETTY_FUNCTION__, 0x187,
            "Error process Item (charac:%u, item:%u, count:%d)",
            (unsigned int)user->getCurCharacNo(),
            (unsigned int)item.m_addInfo, item.m_addInfo2);
        return false;
    }
    return true;
}

}  // namespace ARAD
