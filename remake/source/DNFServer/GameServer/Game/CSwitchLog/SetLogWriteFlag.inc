void CSwitchLog::SetLogWriteFlag(register int flag)
{
    switch (flag)
    {
    case 0:
        log_write_[0] = 0;
        break;
    case 1:
        log_write_[0] = 1;
        break;
    }
