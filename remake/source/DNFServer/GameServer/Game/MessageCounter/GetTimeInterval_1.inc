long long CUnixTimeInterval::GetTimeInterval()
{
    struct timeval now;
    gettimeofday(&now, 0);

    // [b28d-51] ORIG 0x0857028f 段：x87 全程在栈上（无具名 double 局部往返），
    // fldcw 设 RC=11(截断) 后直接 fistpll 返回，无 `>=0.5` 进位调整。
    return (long long)((double)((int)(now.tv_sec - m_last.tv_sec) * 1000) +
                       ((double)((int)(now.tv_usec - m_last.tv_usec)) / 1000.0));
