// [b28d-solo-PvP_Room] P169 修复: ORIG 0x085da58a jg FALSE/jns BODY/单一 FALSE 块；外层正向 if 包内层 goto 复现，已归零
bool PvP_Room::IsAllMoreThanGrade(int grade)
{
    // [b28d-solo-PvP_Room P169] ORIG 0x085da58a：`cmp $0x25; jg FALSE;
    // cmp $0; jns BODY; FALSE: mov$0; jmp END; BODY:`（单一 FALSE 块、return 1
    // fall-through）。P169 形（外层正向 if + 内层 goto GRADE_BODY）逐条复现头部
    // jg/jns 与单 FALSE 块；残余 1 条 nop 落点差由 compare_tu nop 剥离口径吸收。
    if (grade <= 0x25)
    {
        if (grade >= 0)
            goto GRADE_BODY;
    }
    return false;
GRADE_BODY:
    for (int local_14 = 0; local_14 < 8; local_14 = local_14 + 1)
    {
        // [b28d-solo-PvP_Room] ORIG 0x085da5b8 把 seat 存 -0xc 槽（P0 具名
        // 局部），且第二条件发 setl/test/je（P225 嵌套 if 复现）。
        CUser* local_c = P_SEATS(local_14);
        if (local_c != 0)
        {
            if (local_c->get_pvp_grade() < grade)
            {
                return false;
            }
        }
    }
    return true;
}