CDynamicDisposition::CDynamicDisposition(CUser** users, int* counts) // ORIG 0x82e6032
{
    // b28d-solo-CAssaultMgr: ORIG `shr %ax` 是 4.1 对 **unsigned short 除以 2**
    // 的 16 位无符号除法降级（探针 /tmp/shr_probe.cpp f6 `g_b = g_a / 2;` 逐条
    // 同形 `movzwl g_a,%eax; shr %ax`）；写 `>> 1` 会被整型提升成 32 位 `sar %eax`
    //（N87）。值域 0..0xffff，`/2` 与 `>>1` 语义等价。
    // 另两处已修：m_users 循环拷贝（+5）、m_matrices 拆具名成员消数组构造循环+EH。
    // b28d-solo-CAssaultMgr: ORIG 0x82e6055 段用循环拷贝 m_users
    //（`movl $0,-0xc; jmp cond; ...; addl $1,-0xc; cmpl $1; setle; test; jne`），
    // 展开赋值恒少 5 条。
    for (int i = 0; i <= 1; ++i)
        m_users[i] = users[i];
    m_anchor.m_x = (unsigned short)m_users[0]->get_posX()
                 + ((unsigned short)m_users[1]->get_posX() / 2);
    m_anchor.m_y = (unsigned short)m_users[0]->get_posY()
                 + ((unsigned short)m_users[1]->get_posY() / 2);
    // ORIG 82e6101..82e6208：每个 MakeCandidatePoint 各自构造 Point2d（P48 去缓存）。
    // b28d-solo-CAssaultMgr 真语义 bug 修正：ORIG 0x82e6189 段 m_matrices[0] 的
    // a=&(-0x24)=Point2d(users[0])、b=&(-0x2c)=Point2d(users[1])（原码实参序
    // 为 (users[0],users[1])，旧写 (users[1],users[0]) 反了）；m_matrices[1]
    // 的 a=&(-0x14)=users[1]、b=&(-0x1c)=users[0] 与旧写一致。
    m_matrix0.MakeCandidatePoint(
        Point2d(m_users[0]->get_posX(), m_users[0]->get_posY()),
        Point2d(m_users[1]->get_posX(), m_users[1]->get_posY()));
    m_matrix1.MakeCandidatePoint(
        Point2d(m_users[1]->get_posX(), m_users[1]->get_posY()),
        Point2d(m_users[0]->get_posX(), m_users[0]->get_posY()));
