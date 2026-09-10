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
    // ORIG 0x081ce714: `mov 0x14(%ebp),%eax; mov %eax,-0x10(%ebp)` —— 具名 param 引用占槽；
    //   0x081ce71d `movzbl 0xd(%eax),%edx` / 0x081ce724 `mov %dl,0x4(%eax)` 均常量偏移，
    //   原 `*(uchar*)((char*)&param+4)` 会产 `lea 0x4(%eax),%edx; add $0xd,%eax` 形。
    stClientSpecStatisticParam& res = static_cast<stClientSpecStatisticParam&>(param);
    res.m_spec = (unsigned char)req.m_padD;
    memcpy(user->getClientHWSpec(), reinterpret_cast<char*>(&req) + 0xe, 0xbf8);
    return 0;
