void RwSpinLock::enterReadToValue(long volatile* value)
{
    while (true)
    {
        long ostate = *value;
        if ((ostate & 0xc0000000) == 0)
        {
            if (__sync_val_compare_and_swap(value, ostate, ostate + 1) == ostate)
                return;
        }
        YieldProcessor();
    }
