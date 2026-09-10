void CAssaultPlace::_MoveUser(pvp_assault::CDisposition* disp, int x, int y)
{
    // [b28d-solo-W6_AssaultPlaceDepsB] P0 槽-局部律修正：ORIG -0x18 不是独立
    //   dir 局部，而是 fm.m_dir（fm 基址 -0x34 + 0x1c = -0x18；fm 占
    //   -0x34..-0x15，与独立 dir 槽重叠在物理上不可能）⇒ 原码写
    //   `fm.m_dir = GetDirectionByTeam(...)`。据此槽序自洽：
    //   fm(-0x34..-0x15) > counts(-0x14/-0x10) > i(-0xc)。
    int counts[2] = {0};
    for (int i = 0; i <= 7; ++i) {
        // P223：ORIG `test %al,%al; jne <+0xd8 nop 岛>` + 体尾 `jmp <addl>; nop`
        //   ——正极性 guard-continue 岛形（if(!Empty()){...} 反产 xor+je 无岛）。
        if (m_assaulters[i].Empty())
            continue;
        CForcedMoveCharacter fm;                 // 栈上结构，偏移对应见头
        fm.m_field00 = static_cast<int>(m_assaulters[i].GetTeam());
        fm.m_slotIdx = counts[static_cast<int>(m_assaulters[i].GetTeam())];
        fm.m_user    = m_assaulters[i].GetUser();
        // asm：param_2→+0x0c（m_destVill）、param_3→+0x10（m_destArea）。
        fm.m_destVill = x;
        fm.m_destArea = y;
        fm.m_dir = pvp_assault::GetDirectionByTeam(
            static_cast<ENUM_ASSAULT_TEAM>(m_assaulters[i].GetTeam()));
        disp->MoveUser(&fm);
        counts[static_cast<int>(m_assaulters[i].GetTeam())]++;
    }
