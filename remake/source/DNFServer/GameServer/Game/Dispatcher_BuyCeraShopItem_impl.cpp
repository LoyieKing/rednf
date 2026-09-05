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
#include "PacketGuard.h"        // PacketGuard（ORIG -0x80 槽 ctor/dtor + eh cleanup）
#include "CPrivateStoreMgr.h"   // private_store::CPrivateStoreMgr/CPrivateStore 权威声明
                               //（IsBusyPrivateStore 0x085cad5e / FindStoreSeller 0x085ca358 /
                               // isDollCreated 0x082345e4；TU 局部最小声明已删除）
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
// B1 WongWork::CCeraShop::ProcessError     _ZN8WongWork9CCeraShop12ProcessErrorEP5CUsertijiic @0x8323978  (无 producer)
//   B2 ItemVendingMachine::BuyItem           _ZN18ItemVendingMachine7BuyItemEP5CUserRK21MSG_BUY_CERASHOP_ITEM @0x854c4d0  (无 producer)
//   B3 private_store::CPrivateStore::isDollCreated _ZN13private_store13CPrivateStore13isDollCreatedEv @0x82345e4  (声明改由 CPrivateStoreMgr.h 权威头提供)
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
// CSecu_ProtectionField 权威完整定义见 CInventory.h:400 区（本地最小镜像已删，避免 ODR）。

// LineFunc / NumberToString（ORIG 自由函数，权威头缺失但符号真实存在）。
extern int LineFunc(int, const char*, int, unsigned int);
extern char* NumberToString(unsigned int value, int radix);

// CEquipItem::getAvatarOptionSize 权威声明见 CEquipItem.h（ORIG W 0x81803bc）。
// 本 TU include "CEquipItem.h"，不做局部重定义。

// ============================================================================
// dispatch_sig 条目访问视图：SigTypes_A.h MSG_BUY_CERASHOP_ITEM（0x3f84）之
//   MSG_CERA_SHOP_BUY_SCALARS（+0x0d buytype / +0x0f buyFlag / +0x10..0x2d 收件人名 /
//   +0x36 itemCount）与 MSG_CERA_SHOP_BUY_ENTRIES（+0x30 起、步长 0x1cf 条目，
//   字段 +0x07/+0x08/+0x09/+0x0d/+0x0e stride5/+0xbd/+0xbe stride8）。
// 原「本地 stBuyCeraEntry[0x24]（步长 0x250）+ memset + 尾部重组 buyMsg」方案与
// ORIG 帧形不符（ORIG 全程原地读入唯一消息对象、无 memset、尾块直接传该对象），
// 已删除；证据 docs/class_func_reports/Dispatcher_BuyCeraShopItem/dispatch_sig.md
// ORIG 汇编 0x081fd396..0x081fe4b7。
//
// 【§6.2 验收标注 2026-09-05】o=1002 n=1011（gap 116→|9|）；调用点差集 96/96 归零
// （仅 cMyTrace/PacketGuard C1↔C2、D1↔D2 ORIG 同址别名形态差）；msg 基址 -0x4008、
// 槽 msg+0x0d/0x0f/0x36/0x39 绝对位与 ORIG 全同，帧 0x7fb0↔0x7fbc（push %edi 差）。
// 残余 9 指令与行级 hunk 为 N82 槽号/寄存器分配 + eh 清理复用形态 ⇒ 判语义完成、
// byte 层 N 封顶。
// ============================================================================

