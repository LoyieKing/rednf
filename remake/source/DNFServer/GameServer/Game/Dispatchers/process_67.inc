// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d52a6 orig/ours=68/69 ndiff-blocks=7
//   帧差 sub $24 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | movl   $0x0,-0xc(%ebp)
int Dispatcher_GuildJoinList::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d52a6：state==3 且会长/副会长以外 → SendCmdErrorPacket(0x161,0x24)；
    // 否则按渠道取 server group 并发起 DB_GuildJoinList 请求。
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x161, 0x24);
        return 0;
    }

    // ORIG 0x081d5322 `movl $0x0,-0xc(%ebp)`（预清）+ 0x081d5336 `test %al,%al; je`
    // （IsIntegratedPvPBaseChannel 非零 → fall-through 取 GetServerGroup）⇒ 正极性 then。
    unsigned int serverGroup = 0;                                   // -0xc
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup = user->GetServerGroup();
    }
    else
    {
        // ORIG 0x081d534f `mov 0x378(%eax),%eax`（disp 折叠，非 add 形）
        struct EnvView { char m_pad[0x378]; unsigned int m_group; };
        serverGroup = reinterpret_cast<EnvView*>(G_CEnvironment())->m_group;
    }

    // ORIG 0x081d5363 `mov %eax,%ebx`（guildkey 先算、跨 GetUID call 保活）+ 0x081d5370
    // 直接 makeRequest ⇒ 单语句内联实参（P52 右→左），禁具名 guildKey/uid 局部。
    DB_GuildJoinList::makeRequest((int)user->GetUID(),
                                  (unsigned int)user->get_charac_guildkey(),
                                  (ENUM_SERVER_GROUP)serverGroup);
    return 0;
}