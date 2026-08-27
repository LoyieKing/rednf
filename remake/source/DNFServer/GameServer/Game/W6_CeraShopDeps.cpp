// ============================================================================
// df_game_r 还原 —— WongWork::CCeraShop 12 个 ORIG-only 符号批次
//   （Wave6 / W6_CeraShopDeps 独占 TU；报告目录 docs/class_func_reports/WongWork__CCeraShop/）
// 符号清单（nm -S 核实于 df_game_r；所有反汇编/反编译依据逐一读取）：
//   CallSpecialItemHandler(int,STSpecailItem_Param const&,STSpecailItem_Result&)
//                                                       0x08325854 (0x2a)
//   LogError(stErrorLogInput_t const&) const            0x0832033a (0x13f)
//   _ProcessIPGError(CUser*,SIPGData*,SIPGData*,hijiic) 0x083237f6 (0x182)
//   _processCoinGift(CUser*,ushort,uint,int)            0x0832619c (0x195)
//   _processCoinGift(CUser*,ushort,uint,int,int)        0x08326332 (0x1a4)
//   _processItemGift(CUser*,Inven_Item,uint,int)        0x083264d6 (0x112)
//   _processAvatarGift(CUser*,i,i,c,c,PKc,j,i)          0x08326036 (0x166)
//   _processCreatureGift(CUser*,Inven_Item,uint,int,int,int)
//                                                       0x083265e8 (0x151)
//   ProcessBuyRequest(CUser*,MSG_BUY_CERASHOP_ITEM const&)
//                                                       0x0832047a (0x18b3)
//   ProcessIPGUserData(CUser*,SIPGData*,SIPGData*,Stream*)
//                                                       0x8321d2e (0x1ac7)
//   _CheckValidCount(CCeraShopGoods const**)            0x08323af4 (0x435)
//       → 定义位于 CCeraShop_Special.cpp；本批次修正其二级指针签名以匹配
//         ORIG mangled EPPK14CCeraShopGoods（原 PKPK… 不产 ORIG 符号）。
//   Is2010ChuseokPacket(unsigned int)                   0x083280dc W
//       → 同上；参数类型 ulong→uint（ORIG mangled …Ej）。
// ============================================================================

#include "CCeraShop.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "SpecialItemHandler.h"
#include "CStreamGuard.h"
#include "Stream.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "InterfacePacketBuf.h"
#include "PacketTypes.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "CItemLimitEdition.h"
#include "CEquipItem.h"
#include "CUser.h"
#include "DB_UpdateGiftCount.h"     // DB_UpdateGiftCount / DB_UpdateBreakAwayAccruedCera 权威声明
#include "CInventory.h"
#include "CMailBoxHelper.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"   // CEventManager / CEventBase 权威声明
#include "CEventMgr.h"
#include "CGenUniqueNo.h"
#include "LogManager.h"
#include "CCirculationStatistic.h"
#include "RDARScriptStringManager.h"
#include "TaiwanCash.h"          // WongWork::IPG::CIPGHelper 权威声明
#include "CSystemTime.h"
#include "CEnvironment.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <algorithm>

// ---------------------------------------------------------------------------
// 仅调用视图（与 LinkClosureB_Misc.cpp / WongWorkEvents_A.cpp 既定先例同型；
// 真实实现由其它 TU 提供，本 TU 不定义任何函数体）。
// ---------------------------------------------------------------------------
namespace break_away_prevent
{
class CBreakAwayPreventSystem
{
public:
    void updateUsedCera(unsigned int accId, unsigned int cera);   // ORIG T 0x0831ce5e
};
}  // namespace break_away_prevent

// ORIG T @0x0831ce5e [updateUsedCera.md]：仅转发 DB_UpdateBreakAwayAccruedCera::
// makeRequest；不访问任何成员字段，调用视图本地布局即 ABI 安全。
void break_away_prevent::CBreakAwayPreventSystem::updateUsedCera(unsigned int accId,
                                                                 unsigned int cera)
{
    DB_UpdateBreakAwayAccruedCera::makeRequest(accId, cera);
}

// DB_UpdateGiftCount/DB_UpdateBreakAwayAccruedCera 权威声明（含 makeRequest/dispatch
// 与生命周期实现）见 DB_UpdateGiftCount.h（本批次新建 TU，已登记 CMake note）。

// WongWork::CWebEvent 权威声明现位于 GlobalData.h（ReqSaveEventItem ORIG T 0x086c1910）。

// 全局类；getBonusRate T 符号由现有 CMake TU 提供（此处仅调用视图）。
class CCeraShopBonusItemEvent : public CEventBase
{
public:
    int getBonusRate();                                          // ORIG 0x0830a8e4
};

namespace WongWork
{

namespace
{
const int kBuyEntryStride = 0x1cf;   // MSG 条目区步长（ProcessBuyRequest imul 实证）

inline int AtoiAt(const WongWork::IPG::SIPGData* data, int off)
{
    // SIPGData 当前为 char 镜像（TaiwanCash.h）；IPG 协议文本槽按偏移 atoi，
    // 语义字段化由后续 IPG 批次收口【推断】。
    return atoi(reinterpret_cast<const char*>(data) + off);
}

inline int ReadIntAt(const WongWork::IPG::SIPGData* data, int off)
{
    return *reinterpret_cast<const int*>(reinterpret_cast<const char*>(data) + off);
}
inline char* BufAt(WongWork::IPG::SIPGData* data, int off)
{
    return reinterpret_cast<char*>(data) + off;
}

// gift 族公共形态：buf 清零后 sprintf(findString(4,"game_server_msg_02"), 角色名)。
void FormatGiftMessage(char (&buf)[256], CUser* user)
{
    std::memset(buf, 0, sizeof(buf));
    const char* name = user->getCurCharacName();
    const char* fmt = g_scriptStringManager_.findString(4, "game_server_msg_02", 0);
    sprintf(buf, fmt, name);
}

inline cUserHistoryLog* GiftHistoryLog(CUser* user)
{
    return reinterpret_cast<cUserHistoryLog*>(reinterpret_cast<char*>(user) + 0x79700);
}
}  // namespace

// ---------------------------------------------------------------------------
// CallSpecialItemHandler —— ORIG T 0x08325854 (0x2a)
// 薄转发：m_handler->execute(type,param,result)。
// ---------------------------------------------------------------------------
void CCeraShop::CallSpecialItemHandler(int type, const STSpecailItem_Param& param,
                                       STSpecailItem_Result& result)
{
    m_handler->execute(type, param, result);
}

// ---------------------------------------------------------------------------
// LogError —— ORIG T 0x0832033a (0x13f)
// StreamPool 取流 → Guard<<0x32e<<input[0] → 组装 0x1c 字节 SIG 包体 →
// put_binary → MsgQueueMgr::put(DB_Q)。SIG ctor 0x08328110 清零 +0..+0x11，
// 各槽来源见 BuildCerashopErrorLogBody 对应注释【字段语义推断】。
// ---------------------------------------------------------------------------
void CCeraShop::LogError(const stErrorLogInput_t& input) const
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x7a);
    CStreamGuard guard(stream, true);

    guard << 0x32e;                       // operator<<(0x32e)
    guard << input.m_field0;              // operator<<(input[0])

    unsigned char body[0x1c];
    std::memset(body, 0, sizeof(body));
    *reinterpret_cast<int*>(&body[0x00]) = input.m_field0;
    *reinterpret_cast<int*>(&body[0x04]) = input.m_field4;
    body[0x08] = static_cast<unsigned char>(
        *reinterpret_cast<const int*>(
            reinterpret_cast<const char*>(G_CEnvironment()) + 0x378));
    *reinterpret_cast<int*>(&body[0x0c]) = G_CEnvironment()->get_channel_no();
    *reinterpret_cast<unsigned short*>(&body[0x10]) = input.m_field10;
    *reinterpret_cast<int*>(&body[0x14]) = input.m_field8;
    *reinterpret_cast<int*>(&body[0x18]) = input.m_fieldc;

    guard.put_binary(body, 0x1c);         // try 域（catch 后 resume，等价 RAII）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---------------------------------------------------------------------------
