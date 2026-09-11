// ORIG T 0x08104884 _Z11SetSockNonbim：ioctl(fd, FIONBIO=0x5421, &arg) == 0。
// 返回 int（ORIG 尾 sete %al;movzbl %al,%eax 加宽形，bool 返回 4.4 不加宽）。
int SetSockNonb(int fd, unsigned long arg)
{
    unsigned long on = arg;
    return ioctl(fd, 0x5421, &on) == 0;
}
