float PvpUserTable::GetExpectVal(int grade1, int grade2, CUser* user)
{
    int saved1 = grade1;
    int saved2 = grade2;
    grade1 = GradeFilter(grade1);
    grade2 = GradeFilter(grade2);

    bool changed = false;
    if ((grade1 == 1 && grade2 == 0) || (grade1 == 0 && grade2 == 1))
    {
        // b28d-solo-PvpUserTable：ORIG 0x085d4b2d `cmpl $0,0x14(%ebp); je 岛` +
        //   `cmp $9,%eax; jg 岛`（两条失败路径同落 `mov $0` 块）+ `mov $1/jmp/
        //   mov $0/test %al,%al/jne`（成功才跳过回退体）⇒ 4.1 物化的是
        //   `user != 0 && grade <= 9`（正向合取，P272 的德摩根块序）。
        if (user != 0 && ((CUserCharacInfo*)user)->get_pvp_grade() <= 9)
        {
        }
        else
        {
            grade1 = saved1;
            grade2 = saved2;
            changed = true;
        }
    }

    // b28d-54 翻正：ORIG 0x085d4b4a/0x085d4b5f 两个独立守卫各自
    //   `call; xor $1,%eax; test %al,%al; je <独立 return -1 块>`（P133：
    //   合并 `||` 会生 bool 汇聚 setne 物化）。
    if (!IsValidConvertedGrade(grade1))
        return -1.0f;
    if (!IsValidConvertedGrade(grade2))
        return -1.0f;
    if (grade1 != grade2 && !changed)
    {
        if (grade1 == 0)
        {
            return 0.0f;
        }
        if (grade2 == 0)
        {
            return 1.0f;
        }
    }
    return 0.024f * (float)(grade1 - grade2) + 0.5f;
