TimerDispatcher::~TimerDispatcher()
{
    // [b28c-L18] N7 判词：虚删除 `call *%ecx`（ORIG vtable 载入 %ecx、槽读 %edx、
    // 元素基址 %eax）在 4.4 恒把间接 call 目标放 %eax（元素/槽角色轮换）——
    // 36=36 助记符全等仅寄存器名差，语义同，勿再耗轮次。
    for (int i = 0; i <= 0xae; ++i)
    {
        if (m_dispatchers[i] != 0)
        {
            delete m_dispatchers[i];
        }
        m_dispatchers[i] = 0;
    }
