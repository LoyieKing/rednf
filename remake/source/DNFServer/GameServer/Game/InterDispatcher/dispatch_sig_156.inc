int Inter_LoadGameOption::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 1)                        // cmp $1; setle
        return 0;

    // ORIG 0x084d7ea6 起 4 处 `mov -0x10(%ebp),%eax` 先于参数 `movl $…,(,4|8)(%esp)`
    // —— this 装载提前 = 「struct 成员指针作调用对象」形（probe this1 e25：裸指针局部
    // 在 4.4/4.1 均产常量先、load 后形；struct 成员形产 ORIG 序）。
    struct { CETCOption* etcOpt; } etcWrap;
    etcWrap.etcOpt = reinterpret_cast<CETCOption*>(data);
    etcWrap.etcOpt->setETCOption((ENUM_OPTION_ETC)1, 1);

    CGameOption* opt = user->GetGameOptionRef();
    opt->SetEtcOption(*etcWrap.etcOpt);
    opt->SetQuickChatOption(
        *reinterpret_cast<CQuickChattingOption*>(
            reinterpret_cast<char*>(etcWrap.etcOpt) + 0x48), 0);   // ORIG 经 -0x10 槽
    opt->SetQuickChatOption(
        *reinterpret_cast<CQuickChattingOption*>(
            reinterpret_cast<char*>(etcWrap.etcOpt) + 0x174), 1);  // ORIG 经 -0x10 槽
    opt->SetEmoticonInfo(
        *reinterpret_cast<CChattingEmoticonConfig*>(
            reinterpret_cast<char*>(etcWrap.etcOpt) + 0x2a0));     // ORIG 经 -0x10 槽

    PacketGuard packet;
    packet.put_header(0, 0xad);
    packet.put_int(0x48);
    if (G_GameWorld()->IsSchoolPvPChannel())
    {
        etcWrap.etcOpt->setETCOption((ENUM_OPTION_ETC)0xa, 1);
        etcWrap.etcOpt->setETCOption((ENUM_OPTION_ETC)0xb, 0);
    }
    // ORIG 0x084d7f4x：基址取 etcOpt 槽 -0x10(%ebp)（非 data 形参 0x10）。
    packet.put_binary(reinterpret_cast<char*>(etcWrap.etcOpt), 0x48);
    packet.put_int(300);
    packet.put_binary(reinterpret_cast<char*>(etcWrap.etcOpt) + 0x48, 300);
    packet.put_int(300);
    packet.put_binary(reinterpret_cast<char*>(etcWrap.etcOpt) + 0x174, 300);
    packet.finalize(true);
    user->Send(packet);
    return 0;
