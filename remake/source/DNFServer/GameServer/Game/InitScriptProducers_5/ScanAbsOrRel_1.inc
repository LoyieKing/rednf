float ScanAbsOrRel(bool& isAbs, std::string& str, bool& gotValue, bool consume)
{
    float result = 0;
    isAbs = true;
    for (;;)
    {
        int tok = g_dnfLex.getToken();
        if (tok == 2)
        {
            result = (float)g_dnfLex.getDecimal();
            isAbs = true;
            gotValue = true;
        }
        else if (tok == 3)
        {
            result = (float)g_dnfLex.getHexDecimal();
            isAbs = true;
            gotValue = true;
        }
        else if (tok == 4)
        {
            result = g_dnfLex.getFloat();
            isAbs = true;
            gotValue = true;
        }
        else if (tok == 5)
        {
            str = toTString(g_dnfLex.getType());
            std::string::size_type pos = str.find("[", 0) + 1;
            str.erase(0, pos);
            pos = str.find("]", 0);
            str.erase(pos, str.size());
            result = (float)atof(str.c_str());
            isAbs = false;
            gotValue = true;
        }
        else
        {
            isAbs = true;
            gotValue = false;
        }

        // P223 多岛形（ORIG 0x088bce8f..0x088bce9c 五枚 nop;jmp 岛，落点各异）：
        // 独立正向 goto + 连续尾标签，标签序 = 守卫序（2,3,4,5,0）。
        if (tok == 2)
            goto retTok2;
        if (tok == 3)
            goto retTok3;
        if (tok == 4)
            goto retTok4;
        if (tok == 5)
            goto retTok5;
        if (tok == 0)
            goto retTok0;

        if (consume)
        {
            // P0/P37：ORIG 无具名 string 局部（-0x18/-0x1c 为两个按值返回临时，
            // 右起求值：lastData→%esi、lineNo→%ebx、streamName→%eax），保持
            // 全表达式临时链（探针 /tmp/probe55h.cpp p5 逐条同形，含 EH 块序）。
            LogManager::logFormat(
                0, "../RDARScript.cpp", __FUNCTION__, 1496,
                "ERROR! %s - TYPE mismatch. %s:%d, token = '%s'",
                "ScanAbsOrRel", g_dnfLex.getStreamName().c_str(),
                g_dnfLex.getLastLineNo(),
                g_dnfLex.getLastTokenDataAsString().c_str());
        }
    }
retTok2:;
retTok3:;
retTok4:;
retTok5:;
retTok0:;
    return result;
}