int Dispatcher_BuyCeraShopItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    static const char kPretty[] = "virtual int Dispatcher_BuyCeraShopItem::dispatch_sig(CUser*, PacketBuf&)";
    // ORIG 帧（sub $0x7fbc）：msg = -0x4008（MSG_BUY_CERASHOP_ITEM 0x3f84，
    // **无 memset**，全部 get_* 原地写入；未初始化读忠实保留——ORIG 各
    // ProcessError 第 4 参直接读 msg+0x39（entry0 productNo 槽，可能未写入））；
    // mailReq = -0x7f90（MSG_CHECK_MAIL_RECEIVER 0x3f88）；recvLen = -0x84(int)；
    // 循环节变量 i=-0x40 j=-0x2c k=-0x28 x=-0x24 y=-0x20；
    // goods=-0x38 item=-0x34 rand=-0x30 seller=-0x3c secuErr=-0x44 mailRc=-0x1c。
    // PacketGuard pg = -0x80（state 门之后构造，其后全部代码在 try 区，
    // catch handler 0x081fe484 → dtor + _Unwind_Resume）。
    MSG_BUY_CERASHOP_ITEM msg;
    MSG_CHECK_MAIL_RECEIVER mailReq;
    int  recvLen;                    // -0x84：仅 buyFlag==1 分支写入/读取
    int  i;                          // -0x40
    int  j;                          // -0x2c
    int  k;                          // -0x28
    int  x;                          // -0x24
    int  y;                          // -0x20
    int  randIdx;                    // -0x30
    CCeraShopGoods* goods;           // -0x38
    CItem* item;                     // -0x34
    private_store::CPrivateStore* seller;  // -0x3c
    int  secuErr;                    // -0x44
    int  mailRc;                     // -0x1c

    // ---- acc_id trace（NumberToString + cMyTrace）----
    char* idStr = NumberToString(user->get_acc_id(), 0);
    cMyTrace tr0(kPretty, 0x8f66, 0);
    tr0("Dispatcher_BuyCeraShopItem(m_id(%s))", idStr);

    // ---- user/trade/state 校验 ----
    if (user == 0 || user->CheckInTrade()) return 0;
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return LineFunc(0x8f7b, kPretty, 0, 0);

    PacketGuard pg;                  // ORIG 0x081fd482（-0x80）

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

    // ---- get_short → msg+0x0d（buytype，ORIG get_short(short&) 直写消息槽）----
    if (!packet.get_short((short&)msg.m_scalars.m_buytype))
        return LineFunc(0x8fa9, kPretty, 0, 0);
    if ((short)msg.m_scalars.m_buytype < 0 || (short)msg.m_scalars.m_buytype > 1)
        return LineFunc(0x8fad, kPretty, 0, 0);

    // ---- get_byte → msg+0x0f（buyFlag）----
    if (!packet.get_byte(msg.m_scalars.m_buyFlag))
        return LineFunc(0x8fb7, kPretty, 0, 0);

    // ---- buyFlag==1 → get_int(recvLen)+get_str(msg+0x10, 0x1e, recvLen) ----
    // ORIG get_str 目标 = msg+0x10（收件人名区，即 m_pad10 的 0x1e 字节），非独立缓冲。
    if (msg.m_scalars.m_buyFlag == 1)
    {
        if (!packet.get_int(recvLen))
            return LineFunc(0x8fbb, kPretty, 0, 0);
        if (!packet.get_str(&msg.m_scalars.m_pad10[0], 0x1e, recvLen))
            return LineFunc(0x8fbc, kPretty, 0, 0);
    }

    // ---- buyFlag==1 && buytype==1 → 拒绝 ----
    if (msg.m_scalars.m_buyFlag == 1 && (short)msg.m_scalars.m_buytype == 1)
        return LineFunc(0x8fc2, kPretty, 0, 0);

    // ---- 整合 PvP base channel / 渠道类型拦截 ----
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        // ORIG 0x081fd749 cmp $0xf; sete; test; je：物化 (type == 0xf) 后取反。
        if (!(G_GameWorld()->GetChannelType() == 0xf))
        {
            GlobalData::s_pCeraShop->ProcessError(user, 0x3c, 0, 0, 1, 1, (char)0);
            return 0;
        }
        if (msg.m_scalars.m_buyFlag == 1)
        {
            GlobalData::s_pCeraShop->ProcessError(user, 0x3c, 0, 0, 1, 1, (char)0);
            return 0;
        }
    }

    // ---- 计费 trace（NEXON BILLING；ORIG 0x081fd7fe 先缓存 buyFlag→-0x7fa4，
    //      右→左求值：name→edi、no→esi、acc_id→NumberToString→ebx）----
    unsigned int buyType = (unsigned int)msg.m_scalars.m_buyFlag;   // -0x7fa4
    cMyTrace trB(kPretty, 0x9004, 0);
    trB("NEXON BILLING : BuyCeraShopItem m_id(%s), charac_no(%d), charac_name(%s), buytype(%d)",
        NumberToString(user->get_acc_id(), 0), user->getCurCharacNo(),
        user->getCurCharacName(), buyType);

    // ---- get_byte → msg+0x36（itemCount）：bad = !(读成功 && !=0 && <=0x23) ----
    // ORIG 0x081fd8a4 起物化判定；失败走 ProcessError(1,0,msg.entry0.productNo,1,1,0)
    // （entry0 此时未写入 → 忠实读未初始化 msg+0x39）。
    if (!(packet.get_byte(msg.m_scalars.m_itemCount) == 1 &&
          msg.m_scalars.m_itemCount != 0 &&
          msg.m_scalars.m_itemCount <= 0x23))
    {
        GlobalData::s_pCeraShop->ProcessError(
            user, 1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
        return 0;
    }

    // ---- 逐条解析（条目基址 msg+0x30、步长 0x1cf；ORIG 每字段现算地址）----
    for (i = 0; i < (int)msg.m_scalars.m_itemCount; ++i)
    {
        if (!packet.get_byte(msg.m_entrySection.m_arr[i].m_avatarType0))
            return LineFunc(0x9017, kPretty, 0, 0);
        // +0x08：ORIG 走 get_byte(char&) 重载（0x0858cf22）。
        if (!packet.get_byte((char&)msg.m_entrySection.m_arr[i].m_avatarType1))
            return LineFunc(0x9018, kPretty, 0, 0);
        // +0x09：ORIG 走 get_int(unsigned int&) 重载（0x0858d240）。
        if (!packet.get_int((unsigned int&)msg.m_entrySection.m_arr[i].m_productNo))
            return LineFunc(0x9019, kPretty, 0, 0);
        // +0x0d subCount：失败分支 ORIG **不是** LineFunc，而是 ProcessError(1,...)
        if (!packet.get_byte(msg.m_entrySection.m_arr[i].m_subCount))
        {
            GlobalData::s_pCeraShop->ProcessError(
                user, 1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
            return 0;
        }
        if (msg.m_entrySection.m_arr[i].m_subCount > 0x23)
        {
            GlobalData::s_pCeraShop->ProcessError(
                user, 1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
            return 0;
        }

        // -- 无子项且 avatarType0==0xff 时做头像选项随机（ORIG 0x081fdb37）--
        if (msg.m_entrySection.m_arr[i].m_avatarType0 == 0xff &&
            msg.m_entrySection.m_arr[i].m_subCount == 0)
        {
            goods = (CCeraShopGoods*)G_CDataManager()->FindGoods(
                msg.m_entrySection.m_arr[i].m_productNo);
            if (goods == 0)
                return LineFunc(0x9032, kPretty, 0, 0);
            item = G_CDataManager()->find_item(goods->GetItemIdx());
            if (item != 0 && item->IsAvatarItem())
            {
                randIdx = get_rand_int(((CEquipItem*)item)->getAvatarOptionSize());   // B7
                if (randIdx <= 100)   // ORIG cmp $0x63; jg skip → ≤100 才写
                    msg.m_entrySection.m_arr[i].m_avatarType0 = (unsigned char)randIdx;
            }
        }

        // -- 子项表 (stride5，基址 entry+0x0e) --
        for (j = 0; j < (int)msg.m_entrySection.m_arr[i].m_subCount; ++j)
        {
            // ORIG get_int(unsigned long&)（0x0858d27e）。
            if (!packet.get_int((unsigned long&)msg.m_entrySection.m_arr[i].m_subEntries[j].m_itemIdx))
                return LineFunc(0x9045, kPretty, 0, 0);
            if (!packet.get_byte(msg.m_entrySection.m_arr[i].m_subEntries[j].m_value))
                return LineFunc(0x9046, kPretty, 0, 0);
        }

        // -- 选项数 optCount（entry+0xbd；ORIG 0x081fdd6c 物化：失败或 >=0x24
        //    统一走 ProcessError(1,...)，失败路径并非 LineFunc）--
        if (!(packet.get_byte(msg.m_entrySection.m_arr[i].m_optCount) == 1 &&
              msg.m_entrySection.m_arr[i].m_optCount <= 0x23))
        {
            GlobalData::s_pCeraShop->ProcessError(
                user, 1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
            return 0;
        }

        // -- 选项表 (stride8，基址 entry+0xbe) --
        for (k = 0; k < (int)msg.m_entrySection.m_arr[i].m_optCount; ++k)
        {
            if (!packet.get_item_idx(msg.m_entrySection.m_arr[i].m_optEntries[k].m_itemUid))
                return LineFunc(0x9053, kPretty, 0, 0);
            if (!packet.get_short(msg.m_entrySection.m_arr[i].m_optEntries[k].m_a))
                return LineFunc(0x9054, kPretty, 0, 0);
            if (!packet.get_short(msg.m_entrySection.m_arr[i].m_optEntries[k].m_b))
                return LineFunc(0x9055, kPretty, 0, 0);
        }

        // -- 重复选项校验（同 itemUid(4B) + 同 a(2B) 视为重复）--
        for (x = 0; x < (int)msg.m_entrySection.m_arr[i].m_optCount; ++x)
        {
            for (y = x + 1; y < (int)msg.m_entrySection.m_arr[i].m_optCount; ++y)
            {
                if (msg.m_entrySection.m_arr[i].m_optEntries[x].m_itemUid ==
                        msg.m_entrySection.m_arr[i].m_optEntries[y].m_itemUid &&
                    msg.m_entrySection.m_arr[i].m_optEntries[x].m_a ==
                        msg.m_entrySection.m_arr[i].m_optEntries[y].m_a)
                    return LineFunc(0x905e, kPretty, 0, 0);
            }
        }

        // -- 私人商店娃娃（isDollCreated）拦截：product_no==0x1caad --
        seller = private_store::GetInstancePrivateStoreMgr()->FindStoreSeller(user);
        if (seller != 0 && seller->isDollCreated() &&
            msg.m_entrySection.m_arr[i].m_productNo == 0x1caad)
        {
            // ORIG 0x081fe0d9：第 4 参取 -0x3fcf（**entry0** productNo，非 entry_i）。
            GlobalData::s_pCeraShop->ProcessError(
                user, 0xe7, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
            return 0;
        }

        // -- 计费 trace（含 product_no；ORIG 0x081fe13d product→-0x7fa0、
        //    buyFlag→-0x7f9c 两独立临时槽）--
        unsigned int productNo = (unsigned int)msg.m_entrySection.m_arr[i].m_productNo;
        unsigned int buyType2  = (unsigned int)msg.m_scalars.m_buyFlag;
        cMyTrace trP(kPretty, 0x9070, 0);
        trP("NEXON BILLING : BuyCeraShopItem m_id(%s), charac_no(%d), charac_name(%s), buytype(%d), product_no(%d)",
            NumberToString(user->get_acc_id(), 0), user->getCurCharacNo(),
            user->getCurCharacName(), buyType2, productNo);
    }

    // ---- ServiceRestrictManager::isRestricted(user, (Enum)1, 0x11) ----
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(user, (RestrictCategory::Enum)1, 0x11))
    {
        // ORIG 0x081fe227：第 4 参 entry0 productNo、末参 movsbl(buyFlag)。
        GlobalData::s_pCeraShop->ProcessError(
            user, 0xd1, 0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo,
            1, 1, (char)msg.m_scalars.m_buyFlag);
        return 0;
    }

    // ---- CSecu_ProtectionField::Check(user, 0x38) ----
    secuErr = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x38);
    if (secuErr != 0)
    {
        // ORIG 0x081fe2c4：GetCashShopErr 结果 movzwl 直传错误码槽，无具名局部。
        GlobalData::s_pCeraShop->ProcessError(
            user,
            (unsigned short)GlobalData::s_pSecuProtectionField->GetCashShopErr(secuErr),   // B4
            0, (unsigned int)msg.m_entrySection.m_arr[0].m_productNo, 1, 1, (char)0);
        return 0;
    }

    // ---- 结算：先倒计时币副本检查，再 buyFlag==1（邮件赠送）/普通购买 ----
    // ORIG 直接传已读满的唯一消息对象（先前版本 memset+重组 buyMsg 属语义缺口）。
    if (useCountDownCoinInFreeCoinDungeon(user, msg))
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x13, 0, 0, 1, 1, (char)0);
        return 0;
    }

    if (msg.m_scalars.m_buyFlag == 1)
    {
        // MSG_CHECK_MAIL_RECEIVER::set<ENUM_MAILBOX_CHECKTYPE_1, MSG_BUY_CERASHOP_ITEM>
        // 权威模板（CGameManager.h，ORIG _ZN23MSG_CHECK_MAIL_RECEIVER3setILNS_22ENUM_MAILBOX_CHECKTYPEE1E21MSG_BUY_CERASHOP_ITEMEEvRKT0_）：
        // *(int*)this=TYPE; memcpy((char*)this+4,&msg,sizeof(MSG_BUY_CERASHOP_ITEM)=0x3f84)。
        // MSG_CHECK_MAIL_RECEIVER 为全局作用域类型（ORIG mangled _ZN23MSG_CHECK_MAIL_RECEIVER... 无 WongWork）。
        // WongWork::CMailBoxHelper::ReqDBCheckReceiver 确在 WongWork 命名空间。
        // ORIG 0x081fe388：set 目标 = 栈对象（-0x7f90）；第 2 参 = msg+0x10 收件人名区。
        mailReq.set<MSG_CHECK_MAIL_RECEIVER::ENUM_MAILBOX_CHECKTYPE_1>(msg);
        mailRc = WongWork::CMailBoxHelper::ReqDBCheckReceiver(
            user, &msg.m_scalars.m_pad10[0], &mailReq);
        if (mailRc != 0)
        {
            LogManager::logFormat(1, "PacketDispatcher_Impl_1.cpp", kPretty, 0x90c2,
                                  "[ITEM_GIFT] Fail charac_no[%u] error_code[%d]",
                                  user->get_charac_no(-1), mailRc);
            user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x43, (unsigned short)(mailRc & 0xffff));
        }
        return 0;
    }

    // ---- 普通购买：ItemVendingMachine::BuyItem（B2）----
    if (!GlobalData::s_pItemVendingMachine->BuyItem(user, msg))
        return LineFunc(0x90ce, kPretty, 0, 0);
    return 0;
}

int Dispatcher_BuyCeraShopItem::process(CUser* user, MSG_BASE& msg, ParamBase& param) { (void)user; (void)msg; (void)param; return 0; }
int Dispatcher_BuyCeraShopItem::read(PacketBuf& packet, MSG_BASE& msg) { (void)packet; (void)msg; return 0; }