// _ProcessIPGError —— ORIG T 0x083237f6 (0x182)
// resData->status(+0x1c)==2 且 atoi(resData+0x173)==1 时走 helper 取消链；
// 否则 logFormat "IPG ERROR (Goods No: %d)(%s)"；末尾 user!=0 转 ProcessError。
// ---------------------------------------------------------------------------
void CCeraShop::_ProcessIPGError(CUser* user, IPG::SIPGData* reqData,
                                 IPG::SIPGData* resData, unsigned char errCodeA,
                                 int errCodeB, unsigned int productNo, int subCode,
                                 int code2, char flag)
{
    if (ReadIntAt(resData, 0x1c) == 2)
    {
        if (atoi(BufAt(resData, 0x173)) == 1)
        {
            if (user == 0)
            {
                GlobalData::s_pIPGHelper->IPGOutputCancel(reqData, resData);
            }
            else
            {
                GlobalData::s_pIPGHelper->IPGOutputCancel(
                    user,
                    BufAt(resData, 0x159),
                    BufAt(reqData, 0x69),
                    BufAt(reqData, 0x71),
                    BufAt(reqData, 0x7d),
                    BufAt(reqData, 0x91),
                    BufAt(reqData, 0xf5),
                    static_cast<void*>(0),
                    static_cast<Taiwan::BillTransaction*>(0));
            }
        }
        else
        {
            LogManager::logFormat(
                1, "CeraShop.cpp",
                "void WongWork::CCeraShop::_ProcessIPGError(CUser*, WongWork::IPG::SIPGData*, "
                "WongWork::IPG::SIPGData*, unsigned char, int, unsigned int, int, int, char)",
                0xb47, "IPG ERROR (Goods No: %d)(%s)",
                static_cast<int>(productNo), BufAt(resData, 0x177));
        }
    }

    if (user != 0)
    {
        ProcessError(user, static_cast<unsigned short>(errCodeA), errCodeB,
                     productNo, subCode, code2, flag);
    }
}

// ---------------------------------------------------------------------------
// _processCoinGift —— ORIG T 0x0832619c (0x195)：四参重载
// 代币物品 idx=1、count=currencyType 构造 Inven_Item → 赠送邮件 + 统计日志。
// ---------------------------------------------------------------------------
void CCeraShop::_processCoinGift(CUser* user, unsigned short currencyType,
                                 unsigned int buyIdx, int cost)
{
    char msgBuf[256];

    Inven_Item item;
    item.m_field1 = 2;                                // ITEM_TYPE_2（movb $2,-0x44）
    item.m_addInfo = 1;                               // movl $1 → idx=1
    item.m_addInfo2 = static_cast<int>(currencyType); // movzwl → set_add_info2
    item.m_fieldb = 2;                                // endurance=2
    item.ResetItemAttr();

    FormatGiftMessage(msgBuf, user);

    GiftHistoryLog(user)->SendMailForCeraShopGift(
        static_cast<unsigned int>(cost), '\x01', 1u,
        static_cast<unsigned int>(currencyType));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(cost),
                                       1L, static_cast<unsigned int>(currencyType));

    CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                             static_cast<unsigned int>(cost), true,
                                             msgBuf, static_cast<int>(strlen(msgBuf)));
}

// ---------------------------------------------------------------------------
// _processCoinGift —— ORIG T 0x08326332 (0x1a4)：五参重载（count 选用赠品种类）
// kindFlag：count==0xec→4、count==0xca→5、否则 0。
// ---------------------------------------------------------------------------
void CCeraShop::_processCoinGift(CUser* user, unsigned short currencyType,
                                 unsigned int buyIdx, int cost, int count)
{
    char msgBuf[256];

    Inven_Item item;
    item.m_addInfo = count;                           // movl param_5 → idx=count
    item.m_field1 = 2;                                // ITEM_TYPE_2
    item.m_addInfo2 = static_cast<int>(currencyType);
    item.m_fieldb = 2;
    item.ResetItemAttr();

    FormatGiftMessage(msgBuf, user);

    char kindFlag = '\0';
    if (count == 0xec)
    {
        kindFlag = '\x04';
    }
    else if (count == 0xca)
    {
        kindFlag = '\x05';
    }

    GiftHistoryLog(user)->SendMailForCeraShopGift(
        static_cast<unsigned int>(cost), kindFlag,
        static_cast<unsigned int>(count),
        static_cast<unsigned int>(currencyType));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(cost),
                                       static_cast<long>(count),
                                       static_cast<unsigned int>(currencyType));

    CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                             static_cast<unsigned int>(cost), true,
                                             msgBuf, static_cast<int>(strlen(msgBuf)));
}

// ---------------------------------------------------------------------------
// _processItemGift —— ORIG T 0x083264d6 (0x112)
// 入参为按值 Inven_Item（调用方已完成拷贝）；giftKind 作邮件参数与统计额。
// ---------------------------------------------------------------------------
void CCeraShop::_processItemGift(CUser* user, Inven_Item item,
                                 unsigned int buyIdx, int giftKind)
{
    char msgBuf[256];
    std::memset(msgBuf, 0, sizeof(msgBuf));           // ORIG rep stos 0x40 dword

    FormatGiftMessage(msgBuf, user);

    GiftHistoryLog(user)->SendMailForCeraShopGift(
        static_cast<unsigned int>(giftKind), '\0',
        static_cast<unsigned int>(item.m_addInfo),
        static_cast<unsigned int>(item.m_addInfo2));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(giftKind),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                             static_cast<unsigned int>(giftKind), true,
                                             msgBuf, static_cast<int>(strlen(msgBuf)));
}

