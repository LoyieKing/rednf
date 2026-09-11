void Dispatcher_Select_Item_Grwoth_Power::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081dbc66（报告 Dispatcher_Select_Item_Grwoth_Power/send.md）：
    // 唯一槽 -0xc=param 指针；get_charac_level 结果免槽直用（%eax 留寄存器
    // 填 0x4(%esp)，技巧37）。具名 level 局部会膨胀出第二槽致整帧平移。
    stGrowthPowerParam& out = static_cast<stGrowthPowerParam&>(param);
    if (out.m_4 == 0)
    {
        user->set_growth_power_reward(user->get_charac_level());
        user->SendCmdOkPacket(static_cast<ENUM_CMDPACKET>(0x1b0));
    }
    else
    {
        user->SendCmdErrorPacket(static_cast<ENUM_CMDPACKET>(0x1b0),
                                 static_cast<unsigned char>(out.m_4));
    }
}