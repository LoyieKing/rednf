// ---------------------------------------------------------------------------
// ORIG 0x08273c44：基类构造 + vptr 置位 + m_limitType(+0x0c) 置 2。
CRestrictCharacCreationEvent::CRestrictCharacCreationEvent()
{
    m_limitType = 2;
}