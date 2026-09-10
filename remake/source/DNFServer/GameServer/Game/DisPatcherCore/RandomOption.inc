void cUserHistoryLog::RandomOption(const Inven_Item& item, randomOptionReason::T reason)

{
    // [FL4] P183/P157：ORIG 0x0815f6xx 每处 `mov 0xc(%ebp),%eax; movzbl 0x32(%eax),%eax`
    // —— 14 个字节全部为「item 基址 + 常量 disp」具名成员直读（0x25..0x32 =
    // Inven_Item::m_random(+0x1f)::m_random(+0x06) 的 0x0e 字节），**无** opt 指针缓存槽
    // （旧形 `const unsigned char* opt=...` 占 -0x1c 槽 + 每处 add $0x25 ⇒ +16 insn）。
    // 成员展开 = m_fields[0..2]（3×3B）+ m_seed9 + m_fielda（3B）+ m_seedd = 1+1+1+1+1+1+1+1+1+1+1+1+1+1 字节。
    // 实参右→左求值 ⇒ ORIG 读序 0x32→0x25（m_seedd → m_fields[0].m_field0），故书写序即源序。
    m_user->LogHistory(
        "RandomOptionItem,%d,%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d",
        item.m_addInfo,
        item.m_random.m_random.m_fields[0].m_field0,
        item.m_random.m_random.m_fields[0].m_field1,
        item.m_random.m_random.m_fields[0].m_field2,
        item.m_random.m_random.m_fields[1].m_field0,
        item.m_random.m_random.m_fields[1].m_field1,
        item.m_random.m_random.m_fields[1].m_field2,
        item.m_random.m_random.m_fields[2].m_field0,
        item.m_random.m_random.m_fields[2].m_field1,
        item.m_random.m_random.m_fields[2].m_field2,
        item.m_random.m_random.m_seed9.m_field,
        item.m_random.m_random.m_fielda.m_field0,
        item.m_random.m_random.m_fielda.m_field1,
        item.m_random.m_random.m_fielda.m_field2,
        item.m_random.m_random.m_seedd.m_field,
        (int)reason);
}
