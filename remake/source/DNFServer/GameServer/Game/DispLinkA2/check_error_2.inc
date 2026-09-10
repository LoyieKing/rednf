unsigned int Dispatcher_GuildCargo::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // P196 真语义 bug：ORIG 当 getCurCharacR()==0 才返 -1（原 !=0 方向反）。
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return -1;

    if (user->get_charac_guildkey() == 0)                  // 0x0822f46c
        return 100;
    if (!user->IsExistGuildAgit())                         // 0x082301d8（ORIG xor$1 形）
        return 0xb5;
    return 0;
