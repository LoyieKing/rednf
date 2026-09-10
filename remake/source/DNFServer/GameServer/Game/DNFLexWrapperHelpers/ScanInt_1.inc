bool ScanInt(int* value)
{
    if (value == 0)
        return 0;
    bool ok = 0;
    *value = ScanInt(&ok);
    return ok;
