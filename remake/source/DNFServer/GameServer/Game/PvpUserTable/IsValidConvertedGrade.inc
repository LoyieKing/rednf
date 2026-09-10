bool PvpUserTable::IsValidConvertedGrade(int grade)
{
    return (grade >= 0 && grade <= 0x14) ? 1 : 0;
