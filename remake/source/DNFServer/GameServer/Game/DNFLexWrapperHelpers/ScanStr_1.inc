bool ScanStr(char* dst, int maxLen)
{
    bool ok = 0;
    int tok = g_dnfLex.getToken();
    switch (tok)
    {
    case DNF_TOK_STRING:
    {
        const char* p = g_dnfLex.getString();
        if (std::strlen(toTString(p)) <= (unsigned)maxLen)
        {
            std::strncpy(dst, toTString(p), (unsigned)maxLen);
            ok = 1;
        }
        break;
    }
    case DNF_TOK_PAIR_INT:
    {
        int idx = g_dnfLex.getPairInt();
        if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR)
        {
            const char* found = g_scriptStringManager_.findString(idx, g_dnfLex.getPairStr(), 0);
            if (std::strlen(found) <= (unsigned)maxLen)
            {
                std::strncpy(dst, found, (unsigned)maxLen);
                ok = 1;
            }
        }
        break;
    }
    default:
        break;
    }
    return ok;
}