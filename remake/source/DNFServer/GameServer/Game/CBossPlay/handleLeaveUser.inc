// ============================================================================
// handleLeaveUser（ORIG 0x08149724）
// [N18b 封顶] ORIG 0x08149771 对 get_user(i)==user（call 结果 vs 形参比较）生成
// `cmp 0xc(%ebp),%eax; sete %al; test` 物化；4.4 恒折直 cmp+jne 双岛，无源码写法
// 可产 sete（N18b 判据），其余 128 条逐条同形。
// [b27 复证] N93附 尾块序亦不适用（此处非 return 块而是 && 汇聚物化；
// 探针 44 再证 4.4 双岛恒 jne+mov$1/mov$0）。维持封顶。
// [b27 DL3R] 返回类型 int→bool（ORIG 调用点 CBossTower 0x08143b04 `test %al`
// 单字节直判实证）：callee 行数 128:131 两型实测完全一致（零成本），受益方为
// 调用点 xor 消形。
// ============================================================================
bool CBossPlay::handleLeaveUser(CUser* user)
{
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        // P133：ORIG 两守卫独立 jne（checkValidUser 直跳 + get_user==user
        // sete 岛），合并 && 生 bool 展开形。拆两条 + 直比（同型件
        // CDeathTower::handleLeaveUser 清绑后 AE 先例）。
        if (m_party->checkValidUser(i))
        {
        if (m_party->get_user(i) == user)
        {
            if (m_memberField[i] == 0 && user->get_state() == 0xc)
            {
                user->getHackAnalyzer()->addServerHackCnt(user, (ENUM_HACKTYPE)0x13a, 1, 0, 0);
            }
            m_party->leave_user(user, (ENUM_PARTY_INFO_TYPE)2);
            setMemberAlive(i, false);
        }
        }
    }

    G_GameWorld()->out_from_dungeon(user);

    PacketGuard packet;
    packet.put_header(0, 2);
    packet.put_byte(0);
    packet.put_short(1);
    user->make_basic_info((char*)&packet, '\0');
    packet.finalize(true);
    G_GameWorld()->send_all(packet, user);
    return 1;
}
