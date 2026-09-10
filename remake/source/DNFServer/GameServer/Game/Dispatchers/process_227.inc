int Dispatcher_Client_Spec_Statistic::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    MSG_CLIENT_SPEC_STATISTIC& req = reinterpret_cast<MSG_CLIENT_SPEC_STATISTIC&>(msg);
    *(unsigned char*)((char*)&param + 4) =
        *(unsigned char*)(reinterpret_cast<char*>(&req) + 0xd);
    memcpy(user->getClientHWSpec(), reinterpret_cast<char*>(&req) + 0xe, 0xbf8);
    return 0;
