#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Dispatcher_UseVendingMachine_impl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Dispatchers.h"

// ORIG 自由函数 LineFunc(int, const char*, int, unsigned int) @0x085908d7 T；
// 定义由 CompoundEmblemDeps.cpp 提供（见 CompoundEmblemDeps.h）。
extern int LineFunc(int, const char*, int, unsigned int);

// ---------------------------------------------------------------------------
// Dispatcher_UseVendingMachine::dispatch_sig —— 自动售货机（扫地机/道具售货机）
// 购买流程还原。
//
// 依据 docs/class_func_reports/Dispatcher_UseVendingMachine/（ORIG
// dispatch_sig @ 0x0821c2e6，约 1009 指令；连同 6 个辅助 _get/_put/_notify 方法）。
//
// 说明（`IMPL` = 本文件重构决策，非 ORIG 事实）：
//   1. ORIG 的这些辅助方法是类成员（_ZNK28Dispatcher_UseVendingMachine…），
//      Dispatchers.h 类声明未含这些成员且被约定只读，故本文件以 TU 内
//      file-local 辅助函数实现，显式传 CMTRand* 与数据源上下文。
//   2. ORIG 使用 this->m_rand（CMTRand* @ +0x04，由 ctor 以 time 种子创建）。
//      Dispatchers.cpp 的 ctor 为空桩，不改；故 dispatch_sig 内新建本地 CMTRand
//      并按 ORIG ctor 语义以 time 播种，传给各辅助函数。
//   3. 外部依赖已由 producer 在权威头/TU 落地：CVendingMachineStatistic /
//      TimerVendingMachineItemNotify（VendingMachine.h）、CGlobalEffectManager
//      （CGlobalEffectManager.h）、CStackableItem::getGlobalEffectData、
//      user_creature::CCreatureItemConverter::SetEggItem（CInventory.h）。
//      本文件直调这些真实成员，不再定义本地替身。
//   4. 返回码层面的错误路径（ORIG 为 __cxa_throw ENUM_ERROR / LineFunc 断言）
//      在本文件以对应 return 值近似表达，语义与报告一致（见 game_verify_guide）。
// ---------------------------------------------------------------------------

#include <ctime>

#include <cstring>

#include <algorithm>

#include "CDataManager.h"
#include "CInventory.h"
#include "CUserCharacInfo.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "Inven_Item.h"
#include "CUser.h"
#include "GlobalData.h"
// 权威 WongWork::CGenUniqueNo 见 CInventory.h（含 genIPGNo(ENUM_IPGNO_TYPE,uint,char*) @0x084b7750），
// 故不再 include CGenUniqueNo.h（其类签名不同且与 CInventory.h 冲突）。
#include "CCeraShop.h"
#include "PacketGuard.h"
#include "CSystemTime.h"
#include "PvPTypes.h"   // 权威 CMTRand 声明（PvPTypes.h:178；CMTRand.h 不存在，类定义于 CMTRand.cpp）
#include "VendingMachine.h"   // CVendingMachineStatistic / TimerVendingMachineItemNotify 权威声明
#include "CGlobalEffectManager.h"
#include "CUserProc.h"   // APSystem::CUserProc::ClearActionAndSendtoUser 权威声明
#include "CGuildServerProxy.h"   // CEventManager::GetRepeatEvent / CEventBase::IsEventing 权威声明

/* =========================================================================
 * ORIG 嵌套类型 stPutItemResult_t —— _putItemIntoUser/_notifyItemInfo 共用输出
 * （0x10 字节；ORIG _putItemIntoUser 0x0821b71c 开头两 store +0x00/+0x04、
 *  _notifyItemInfo 0x0821c17c 读 +0x00/+0x04 实证）。
 * 声明见 Dispatchers.h（类内 struct stPutItemResult_t; 前向）。
 * ====================================================================== */
struct Dispatcher_UseVendingMachine::stPutItemResult_t
{
    unsigned long  m_itemIdx;   // +0x00
    unsigned int   m_count;     // +0x04
    int            m_result;    // +0x08
    short          m_slot;      // +0x0c
    unsigned char  m_success;   // +0x0e
};

