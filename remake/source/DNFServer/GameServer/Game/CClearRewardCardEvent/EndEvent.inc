// ---------------------------------------------------------------------------
// ORIG 0x0810a7e2：SetEventFlag(false) + cMyTrace(__FUNCTION__,0x1e,0)
// ("Clear reward card event End")。
void CClearRewardCardEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__FUNCTION__, 0x1e, 0);
    tr("Clear reward card event End");
}