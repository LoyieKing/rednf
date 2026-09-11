// ORIG 0x8284512：IObject 基类 ctor -> 置 CTimeGate vptr -> m_openState=0。
// C++ 中基类构造、vptr 覆写由编译器自动完成，函数体只需置 m_openState=0。
// [A31-GI · P327] put_object_info 定义前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN9CTimeGate15put_object_infoER11PacketGuard`（ORIG 0x8284584）。
// ORIG 0x82844d8：put_byte(2); put_byte(m_openState 零扩展); return 1。
bool CTimeGate::put_object_info(PacketGuard& p)
{
    p.put_byte(2);
    p.put_byte((int)m_openState);
    return true;
}