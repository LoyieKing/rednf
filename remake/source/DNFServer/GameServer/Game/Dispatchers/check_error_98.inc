unsigned int Dispatcher_GuildCargoMoveItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    // ORIG 0x081d18dc 控制流（技巧6/7 线性 early-return + 技巧40 flag-island）：
    // state==3 && characR!=0 物化成 %eax 双岛后 `test %al,%al; je 岛(-1)`；
    // guildkey `test;sete %al`（==0 → 100）；agit `xor $1;test;je`（false → 0xb5）；
    // 全部通过则 fall 到 `mov $0x0` 尾岛。
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
        return -1;
    if (user->get_charac_guildkey() == 0)
        return 100;
    if (!user->IsExistGuildAgit())
        return 0xb5;
    return 0;
