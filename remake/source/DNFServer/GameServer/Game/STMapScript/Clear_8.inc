void STAssignPassiveObject::Clear()
{
    // 定义置于 ctor 之后：4.4 对"被调函数定义未前置"的 ctor 帧发 sub $0x18，
    // 与 ORIG 0x089e8440 帧形一致（探针 /tmp/near7/p7-p8）。
    m_0 = -1;       // +0x00 movl $0xffffffff
    m_4 = 0;        // +0x04
    m_8 = 0;        // +0x08
    m_c = 0;        // +0x0c
}