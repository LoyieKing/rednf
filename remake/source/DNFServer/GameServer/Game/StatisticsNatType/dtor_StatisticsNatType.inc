StatisticsNatType::~StatisticsNatType()
{
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it = m_counts.begin();
    while (it != m_counts.end())
    {
        RESULT_COUNT* rc = it->second;
        if (rc != 0)
        {
            delete rc;
            rc = 0;
        }
        it++;
    }
