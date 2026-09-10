int Dispatcher_Equipslot_Switch::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stEquipslotSwitch& out = static_cast<stEquipslotSwitch&>(param);
    MSG_EQUIPSLOT_SWITCH& req = reinterpret_cast<MSG_EQUIPSLOT_SWITCH&>(msg);
    char* pReq = reinterpret_cast<char*>(&req);
    int err = check_error(user, msg);
    if (err > 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18d,
                                 (unsigned char)(err & 0xff));
        return -1;
    }
    if (err < 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x18d, 0x15);
        return LineFunc(0x3e2c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    CExpandEquipslot* expand =
        user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)9);
    out.m_result = (unsigned char)(char)expand->EquipslotSwitch(
        user, pReq[0xf], pReq[0xd], pReq[0xe]);
    return 0;
