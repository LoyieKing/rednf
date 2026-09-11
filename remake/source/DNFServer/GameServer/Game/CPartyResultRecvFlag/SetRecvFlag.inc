void CPartyResultRecvFlag::SetRecvFlag(int idx)
{
    if (!IsValidIdx(idx))
    {
        return;
    }
    m_flag[idx] = 1;
}