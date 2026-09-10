void cUserHistoryLog::AuctionPoint(SIG_AUCTION_LOG_MESSAGE_PG* msg)
{
    switch (msg->m_type)
    {
        case 0:
            m_user->LogHistory("POINT_reg_s,%llu,%d,%u,%hhu,%d,%hu,%u",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg15,
                msg->m_arg16, msg->m_arg1a, msg->m_arg1c);
            break;
        case 1:
            m_user->LogHistory("POINT_reg_f,%llu,%d,%u,%hhu,%d,%hu,%u",
                msg->m_key, msg->m_arg09, msg->m_arg11, msg->m_arg15,
                msg->m_arg16, msg->m_arg1a, msg->m_arg1c);
            break;
        case 2:
            m_user->LogHistory("POINT_bid_s,%llu,%d,%d",
                msg->m_key, msg->m_arg09, msg->m_arg0d);
            break;
        case 3:
            m_user->LogHistory("POINT_bid_f,%llu,%d,%d",
                msg->m_key, msg->m_arg09, msg->m_arg0d);
            break;
        default:
            m_user->LogHistory("POINT_unknown,%d", (int)msg->m_type);
            break;
    }
