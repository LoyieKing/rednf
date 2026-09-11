// ---------------------------------------------------------------------------
// ORIG 0x081bb5e2：清事件标志；频道类型为 0xe 时关闭匹配。
void COnlinePreliminaryEvent::EndEvent()
{
    SetEventFlag(false);
    if (G_GameWorld()->GetChannelType() == 0xe)
    {
        online_preliminary::COnlinePreliminary::SetMatchAble(false);
    }
}