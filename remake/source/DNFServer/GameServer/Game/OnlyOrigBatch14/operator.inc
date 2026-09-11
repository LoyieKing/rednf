// ORIG T 0x085e709e _ZN15MissionLessThanclEPK8CMissionS2_：
// 先比 MissionKind 再比 MissionIndex。
bool MissionLessThan::operator()(const CMission* a, const CMission* b)
{
    if (a->GetMissionKind() != b->GetMissionKind())
    {
        return a->GetMissionKind() < b->GetMissionKind();
    }
    if (a->GetMissionIndex() != b->GetMissionIndex())
    {
        return a->GetMissionIndex() < b->GetMissionIndex();
    }
    return false;
}
