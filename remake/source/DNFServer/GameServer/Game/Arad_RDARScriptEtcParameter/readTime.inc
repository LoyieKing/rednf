// ---- readTime ---------------------------------------------------------------
// 报告 ARAD__SCRIPT/readTime.md 全文转写：连读 4 个 short；任一 ScanInt(bool*)
// 失败抛出该字段名常量，catch 后 logFormat(line=0x27) 并返回 false。
bool readTime(short* out)
{
    int idx = 0;
    bool ok = false;
    try
    {
        out[idx] = (short)ScanInt(&ok);
        idx += 1;
        if (!ok)
            throw "start year";
        out[idx] = (short)ScanInt(&ok);
        idx += 1;
        if (!ok)
            throw "start month";
        out[idx] = (short)ScanInt(&ok);
        idx += 1;
        if (!ok)
            throw "start day";
        out[idx] = (short)ScanInt(&ok);
        idx += 1;
        if (!ok)
            throw "start time";
        return true;
    }
    catch (char const* tag)
    {
        LogManager::logFormat(
            1, "../localjapan/Arad_RDARScriptEtcParameter.cpp", __FUNCTION__, 0x27,
            "[ARAD SCRIPT] Load fail. AradEventPeroid.etc %s", tag);
        return false;
    }
}