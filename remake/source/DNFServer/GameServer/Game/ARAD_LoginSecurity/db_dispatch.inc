// db_dispatch @ 0x0818ef4c（0x27d 指令）
int Arad_Dispatcher_Login_Impl::db_dispatch(int code, int userIdx,
                                            Stream* stream)
{
    // [gap-2 修复·新范式 P339] ORIG 0x0818ef4c：168 vs 170、p167 CLEAR。
    // 已修：P0/P48 删 channelType/secondCheckType 具名槽改实参内联（复现 ORIG
    // `cmp $8/%7,%eax; sete %al; test %al,%al; je` 的 call 结果直比物化，P197④）；
    // pwdBuf 改 `={0}` 复现 rep stos $0x21 dwords；`==7 && (...)` 拆嵌套 if 消
    // 4.4 jump-thread（P225/P286）；补 ORIG 死局部 `int padDead = 0`（0x0818f0f5
    // `movl $0,-0x1c`，P232 帧律使帧 0xdc 与槽基全等）。残余=ORIG 保留
    // punish_user_check/pad_certifiaction_check 返回值两处死 `test %eax,%eax`
    // （ORIG 0x0818f0f3 / 0x0818f112，test 后无 jcc）——旧判词 N99「不可复现」
    // **作废**：4.4.7 对空体 `if (f()) {}`（结果丢弃）正是该形
    // （探针 /tmp/p_arad/t4.cpp c3：`call f; test %eax,%eax; <下一条>`，无 setne/
    // jcc；c1/c2 的 while 形才会多 setne）。改用空 if 即逐条复现。
    // [R2PrettAClass 2026-09-11 · prett A 类落地] 手写签名串 → __PRETTY_FUNCTION__。
    //   ORIG .rodata @0x08b98f80 size 0x4d = "virtual int ARAD::Arad_Dispatcher_Login_Impl::"
    //   "db_dispatch(int, int, Stream*)"；本函数类内声明为虚（.h:158）⇒ 宏自带 `virtual `
    //   （探针 /tmp/r2/probe.cpp 实证：类外定义不写 virtual 也打印）。定义侧拼写 =
    //   `int Arad_Dispatcher_Login_Impl::db_dispatch(int code, int userIdx, Stream* stream)`
    //   （db_dispatch.inc:1）⇒ 与 ORIG 逐字节一致。

    SIG_LOGIN_DATA* loginData = stream->GetOutBuffer<SIG_LOGIN_DATA>();
    if (loginData == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_Login_Impl.cpp", __PRETTY_FUNCTION__, 0x39,
            "%s", "recv stream buffer null!!");
        return -1;
    }

    loginData->m_errCode = 0;

    // P0/P48：ORIG 无 channelType 槽，GetChannelType 结果寄存器直比（`cmp $8,%eax;
    // sete %al; test %al,%al; je`，P197④ call 结果直比出 sete）⇒ 实参内联。
    if (G_GameWorld()->GetChannelType() == 8)
    {
        DBMgr::stUserDBInfo_t info;
        info.m_userDBIdx = 0;
        info.m_userDBType = 0;
        info.m_serverGroup =
            static_cast<int>(loginData->m_channelRestrictField);
        GlobalData::s_db_mgr->addUserDBInfo(userIdx, info);
        GlobalData::s_db_mgr->setUserIdx(userIdx);
    }

    char pwdBuf[0x84] = {0};   // ORIG rep stos $0x21 dwords

    if (DATABASE::LOGIN_QUERY::arad_sp_select_account_info(loginData, pwdBuf)
        != 0)
    {
        loginData->m_errCode = 4;
    }

    if (account_password_check(loginData, pwdBuf) != 0)
    {
        loginData->m_errCode = 1;
    }

    if (loginData->m_accountFail > 2 || loginData->m_totalAccountFail > 9)
    {
        loginData->m_errCode = 7;
    }

    if (DATABASE::LOGIN_QUERY::arad_sp_select_punish_user_info(loginData) != 0)
    {
        loginData->m_errCode = 0xf;
    }

    DATABASE::LOGIN_QUERY::arad_sp_select_punish_second_user(loginData);

    if (punish_user_check(loginData)) {}

    // ORIG 0x0818f0f5 在 punish_user_check 之后对 -0x1c 槽写 0（死局部，4.1 保留）
    int padDead = 0;
    (void)padDead;

    DATABASE::LOGIN_QUERY::arad_sp_convert_pass_pad_info(loginData);
    if (pad_certifiaction_check(loginData)) {}

    std::vector<DB_Login::tagDNFEventEntry> entries;
    DATABASE::LOGIN_QUERY::arad_sp_load_event_entry(loginData, entries);
    premium_info_set(loginData, entries, userIdx);

    // P225/P286：ORIG 第一条件单独物化（`cmp $7,%eax; sete %al; test; je`），
    // `== 7 && (...)` 合并形会被 4.4 jump-thread 成 `jne` 直跳 ⇒ 拆嵌套 if。
    if (G_GameWorld()->GetChannelType() == 7)
    {
        if (loginData->m_tournamentAccountFlag == 0 ||
            loginData->m_tournamentServerGroup == 0)
        {
            loginData->m_errCode = 8;
        }
    }

    return 0;
}