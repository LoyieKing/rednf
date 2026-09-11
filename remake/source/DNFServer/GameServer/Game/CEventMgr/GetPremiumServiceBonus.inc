// ---- ORIG @0x0848f078 ------------------------------------------------------
// GetPremiumServiceBonus: _IsEventing(2006,1,2,0, 2006,1,11,0, EVENT_PLAIN) ? 7 : 0
int CEventMgr::GetPremiumServiceBonus()
{
    if (_IsEventing(2006, 1, 2, 0,
                    2006, 1, 11, 0,
                    EVENT_PLAIN))
    {
        return 7;
    }
    return 0;
}