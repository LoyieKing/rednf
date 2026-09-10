int ScanFloatStr(float& value, std::string& token)
{
    int kind = -1;
    value = 0;
    int tok = g_dnfLex.getToken();
    if (tok == DNF_TOK_DECIMAL)
    {
        value = (float)g_dnfLex.getDecimal();
        kind = 0;
    }
    else if (tok == DNF_TOK_HEXADECIMAL)
    {
        value = (float)g_dnfLex.getHexDecimal();
        kind = 0;
    }
    else if (tok == DNF_TOK_FLOAT)
    {
        value = g_dnfLex.getFloat();
        kind = 0;
    }
    else if (tok == DNF_TOK_STRING)
    {
        token = toTString(g_dnfLex.getString());
        kind = 1;
    }
    else
        kind = -1;
    return kind;
