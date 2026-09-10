int Dispatcher_EvadeAssault::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    (void)param;
    int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    // ORIG 0x81ce532：GetInstanceAssaultMgr 结果免槽直用（%eax 即填 (%esp)，
    // user 从 0xc(%ebp) 经 %edx 直落 0x4(%esp)）——具名 mgr 指针会多占 -0xc
    // 槽并把 err 挤到 -0x10（技巧36/37）。ORIG 0x81ce546 xor $1：r!=1 →
    // fallthrough LineFunc(0x27b8)；r==1 → je mov $0（技巧1）。
    if (!pvp_assault::GetInstanceAssaultMgr()->OnEvadeAssaultPlace(user))
    {
        return LineFunc(0x27b8,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    return 0;
