// ORIG 0x86329f6：空槽返回 0。
TimerSig* TimerDispatcher::get_dispatcher(int msg)
{
    // ORIG 0x086329f6：非空分支为 fall-through（先 reload 再 jmp），空走 mov $0。
    if (m_dispatchers[msg] != 0)
    {
        return m_dispatchers[msg];
    }
    return 0;
}