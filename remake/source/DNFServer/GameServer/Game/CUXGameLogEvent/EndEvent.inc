// ---------------------------------------------------------------------------
// ORIG 0x0811774e：SetEventFlag(false) + cMyTrace(<prett>,0x1e,0)
// ("CUXGameLogEvent Event End!")。
// [Q7SemanticAxis 2026-09-11] 同上：ORIG prett 串实读 0x08b4f100 size 0x29 =
// `virtual void CUXGameLogEvent::EndEvent()`（40 字符 + NUL）。
void CUXGameLogEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1e, 0);
    tr("CUXGameLogEvent Event End!");
}