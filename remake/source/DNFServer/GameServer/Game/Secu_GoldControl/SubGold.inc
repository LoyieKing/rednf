void Secu_GoldControl::SubGold(unsigned int value, eMoneySubReason reason)
{
    CheckDate();
    if (reason == 0xe)
    {
        SubImportMoney(value);
    }
    else if (reason < 0xf)
    {
        if (reason == 2)
        {
            return;
        }
    }
    else
    {
        if (reason == 0x11)
        {
            MailGold(value, true);
            unsigned int accId = m_pUser->get_acc_id();
            int uid = m_pUser->GetUID();
            DB_CheckCharacName::makeRequest(uid, accId, m_mailCharName, true);
            return;
        }
        if (reason == 0x1a)
        {
            SubAuctionMoney(value);
        }
    }
    SubMoney(value);
    CheckMoneyRange();
