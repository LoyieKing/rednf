int Dispatcher_VerifyCreatureQuest::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG：入口岛 `cmpl $0,0xc; jne BODY; mov $0; jmp END`（技巧23）；
    // `cmp $2; setle;test;je → ret0 岛`（技巧9）。
    // b28c-33：goto 形在 4.4 恒多一条块间 nop；普通早返形（探针 l33b f_vc）逐条命中
    // ORIG 的 `jne BODY; mov $0; jmp END`（无 nop）。
    if (user == 0)
        return 0;
    if ((bool)(user->get_state() <= 2))
    {
        return 0;
    }
    user->getCurCharacInvenR()->GetCreatureMgrR()->VerifyEquipedCreatureQuest();
    return 0;
