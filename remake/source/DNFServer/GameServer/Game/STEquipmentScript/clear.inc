void STEquipmentStatInfo::clear()  // ORIG 0x0898d272：10 直接 + 循环 i<=3(+0x28)
                                   // + +0x38 + 循环 j<=16(+0x3c) + +0x80/+0x84
                                   // + 循环 k<=3(+0x88)；38 int 全清，sizeof 0x98
{
    // [b28d-solo-STEquipmentScript] 旧 N81/P30 判词作废：ORIG 三处循环体发
    // `mov i,%edx; mov this,%eax; add $K,%edx; movl $0,disp(%eax,%edx,4)`，
    // 由头内 +0x08 起的二维成员数组视角 m_view[9][4] 复现（P311；探针
    // /tmp/b28d_s219/probe1.cpp T::h1/h2/h3 与 ORIG 逐条同形）。字段偏移不变。
    m_hpMax = 0;
    m_mpMax = 0;
    m_phyAtt = 0;
    m_phyDef = 0;
    m_magAtt = 0;
    m_magDef = 0;
    m_equipPhyAtt = 0;
    m_equipPhyDef = 0;
    m_equipMagAtt = 0;
    m_equipMagDef = 0;
    for (int i = 0; i <= 3; ++i)
    {
        m_view[2][i] = 0;           // +0x28..+0x34 元素耐性（字索引 8+i）
    }
    m_refAll = 0;                   // +0x38
    for (int j = 0; j <= 16; ++j)
    {
        m_view2[3][j] = 0;          // +0x3c..+0x7c 状态耐性（视图基址 +0x0c，索引 12+j）
    }
    m_refAllState = 0;              // +0x80
    m_84 = 0;                       // +0x84
    for (int k = 0; k <= 3; ++k)
    {
        m_view[8][k] = 0;           // +0x88..+0x94（字索引 32+k）
    }
}