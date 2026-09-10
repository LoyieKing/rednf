int Dispatcher_SecretShopOpenClose::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d28b4：check_error(三参) 结果写 param+0x04；
    // err>0 → 0；err<0 → LineFunc(0x2fdc)；err==0 → 设置/清除秘密商店购买位。
    stSecretShopOpenCloseParam& p = static_cast<stSecretShopOpenCloseParam&>(param);
    p.m_4 = check_error(user, msg, param);
    if (p.m_4 > 0)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        LineFunc(0x2fdc,
                 __PRETTY_FUNCTION__,
                 p.m_4, 0);
        return 0;
    }

    void* shopData = user->GetSecretShopData();
    if (shopData == 0)
    {
        p.m_4 = 0x7fffffff;
        return 0;
    }

    // msg+0xd：购买开关字节（非 0 → true）。
    bool buy =
        *reinterpret_cast<unsigned char*>(reinterpret_cast<char*>(&msg) + 0xd) != 0;
    int pos = user->GetUserPosInParty();
    reinterpret_cast<secretshop::SECRET_SHOP_DATA*>(shopData)->SetBuying(pos, buy);
    return 0;
