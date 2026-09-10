int ScanAll(float* value, std::string* str)
{
    int v = 0;
    int r = ScanAll(&v, value, str);
    if (r == 2 || r == 3)
    {
        r = 4;
        *value = (float)v;
    }
    return r;
