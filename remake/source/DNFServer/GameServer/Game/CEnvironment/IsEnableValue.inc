bool CEnvironment::IsEnableValue(const char* value)
{
    if (strcmp(value, "yes") == 0 || strcmp(value, "YES") == 0 ||
        strcmp(value, "Yes") == 0 || strcmp(value, "1") == 0)
        return 1;
    return 0;
}