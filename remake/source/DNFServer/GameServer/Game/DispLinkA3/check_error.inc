// ============================================================================
// 1. Dispatcher_SecretShopBuyItem::check_error
//    ORIG @0x081d26c0 [0x081d26c0, 0x081d26e9]
//    语义：channel state 必须为 5（dungeon 内）；否则 0x7fffffff（上层静默）。
// ============================================================================
unsigned int Dispatcher_SecretShopBuyItem::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)msg;
    (void)param;
    if (user->get_state() != 5)
    {
        return (int)0x7fffffff;
    }
    return 0;
}