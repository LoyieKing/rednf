int Dispatcher_Select_3rdChronicleItem_ForEnchant::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // ORIG 0x081d3420：param→-0x8、msg→-0x4 死槽（4.4 先声明→远槽，probe slots4），
    // 恒返 0 + leave（技巧8 双槽版）。
    ParamBase* pp = &param;
    MSG_BASE* pm = &msg;
    (void)user;
    (void)pp;
    (void)pm;
    return 0;
}