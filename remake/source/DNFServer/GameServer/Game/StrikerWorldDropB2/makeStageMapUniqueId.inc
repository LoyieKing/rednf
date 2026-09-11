// ============================================================================
// ORIG T 0x08364818：三参在栈上分别 +100/+1000/+1000（addl 直改栈槽）后
// imul $0x5f5e100 / imul $0x2710 / add 线性组合。保持同构写法以对齐指令形态。
int StageMapList::makeStageMapUniqueId(int areaId, int districtId, int stageIdx)
{
    areaId += 100;
    districtId += 1000;
    stageIdx += 1000;
    return areaId * 100000000 + districtId * 10000 + stageIdx;
}