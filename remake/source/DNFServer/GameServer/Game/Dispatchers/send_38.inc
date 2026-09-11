void Dispatcher_Request_Seria_Buff::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081dbe08 T：err=*(int*)((char*)&param+4)；0 → set_growth_power_exp_reward(
    // get_charac_level()) + SendCmdOkPacket(0x1b1)；否则 SendCmdErrorPacket(0x1b1,…)。
    // ORIG 0x081dbe08：param 指针→-0xc 槽，字段两次 mov 0x4(%eax) 单级读。
    Err4ParamView* p = reinterpret_cast<Err4ParamView*>(&param);
    if (p->m_4 == 0)
    {
        user->set_growth_power_exp_reward(user->get_charac_level());
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x1b1);
    }
    else
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1b1, (unsigned char)p->m_4);
    }
}