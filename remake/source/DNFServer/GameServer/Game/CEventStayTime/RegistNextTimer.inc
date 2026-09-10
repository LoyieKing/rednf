void CEventStayTime::RegistNextTimer()
{
    if (!IsEventing(0))
        return;
    // [b28-L14] 残差① 封顶 N7：IsEventing 虚调用 ORIG `mov (%eax),%edx;
    // mov this,%eax; call *%edx` vs 4.4 恒把间接 call 目标钉 %eax
    // （探针 /tmp/l14p/virt2.cpp f/g/h 四形实测）。
    // 残差② 已翻正：ORIG 0x816befa `mov 0x10(%eax),%ebx` 单段直落 callee-saved
    // 并跨 G_TimerQueue() 保活——P226 语句表达式形无效，但**裸 register（无
    // asm 绑定，P0 合规）**使 4.4 强度归约为同一条 `mov disp(%eax),%ebx`
    // （探针 /tmp/l14p/plreg.cpp a2/a3/a5：a4 普通局部两段、a5 裸 register 单段）。
    register unsigned int interval = m_intervalStayTime;
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0,
                                static_cast<TIMER_MESSAGE>(0xad),
                                interval, 0, 0);
