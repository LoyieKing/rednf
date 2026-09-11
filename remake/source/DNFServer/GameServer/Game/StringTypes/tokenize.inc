int CharString::tokenize(const CharString& str, std::vector<CharString>* tokenVector)
{
    return tokenize(str, tokenVector, " \t\n\r", ".,", false);
}