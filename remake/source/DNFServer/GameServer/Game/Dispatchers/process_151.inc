// ORIG 0x081d26ea T _ZN28Dispatcher_SecretShopBuyItem7processEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/Dispatcher_SecretShopBuyItem/process.md 汇编+反编译 C 全文转写）
// param 为模板实参 stSecretShopBuyItemParam，ORIG 裸访问 +0x04 错误码槽（m_4）。
// 调用同 TU 模板 check_error(CUser*, MSG_BASE&, ParamBase&)（ORIG check_error(CUser*,MSG_BASE&,ParamBase&)）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d26ea orig/ours=75/77 ndiff-blocks=15
//   帧差 sub $38 vs $34 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x38,%esp | mov    %eax,-0x18(%ebp) | mov    %eax,%edx
int Dispatcher_SecretShopBuyItem::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
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
    // ORIG +0x07d 才装载 msg 引用（声明点在使用处的错误分支之后）：p 先声明占 -0x18、mref 后占 -0x14
    MSG_SECRET_SHOP_BUY_ITEM& mref = reinterpret_cast<MSG_SECRET_SHOP_BUY_ITEM&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
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
}