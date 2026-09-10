void cUserHistoryLog::Auction(SIG_AUCTION_LOG_MESSAGE_AG* msg)
{
    switch (msg->m_type)
    {
        case 0:
            m_user->LogHistory("Auction_reg_s,%llu,%d,%u,%hhu,%d,%hu,%u",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg15,
                msg->m_arg16, msg->m_arg1a, msg->m_arg1c);
            break;
        case 1:
            m_user->LogHistory("Auction_reg_f,%llu,%d,%u,%hhu,%d,%hu,%u,%hhu",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg15,
                msg->m_arg16, msg->m_arg1a, msg->m_arg1c, msg->m_arg20);
            break;
        case 2:
            m_user->LogHistory("Auction_bid_s,%llu,%d,%d,%d",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg0d);
            break;
        case 3:
            m_user->LogHistory("Auction_bid_f,%llu,%d,%d",
                msg->m_key, msg->m_arg09, msg->m_arg0d);
            break;
        default:
            m_user->LogHistory("Auction_unknown,%d", (int)msg->m_type);
            break;
    }
