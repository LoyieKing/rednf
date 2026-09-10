int Dispatcher_SecretShopBuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_SECRET_SHOP_BUY_ITEM& mref = reinterpret_cast<MSG_SECRET_SHOP_BUY_ITEM&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    stSecretShopBuyItemParam& p = static_cast<stSecretShopBuyItemParam&>(param);
    p.m_4 = check_error(user, msg, param);
    if (p.m_4 >= 1)
    {
        return 0;
    }
    if (p.m_4 < 0)
    {
        return LineFunc(0x2fa1,
                        __PRETTY_FUNCTION__,
                        p.m_4, 0);
    }
    secretshop::SECRET_SHOP_DATA* data =
        static_cast<secretshop::SECRET_SHOP_DATA*>(user->GetSecretShopData());
    secretshop::SECRET_SHOP_INFO* info =
        static_cast<secretshop::SECRET_SHOP_INFO*>(user->GetUserSecretShop());
    if (data == 0 || info == 0)
    {
        p.m_4 = 0x7fffffff;
        return 0;
    }
    if (data->IsOpen() != 0)
    {
        GlobalData::s_secret_shop->BuyItem(user, *info,
                                           mref.m_itemIdx,
                                           mref.m_count);
    }
    return 0;
