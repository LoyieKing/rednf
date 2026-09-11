// ---- updateHumanCertify(mId, accountFail)  ORIG 0x08603226
bool WongWork::CSimpleSSO::updateHumanCertify(unsigned int mId, int accountFail)
{
    char tableName[0x14];
    memset(tableName, 0, 0x14);                // ORIG call memset
    memcpy(tableName, "member_login", 0xd);    // ORIG call memcpy
    // [b28d-D13] ORIG 0x0860326c cmpl+setne %al+movzbl %al,%eax+mov %eax,-0xc
    // ⇒ 源码为 `int x = (accountFail != 0);`（非三目）。ORIG 0x08603290
    // NumberToString 结果经 %eax 直入 0x14(%esp)（无 idStr 槽）⇒ 内联实参（P0）。
    int totalAccountFailInc = (accountFail != 0);
    m_mysql->set_query(
        "upDate %s set last_play_time=unix_timestamp(now()),account_fail=%d,"
        "total_account_fail=total_account_fail+%d where m_id=%s",
        tableName, accountFail, totalAccountFailInc, NumberToString(mId, 0));
    if (!m_mysql->exec(true))
        return false;
    if (m_mysql->getAffectedRowCount() == 0)
        return false;
    return true;
}