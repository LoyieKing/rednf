int ncl_check_date(char* a, char* b)
{
    int ok = 1;
    char today[0x80];
    std::memset(today, 0, sizeof(today));
    ncl_make_date(today);
    if (std::strcmp(today, a) < 0 || std::strcmp(today, b) > 0)
    {
        ok = 0;
    }
    return ok;
