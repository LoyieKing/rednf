// ---- less_TCHAR（CEnvironment map 比较器）----
bool less_TCHAR::operator()(const char* x, const char* y) const
{
    return strcmp(x, y) < 0;
}