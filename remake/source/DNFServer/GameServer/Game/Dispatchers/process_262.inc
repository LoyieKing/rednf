// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081d55a4 orig/ours=58/59 ndiff-blocks=12
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | mov    %eax,-0x18(%ebp)
int Dispatcher_GoldTakeIncreasingAmount::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d55a4
    int err = check_error(user, msg);
    if (err < 1)
    {
        if (err < 0)
        {
            return -1;
        }
        // ORIG 槽序：msg 引用(-0x14) 先、param 引用(-0x10) 后；
        //   0x081d?: `mov -0x14(%ebp),%eax; mov 0xd(%eax),%edx` 常量偏移读 +
        //   `mov -0x10(%ebp),%eax; lea 0x4(%eax),%ebx`。原序相反且用
        //   `*(int*)((char*)&req+0xd)`（产 `add $0xd,%eax` 形）。
        MSG_GOLD_TAKE_INCREASING_AMOUNT& req = reinterpret_cast<MSG_GOLD_TAKE_INCREASING_AMOUNT&>(msg);
        stGoldTakeIncreasingAmount& p = static_cast<stGoldTakeIncreasingAmount&>(param);
        p.m_4 = req.m_overSkillTime;
        if (p.m_4 == 0xf68a)
        {
            // ORIG 0x081d5629 `mov %eax,-0xc(%ebp)` + 0x081d562c
            //   `cmpl $0x0,-0xc(%ebp)` + `jne` ⇒ 原码是**具名 int ret 槽**
            //   后接直比较（链式 `call()==0` 恒产 test/sete/test 三条，非 ORIG 形）。
            int ret = reinterpret_cast<CInventory*>(user->getCurCharacInvenW())
                          ->GetCreatureMgrW()->SetOverSkillTime((const int&)p.m_4);
            if (ret == 0)
            {
                p.m_8 = 1;
            }
            else
            {
                p.m_8 = 0;
            }
            return 0;
        }
        return 0x15;
    }
    return err;
}