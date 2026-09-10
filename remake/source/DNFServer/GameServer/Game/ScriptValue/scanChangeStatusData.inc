bool scanChangeStatusData(ChangeStatusData& data)
{
    std::string s;
    bool isType = false;
    if (!ScanTypeOrStr(isType, s, true)) return false;
    if (isType) return false;
    data.type = getChangeStatusType(s);
    if (!ScanStr(&s)) return false;
    data.percent = (s == "%") ? 1 : 0;
    bool ok = false;
    data.value = (float)ScanInt(&ok);
    if (!ok) return false;
    return true;
