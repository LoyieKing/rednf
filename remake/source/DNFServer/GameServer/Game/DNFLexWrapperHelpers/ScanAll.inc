int ScanAll(int* intOut, float* floatOut, std::string* strOut)
{
    int token = g_dnfLex.getToken();
    if (token == 2)
    {
        if (intOut != 0)
            *intOut = g_dnfLex.getDecimal();
    }
    else if (token == 3)
    {
        if (intOut != 0)
            *intOut = g_dnfLex.getHexDecimal();
    }
    else if (token == 4)
    {
        if (floatOut != 0)
            *floatOut = g_dnfLex.getFloat();
    }
    else if (token == 7)
    {
        if (strOut != 0)
            *strOut = toTString(g_dnfLex.getString());
    }
    else if (token == 5)
    {
        if (strOut != 0)
            *strOut = toTString(g_dnfLex.getType());
    }
    else if (token == 6)
    {
        if (strOut != 0)
            *strOut = toTString(g_dnfLex.getType());
    }
    else if (token == 9)
    {
        int pair = g_dnfLex.getPairInt();
        if (g_dnfLex.getToken() == 0xa)
        {
            token = 7;
            if (strOut != 0)
                *strOut = g_scriptStringManager_.findString(
                    pair, g_dnfLex.getPairStr(), 0);
        }
    }
    return token;
}