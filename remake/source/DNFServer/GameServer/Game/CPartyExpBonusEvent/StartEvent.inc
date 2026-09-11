// ORIG T 0x08116c70：置事件标志后经 cMyTrace 输出
// "Party Exp Bonus Event Start! (%x%%)"（实参为 m_field0c 千分数）。
void CPartyExpBonusEvent::StartEvent()
{
    SetEventFlag(true);
    // [b28c-L15 P227] ORIG 在 cMyTrace ctor 前读 m_field0c 入 %ebx 跨 ctor 保活
    //   ⇒ 全表达式临时链形（具名对象则 ctor 后才读）。
    cMyTrace(__PRETTY_FUNCTION__, 0x10, 0)(
        "Party Exp Bonus Event Start! (%x%%)", m_field0c);
}