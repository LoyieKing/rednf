void CIPGHelper::IPGRequest(SIPGData* data)
{
    data->m_recordLen = m_dataTableA[data->m_type];
    strncpy(data->m_envCode, m_str24, 0x14);
    data->m_productCode =
        (unsigned short)m_dataTableB[data->m_type];
    m_mgr->PutIPGRequest(data, true);
}
