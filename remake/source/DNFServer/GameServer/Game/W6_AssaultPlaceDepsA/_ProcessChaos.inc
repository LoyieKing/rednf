void CAssaultPlace::_ProcessChaos(int killer, int victim)
{
    if (victim < 0 || victim > 7)
        return;
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ebbd2 `cmp %edx,%eax; sete %al;
    //   test %al,%al; je <函数尾>`（零槽物化）⇒ P226 裸语句表达式条件向。
    //   ORIG 0x082ebc25 `movzbl 0x128(%edx,%eax,1),%eax; xor $1; test %al,%al; je`
    //   ⇒ P248 档② 字段 bool 化 + 裸 if(!field)，无 chaosFlag 槽。
    // 操作数序：交换成 `field == call` 使 4.4 把字段提前载入 %ebx（push %ebx
    //   劣化，实测回退）；残差 = ORIG `cmp %edx,%eax` vs 我方 `cmp %eax,%edx`
    //   角色互换（N7/N18a 族）。
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ebbd2 `call GetTeam; mov 0x8(%ebp),%edx;
    //   mov 0x124(%edx),%edx; cmp %edx,%eax` ⇒ 删 `(int)` cast（同 EvadeAssaultPlace）。
    if (({ m_assaulters[victim].GetTeam() == m_winTeam /*+0x124*/; })) {
        CUserCharacInfo* killerUser =
            (CUserCharacInfo*)m_assaulters[killer].GetUser();
        CUserCharacInfo* victimUser = (CUserCharacInfo*)m_assaulters[victim].GetUser();
        if (victimUser != 0) {
            if (!m_chaosFlags[m_assaulters[killer].GetTeam()]) {
                victimUser->AddCurCharacChaosPoint(10);
                if (victimUser->getCurCharacR() != 0) {
                    PacketGuard packet;
                    packet.put_header(0, 2);
                    packet.put_byte(0);
                    packet.put_short(1);
                    ((CUser*)victimUser)->make_basic_info((char*)&packet, 0);
                    packet.finalize(true);
                    G_GameWorld()->send_all(packet, (CUser*)victimUser);
                }
            }
            if (killerUser != 0) {
                // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ebd32 `setg %al; test;
                //   je <0x082ebd46 Kill 块>` + then 直落 `jmp <0x082ebd51 Die 块>`
                //   ⇒ 原码是 `if (point > 0) Mode; else Kill;`（写 `< 1` 产 setle 反形）。
                if (killerUser->GetCurCharacChaosPoint() > 0)
                    victimUser->IncCurCharacChaosModeCount();
                else
                    victimUser->IncCurCharacChaosKillCount();
                killerUser->IncCurCharacChaosDieCount();
            }
        }
    }
