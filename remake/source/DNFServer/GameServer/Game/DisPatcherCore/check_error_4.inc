// ORIG 0x081c3f4a T _ZN25DisPatcher_RecoverStamina11check_errorEP5CUserR8MSG_BASE
// （报告 DisPatcher_RecoverStamina/check_error.md 逐块映射）：state!=3 → 99；
// SecuProtectionField Check(user,0x2f) 非 0 直返；否则 getCurCharacR 为空 → -1。
int DisPatcher_RecoverStamina::check_error(CUser* user, MSG_BASE& msg)

{
    if (user->get_state() != 3)
    {
        return 0x63;
    }
    int secu = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x2f);
    if (secu != 0)
    {
        return secu;
    }
    if (user->getCurCharacR() == 0)
    {
        return 0xffffffff;
    }
    return 0;
}
