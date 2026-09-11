int WideString::find(const WideString& pat) const
{
    return find(0, pat.c_str(), pat.length());
}