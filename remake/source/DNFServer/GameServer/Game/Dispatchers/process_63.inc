int Dispatcher_DenyJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x81d51e6：bool deny 无槽（mov $1/$0,%eax merge test）；applicant=param+0xd→%esi、
    // guildkey→%ebx 跨 GetUID 调用（register 具名）；master/sub xor $1 直判（bool callee）。
    MSG_BASE* pm = &msg;               // ORIG 顶部死槽 -0xc（0x10(%ebp)=msg，后回读 +0xd）
    if (user->get_state() != 3)
    {
        return 0;
    }
    // ORIG 短路 && 直判形（dA 探针实证 xor1;test;je 双段 + mov $1/$0 merge test）
    if (!user->IsGuildMaster() && !user->IsSubGuildMaster())
    {
        return 0;
    }
    struct MsgView { char pad[0xd]; unsigned int applicant; } __attribute__((packed));
    // ORIG 0x081d5257: mov 0xd(%eax),%esi 单级非对齐装载（packed 视图）
    register const unsigned int applicant = reinterpret_cast<MsgView*>(pm)->applicant;
    register const int guildkey = user->get_charac_guildkey();
    DB_DenyJoinGuild::makeRequest(user->GetUID(), (unsigned int)guildkey, applicant);
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x160);
    return 0;
