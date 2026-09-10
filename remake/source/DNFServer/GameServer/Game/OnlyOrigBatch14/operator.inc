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
