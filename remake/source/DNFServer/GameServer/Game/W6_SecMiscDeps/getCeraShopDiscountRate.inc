unsigned short WongWork::CUserPremium::getCeraShopDiscountRate() const
{
        // ORIG W 0x08328100 (size 0xf)：movzwl +0x860（折扣率半字）。
    return m_ceraDiscountRate;
