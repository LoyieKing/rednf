int CharString::rfind(int pos, const char* pat) const
{
    return rfind(pos, pat, static_cast<int>(strlen(pat)));
