bool CAssaultPlace::_CheckPowerWarInPlayer(ENUM_POWER_SIDE_TYPE side,
                                           ENUM_POWER_WAR_WINNER_OR_LOSER winner,
                                           int& winCnt, int& loseCnt,
                                           int& aliveCnt)
{
    // b28 BM4 判词（jump-threading 跨版本负例族）：fresh .o 实残 1 条 = 块尾
    // PacketGuard 段前 4.4 多发 `jmp <T>` 中继（ORIG 4.1 直落）。助记符序列
    // 其余逐条对齐（含空块 `{}` 限定 u 作用域形），语义完成，byte 层封顶。
    for (int i = 0; i <= 7; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        {
            CUser* u = m_assaulters[i].GetUser();
            if (u != 0 && u->isJoinPowerWar()) {
                if (winner == 1) {
                    if (u->getPowerSide() == side) {
                        if (m_assaulters[i].GetState() == 5)
                            aliveCnt++;
                        winCnt++;
                        m_assaulters[i].SetPowerWarWinner(true);
                        u->SetCurCharacSuperState(true);
                    } else {
                        loseCnt++;
                        m_assaulters[i].DieUser();
                        u->setPowerWarHP(100);              // 0x64
                    }
                } else {
                    if (u->getPowerSide() == side) {
                        loseCnt++;
                        m_assaulters[i].DieUser();
                        u->setPowerWarHP(100);
                    } else {
                        if (m_assaulters[i].GetState() == 5)
                            aliveCnt++;
                        winCnt++;
                        m_assaulters[i].SetPowerWarWinner(true);
                        u->SetCurCharacSuperState(true);
                    }
                }
                PacketGuard packet;
                _MakeChangeHp(&packet, i, u->getPowerWarHP() & 0xffff);
                _SendPacketToVill(packet);
            }
        }
    }
    return 1;
