// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d4970 orig/ours=127/120 ndiff-blocks=20
//   帧差 sub $3c vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | sub    $0x3c,%esp
int Dispatcher_DonateGuildFund::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    // [round5 A01] ORIG 0x081d4979：msg 引用(-0x20)是**函数第一句**；随后 0x081d498d
    //   `setle`(=state<=2 则 `mov$0;jmp exit`) ⇒ 负极性 guard，禁止写成 `if(state>2){…}`。
    MSG_DONATE_GUILD_FUND& donateMsg =
        reinterpret_cast<MSG_DONATE_GUILD_FUND&>(msg);
    if (user->get_state() <= 2)
    {
        return 0;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0xa);
    if (secu != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b,
                                 (unsigned char)(secu & 0xff));
        return 0;
    }
    // ORIG 0x081d49f7 `test %eax; sete` ⇒ guildkey==0 则 return 0（负极性）。
    if (user->get_charac_guildkey() == 0)
    {
        return 0;
    }
    // [G6-99] ORIG 0x081d4a08 段：先算 LHS（donateMsg.m_donateMoney → %ebx）后 call
    //   getCurCharacMoney（%eax），`cmp %eax,%ebx; setg` ⇒ 源序 = m_donateMoney > money
    //   （旧译 money < m_donateMoney 产 cmp %edx,%eax; setl 镜像形，语义等价、形不同）。
    if (donateMsg.m_donateMoney > user->getCurCharacMoney())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b, 0xa);
        return 0;
    }
    if (!(user->IsAddableGuildFund(donateMsg.m_donateMoney) == 1))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b, 0x5f);
        return 0;
    }
    if (!(user->getCurCharacInvenW()->use_money(
              donateMsg.m_donateMoney, (eMoneySubReason)0x2a, true) == 1))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15b, 0xa);
        return 0;
    }
    // ORIG 0x081d4b23：四实参右到左（donateMoney→guildKey→characNo→receiver 链），
    //   全程无具名局部（无对应槽）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendAddGuildFund((unsigned int)user->get_charac_no(-1),
                           (unsigned int)user->get_charac_guildkey(),
                           donateMsg.m_donateMoney,
                           (Packet_Add_Guild_Fund::eFundType)1);
    return 0;
}