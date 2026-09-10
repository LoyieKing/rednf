void CBloodDungeonRewardFirstEvent::StartEvent()
{
    // ORIG 0x0811bd12：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__, 0x10, 0)。
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x10, 0);
    tr("CBloodDungeonRewardFirstEvent Start!");
