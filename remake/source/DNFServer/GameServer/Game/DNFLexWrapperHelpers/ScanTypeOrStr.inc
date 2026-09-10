bool ScanTypeOrStr(bool& isType, std::string& s, bool consume)
{
    bool ok = true;
    isType = true;
    goto L_get;
L_cont:
    ;
L_get:
    {
        int tok = g_dnfLex.getToken();
        if (tok == DNF_TOK_STRING)
        {
            s = toTString(g_dnfLex.getString());
            ok = true;
            isType = false;
        }
        else if (tok == DNF_TOK_TYPE)
        {
            s = toTString(g_dnfLex.getType());
        }
        else if (tok == DNF_TOK_PAIR_INT)
        {
            int idx = g_dnfLex.getPairInt();
            if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR)
            {
                tok = DNF_TOK_STRING;
                s = g_scriptStringManager_.findString(idx, g_dnfLex.getPairStr(), 0);
                ok = true;
                isType = false;
            }
        }
        else
        {
            ok = false;
            isType = false;
        }
        if (tok == DNF_TOK_STRING)
            goto L_done;
        if (tok == DNF_TOK_TYPE)
            goto L_done;
        if (tok == DNF_TOK_EOF)
            goto L_done;
        if (consume)
        {
            std::string last = g_dnfLex.getLastTokenDataAsString();
            LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x58e,
                                  "ERROR! %s - TYPE mismatch. %s:%d, token = '%s'",
                                  __FUNCTION__, g_dnfLex.getStreamName().c_str(),
                                  g_dnfLex.getLastLineNo(), last.c_str());
        }
        else
            goto L_cont;
        goto L_get;
    }
L_done:
    return ok;
