int CUser::GetChildrensDayEventTargetItemSlot(int idx)
{
    if (idx > 9)
        return -1;
    return m_childrensDaySlots[idx];           // P30 [i+K] 两步寻址源形
}
