WongWork::CSimpleSSO::CSimpleSSO()
{
    // ORIG 0x082a7260（81 insn）：m_keys[31][24] 数组成员自动构造
    // （行主序 31×24 CRijndael，列步长 0x47c、行步长 0x6ba0，覆盖 0xd0860），
    // 由编译器按 ORIG 同形生成（含异常清理 vtable 槽 0 虚析构）；尾部 m_mysql=0。
    m_mysql = 0;
