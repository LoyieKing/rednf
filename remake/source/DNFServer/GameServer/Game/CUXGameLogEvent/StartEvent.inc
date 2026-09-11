// ---------------------------------------------------------------------------
// ORIG 0x081176e2：SetEventFlag(true) + cMyTrace(<prett>,0x13,0)
// ("CUXGameLogEvent Event Start!")。
// [Q7SemanticAxis 2026-09-11] §6.5 串内容轴：ORIG 第一实参是 __PRETTY_FUNCTION__ 展开
// `virtual void CUXGameLogEvent::StartEvent()`（实读 0x08b4f140 size 0x2b = 42 字符 + NUL；
// 我方曾手写 "CUXGameLogEvent::StartEvent()" ⇒ 缺 `virtual void ` 前缀，trace 文本与 ORIG 不符，
// 且 prett 符号不发射）。本函数为 virtual ⇒ 宏展开即带 `virtual ` 前缀，逐字节对齐。
void CUXGameLogEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x13, 0);
    tr("CUXGameLogEvent Event Start!");
}