// ---------------------------------------------------------------------------
// ORIG 0x08268f22：SetEventFlag(true) +
// cMyTrace(__FUNCTION__, 0x13, 0) + "CPvPLiveEvent Start!"。
// ORIG 汇编取 &_ZZN13CPvPLiveEvent10StartEventEvE12__FUNCTION__ 局部静态
// （11 字节 "StartEvent"，nm 0x08bf465b）——必须传 __FUNCTION__ 而非手写
// 签名串（P118/P139 反例：手写串只生成匿名 .rodata，符号差集残留）。
void CPvPLiveEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__FUNCTION__, 0x13, 0);
    tr("CPvPLiveEvent Start!");
}