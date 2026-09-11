// ---------------------------------------------------------------------------
// ORIG 0x0810a790：SetEventFlag(true) + cMyTrace(__FUNCTION__,0x18,0)
// ("Clear reward card event Start")。
void CClearRewardCardEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x18, 0);
    tr("Clear reward card event Start");
}