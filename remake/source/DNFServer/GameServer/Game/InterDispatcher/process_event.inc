// ORIG T _ZN18Inter_CreateCharac13process_eventEP5CUserP17SIG_CREATE_CHARAC
// @0x084bf4ac（报告 process_event.md 两段全读；薄转发）。
void Inter_CreateCharac::process_event(CUser* user, SIG_CREATE_CHARAC* sig)
{
    process_createGift(user, sig);
}