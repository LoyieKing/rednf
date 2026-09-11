// ---- BuyPrivateStoreItem（ORIG T 0x85c7e74，报告 BuyPrivateStoreItem.md）----
// ISeller 槽 +0x18 / IBuyer 槽 +0x00。买家购买：校验围观/槽位/忙碌与物品可售性，
// 计价后经 BuyItem 扣买家入包、SellItem 结算卖家金币（扣手续费），广播条目变化，
// 反外挂检测（金额阈值公式常量 0.01/0.7，ORIG rodata 0x8cbead0/0x8cbead8），
// 最后写入流通统计。
// [b28d-24] DIFF 残177: 帧/槽分配 + 分支块序（N82/P112）。ORIG 0x085c7e74
// [b28d-solo-CPrivateStoreMgr] P167 归零。残 335 行 = 帧 0xbc vs 0xe0（N82 槽复用
// 差异）+ 块序/调用序（CErrorHandler::Do / GetAttachType / getCurCharacInvenR
// 的发射位置）。
// [L14] 本轮复核：602/602，13 块/129 行 = ORIG 有 `push %edi` 而我方无 + 帧
// 0xbc vs 0xc0 + 槽整块差 4（`movl $0,-0x38` vs `-0x3c` 全链）⇒ N82/N115 封顶。
// [K2 2026-09-11 复验] 定位到根因：`push %edi` 缺失是**唯一实体差**——ORIG
// 0x085c83e7 `mov -0x28(%ebp),%edi; mov %edi,0x1c(%esp)`（vararg 装载顺手用
// callee-saved %edi）；其后本体逐条同形（fn_diff 的交替显示只是错位）。
// 帧 0xbc↔0xc0 与全部 8 组 +4 slot-pair 均由该 push 派生 ⇒ N7 寄存器选择
// （该值不跨 call，4.4 无保 edi 需求；诱导 4.4 升格无合法源形）⇒ 维持封顶。
bool CPrivateStore::BuyPrivateStoreItem(int slot, int count)
{
    m_errorHandler.Set(m_pBuyer, 1, 0x55);

    if (m_pBuyer == 0)
        return false;
    if (m_pParent == 0)
    {
        m_errorHandler.Do(0x3a);
        return false;
    }
    if (!IsExistBuyerList(m_pBuyer))
    {
        m_errorHandler.Do(0x15);
        return false;
    }
    // [b28d-solo-CPrivateStoreMgr] P225：ORIG 0x085c8804 `test %eax,%eax; sete %al;
    // test; je`（寄存器比零物化）⇒ while(==0) 形复现。
    while (m_pBuyer->getCurCharacR() == 0)
    {
        m_errorHandler.Do(1);
        return false;
    }
    if (slot < 0 || 0xd < slot)
    {
        m_errorHandler.Do(0x39);
        return false;
    }
    if (m_pParent->IsBusyPrivateStore(m_pBuyer))
    {
        m_errorHandler.Do(0x3c);
        return false;
    }

    CItem* itemInfo = 0;
    PrivateStoreItem& storeItem = m_items[slot];
    itemInfo = G_CDataManager()->find_item((int)storeItem.m_item.m_addInfo);
    // ORIG 0x085c895e `cmpl $0,-0x1c; sete %al; test; je` ⇒ while 形。
    while (itemInfo == 0)
    {
        m_errorHandler.Do(0x11);
        return false;
    }

    int attachType = itemInfo->GetAttachType();
    if (attachType != 0)
    {
        if (attachType == 3)
        {
            if ((int)storeItem.m_item.m_field0 != 1)
            {
                m_errorHandler.Do(0x11);
                return false;
            }
        }
        else
        {
            // [b28d-CPrivateStoreMgr] 同 Verify：ORIG 正条件嵌套（==7 then 直落 /
            // else Do 块），负条件早返在 4.4 反极性（P59/P222）。
            if (attachType == 7)
            {
                // 绑定类：无剩余交易次数时静默拒绝。
                if (!storeItem.m_item.GetTradeLimitCount())
                    return false;
            }
            else
            {
                m_errorHandler.Do(0x11);
                return false;
            }
        }
    }
    // 生物类道具禁止私商交易。
    if (IsCreatureItemType((unsigned int)(unsigned char)storeItem.m_item.m_field1))
    {
        m_errorHandler.Do(0x11);
        return false;
    }

    // [b28d-CPrivateStoreMgr] P0/P88：ORIG 0x085c8261 段直接 lea 三个出参槽
    // （无前置 movl $0x0）⇒ 原码是裸声明（出参由 GetItemPrice 写）。
    int price;
    unsigned int dealGoldA;
    unsigned int dealGoldB;
    // [b28d-solo-L43] 声明序反转实测无效（4.4 按首次使用序分配：0x18=dealGoldB
    // 恒最深 -0x28；ORIG 0x18=-0x24/0x10=-0x28 的置换不可由声明序翻动）⇒ 保持原序。
    if (!GetItemPrice(itemInfo, storeItem, count, price, dealGoldA, dealGoldB))
        return false;
    if (!IsPossibleDeal(price, dealGoldA, dealGoldB))
        return false;
    // 非可信用户受每日交易金币上限约束。
    // [BigStore REAL-BUG] ORIG 0x085c8186 实参 = GetItemPrice out1 槽 -0x44（=price，
    // 0x085c80f6 lea -0x44 → esp+0x10 实证），旧写成 dealGoldA（out2 恒 0）⇒ 上限
    // 校验恒见 0 而被短路。按 ORIG 传成交金额。
    // [b28d-CPrivateStoreMgr] P133/P225 推广：两条件各自独立守卫（同 Verify）。
    if (!m_pBuyer->GetReliablePerson())
    {
        if (!m_pBuyer->IsAvailableCurCharacTradeGoldDaily((unsigned int)price))
        {
            m_errorHandler.Do(0x46);
            return false;
        }
    }

    Inven_Item copyItem;
    copyItem.setCopy(storeItem.m_item);
    int useCount = count;                       // ORIG local_8e
    int buyResult = -1;                          // ORIG local_2c
    int outSlot = 0;                             // ORIG local_4c
    if ((int)copyItem.m_field1 == 8)             // avatar：数量实参替换为 avatar uid
        useCount = (int)storeItem.m_item.m_addInfo2;

    if (IsTradeLimitAttachTypeItem(copyItem))
    {
        DecreaseTradeLimitCount(copyItem);
        m_pSeller->m_historyLog.ChangeItemLimitCount(-1, -1,
                                                     (int)copyItem.m_addInfo, -1, 1);
    }

    // [BigStore 忠实性] ORIG 0x085c8261-0x085c8298：BuyItem 三 int 实参 =
    // (-0x44, -0x3c, -0x40) = (price, out2, out3)；第 5 参在 ORIG 体内不读，
    // 旧写 useCount 无行为差，按 ORIG 数据流改传 out3。
    buyResult = BuyItem(slot, copyItem, price, (int)dealGoldA, (int)dealGoldB, outSlot);
    // [b28d-CPrivateStoreMgr] ORIG 0x085c82a3 `shr $0x1f,%eax; test %al,%al;
    // je <主流程>`（符号位==0 → 继续）⇒ 源码是 fail 前置守卫
    // `if ((unsigned int)buyResult >> 0x1f) { Do(4); return false; }`；
    // 4.4 的 `if (buyResult >= 0)` 产 `js <fail>` 反块序（P59/P196）。
    if ((unsigned int)buyResult >> 0x1f)
    {
        m_errorHandler.Do(4);
        return false;
    }
    int commission = GetCommissionPercent() * price / 100;
    SellItem(slot, copyItem, price, commission, useCount,
             (int)storeItem.m_item.m_addInfo2);
    SubtractItemCount(itemInfo, storeItem, count);
    SavePrivateStore();
    m_pSeller->UpdateData();
    m_pBuyer->UpdateData();

    CPacketBufPrivate packet;
    packet.MakeNotipacketPrivateStoreItem(slot, count);
    SendUsers(&packet, 0, 0);

    packet.MakeNotipacketBuyPrivateStoreItem(
        m_pSeller, (int)copyItem.m_addInfo, slot, useCount,
        m_pBuyer->getCurCharacName(), price, commission);
    m_pSeller->Send(packet);

    packet.MakeCmdpacketBuyPrivateStoreItem(m_pBuyer, copyItem,
                                            (int)dealGoldB, (int)dealGoldA,
                                            buyResult, useCount);
    m_pBuyer->Send(packet);

    unsigned char flagByte = 0;              // ORIG local_27
    if (!itemInfo->IsAvatarItem() &&
        (int)itemInfo->get_rarity() <= 1)
    {
        flagByte = 1;
    }
    // CUser::IncrementTradeGold(uint,int) ORIG _ZN5CUser18IncrementTradeGoldEji @0x0864658a
    // （定义在 CUserOnlyOrig.cpp；CUser.h 两参声明，直接调用与 ORIG 字节一致。）
    m_pBuyer->IncrementTradeGold((unsigned int)price, (int)flagByte);

    unsigned short usedFatigue =
        (unsigned short)m_pBuyer->getCurCharacUsedFatigue();   // ORIG local_26
    int rand25 = get_rand_int(0x19);         // ORIG local_24
    int rand20 = get_rand_int(0x14);         // ORIG local_20
    // [BigStore P329] ORIG 0x085c8519：getCurCharacInvenR 结果 eax 于
    // 0x085c851e 直落 get_money (%esp) 实参槽，无具名缓存 ⇒ 链式内联。
    // 反外挂：低价倾销检测。ORIG 浮点常量 [0x8cbead0]=0x3f847ae147ae147b(0.01)、
    // [0x8cbead8]=0x3fe6666666666666(0.7)（均 double）。
    // [BigStore REAL-BUG] ORIG 0x085c8526 `mov -0x44,%edx; add %edx,%eax` 与
    // 0x085c8558 `mov -0x44,%eax` 实证两处用 out1(=price)，旧写 dealGoldA
    //（out2 恒 0）⇒ `money*scale < 0` 永假，倾销检测整体失效。
    // [BigStore P253-x87] ORIG 0x085c8542 `fstps -0x9c; flds -0x9c` 为 rand25×0.01
    // 后的 float 窄化回展 ⇒ 按 (float) 显式复现；比较在 double 栈上直做。
    double hackScale =
        (double)(float)((double)rand25 * 0.01) + 0.7;
    if (hackScale * (double)((int)m_pBuyer->getCurCharacInvenR()->get_money() + price) <
        (double)price)
    {
        // [b28d-G11 试形·已回滚] 曾按 P227/P0 删本具名量、改三处 SetETC 为实参
        // 内联表达式（ORIG 三处均 `mov %eax,%esi` 保活、无 -0xc 槽）：fresh 读数
        // ours 603→599、行级残差 255→249 均改善，但 **TU identical 172→171**
        // （另有一件 identical→AE）⇒ 违反「identical 不得下降」硬约束，A/B 复核
        // （恢复具名即 identical 回 172）确认因果后回滚。
        unsigned int etcResult = 0;
        etcResult = m_pBuyer->SetETC(0x2c0, m_pSeller->get_acc_id());
        m_pBuyer->getHackAnalyzer()->addServerHackCnt(
            m_pBuyer, (WongWork::ENUM_HACKTYPE)0x2c0, 1u, etcResult, 0u);
        if (rand20 + 0x78 <= (int)usedFatigue)
        {
            etcResult = m_pBuyer->SetETC(0x2bd, m_pSeller->get_acc_id());
            m_pBuyer->getHackAnalyzer()->addServerHackCnt(
                m_pBuyer, (WongWork::ENUM_HACKTYPE)0x2bd, 1u, etcResult, 0u);
        }
        etcResult = m_pSeller->SetETC(0x2c1, m_pBuyer->get_acc_id());
        m_pSeller->getHackAnalyzer()->addServerHackCnt(
            m_pSeller, (WongWork::ENUM_HACKTYPE)0x2c1, 1u, etcResult, 0u);
    }

    GetInstanceValueStatistic()->AddValueStatistic(
        (VALUE_STATISTIC_FIELD)0x12, m_pSeller, (unsigned int)commission);
    GetInstanceCirculationStatistic()->AddCirculationStatistic(
        (CIRCULATION_STATISTIC_FIELD)0x15, m_pSeller, (unsigned int)commission);
    return true;
}