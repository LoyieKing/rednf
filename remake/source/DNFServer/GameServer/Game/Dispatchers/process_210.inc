int Dispatcher_Check3rdPartyConcent::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081df3d0 T：state==3 时 DB_Check3rdPartyConcent::makeRequest(
    // user->GetUID(), user->get_acc_id())；恒返回 0。
    // ORIG 形态 [setne;test;je→body][mov $0;jmp]=提前 return 0：反极性守卫 + (bool) 物化。
    if ((bool)(user->get_state() != 3))
    {
        return 0;
    }
    {
        DB_Check3rdPartyConcent::makeRequest(user->GetUID(),
                                            user->get_acc_id());
    }
    return 0;
