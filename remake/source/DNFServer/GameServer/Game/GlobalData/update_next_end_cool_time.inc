void Teleport::update_next_end_cool_time(CUser* user)
{
    // ORIG 0x08283e30：is_equip_aura_avatar('..', &remain) 取剩余；
    // set_aura_avatar_option_value(2, getCurSec()+remain)；UpdateAuraAvatarOption。
    if (user != 0)
    {
        int remain = 0;
        user->is_equip_aura_avatar(2, remain);
        int v = GlobalData::s_systemTime_.getCurSec() + remain;
        user->set_aura_avatar_option_value(2, v);
        // ORIG 0x08283e??：get_aura 结果 %eax 直落实参槽（无中间局部槽）。
        user->UpdateAuraAvatarOption(2, user->get_aura_avatar_option_value(2));
    }
    // ORIG 尾岛：末 call 后 jmp 到共享 epilogue（技巧 7 尾置 return）。
    return;
