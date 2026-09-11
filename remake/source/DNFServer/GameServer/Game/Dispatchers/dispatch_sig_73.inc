// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x082001f0 orig/ours=161/173 ndiff-blocks=29
//   帧差 sub $6c vs $88 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | push   %ebx
int Dispatcher_CallGuildLevelUp::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 08200253 `movl $0x0,-0x28(%ebp)` 位于 isRestricted 门**之后** ⇒
    // local_2c 的声明/初始化随使用点下移（非函数入口）。
    unsigned int local_24;               // -0x20：CheckGuildLevelUp 结果
    int local_28;                        // -0x24：Secu Check
    char local_2f;                       // -0x2b：临时 level
    unsigned char local_2e;              // -0x2a：临时 powerSide
    char cVar2;
    bool bVar1;

    // ORIG 082001f0 段：`call get_state; cmp $0x2,%eax; jle`（值留 %eax，无槽往返）
    // ⇒ 表达式直用（P0）；原 iVar3 具名量多 store+load 并撑帧。
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        // [DispA2 语义修复·返回值直通] ORIG 0x08200247 `call LineFunc(0x947f)`
        //   后紧接 0x0820024c `jmp 08200440 <+0x250>`（尾 epilogue），**无**
        //   `mov $0x0,%eax` ⇒ 原码是 `return LineFunc(...)`。
        return LineFunc(0x947f, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0820025f 段：`call isRestricted; test %al,%al; je`（结果不落槽）
    // ⇒ 删 cVar2 具名量（P0），谓词内联进条件。
    if ((char)((ServiceRestrictManager*)ARAD::Singleton<ServiceRestrictManager>::Get())
            ->isRestricted(user, (RestrictCategory::Enum)1, 0x1d) != 0)
    {
        send_packet_guild_levelup(user, 0xd1);
        return 0;
    }
    unsigned int local_2c = 0;           // -0x28：升级价格输出
    local_28 = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)7);
    if (local_28 != 0)
    {
        send_packet_guild_levelup(user, (unsigned char)(local_28 & 0xff));
        return 0;
    }
    local_24 = user->CheckGuildLevelUp(local_2c);
    if (local_24 != 0)
    {
        send_packet_guild_levelup(user, (unsigned char)(local_24 & 0xff));
        return 0;
    }
    send_packet_guild_levelup(user, 0);
    // ORIG 0820034c..0x082003ac：guildkey→%edi / CurCharacNo→%esi / UID→%ebx 各
    // callee-saved 独占（无具名槽），随后 ServerGroup→GetServerProxy 链式 receiver；
    // 原 uVar6/uVar7/iVar3b/uVar4/pCVar8 具名量每处多 store+load（P52/P183）。
    unsigned int uVar9 = local_2c;              // ORIG -0x4c 具名副本（保留）
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendGuildLevelup((int)user->GetUID(), user->getCurCharacNo(),
                           user->get_charac_guildkey(), uVar9);
    void* local_20 = user->GetGuildDBInfo();
    STGuildProxy local_46;
    STGuildDBInfoOnly* guildDb = reinterpret_cast<STGuildDBInfoOnly*>(local_20);
    local_2f = (char)guildDb->m_fundLevel + 1;
    local_46.m_level = local_2f;
    memcpy(local_46.m_guildName, local_20, 0x16);
    local_2e = guildDb->m_field95;
    local_46.m_powerSide = local_2e;
    // ORIG 082003fe..0x08200436：同一链式形（guildkey→%ebx，无具名槽）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->ModifyGuildInfoProxy(user->get_charac_guildkey(), local_46);
    return 0;
}