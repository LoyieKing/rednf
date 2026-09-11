// ORIG 0x84be970 逐指令转写（getUser 参数序、DisConnSig 与双路径日志均按汇编）。
// [b28d-lane2] verdict=DIFF ndiff=42（ORIG 0x084be970）：混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=37（ORIG 0x84be970）：混合残差（寄存器分配/槽序：N7/N18a/N82/P112）。
bool InterDispatcher::dispatch(int code, int userId, char* data, int len)
{
    CUser* user = 0;
    if (userId != -1)
    {
        user = G_CGameManager()->getUser(userId, code + 0x3e8);
        if (user == 0)
        {
            return false;
        }
    }
    ISigDispatcher* d = get_dispatcher(code);
    if (d == 0)
    {
        LogManager::logFormat(1, "InterDispatcher.cpp",
                              __PRETTY_FUNCTION__,
                              0x602, "get_dispatcher(%d)", code);
        return false;
    }
    int result = 0;
    if ((result = d->dispatch_sig(user, data, len)) != 0)
    {
        if (user != 0)
        {
            user->DisConnSig((DISCONN_SIG)2, true, result + 0xf4240);
            cMyTrace(__PRETTY_FUNCTION__, 0x632, 0)("discon interdispatcher return false: %d line(%d)(m_id: %s)",
                  code, result, NumberToString(user->get_acc_id(), 0));
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x636, 0)("discon interdispatcher return false: %d line(%d)(m_id: 0)",
                  code, result);
        }
        // ORIG 0x84beb29/0x84beb30：if 体内与函数尾各一枚 `mov $0x1,%eax`（早退型带
        //   `jmp 0x84beb35` 跳过尾块；尾块直落 epilogue）⇒ 两支 return true 照抄（P171）。
        return true;
    }
    return true;
}