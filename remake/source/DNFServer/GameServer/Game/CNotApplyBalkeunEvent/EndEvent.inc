void CNotApplyBalkeunEvent::EndEvent()
{
    if (IsEventing(0))
    {
        SetEventFlag(false);
        cMyTrace tr(__PRETTY_FUNCTION__, 0x28, 0);
        tr("Not Apply Balkeun Event End");
    }
