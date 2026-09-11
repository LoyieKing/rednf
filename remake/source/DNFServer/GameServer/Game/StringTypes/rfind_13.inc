int WideString::rfind(int pos, const WideString& pat) const
{
    return rfind(pos, pat.c_str(), pat.length());
}