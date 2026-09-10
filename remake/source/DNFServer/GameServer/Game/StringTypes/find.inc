int CharString::find(const char* pat) const
{
    return find(0, pat, static_cast<int>(strlen(pat)));
