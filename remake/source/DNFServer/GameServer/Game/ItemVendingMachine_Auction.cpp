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
#include "CUser.h"
#include "CInventory.h"  // OS_API::snprintf（ORIG 0x858c81c）
#include "CDataManager.h"  // G_CDataManager/FindGoods/getItemList（GetFinalProbability）
#include "CItemList.h"     // CCeraShopGoods 完整定义（_GetItemType/_GetInvenSlotInfo 等）
#include <cstdlib>  // atoi（ORIG atoi@plt）
#include <cstdlib>  // atoi（ORIG atoi@plt）
#include "CDataManager.h"  // G_CDataManager/FindGoods/getItemList（GetFinalProbability）
#include "CItemList.h"     // CCeraShopGoods 完整定义（_GetItemType/_GetInvenSlotInfo 等）
#include <cstdlib>  // atoi（ORIG atoi@plt）
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

// ============================================================================
// W8_LongTail 批次 5 符号（ItemVendingMachine.h 声明；ORIG objdump 逐指令核对，
// 报告 docs/class_func_reports/ItemVendingMachine/{_GetItemType,_GetInvenSlotInfo,
// _CheckEnableInventoryCondition,LoadPurcahseBonusScript,GetFinalProbability}.md）
// ============================================================================

// ---- _GetItemType —— ORIG 0x08176348 ----
// 函数局部 static Inven_Item temp（guard 惰性构造）；GetRelatedItem 取 CItem*
// 后经 vtable+0x08（ORIG vtable 槽 2 = make_item）拷入 temp，返回 temp.m_field1
// （+0x01，ITEM_TYPE）。temp 每次调用先 reset（0x80cb7d8）。
Inven_Item::ITEM_TYPE ItemVendingMachine::_GetItemType(const CCeraShopGoods* goods)
{
    static Inven_Item temp;
    temp.reset();
    goods->GetRelatedItem()->make_item(temp);
    return (Inven_Item::ITEM_TYPE)temp.m_field1;
}

// ---- _GetInvenSlotInfo —— ORIG 0x081763be ----
// 稀疏 switch 决策树（P46：case 数值升序 0/2/3/7；叶块排放 = 书写序）。
// groupIdx: 0→AVATAR；2→EQUIP+_GetItemType；3→CREATURE+_GetItemType；
// 7→EQUIP+itemType=2；其余保持调用方初值，恒 return true。
// [b28c-L19] AE：store 序 LHS-first 靠 _GetItemType 返回类型改 Inven_Item::ITEM_TYPE
// （P211 同型化，ItemVendingMachine.h:125 同步；mangled 不含返回类型）。
bool ItemVendingMachine::_GetInvenSlotInfo(const CCeraShopGoods* goods,
                                           INVEN_TYPE& invenType,
                                           Inven_Item::ITEM_TYPE& itemType)
{
    int groupIdx = goods->GetGroupIdx();
    switch (groupIdx)
    {
    case 0:
        invenType = INVEN_TYPE_AVATAR;
        break;
    case 2:
        invenType = INVEN_TYPE_EQUIP;
        itemType = _GetItemType(goods);
        break;
    case 3:
        invenType = INVEN_TYPE_CREATURE;
        itemType = _GetItemType(goods);
        break;
    case 7:
        invenType = INVEN_TYPE_EQUIP;
        itemType = (Inven_Item::ITEM_TYPE)2;
        break;
    }
    return true;
}

// ---- _CheckEnableInventoryCondition —— ORIG 0x081768ba ----
// getCurCharacInvenR()->GetRemainCapacity(invenType,itemType) > 0，或物品
// get_index ∈ {200,0xcb,0xcc,0xcd,5,6,7} → true。ORIG 形态：条件真路径
// 统一 je/jg 到 mov $0，全假 fallthrough mov $1，再 test/je 双常反转
// （对应 if (!(C)) return false; return true; 逐字同构）。
bool ItemVendingMachine::_CheckEnableInventoryCondition(CUser* user, const CItem* item,
                                                        INVEN_TYPE invenType,
                                                        Inven_Item::ITEM_TYPE itemType)
{
    if (!(user->getCurCharacInvenR()->GetRemainCapacity(invenType, itemType) > 0 ||
          item->get_index() == 200 ||
          item->get_index() == 0xcb ||
          item->get_index() == 0xcc ||
          item->get_index() == 0xcd ||
          item->get_index() == 5 ||
          item->get_index() == 6 ||
          item->get_index() == 7))
        return false;
    return true;
}

