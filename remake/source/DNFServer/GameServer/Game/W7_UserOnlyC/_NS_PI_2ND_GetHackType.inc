int _NS_PI_2ND_GetHackType(unsigned int type, unsigned char b,
                           unsigned short c);

// _header（CUser.h 冻结）m_checksum 为 unsigned int；ORIG UpdatePacket 赋值段
// `mov 0x7(%eax),%edx` 要求与 int 字段 m_lastChecksum 同型（P211）。
// pack(1) 具名视图提供 int 视角，仅本 TU 使用，无成员函数 ⇒ 零符号发射。