// ---------------------------------------------------------------------------
// _processAvatarGift —— ORIG T 0x08326036 (0x166)
// deadline = now + periodDays*86400（periodDays∈{0,0x16d} 保持 0）→ avatar 赠送邮件。
// ---------------------------------------------------------------------------
void CCeraShop::_processAvatarGift(CUser* user, int slot, int periodDays, char c1,
                                   char c2, char const* agencyName,
                                   unsigned int buyIdx, int cost)
{
    char msgBuf[256];

    int deadline = 0;
    if (periodDays != 0 && periodDays != 0x16d)
    {
        deadline = GlobalData::s_systemTime_.getCurSec() + periodDays * 0x15180;
    }

    std::memset(msgBuf, 0, sizeof(msgBuf));
    FormatGiftMessage(msgBuf, user);

    GiftHistoryLog(user)->SendMailForCeraShopGift(
        static_cast<unsigned int>(cost), '\x02', static_cast<unsigned int>(slot), 1u);
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(cost),
                                       static_cast<long>(slot), 1u);

    CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
        user, cost, slot, deadline, c1, c2, agencyName, msgBuf,
        static_cast<int>(strlen(msgBuf)));
}

// ---------------------------------------------------------------------------
// _processCreatureGift —— ORIG T 0x083265e8 (0x151)
// 宠物赠送：ITEM_TYPE_CREATURE_EQUIP(5) 走 ReqDBSendNewCreatureMail，
// 其余走 ReqDBSendNewMailCashShop。
// ---------------------------------------------------------------------------
void CCeraShop::_processCreatureGift(CUser* user, Inven_Item item, unsigned int buyIdx,
                                     int a4, int a5, int a6)
{
    (void)a5;   // ORIG 栈参存在但本函数未消费（对照报告完整指令清单）
    (void)a6;

    char msgBuf[256];
    std::memset(msgBuf, 0, sizeof(msgBuf));

    FormatGiftMessage(msgBuf, user);

    GiftHistoryLog(user)->SendMailForCeraShopGift(
        static_cast<unsigned int>(a4), '\x03',
        static_cast<unsigned int>(item.m_addInfo),
        static_cast<unsigned int>(item.m_addInfo2));
    user->SendCeraShopGiftStatisticLog(buyIdx, static_cast<unsigned int>(a4),
                                       static_cast<long>(item.m_addInfo),
                                       static_cast<unsigned int>(item.m_addInfo2));

    if (static_cast<unsigned int>(item.m_field1) == 5u)
    {
        CMailBoxHelper::ReqDBSendNewCreatureMail(user, a4, item, msgBuf,
                                                 static_cast<int>(strlen(msgBuf)));
    }
    else
    {
        CMailBoxHelper::ReqDBSendNewMailCashShop(user, item, 0u,
                                                 static_cast<unsigned int>(a4), true,
                                                 msgBuf, static_cast<int>(strlen(msgBuf)));
    }
}

