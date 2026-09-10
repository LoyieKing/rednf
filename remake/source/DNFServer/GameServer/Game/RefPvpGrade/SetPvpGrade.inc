int RefPvpGrade::SetPvpGrade(int grade, const RefPvpGradeData& data)
{
    if (grade < 0 || grade > 0x25)
    {
        return 0;
    }
    m_data[grade] = data;   // ORIG：8 字节整体赋值（ebx=grade 缓存 + 双 scaled store）
    return 1;
