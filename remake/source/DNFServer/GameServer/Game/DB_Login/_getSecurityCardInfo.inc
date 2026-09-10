// ============================================================================
// _getSecurityCardInfo（ORIG T 0x084111e2）
// ============================================================================

void DB_Login::_getSecurityCardInfo(SIG_LOGIN_DATA* sig)
{
    // P1 组件视图：ORIG 单指令 disp 读/写点（get_* 传引用处保持 SGFC add 两步形态）
    struct VSecValid { char m_pad[0x39dc]; unsigned char m_valid; };        // +0x39dc
    struct VCertFlag { char m_pad[0x39e8]; unsigned int m_certFlag; };      // +0x39e8
    struct VOccTime  { char m_pad[0x3958]; long m_occTime; };               // +0x3958（card 有效期基准）
    struct VEnvSched { char m_pad[0x37c]; int m_dailyResetHour; };          // CEnvironment +0x37c
    if (((VSecValid*)sig)->m_valid == 0)             // +0x39dc（SigTypes_D pad 区保留）
    {
        sig->m_secCardActivate = false;             // +0x39e0
        return;
    }
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect card,fail_cnt,validity_time>unix_timestamp(now()),cert_flag, cert_cnt, q_pos from member_security_card where m_id=%s and apply_flag=1",
        NumberToString(sig->m_accId, 0));
    db->exec(true);
    if (db->get_n_rows() == 0)
    {
        sig->m_secCardActivate = false;             // +0x39e0
        return;
    }
    unsigned int validity = 0;
    db->fetch();
    db->get_str(0, sig->m_secCardSerial, 0x27);                    // card → serial(+0x39ed)
    db->get_int(1, SGFC(int, sig, OFF_SEC_FAILCNT));               // fail_cnt
    db->get_uint(2, validity);                                    // validity_time>now()
    db->get_uint(3, SGFC(unsigned int, sig, OFF_SEC_CERTFLAG));    // cert_flag
    db->get_byte(4, SGFC(char, sig, OFF_SEC_CERTCNT));             // cert_cnt（ORIG 先读列 4）
    db->get_short(5, SGFC(int, sig, OFF_SEC_QPOS));                // q_pos（int 引用重载）
    db->get_str(5, SGBC(sig, OFF_SEC_QPOS_STR), 5);                // ORIG：列 5 再读 str 到 +0x3a18（长 5）
    if (validity == 1)
    {
        sig->m_secCardCertified = true;             // +0x39e1
    }
    sig->m_secCardActivate = true;                  // +0x39e0

    // cert_flag != 0 且当日已过疲劳重置点 → 清零 cancel 计数
    // [b28c-L9] ORIG 0x084113aa..0x084113f0：`bool hit = (certFlag!=0) && call(...)` 短路形——
    // 首条件 je 与 call 失败 je 共享 L_false 岛（mov $0,%eax），成功后 mov $1,%eax 汇合
    // 再 test %al（P0：值全程经 %eax 无槽往返 ⇒ 原码是表达式不是具名中间量）；
    // 两 callee-saved 由实参右→左求值定：curSec→%ebx、occTime→%esi（P52/P87）。
    if (({ ((VCertFlag*)sig)->m_certFlag != 0
           && CheckDailyScheduleTime(
                  ((VEnvSched*)G_CEnvironment())->m_dailyResetHour,
                  ((VOccTime*)sig)->m_occTime,
                  GlobalData::s_systemTime_.getCurSec()); }))
    {
        ((VCertFlag*)sig)->m_certFlag = 0;
        db->set_query("upDate member_security_card set cancel_cnt=0 where m_id=%s",
                      NumberToString(sig->m_accId, 0));
        db->exec(true);
    }
}
