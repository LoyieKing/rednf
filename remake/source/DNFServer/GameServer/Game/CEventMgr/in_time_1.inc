bool in_time(struct tm* begin, struct tm* end)
{
    time_t t1 = mktime(begin);
    time_t t2 = mktime(end);
    time_t now = time(NULL);
    return (now >= t1 && now < t2);
