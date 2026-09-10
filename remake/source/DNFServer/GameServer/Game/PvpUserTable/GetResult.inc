int PvpUserTable::GetResult(int team, bool isWinTeam, int mode)
{
    if (!IsValidTeam(team))
    {
        return -1;
    }

    if (isWinTeam)
    {
        if (team != 2 && team != 1)
        {
            cMyTrace tr(__PRETTY_FUNCTION__, 0x1a0, 5);
            tr("\xea\xb2\xb0\xed\x88\xac\xeb\xa1\x9c\xec\xa7\x81"
               " \xec\x98\xa4\xeb\xa5\x98"
               " PvpUserTable::GetResult Invalid Team<%d>", team);
            return 0;
        }
        int winSide = (team == 1) ? 2 : 1;
        // 0x085d511d..0x085d512a：`movzbl 0x344(%edx,%eax,1),%eax; xor $0x1,%eax;
        // test %al,%al; je` ⇒ 原码是"值即 bool 的取反"（技巧 1/19），
        // 写 `!= 1` 时 4.4 出 `cmp $0x1,%al`。
        if (m_winTeam == team && !m_dan[winSide] &&
            m_teamVsTeam[m_winTeam][team] != 0.0f)
        {
            return 0;
        }
    }

    // 常数经 .rodata 实测核对（objdump -s -j .rodata 0x08cc27d0 段）：
    //   0x08cc27d8 = 1000.0f、0x08cc27dc = 40.0f、0x08cc27e0 = 100.0f、
    //   0x08cc27e4 = 120.0f ⇒ 四分支乘除常数与此前一致（复核通过）。
    // 关键：**不能有 f / v 中间局部**。ORIG 每个边界测试、trace 实参与 return
    // 各自完整重算一遍 x87 链（0x085d5433/0x085d54c4/0x085d550b/0x085d55b6 段
    // 各出现 4 次 flds+shl+fmulp+fldcw+fistpl），指令数差即本函数 gap 主体；
    // 一旦引入具名局部，4.4 会复用 ⇒ 少 ~110 条（技巧 36/37 的浮点推广）。
    if (G_GameWorld()->IsCharacterPvPExpRevisionChannel())
    {
        // b28d-solo-PvpUserTable：ORIG 分支物理序 = A(mode<=14, [team][team],
        //   +0x128) → B(mode>14, [m_winTeam][team], +0x28d) → C(m_winTeam==team,
        //   ×120, +0x410)。4.4 对「独立 if + return」恒把 then 放前 ⇒ 写成
        //   `if (m_winTeam != team) { if (mode <= 0xe) A else B } else C`
        //   （P222/P47 分支序翻正）。
        if (m_winTeam != team)
        {
            if (mode <= 0xe)
            {
                // A：0x085d5182 区（line 0x1b9），下标 [team][team]
                // （0x085d5195 `shl $0x3,%ecx; lea (%ecx,%edx,1)` ecx=edx=team）
                if ((int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f) < -100000 ||
                    100000 < (int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f))
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x1b9, 5)("pvp_exp_error1 %d, %f",
                       (int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f),
                       (double)m_teamVsTeam[team][team]);
                }
                return (int)(m_teamVsTeam[team][team] * 1000.0f * 40.0f / 100.0f);
            }
            else
            {
                // B：0x085d52f1 区（line 0x1c4），mode > 14
                if ((int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f) < -100000 ||
                    100000 < (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f))
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x1c4, 5)("pvp_exp_error2 %d, %f",
                       (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f),
                       (double)m_teamVsTeam[m_winTeam][team]);
                }
                return (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 40.0f / 100.0f);
            }
        }
        else
        {
            // C：0x085d5410 区（line 0x1d0），下标 [m_winTeam][team]
            // ORIG 0x085d5217 段先 `flds; fstpl -0x98` 求值 double 实参、再
            // 重算 int 并 `mov -0x74,%ebx` 缓存，全部位于 ctor call 之前
            // （P227 全表达式临时链形；具名 tr 对象 4.4 恒 ctor 后求值实参）。
            if ((int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f) < -100000 ||
                100000 < (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x1d0, 5)("pvp_exp_error3 %d, %f",
                   (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f),
                   (double)m_teamVsTeam[m_winTeam][team]);
            }
            return (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f * 120.0f / 100.0f);
        }
    }

    // 0x085d55f7 区（line 0x1dd）：非 revision 频道 ⇒ 仅 ×1000。
    // b28d-solo-PvpUserTable：ORIG trace 的 %d 实参为 0x085d569e
    //   `flds (%eax,%edx,4); fldcw -0x70; fistpl -0x74`（**无 ×1000 乘法**）
    //   ⇒ 原码写 `(int)m_teamVsTeam[m_winTeam][team]`（此前多乘 1000，真语义差异）。
    if ((int)(m_teamVsTeam[m_winTeam][team] * 1000.0f) < -100000 ||
        100000 < (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1dd, 5)("pvp_exp_error4 %d, %f",
           (int)m_teamVsTeam[m_winTeam][team],
           (double)m_teamVsTeam[m_winTeam][team]);
    }
    return (int)(m_teamVsTeam[m_winTeam][team] * 1000.0f);
