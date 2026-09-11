unsigned char CPartyResultRecvFlag::IsRecv(int idx) const
{
    if (!IsValidIdx(idx))
    {
        return 0;
    }
    return m_flag[idx];
}