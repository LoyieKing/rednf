// CAutoMarketConditionsControl：ORIG ctor 0x082f8108 —— m_vec3dc(+0x3dc) 默认
// 构造（非平凡成员先行，0x082f8119），随后函数体逐句置 0，顺序
// +0x380 → +0x00 → +0x3e8（0x082f8121/26/2f，每句重取 this = 体赋值形 P29）。
CAutoMarketConditionsControl::CAutoMarketConditionsControl()
{
    m_field380 = 0;
    m_field00 = 0;
    m_field3e8 = 0;
}