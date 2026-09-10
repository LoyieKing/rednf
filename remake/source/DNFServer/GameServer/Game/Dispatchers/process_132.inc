int Dispatcher_RequestJoinGuild::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d4e28 (0x14c)：state==3 且 get_charac_guildkey(user)==0 才填
    // SIG_REQUEST_JOIN_GUILD 并 DB_RequestJoinGuild::makeRequest(GetUID,&sig)；
    // guildkey!=0 → SendCmdErrorPacket(0x15d, 0x12)；恒返 0。
    if (user->get_state() != 3)
    {
        return 0;
    }
    if (user->get_charac_guildkey() != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x15d, 0x12);
        return 0;
    }

    SIG_REQUEST_JOIN_GUILD sig;                 // 栈 0x44（SigTypes_D.h m_pad[0x44]）
    memset(&sig, 0, 0x44);
    // +0x00 char name[0x16] ← msg+0x11
    strncpy((char*)&sig.m_pad[0x00], (char*)&((char*)&msg)[0x11], 0x16);
    // +0x24 characNo = getCurCharacNo(user)
    *reinterpret_cast<int*>(&sig.m_pad[0x24]) = user->getCurCharacNo();
    // +0x28 accId = get_acc_id(user)
    *reinterpret_cast<int*>(&sig.m_pad[0x28]) = user->get_acc_id();
    // +0x2c serverGroup
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        *reinterpret_cast<int*>(&sig.m_pad[0x2c]) = user->GetServerGroup();
    }
    else
    {
        *reinterpret_cast<int*>(&sig.m_pad[0x2c]) =
            *reinterpret_cast<int*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x378);  // m_serverEnvir.m_serverGroup
    }
    // +0x30 ssn[2] ← GetSsnString(user)
    memcpy(&sig.m_pad[0x30], user->GetSsnString(), 2);
    // +0x32 char name[0x19] ← msg+0x2c
    strncpy((char*)&sig.m_pad[0x32], (char*)&((char*)&msg)[0x2c], 0x19);

    DB_RequestJoinGuild::makeRequest(user->GetUID(), &sig);
    return 0;
