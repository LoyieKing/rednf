int CharString::find(int pos, const CharString& pat) const
{
    return find(pos, pat.c_str(), pat.length());
