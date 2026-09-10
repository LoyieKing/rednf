void COnlinePreliminaryEvent::EndEvent()
{
    SetEventFlag(false);
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(false);
    }
