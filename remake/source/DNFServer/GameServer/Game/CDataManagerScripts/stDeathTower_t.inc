// ORIG C1 0x8370970：四个 200 元素表初始化 + 尾部字段，非全量 memset。
// [b28d-solo-CDataManagerScripts] AE：P310 —— `for (int i = 0; (unsigned int)i < 200; ++i)` 复现 ORIG 两段寄存器尾测；fresh clean 45=45；ORIG 0x08370970。
stDeathTower_t::stDeathTower_t()
{
    m_apcExpWeight = 0.0f;        // ORIG +0x00 位清零（float 语义：import [apc exp weight] fstps 实证）
    m_monsterExpWeight = 0.0f;    // float 语义（[monster exp weight] fstps）
    m_rewardGoldWeight = 1.0f;
    //   【N】循环条件 ORIG `mov -0xc,%eax; cmp $0xc7,%eax`（先载入寄存器），
    //   4.4 O0 恒折 `cmpl $0xc7,-0xc(%ebp)`（mem 直比），语义全等，register 形差。
    // [b28d-solo-CDataManagerScripts] P310：int 局部 + 显式 (unsigned int) 比较。
    for (int i = 0; (unsigned int)i < 200; ++i)
    {
        m_rewardExpWeight[i] = 1.0f;          // this+0x04+i*4
        m_rewardCardNum[i] = 0;               // this+0x324+i*4
        m_illusionRewardExpWeight[i] = 1.0f;  // this+0x644+i*4
        m_illusionRewardCardNum[i] = 0;       // this+0x964+i*4
    }
    memset(m_rewardItemProb, 0, sizeof(m_rewardItemProb));  // ORIG memset(+0xc88,0,12)
}