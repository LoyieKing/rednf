int CharString::rfind(const char* pat) const
{
    int patLength = static_cast<int>(strlen(pat));
    return rfind(length() - patLength, pat, patLength);
