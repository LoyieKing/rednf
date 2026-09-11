// ---------------------------------------------------------------------------
// ORIG 0x081b9e4c：同 StartEvent()（忽略 param）。
void CNotApplyBalkeunEvent::StartEvent(Word_Param param)
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        // [b28c-L15 P227] 同 StartEvent()（ORIG 0x081b9e66），忽略 param。
        cMyTrace(__PRETTY_FUNCTION__, 0x1f, 0)(
            "Not Apply Balkeun Event Start! server(%d)",
            G_CEnvironment()->m_serverEnvir.m_channelNo);
    }
}
