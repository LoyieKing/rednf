void CUser::CalcurateManagePoint()
{
    // ORIG 0x0868ffa0：total=-0x10（先声明远槽）/i=-0xc（int，`cmp -0xc,%eax; setg`
    // 有符号比较）/level 无槽直落 %ebx（`movzwl 0x27,%eax; movswl %ax,%ebx` 跨
    // G_CDataManagerv 存活）。short→int 拓宽作为 GetPoint 实参**直传**（probe s3）；
    // 经 `register int asm("ebx")` 中间形反产 cwtl+mov。
    int total = 0;
    for (int i = 0; i < (int)m_characList.size(); ++i)
    {
        total += G_CDataManager()->m_altarConfig.m_characManageScript->GetPoint(
            m_characList[i].m_level);
    }
    SetManagePoint(total);
