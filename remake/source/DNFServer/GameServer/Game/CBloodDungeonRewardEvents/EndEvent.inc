void CBloodDungeonRewardFirstEvent::EndEvent()
{
    // ORIG 0x0811bd8c：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__, 0x2d, 0)。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x2d, 0);
    tr("CBloodDungeonRewardFirstEvent End!");
}