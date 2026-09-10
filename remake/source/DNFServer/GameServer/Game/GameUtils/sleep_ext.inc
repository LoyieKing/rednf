void sleep_ext(int sec, int usec)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    select(0, NULL, NULL, NULL, &tv);
