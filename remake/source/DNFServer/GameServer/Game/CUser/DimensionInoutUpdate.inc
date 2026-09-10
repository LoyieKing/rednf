void CUser::DimensionInoutUpdate(bool flag1, bool flag2)
{
    // ORIG i 槽 -0xc（probe：4.4 首声明 4B 局部占 -0x10；前置 dead int 占远槽
    // -0x10 后 i 平移至 -0xc，技巧 31）。
    int deadpad;
    (void)deadpad;
    for (int i = 0; i <= 5; i++)
    {
        if (flag1)
        {
            // ORIG：value 无槽——call 结果原地 `movsbl %al,%ecx`（右实参先评估）；
            // i 次之 `mov -0xc,%eax; movsbl %al,%edx`。
            setDemensionInoutValue((char)i,
                                   (char)G_CDataManager()->get_dimensionInout(i));
        }
    }
    if (flag2)
    {
        setUltimateInoutValue(2,
                              (char)G_CDataManager()->get_limit_inout_count(
                                  (ENUM_BLOOD_TYPE)2));
    }
