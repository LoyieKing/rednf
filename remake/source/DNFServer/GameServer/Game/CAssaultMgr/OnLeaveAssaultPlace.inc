int CAssaultMgr::OnLeaveAssaultPlace(CUser* user, bool flag)
{
    // ORIG 0x82ef176：取用户所在区域并执行离开流程；ORIG EH 为 **catch(ENUM_ERROR)**
    // （类型过滤 landing pad `cmp $0x1,%edx; je <resume>` + begin_catch 后
    // `mov (%eax),%eax; mov %eax,-0xc` 取值落槽 + end_catch），吞掉 P6 族
    // throw ENUM_ERROR(int)。此前写 catch(...) 少 6 条（无过滤器/无取值）。
    try
    {
        CAssaultPlace* place = _GetAssaultPlace(
            (unsigned int)user->GetAssaultPlace());
        place->LeaveAssaultPlace(user, flag);
    }
    // [R18exc 异常类型轴 2026-09-13] ORIG LSDA 类型表 = _ZTI10ENUM_ERROR（0x8b75f40）。
    catch (ENUM_ERROR)
    {
    }
    // ORIG 0x82ef1da：尾 mov $1,%eax ⇒ int 返回 1
    return 1;
}