bool nextToken(std::string& token)
{
    if (g_nextToken >= g_tokens.size())
        return false;
    token = g_tokens[g_nextToken++];
    return true;
}