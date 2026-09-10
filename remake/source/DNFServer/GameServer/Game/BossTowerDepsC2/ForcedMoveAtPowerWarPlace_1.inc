void CAssaultMgr::ForcedMoveAtPowerWarPlace(CUser* user)
{
    try
    {
        CAssaultPlace::ForcedMoveAtPowerWarPlace(user);
    }
    catch (int caughtException)
    {
        // ORIG landing pad：__cxa_begin_catch 取异常对象内容（指针值）入栈
        // 局部后 __cxa_end_catch 吞掉——按 void* 值捕获同型。
        (void)caughtException;
    }
