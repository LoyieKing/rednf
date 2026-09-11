void CBloodDungeonRewardSecondEvent::EndEvent()
{
    // ORIG 0x0811becc：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__, 0x1b, 0)。
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1b, 0);
    tr("CBloodDungeonRewardSecondEvent End!");
}