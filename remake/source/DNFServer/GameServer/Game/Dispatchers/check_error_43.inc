int Dispatcher_ChangeHostWarRoom::check_error(CUser* user, MSG_BASE& msg)
{
    if (user == 0)
    {
        // ORIG 0x081d74f0 `movl $&_ZZ…E19__PRETTY_FUNCTION__,0x4(%esp)`（P139/P118：
        // 手写签名串 + 具名指针局部会多占 -0x14 槽并产两段装载）⇒ 直传 __PRETTY_FUNCTION__。
        return LineFunc(0x41b1, __PRETTY_FUNCTION__, 0, 0);
    }
    if (user->getCurCharacR() == 0)
    {
        return LineFunc(0x41b4, __PRETTY_FUNCTION__, 0, 0);
    }
    const MSG_CHANGE_HOST_WARROOM& req =
        reinterpret_cast<const MSG_CHANGE_HOST_WARROOM&>(msg);
    // ORIG 0x081d7549/0x081d7554：两条件各自重读 `mov -0xc,%eax; movzbl 0xd(%eax),%eax`
    // （P0/P48：无 warRoom 槽往返 ⇒ 表达式直读，禁具名局部）。
    if ((signed char)req.m_field0d < 0 || (signed char)req.m_field0d > 5)
    {
        return LineFunc(0x41b8, __PRETTY_FUNCTION__, 0, user->get_acc_id());
    }
    // ORIG 0x081d7597：`cmp $0x8,%eax; setne %al; test %al,%al; je <mov $0>`（4.1 bool
    // 物化后分支）⇒ P225 while+return 形复现；三目/裸 if 在 4.4 折成 jne 直跳。
    while (user->get_state() != 8)
        return -1;
    return 0;
}