// ============================================================================
// setCreatureData —— ORIG 0x0841dc84（普通成员）：getList2inQuery(list) 拼串 →
//   查 creature_items（stat=1）→ +0xd30 条数 → 逐行回填 +0xd30+i*0x25 区。
// ============================================================================
// [b28d-solo-DB_MailBox_Req_List] verdict=AE（check_tu_routed，125=125）。
// 修法：①SIG_MAILBOX_LIST::m_creature 具名视图（stride 0x25、基址 +0xd30、
// item[0] 首 4 字节条数）复现 `mov (%edx,%eax,1)` 与 `lea; add $0xd30; mov disp`
// 形；②三个具名局部（声明序按 ORIG 槽 -0x10/-0x14/-0x18）承接 get_int 再回写；
// ③m_count 用 unsigned 触发 `mov %eax,%edx` 中转（P211/P217）。
bool DB_MailBox_Req_List::setCreatureData(MySQL* db, unsigned int characNo,
                                          unsigned int* list,
                                          SIG_MAILBOX_LIST* out)
{
    (void)characNo;
    char buf[0x1000] = { 0 };

    // [b28d-solo-DB_MailBox_Req_List] ORIG 0x0841dc84 逐条复现：三个**具名局部**
    // （ORIG -0x10/-0x14/-0x18 三槽，0x0841dca5..0x0841dcb3 三条 movl $0 实证）
    // 逐个 get_int 接收后再写入明细（ORIG 不把 &item->field 直接传给 get_int）；
    // 明细经 SIG_MAILBOX_LIST::m_creature 视图（stride 0x25、基址 +0xd30、
    // item[0] 首 4 字节为条数）。
    // 声明序按 ORIG 槽位排（4.4「先声明占浅槽」：-0x10/-0x14/-0x18）。
    int expireDate = 0;
    int regDate = 0;
    int uiId = 0;

    getList2inQuery(list, buf);
    db->set_query(
        "seLect ui_id,unix_timestamp(expire_date),unix_timestamp(reg_date) from creature_items where ui_id %s and stat = 1",
        buf);
    db->exec(true);

    out->m_creature.m_items[0].m_count = db->get_n_rows();

    unsigned int i;
    for (i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        if (!db->fetch())
            return 0;

        if (!db->get_int(0, uiId))
            return 0;
        if (!db->get_int(1, expireDate))
            return 0;
        if (!db->get_int(2, regDate))
            return 0;

        out->m_creature.m_items[i].m_uiId = uiId;
        out->m_creature.m_items[i].m_expireDate = expireDate;
        out->m_creature.m_items[i].m_regDate = regDate;
    }
    return 1;
}