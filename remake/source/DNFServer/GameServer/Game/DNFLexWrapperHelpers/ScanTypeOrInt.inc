int ScanTypeOrInt(bool& isType, std::string& s, bool& isInt, bool consume)
{
    int value = 0;
    isType = true;
    goto L_get;
L_cont:
    ;
L_get:
    {
        int tok = g_dnfLex.getToken();
        if (tok == DNF_TOK_DECIMAL)
        {
            value = g_dnfLex.getDecimal();
            isType = false;
            isInt = true;
        }
        else if (tok == DNF_TOK_HEXADECIMAL)
        {
            value = g_dnfLex.getHexDecimal();
            isType = false;
            isInt = true;
        }
        else if (tok == DNF_TOK_FLOAT)
        {
            value = (int)g_dnfLex.getFloat();
            isType = false;
            isInt = true;
        }
        else if (tok == DNF_TOK_TYPE)
        {
            s = toTString(g_dnfLex.getType());
            isType = true;
            isInt = true;
        }
        else
        {
            isType = false;
            isInt = false;
        }
        if (tok == DNF_TOK_DECIMAL)
            goto L_done;
        if (tok == DNF_TOK_HEXADECIMAL)
            goto L_done;
        if (tok == DNF_TOK_FLOAT)
            goto L_done;
        if (tok == DNF_TOK_TYPE)
            goto L_done;
        if (tok == DNF_TOK_EOF)
            goto L_done;
        if (consume)
        {
            std::string last = g_dnfLex.getLastTokenDataAsString();
            LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x613,
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
