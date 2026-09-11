// ---- SpeedCtrlPassiveObjInfo（ORIG T 0x08a3ad5a）----
// +0=-1（movl 直写）、+4 byte=0（movb）；两次重读 this → 函数体逐句赋值。
// D1/D2 为 ORIG W 0x08a4eab2 trivial，由编译器隐式生成，本 TU 不定义。
SpeedCtrlPassiveObjInfo::SpeedCtrlPassiveObjInfo()
{
    m_field0 = -1;
    m_b4 = 0;
}