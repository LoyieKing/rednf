// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d31ee orig/ours=93/98 ndiff-blocks=18
//   帧差 sub $4c vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | sub    $0x4c,%esp
int Dispatcher_Write_On_The_GuildBoard::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d31ee
    // [G5·256 负结论 2026-09-11] 试具名引用 p/m + 裸偏移替换 ⇒ 实测 92→**94**（劣化 2）
    //   ⇒ 回滚保裸偏移形（ORIG 0x081d3210 槽形在 4.4 下反劣）。
    // msg+0xd：公会板标题文本区首字节（非 0 才有效）
    if (*(unsigned char*)((char*)&msg + 0xd) == 0)
    {
        *(int*)((char*)&param + 4) = 1;
        return 0;
    }

    // ORIG 0x081d323b 段：`test %al,%al; jne <true 岛>`（hasPreventString 为真即短路）、
    //   非真再测 hasSpecialCharacterExceptSpace `je <false>` —— 正向 `||` 短路合流，
    //   无德摩根 `xor $1`、无具名 bool 槽（我方原 `!a && !b` + `bool invalid`
    //   ⇒ 多 2 条 xor + 3 条 movb/cmpb，-0x25 槽为本方发明）。
    if (G_CDataManager()->hasPreventString((const char*)((char*)&msg + 0xd)) ||
        G_CDataManager()->hasSpecialCharacterExceptSpace((const char*)((char*)&msg + 0xd)))
    {
        *(int*)((char*)&param + 4) = 1;
        return 0;
    }

    // 实参求值顺序按 ORIG：guildkey → growType(movsbl) → job → characNo → accId → GetServerGroup
    unsigned int guildkey = (unsigned int)user->get_charac_guildkey();
    char growType = (char)user->getCurCharacGrowType();
    int job = user->get_charac_job();
    unsigned int characNo = (unsigned int)user->getCurCharacNo();
    unsigned int accId = user->get_acc_id();
    ENUM_SERVER_GROUP group = (ENUM_SERVER_GROUP)user->GetServerGroup();
    CGuildServerProxy* proxy = GlobalData::s_guild_proxy_mgr->GetServerProxy(group);
    proxy->SendGuildBoardWrite((char*)((char*)&msg + 0xd), accId, characNo, job, growType,
                               guildkey);
    return 0;
}