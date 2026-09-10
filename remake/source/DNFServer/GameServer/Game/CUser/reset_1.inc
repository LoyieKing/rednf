void CUser::_379::reset()
{
    m_field0 = 0;
    m_field1 = 0;
    // ORIG 0x86454e8-0x86454f5 单装载双 store = DImode 清零形；ll 成员直写复现（w2 probe 实证）。
    m_hackTradeGold = 0;
    m_hackTradeCnt = 0;
