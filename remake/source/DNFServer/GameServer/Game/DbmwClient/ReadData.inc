int DbmwClient::ReadData(char* dst, int len)
{
    // ORIG @0x08120b32：循环 recv 凑满 len；EINTR 重试；EOF 提前返回已读数；
    // 错误记日志返回 -1。
    // ORIG 槽：got@-0x14 < remain@-0x10 < cur@-0xc（4.4 先声明→低槽，
    // 声明序 got→remain→cur）。
    ssize_t got;
    int remain = len;
    char* cur = dst;
    // ORIG 0x08120b32：while (remain > 0) 底部 setg 守卫 + 入口 jmp；
    // recv==0 直接 return len-remain（循环内非 break）。
    while (remain > 0)
    {
        got = ::recv(m_sock, cur, remain, 0);
        if (got < 0)
        {
            if (errno == EINTR)
                continue;
            // ORIG 0x08120b32：errno 在各使用点独立宏展开（__errno_location
            // 多调，%ebx 缓存 ptr 跨 strerror call）——errNo 命名缓存形少 2 条。
            LogManager::logFormat(
                1, "localchina/DbmwClient.cpp", __FUNCTION__, 0x1dd,
                "AccountDBMW read error; recv result:%d, errno:%d:%s",
                (int)got, errno, strerror(errno));
            return -1;
        }
        if (got == 0)
            return len - (int)remain;
        cur += got;
        remain -= got;
    }
    return len;
}