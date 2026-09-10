int CharString::rfind(int pos, const CharString& pat) const
{
    return rfind(pos, pat.c_str(), pat.length());
