// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ItemVendingMachine_Auction/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "ItemVendingMachine_Auction/BuyAuctionItem.inc"
                                        int /*a3*/, bool bPremiumFee, int refValue)
{
    // ---- 手续费基数（ORIG 0x8175c96..0x8175dc4）----
    // 具名局部集合照 ORIG 槽位（P0：有槽往返 ⇒ 原码有具名局部）：
    //   -0x44 nRef / -0x3d bPremium / -0x3c nFeeNormal(5) / -0x38 nFeePremium(2)
    //   / -0x34 feeBase / -0x30 price / -0x20 rate（double）。
    int  nRef        = refValue;
    bool bPremium    = bPremiumFee;
    int  nFeeNormal  = 5;
    int  nFeePremium = 2;
    int  feeBase     = 0;
    int  price       = info.m_price;
    if (nRef == 0)
    {
        // ORIG 0x08175ca4..0x08175cbb：两臂各自 `mov 常量槽,%eax; mov %eax,feeBase槽`
        // （三目在 4.4 会合并成共享 store ⇒ 少 1 条，故写 if/else）。
        if (bPremium)
            feeBase = nFeePremium;
        else
            feeBase = nFeeNormal;
    }
    else
    {
        // .rodata 实读（objdump -s -j .rodata 0x8b871a0 段）：
        // 0x08b871a8=0.5 / b0=1.5 / b8=1.6 / c0=1.7 / c8=1.8 ⇒ 分档边界是 **1.7**，
        // 判定链是嵌套短路对（0x08175cd0..0x08175d80 共 8 次 fucompp），每层写成
        // `(rate < Lo) || (Hi <= rate)`。ORIG 把商缓存进具名 double rate(-0x20) 后
        // 每次比较只 `fldl -0x20` 复用（0x08175cc8 fstpl -0x20 / 0x08175ccb fldl -0x20）。
        double rate = static_cast<double>(price) / static_cast<double>(nRef);
        if (rate < kRateDeny)
            return false;
        // [S6] ORIG 0x08175cec..0x08175d1a：顶层判定是 A 形态（`fldl rate; fldl 1.5;
        // fucompp; test $0x45,%ah; sete`＝「rate < 1.5」）且 fall-through 直落
        // bPremium 臂 ⇒ 源形为 `if (rate < kRate150) {裸费率臂} else {分档链}`
        // （NaN 亦落裸费率臂，与 ORIG 一致；此前 `kRate150 <= rate` 形在 4.4 生成
        // 相位相反的 `fxch/setae` 形态且 NaN 落分档链）。
        if (rate < kRate150)
        {
            if (bPremium)
            {
                feeBase = nFeePremium;
            }
            else
            {
                feeBase = nFeeNormal;
            }
        }
        else
        {
            // [S6] ORIG 0x08175c3..0x08175dc4 的分档链在 4.1 下是「假跳转 + 取反判定」
            // 交替形态：B(1.5)/A(1.6) → B(1.6)/A(1.7) → B(1.7)/A(1.8)，then 臂各写
            // 0x32 后跳 END，末层 0x50 fall-through。该形态由 `Lo <= rate && rate < Hi`
            // （德摩根，then 写 0x32）产出；旧 `rate < Lo || Hi <= rate`（Ghidra 显示序）
            // 在 4.4 生成相位相反的 A/B 并合并两个 0x32 尾块。语义等价（NaN 亦同落
            // 末层，与 ORIG 0x08175d66 起 setae 判定一致）。
            if (kRate150 <= rate && rate < kRate160)
            {
                feeBase = 0x32;
            }
            else if (kRate160 <= rate && rate < kRate170)
            {
                feeBase = 0x32;
            }
            else if (kRate170 <= rate && rate < kRate180)
            {
                feeBase = 0x32;
            }
            else
            {
                feeBase = 0x50;
            }
        }
    }

    // ---- fee = trunc(price/100.0 * feeBase)（ORIG fldcw RC=0b11 后 fistpl）----
    // ORIG 0x08175dcb..0x08175dfa：先 `fildl price; fldl 100.0; fdivrp` 再 `fildl feeBase;
    // fmulp`（price/100.0 是左操作数），`fistpl -0x34` **写回 feeBase 槽**（无独立 fee
    // 局部），随后 `mov -0x34,%edx; mov %edx,0xc(%eax)` 直写 info.m_fee。
    feeBase = static_cast<int>(static_cast<double>(price) / kFeeDivisor *
                              static_cast<double>(feeBase));
    info.m_fee = feeBase;

    // ---- 审计流：put_binary(&info, 0x28)（ORIG 0x8175e09..0x8175e5a）----
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/AppendVendingMachine.cpp", 0x7b),
        true);
    // 0x08175e51..0x08175e5a：`lea -0x50; call CStreamGuard::operator->`
    // 之后再 `call put_binary` ⇒ 原码写的是 guard->put_binary（P-B 代理链形）。
    guard->put_binary(&info, 0x28);

    // ---- 计费入参字符串（ORIG buf8/buf36/buf21 三缓冲 + atoi 回读）----
    // priceBuf ← "%ld" 99999（购买单价槽恒为 99999）；payBuf ← "%d"(price-fee 或 fee)；
    // uidBuf ← "%ld" itemUid（ORIG 压两个 dword：低 32 位 0xc(%esp)+高 32 位 0x10(%esp)）。
    // 槽位/声明序照 ORIG：payBuf(-0x8d,0x24) → uidBuf(-0x69,0x15) → priceBuf(-0x54,8)；
    // uidBuf 带 `= {0}`：ORIG 0x08175e5f..0x08175ebd 对其发 21 字节对齐感知清零展开
    // （P214③/P312），且清零发生在 put_binary 之后 ⇒ 声明点即此处。
    char payBuf[0x24];
    char priceBuf[8];
    char uidBuf[0x15] = {0};
    // ORIG 0x08175ec0 `movl $0x1869f,-0x2c(%ebp)` + 0x08175ec7 回读 ⇒ 具名 int 局部，
    // 两段 snprintf 共用（第二段 0x081760f1 直接读 -0x2c，无重赋值，P244）。
    int  nPrice = 99999;

    OS_API::snprintf(priceBuf, sizeof(priceBuf), "%ld", nPrice);
    OS_API::snprintf(payBuf, sizeof(payBuf), "%d", info.m_price - feeBase);
    OS_API::snprintf(uidBuf, sizeof(uidBuf), "%ld", info.m_itemUid);

    // 名字不缓存：ORIG 每笔计费前各自 call get_acc_name / getCurCharacName
    // （0x08175f6a、0x08175f7f 与第二笔同址各一次 ⇒ 共 4 次 call），
    // 缓存成局部会少 2 次 call（本函数 ORIG-only call 缺口的来源之一）。

    // 经 s_pIPGHelper 虚表 +0x3c 取 CIPGMgr*（ORIG 0x8175f46..0x8175f5a），再取计费句柄
    //（getBillingCash → *(mgr+0xec)，ORIG 0x0817a2ee）。虚表槽未建模，沿用工程内
    // CCeraShop_Special.cpp 的裸函数指针调用模式。【推断：等价 ABI】
    // ORIG 无函数指针具名槽（0x8175f46..0x8175f5a 直接 `mov (%eax),%edx; call *%edx`）
    // ⇒ 按 P0 把取槽表达式内联进调用点，勿缓存成局部。
    typedef void* (*GetIPGMgrFn)(void*);
    // ORIG 0x08175f3f `movl $0x0,-0x28(%ebp)` 先清零具名 mgr，0x08175f5c 再写回
    // （Ghidra `local_2c = 0; local_2c = (…)()` 两段）⇒ 保留两段赋值形。
    WongWork::IPG::CIPGMgr* mgr = 0;
    mgr = reinterpret_cast<WongWork::IPG::CIPGMgr*>(
            (*reinterpret_cast<GetIPGMgrFn*>(
                *reinterpret_cast<char**>(GlobalData::s_pIPGHelper) + 0x3c))(
                    GlobalData::s_pIPGHelper));
    // getBillingCash 不缓存：ORIG 两段计费各 call 一次
    // （0x08175f94 与第二笔 0x081760xx 同址），缓存会少 1 次 call。

    // ---- 第一段：主扣费（kind=0x3c，金额 price-fee）----
    int rc = mgr->getBillingCash()->purchase(
                        info.m_itemUid,
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
    OS_API::snprintf(priceBuf, sizeof(priceBuf), "%ld", nPrice);   // 0x081760f1 读 -0x2c
    OS_API::snprintf(payBuf, sizeof(payBuf), "%d", feeBase);
    OS_API::snprintf(uidBuf, sizeof(uidBuf), "%ld", info.m_itemUid);

    // [S6] ORIG 0x08176164..0x0817617a：第二段计费前**重取** IPGMgr（与首段
    // 0x08175f46..0x08175f5c 逐条同形：`mov s_pIPGHelper,%eax; mov (%eax),%eax;
    // add $0x3c,%eax; mov (%eax),%edx; mov s_pIPGHelper,%eax; mov %eax,(%esp);
    // call *%edx; mov %eax,-0x28(%ebp)`，写回**同一** mgr 槽 -0x28）⇒ 源形为对
    // mgr 再次赋值，而非复用首段结果（旧重建漏第二次 ⇒ 少 8 条 + 少一次虚调用）。
    mgr = reinterpret_cast<WongWork::IPG::CIPGMgr*>(
            (*reinterpret_cast<GetIPGMgrFn*>(
                *reinterpret_cast<char**>(GlobalData::s_pIPGHelper) + 0x3c))(
                    GlobalData::s_pIPGHelper));

    rc = mgr->getBillingCash()->purchase(
                        info.m_itemUid,
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
#include "ItemVendingMachine_Auction/_GetItemType.inc"
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
#include "ItemVendingMachine_Auction/_GetInvenSlotInfo.inc"
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
#include "ItemVendingMachine_Auction/_CheckEnableInventoryCondition.inc"
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
#include "ItemVendingMachine_Auction/LoadPurcahseBonusScript.inc"
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
// [b28d-26] NEAR 11 保持，N82 封顶（实测三形）：ORIG 0x0854c470 具名槽
//   goods(-0xc)/rate(-0x1c)/weight(-0x24) 且 cw 临时夹在 -0x1e/-0x20；4.4 恒
//   goods(-0xc)/rate(-0x10)/weight(-0x14)、cw 落 -0x1a/-0x1c（分配方向相反）。
//   试形①char padD[4]+padX[0xc] 声明穿插 → 仅帧 0x38→0x48、具名槽不动；
//   试形②double framePad;(void)&framePad（P190）→ 同样只撑帧不动槽基 ⇒ 封 N82。
#include "ItemVendingMachine_Auction/GetFinalProbability.inc"
{
    // [P6S6 2026-09-11 复核·仍 N] 试 P339 声明序反转（goods→rate→weight）：
    //   4.4 槽位随声明序变（goods -0x14/rate -0x10/weight -0xc），difflib 块 3→4
    //   反劣（ORIG 为 goods -0xc/rate -0x1c/weight -0x24 稀疏深槽 + cw 夹 -0x1e/-0x20，
    //   4.4 恒紧凑 + cw 置尾）⇒ 已回滚，维持 N82。
    float weight;
    int rate;
    CCeraShopGoods* goods;
    goods = reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(goodsIdx));
    weight = goods->GetBonusWeight();
    rate = reinterpret_cast<VendingItemListView*>(G_CDataManager())->m_list->GetVendingBonusRate();
    rate = (int)((float)rate * weight);
    return rate;
}