// ---------------------------------------------------------------------------
// ProcessBuyRequest —— ORIG T 0x0832047a (0x18b3)
// MSG 条目逐个 FindGoods → 非 GM 走 sticker/gift 禁售检查 → GM/常规路径查
// GetRelatedItem/PVP 禁购清单/_CheckBuyingCondition(0x8323f86) →
// _CheckValidCount 通过后重置 UsedCera 并逐条折扣计价、额度校验 →
// 组装 stCeraShopIPGRequest_t 从 stream 发往 s_pIPGHelper 的
// IPGOutputSimulation/IPGOutput（cerapoint==0 分支）。控制流、错误码、
// cMyTrace 行号与字符串均照报告汇编/反编译转写。
// 说明【忠实度局限】：0x8323f86 六参校验在 ORIG 以 6 数据槽装载
// （goodsNo / itemIdx:stackCount 组合 long / expiry / byte37 / byte38sext），
// 对齐现存 CCeraShop_Special.cpp 实现体的头声明，末参 e 以 0 补位。
// ---------------------------------------------------------------------------
int CCeraShop::ProcessBuyRequest(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg)
{
    const unsigned char* raw = reinterpret_cast<const unsigned char*>(&msg);

    const unsigned char itemCount = raw[0x36];
    const unsigned char bGiftZone = raw[0xf];

    char* idStr = NumberToString(user->get_acc_id(), 0);

    cMyTrace trace(
        "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
        0x92, 0);
    trace("NEXON BILLING : ProcessBuyRequest(m_id(%s),count(%d))", idStr,
          static_cast<int>(itemCount));

    CCeraShopGoods* goodsList[36];
    for (int i = 0; i < 36; ++i)
    {
        goodsList[i] = 0;
    }
    int relatedItems[36];                 // local_3cc / entryCosts
    for (int i = 0; i < 36; ++i)
    {
        relatedItems[i] = 0;
    }

    CCeraShopGoods* firstGoods =
        reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(
            *reinterpret_cast<const int*>(&raw[0x39])));
    if (firstGoods == 0)
    {
        ProcessError(user, 0x15, 0, *reinterpret_cast<const unsigned int*>(&raw[0x39]),
                     1u, 1u, static_cast<char>(bGiftZone));
        return 1;
    }

    if (WongWork::CEventMgr::IsChildrensDayEventing())
    {
        user->ClearChildrensDayEvent();
    }

    for (int i = 0; i < static_cast<int>(itemCount); ++i)
    {
        const unsigned char* entry = &raw[i * kBuyEntryStride];
        const unsigned int productNo = *reinterpret_cast<const unsigned int*>(&entry[0x39]);

        goodsList[i] = reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(
            *reinterpret_cast<const int*>(&entry[0x39])));
        if (goodsList[i] == 0)
        {
            ProcessError(user, 0x15, -1, productNo,
                         static_cast<unsigned int>(i + 1),
                         static_cast<unsigned int>(itemCount),
                         static_cast<char>(bGiftZone));
            goodsList[i] = 0;

            cMyTrace t(
                "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                0xc3, 0);
            t("[%s][%d]",
              "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
              0xc3);
            continue;
        }

        int groupIdx = goodsList[i]->GetGroupIdx();
        bool proceedMain = true;
        if (user->IsGameMasterMode() != 1)
        {
            // 非 GM：sticker/gift 禁售检查（ORIG xor/test 走向）
            if (goodsList[i]->GetSticker() == 99)
            {
                ProcessError(user, 8, groupIdx, productNo,
                             static_cast<unsigned int>(i + 1),
                             static_cast<unsigned int>(itemCount),
                             static_cast<char>(bGiftZone));
                goodsList[i] = 0;
                cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                           0xde, 0);
                t("[%s][%d]",
                  "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                  0xde);
                continue;
            }
            if (bGiftZone == 1 && goodsList[i]->isGiftDisallowance_() != 0)
            {
                ProcessError(user, 8, groupIdx, productNo,
                             static_cast<unsigned int>(i + 1),
                             static_cast<unsigned int>(itemCount),
                             static_cast<char>(bGiftZone));
                goodsList[i] = 0;
                cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                           0xe8, 0);
                t("[%s][%d]",
                  "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                  0xe8);
                continue;
            }
        }
        if (!proceedMain)
        {
            continue;
        }

        // LAB_083208e9 主路径
        groupIdx = goodsList[i]->GetGroupIdx();
        if (groupIdx != 1)
        {
            relatedItems[i] = reinterpret_cast<int>(goodsList[i]->GetRelatedItem());
            if (relatedItems[i] == 0)
            {
                ProcessError(user, 0x11, groupIdx, productNo,
                             static_cast<unsigned int>(i + 1),
                             static_cast<unsigned int>(itemCount),
                             static_cast<char>(bGiftZone));
                goodsList[i] = 0;
                cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                           0x116, 0);
                t("[%s][%d]",
                  "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                  0x116);
                continue;
            }
        }

        // PVP 渠道禁购清单（LAB_083208..LAB_08320aba 判定树）
        int itemIdx = goodsList[i]->GetItemIdx();
        bool pvpRestricted = false;
        if (itemIdx == 0x334)
        {
            pvpRestricted = true;
        }
        else if (itemIdx < 0x335)
        {
            if (itemIdx < 0xc6)
            {
                if (itemIdx > 0xc3 || itemIdx == 0xc2)
                {
                    pvpRestricted = true;
                }
                else if (itemIdx == 0xc3)
                {
                    user->isAffectedPremium(static_cast<ENUM_PREMIUM_TYPE>(0x21));   // 副作用查询（ORIG 调用）
                }
            }
            else if (itemIdx == 0x2d0 ||
                     (itemIdx > 0x2cf && (itemIdx - 0x2e7) < 2))
            {
                pvpRestricted = true;
            }
        }
        else if (itemIdx < 0x289833)
        {
            if (itemIdx > 0x289830 || itemIdx == 0x2897c2 ||
                itemIdx == 0x289806 || itemIdx == 0x398)
            {
                pvpRestricted = true;
            }
        }
        else if (itemIdx == 0x28beba || itemIdx == 0x28d154)
        {
            pvpRestricted = true;
        }

        if (pvpRestricted)
        {
            char blockPvp = '\0';
            GameWorld* world = G_GameWorld();
            if (world->IsPVPChannel())
            {
                blockPvp = '\x01';
                if (!world->IsPvPSkilTreeChannel() ||
                    user->get_pvp_grade() < 10)
                {
                    // PvP 技能树渠道且 10 段以上豁免
                }
                else
                {
                    blockPvp = '\0';
                }
            }
            if (blockPvp != '\0')
            {
                ProcessError(user, 7, groupIdx, productNo,
                             static_cast<unsigned int>(i + 1),
                             static_cast<unsigned int>(itemCount),
                             static_cast<char>(bGiftZone));
                goodsList[i] = 0;
                cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                           0x16c, 0);
                t("[%s][%d]",
                  "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                  0x16c);
                continue;
            }
        }

        const unsigned char selByteB = entry[0x37];
        const unsigned char selByteA = entry[0x38];
        const unsigned short expiry =
            static_cast<unsigned short>(goodsList[i]->GetExpiryDate());

        bool ok = this->_CheckBuyingCondition(
            user, productNo,
            static_cast<unsigned long>(
                static_cast<unsigned int>(goodsList[i]->GetItemIdx())),
            static_cast<unsigned int>(goodsList[i]->GetStackCount()),
            expiry, selByteB,
            static_cast<unsigned short>(
                static_cast<short>(static_cast<signed char>(selByteA))));
        if (!ok)
        {
            ProcessError(user, 0x4f, groupIdx, productNo,
                         static_cast<unsigned int>(i + 1),
                         static_cast<unsigned int>(itemCount),
                         static_cast<char>(bGiftZone));
            goodsList[i] = 0;
            cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                       0x185, 0);
            t("[%s][%d]",
              "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
              0x185);
        }
    }   // 条目循环（校验段）

    if (!_CheckValidCount(const_cast<const CCeraShopGoods**>(goodsList)))
    {
        ProcessError(user, 1, -1, *reinterpret_cast<const unsigned int*>(&raw[0x39]),
                     1u, 1u, static_cast<char>(raw[0xf]));
        return 1;
    }

    user->resetUsedCera();

    char priceStr[0x24];
    char countStr[8];
    char serialBuf[0x15];
    char itemName[100];
    std::memset(priceStr, 0, sizeof(priceStr));
    std::memset(countStr, 0, sizeof(countStr));
    std::memset(serialBuf, 0, sizeof(serialBuf));
    std::memset(itemName, 0, sizeof(itemName));

    const unsigned int nowSec =
        static_cast<unsigned int>(GlobalData::s_systemTime_.getCurSec());
    unsigned int moneyLow = static_cast<unsigned int>(user->getCurCharacMoney());
    int moneyHigh = static_cast<int>(moneyLow) >> 31;
    unsigned int wpLow = static_cast<unsigned int>(user->GetWinPoint());
    unsigned int wpHigh = 0;
    unsigned int ceraPrice = 0;
    unsigned int goldPrice = 0;
    unsigned int wpPrice = 0;
    unsigned int avatarSelField = 0;

    for (int i = 0; i < static_cast<int>(itemCount); ++i)
    {
        if (goodsList[i] == 0)
        {
            continue;
        }
        const unsigned char* entry = &raw[i * kBuyEntryStride];
        const unsigned int productNo = *reinterpret_cast<const unsigned int*>(&entry[0x39]);

        int groupLocal = goodsList[i]->GetGroupIdx();
        if (groupLocal == 0)
        {
            int groupSub = 0;
            CItem* related = reinterpret_cast<CItem*>(relatedItems[i]);
            if (related != 0 && related->IsAvatarItem())
            {
                std::vector<stAvatarTypeSelect_t>* selects =
                    static_cast<std::vector<stAvatarTypeSelect_t>*>(
                        reinterpret_cast<CEquipItem*>(related)->getAvatarTypeSelect());
                const bool badRange = selects->empty() ||
                                      (static_cast<int>(entry[0x38]) >=
                                       static_cast<int>(selects->size()));
                if (badRange)
                {
                    ProcessError(user, 0x17, groupSub, productNo,
                                 static_cast<unsigned int>(i + 1),
                                 static_cast<unsigned int>(itemCount),
                                 static_cast<char>(bGiftZone));
                    goodsList[i] = 0;
                    cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                               0x253, 0);
                    t("[%s][%d]",
                      "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                      0x253);
                    continue;
                }
                const unsigned char selIdx = entry[0x38];
                if (selIdx < 100)
                {
                    const stAvatarTypeSelect_t& sel = (*selects)[selIdx];
                    wpHigh = static_cast<unsigned int>(sel.m_fieldc);
                    goldPrice = static_cast<unsigned int>(sel.m_field4);
                    wpPrice = static_cast<unsigned int>(sel.m_field8);
                    avatarSelField = static_cast<unsigned int>(sel.m_field0);
                }
                else
                {
                    ProcessError(user, 0x17, groupSub, productNo,
                                 static_cast<unsigned int>(i + 1),
                                 static_cast<unsigned int>(itemCount),
                                 static_cast<char>(bGiftZone));
                    goodsList[i] = 0;
                    cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                               0x25e, 0);
                    t("[%s][%d]",
                      "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                      0x25e);
                    continue;
                }
            }
            else
            {
                ProcessError(user, 0x17, groupSub, productNo,
                             static_cast<unsigned int>(i + 1),
                             static_cast<unsigned int>(itemCount),
                             static_cast<char>(bGiftZone));
                goodsList[i] = 0;
                cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                           0x247, 0);
                t("[%s][%d]",
                  "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                  0x247);
                continue;
            }
        }
        else
        {
            ceraPrice = static_cast<unsigned int>(goodsList[i]->GetCeraPrice());
            goldPrice = static_cast<unsigned int>(goodsList[i]->GetGoldPrice());
            wpPrice = static_cast<unsigned int>(goodsList[i]->GetWinPoint());
        }

        // LAB_083212dc：会员折扣
        WongWork::CUserPremium* premium = user->GetPremiumInfo();
        unsigned short discountRate = premium->getCeraShopDiscountRate();
        if (discountRate != 0)
        {
            ceraPrice -= discountRate * ceraPrice / 100;
            goldPrice -= discountRate * goldPrice / 100;
            wpPrice -= discountRate * wpPrice / 100;
        }

        // 额度校验（错误码：11=cera 不足，10=金币不足，12=WP 不足）
        int errorReason = 0;
        if (user->GetCera() < ceraPrice)
        {
            errorReason = 0xb;
        }
        else
        {
            const CInventory* invR = user->getCurCharacInvenR();
            if (invR->get_money() < static_cast<int>(goldPrice))
            {
                errorReason = 10;
            }
            else if (wpLow < wpPrice)
            {
                errorReason = 0xc;
            }
        }

        // asm：64 位符号视图余额下限判定（ORB 的 max<long long> 语义）
        {
            const long long money64 =
                (static_cast<long long>(moneyHigh) << 32) |
                static_cast<long long>(moneyLow);
            const long long wp64 =
                (static_cast<long long>(wpHigh) << 32) |
                static_cast<long long>(wpLow);
            if (money64 - static_cast<long long>(goldPrice) < 0)
            {
                errorReason = 10;
            }
            if (wp64 - static_cast<long long>(wpPrice) < 0 &&
                wpHigh == 0)
            {
                errorReason = 0xc;
            }
        }

        if (errorReason != 0)
        {
            const char* charName = user->getCurCharacName();
            LogManager::logFormat(
                1, "CeraShop.cpp",
                "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                0x2d6,
                "IPG ERROR [Dispatcher_BuyCeraShopItem::dispatch_sig] UserID[%s] ACera[%d] AGold[%d] AWP[%d]",
                charName, static_cast<int>(ceraPrice),
                static_cast<int>(goldPrice), static_cast<int>(wpPrice));
            LogManager::logFormat(
                1, "CeraShop.cpp",
                "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                0x2d7, " / UCera[%d] UGold[%d] UWP[%d]\n",
                user->GetCera(), user->getCurCharacMoney(), user->GetWinPoint());
            LogManager::logFormat(
                1, "CeraShop.cpp",
                "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                0x2d9, "Error Goods No : %d", productNo);

            ProcessError(user, static_cast<unsigned short>(errorReason),
                         goodsList[i]->GetGroupIdx(), productNo,
                         static_cast<unsigned int>(i + 1),
                         static_cast<unsigned int>(itemCount),
                         static_cast<char>(bGiftZone));
            goodsList[i] = 0;
            cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                       0x2e1, 0);
            t("[%s][%d]",
              "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
              0x2e1);
            continue;
        }

        int featuredIdx = goodsList[i]->GetFeaturedIdx();
        if (user->IsRestrictedGoods(featuredIdx))
        {
            ProcessError(user, 0x14, goodsList[i]->GetGroupIdx(), productNo,
                         static_cast<unsigned int>(i + 1),
                         static_cast<unsigned int>(itemCount),
                         static_cast<char>(bGiftZone));
            goodsList[i] = 0;
            cMyTrace t("bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
                       0x2ed, 0);
            t("[%s][%d]",
              "bool WongWork::CCeraShop::ProcessBuyRequest(CUser*, const MSG_BUY_CERASHOP_ITEM&)",
              0x2ed);
            continue;
        }

        Stream* reqStream = GlobalData::s_stream_pool->Acquire("CeraShop.cpp", 0x37b);
        CStreamGuard guard(reqStream, false);

        stCeraShopIPGRequest_t* req = guard.GetInBuffer<stCeraShopIPGRequest_t>();
        req->m_productNo = productNo;
        req->m_groupIdx = goodsList[i]->GetGroupIdx();
        req->m_itemIdx = goodsList[i]->GetItemIdx();
        req->m_stackCount = static_cast<unsigned short>(goodsList[i]->GetStackCount());
        req->m_ceraPrice = ceraPrice;
        req->m_goldPrice = goldPrice;
        req->m_winPoint = wpPrice;
        *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(req) + 0x2c) = entry[0x37];
        *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(req) + 0x2d) = entry[0x38];
        req->m_avatarField = avatarSelField;
        req->m_featuredIdx = featuredIdx;
        req->m_itemCount = itemCount;
        req->m_seq = i + 1;
        req->m_curSec = nowSec;
        *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(req) + 0x34) = raw[0xf];
        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(req) + 0x40) =
            *reinterpret_cast<const unsigned int*>(&raw[0x2e]);
        *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(req) + 0x38) =
            *reinterpret_cast<const unsigned int*>(&raw[0x32]);
        *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(req) + 0x44) = entry[0x3d];
        for (int s = 0; s < static_cast<int>(entry[0x3d]); ++s)
        {
            *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(req) +
                                             (s + 8) * 8 + 8) =
                *reinterpret_cast<const unsigned int*>(entry + 0x3e + s * 5);
            *(reinterpret_cast<unsigned char*>(req) + (s + 8) * 8 + 0xc) =
                entry[0x42 + s * 5];
        }
        *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(req) + 0x160) =
            entry[0xed];
        for (int o = 0; o < static_cast<int>(entry[0xed]); ++o)
        {
            *reinterpret_cast<unsigned int*>(reinterpret_cast<char*>(req) +
                                             (o + 0x2c) * 8 + 4) =
                *reinterpret_cast<const unsigned int*>(entry + 0x3e + (o + 0x16) * 8);
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(req) +
                                               (o + 0x2c) * 8 + 8) =
                *reinterpret_cast<const unsigned short*>(entry + 0x42 + (o + 0x16) * 8);
            *reinterpret_cast<unsigned short*>(reinterpret_cast<char*>(req) +
                                               (o + 0x2c) * 8 + 10) =
                *reinterpret_cast<const unsigned short*>(entry + 0x44 + (o + 0x16) * 8);
        }

        OS_API::snprintf(priceStr, 0x24, "%d", static_cast<int>(ceraPrice));
        OS_API::snprintf(countStr, 8, "%d",
                         static_cast<int>(*reinterpret_cast<const unsigned int*>(&entry[0x39])));

        if (goodsList[i]->GetItemIdx() == 1)
        {
            OS_API::snprintf(itemName, 0x62,
                             "\xbd\xc3\xb4\xdc\xc7\xd2\xc0\xaf %d\xb0\xb3",
                             goodsList[i]->GetStackCount());   // ORIG DAT_08c25d6b 格式串
        }
        else
        {
            CItem* priced = reinterpret_cast<CItem*>(relatedItems[i]);
            if (priced != 0 && priced->is_stackable() &&
                goodsList[i]->GetStackCount() >= 2)
            {
                OS_API::snprintf(itemName, 0x62,
                                 "\xbd\xc3\xb4\xdc\xc7\xd2\xc0\xaf %s %d\xb0\xb3",
                                 priced->GetItemName(),
                                 goodsList[i]->GetStackCount());   // ORIG DAT_08c25d63
            }
            else if (priced != 0)
            {
                strcpy(itemName, priced->GetItemName());
            }
            else
            {
                itemName[0] = '\0';
            }
        }

        CGenUniqueNo uniqueGen;                 // ORIG 调 GenUniqueNo(&serialBuf)
        char* uniqueNo = uniqueGen.GenUniqueNo(serialBuf);
        if (ceraPrice == 0)
        {
            GlobalData::s_pIPGHelper->IPGOutputSimulation(
                user, countStr, priceStr, uniqueNo, itemName,
                user->getCurCharacName(),
                static_cast<void*>(&guard));
        }
        else
        {
            GlobalData::s_pIPGHelper->IPGOutput(
                user, countStr, priceStr, uniqueNo, itemName,
                user->getCurCharacName(),
                static_cast<void*>(&guard));
        }
        // guard RAII 析构释放流（ORIG leave 前 CStreamGuard::~）
    }

    return 1;
}


