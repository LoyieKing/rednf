
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

// ORIG T 0x0821b60a _ZNK28Dispatcher_UseVendingMachine18_getVendingMachineEj
//   遍历 G_CDataManager()->m_vendingMachine 按 m_id 匹配，返回机器指针（无则 0）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：删除 dm==0 判空（ORIG 无），
//   改 `getVendingMachineTable()` + const_iterator 循环（ORIG 的 begin/end/
//   operator!=/operator++/operator* 调用点逐条对齐；ORIG 0x0821b610 段）。
const stVendingMachine_t* Dispatcher_UseVendingMachine::_getVendingMachine(
    unsigned int machineId) const
{
    const std::vector<stVendingMachine_t>& table =
        *G_CDataManager()->getVendingMachineTable();
    for (std::vector<stVendingMachine_t>::const_iterator it = table.begin(),
             e = table.end(); it != e; ++it)
    {
        if (it->m_id == machineId)
            return &(*it);
    }
    return 0;
}

// ORIG T 0x0821b698 _ZNK28Dispatcher_UseVendingMachine22_getVendingMachineInfoEPK18stVendingMachine_tj
//   在 vm->m_infoList 中按栏位 id（m_groupNum）匹配，返回栏信息（无则 0）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：成员 vector 在 begin/end 处
//   内联重算（去具名引用，ORIG 0x0821b6a1 `lea 0x4(%eax),%edx` 两次）+ 迭代器循环。
const stVendingMachineInfo_t* Dispatcher_UseVendingMachine::_getVendingMachineInfo(
    const stVendingMachine_t* vm, unsigned int slotId) const
{
    for (std::vector<stVendingMachineInfo_t>::const_iterator it = vm->m_infoList.begin(),
             e = vm->m_infoList.end(); it != e; ++it)
    {
        if (it->m_groupNum == slotId)
            return &(*it);
    }
    return 0;
}

/* =========================================================================
 * 辅助：按权重随机选一个给/赏物品，写入 out（Inven_Item）。
 * ==================================================================== */

// ORIG T 0x0821bf82 _ZNK28Dispatcher_UseVendingMachine12_getGiveItemEPK22stVendingMachineInfo_t
//   依 m_giveCount 权重（范围 = m_giveCount-1）经 this->m_rand 随机选 m_vecGive 中
//   一个输出；命中写 m_itemId → m_addInfo(+0x02)、m_count → m_addInfo2(+0x07)。
//   按值返回 Inven_Item（sret，0x3d 字节；ORIG 调用处逐字段拷贝实证）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：删除 range<0 防御（ORIG 无）、
//   randInt 直传具名 range（去中间 r）、迭代器循环（operator-> 每点重调）。
Inven_Item Dispatcher_UseVendingMachine::_getGiveItem(
    const stVendingMachineInfo_t* info) const
{
    Inven_Item out;               // ORIG：Inven_Item::Inven_Item(sret)
    out.m_addInfo = -1;           // 初始 idx = -1（“未选择”标记）
    unsigned long range = info->m_giveCount - 1;
    unsigned long pick = m_rand->randInt(range);
    for (std::vector<stVendingMachineOutput_t>::const_iterator it = info->m_vecGive.begin(),
             e = info->m_vecGive.end(); it != e; ++it)
    {
        if (pick < it->m_cumWeight)
        {
            out.m_addInfo = it->m_itemId;
            out.m_addInfo2 = (int)it->m_count;
            return out;
        }
    }
    return out;
}

// ORIG T 0x0821c058 _ZNK28Dispatcher_UseVendingMachine13_getBonusItemEPK22stVendingMachineInfo_t
//   依 m_bonusCount 权重随机选 m_vecBonus 中一个输出（事件开启检查见上版推断注释）。
//   命中写 m_itemId → m_addInfo(+0x02)、m_count → m_addInfo2(+0x07)。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：事件判活改内联链式
//   `s_event_manager->GetRepeatEvent(0x23)->IsEventing(0)`（ORIG 无 ev 判空/无具名槽，
//   0x0821c071 段），range 防御删除、迭代器循环同 _getGiveItem。
Inven_Item Dispatcher_UseVendingMachine::_getBonusItem(
    const stVendingMachineInfo_t* info) const
{
    Inven_Item out;               // ORIG：Inven_Item::Inven_Item(sret)
    out.m_addInfo = -1;
    // ORIG 0x0821c058：GetRepeatEvent(0x23) → IsEventing(0) 判活，
    // 事件未开启 → 返回空 Inven_Item（m_addInfo=-1）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x23)->IsEventing(0))
        return out;
    if (info->m_vecBonus.empty())
        return out;
    unsigned long range = info->m_bonusCount - 1;
    unsigned long pick = m_rand->randInt(range);
    for (std::vector<stVendingMachineOutput_t>::const_iterator it = info->m_vecBonus.begin(),
             e = info->m_vecBonus.end(); it != e; ++it)
    {
        if (pick < it->m_cumWeight)
        {
            out.m_addInfo = it->m_itemId;
            out.m_addInfo2 = (int)it->m_count;
            return out;
        }
    }
    return out;
}