// ---- LoadPurcahseBonusScript —— ORIG 0x08179eae ----
// 委托 m_bonus.LoadScript()；ORIG 形态为 call 后 xor $1、test/je 双常返回
// （if (!LoadScript()) return false; return true;）。
bool ItemVendingMachine::LoadPurcahseBonusScript()
{
    if (!m_bonus.LoadScript()) return false;
    return true;
}

// ---- GetFinalProbability —— ORIG 0x0854c470 ----
// FindGoods(goodsIdx) 视作 CCeraShopGoods* 取 GetBonusWeight()（float）；
// G_CDataManager()+0x0c 的 CItemList*（ORIG 内联读，非 getItemList W call）的
// GetVendingBonusRate()（W 0x0854c596，读 +0x4c）。fistpl 复用 rate 槽
// （ORIG fistpl -0x1c）。栈槽序依 ORIG：weight(-0x24)/rate(-0x1c)/goods(-0xc)，
// 4 个未用 dummy 撑出中间空隙（P31 帧差对齐）。+0x0c 读用 P1 组件视图折叠。
struct VendingItemListView { char m_pad[0xc]; CItemList* m_list; };  // CDataManager+0x0c 视图
// [b26-NL28] GetFinalProbability：删 4 个无 ORIG 证据的编造局部后帧 sub $0x38=ORIG；
//   残差=rate/weight 槽位平移（4.1 把 FPU cw 临时夹在具名槽之间，4.4 恒置槽区尾部，
//   N82 族源形不可消除），封顶。
int ItemVendingMachine::GetFinalProbability(int goodsIdx)
{
    float weight;
    int rate;
    CCeraShopGoods* goods;
    goods = reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(goodsIdx));
    weight = goods->GetBonusWeight();
    rate = reinterpret_cast<VendingItemListView*>(G_CDataManager())->m_list->GetVendingBonusRate();
    rate = (int)((float)rate * weight);
    return rate;
}

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
        // **语义修正 + P-H**：.rodata 实读（objdump -s -j .rodata 0x8b871a0 段）
        // 0x08b871a8=0.5 / b0=1.5 / b8=1.6 / c0=1.7 / c8=1.8 ⇒ 原码分档边界是
        // **1.7**（不是 1.8），且判定链是嵌套短路对（0x08175cd0..0x08175d80 共
        // 8 次 fucompp），每层写成 `(rate < Lo) || (Hi <= rate)` 两次比较。
        // 表达式必须逐处重算（不得有 rate 中间局部，否则 4.4 复用 ⇒ 少 5 组
        // fldl/fucompp/fnstsw/sahf 链，正是本函数 gap 主体之一）。
        if (static_cast<double>(info.m_price) / static_cast<double>(refValue) < kRateDeny)
            return false;
        if (kRate150 <= static_cast<double>(info.m_price) / static_cast<double>(refValue))
        {
            if (static_cast<double>(info.m_price) / static_cast<double>(refValue) < kRate150 ||
                kRate160 <= static_cast<double>(info.m_price) / static_cast<double>(refValue))
            {
                if (static_cast<double>(info.m_price) / static_cast<double>(refValue) < kRate160 ||
                    kRate170 <= static_cast<double>(info.m_price) / static_cast<double>(refValue))
                {
                    feeBase = 0x50;
                }
                else
                {
                    feeBase = 0x32;
                }
            }
            else
            {
                feeBase = 0x32;
            }
        }
        else if (bPremiumFee)
        {
            feeBase = 2;
        }
        else
        {
            feeBase = 5;
        }
    }

    // ---- fee = trunc(feeBase * price/100.0)（ORIG fldcw RC=0b11 后 fistpl）----
    const int fee = static_cast<int>(feeBase *
                       (static_cast<double>(info.m_price) / kFeeDivisor));
    info.m_fee = fee;

    // ---- 审计流：put_binary(&info, 0x28)（ORIG 0x8175e09..0x8175e5a）----
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/AppendVendingMachine.cpp", 0x7b),
        true);
    // 0x08175e51..0x08175e5a：`lea -0x50; call CStreamGuard::operator->`
    // 之后再 `call put_binary` ⇒ 原码写的是 guard->put_binary（P-B 代理链形）。
    guard->put_binary(&info, 0x28);

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

    // 名字不缓存：ORIG 每笔计费前各自 call get_acc_name / getCurCharacName
    // （0x08175f6a、0x08175f7f 与第二笔同址各一次 ⇒ 共 4 次 call），
    // 缓存成局部会少 2 次 call（本函数 ORIG-only call 缺口的来源之一）。

    // 经 s_pIPGHelper 虚表 +0x3c 取 CIPGMgr*（ORIG 0x8175f46..0x8175f5a），再取计费句柄
    //（getBillingCash → *(mgr+0xec)，ORIG 0x0817a2ee）。虚表槽未建模，沿用工程内
    // CCeraShop_Special.cpp 的裸函数指针调用模式。【推断：等价 ABI】
    typedef void* (*GetIPGMgrFn)(void*);
    GetIPGMgrFn getMgr =
        *reinterpret_cast<GetIPGMgrFn*>(*reinterpret_cast<char**>(GlobalData::s_pIPGHelper) + 0x3c);
    WongWork::IPG::CIPGMgr* mgr =
        reinterpret_cast<WongWork::IPG::CIPGMgr*>(getMgr(GlobalData::s_pIPGHelper));
    // getBillingCash 不缓存：ORIG 两段计费各 call 一次
    // （0x08175f94 与第二笔 0x081760xx 同址），缓存会少 1 次 call。

    // ---- 第一段：主扣费（kind=0x3c，金额 price-fee）----
    int rc = mgr->getBillingCash()->purchase(
                        static_cast<unsigned long long>(info.m_itemUid),
                        atoi(priceBuf), atoi(payBuf),
                        user->get_acc_name(), user->getCurCharacName(),
                        info.m_trans1, 0x3c);
    if (!CheckBillingErrorCode(rc))
    {
        user->m_historyLog.BillingFail(static_cast<ENUM_BILLING_TYPE>(1),
                                       info.m_trans1.m_tranNo,
                                       info.m_price, rc,
                                       const_cast<char*>("purchase"));
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp",
            __PRETTY_FUNCTION__,
            0xa5, "IPG_OUTPUT user: %u, charac: %s reason: %d",
            user->getCurCharacNo(), user->getCurCharacName(), rc);
        return false;
    }
    user->m_historyLog.BillingSuccess(static_cast<ENUM_BILLING_TYPE>(1),
                                      info.m_trans1.m_tranNo, info.m_price);

    // ---- 第二段：手续费（kind=0x3e，金额 fee）----
    OS_API::snprintf(priceBuf, sizeof(priceBuf), "%ld", 99999L);   // 0x08176015
    OS_API::snprintf(payBuf, sizeof(payBuf), "%d", fee);
    OS_API::snprintf(uidBuf, sizeof(uidBuf), "%ld",
                     static_cast<long>(info.m_itemUid));

    rc = mgr->getBillingCash()->purchase(
                        static_cast<unsigned long long>(info.m_itemUid),
                        atoi(priceBuf), atoi(payBuf),
                        user->get_acc_name(), user->getCurCharacName(),
                        info.m_trans2, 0x3e);
    if (!CheckBillingErrorCode(rc))
    {
        user->m_historyLog.BillingFail(static_cast<ENUM_BILLING_TYPE>(1),
                                       info.m_trans2.m_tranNo,
                                       info.m_fee, rc,
                                       const_cast<char*>("purchase"));
        LogManager::logFormat(
            1, "localjapan/AppendVendingMachine.cpp",
            __PRETTY_FUNCTION__,
            0xe8, "IPG_OUTPUT user: %u, charac: %s reason: %d",
            user->getCurCharacNo(), user->getCurCharacName(), rc);
        return false;
    }
    user->m_historyLog.BillingSuccess(static_cast<ENUM_BILLING_TYPE>(1),
                                      info.m_trans2.m_tranNo, info.m_fee);

    return true;
}
