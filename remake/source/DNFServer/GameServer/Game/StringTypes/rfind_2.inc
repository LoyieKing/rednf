int CharString::rfind(const char* pat, int patLength) const
{
    return rfind(length() - patLength, pat, patLength);
