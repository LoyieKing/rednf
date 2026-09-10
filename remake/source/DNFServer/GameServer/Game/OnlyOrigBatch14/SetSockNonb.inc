int SetSockNonb(int fd, unsigned long arg)
{
    unsigned long on = arg;
    return ioctl(fd, 0x5421, &on) == 0;
