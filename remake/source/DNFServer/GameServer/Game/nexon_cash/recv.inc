// ORIG 0x081af76e：
//   if (!buffer || !length) return -1;
//   n = read(...); if (n < 0) { (errno==EAGAIN||errno==EINTR||errno==EAGAIN||
//       errno==0) → return 0（0x081af7b5/7c1/7cd je 共岛 +0x6c；test jne→+0x80）；
//       其余 → return n（原负值，0x081af7ee mov -0xc,%eax） }
//   else if (n == 0) return -1;   // 对端关闭
//   return n;
int TCPSocket::recv(char* buffer, unsigned int length)
{
    if (buffer == 0 || length == 0)
        return -1;

    int n = static_cast<int>(::read(m_socket, buffer, length));
    if (n < 0)
    {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN || errno == 0)
            return 0;   // 0x081af7da 共岛 mov $0
    }
    else
    {
        if (n == 0)
            return -1;   // ORIG 0x081af7e7 n>=0 岛（jns +0x73）
    }
    return n;            // ORIG 0x081af7ee 三路共享尾块（6a/77 jne 同标）
}