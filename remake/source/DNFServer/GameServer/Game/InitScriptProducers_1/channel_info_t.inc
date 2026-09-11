// ORIG W 0x088dd2a8：发射序=三个 string 构造（+0x00/+0x08/+0x0c）**先于**
// 标量清零（+0x04/+0x10/+0x14）⇒ 4.1 类成员构造分组在前；4.4 初始化列表按
// 声明序混合发射，故标量改 body 赋值复现分组序。
// 注意：m_gridPercent() 值初始化会被 gcc 展开成 9 次循环（36B），与 ORIG
// memset 0x28（40B）不一致，故显式 memset 全部 10 个 float。
channel_info_t::channel_info_t()
    : m_name(),
      m_dungeonName(),
      m_dungeonKey()
{
    m_channelType = 0;
    m_pBonusRates = 0;
    m_bonusRate = 0;
    memset(m_gridPercent, 0, sizeof(m_gridPercent));
}
