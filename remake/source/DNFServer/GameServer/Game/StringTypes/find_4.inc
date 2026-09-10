int CharString::find(const CharString& pat) const
{
    return find(0, pat.c_str(), pat.length());
