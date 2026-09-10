bool CActionPointEtcParameter::IsValidActionGroupIndex(_ActionGroupIndex group)
{
    return (int)group >= 1 && (int)group <= 0x2d;
