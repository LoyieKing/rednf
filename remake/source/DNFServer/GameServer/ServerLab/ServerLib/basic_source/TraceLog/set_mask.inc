void TraceLog::set_mask(int bit, const char* yesno)
{
    // ORIG：外层早退（if NULL return）→ else-if 链尾有 jmp EPI + nop。
    if (yesno == NULL)
    {
        return;
    }
    if (strcmp(yesno, "yes") == 0)
    {
        logmask = logmask | (1 << bit);
    }
    else if (strcmp(yesno, "no") == 0)
    {
        logmask = logmask & ~(1 << bit);
    }
