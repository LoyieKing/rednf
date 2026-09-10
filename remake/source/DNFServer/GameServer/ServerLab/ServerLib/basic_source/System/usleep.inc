void LinuxSystem::usleep(int microsecond)
{
    timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = microsecond;
    select(0, NULL, NULL, NULL, &tv);
