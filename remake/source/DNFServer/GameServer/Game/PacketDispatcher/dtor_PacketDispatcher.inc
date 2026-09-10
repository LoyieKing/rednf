PacketDispatcher::~PacketDispatcher()
{
    // [b28c-L18] N7 判词（同 TimerDispatcher::~TimerDispatcher）：虚删除
    // `call *%ecx`（ORIG vptr→%ecx/槽→%edx/基址→%eax）4.4 恒间接 call 目标=%eax，
    // 56=56 助记符全等仅寄存器轮换差，封顶。
    for (int i = 0; i <= 0x25d; ++i)
    {
        if (m_dispatchers[i] != 0) delete m_dispatchers[i];
        m_dispatchers[i] = 0;
    }
