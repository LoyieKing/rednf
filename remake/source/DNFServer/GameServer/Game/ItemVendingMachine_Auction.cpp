// ============================================================================
// df_game_r 还原 —— ItemVendingMachine::BuyAuctionItem（ORIG 0x08175c5c，
// _ZN18ItemVendingMachine14BuyAuctionItemEP5CUserRNS_15AuctionNeedInfoEibi，
// range [0x08175c5c, 0x08176347]，源文件 localjapan/AppendVendingMachine.cpp）。
//
// 依据 docs/class_func_reports/ItemVendingMachine/BuyAuctionItem.md + df_game_r
// objdump 逐指令核对（报告汇编段存在错位解码，常量与分支以 objdump 原文为准）。
// 声明见 ItemVendingMachine.h；Taiwan 计费依赖见 TaiwanCash.h/.cpp。
// C++98 / -m32 / O0 基线。
//
// ORIG 流程概要：
//   1) 手续费基数：refValue==0 → premium?2:5；否则 rate=price/refValue：
//      <0.5 拒绝（return false）；<1.5 → premium?2:5；<1.8 → 50；≥1.8 → 80。
//   2) fee = (int)(基数 * price/100.0)，fldcw RC=0b11 向零截断后 fistpl；
//      写回 info.m_fee(+0xc)。
//   3) StreamPool::Acquire("localjapan/AppendVendingMachine.cpp",0x7b) +
//      CStreamGuard(stream,true).put_binary(&info,0x28)（审计流序列化）。
//   4) 第一次 purchase(uid, 99999, price-fee, acc, charac, trans1, 0x3c)：
//      失败 → BillingFail + LogManager::logFormat(行号 0xa5) → false。
//   5) 第二次 purchase(uid, 99999, fee, acc, charac, trans2, 0x3e)：
//      失败 → 同上行号 0xe8 → false；成功 → true。
//   每段成功均 cUserHistoryLog::BillingSuccess(type=1, trans.m_tranNo, 金额)。
// ============================================================================

#include "ItemVendingMachine.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "StreamPool.h"
#include "LogManager.h"
#include "CUser.h"
#include "CInventory.h"  // OS_API::snprintf（ORIG 0x858c81c）
#include <cstdlib>  // atoi（ORIG atoi@plt）

namespace
{

// .rodata 费率常量（df_game_r 实测字节）
const double kRateDeny   = 0.5;    // 0x08b871a8
const double kRate150    = 1.5;    // 0x08b871b0
const double kRate160    = 1.6;    // 0x08b871b8
const double kRate170    = 1.7;    // 0x08b871c0
const double kRate180    = 1.8;    // 0x08b871c8
const double kFeeDivisor = 100.0;  // 0x08b871d0

}  // namespace

// ORIG 布局断言：AuctionNeedInfo 0x28（put_binary 长度；trans1.m_tranNo 落在
// +0x14、trans2.m_tranNo 落在 +0x20，与 BillingSuccess/Fail 读参偏移一致）。
typedef char (*AuctionNeedInfoSizeAssert)[sizeof(ItemVendingMachine::AuctionNeedInfo) == 0x28 ? 1 : -1];
AuctionNeedInfoSizeAssert g_auctionNeedInfoSizeAssert = 0;

