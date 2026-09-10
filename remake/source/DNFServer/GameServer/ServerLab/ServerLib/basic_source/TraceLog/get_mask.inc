int TraceLog::get_mask(int bit)
{
    // ORIG：logmask 先载 edx，1<<bit 走 ebx/esi（callee-saved）——
    // 操作数顺序 logmask & (1<<bit) 才能复现（反过来是 edx/ecx 直算，16 条）。
    if ((logmask & (1 << bit)) > 0)
    {
        return 1;
    }
    return 0;
