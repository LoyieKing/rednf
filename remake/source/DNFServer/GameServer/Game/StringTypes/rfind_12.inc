int WideString::rfind(const WideString& pat) const
{
    int patLength = pat.length();
    return rfind(length() - patLength, pat.c_str(), patLength);
}