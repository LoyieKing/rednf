// ---------------------------------------------------------------------------
// ORIG T 0x82ea716 (size 0x183)：TimeoutConnectP2P()
// 超时尚未 P2P 连接的参战者逐一登记/通报，随后重试开战；若期间某空场转失败
// （state 变 6）则提前返回 0。
// ---------------------------------------------------------------------------
// [b28c-L7][SIGFIX:pvp_assault::CAssaultPlace::TimeoutConnectP2P] char→bool
//（ORIG __PRETTY_FUNCTION__ 串 "bool ...TimeoutConnectP2P()"，tsv 第 16 行；
// mangled 不变；`started` 局部改 bool——ORIG 尾 `movzbl -0x19(%ebp),%eax` 无
// setne 归一化 + 消费点 `cmpb $0x0,-0x19(%ebp); je`，P292/P321-T3 判 bool）。
// [b28d-solo-W6_AssaultPlaceDepsA] AE：P0 删 log 具名槽（改链式直传，槽号一次
//   全对齐）+ P227 name 内联 + P223 双标签 goto 复现三条守卫岛（Empty 的
//   jne→nop 岛 + 语句表达式 setne 守卫 + body 尾 jmp）⇒ 262=262 ndiff=0；
//   p167 CLEAR。ORIG 0x082ea716
bool CAssaultPlace::TimeoutConnectP2P()
{
    if (m_state != 3)
        throw (ENUM_ERROR)0x13;

    for (int i = 0; i < 8; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ea777 `test %al,%al; jne +0x304`
        //   （nop 岛）+ 0x082ea796 `cmp $0x4,%eax; setne %al; test %al,%al; je +0x305`
        //   + body 尾 0x082eaa18 `jmp +0x305` ⇒ 两条独立守卫（P223 双标签 goto 形；
        //   合并 `&&` 使 4.4 产 xor $1 短路 + mov$1/jmp/mov$0 岛，缺 jne/nop/jmp 三条）。
        //   语句表达式保 setne 无槽物化（P226 条件向）。
        if (m_assaulters[i].Empty())
            goto LOOP_CONT;
        if (({ (int)m_assaulters[i].GetState() != 4; }))
        {
            if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
                // [b28d-solo-W6_AssaultPlaceDepsA] P0：ORIG 0x082ea7b0 段无 log 具名
                //   局部（`call GetPowerWarLog; mov %eax,(%esp); call IncP2PTimeOut…`
                //   寄存器直传）⇒ 删局部改链式调用。
                GlobalData::s_power_manager->GetPowerWarLog()
                    ->IncP2PTimeOutCountSecede();
                // [b28d-solo-W6_AssaultPlaceDepsA] P221：ORIG 槽序 -0x5c/-0x4c/
                //   -0x3c/-0x2c（先声明深）⇒ 全表达式临时链。
                cMyTrace(__PRETTY_FUNCTION__, 0xa80, 0)("POWER WAR P2P SECEDE");
                for (int j = 0; j < 8; ++j) {
                    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ea81b `test %al,%al;
                    //   jne +0x1d3`（nop 岛）+ body 尾 0x082ea890 `jmp +0x1d4`
                    //   ⇒ P223 双标签 goto 形（`if(!Empty())` 合并形产 xor$1/je 反极性）。
                    if (m_assaulters[j].Empty())
                        goto JCONT;
                    {
                        if ((int)m_assaulters[j].GetTeam() == 0) {
                            // [b28d-solo-W6_AssaultPlaceDepsA] P227：同 StartAssault——
                            //   ORIG `mov %eax,%ebx` 升格保活，具名 name 落槽。
                            cMyTrace(__PRETTY_FUNCTION__, 0xa88, 0)(
                                "BLUE TEAM : %s",
                                ((CUserCharacInfo*)m_assaulters[j].GetUser())
                                    ->getCurCharacName());
                        } else {
                            cMyTrace(__PRETTY_FUNCTION__, 0xa8a, 0)(
                                "RED TEAM : %s",
                                ((CUserCharacInfo*)m_assaulters[j].GetUser())
                                    ->getCurCharacName());
                        }
                    }
                    goto JEND;
JCONT:
                    ;
JEND:
                    ;
                }
                // ORIG 0x082ea909 `call GetUser; mov %eax,%ebx` 直传 GetUser 返回值
                //（无 getCurCharacName 调用，P171 照抄 ORIG；p167 EXTRA 归零）。
                cMyTrace(__PRETTY_FUNCTION__, 0xa8d, 0)(
                    "SECEDE : %s", m_assaulters[i].GetUser());
            }

            CUser* leaveUser = m_assaulters[i].GetUser();
            if (GlobalData::s_power_manager->IsPowerWarEventOn()
                && ((CUserCharacInfo*)leaveUser)->getCurCharacVill() == 7)
            {
                PacketGuard packet;
                packet.put_header(0, 0xe6);
                packet.put_byte(2);
                packet.finalize(true);
                leaveUser->Send(packet);
            }
        }
        goto LOOP_END;
LOOP_CONT:
        ;
LOOP_END:
        ;
    }

    for (int i = 0; i < 8; ++i) {
        // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082eaa48 `jne +0x388`（nop 岛）+
        //   `cmp $0x4,%eax; setne %al; test %al,%al; je +0x389` ⇒ 同第一循环形。
        if (m_assaulters[i].Empty())
            goto LOOP2_CONT;
        if (({ (int)m_assaulters[i].GetState() != 4; }))
        {
            LeaveAssaultPlace(i, true, false);
            if (m_state == 6)
                return false;
        }
        goto LOOP2_END;
LOOP2_CONT:
        ;
LOOP2_END:
        ;
    }

    // [b28d-solo-W6_AssaultPlaceDepsA] P292/P321-T3：ORIG 0x082eaab4 `call StartAssault;
    //   mov %al,-0x19` + 尾 `movzbl -0x19(%ebp),%eax`（无 setne 归一化）+ 消费点
    //   `cmpb $0x0,-0x19(%ebp); je` ⇒ started 是 bool 局部（char 使 4.4 尾多 setne）。
    bool started = StartAssault();
    if (started) {
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x04');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x04');
    }
    return started;
}