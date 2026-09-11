void CGuildServerProxy::OnBuyItem(CUser* user, CItem const* item,
                                  unsigned int powerWarPoint, unsigned int pp)
{
    // [b26L17 判词 N7] 唯一残差：两处虚调用 callee 装载 %eax↔%edx 互换
    //   （ORIG vtable 槽→edx、实参→eax；4.4 恒反）。118/118 助记符全等。
    if (item->GetGuildPowerWarPoint() != 0)   // vtable+0x28（槽 10）
    {
        user->DecreaseGuildPowerWarPoint(powerWarPoint);
        SendIncreaseDecreasePowerWarPoint(user->get_charac_guildkey(),
                                          user->get_charac_no(-1),
                                          -(int)powerWarPoint);
    }
    if (item->GetUserPowerWarPoint() != 0)    // vtable+0x2c（槽 11）
    {
        user->DecreasePowerWarPoint((short)pp);
    }
}