// ---------------------------------------------------------------------------
// ProcessIPGUserData —— ORIG T 0x8321d2e (0x1ac7)
// IPG 结果回包处理：resData->status(+0x1c) 4=回填 cera、0=状态包、2=业务结算
// （avatar/coin/item/item-special/creature/additional 六路分派）→ 成功路径
// 扣款与响应包 0x43/0x35/0xe 广播 + ChildrensDay 补发 + Bonus 概率奖励。
// ---------------------------------------------------------------------------
void CCeraShop::ProcessIPGUserData(CUser* user, IPG::SIPGData* reqData,
                                   IPG::SIPGData* resData, Stream* stream)
{
    PacketGuard outPkt;                                   // local_d8

    if (ReadIntAt(resData, 0x1c) == 4)
    {
        if (user != 0)
        {
            const int total =
                AtoiAt(resData, 0x99) + AtoiAt(resData, 0xa5) + AtoiAt(resData, 0x81) +
                AtoiAt(resData, 0x75) + AtoiAt(resData, 0x8d) + AtoiAt(resData, 0x69);
            user->SetCera(static_cast<unsigned int>(total));

            outPkt.put_header( 0, 0x35);
            outPkt.put_byte( 1);
            outPkt.put_int( user->GetCera());
        }
        outPkt.finalize( true);
        user->Send(outPkt);
        return;                                           // LAB_083237dc
    }

    if (ReadIntAt(resData, 0x1c) == 0)
    {
        if (atoi(BufAt(resData, 0x26e)) == 1)
        {
            int a = 0;
            int b = 0;
            *stream >> a;
            *stream >> b;
            if (user != 0)
            {
                GlobalData::s_pIPGHelper->IPGQuery(user);   // ORIG vtable slot11
            }
            return;
        }
        outPkt.put_header( 0, 0x35);
        outPkt.put_byte( 0);
        outPkt.put_int( 0);
    }
    else if (ReadIntAt(resData, 0x1c) == 2)
    {
        if (user->getCurCharacR() == 0)
        {
            return;
        }

        stCeraShopIPGRequest_t* q = stream->GetOutBuffer<stCeraShopIPGRequest_t>();
        const int groupIdx =
            *reinterpret_cast<const int*>(reinterpret_cast<const char*>(q) + 0x04);
        const int seq =
            *reinterpret_cast<const int*>(reinterpret_cast<char*>(q) + 0x24);
        const int itemCountField =
            *reinterpret_cast<const int*>(reinterpret_cast<char*>(q) + 0x20);
        const int featuredIdxReq =
            *reinterpret_cast<const int*>(reinterpret_cast<char*>(q) + 0x28);
        const unsigned int productNo = *reinterpret_cast<const unsigned int*>(q);
        const unsigned long long itemIdx64 =
            *reinterpret_cast<const unsigned long long*>(
                reinterpret_cast<const char*>(q) + 0x08);
        const unsigned short stackCount = *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const char*>(q) + 0x0c);
        const unsigned char avatarSelB =
            *(reinterpret_cast<const unsigned char*>(q) + 0x2c);
        const unsigned char avatarSelA =
            *(reinterpret_cast<const unsigned char*>(q) + 0x2d);
        const unsigned int ceraPriceReq = *reinterpret_cast<const unsigned int*>(
            reinterpret_cast<const char*>(q) + 0x10);
        const unsigned int winPointReq = *reinterpret_cast<const unsigned int*>(
            reinterpret_cast<const char*>(q) + 0x14);
        const int goldSpend =
            *reinterpret_cast<const int*>(reinterpret_cast<char*>(q) + 0x18);
        const long curSec =
            *reinterpret_cast<const long*>(reinterpret_cast<char*>(q) + 0x30);
        std::vector<Inven_Item> resultItems;              // local_ec（RAII）
        const unsigned char buyFlag =
            *(reinterpret_cast<const unsigned char*>(q) + 0x34);
        const int characNoReq =
            *reinterpret_cast<const int*>(reinterpret_cast<char*>(q) + 0x38);
        const unsigned int accIdReq = *reinterpret_cast<const unsigned int*>(
            reinterpret_cast<char*>(q) + 0x40);

        stCeraShopItemParam_t itemParam;                  // local_104（RAII）
        for (int s = 0;
             s < static_cast<int>(*(reinterpret_cast<const unsigned char*>(q) + 0x44));
             ++s)
        {
            const unsigned char* sub =
                reinterpret_cast<const unsigned char*>(q) + (s + 8) * 8 + 8;
            itemParam.m_pairs.push_back(std::make_pair(
                static_cast<int>(*reinterpret_cast<const unsigned int*>(sub)),
                static_cast<int>(sub[4])));
        }
        for (int o = 0;
             o < static_cast<int>(*(reinterpret_cast<const unsigned char*>(q) + 0x160));
             ++o)
        {
            tagPackageItemSelection selEntry;
            std::memcpy(&selEntry,
                        reinterpret_cast<const unsigned char*>(q) +
                            (o + 0x2c) * 8 + 4,
                        sizeof(selEntry));
            itemParam.m_sel.push_back(selEntry);
        }

        int restrictCode = atoi(BufAt(resData, 0x173));   // local_108
        bool success = false;
        const int errArgB = groupIdx;
        if (!(restrictCode == 1 && user != 0 && user->getCurCharacR() != 0))
        {
            _ProcessIPGError(user, reqData, resData,
                             static_cast<unsigned char>(restrictCode == -0xc ? '\x1a' : '\x19'),
                             errArgB, productNo, seq, itemCountField,
                             static_cast<char>(buyFlag));
        }
        else
        {
            int resultState = -1;                         // local_10c → 0
            int slotKind = 2;                             // local_5c
            int insertSlot = -1;                          // local_60
            restrictCode = 0;
            if (groupIdx == 0)
            {
                if (buyFlag == 1)
                {
                    _processAvatarGift(user, static_cast<int>(itemIdx64), featuredIdxReq,
                                       static_cast<char>(avatarSelB),
                                       static_cast<char>(avatarSelA),
                                       BufAt(reqData, 0x7d), accIdReq, characNoReq);
                }
                else
                {
                    CInventory* invW = user->getCurCharacInvenW();
                    insertSlot = invW->AddAvatarItem(
                        static_cast<int>(itemIdx64), featuredIdxReq, false,
                        static_cast<char>(avatarSelB),
                        static_cast<char>(avatarSelA), BufAt(reqData, 0x7d),
                        eAvatarItemAddReason(0), false, 0);
                    if (insertSlot >= 0)
                    {
                        user->SendUpdateItemList(CUser::eSendTarget(1),
                                                 ENUM_ITEMSPACE(1), insertSlot);
                        if (WongWork::CEventMgr::IsChildrensDayEventing())
                        {
                            CCeraShopGoods* goods = reinterpret_cast<CCeraShopGoods*>(
                                G_CDataManager()->FindGoods(groupIdx));
                            CItem* found =
                                G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                            if (goods != 0 && found != 0 &&
                                goods->GetSticker() == 0x16)
                            {
                                const int kindValue = found->GetItemType();
                                user->SetChildrensDayEventValue(kindValue, insertSlot,
                                                                featuredIdxReq);
                            }
                        }
                    }
                    else
                    {
                        const char* errMsg = g_scriptStringManager_.findString(
                            4, "game_server_msg_104", 0);
                        CMailBoxHelper::ReqDBSendNewAvatarMail(
                            user, user->getCurCharacNo(),
                            static_cast<int>(itemIdx64), featuredIdxReq,
                            static_cast<char>(avatarSelB),
                            static_cast<char>(avatarSelA), BufAt(reqData, 0x7d),
                            errMsg, static_cast<int>(strlen(errMsg)));
                    }
                }
            }
            else if (groupIdx == 1)
            {
                if (buyFlag == 1)
                {
                    _processCoinGift(user, stackCount, accIdReq, characNoReq);
                }
                else
                {
                    _processCoin(user, stackCount, true);
                }
            }
            else if (groupIdx == 2 || groupIdx == 7 || groupIdx == 9)
            {
                slotKind = 1;
                CItem* src = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                Inven_Item item;
                if (src != 0)
                {
                    src->make_item(item);     // ORIG vtable slot2（make_item 填 Inven_Item）
                    item.m_addInfo = static_cast<int>(itemIdx64 & 0xffffffffu);
                    item.m_fieldb = stackCount;
                }
                if (buyFlag == 1)
                {
                    _processItemGift(user, item, accIdReq, characNoReq);
                }
                else
                {
                    _processItem(user, item, item.m_addInfo2, restrictCode, curSec,
                                 false, true, resultItems, itemParam);
                }
            }
            else if (groupIdx == 4)
            {
                slotKind = 1;
                CItem* src = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                if (src == 0)
                {
                    success = false;
                }
                else
                {
                    Inven_Item item;
                    src->make_item(item);
                    item.m_addInfo = static_cast<int>(itemIdx64 & 0xffffffffu);
                    item.m_fieldb = stackCount;
                    const bool okSpecial = ProcessSpecialItem(
                        user, static_cast<int>(itemIdx64), featuredIdxReq, restrictCode,
                        curSec, false, resultItems, itemParam, '\0', '\0', false, 1);
                    if (!okSpecial)
                    {
                        CInventory* invW = user->getCurCharacInvenW();
                        insertSlot = invW->insertItemIntoInventory(
                            item, eItemAddReason(0x1e), true, false);
                        if (insertSlot == -1)
                        {
                            restrictCode = 4;
                        }
                        else
                        {
                            user->SendUpdateItemList(CUser::eSendTarget(1),
                                                     ENUM_ITEMSPACE(0), insertSlot);
                            user->SaveInventory();
                        }
                    }
                }
            }
            else if (groupIdx == 3)
            {
                CItem* src = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                Inven_Item item;
                if (src != 0)
                {
                    src->make_item(item);
                    item.m_addInfo = static_cast<int>(itemIdx64 & 0xffffffffu);
                    item.m_fieldb = stackCount;
                    const int expirationDate = src->getExpirationDate();
                    const int usablePeriod = src->getUsablePeriod();
                    if (buyFlag == 1)
                    {
                        _processCreatureGift(user, item, accIdReq, characNoReq, 0, 0);
                        (void)expirationDate;
                        (void)usablePeriod;
                    }
                    else
                    {
                        _processCreature(user, item, restrictCode, resultState,
                                         true, usablePeriod, expirationDate);
                    }
                }
            }
            else if (groupIdx == 10)
            {
                _processAdditionalItem(user, itemIdx64, stackCount, avatarSelB,
                                       avatarSelA, resultItems, itemParam, '\0');
                int followGroup = -1;
                CItem* pkg = G_CDataManager()->find_item(static_cast<int>(itemIdx64));
                if (pkg != 0 && pkg->isCerashopPackageItem())
                {
                    followGroup = 7;
                }
                if (followGroup != 7)
                {
                    resultItems.clear();
                }

                CDataManager* dm = G_CDataManager();
                CItemLimitEditionMgr* limitMgr = dm->getItemLimitEditionMgr();
                CItemLimitEdition* limit = limitMgr->getItemInfo(productNo);
                if (limit == 0)
                {
                    throw ENUM_ERROR(0x11);   // ORIG __cxa_throw(int 0x11)
                }
                int restrictLocal = limit->getRelateditem();
                int hitVal[2] = { 0, 0 };
                bool isSpecialFlag = false;
                const bool specialHit = (limit->getFeaturedIdx() != 0) ||
                    IsSpecialItem(restrictLocal, hitVal[1], hitVal[0], isSpecialFlag);
                if (specialHit)
                {
                    restrictLocal = G_CDataManager()->getItemList()->GetRestrictCode(
                        limit->getRelateditem());
                }
                if (restrictLocal != 0)
                {
                    SaveFeaturedIdx(user, limit->getFeaturedIdx(),
                                    GlobalData::s_systemTime_.getCurSec(), 0);
                }
            }

            if (restrictCode == 0)
            {
                if (ceraPriceReq != 0)
                {
                    GlobalData::s_BreakAwaySys->updateUsedCera(accIdReq, ceraPriceReq);
                }
                if (goldSpend != 0)
                {
                    GetInstanceCirculationStatistic()->AddCirculationStatistic(
                        CIRCULATION_STATISTIC_FIELD(0x26), user,
                        static_cast<unsigned int>(goldSpend));
                }
                user->getCurCharacInvenW()->use_money(goldSpend, eMoneySubReason(8), true);
                user->UseWinPoint(winPointReq, eWPSubReason(2));
                user->UseCera(ceraPriceReq);
                user->addUsedCera(ceraPriceReq);
                if (featuredIdxReq != 0 &&
                    !user->IsRestrictedGoods(featuredIdxReq))
                {
                    user->SetRestrictedGoods(featuredIdxReq);
                    SaveFeaturedIdx(user, featuredIdxReq, static_cast<int>(curSec), 0);
                }

                outPkt.put_header( 1, 0x43);
                outPkt.put_byte( 1);
                outPkt.put_byte( static_cast<int>(
                    static_cast<signed char>(buyFlag)));
                outPkt.put_int( groupIdx);
                outPkt.put_int( static_cast<int>(productNo));
                outPkt.put_int( seq);
                outPkt.put_int( itemCountField);
                outPkt.put_int( resultState);
                outPkt.put_short(
                                              static_cast<int>(resultItems.size()));
                for (std::vector<Inven_Item>::const_iterator it = resultItems.begin();
                     it != resultItems.end(); ++it)
                {
                    outPkt.put_int( it->m_addInfo);
                    outPkt.put_int( it->m_addInfo2);
                }

                unsigned long bonusItem = 0xffffffffUL;     // local_124
                unsigned int bonusCount = 0;                // local_128
                if (buyFlag == 1 && accIdReq != 0)
                {
                    DB_UpdateGiftCount giftReq;
                    giftReq.makeRequest(accIdReq, 1);
                }
                if (seq == itemCountField)
                {
                    if (WongWork::CEventMgr::IsChildrensDayEventing() &&
                        user->IsOKChildrensDayEventCondition())
                    {
                        WongWork::CWebEvent::ReqSaveEventItem(
                            user->get_acc_id(),
                            static_cast<unsigned int>(user->getCurCharacNo()),
                            0x65ae, 1u, 0x30d47, 0u, true);
                        PacketGuard notiPkt;                // local_140
                        notiPkt.put_header( 0, 0xe);
                        notiPkt.put_byte( 1);
                        int idxPos = notiPkt.get_index();
                        int filled = 0;
                        notiPkt.put_short( 10);
                        for (int d = 0; d < 10; ++d)
                        {
                            const int targetSlot =
                                user->GetChildrensDayEventTargetItemSlot(d);
                            const int targetPeriod =
                                user->GetChildrensDayEventTargetItemPeriod(d);
                            if (targetSlot < 0 || targetPeriod == -1)
                            {
                                break;
                            }
                            Inven_Item bufItem;
                            GetInvenSlot(&bufItem,
                                         const_cast<CInventory*>(user->getCurCharacInvenR()),
                                         2, targetSlot);
                            CInventory* invW = user->getCurCharacInvenW();
                            const int newSlot = invW->AddAvatarItem(
                                bufItem.m_addInfo, targetPeriod, false,
                                static_cast<char>(bufItem.m_field1), '\xff',
                                "E_CHILDRENS_DAY", eAvatarItemAddReason(2), false, 0);
                            if (newSlot >= 0)
                            {
                                user->getCurCharacInvenR()->MakeItemPacket(
                                    INVEN_TYPE(2), newSlot, notiPkt);
                                ++filled;
                            }
                        }
                        notiPkt.put_short(idxPos, filled);
                        notiPkt.finalize( true);
                        user->Send(notiPkt);
                    }

                    CEventManager* evMgr = GlobalData::s_event_manager;
                    CEventBase* bonusBase = evMgr->GetRepeatEvent(0x11);
                    if (bonusBase != 0 && bonusBase->IsEventing(user))
                    {
                        CCeraShopBonusItemEvent* bonus =
                            static_cast<CCeraShopBonusItemEvent*>(bonusBase);
                        const int rate = bonus->getBonusRate();
                        if (get_rand_int(1000) <= rate)
                        {
                            const int usedCera = static_cast<int>(user->getUsedCera());
                            G_CDataManager()->getItemList()->getCeraShopBonusItem(
                                usedCera, bonusItem, bonusCount);
                            if (bonusItem != 0xffffffffUL)
                            {
                                std::vector<Inven_Item> bonusItems;    // local_150
                                std::vector<std::pair<int,int> > bonusPairs; // local_15c
                                _processAdditionalItem(user, bonusItem,
                                                       static_cast<unsigned short>(bonusCount),
                                                       '\0', '\xff', bonusItems,
                                                       itemParam, '\0');
                            }
                        }
                    }
                    outPkt.put_int( static_cast<int>(bonusItem));
                    outPkt.put_int( static_cast<int>(bonusCount));

                    PacketGuard ceraPkt;                    // local_134
                    ceraPkt.put_header( 0, 0x35);
                    ceraPkt.put_byte( 1);
                    ceraPkt.put_int( user->GetCera());
                    ceraPkt.finalize( true);
                    user->Send(ceraPkt);

                    ceraPkt.clear();
                    ceraPkt.put_header( 0, 0xe);
                    ceraPkt.put_byte( 0);
                    ceraPkt.put_short( 2);
                    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE(1), 0, ceraPkt);
                    user->getCurCharacInvenR()->MakeItemPacket(INVEN_TYPE(1), 2, ceraPkt);
                    ceraPkt.finalize( true);
                    user->Send(ceraPkt);
                }
                success = true;
            }
            else
            {
                _ProcessIPGError(user, reqData, resData,
                                 static_cast<unsigned char>(restrictCode),
                                 groupIdx, productNo, seq, itemCountField,
                                 static_cast<char>(buyFlag));
                cMyTrace t(
                    "void WongWork::CCeraShop::ProcessIPGUserData(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)",
                    0x955, 0);
                t("[%s][%d]",
                  "void WongWork::CCeraShop::ProcessIPGUserData(CUser*, WongWork::IPG::SIPGData*, WongWork::IPG::SIPGData*, Stream*)",
                  0x955);
                success = false;
            }
        }

        if (!success)
        {
            return;
        }
        outPkt.finalize( true);
        user->Send(outPkt);
    }
}

}  // namespace WongWork

// ---------------------------------------------------------------------------
// ORIG W 0x08179fb4 _ZNK5CItem21isCerashopPackageItemEv：逐指令转写。
// 经 vtable 槽 +0x0c 虚方法（子类型索引，与 CI_SubType 先例同型）连续两次
// 调用，返回值 ==0xe 或 ==0xf 判定礼包类目 → true，否则 false。
// ---------------------------------------------------------------------------
bool CItem::isCerashopPackageItem() const
{
    typedef int (*Fn)(const void*);
    const void* item = this;
    int slot = (*reinterpret_cast<Fn*>(*reinterpret_cast<void* const*>(item) + 0x0c))(item);
    if (slot == 0x0e)
        return true;
    return (*reinterpret_cast<Fn*>(*reinterpret_cast<void* const*>(item) + 0x0c))(item) == 0x0f;
}
