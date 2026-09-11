// ORIG SetLastGrade 0x85d4768：grade<0 || grade>0x26 → 0；否则 this+0=grade，返回 1。
int RefPvpGrade::SetLastGrade(int grade)
{
    if (grade < 0 || grade > 0x26)
    {
        return 0;
    }
    m_lastGrade = grade;
    return 1;
}