// ---- ORIG T 0x081333b2（resetStar.md 全文）：非 0 类型→返 2；0 型扣重置费
// （费用表按 m_resetCount 查，miss 回退 getMaxStarResetGoldFee）→ 不足返 3 →
// use_money(fee,0x32,1) 失败返 3 → SendUpdateItemList(1,0,0) → ++m_resetCount
// → setDefaultSlot + 三商店 default → 遍历 stages（m_field4!=-1）三商店重放
// unlock → 星池差值入账 + HistoryLog::starAdd。----
// [b28d-solo-advancealtar_Manager N 封顶] 本次修：type 守卫正向包裹（P222/P257）、
// 缓存 &m_starRestMap（ORIG `add $0x6334`=0x631c+0x18，非 &m_altarShop）、具名
// int key（ORIG -0x30）、`== end` 分支前置（ORIG `eq;test;je`）、去 invenR/invenW
// 具名指针（内联）、fee 声明移到 find 后（ORIG `movl $0,-0xc` 位置）、logType
// 反极性 if/else + unsigned 比较（ORIG `mov 0x10(%ebp),%eax; cmp $1,%eax; je`）、
// feeMap const 视图 + const_iterator（p167：ORIG const find/end/eq/ptEv）⇒
// 234=234 条 + p167 CLEAR。残差 = 块级槽逆序（fee/feeMap/logType/delta，
// N93/N98）+ return 2 块位置（ORIG 中间 +0x1c3 vs 我方函数尾，N93附 尾部双
// return 块序）⇒ DIFF 封顶，语义等价。
// [R22 2026-09-10 复测] 235=235，残差 13 行全为 feeMap/fee/logType 三变量槽位
// **完全倒置**（ORIG logType@-0x18,feeMap@-0x10,fee@-0xc vs 我方 feeMap@-0x18,
// fee@-0x10,logType@-0xc）；实测「裸声明前置（赋值留原位）」两形（logType 单独/
// logType+fee 同时）4.4 产物**零变化** ⇒ 4.4 按首次使用序分配、混合区声明序不可翻
// （P112/P331 限定），维持 N 封顶。
unsigned int CharacAdvanceAltarManager::resetStar(CUser* user, CMDPacketStruct::ResetStarType::T type)
// [P8S4 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
{
    if (user == 0 || m_currentAdvanceAltarData == 0)
    {
        return 2;
    }
    // [L4d 2026-09-11] ORIG 0x081333d5 `jne 08133575`：返回 2 的块在**函数中部**
    //   （A 段 setup 之后、循环体 0x0813357f 之前）= else 块位置 ⇒ 原码结构为
    //   `if (type == 0) { A段 } else { return 2; }`，循环与尾段在 if/else 之外
    //   （语义等价：type!=0 走 else 早退，循环不可达）。
    if (type == static_cast<CMDPacketStruct::ResetStarType::T>(0))
    {
    // ORIG +0x14 `call G_CDataManager; add $0x6334,%eax; mov %eax,-0x10` ⇒ 缓存
    // &m_altarShop.m_starRestMap（0x631c+0x18），非 &m_altarShop 本体。
    // ORIG p167 用 const 版 find/end/eq/ptEv（_ZNKSt3map...4findERS5_ /
    // _ZNKSt23_Rb_tree_const_iterator...）⇒ const map 视图 + const_iterator。
    const std::map<int, _StarRestFeeByGold>& feeMap =
        G_CDataManager()->m_altarShop.m_starRestMap;
    // ORIG +0x1c `movzwl 0x1a(%eax),%eax; cwtl; mov %eax,-0x30` ⇒ 具名 int key 局部。
    const int resetCount =
        static_cast<int>(m_currentAdvanceAltarData->m_resetCount);
    std::map<int, _StarRestFeeByGold>::const_iterator feeIt = feeMap.find(resetCount);
    // ORIG +0x30 `movl $0x0,-0xc(%ebp)` 在 find 之后、end 之前 ⇒ fee 声明于此处。
    int fee = 0;
    // ORIG +0x40 `eq; test; je <else 块>` ⇒ `== end` 分支在前（先取默认费用）。
    if (feeIt == feeMap.end())
    {
        fee = G_CDataManager()->m_altarShop.getMaxStarResetGoldFee();
    }
    else
    {
        fee = feeIt->second.m_field0;                       // pair 视图 *(feeIter+4)=second.m_field0
    }

    // ORIG +0xb6 内联 `getCurCharacInvenR()->get_money()`（无具名指针槽，P183）。
    if (user->getCurCharacInvenR()->get_money() < fee)
    {
        return 3;
    }
    // ORIG +0xd0 内联 `getCurCharacInvenW()->use_money(...)`（无具名指针槽）。
    if (!user->getCurCharacInvenW()->use_money(fee, static_cast<eMoneySubReason>(0x32), true))
    {
        return 3;
    }
    user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                             static_cast<ENUM_ITEMSPACE>(0), 0);

    ++m_currentAdvanceAltarData->m_resetCount;              // ORIG short 自增
    setDefaultSlot();
    setDefaultBuyItem(AdvanceAltarShopType::T_0);
    setDefaultBuyItem(AdvanceAltarShopType::T_1);
    setDefaultBuyItem(AdvanceAltarShopType::T_2);
    }
    else
    {
        return 2;
    }

    for (std::map<int, _Stage>::const_iterator it = m_currentAdvanceAltarData->m_stages.begin();
         it != m_currentAdvanceAltarData->m_stages.end(); it++)
    {
        if (it->second.m_field4 == -1)
        {
            continue;
        }
        setUnlockBuyItem(AdvanceAltarShopType::T_0, it->second.m_field0, 0);
        setUnlockBuyItem(AdvanceAltarShopType::T_1, it->second.m_field0, 0);
        setUnlockBuyItem(AdvanceAltarShopType::T_2, it->second.m_field0, 0);
    }

    const int delta = (m_currentAdvanceAltarData->m_curStar +
                       m_currentAdvanceAltarData->m_starPoolBase) -
                      m_currentAdvanceAltarData->m_starPoint;
    m_currentAdvanceAltarData->m_starPoint = m_currentAdvanceAltarData->m_curStar +
                                             m_currentAdvanceAltarData->m_starPoolBase;
    m_currentAdvanceAltarData->m_update = 1;

    // ORIG +0x320 `movl $0x2,-0x14; cmp $1,%eax; je; movl $0x2,-0x14; jmp;
    // movl $0x3,-0x14` ⇒ 初值 + if/else 双写（P305：窄枚举经 int 比较，
    // 否则 -fstrict-enums 静默删分支）。
    advancealtar::AddStarType::T logType = advancealtar::AddStarType::ADD_BY_RESET;
    // P286：形参载入+寄存器比较链；P222 反极性使 then(=$3) 块后置、else 直落
    // （ORIG +0x32d `je <movl $3>`）。
    if (static_cast<unsigned int>(type) != 1u)
    {
        logType = advancealtar::AddStarType::ADD_BY_RESET;
    }
    else
    {
        logType = static_cast<advancealtar::AddStarType::T>(3);
    }
    if (delta > 0)
    {
        HistoryLog::starAdd(*user, logType, 0, delta);
    }
    return 0;
}