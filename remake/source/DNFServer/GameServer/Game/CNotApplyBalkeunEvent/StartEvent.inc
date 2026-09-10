void CNotApplyBalkeunEvent::StartEvent()
{
    if (!IsEventing(0))
    {
        SetEventFlag(true);
        // [b28c-L15 P227] 原 `register int serverNo` 裸 register 占位系为把 channelNo
        //   提前到 ctor 前并落 %ebx（ORIG 0x081b9de0 `mov 0x1b0(%eax),%ebx`）；
        //   全表达式临时链形令 4.4 自然以 %ebx 跨 ctor 保活 ⇒ 删除 register 关键字。
        cMyTrace(__PRETTY_FUNCTION__, 0x16, 0)(
            "Not Apply Balkeun Event Start! server(%d)",
            G_CEnvironment()->m_serverEnvir.m_channelNo);
    }
}
