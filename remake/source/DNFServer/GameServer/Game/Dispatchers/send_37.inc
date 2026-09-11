void Dispatcher_RequestMemberEnter::send(CUser* user, ParamBase& param)
{
    (void)user;
    // ORIG 0x19：读 +0x04 与 0x7fffffff 比较，条件真则提前返回（无副作用）。
    stRequestMemberEnterParam& p = static_cast<stRequestMemberEnterParam&>(param);
    if (p.m_4 == 0x7fffffff)
        return;
}