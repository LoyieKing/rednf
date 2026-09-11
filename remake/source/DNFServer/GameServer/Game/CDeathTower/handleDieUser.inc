int CDeathTower::handleDieUser(CUser* user)
{
    // [b28d-solo-CDeathTower] verdict=AE（orig 102 / our 102，P167 CLEAR 15/15）：修法 =
    // ①ORIG 0x08465a53 循环体 body 后 `jmp` 跳出（0x08465a7b 再 jmp 到 0x08465a90）
    //   ⇒ 源码是循环内 break、`_checkMemberDie()` 在循环**外**统一判定；
    // ②P225 推广：两守卫改嵌套正向 if（continue 守卫形 4.4 恒产 xor/setne 反极性
    //   + 中继岛），复现 ORIG 0x08465989 `test; je` 与 0x084659a8 `cmp; sete; test; je`
    //   两守卫同落 LOOP_END。P191 双初始化保留。
    int i = 0;
    for (i = 0; i <= 3; ++i)
    {
        // [b28d-solo-CDeathTower] P225 推广：ORIG 两守卫各自 `test/sete + je`
        // 同落 LOOP_END（0x08465989/0x084659ad 均 je 08465a7d）⇒ 源码为嵌套正向 if
        //（continue 守卫形 4.4 恒产 xor/setne 反极性 + 中继岛）。
        if (m_party->checkValidUser(i))
        {
            // ORIG 0x084659a8 `cmp 0xc(%ebp),%eax; sete %al; test %al,%al; je`。
            if (m_party->get_user(i) == user)
            {
                if (!m_playData.isMemberAlive(i))
                {
                    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x2b, 0x12);
                    return 0;
                }
                m_playData.setMemberAlive(i, false);

                CPacketHandler packet;
                // [b28d-solo-CDeathTower] P0：ORIG 0x08465a.. `movzwl %ax,%eax` 直传
                // （无 uid 具名槽）⇒ 内联实参。
                packet.makeDieState(user->get_unique_id());    // ORIG：死亡状态包
                m_party->send_to_party(packet);
                break;
            }
        }
    }
    if (_checkMemberDie())
    {
        _onFinishStage();
        _onPrepareFinishTower();
    }
    return 1;
}