// ---------------------------------------------------------------------------
// ORIG T 0x82e910c (size 0x35)：_MakeGiveAlarm(PacketGuard*) —— 裸包 0x8b。
// 无判空防御（P140：ORIG 切片 0x082e910c 无 `cmpl $0,0xc(%ebp); je`，重建
// over-guard 已删；两调用点 152/364 均传局部 &packet，语义无损）。
// ---------------------------------------------------------------------------
void CAssaultPlace::_MakeGiveAlarm(PacketGuard* p)
{
    p->put_header(0, 0x8b);
    p->finalize(true);
}