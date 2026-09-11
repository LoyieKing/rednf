bool CPartyTelePort::check_cur_teleport_state()
{
    // ORIG 0x85bbb1x：null 路径 mov $0,%eax 直返（无 result 槽写入）⇒ 源形为
    // 直接 return 0，result 局部仅存活于 else 域。
    if (m_party == 0)
    {
        return 0;
    }
    // [N6S2NegB 2026-09-11] ORIG 0x85bbb21 `movb $0x1,-0xd(%ebp)` / 0x85bbb7f
    // `movb $0x0,-0xd(%ebp)` / 尾 0x85bbbb1 `movzbl -0xd(%ebp),%eax` ⇒ 局部为
    // **bool**（char 形在 4.4 下 `return` 会被插 `cmpb; setne`，多 1 条）。
    // 配套：callee 返回类型已由父泳道 N6Free231to250 改 bool（.h:48）。
    bool result = true;
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
                result = false;
            }
        }
        if (result != 0)
        {
            set_teleport_state(1);
        }
    }
    return result;
}