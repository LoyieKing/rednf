// ---------------------------------------------------------------------------
// ORIG 0x08273d0a：SetEventFlag(true) → m_limitType = param 低 ushort
// （movzwl 0xc(%ebp) → mov %edx,0xc(%eax) 整 4 字节写）→ trace（行 0x23）。
void CRestrictCharacCreationEvent::StartEvent(Word_Param param)
{
    SetEventFlag(true);
    m_limitType = param.m_duration;
    cMyTrace tr(__FUNCTION__, 0x23, 0);
    tr("Restrict Character Creation Start!");
}