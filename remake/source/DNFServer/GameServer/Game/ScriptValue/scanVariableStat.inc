bool scanVariableStat(VariableStat& vs)
{
    if (!scanChangeStatusData(vs.status)) return false;
    return scanScriptValue(vs.value);
