// ============================================================================
// ORIG 0x08166700（EndEvent.md 全文已读）：先 cMyTrace("virtual void
// HeroMissionEvent::EndEvent()",0x20d,0)("[HeroMission] End!")，再
// SetEventFlag(false)。镜像布局以 m_isEventing=0 等价。
// ============================================================================
void HeroMissionEvent::EndEvent()
{
    cMyTrace tr(__PRETTY_FUNCTION__, 0x20d, 0);
    tr("[HeroMission] End!");
    // ORIG 0x816674a：SetEventFlag(false)（写 +0x08 标志，与 IsEventing 同字段）
    SetEventFlag(false);
}