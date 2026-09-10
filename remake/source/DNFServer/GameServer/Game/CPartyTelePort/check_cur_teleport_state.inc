char CPartyTelePort::check_cur_teleport_state()
{
    // ORIG 0x85bbb1x：null 路径 mov $0,%eax 直返（无 result 槽写入）⇒ 源形为
    // 直接 return 0，result 局部仅存活于 else 域。
    if (m_party == 0)
    {
        return 0;
    }
    char result = 1;
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (m_party->checkValidUser(i) != 1)
            {
                continue;
            }
            if (get_teleport_member_state(i) == 4)
            {
                continue;
            }
            // [b28-L14] ORIG 0x85bbb6x：`cmp $0x1,%al; setne %al; test; je
            // <循环尾>` + fall-through `movb $0,result` ⇒ 正向 `!= 1` 物化包裹
            // （P222），负极 `== 1 → continue` 产 sete/jne 反演。
            if (get_teleport_member_state(i) != 1)
            {
                result = 0;
            }
        }
        if (result != 0)
        {
            set_teleport_state(1);
        }
    }
    return result;
