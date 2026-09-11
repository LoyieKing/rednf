// ============================================================================
// DB_AvatarExtendPeriod::ExtendPeriod —— ORIG 0x083ffbb4（报告 ExtendPeriod.md）
// ============================================================================
// GetDBHandle(DB_HANDLE_3,0) → set_query("upDate user_items set expire_date =
// adddate(expire_date, interval %d day) where charac_no=%u and slot=%d",
// days(+0x05 ushort), charac_no(+0x00), slot(+0x04 byte))；exec(true) 失败
// 返 0，成功返 1（P101 分支形态）。
// ============================================================================
bool DB_AvatarExtendPeriod::ExtendPeriod(SIG_AVATAR_RENEW& renew)
{
    // ORIG 栈槽：ok(-0xd) db(-0xc)——bool 先声明（低地址），db 后。
    bool ok;      // -0xd（exec 结果落槽，P102）
    MySQL* db;    // -0xc
    // [C-3/2026-09-11] ORIG 实有函数局部 static
    // `_ZZN21DB_AvatarExtendPeriod12ExtendPeriodER16SIG_AVATAR_RENEWE9handleIdx`
    // （.rodata 0x08c5da4c size 4，实测值 0x3；使用点 0x083ffbc7 折叠为
    //  `movl $0x3,0x4(%esp)`）⇒ 恢复声明（零 codegen 变化）。
    static const int handleIdx = 3;
    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                           E_SERVER_GROUP_NONE);
    // ORIG 0x83ffbb4：set_query("upDate user_items set expire_date =
    // adddate(expire_date, interval %d day) where charac_no=%u and slot=%d",
    // days, charac_no, slot) 后 exec(true)
    db->set_query(
        "upDate user_items set expire_date = adddate(expire_date, "
        "interval %d day) where charac_no=%u and slot=%d",
        renew.m_days, renew.m_characNo, renew.m_slot);
    ok = db->exec(true);
    if (ok != 1)
        return 0;
    return 1;
}