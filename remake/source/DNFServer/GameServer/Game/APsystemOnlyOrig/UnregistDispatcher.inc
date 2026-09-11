// ---------------------------------------------------------------------------
// UnregistDispatcher —— ORIG 0x083ff156 sz 0x72
// 遍历 852 槽：槽非空则调用其 vtable +0x08 槽（ORIG
// `mov (%eax),%ecx` 其中 eax=vptr+0x8），随后清槽；等价于 delete p。
// ORIG 存在重复判空分支（0x83ff173 与 0x83ff17d 连续两次 test），忠实保留。
// 返回恒 1。
// ---------------------------------------------------------------------------
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
}