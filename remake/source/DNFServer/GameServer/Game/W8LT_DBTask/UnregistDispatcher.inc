bool DBTask::UnregistDispatcher()
{
    // [b26N7] 循环内虚调用 callee 寄存器 ORIG vfn→%ecx vs 4.4 恒 %eax（N7
    // 探针实证）；其余全等，ndiff 12 封顶。
    for (int i = 0; i <= DISPATCHER_COUNT - 1; ++i)
    {
        if (m_dispatchers[i] != 0)
        {
            delete m_dispatchers[i];            // ORIG 判空×2：if + delete 隐式判空
            m_dispatchers[i] = 0;
        }
    }
    return true;
