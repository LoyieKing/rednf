bool CUserTeen::IsRight(CUser* user)
{
    // ORIG +0x06：无 `movl $0x0,-0xc(%ebp)` ⇒ 局部不初始化（仅取址传引用）；
    int dummy;
    if (!user->CheckLimitMiniumAge(0xf, dummy))
        return 0;
    return 1;
