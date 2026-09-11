// [b28d-solo-CPartyTelePort] AE（check_tu_routed 口径）：ORIG 0x085bc2b0。
// 修法：struct 名 TeleportEffect→effect_data（P0/N114 符号名）；内层迭代器
// begin/end 缓存（P13）；effect_data 逐字段赋值（P54，area 先于 village）；
// 第二个守卫改正向包裹（P222）消 4.4 多余 nop+jmp 岛；第二循环 int i（setg）。
// 残余仅重定位地址与 .o 阶段立即数显示（N124/P264）；p167 调用点差集 CLEAR。
void CPartyTelePort::send_teleport_effect_at_different_place(int param)
{
    if (m_party == 0)
    {
        return;
    }

    std::vector<effect_data> effects;
    for (int i = 0; i <= 3; ++i)
    {
        if (m_party->checkValidUser(i) != 1)
        {
            continue;
        }
        CUser* user = (CUser*)m_party->get_user(i);
        if (user != 0)
        {
        // [b28d-solo-CPartyTelePort] ORIG 0x85bc31c：begin sret -0x38 / end
        // sret -0x3c 均在循环初始化区求值一次（P13 缓存 end）；found 的
        // `movb $0,-0x19` 落 end 之后（ORIG 0x85bc346）⇒ 声明在 it/e 之后。
        std::vector<effect_data>::iterator it = effects.begin(),
            e = effects.end();
        bool found = false;
        for (; it != e; ++it)
        {
            if (it->area == user->get_area(false) &&
                it->village == user->getCurCharacVill())
            {
                found = true;
                it->users.push_back(user->get_unique_id());
                break;
            }
        }
        if (!found)
        {
            // [b28d-solo-CPartyTelePort] ORIG 0x85bc157：effect_data 默认构造
            // 后逐字段写（area 先于 village，P54 逐字段赋值≠聚合初始化）。
            effect_data ed;
            ed.area = user->get_area(false);
            ed.village = user->getCurCharacVill();
            ed.users.push_back(user->get_unique_id());
            effects.push_back(ed);
            // [b28d-solo-CPartyTelePort] ORIG 0x85bc46c `movl $0x0,-0x18(%ebp)`
            // 死初始化局部（写后不读，P31/P232 槽基对齐；语义等价）。
            int pad0 = 0;
        }
        }
    }

    // [b28d-solo-CPartyTelePort] ORIG 0x85bc608 用 setg（有符号）⇒ 源码 int i。
    for (int i = 0; i < (int)effects.size(); ++i)
    {
        PacketGuard pg;

        pg.put_header(0, 0x17b);
        pg.put_byte((int)(char)param);
        int cnt = (int)effects[i].users.size();
        pg.put_byte((int)(char)cnt);
        for (int j = 0; j < cnt; ++j)
        {
            pg.put_short((int)effects[i].users[j]);
        }
        pg.finalize(true);
        G_GameWorld()->send_to_area(
                                   (int)effects[i].village,
                                   effects[i].area, pg);

    }
}