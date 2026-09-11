// ORIG T 0x82e8de2 _MakeUserUdpIpPort(PacketGuard*)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 166/166（p167 33/33 CLEAR）
//   get_mtu 段（同型 int 实参）4.4 恒 packet 先载，ORIG 为 `mov %eax,%edx` 值先
//   中转 ⇒ P226 裸语句表达式 `p->put_int(({ get_mtu(); }))` 复现（cast/双 cast
//   两形实测不翻），ndiff 7→0。
void CAssaultPlace::_MakeUserUdpIpPort(PacketGuard* p)
{
    // b28 BM4 判词（N18a/N7）：fresh .o 实残 1 条 = ORIG +0x175 stamina 段
    // `movsbl %al,%edx` 拓宽即落第二寄存器 + receiver 重载（4.4 恒 `%eax`
    // 单段直落参数槽，冗余 regmove 不可造；P211 异型化探针 pbq 同形已证）。
    // 其余 6 行 difflib 连锁移位由该 ±1 行产生。无源码解，语义等价封顶。
    p->put_header(0, 0xb);
    int countIdx = 0;
    int count = 0;
    countIdx = p->get_index();
    p->put_byte(count);
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        p->put_short(m_assaulters[i].GetUser()->get_unique_id() & 0xffff);
        p->put_int(static_cast<int>(
            m_assaulters[i].GetUser()->m_network.get_inner_ip()));
        p->put_int(static_cast<int>(
            m_assaulters[i].GetUser()->m_network.get_ip()));
        p->put_short(m_assaulters[i].GetUser()->m_network.get_port() & 0xffff);
        p->put_int(m_assaulters[i].GetUser()->get_acc_id());
        p->put_byte(static_cast<int>(
            m_assaulters[i].GetUser()->m_network.get_nat_type()));
        // [b28d-solo-W6_AssaultPlaceDepsB] ORIG 0x82e8f9c 为 `mov %eax,%edx;
        // mov 0xc(%ebp),%eax` 值先装载形；4.4 对同型 int 实参恒折 packet 先载
        // （enum/unsigned cast 两形实测不翻）⇒ P226 裸语句表达式复现值先中转。
        p->put_int(({ m_assaulters[i].GetUser()->m_network.get_mtu(); }));
        // [b23S3C] P48 去缓存 + P191 极性：ORIG 每用点重取 getCurCharacR()（无 ch 槽），
        // 且以指针真值判定（test;setne;test;je），m_stamina 落 fall-through。
        if (m_assaulters[i].GetUser()->getCurCharacR())
            p->put_byte(
                m_assaulters[i].GetUser()->getCurCharacR()->m_stamina);
        else
            p->put_byte(0);
        count++;
    }
    p->put_byte(countIdx, count);
    p->finalize(true);
}