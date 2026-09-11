// ORIG T 0x081007e8 (0x76)。
// b28d-60 verdict=AE（IDENTICAL）：m_type→enum（P254）+ m_dataTableA→int（P265）
// 两处类型旋钮修正后 37=37 逐条全等。
void CIPGHelper::IPGRequest(SIPGData* data)
{
    data->m_recordLen = m_dataTableA[data->m_type];
    strncpy(data->m_envCode, m_str24, 0x14);
    data->m_productCode =
        (unsigned short)m_dataTableB[data->m_type];
    m_mgr->PutIPGRequest(data, true);
}