/* =========================================================================
 * 售货机数据直接使用 CDataManager.h 权威结构（不再本地副本）：
 *   stVendingMachine_t      +0x00 m_id；+0x04 m_infoList
 *   stVendingMachineInfo_t  +0x00 m_groupNum；+0x04 m_payType；+0x08 m_payAmount；
 *                           +0x0c m_giveCount；+0x10 m_vecGive；+0x1c m_bonusCount；
 *                           +0x20 m_vecBonus；+0x2c m_lottery
 *   stVendingMachineOutput_t +0x00 m_itemId；+0x04 m_cumWeight；+0x08 m_count
 * ====================================================================== */

#include "Dispatcher_UseVendingMachine_impl/_getVendingMachine.inc"


#include "Dispatcher_UseVendingMachine_impl/_getVendingMachineInfo.inc"


/* =========================================================================
 * 辅助：按权重随机选一个给/赏物品，写入 out（Inven_Item）。
 * ==================================================================== */

#include "Dispatcher_UseVendingMachine_impl/_getGiveItem.inc"


#include "Dispatcher_UseVendingMachine_impl/_getBonusItem.inc"


/* =========================================================================
 * 辅助：把物品/货币/胜点交给用户，填充 result。
 * ==================================================================== */
#include "Dispatcher_UseVendingMachine_impl/_putItemIntoUser.inc"
    CUser* user, unsigned long itemIdx, unsigned int count,
    stPutItemResult_t& result) const
{
    // 常量（ORIG 直接 cmpl 立即数比较，避免局部 const 数组）
    enum { PAY_MONEY = 0, PAY_EVENTCOIN = 1, PAY_WINPOINT = 2 };
    // （其它值 = 给实物物品，按 itemIdx）

    result.m_itemIdx = itemIdx;
    result.m_count   = count;
    result.m_result  = 0;
    result.m_slot    = -1;
    result.m_success = 1;

    if (result.m_itemIdx == 0xffffffffUL)
        return;

    short slot = -1;

    if (itemIdx == PAY_MONEY)
    {
        static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->gain_money(
            (int)count, (eMoneyAddReason)0x15, 1, 0);
        slot = 0;
    }
    else if (itemIdx == PAY_EVENTCOIN)
    {
        static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->SetEventCoin(
            static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->GetEventCoin()
                + (int)count);
        // ORIG: cUserHistoryLog::EventCoinAdd((cUserHistoryLog*)(user+0x79700),coin,amount,6)
        user->m_historyLog.EventCoinAdd(
            (int)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()->GetEventCoin(),
            (int)count, (eCoinAddReason)6);
        slot = 1;
    }
    else if (itemIdx == PAY_WINPOINT)
    {
        // ORIG _ZN5CUser12gainWinPointEi12eWPAddReason @0x0864fd2c（CUser 域已补声明+实现）。
        user->gainWinPoint((int)count, (eWPAddReason)3);
        slot = 2;
    }
    else
    {
        // 实物物品
        CStackableItem* item = static_cast<CStackableItem*>(
            G_CDataManager()->find_item((int)itemIdx));
        if (item == 0)
        {
            result.m_success = 0;
            return;
        }

        Inven_Item outItem;
        outItem.m_addInfo = itemIdx;
        G_CDataManager()->m_itemList->create_item((ENUM_ITEM_CREATE_TYPE)0, outItem,
                                                  (int)count);

        if (item->GetItemType() == 0x17)
        {
            // 需要全局效果：按 m_vec280（vector<stGlobalEffect_t>）逐项注册。
            // ORIG：registNewEffect(m_field0, m_field4, characName, 5, m_field8)。
            const std::vector<stGlobalEffect_t> eff = item->getGlobalEffectData();
            for (std::vector<stGlobalEffect_t>::const_iterator it = eff.begin(),
                     e = eff.end(); it != e; ++it)
            {
                GlobalData::s_globalEffectManager_->registNewEffect(
                    it->m_field0, it->m_field4,
                    static_cast<CUserCharacInfo*>(user)->getCurCharacName(), 5,
                    (long)it->m_field8);
            }
        }
        else if (item->IsAvatarItem())
        {
            char ipgBuf[0x18];
            slot = (short)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                       ->AddAvatarItem((int)itemIdx, item->getUsablePeriod(), /*b*/0, /*c*/0,
                                       /*d*/(char)-1,
                                       WongWork::CGenUniqueNo::genIPGNo(
                                           (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)4,
                                           static_cast<CUserCharacInfo*>(user)->getCurCharacNo(),
                                           ipgBuf),
                                       (eAvatarItemAddReason)0xe,
                                       /*e*/0, /*f*/(unsigned short)0);
            result.m_result = 1;
        }
        else if (item->IsCreatureItem() && item->GetItemType() == 0x16)
        {
            // 生物（蛋/宠物），先 SetEggItem 再 insert
            if (outItem.m_itemType == 5 /* ITEM_TYPE_CREATURE_EQUIP */)
            {
                if (!user_creature::CCreatureItemConverter::SetEggItem(&outItem))
                    return;
                slot = (short)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                           ->insertItemIntoInventory(outItem, (eItemAddReason)0x23, 1, 0);
                if (slot >= 0)
                {
                    static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                        ->GetCreatureMgrW()->InsertCreatureItem(
                            &outItem, (int)slot, 0x23, item->getUsablePeriod(),
                            item->getExpirationDate());
                }
                // ORIG 0x0821bc0d：仅 SetEggItem 成功后才标记结果类型 7；
                // 转换失败或非 creature equipment 直接沿用默认结果类型 0。
                result.m_result = 7;
            }
        }
        else
        {
            // 普通实物
            // ORIG 0x0821bc1c 起：Cera 特殊物品处理 + 受限商品（防 hack）完整路径。
            //   IsSpecialItem → 非 0x10（普通 stackable）时 ProcessSpecialItem 分发，
            //   restrictCode 非 0 且未受限 → SetRestrictedGoods + SaveFeaturedIdx +
            //   SendRestrictedGoods；ProcessSpecialItem 返回 true 且 rc!=0 表示特殊
            //   处理已发物，直接结束（result 保持默认）。
            int specialType = 0, specialValue = 0;
            bool specialFlag = false;
            bool specialHandled = false;   // ORIG -0x21(%ebp)
            if (GlobalData::s_pCeraShop->IsSpecialItem((int)outItem.m_addInfo, specialType,
                                                       specialValue, specialFlag)
                && item->GetItemType() != 0x10)
            {
                std::vector<Inven_Item> items;    // ORIG -0x70(%ebp)
                int rc = 0;                       // ORIG -0x74(%ebp)
                int restrictCode =
                    G_CDataManager()->m_itemList->GetRestrictCode(
                        (int)outItem.m_addInfo);
                long curSec = GlobalData::s_systemTime_.getCurSec();
                WongWork::CCeraShop::stCeraShopItemParam_t param;  // ORIG -0x8c(%ebp)
                if (GlobalData::s_pCeraShop->ProcessSpecialItem(
                        user, (int)outItem.m_addInfo, restrictCode, rc, curSec, true,
                        items, param, 0, 0, false, 1)
                    && rc != 0)
                    return;   // ORIG：特殊处理已放物品，直接结束
                if (restrictCode != 0 && !user->IsRestrictedGoods(restrictCode))
                {
                    user->SetRestrictedGoods(restrictCode);
                    GlobalData::s_pCeraShop->SaveFeaturedIdx(user, restrictCode,
                                                             (int)curSec, 0);
                    user->SendRestrictedGoods(true, restrictCode);
                }
                specialHandled = true;   // ORIG：跳过普通 insert
            }
            if (!specialHandled)
            {
                slot = (short)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()
                           ->insertItemIntoInventory(outItem, (eItemAddReason)0x23, 1, 0);
                if (slot == -1)
                    result.m_success = 0;
            }
        }
    }

    result.m_slot = slot;
    if (result.m_success != 0)
    {
        GetInstanceVendingMachineStatistic()->addOutputItem(result.m_itemIdx,
                                                            result.m_count);
    }
}

/* =========================================================================
 * 辅助：给用户发"物品已放入"计时通知（若该物品尚未出现在用户拥有的列表）。
 * ==================================================================== */
#include "Dispatcher_UseVendingMachine_impl/_notifyItemInfo.inc"


// §6.2 巨件核销：P167 112/112 [0缺0多]；gap 1008→976；残差归因 N82
// （帧 0x1c0→0x200 全帧槽号平移 + 未链接 .o 地址归一，详见
//   docs/asm_translation_patterns.md 负例表）。
/* =========================================================================
 * dispatch_sig —— 主入口。
 * ==================================================================== */
#include "Dispatcher_UseVendingMachine_impl/dispatch_sig.inc"


#include "Dispatcher_UseVendingMachine_impl/process.inc"


#include "Dispatcher_UseVendingMachine_impl/read.inc"
