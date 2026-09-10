bool CUser::ClearSkills()
{
    // ORIG 0x0867b4ca：`movzbl 0x711d8; xor $0x1,%eax; test al; jne ea / call getCurCharacR;
    // test eax; jne f1 / ea: mov $1; jmp f6 / f1: mov $0 / f6: test al; je <reset>`
    // ⇒ A=(flag==0)||(ptr==0) 具名物化（技巧22 绑 eax 无槽）；A==0 ⇒ reset+true。
    // 首项写 (flag^1)!=0 复现 xor 物化（4.4 对 ==0 恒 test;je 无 xor）
    // [b28d-solo-CUserOnlyOrig P226/P0] ORIG 0x0867b4e6 `test %al,%al; je` 无槽
    // ⇒ 内联短路条件（旧具名 a 生 -0x9 槽、cmpb 回读、帧 0x28 vs ORIG 0x18）。
    if (((m_gmUserFlag ^ 1) != 0) || (getCurCharacR() == 0))
        return false;
    ((SkillSlot*)getCurCharacSkillW())->debugCommandResetSkill();
    return true;
