void CUserCharacInfo::calcCurCharacSchoolPoint(int fatigue)
{
    static const int point[24] = {
        0, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 3, 3, 3, 3,
        4, 4, 4, 4, 5, 5, 5, 5
    };
    static const int pointSize = sizeof(point) / sizeof(point[0]);
    unsigned short used = getCurCharacUsedFatigue();
    // [b28d-L14 实证] 右操作数显式 (int) 化消 4.4 的 int→unsigned 物化四连
    // （探针 /tmp/l14_probe4.cpp c4 与 ORIG 0x08655bce 逐条全等：sub %eax,%edx 直落）。
    if (fatigue / 10 == (int)(used / 10u))
        return;
    unsigned int index = used / 10u;
    if (index > 0x17)
        index = 0x17;
    CUR->m_reliablePerson.m_schoolPoint += point[index];
