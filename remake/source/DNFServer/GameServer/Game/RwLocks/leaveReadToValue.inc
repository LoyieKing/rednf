void RwSpinLock::leaveReadToValue(long volatile* value)
{
    while (true)
    {
        long ostate = *value;
        if (__sync_val_compare_and_swap(
                value, ostate, ((ostate & 0x3fffffff) - 1) | (ostate & 0x40000000)) == ostate)
            return;
        YieldProcessor();
    }
}