void CExpertJobMgr::OnLeaveUser(CUser* user)
{
    // ORIG 0x0849e214：getter 两次独立调用、返回值不出具名槽（P0 律）；
    // 虚调用实参用第二次返回值。虚调用 vptr+4 = 业务槽 1 OnCloseMission。
    // [b22判词·N-REG] 非空判断 ORIG 为 `test;setne;test;je` bool 惯用法且
    // 虚调用指针走 %edx/%ecx 暂存，gcc 新版直接 `cmpl $0`+单寄存器复用；
    // 寄存器/惯用法选择为编译器结构，不可源码复现，语义等价。
    if (user->GetCurCharacExpertJob())
        user->GetCurCharacExpertJob()->OnCloseMission(
            user, ENUM_CLOSE_MISSION_TYPE_1);
