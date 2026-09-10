int CDataManager::loadAdvanceAltarTimeLineParameter(
    advancealtar::StageTimeLineParameter& param)
{
    param.reset();
    int result = param.importStageTimeLineParameter(
        "Etc/AdvanceAltar/AdvanceAltarSummonData.etc");
    if (result != 0)
    {
        printf("importStageTimeLineParameter Fail(%d)\n", result);
        return result;
    }
    param.debugLog();
    return result;
