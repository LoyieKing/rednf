int WideString::tokenize(const WideString& str, std::vector<WideString>* tokenVector)
{
    return tokenize(str, tokenVector, L" \t\n\r", L".", false);
}