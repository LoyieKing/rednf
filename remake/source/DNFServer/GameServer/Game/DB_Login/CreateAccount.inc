// ============================================================================
// CreateAccount（ORIG T 0x08412eb8）
// 【注】ORIG 把 SIG_LOGIN_DATA* 直接当 user_id 字符串传给 %s（两处 set_query/
// 日志同证；SIG 起始即 user_id 缓冲），忠实保留。
// ============================================================================

// [b28d-solo-DB_Login] verdict=NEAR ndiff=3 gap=0（N18a：hangame_flag 实参 ORIG
// `movzbl %al,%edx` 单段直落 %edx，4.4 恒 `movzbl %al,%eax`；P267 双 cast / P268
// mask / 字段 uchar / 探针 probe_cast2 八形全败，助记符序列相同 ⇒ NEAR 封顶）。
// S216：P267 双 cast 序 / P268 mask / P226 语句表达式三形再测均不翻
// （movzbl %al,%eax vs ORIG movzbl %al,%edx）⇒ 维持 NEAR 封顶。
// [Lane26 复核] P268 mask 形 `(unsigned int)(sig->m_loginType & 0xff)` 实测反劣
// （`movsbl %al,%eax; and $0xff,%eax` 8 条 vs 原 5 条，jcc/jmp 序列 bmm 0→0 但 gap 0→-1）
// ⇒ 回退；维持 `(unsigned int)(unsigned char)` 双 cast 形。
bool DB_Login::CreateAccount(SIG_LOGIN_DATA* sig, MySQL* db)
{
    // [b28c-L9] 残差 3 = N18a：hangame_flag 实参 ORIG `movzbl %al,%edx; mov %edx,0xc(%esp)`
    // 单段直落 %edx，4.4 恒 `movzbl %al,%eax` + 独立中转（探针 P200/P226 形不可造）；
    // 助记符序列同 ⇒ NEAR 封顶。
    if (sig == 0 || db == 0)
    {
        return false;
    }
    db->set_query("call usp_create_account('%s')", sig);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x37e6, "[Taiwan, Account] Fail create account. id=%s", sig);
        return false;
    }
    register const char* logId = reinterpret_cast<const char*>(sig);   // P87：ORIG ctor 前载 %ebx 跨调用
    cMyTrace trace(__PRETTY_FUNCTION__, 0x37ea, 0);
    trace("[Taiwan, Account] Success create account. id=%s", logId);

    db->set_query(
        "seLect m_id, passwd,first_ssn, second_ssn,DATE_FORMAT(from_unixtime(reg_date),'%%Y%%m%%d%%H'),user_name,email,hangame_flag,reg_date from member_info where user_id='%s' and hangame_flag=%d",
        sig, (unsigned int)(unsigned char)sig->m_loginType);
    if (!db->exec(true))
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x3804, "[Taiwan, Account] Fail select member_info m_id=%s", sig);
        return false;
    }
    return true;
}
