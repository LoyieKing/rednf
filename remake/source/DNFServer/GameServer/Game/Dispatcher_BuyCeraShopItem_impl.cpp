#include "Dispatchers.h"
#include "GlobalData.h"
#include "CDataManager.h"
#include "CUserCharacInfo.h"
#include "CItemList.h"
#include "CItem.h"
#include "GameWorld.h"
#include "GameRand.h"
#include "GameTypes.h"
#include "LogManager.h"
#include "Arad_DataManager.h"
#include "ServiceRestrictManager.h"
#include "SigTypes.h"
#include "CParty.h"
#include "CEquipItem.h"
class CDungeon;  // 仅 CDungeon* 指针使用，前向声明即可；避免引入 CDungeon.h 触发 QuickParty 重复定义

// ============================================================================
// Dispatcher_BuyCeraShopItem::dispatch_sig 还原
//   ORIG _ZN26Dispatcher_BuyCeraShopItem12dispatch_sigEP5CUserR9PacketBuf @ 0x081fd396 T
// 报告：docs/class_func_reports/Dispatcher_BuyCeraShopItem/dispatch_sig.md
// 辅助：docs/class_func_reports/Dispatcher_BuyCeraShopItem/useCountDownCoinInFreeCoinDungeon.md
//
// 【依赖说明】下列 ORIG 符号当前重建尚无 producer（未派发/未还原/无权威头）。
// 本 TU 内只做「局部最小声明（推断）」并调用，绝不伪造实现体，也不重复定义。
// 依赖 producer 落定后应改为 include 权威头 + 真实调用，并删除本地声明。
//   blocked_dependencies（已向 Main 上报）：
//   B1 WongWork::CCeraShop::ProcessError     _ZN8WongWork9CCeraShop12ProcessErrorEP5CUsertijiic @0x8323978  (无 producer)
//   B2 ItemVendingMachine::BuyItem           _ZN18ItemVendingMachine7BuyItemEP5CUserRK21MSG_BUY_CERASHOP_ITEM @0x854c4d0  (无 producer)
//   B3 private_store::CPrivateStore::isDollCreated _ZN13private_store13CPrivateStore13isDollCreatedEv @0x82345e4  (未还原)
//   B4 CSecu_ProtectionField::GetCashShopErr  _ZN21CSecu_ProtectionField14GetCashShopErrEi @0x82881a2  (头未声明)
//   B5 LineFunc                             _Z8LineFunciPKcij @0x85908d7  (无权威头)
//   B6 MSG_CHECK_MAIL_RECEIVER::set<...>     模板无 producer
//   B7 CEquipItem::getAvatarOptionSize       _ZNK10CEquipItem19getAvatarOptionSizeEv @0x81803bc  (头未声明)
//
// 注：useCountDownCoinInFreeCoinDungeon 为同类成员但未在只读头 Dispatchers.h 声明，
//   本 TU 无法产出该成员符号；其逻辑按报告内联（useCountDownCoinInFreeCoinDungeon_inline）。
// ============================================================================

// CCeraShop 权威声明见 CCeraShop.h；CMailBoxHelper 权威声明见 CGameManager.h
//（均经下方 ItemVendingMachine.h / CGameManager.h include；此处不再重复定义，避免 ODR）。
// ::ItemVendingMachine（权威声明见 ItemVendingMachine.h；BuyItem 返回 int，matches ORIG）。
#include "ItemVendingMachine.h"

// MSG_CHECK_MAIL_RECEIVER 为全局作用域权威定义（CGameManager.h，ORIG _ZN23MSG_CHECK_MAIL_RECEIVER..无 WongWork），
// 含 set<ENUM_MAILBOX_CHECKTYPE,MSG_T> 模板。WongWork::CMailBoxHelper::ReqDBCheckReceiver 在 WongWork 命名空间。
#include "CGameManager.h"

// CSecu_ProtectionField（GlobalData.h 仅前置；Check/GetCashShopErr 取成员形态符号）。
#ifndef DNF_ENUM_SECURITY_PROTCTION_DEFINED
#define DNF_ENUM_SECURITY_PROTCTION_DEFINED
enum SECURITY_PROTCTION
{
    SECURITY_PROTCTION_0 = 0,
    SECURITY_PROTCTION_0x16 = 0x16
};
#endif
class CSecu_ProtectionField
{
public:
    int Check(CUser* user, SECURITY_PROTCTION type);
    int GetCashShopErr(int err);
private:
    char m_pad[0x18];
};

