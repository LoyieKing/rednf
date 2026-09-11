// ORIG T 0x82e782a _MoveUser(pvp_assault::CDisposition*, int, int)
// [b28d-solo-W6_AssaultPlaceDepsB] verdict=AE 70/70（p167 7/7 CLEAR，446 后端）
//   修法：continue 形复现 nop 岛。
//   [b28d-solo-W6_AssaultPlaceDepsB] 真 bug 修正：ORIG fm 基址 -0x34（GetTeam→
//   -0x34/+4=-0x30/GetUser=-0x2c 三证），param_2→-0x28=+0xc=m_destVill、
//   param_3→-0x24=+0x10=m_destArea；调用点 W6_AssaultPlaceDepsA.cpp:809 传
//   (m_pos.m_x=vill, m_pos.m_y=area) 与 ForcedMove 的 m_destVill/m_destArea 语义
//   自洽。原写 m_destArea/m_x 整体错位一格（m_destVill 未初始化 + 覆盖矩阵 x）。
//   残差：N7（间接 call 目标 4.4.7 恒 %eax，ORIG 用 %edx）。
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
}