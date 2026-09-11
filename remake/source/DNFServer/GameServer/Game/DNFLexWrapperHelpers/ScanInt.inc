int ScanInt(bool* ok)
{
    int value = 0;
    int tok = g_dnfLex.getToken();
    if (tok == DNF_TOK_DECIMAL)
    {
        value = g_dnfLex.getDecimal();
        if (ok)
            *ok = true;
    }
    else if (tok == DNF_TOK_HEXADECIMAL)
    {
        value = g_dnfLex.getHexDecimal();
        if (ok)
            *ok = true;
    }
    else if (tok == DNF_TOK_FLOAT)
    {
        value = (int)g_dnfLex.getFloat();
        if (ok)
            *ok = true;
    }
    else if (ok)
        *ok = false;
    return value;
}