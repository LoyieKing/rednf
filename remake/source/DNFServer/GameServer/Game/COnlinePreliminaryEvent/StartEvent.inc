// ---------------------------------------------------------------------------
// ORIG 0x081bb4fa：置事件标志、打日志；频道类型为 0xe 时开放匹配。
void COnlinePreliminaryEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x15, 0);
    tr("COnlinePreliminaryEvent Start!");
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(true);
    }
}