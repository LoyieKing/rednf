bool Teleport::move_area(CUser* user)
{
    // ORIG 0x08283fb8：effect(1) → GameWorld::move_area(...) → effect(2)+
    // update_next_end_cool_time。ORIG 经 stPartyTelePort getters 取 5 字段
    // （add this+4 → mov %eax,(%esp) → call *_get_teleport_*），故此处调 getter
    // 而非直读字段（P：getter 为 ORIG 真符号 CPartyTelePort.h 声明）。
    if (user == 0)
    {
        return false;
    }
    send_teleport_effect(user, 1);
    // ORIG xor $0x1,%eax; test %al,%al; je（直测返回寄存器，无槽，P198）：
    // 谓词整体内联进 if(!...)，禁提同名 bool 局部（反多 mov %al,-off; cmpb 两步）。
    if (!G_GameWorld()->move_area(
            user, (int)(unsigned char)m_tp.get_teleport_village(),
            (int)(unsigned char)m_tp.get_teleport_area_index(),
            // ORIG Ghidra sVar2/sVar3 = get_teleport_pos* 记作 short、以 (int)
            // 直传 ⇒ movswl/cwtl 有符号拓宽（而非 getter 的 unsigned short movzwl）。
            (int)(short)m_tp.get_teleport_posx(),
            (int)(short)m_tp.get_teleport_posy(),
            (int)(unsigned char)m_tp.get_teleport_direction(), true, 2, 0, 0))
    {
        return false;
    }
    send_teleport_effect(user, 2);
    update_next_end_cool_time(user);
    return true;
}