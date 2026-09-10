bool ScanStr(std::string* value)
{
    bool ok = 0;
    int tok = g_dnfLex.getToken();
    switch (tok)
    {
    case DNF_TOK_STRING:
        *value = toTString(g_dnfLex.getString());
        ok = 1;
        break;
    case DNF_TOK_PAIR_INT:
    {
        int idx = g_dnfLex.getPairInt();
        if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR)
        {
            *value = g_scriptStringManager_.findString(idx, g_dnfLex.getPairStr(), 0);
            ok = 1;
        }
        break;
    }
    default:
        break;
    }
    return ok;
