unsigned char CUser::GetBuyingGold()
{
    return m_field8ead8;   // ORIG movzbl 直返（无 test/setne）
}