bool ScanType(std::string& token, bool consume)
{
    // [b28d-47] ORIG 0x088bc85f：tok@-0x10（深）/ok@-0x9（浅）；P212 全头部
    // 声明按 ORIG 槽深序（tok 先声明）。logFormat 第 4 个 vararg 同 __FUNCTION__。
    int tok;
    bool ok = true;
    goto L_get;
L_cont:
    ;
L_get:
    {
        tok = g_dnfLex.getToken();
        if (tok == DNF_TOK_TYPE)
            token = toTString(g_dnfLex.getType());
        else
            ok = false;
        if (tok == DNF_TOK_TYPE)
            goto L_done;
        if (tok == DNF_TOK_EOF)
            goto L_done;
        if (consume)
        {
            std::string last = g_dnfLex.getLastTokenDataAsString();
            LogManager::logFormat(0, "../RDARScript.cpp", __FUNCTION__, 0x555,
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
}