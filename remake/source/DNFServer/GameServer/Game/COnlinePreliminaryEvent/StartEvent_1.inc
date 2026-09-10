void COnlinePreliminaryEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1f, 0);
    tr("COnlinePreliminaryEvent Start!");
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(true);
    }
