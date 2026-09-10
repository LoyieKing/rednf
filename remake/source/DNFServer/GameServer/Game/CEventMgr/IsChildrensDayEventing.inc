bool CEventMgr::IsChildrensDayEventing()
{
    if (_IsEventing(0x7d6, 5, 5, 6,
                    0x7d6, 5, 8, 6,
                    EVENT_PLAIN))
    {
        return true;
    }
    return false;
