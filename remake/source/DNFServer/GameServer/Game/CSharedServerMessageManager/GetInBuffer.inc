T* Stream::GetInBuffer()
{
    // [b28d-solo-CSharedServerMessageManager 判词 · AE] ORIG 0x08602160/0x0860219e（两实例
    // <SIG_SELECT>/<SIG_UPDATE> 均 fresh 33=33，TU near=0 diff=0，p167 4/4 CLEAR）：
    // m_buffer 载入 %eax 后中转 %edx
    // （`mov 0x8(%eax),%eax; mov %eax,%edx`）。4.4 对 `&m_buffer[m_inPtr]` / `m_buffer+idx`
    // 均折单段 `mov 0x8(%eax),%edx`；改为整型加法 + 回 cast（探针 /tmp/gs_probe.cpp f5，
    // `(char*)((unsigned int)m_buffer + (unsigned int)m_inPtr)`）即逐条复现两段装载形。
    // 32 位下指针算术与无符号整型加法逐位等价（i386 无分段，m_buffer 非空）。
    while (!enable_in((int)sizeof(T)))
        resize(size() * 2);
    char* p = (char*)((unsigned int)m_buffer + (unsigned int)m_inPtr);
    in_ptr((int)sizeof(T));
    return (T*)p;
}