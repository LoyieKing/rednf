int ScanStrOrInt(bool& isStr, std::string& s, bool& isInt, bool consume)
{
    int value = 0;
    isStr = true;
    goto L_get;
L_cont:
    ;
L_get:
    {
        int tok = g_dnfLex.getToken();
        if (tok == DNF_TOK_DECIMAL)
        {
            value = g_dnfLex.getDecimal();
            isStr = false;
            isInt = true;
        }
        else if (tok == DNF_TOK_HEXADECIMAL)
        {
            value = g_dnfLex.getHexDecimal();
            isStr = false;
            isInt = true;
        }
        else if (tok == DNF_TOK_FLOAT)
        {
            value = (int)g_dnfLex.getFloat();
            isStr = false;
            isInt = true;
        }
        else if (tok == DNF_TOK_STRING)
        {
            s = toTString(g_dnfLex.getString());
            isStr = true;
            isInt = true;
        }
        else if (tok == DNF_TOK_PAIR_INT)
        {
            int idx = g_dnfLex.getPairInt();
            if (g_dnfLex.getToken() == DNF_TOK_PAIR_STR)
                s = g_scriptStringManager_.findString(idx, g_dnfLex.getPairStr(), 0);
            isStr = true;
            isInt = true;
        }
        else
        {
            isStr = false;
            isInt = false;
        }
        if (tok == DNF_TOK_DECIMAL)
            goto L_done;
        if (tok == DNF_TOK_HEXADECIMAL)
            goto L_done;
        if (tok == DNF_TOK_FLOAT)
            goto L_done;
        if (tok == DNF_TOK_STRING)
            goto L_done;
        if (tok == DNF_TOK_PAIR_INT)
            goto L_done;
        if (tok == DNF_TOK_EOF)
            goto L_done;
        if (consume)
        {
            std::string last = g_dnfLex.getLastTokenDataAsString();
            LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x65a,
                                  "ERROR! %s - TYPE mismatch. %s:%d, token = '%s'",
                                  __FUNCTION__, g_dnfLex.getStreamName().c_str(),
                                  g_dnfLex.getLastLineNo(), last.c_str());
        }
        else
            goto L_cont;
        goto L_get;
    }
L_done:
    return value;
}