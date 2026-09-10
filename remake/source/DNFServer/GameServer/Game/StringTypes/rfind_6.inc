int CharString::rfind(char pat) const
{
    return rfind(length() - 1, pat);
