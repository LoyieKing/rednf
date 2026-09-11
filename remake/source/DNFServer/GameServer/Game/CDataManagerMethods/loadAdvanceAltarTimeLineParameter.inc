// ===================== loadAdvanceAltarTimeLineParameter =====================
// ORIG 0x08365010（静态）：reset → importStageTimeLineParameter(
// Etc/AdvanceAltar/AdvanceAltarSummonData.etc)；非 0 打印 Fail(%d)，
// 否则 debugLog；透传返回值。
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
}