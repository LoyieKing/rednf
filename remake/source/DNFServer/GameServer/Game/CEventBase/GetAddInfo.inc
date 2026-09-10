stADDINFO CEventBase::GetAddInfo()
{
    // ORIG 0x080c84d0：返回 this+0x04 的 stADDINFO（4 字节非 POD → gcc 4.4
    // sret：隐藏 out 指针 + this 栈参，ret $4；本定义编译产物与 ORIG 逐条同形，
    // 2026-08-28 OrigEventLeaf8f 由 int 修正为 stADDINFO，mangled 不变 Ev）。
    return m_addInfo;
