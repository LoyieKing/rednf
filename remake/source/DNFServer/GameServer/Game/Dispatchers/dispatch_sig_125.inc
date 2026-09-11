// b28d-solo-Dispatchers verdict=DIFF(routed) [N89] ORIG 0x081ffcc2 orig/ours=287/293 ndiff-blocks=30
//   ORIG 段内 std::/__gnu_cxx out-of-line 内联实例 call 9 个，4.4 libstdc++ 全内联不可复现
//   首差: push   %edi | sub    $0x3c,%esp | movl   $&_ZZN31Dispatcher_CallGuildCreateRight12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
int Dispatcher_CallGuildCreateRight::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ffcc2..0x08200125 (0x464 字节)。packet 未读取（ORIG 不消费包）。

    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
        return LineFunc(0x93f4, __PRETTY_FUNCTION__, 0, 0);

    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 0x1c))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0xd1);
        return 0;
    }

    const unsigned int secuErr =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)6);
    if (secuErr != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, (unsigned char)(secuErr & 0xff));
        return 0;
    }

    // 公会名（getCreateGuildName 返回 char*，构造局部 std::string）
    std::string guildName(user->getCreateGuildName());

    if (!restrict_inputting_name::isUtf8StrInUnicodeRange(guildName.c_str()))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x9f);
        return 0;
    }
    if (CSyncSlangFilter::GetInstance()->HasSlangName(guildName))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x9f);
        return 0;
    }

    // [G1Csig] 实测复核（父泳道 23:51 重编 .o）：我方多 `movb $0x0,-0x21(%ebp)` 与
    // `movb $0x1,-0x21/%al` 物化（diff our[116]/our[123:126]）⇒ 具名 bool badName 占 1 槽、
    // 余下槽整体下移 4（帧 0x3c→0x40）；ORIG 为 `jne` 直短路 + %al 常量岛（无槽）⇒ 去具名。
    if (CSyncSlangFilter::GetInstance()->HasSlang(guildName) ||
        G_CDataManager()->hasSpecialCharacter(user->getCreateGuildName()))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x9f);
        return 0;
    }

    if (user->get_charac_guildkey() != 0 || user->get_guild_create_right() == 2)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x20);
        return 0;
    }

    if (!user->isSetGuildCreateFlag(2))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x6d);
        return 0;
    }
    if (!user->isSetGuildCreateFlag(4))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x71);
        return 0;
    }

    const int money = user->getCurCharacInvenR()->get_money();
    const int cost = G_CDataManager()->m_guildParameter.m_field0;   // ORIG dm+0xa50c
    // [G3 2026-09-11 候选-1] 对偶式：ORIG 0x08200000 段 `mov -0x1c(%ebp),%eax;
    //   cmp -0x20(%ebp),%eax; jle <跳过 error>`（cost 先装 eax）。
    if (cost > money)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x47, 0x1f);
        return 0;
    }

    user->getCurCharacInvenW()->use_money(cost, (eMoneySubReason)0xb, true);
    send_packet_guildcreateright(user, 1);
    user->set_guild_create_right(1);
    user->UpdateData();

    // [G1Csig · P0 槽律] ORIG 0x081ffcc2 段直证：`mov %eax,%edi/%esi/%ebx` —— 三个实参
    // 全程寄存器保存（guildAddr/name/characNo 各有 4B 栈槽往返是我方增帧主因，
    // 帧 0x3c→0x40、丢 `push %edi`）；group/proxy 亦无槽（`mov <A>,%edx; mov %edx,(%esp);
    // call GetServerProxy` 直传）⇒ 全部去具名改链式内联（同 dispatch_sig_146 手法）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendGuildCreate((unsigned int)user->getCurCharacNo(),
                          const_cast<char*>(user->getCreateGuildName()),
                          const_cast<char*>(user->getCreateGuildAddr()));

    user->resetGuildCreateFlag();
    user->resetTempGuildNameAddr();
    return 0;
}