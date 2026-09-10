int Dispatcher_JoinGuildInfo::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    // [round5 A01] ORIG 0x081d53aa/0x081d53c6 双**负极性**独立 guard
    //   （`cmp $3; setne; test; je body; mov$0; jmp exit`）⇒ 禁止合并成 `A==3 && B==0` 正极性块。
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (user->get_charac_guildkey() != 0)
    {
        return 0;
    }
    int serverGroup;
    // ORIG 0x081d53e5 `je +0x5f`：then=GetServerGroup（fall-through），else=env（跳远侧）。
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup = (int)user->GetServerGroup();
    }
    else
    {
        serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    }
    // [round5 A01] P0/P183：ORIG 0x081d540b/0x081d5418 结果分别在 %ebx/%eax，
    //   无具名槽（我方旧码 characNo/uid 各占 4B ⇒ 帧 0x24→0x28）。实参右到左：
    //   a2=GetUID(最先求值) → a1=serverGroup → a0=GetUID 形参顺序见下。
    // [FL1 2026-09-10 负结论] P290 变体（首实参 `(unsigned int)`）实测无效：
    //   仍 52/54、`push %esi` 保留 ⇒ ORIG %ebx vs 我方 %esi 属 N7 寄存器角色差
    //   （帧 0x28 vs 0x24），封顶。
    DB_JoinGuildInfo::makeRequest((int)user->GetUID(),
                                  (ENUM_SERVER_GROUP)serverGroup,
                                  (unsigned int)user->getCurCharacNo());
    return 0;
