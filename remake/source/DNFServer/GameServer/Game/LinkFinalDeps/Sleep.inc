// ORIG T 0x0858c4e0:usleep(ms*1000)
void Sleep(int ms)
{
    usleep(static_cast<useconds_t>(ms) * 1000u);
}