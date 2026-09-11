// ==== Dispatcher_MonstercardBind::get_bind_result (ORIG 0x081d84a0 T) ====
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P197④] ORIG 0x081d84a0 orig/ours=96/95 ndiff-blocks=13
//   ORIG setcc 物化（bool 归一岛）而 4.4 折直跳/槽化，源形不可择
//   首差: movswl -0x6c(%ebp),%ebx | lea    -0x61(%ebp),%edx | mov    %ebx,0xc(%esp)
int Dispatcher_MonstercardBind::get_bind_result(
    CUser* user, short slot, ENUM_RARITY rarity,
    std::vector<std::pair<int, int> >& results)
{
    // [负结论 2026-09-11 H1Dispatchers] 试过 P183/P37 去 invenR 缓存（内联
    //   `const_cast<CInventory*>(user->getCurCharacInvenR())->GetInvenSlot(1, slot)`）：
    //   实测 ORIG 95 / ours 95→93（gap 0→2 **劣化**）⇒ 已回滚，保留具名 invenR。
    //   ORIG 0x081d84a0 区 `movswl -0x6c(%ebp),%ebx`（slot 提升保活）+ invenR 具名槽，
    //   4.1 内联后反而把 slot 落栈 ⇒ 不闭合（N-P197④ 封顶）。
    CInventory* invenR = const_cast<CInventory*>(user->getCurCharacInvenR());
    Inven_Item inven = invenR->GetInvenSlot(1, slot);
    CItem* card = G_CDataManager()->find_item(inven.m_addInfo);
    // +0x330：怪物卡绑定数据索引（CStackableItem::m_int330，ORIG mov 0x330(%eax)）。
    const int cardDataIdx =
        reinterpret_cast<const CStackableItem*>(card)->m_int330;
    STExpertJobScript* script = static_cast<STExpertJobScript*>(
        G_CDataManager()->GetExpertJobScript(1));
    stMonsterCardBindRate rate;
    // [G6·get_bind_result 负结论 2026-09-11] P198① 具名 bool 预清形（`bool ok = false;
    //   ok = …; if (!ok)`）实测 95/95 gap0 ndiff=29 → 95/98 gap−3 ndiff=45 ⇒ 劣化
    //   （4.4 对该 callee 不产 ORIG 的 movb 预清 + xor 段）⇒ 保持直接 `if (!callee())` 形。
    if (!script->GetMonsterCardBindRate(cardDataIdx, rate))
    {
        return 0;
    }
    results.clear();
    const int newRarity = calcurate_rarity(rarity, rate.m_field0);
    unsigned long range = 10000;
    const unsigned int roll = m_rand->randInt(range);
    // [MatchInvLane 2026-09-11 P225 试验·负结果，已回滚] while 形实测
    // 95/95→95/97 gap −2 ndiff 13（零改善且指令数劣化）⇒ 维持 if 形（N-P197④）。
    if (roll < rate.m_field2)
    {
        return _bind_process(BIND_TYPE_SPECIAL, (ENUM_RARITY)newRarity,
                             results);
    }
    return _bind_process(BIND_TYPE_COMMON, (ENUM_RARITY)newRarity, results);
}