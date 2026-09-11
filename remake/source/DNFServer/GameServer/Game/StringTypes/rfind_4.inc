int CharString::rfind(const CharString& pat) const
{
    int patLength = pat.length();
    return rfind(length() - patLength, pat.c_str(), patLength);
}