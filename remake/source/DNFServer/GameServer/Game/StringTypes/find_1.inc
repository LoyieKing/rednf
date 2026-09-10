int CharString::find(int pos, const char* pat) const
{
    return find(pos, pat, static_cast<int>(strlen(pat)));