bool ItemVendingMachine::BuyAuctionItem(CUser* user, AuctionNeedInfo& info,
                                        int /*a3*/, bool bPremiumFee, int refValue)
{
    // ---- 手续费基数（ORIG 0x8175c96..0x8175dc4）----
    int feeBase = 0;
    if (refValue == 0)
    {
        feeBase = bPremiumFee ? 2 : 5;
    }
    else
    {
        const double rate = static_cast<double>(info.m_price) /
                            static_cast<double>(refValue);
        if (rate < kRateDeny)
            return false;
        if (rate < kRate150)
            feeBase = bPremiumFee ? 2 : 5;
        else if (rate < kRate180)   // [1.5,1.8)：ORIG 三段比较合并为同一 50 档
            feeBase = 50;
        else                        // >= 1.8
            feeBase = 80;
    }

    // ---- fee = trunc(feeBase * price/100.0)（ORIG fldcw RC=0b11 后 fistpl）----
    const int fee = static_cast<int>(feeBase *
                       (static_cast<double>(info.m_price) / kFeeDivisor));
    info.m_fee = fee;

    // ---- 审计流：put_binary(&info, 0x28)（ORIG 0x8175e09..0x8175e5a）----
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/AppendVendingMachine.cpp", 0x7b),
        true);
    guard.put_binary(&info, 0x28);

    // ---- 计费入参字符串（ORIG buf8/buf36/buf21 三缓冲 + atoi 回读）----
    // priceBuf ← "%ld" 99999（购买单价槽恒为 99999）；payBuf ← "%d"(price-fee 或 fee)；
    // uidBuf ← "%ld" itemUid 低 32 位（ORIG "%ld" 压两个 dword，仅低 32 位生效）。
    char priceBuf[8];
    char payBuf[0x24];
    char uidBuf[0x15];

    OS_API::snprintf(priceBuf, sizeof(priceBuf), "%ld", 99999L);
    OS_API::snprintf(payBuf, sizeof(payBuf), "%d", info.m_price - fee);
    OS_API::snprintf(uidBuf, sizeof(uidBuf), "%ld",
                     static_cast<long>(info.m_itemUid));

    const char* accName = user->get_acc_name();
    const char* characName = user->getCurCharacName();

    // 经 s_pIPGHelper 虚表 +0x3c 取 CIPGMgr*（ORIG 0x8175f46..0x8175f5a），再取计费句柄
    //（getBillingCash → *(mgr+0xec)，ORIG 0x0817a2ee）。虚表槽未建模，沿用工程内
    // CCeraShop_Special.cpp 的裸函数指针调用模式。【推断：等价 ABI】
    typedef void* (*GetIPGMgrFn)(void*);
    GetIPGMgrFn getMgr =
        *reinterpret_cast<GetIPGMgrFn*>(*reinterpret_cast<char**>(GlobalData::s_pIPGHelper) + 0x3c);
    WongWork::IPG::CIPGMgr* mgr =
        reinterpret_cast<WongWork::IPG::CIPGMgr*>(getMgr(GlobalData::s_pIPGHelper));
    Taiwan::TaiwanCash* cash = mgr->getBillingCash();

    // ---- 第一段：主扣费（kind=0x3c，金额 price-fee）----
    int rc = cash->purchase(static_cast<unsigned long long>(info.m_itemUid),
                            atoi(priceBuf), atoi(payBuf),
                            accName, characName, info.m_trans1, 0x3c);
    if (!CheckBillingErrorCode(rc))
    {
        user->m_historyLog.BillingFail(static_cast<ENUM_BILLING_TYPE>(1),
                                       info.m_trans1.m_tranNo,
                                       info.m_price, rc,
                                       const_cast<char*>("purchase"));
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp",
            "bool ItemVendingMachine::BuyAuctionItem(CUser*, ItemVendingMachine::AuctionNeedInfo&, int, bool, int)",
            0xa5, "IPG_OUTPUT user: %u, charac: %s reason: %d",
            user->getCurCharacNo(), user->getCurCharacName(), rc);
        return false;
    }
    user->m_historyLog.BillingSuccess(static_cast<ENUM_BILLING_TYPE>(1),
                                      info.m_trans1.m_tranNo, info.m_price);

    // ---- 第二段：手续费（kind=0x3e，金额 fee）----
    OS_API::snprintf(payBuf, sizeof(payBuf), "%d", fee);
    OS_API::snprintf(uidBuf, sizeof(uidBuf), "%ld",
                     static_cast<long>(info.m_itemUid));

    rc = cash->purchase(static_cast<unsigned long long>(info.m_itemUid),
                        atoi(priceBuf), atoi(payBuf),
                        accName, characName, info.m_trans2, 0x3e);
    if (!CheckBillingErrorCode(rc))
    {
        user->m_historyLog.BillingFail(static_cast<ENUM_BILLING_TYPE>(1),
                                       info.m_trans2.m_tranNo,
                                       info.m_fee, rc,
                                       const_cast<char*>("purchase"));
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp",
            "bool ItemVendingMachine::BuyAuctionItem(CUser*, ItemVendingMachine::AuctionNeedInfo&, int, bool, int)",
            0xe8, "IPG_OUTPUT user: %u, charac: %s reason: %d",
            user->getCurCharacNo(), user->getCurCharacName(), rc);
        return false;
    }
    user->m_historyLog.BillingSuccess(static_cast<ENUM_BILLING_TYPE>(1),
                                      info.m_trans2.m_tranNo, info.m_fee);

    return true;
}
