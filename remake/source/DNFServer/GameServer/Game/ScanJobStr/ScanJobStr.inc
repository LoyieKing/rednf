bool ScanJobStr(std::string* jobName, std::string** out)
{
    bool found = false;
    int token = g_dnfLex.getToken();
    switch (token)
    {
    case DNF_TOK_STRING:
        *jobName = toTString(g_dnfLex.getString());
        found = true;
        break;
    case DNF_TOK_PAIR_INT:
    {
        int key = g_dnfLex.getPairInt();
        if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR)
        {
            *jobName = g_scriptStringManager_.findString(
                key, g_dnfLex.getPairStr(), 0);
            found = true;
        }
        if (out != 0)
        {
            for (int i = 0; i <= 10; ++i)
            {
                char buf[0x400];
                std::snprintf(buf, sizeof(buf), "%s_%d",
                              g_dnfLex.getPairStr(), i);
                bool ok = false;
                char const* s = g_scriptStringManager_.findString(
                    key, buf, &ok);
                if (s != 0 && ok)
                {
                    out[i] = new std::string();
                    *out[i] = s;
                }
            }
        }
        break;
    }
    }
    return found;
