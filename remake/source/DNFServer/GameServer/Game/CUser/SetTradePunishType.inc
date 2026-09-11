// ORIG _ZN5CUser18SetTradePunishTypeEi T 0x08689890：printf 日志后
// m_postChecksumData.m_tradePunishType |= type。
void CUser::SetTradePunishType(int type)
{
    printf(" CUser::SetTradePunishType(%d) \n", type);
    m_postChecksumData.m_tradePunishType =
        m_postChecksumData.m_tradePunishType | type;
}