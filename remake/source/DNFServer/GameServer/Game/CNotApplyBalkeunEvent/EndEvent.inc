// ---------------------------------------------------------------------------
// ORIG 0x081b9ed2：IsEventing(0) 为真时清事件标志并打日志。
void CNotApplyBalkeunEvent::EndEvent()
{
    if (IsEventing(0))
    {
        SetEventFlag(false);
        cMyTrace tr(__PRETTY_FUNCTION__, 0x28, 0);
        tr("Not Apply Balkeun Event End");
    }
}