// private_store：CPrivateStoreMgr 真实符号由 CPrivateStoreMgr.cpp 提供；isDollCreated 无 producer。
namespace private_store
{
class CPrivateStore
{
public:
    bool isDollCreated();
private:
    char m_pad[0x4c];
};
class CPrivateStoreMgr
{
public:
    CPrivateStoreMgr();
    ~CPrivateStoreMgr();
    bool IsBusyPrivateStore(CUser* user);
    CPrivateStore* FindStoreSeller(CUser* user);
};
CPrivateStoreMgr* GetInstancePrivateStoreMgr();
}  // namespace private_store

// LineFunc / NumberToString（ORIG 自由函数，权威头缺失但符号真实存在）。
extern int LineFunc(int, const char*, int, unsigned int);
extern char* NumberToString(unsigned int value, int radix);

// CEquipItem::getAvatarOptionSize 权威声明见 CEquipItem.h（ORIG W 0x81803bc）。
// 本 TU include "CEquipItem.h"，不做局部重定义。

// ============================================================================
// 单条商城购买条目（还原自报告逐字节读序，条目步长 0x1cf）
// ============================================================================
struct stBuyCeraEntry
{
    unsigned char f_slot;              // 条目字节0：槽位/头像选项
    unsigned char f_tag;               // 条目字节1
    unsigned int  productNo;           // product_no（int）
    unsigned char subCount;            // 子项数
    struct { unsigned int idx; unsigned char val; } sub[0x24];
    unsigned char optCount;            // 选项项数
    struct { unsigned long itemIdx; unsigned short a; unsigned short b; } opt[0x24];
};

// MSG_BUY_CERASHOP_ITEM 内的条目区字节（供 BuyItem / 收件人校验构造）
struct stBuyCeraDump
{
    unsigned char f_slot;    // +0x30 base 条目字节0
    unsigned char f_tag;     // +0x31
    unsigned int  productNo; // +0x34
    unsigned char subCount;  // +0x38
    char          pad[3];
};

// ============================================================================
// useCountDownCoinInFreeCoinDungeon_inline —— ORIG 0x081fe4b8（内联，见头部注释）
// ============================================================================
static bool useCountDownCoinInFreeCoinDungeon_inline(CUser* user, unsigned char itemCount,
                                                     unsigned int productNo0)
{
    if (user == 0) return false;
    if (itemCount != 1) return false;
    CCeraShopGoods* goods = (CCeraShopGoods*)G_CDataManager()->FindGoods((int)productNo0);
    if (goods == 0) return false;
    if (goods->GetItemIdx() != 1) return false;
    CParty* party = (CParty*)user->GetParty();
    if (party == 0) return false;
    CDungeon* dungeon = *(CDungeon**)((char*)party + 0xcac);
    if (dungeon == 0) return false;
    return party->checkFreeRevivalCondition(dungeon) ? true : false;
}

