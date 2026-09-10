int DisPatcher_UseEquip::check_error(CUser* user, MSG_BASE& msg)

{
    MSG_DECREASE_DURABILITY* req = reinterpret_cast<MSG_DECREASE_DURABILITY*>(&msg);  // ORIG 0x81c0ee8 入口 mov 0x10→-0x14，其后不读（dead）
    (void)req;
    // ORIG 0x81c0ef4..0x81c0f4c：各 clause `call get_state; cmp $k; je false_lbl`，
    // 全不匹配 fall→mov $1；merge `test %al,%al; je` 非零 → mov $0x13。&& 值物化形。
    if (user->get_state() != 5 && user->get_state() != 7 &&
        user->get_state() != 8 && user->get_state() != 0xa &&
        user->get_state() != 0xc)
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0)
    {
        if (party->m_battleField.m_bloodState == 1)                    // +0xcd8
        {
            return 0x13;
        }
        CDungeon* dungeon = party->m_battleField.m_pDungeon;           // +0xcac（ORIG -0xc）
        if (dungeon != 0 && dungeon->m_bloodDungeonType != 0)  // +0x89f
        {
            return 0x13;
        }
    }
    return 0;
// [b28d-29] AE：正文 21=21 全等，仅 .o 重定位/字符串地址显示差（跨二进制布局产物）  (ORIG 0x081c0dfa insn 21->22)
}
