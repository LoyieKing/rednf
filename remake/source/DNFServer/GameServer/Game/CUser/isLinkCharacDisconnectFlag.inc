// b28d-solo-CUser：CUser.h:2718 经 Main 代落 unsigned char→bool（P292：ORIG 调用点
//   0x08652c9a `call; test %al,%al; je` 无 setcc）。ORIG 定义体 0x086973f8 直返
//   `movzbl 0x703d9; pop; ret`（P9 直返字节）⇒ 体写 (bool) 转换试直返形。
bool CUser::isLinkCharacDisconnectFlag()
{
    return (bool)m_breakAway.m_field15;
}