// ============================================================================
// importEventAvatarListScript — ORIG 0x0895e1ed（init.md +0x688b 调用，
// "Event/EventAvatarList.evt" → m_eventAvatarList）。
// 语义：loadRDARScriptFile("",path)；clear；扫描 "[avatar list]" 后连续
// ScanInt 入 set（ScanInt 失败回主循环）；"[event avatar list end]" 置返回
// 标记。返回该标记（未见到 end 段返回 false）。
// ============================================================================
bool importEventAvatarListScript(EventAvatarListScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    // [b28c-lane24] ORIG 0x0895e1ed 序：p(-0x10)=s 的 store 先于 s->clear() 的 call；
    // 死变量 unused0(-0xc) 的 movl 先于 seenEnd(-0x1d) 的 movb ⇒ 按 ORIG 声明序。
    EventAvatarListScript* p = s;              // ORIG -0x10 arg 缓存（insert 用）
    s->clear();
    std::string token;
    int unused0 = 0; (void)unused0;   // ORIG 栈槽 -0xc 同款死变量（-O0 保留存储）
    bool seenEnd = false;
    int value;                                 // ORIG -0x24（prologue 无初始化）

    for (;;)                                   // ORIG 4.1 while 落点形（check 在头）
    {
        if (!ScanType(token, true))
            break;
        if (token == "[avatar list]")
        {
            value = -1;
            for (;;)
            {
                value = ScanInt(&seenEnd);          // 复用同一 bool（ORIG 同栈槽）
                if (!seenEnd)
                {
                    break;
                }
                p->m_avatars.insert(value);
            }
        }
        else if (token == "[event avatar list end]")
        {
            seenEnd = true;
            // [b28d-47] ORIG 0x0895e1ed +0x0e3：置位后 jmp +0xea（直接退出主循环），
            // 非回主循环头（+0x56）。缺 break 会多扫一轮，语义差异。
            break;
        }
    }
    return seenEnd;
}