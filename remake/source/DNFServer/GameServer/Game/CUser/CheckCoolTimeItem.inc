bool CUser::CheckCoolTimeItem(unsigned long itemIdx) const
{
    if (getCurCharacR() == 0)
        return false;
    const std::vector<ContinuousItemInfo>* cool =
        GetCurCharacCoolTimeItemListR();
    unsigned int curSec = (unsigned int)GlobalData::s_systemTime_.getCurSec();
    for (std::vector<ContinuousItemInfo>::const_iterator it = cool->begin();
         it != cool->end(); ++it)
    {
        // b28d-solo-CUser p167 修正：ORIG 0x0865e994 发 deEv（`(*it).`）非 ptEv。
        if ((unsigned long)(*it).m_itemIdx == itemIdx)
        {
            if (curSec < (*it).m_expireTime)
                return true;
            return false;
        }
    }
    return false;
