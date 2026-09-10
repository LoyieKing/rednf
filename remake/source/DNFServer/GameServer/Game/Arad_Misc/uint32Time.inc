void uint32Time(short* timeBuf)
{
    // ORIG：time→localtime；tm_year=buf[0]-1900, tm_mon=buf[1]-1,
    // tm_mday=buf[2], tm_hour=buf[3], min/sec=0，mktime 落表。
    time_t now = time(static_cast<time_t*>(NULL));
    struct tm* tmv = localtime(&now);
    tmv->tm_year = *timeBuf - 1900;
    tmv->tm_mon = timeBuf[1] - 1;
    tmv->tm_mday = timeBuf[2];
    tmv->tm_hour = timeBuf[3];
    tmv->tm_min = 0;
    tmv->tm_sec = 0;
    mktime(tmv);
