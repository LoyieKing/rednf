int RefPvpGrade::SetLastGrade(int grade)
{
    if (grade < 0 || grade > 0x26)
    {
        return 0;
    }
    m_lastGrade = grade;
    return 1;
