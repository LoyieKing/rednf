int TimerForbidChat::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 0xa);   // ORIG 第二实参常量 0xa
    if (user == 0)
        return 0;
    // ORIG 0x08635f50：name 临时跨调用驻留 %ebx（register），且每个使用点
    // 各自重新调用 get_charac_name_by_charac_no（真/假分支+首 trace 共 3 次）。
    register const char* name = user->get_charac_name_by_charac_no(p3);
    cMyTrace(__PRETTY_FUNCTION__, 0xa52, 0)("TimerForbidChat %s", name);
    name = user->get_charac_name_by_charac_no(p3);
    // ORIG：`!EnableChat` 前置（xor$1/test），Failed 块 fall-through、Enabled 后置。
    if (!G_GameWorld()->EnableChat(name))
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa54, 0)("Failed to enable to chat for: %s", name);
    }
    else
    {
        name = user->get_charac_name_by_charac_no(p3);
        cMyTrace(__PRETTY_FUNCTION__, 0xa56, 0)("Enabled to chat for: %s", name);
    }
    return 1;
