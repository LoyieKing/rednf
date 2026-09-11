// ============================================================================
// CAssaultMgr::ForcedMoveAtPowerWarPlace —— ORIG T 0x082ed672（报告全文转写）：
//   try { CAassaultPlace::ForcedMoveAtPowerWarPlace(user); } catch(...) {}——
//   landing pad 取异常指针入局部后 __cxa_end_catch 吞掉。
// [b24F] DONE：catch 形参 void*→int（ORIG 解引用 (%eax),%eax = 值类型捕获）。
// ============================================================================
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
}