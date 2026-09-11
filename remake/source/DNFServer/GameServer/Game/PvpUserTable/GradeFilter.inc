int PvpUserTable::GradeFilter(int grade)
{
    if (grade < 0 || grade > 0x22)
    {
        return -1;
    }
    if (grade > 0x14)
    {
        return 0x14;
    }
    if (grade <= 9)        // ORIG: false-branch(return 0) 作 fall-through，grade-9 作跳岛
        return 0;
    return grade - 9;
}