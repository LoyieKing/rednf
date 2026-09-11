int DB_Login::GetLoginCommonData(SIG_LOGIN_DATA* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_9, E_SERVER_GROUP_NONE);
    db->set_query(
        "seLect  member_bonus_fatigue , update_time , login_ip , inform_notice  from login_common where m_id=%s",
        NumberToString(sig->m_accId, 0));
    if (!db->exec(true))
    {
        return 0;
    }
    // [b28d-solo-DB_Login] P0：ORIG 有 rows 具名槽（-0x10，`cmpl $0,-0x10`）；
    // 列号用递增计数器 col（-0xc，`mov -0xc,%eax; addl $1,-0xc` = col++）。
    int rows = db->get_n_rows();
    if (rows == 0)
    {
        sig->m_memberBonusFatigueInc = 0;       // +0x3f5e
        // [b28d-38] SGB() 函数形 off 为运行期参数 ⇒ -O0 物化 `movl $imm,槽; add 槽`
        // 3 条/处；ORIG 为单条 `add $imm,%eax` ⇒ 改宏形 SGBC（off 字面量折叠，P0 右值直传）。
        memset(SGBC(sig, 0x4114), 0, 0x10);     // login_ip（SigTypes_D 隐式区）
        memset(SGBC(sig, 0x4125), 0, 0x10);     // inform_notice
        return 1;
    }
    if (!db->fetch())
    {
        return 0;
    }
    int col = 0;
    int bonusFatigue = 0;
    if (!db->get_int(col++, bonusFatigue))
    {
        return 0;
    }
    int updateTime = 0;
    if (!db->get_int(col++, updateTime))
    {
        return 0;
    }
    sig->m_memberBonusSaveTime = (unsigned int)updateTime;  // +0x3f60 ORIG local_20[0]
    sig->m_memberBonusFatigueInc = (short)bonusFatigue;     // +0x3f5e
    if (!db->get_str(col++, SGBC(sig, 0x4114), 0x10))   // 第 3 列续接索引读取 login_ip
    {
        return 0;
    }
    return 1;
}