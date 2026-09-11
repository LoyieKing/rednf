// ---------------------------------------------------------------------------
// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x080e94dc 空体（仅 push/pop/ret）。
// ORIG 的 C1/D1 为头内 inline 弱符号（不参与键名）⇒ ORIG 键名 = StartEvent；
// 我方 ctor 为外联强符号，故必须把 StartEvent 定义置于其前。
void CEventAdvanceAltarOpen::StartEvent()
{
}