float ScanFloat(bool* ok)
{
    float value = 0;
    int tok = g_dnfLex.getToken();
    if (tok == DNF_TOK_DECIMAL)
    {
        value = (float)g_dnfLex.getDecimal();
        if (ok)
            *ok = true;
    }
    else if (tok == DNF_TOK_HEXADECIMAL)
    {
        value = (float)g_dnfLex.getHexDecimal();
        if (ok)
            *ok = true;
    }
    else if (tok == DNF_TOK_FLOAT)
    {
        value = g_dnfLex.getFloat();
        if (ok)
            *ok = true;
    }
    else if (ok)
        *ok = false;
    return value;
}