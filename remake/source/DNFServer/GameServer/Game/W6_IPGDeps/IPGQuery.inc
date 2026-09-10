void CIPGHelper::IPGQuery(CUser* user)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_QUERY, true);
    data->m_callback = 0;
    m_mgr->PutIPGRequest(data, true);
