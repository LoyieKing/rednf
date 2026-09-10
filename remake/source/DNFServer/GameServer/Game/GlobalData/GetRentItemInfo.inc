bool GetRentItemInfo(CUser& user, char sectionIdx, char seqIdx,
                     EventRewardItemInfo& out, int type)
{
    int selectionIdx = 0;
    if (!GetSelectionIndex(user, selectionIdx, type))
    {
        return false;
    }
    char job = 0;
    if (!GetCharacterJob(user, job))
    {
        return false;
    }

    int packageKey =
        makeFairPVPEquipmentPackageKey((int)job, selectionIdx);

    // ORIG 0x08271396：委托 pc_room::GetRewardItemSelectionList(packageKey,type)
    // （p167 MISSING 实证；原内联三分支多发 G_CDataManager×2 + 具体方法×2）。
    std::vector<std::vector<EventRewardItemInfo> >* list =
        GetRewardItemSelectionList(packageKey, type);
    if (list == 0)
    {
        return false;
    }

    // [b28d-solo-GlobalData] P214①/P222：ORIG 0x08271444 先 call size() 落
    // -0x14，再 `cmpb $0,-0x2c; js F1`、`movsbl; cmp -0x14; jl OK1`（失败块
    // F1 居中、成功体 jl 直落、无 setcc 汇聚）；第二段同构（0x0827148a）。
    const int listSize = (int)list->size();
    if (sectionIdx < 0 || listSize <= sectionIdx)
    {
        return false;
    }
    std::vector<EventRewardItemInfo>& inner = list->at((std::size_t)sectionIdx);
    const int innerSize = (int)inner.size();
    if (seqIdx < 0 || innerSize <= seqIdx)
    {
        return false;
    }
    out = inner[(std::size_t)seqIdx];
    return true;
