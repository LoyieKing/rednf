int StageMapList::makeStageMapUniqueId(int areaId, int districtId, int stageIdx)
{
    areaId += 100;
    districtId += 1000;
    stageIdx += 1000;
    return areaId * 100000000 + districtId * 10000 + stageIdx;
