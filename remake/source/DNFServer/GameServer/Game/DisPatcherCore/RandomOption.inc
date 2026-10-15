// ORIG 0x08682d3a T _ZN15cUserHistoryLog12RandomOptionERK10Inven_ItemN18randomOptionReason1TE
// （docs/class_func_reports/cUserHistoryLog/RandomOption.md + objdump 全文转写）：
// m_user->LogHistory("RandomOptionItem,%d,%02X×14,%d", item.m_addInfo,
// [b28d-29] N82/P183：ORIG 无逐字段临时缓存，4.4 新增 -0x50..-0x48 三槽（帧 0x9c vs 0xac）；语义集合全等  (ORIG 0x08682d3a insn 102->119)
//   item.m_random.m_random 的 14 字节（+0x25..+0x32）逐字节, reason)。无 m_user 空检查。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 124（ORIG 102 / ours 118），残差=N82 帧差；沿用上方 b28d-29 归因，封顶。
void cUserHistoryLog::RandomOption(const Inven_Item& item, randomOptionReason::T reason)

{
    // [FL4] P183/P157：ORIG 0x0815f6xx 每处 `mov 0xc(%ebp),%eax; movzbl 0x32(%eax),%eax`
    // —— 14 个字节全部为「item 基址 + 常量 disp」具名成员直读（0x25..0x32 =
    // Inven_Item::m_random(+0x1f)::m_random(+0x06) 的 0x0e 字节），**无** opt 指针缓存槽
    // （旧形 `const unsigned char* opt=...` 占 -0x1c 槽 + 每处 add $0x25 ⇒ +16 insn）。
    // 成员展开 = m_slots[0..2]（3×3B）+ m_gradeSeed + m_changedOption（3B）+ m_changedSlot = 1+1+1+1+1+1+1+1+1+1+1+1+1+1 字节。
    // 实参右→左求值 ⇒ ORIG 读序 0x32→0x25（m_changedSlot → m_slots[0].m_optionIndex），故书写序即源序。
    m_user->LogHistory(
        "RandomOptionItem,%d,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d",
        item.m_addInfo,
        item.m_random.m_random.m_slots[0].m_optionIndex,
        item.m_random.m_random.m_slots[0].m_firstValue,
        item.m_random.m_random.m_slots[0].m_secondValue,
        item.m_random.m_random.m_slots[1].m_optionIndex,
        item.m_random.m_random.m_slots[1].m_firstValue,
        item.m_random.m_random.m_slots[1].m_secondValue,
        item.m_random.m_random.m_slots[2].m_optionIndex,
        item.m_random.m_random.m_slots[2].m_firstValue,
        item.m_random.m_random.m_slots[2].m_secondValue,
        item.m_random.m_random.m_gradeSeed.m_value,
        item.m_random.m_random.m_changedOption.m_optionIndex,
        item.m_random.m_random.m_changedOption.m_firstValue,
        item.m_random.m_random.m_changedOption.m_secondValue,
        item.m_random.m_random.m_changedSlot.m_value,
        (int)reason);
}
