float ScanFloat(int scale, bool* ok)
{
    double v = ScanFloat(ok);
    v = v * pow(10.0, (double)scale - 1.0);
    v = v + 0.5;
    v = (int)v;
    v = v / pow(10.0, (double)scale - 1.0);
    return (float)v;