int Dispatcher_BuyCeraShopItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    static const char kPretty[] = "virtual int Dispatcher_BuyCeraShopItem::dispatch_sig(CUser*, PacketBuf&)";
    stBuyCeraEntry entries[0x24];
    char           recvName[0x1e];
    unsigned char  itemCount = 0;
    unsigned char  buytype   = 0;
    short          subIdx    = 0;
    int            recvLen   = 0;
    unsigned int   firstProduct = 0;
    std::memset(entries, 0, sizeof(entries));
    std::memset(recvName, 0, sizeof(recvName));

    // ---- acc_id trace（NumberToString + cMyTrace）----
    char* idStr = NumberToString(user->get_acc_id(), 0);
    cMyTrace tr0(kPretty, 0x8f66, 0);
    tr0("Dispatcher_BuyCeraShopItem(m_id(%s))", idStr);

    // ---- user/trade/state 校验 ----
    if (user == 0 || user->CheckInTrade()) return 0;
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return LineFunc(0x8f7b, kPretty, 0, 0);

    // ---- private store 忙检查 ----
    if (private_store::GetInstancePrivateStoreMgr()->IsBusyPrivateStore(user))
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x3c, 0, 0, 1, 1, (char)0);
        return 0;
    }

    // ---- trade 复查 ----
    if (user->CheckInTrade())
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x13, 0, 0, 1, 1, (char)0);
        return 0;
    }

    // ---- get_short(subIdx) ----
    if (!packet.get_short(subIdx))
        return LineFunc(0x8fa9, kPretty, 0, 0);
    if (subIdx < 0 || subIdx > 1)
        return LineFunc(0x8fad, kPretty, 0, 0);

    // ---- get_byte(buytype) ----
    if (!packet.get_byte(buytype))
        return LineFunc(0x8fb7, kPretty, 0, 0);

    // ---- buytype==1 → get_int(recvLen)+get_str(recvName, 0x1e, recvLen) ----
    if (buytype == 1)
    {
        recvLen = 0;
        if (!packet.get_int(recvLen))
            return LineFunc(0x8fbb, kPretty, 0, 0);
        if (!packet.get_str(recvName, 0x1e, recvLen))
            return LineFunc(0x8fbc, kPretty, 0, 0);
    }

    // ---- buytype==1 && subIdx==1 → 拒绝 ----
    if (buytype == 1 && subIdx == 1)
        return LineFunc(0x8fc2, kPretty, 0, 0);

    // ---- 整合 PvP base channel / 渠道类型拦截 ----
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        if (G_GameWorld()->GetChannelType() != 0xf)
        {
            GlobalData::s_pCeraShop->ProcessError(user, 0x3c, 0, 0, 1, 1, (char)0);
            return 0;
        }
        if (buytype == 1)
        {
            GlobalData::s_pCeraShop->ProcessError(user, 0x3c, 0, 0, 1, 1, (char)0);
            return 0;
        }
    }

    // ---- 计费 trace（NEXON BILLING）----
    cMyTrace trB(kPretty, 0x9004, 0);
    trB("NEXON BILLING : BuyCeraShopItem m_id(%s), charac_no(%d), charac_name(%s), buytype(%d)",
        idStr, user->getCurCharacNo(), user->getCurCharacName(), (int)buytype);

    // ---- get_byte(itemCount) 校验 0<,<0x24 ----
    if (!packet.get_byte(itemCount))
        return LineFunc(0x9017, kPretty, 0, 0);
    if (itemCount == 0 || itemCount >= 0x24)
    {
        GlobalData::s_pCeraShop->ProcessError(user, 1, 0, firstProduct, 1, 1, (char)0);
        return 0;
    }

    // ---- 逐条解析 ----
    for (int n = 0; n < (int)itemCount; ++n)
    {
        stBuyCeraEntry& e = entries[n];
        if (!packet.get_byte(e.f_slot))   return LineFunc(0x9017, kPretty, 0, 0);
        if (!packet.get_byte(e.f_tag))    return LineFunc(0x9018, kPretty, 0, 0);
        if (!packet.get_int(e.productNo)) return LineFunc(0x9019, kPretty, 0, 0);
        if (!packet.get_byte(e.subCount)) return LineFunc(0x9017, kPretty, 0, 0);
        if (e.subCount > 0x23)
        {
            GlobalData::s_pCeraShop->ProcessError(user, 1, 0, e.productNo, 1, 1, (char)0);
            return 0;
        }

        // -- 无子项且 f_slot==0xff 时做头像选项随机（ORIG 分支）--
        if (e.f_slot == 0xff && e.subCount == 0)
        {
            CCeraShopGoods* goods = (CCeraShopGoods*)G_CDataManager()->FindGoods((int)e.productNo);
            if (goods == 0) return LineFunc(0x9032, kPretty, 0, 0);
            int itemIdx = goods->GetItemIdx();
            CItem* item = G_CDataManager()->find_item(itemIdx);
            if (item != 0 && item->IsAvatarItem())
            {
                CEquipItem* eq = (CEquipItem*)item;
                int r = get_rand_int(eq->getAvatarOptionSize());   // B7
                if (r < 100) e.f_slot = (unsigned char)r;
            }
        }

        // -- 子项表 (subCount) --
        for (int k = 0; k < (int)e.subCount; ++k)
        {
            if (!packet.get_int(e.sub[k].idx))   return LineFunc(0x9045, kPretty, 0, 0);
            if (!packet.get_byte(e.sub[k].val))  return LineFunc(0x9046, kPretty, 0, 0);
        }

        // -- 选项数（第二段）--
        if (!packet.get_byte(e.optCount)) return LineFunc(0x9017, kPretty, 0, 0);
        if (e.optCount >= 0x24)
        {
            GlobalData::s_pCeraShop->ProcessError(user, 1, 0, e.productNo, 1, 1, (char)0);
            return 0;
        }

        // -- 选项表 (optCount) --
        for (int j = 0; j < (int)e.optCount; ++j)
        {
            if (!packet.get_item_idx(e.opt[j].itemIdx)) return LineFunc(0x9053, kPretty, 0, 0);
            if (!packet.get_short(e.opt[j].a))          return LineFunc(0x9054, kPretty, 0, 0);
            if (!packet.get_short(e.opt[j].b))          return LineFunc(0x9055, kPretty, 0, 0);
        }

        // -- 重复选项校验（同 itemIdx + 同 a 视为重复）--
        for (int x = 0; x < (int)e.optCount; ++x)
        {
            for (int y = x + 1; y < (int)e.optCount; ++y)
            {
                if (e.opt[x].itemIdx == e.opt[y].itemIdx && e.opt[x].a == e.opt[y].a)
                    return LineFunc(0x905e, kPretty, 0, 0);
            }
        }

        // -- 私人商店娃娃（isDollCreated）拦截：product_no==0x1caad --
        private_store::CPrivateStore* seller =
            private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(user);
        if (seller != 0 && seller->isDollCreated() && (int)e.productNo == 0x1caad)   // B3
        {
            GlobalData::s_pCeraShop->ProcessError(user, 0xe7, 0, e.productNo, 1, 1, (char)0);
            return 0;
        }

        // -- 计费 trace（含 product_no）--
        cMyTrace trP(kPretty, 0x9070, 0);
        trP("NEXON BILLING : BuyCeraShopItem m_id(%s), charac_no(%d), charac_name(%s), buytype(%d), product_no(%d)",
            idStr, user->getCurCharacNo(), user->getCurCharacName(), (int)buytype, e.productNo);
        if (n == 0) firstProduct = e.productNo;
    }

    // ---- ServiceRestrictManager::isRestricted(user, (Enum)1, 0x11) ----
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(user, (RestrictCategory::Enum)1, 0x11))
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0xd1, 0, firstProduct, 1, 1, (char)buytype);
        return 0;
    }

    // ---- CSecu_ProtectionField::Check(user, 0x38) ----
    int secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x38);
    if (secuErr != 0)
    {
        int cashErr = GlobalData::s_pSecuProtectionField->GetCashShopErr(secuErr);   // B4
        GlobalData::s_pCeraShop->ProcessError(user, (unsigned short)cashErr, 0, firstProduct, 1, 1, (char)0);
        return 0;
    }

    // ---- useCountDownCoinInFreeCoinDungeon（内联）----
    if (useCountDownCoinInFreeCoinDungeon_inline(user, itemCount, firstProduct))
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x13, 0, 0, 1, 1, (char)0);
        return 0;
    }

    // ---- 结算：buytype==1（邮件赠送）或普通购买 ----
    // 构造 MSG_BUY_CERASHOP_ITEM（0x100 opque）：条目字节区按报告偏移填入。
    MSG_BUY_CERASHOP_ITEM buyMsg;
    std::memset(&buyMsg, 0, sizeof(buyMsg));
    char* body = (char*)&buyMsg;
    body[0x36] = itemCount;
    for (int n = 0; n < (int)itemCount; ++n)
    {
        *(int*)(body + 0x39 + n * 0x1cf) = (int)entries[n].productNo;
    }

    if (buytype == 1)
    {
        // MSG_CHECK_MAIL_RECEIVER::set<ENUM_MAILBOX_CHECKTYPE_1, MSG_BUY_CERASHOP_ITEM>
        // 权威模板（CGameManager.h，ORIG _ZN23MSG_CHECK_MAIL_RECEIVER3setILNS_22ENUM_MAILBOX_CHECKTYPEE1E21MSG_BUY_CERASHOP_ITEMEEvRKT0_）：
        // *(int*)this=TYPE; memcpy((char*)this+4,&msg,sizeof(MSG_BUY_CERASHOP_ITEM)=0x3f84)。
        // MSG_CHECK_MAIL_RECEIVER 为全局作用域类型（ORIG mangled _ZN23MSG_CHECK_MAIL_RECEIVER... 无 WongWork）。
        // WongWork::CMailBoxHelper::ReqDBCheckReceiver 确在 WongWork 命名空间。
        MSG_CHECK_MAIL_RECEIVER mailCk;
        mailCk.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_1>(buyMsg);
        int rc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(user, recvName, &mailCk);
        if (rc != 0)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", kPretty, 0x90c2,
                                  "[ITEM_GIFT] Fail charac_no[%u] error_code[%d]",
                                  user->get_charac_no(-1), rc);
            user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x43, (unsigned short)(rc & 0xffff));
        }
        return 0;
    }

    // ---- 普通购买：ItemVendingMachine::BuyItem（B2）----
    if (!GlobalData::s_pItemVendingMachine->BuyItem(user, buyMsg))
        return LineFunc(0x90ce, kPretty, 0, 0);
    return 0;
}

int Dispatcher_BuyCeraShopItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { (void)user; (void)msg; (void)param; return 0; }
int Dispatcher_BuyCeraShopItem::read(PacketBuf& packet, MSG_BASE& msg) { (void)packet; (void)msg; return 0; }