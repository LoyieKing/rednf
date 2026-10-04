unsigned char CUser::GetBuyingGold()
{
    return m_buyingGold;   // ORIG movzbl 直返（无 test/setne）
}