/* =========================================================================
 * 辅助：把物品/货币/胜点交给用户，填充 result。
 * ==================================================================== */
// ORIG T 0x0821b71c _ZNK28Dispatcher_UseVendingMachine16_putItemIntoUserEP5CUsermjRNS_17stPutItemResult_tE
//   把 itemIdx（0=金币/1=活动币/2=胜点/else=实物）交给用户，填充 result。
// b28d-solo-Dispatcher_UseVendingMachine_impl [N7+N18a+setcc-fold]：ORIG 547 insn /
//   我方 552；p167 调用点差集 CLEAR 62/62（0 缺 0 多）⇒ 语义完成，byte 层封顶（§6.2）。
//   残差：①4 处虚调用 vptr 链目标寄存器（ORIG `mov (%eax),%edx; … call *%edx`
//   vs 4.4.7 恒目标 %eax/this %edx，N7；本 TU 未路由 446）；②`mov 0x8(%eax),%eax;
//   mov %eax,%edi` 被 4.4 折成单步（N18a）；③`A && B` 第二条件 ORIG setne 物化
//   vs 4.4 分支+常量物化（setcc-fold 镜像）；④registNewEffect arg1（int 字段→
//   unsigned 形参）恒多 `mov %eax,%edx` 中转（P265 族；字段侧需动共享头，未改）。
//   语义修正：补 `outItem.m_addInfo = itemIdx;`（ORIG 0x0821b883 实证，旧实现缺失）。
void Dispatcher_UseVendingMachine::_putItemIntoUser(
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
            if (outItem.m_field1 == 5 /* ITEM_TYPE_CREATURE_EQUIP */)
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
// ORIG T 0x0821c17c _ZNK28Dispatcher_UseVendingMachine15_notifyItemInfoEPK22stVendingMachineInfo_tP5CUserRKNS_17stPutItemResult_tE
//   在 info->m_lottery（vector<pair<int,int>>）中查找 (itemIdx,count)；命中才
//   registTimer 延迟发放（ORIG _notifyItemInfo 汇编：pair 构造 + find + operator==
//   判等后跳 registTimer）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：pair 直传 result 字段（去 (int)
//   cast ⇒ 复现 pair<unsigned long const&,unsigned int const&> 转换构造）、m_lottery
//   内联 begin/end（去具名 lot 引用）、具名 it + `== end` + goto 尾空标签复现
//   ORIG 0x0821c21f `jne <nop>` 岛（P223）。
void Dispatcher_UseVendingMachine::_notifyItemInfo(
    const stVendingMachineInfo_t* info, CUser* user,
    const stPutItemResult_t& result) const
{
    std::pair<int, int> key(result.m_itemIdx, result.m_count);
    std::vector<std::pair<int, int> >::const_iterator it =
        std::find(info->m_lottery.begin(), info->m_lottery.end(), key);
    if (it == info->m_lottery.end())
        goto done;
    TimerVendingMachineItemNotify::registTimer(user->GetUID(), result.m_itemIdx,
                                               result.m_count, 5);
done:;
}

// §6.2 巨件核销：P167 112/112 [0缺0多]；gap 1008→976；残差归因 N82
// （帧 0x1c0→0x200 全帧槽号平移 + 未链接 .o 地址归一，详见
//   docs/asm_translation_patterns.md 负例表）。
/* =========================================================================
 * dispatch_sig —— 主入口。
 * ==================================================================== */
int Dispatcher_UseVendingMachine::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ========================================================================
    // ORIG 0x0821c2e6（报告 docs/class_func_reports/Dispatcher_UseVendingMachine/
    // dispatch_sig.md 全文已读：## 汇编 + ## 反编译 C 两段）。
    // 关键结构证据：
    //  · 出口统一 `mov %ebx,%eax`（0x0821d2b7），各 return 路径先写 %ebx ⇒
    //    技巧39 EH-ebx 返回缓存。
    //  · 0x0821c2f1 先 `call get_state; cmp $3; setne %al; test; je`，其后
    //    0x0821c310 才 `cmpl $0x0,0xc(%ebp); je` + CheckInTrade 合并 bool
    //    （旧实现把 user==0 放最前，顺序错）。
    //  · 9 处 __cxa_allocate_exception(4) + __cxa_throw(&ENUM_ERROR::typeinfo=
    //    0x8b75f40) —— 载荷值 1/1/4/0x16/0x16/0x16/0x11/0x16/0x16（旧实现退化成
    //    return 0，既少 ~150 行指令又丢错误码上报，属语义缺陷）。
    //  · 两处 catch handler（0x0821c75b try[56c..6c9] / 0x0821ce0e try[c8bb..cd3e]）
    //    同体：`mov (%eax),%eax` 取 ENUM_ERROR → `movzbl %al` →
    //    SendCmdErrorPacket(0xda, (unsigned char)e) → %ebx=0 → 公共出口。
    //  · 空位检查 ORIG 是 7 次独立 check_empty_count 嵌套 if（0x0821c8bb.. 逐段），
    //    不是循环。
    // ========================================================================
    // b28d-solo-Dispatcher_UseVendingMachine_impl [N82+N7]：ORIG 1009 insn / 我方 998
    //   （gap 11）；p167 调用点差集 CLEAR 112/112（0 缺 0 多）⇒ 语义完成、byte 层
    //   封顶（§6.2）。残差=全帧槽号平移（我方帧 0x210 vs ORIG 0x1c0，N82：4.1/4.4
    //   槽复用策略差）+ 7 处虚调用 vptr 链目标寄存器（N7，本 TU 未路由 446）+
    //   PacketGuard 段循环变量槽错位。
    //   本轮修真语义 bug：空位检查极性反置——旧 `else invFull = false;` 使
    //   check 失败时不抛 4；ORIG 0x0821c8c5 `jne FAIL`→FAIL `mov $1,%eax`→
    //   `test/je` 抛出 ⇒ 失败必须保持 invFull=true。
    // P0/N96：原 `register int ret asm("ebx")` 显式绑定已清除；纯 register
    // 存储类提示（P231）合法，用于复现 ORIG 的 %ebx 返回缓存形。
    register int ret;
    if ((bool)(user->get_state() != 3))
    {
        ret = 0;
        return ret;
    }
    if (user == 0 || user->CheckInTrade())
    {
        ret = 0;
        return ret;
    }
    // ORIG 0x0821c33f：三个输入局部在 user 检查之后才清零。
    unsigned int machineId = 0;
    unsigned int slotId    = 0;
    short        pos       = -1;
    if (!packet.get_int(machineId))
        return LineFunc(0xd029, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(slotId))
        return LineFunc(0xd02a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_short(pos))
        return LineFunc(0xd02b, __PRETTY_FUNCTION__, 0, 0);

    const stVendingMachine_t* vm = _getVendingMachine(machineId);
    if (vm == 0)
        return LineFunc(0xd02f, __PRETTY_FUNCTION__, 0, 0);
    const stVendingMachineInfo_t* info = _getVendingMachineInfo(vm, slotId);
    if (info == 0)
        return LineFunc(0xd033, __PRETTY_FUNCTION__, 0, 0);

    // ORIG 0x0821c4b2：三个只写不读的整型死局部（1/1/2）照抄（P171/P232）。
    int deadA = 1, deadB = 1, deadC = 2;
    (void)deadA; (void)deadB; (void)deadC;
    // 给/赏物品数组（ORIG 0x0821c520：数组默认构造 2 次 + 逐元素写 +0x02=-1，
    // 无 Inven_Item::reset 调用；local_163 生命周期覆盖两 try 之后的 _putItemIntoUser）。
    Inven_Item give[2];
    for (int slot = 0; slot < 2; ++slot)
        give[slot].m_addInfo = -1;
    // ORIG 0x0821c4c7：find_item 结果按 slot 索引保存的 2 元素指针数组（初值 0）。
    CItem* found[2] = {0, 0};
    // ORIG 有**两个**独立 catch handler：try1[0x0821c56c..0x0821c6c9]（给物品抽取，
    // catch @0x0821c75b）、try2[0x0821c8bb..0x0821cd3e]（空位检查 + 扣款，
    // catch @0x0821ce0e），两者同体。赏物品抽取（_getBonusItem）落在两 try 之间
    // （0x0821c7b6..），故不可合并成单个 try。
    try {
        for (int slot = 0; slot < 1; ++slot)
        {
            unsigned int retry = 0;                 // -0x30
            do
            {
                if (retry > 2)
                    throw (ENUM_ERROR)1;            // ORIG 0x0821c56c
                give[slot] = _getGiveItem(info);
                if (give[slot].m_addInfo == -1)
                    throw (ENUM_ERROR)1;            // ORIG 0x0821c672
                found[slot] = G_CDataManager()->find_item(give[slot].m_addInfo);
                register bool redo = false;
                if (found[slot] != 0 && found[slot]->GetItemType() == 0x17 &&
                    GlobalData::s_globalEffectManager_->getEffectNum() != 0)
                    redo = true;
                if (redo)
                {
                    give[slot].m_addInfo = -1;
                    give[slot].m_addInfo2 = 0;
                    ++retry;
                }
            } while (give[slot].m_addInfo == -1);
        }
    } catch (ENUM_ERROR e) {
        // ORIG catch handler 0x0821c75b（e 存 -0x2c）
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xda, (unsigned char)e);
        ret = 0;
        return ret;
    }
    // ORIG 0x0821c7b6：remainingMoney 在赏物品抽取前清零（声明在此处）。
    int remainingMoney = 0;
    for (int slot = 1; slot < 2; ++slot)
        give[slot] = _getBonusItem(info);

    try {
        // 空位检查：ORIG 逐背包区段 7 次 check_empty_count(zone, 2)（0x0821c8bb..），
        // 全部 ==1 才继续查货架 GetRemainCapacity(2, 8)。
        register bool invFull = true;
        if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)1, 2) == '\x01') {
        if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)2, 2) == 1) {
        if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)3, 2) == 1) {
        if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)10, 2) == 1) {
        if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)9, 2) == 1) {
        if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)5, 2) == 1) {
        if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->check_empty_count(
                (Inven_Item::ITEM_TYPE)6, 2) == 1) {
            if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->GetRemainCapacity(
                    (INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8) > 1)
                invFull = false;
        }
        }
        }
        }
        }
        }
        }
        if (invFull)
            throw (ENUM_ERROR)4;                    // ORIG 0x0821ca32

        if (info->m_payType == 0)
        {
            if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->use_money(
                    info->m_payAmount, (eMoneySubReason)0x1c, 1) != 1)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821ca9e
            remainingMoney =
                static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->get_money();
        }
        else if (info->m_payType == 1)
        {
            if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()->getTotalCoin() <
                    (unsigned int)info->m_payAmount)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821cb16
            for (unsigned int i = 0; i < (unsigned int)info->m_payAmount; ++i)
                static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->UseCoin(
                    (eCoinSubReason)6);
            remainingMoney =
                (int)static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()->getTotalCoin();
        }
        else if (info->m_payType == 2)
        {
            if (static_cast<CUserCharacInfo*>(user)->GetWinPoint() <
                    (unsigned int)info->m_payAmount)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821cbba
            user->UseWinPoint(info->m_payAmount, (eWPSubReason)4);
            remainingMoney = static_cast<CUserCharacInfo*>(user)->GetWinPoint();
        }
        else
        {
            Inven_Item payItem =
                static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
                    ->GetInvenSlot((INVEN_TYPE)1, pos);
            if (payItem.m_addInfo != info->m_payType)
                throw (ENUM_ERROR)0x11;             // ORIG 0x0821cc59
            // [轴B] ORIG 0x0821cc6c 为有符号 `jge`（cmp %eax,%edx），而
            // m_fieldd 为 unsigned、m_payAmount 为 int ⇒ 4.4 默认产无符号
            // `jae`。按 ORIG 符号性显式转 int（P196：ORIG 该点有符号直证）。
            if ((int)payItem.m_fieldd < info->m_payAmount)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821cc95
            if (static_cast<CUserCharacInfo*>(user)->getCurCharacInvenW()->delete_item(
                    (INVEN_TYPE)1, (int)pos, info->m_payAmount,
                    (eItemDelReason)0x1c, 1) != 1)
                throw (ENUM_ERROR)0x16;             // ORIG 0x0821cd0a
            // ORIG 0x0821cd3a：第二次取槽结果整体赋回 payItem（逐字段拷贝），
            // 再从 payItem.m_fieldd 取剩余数量。
            payItem = static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
                          ->GetInvenSlot((INVEN_TYPE)1, pos);
            remainingMoney = payItem.m_fieldd;
        }
    } catch (ENUM_ERROR e) {
        // ORIG catch handler 0x0821ce0e（e 存 -0x20）
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0xda, (unsigned char)e);
        ret = 0;
        return ret;
    }

    GetInstanceVendingMachineStatistic()->incUseCount();   // ORIG 无空检查

    // 输出
    stPutItemResult_t result[2];
    for (int i = 0; i < 2; ++i)
        _putItemIntoUser(user, give[i].m_addInfo, give[i].m_addInfo2, result[i]);

    // ORIG 0x0821cee0 区：单元素循环 `for (i=0; i<1; ++i)` 内 ——
    // 失败时走 `vector::begin()` + `__normal_iterator::operator*` 取表首（不是
    // empty()/front()），再 _putItemIntoUser；随后按 (itemIdx != -1 && count != 0)
    // 触发 _notifyItemInfo。
    for (int i = 0; i < 1; ++i)
    {
        if (result[i].m_success != 1)
        {
            stVendingMachineOutput_t fallback = *info->m_vecGive.begin();
            _putItemIntoUser(user, fallback.m_itemId, fallback.m_count, result[i]);
        }
        if (result[i].m_itemIdx != 0xffffffffUL && result[i].m_count != 0)
            _notifyItemInfo(info, user, result[i]);
    }

    // 回包：header 0xda
    PacketGuard resp;
    resp.put_header(1, 0xda);
    resp.put_byte(1);
    resp.put_short(pos);
    resp.put_int(remainingMoney);
    // ORIG 0x0821d0xx：两段单元素循环（`for(i=0;i<1)` 与 `for(i=1;i<2)`）而非
    // 一个 2 次循环 —— 4.1 里这两组 call 各自展开，合并循环会少一次
    // put_item_idx/put_int。
    for (int i = 0; i < 1; ++i)
    {
        resp.put_item_idx(result[i].m_itemIdx);
        resp.put_int((int)result[i].m_count);
    }
    for (int i = 1; i < 2; ++i)
    {
        resp.put_item_idx(result[i].m_itemIdx);
        resp.put_int((int)result[i].m_count);
    }
    int idx = resp.get_index();
    int cnt = 0;
    resp.put_short(0);
    for (int i = 0; i < 2; ++i)
    {
        // ORIG 0x0821d135..0x0821d204（objdump 权威，result[0] 基址 -0xa8、步长 0x10）：
        //   +0x00 int  != -1      → m_itemIdx      (0x0821d146 cmp $0xffffffff)
        //   +0x0c WORD != 0xffff  → m_slot         (0x0821d162 movzwl 0xc;cmp $0xffff)
        //   +0x0e byte == 1       → m_success      (0x0821d181 movzbl 0xe;xor $1;test)
        //   0x0821d19c `sub $0x98` = **+0x08 m_result** → put_byte(m_result)
        //   0x0821d1d6 同 +0x08 → GetInvenTypeFromItemSpace(m_result) 作 MakeItemPacket
        //   第一实参（InvC10 实证该函数全 ORIG 仅一重载：
        //   0x084fc6bc (INVEN_TYPE,int,PacketGuard&) const）。
        if (result[i].m_itemIdx != 0xffffffffUL && result[i].m_slot != -1 &&
            result[i].m_success == 1)
        {
            resp.put_byte(result[i].m_result);                    // 0x0821d1ad
            const int invenType =
                GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)result[i].m_result);  // 0x0821d1e0
            static_cast<CUserCharacInfo*>(user)->getCurCharacInvenR()
                ->MakeItemPacket((INVEN_TYPE)invenType, (int)result[i].m_slot, resp);
            ++cnt;
        }
    }
    resp.put_short(idx, cnt);
    resp.finalize(true);
    user->Send(resp);
    APSystem::CUserProc::ClearActionAndSendtoUser(
        user, (APSystem::_ActionGroupIndex)0x27, 0, (ENUM_PACKETCLASS)0);
    return 0;
}

// ============================================================================
// ORIG 0x08233dbc W _ZN28Dispatcher_UseVendingMachine7processEP5CUserR8MSG_BASER9ParamBase
//   objdump 逐指令核实：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
//   —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x0821c2e6，本文件上方)
//   及 6 个 _get/_put/_notify 辅助方法。忠实转写空返回，勿臆造。
int Dispatcher_UseVendingMachine::process(CUser* /*user*/, MSG_BASE& /*msg*/,
                                          ParamBase& /*param*/)
{
    return 0;
}

// ORIG 0x08233db2 W _ZN28Dispatcher_UseVendingMachine4readER9PacketBufR8MSG_BASE
//   objdump 核实同为纯空 ret0（0x08233db2..0x08233dbc）；ORIG 的包读取在
//   dispatch_sig 内完成（get_int/get_int/get_short 直读 PacketBuf）。
int Dispatcher_UseVendingMachine::read(PacketBuf& /*packet*/, MSG_BASE& /*msg*/)
{
    return 0;
}