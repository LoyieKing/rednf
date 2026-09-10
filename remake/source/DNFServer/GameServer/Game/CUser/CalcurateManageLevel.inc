bool CUser::CalcurateManageLevel()
{
    CalcurateManagePoint();
    // ORIG 0x8690b??：newLevel 槽 -0x10；GetManageLevel() 结果**不落槽**直接
    // `cmp -0x10,%eax; setl;test;je`（真分支 movb $1,-0x9）；后 SetManageLevel；
    int newLevel = G_CDataManager()->m_altarConfig.m_characManageScript->GetLevel(
        GetManagePoint());
    bool up = false;
    if (GetManageLevel() < newLevel)
        up = true;
    SetManageLevel(newLevel);
    return up;
