void stPartyTelePort::reset()
{
    // P68：ORIG 0x0822d764 实参序「0x4→0x0→this」＝ 无 cast 形（cast 引发
    // 4.4 先载 this）。
    memset(this, 0, 4);
