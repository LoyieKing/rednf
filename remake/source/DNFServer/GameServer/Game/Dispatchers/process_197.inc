// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d28b4 orig/ours=66/62 ndiff-blocks=11
//   帧差 sub $24 vs $28 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    0x14(%ebp),%eax
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
        // ORIG 0x081d2928 `call LineFunc` → 0x081d292d `jmp 081d298c`（函数尾，eax 即
        // LineFunc 返回值直通；同函数其它早退才有 `mov $0x0,%eax`）⇒ 原语义是
        // `return LineFunc(...)`，写成语句+return 0 会吞掉非 0 断言码（判据库⑪）。
        return LineFunc(0x2fdc,
                        __PRETTY_FUNCTION__,
                        p.m_4, 0);
    }

    // ORIG +0x7b：msg 引用槽 -0x10（在 GetSecretShopData 之前；p 槽 -0x14 最深）
    MSG_BASE& mref = msg;
    // P58：类型化指针成员使用点免 cast 中转（ORIG receiver 最后装载 `mov -0xc,%eax; mov %eax,(%esp)`）
    secretshop::SECRET_SHOP_DATA* shopData =
        static_cast<secretshop::SECRET_SHOP_DATA*>(user->GetSecretShopData());
    if (shopData == 0)
    {
        p.m_4 = 0x7fffffff;
        return 0;
    }

    // ORIG +0xa6：buy 无槽（setne→ebx 实参内联）；右→左求值先 buy 后 GetUserPosInParty
    shopData->SetBuying(
        user->GetUserPosInParty(),
        reinterpret_cast<const MSG_SECRET_SHOP_OPEN_CLOSE*>(&mref)->m_openFlag != 0);
    return 0;
}