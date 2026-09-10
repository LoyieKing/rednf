bool ScanFloat(float* value)
{
    if (value == 0)
        return 0;
    bool ok = 0;
    *value = ScanFloat(&ok);
    return ok;
