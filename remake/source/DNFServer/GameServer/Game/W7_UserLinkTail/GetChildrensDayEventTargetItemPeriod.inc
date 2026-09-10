int CUser::GetChildrensDayEventTargetItemPeriod(int idx)
{
    if (idx > 9)
        return -1;
    return m_childrensDayPeriods[idx];
}
