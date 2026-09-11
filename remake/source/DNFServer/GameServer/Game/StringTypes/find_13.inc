int WideString::find(int pos, const WideString& pat) const
{
    return find(pos, pat.c_str(), pat.length());
}