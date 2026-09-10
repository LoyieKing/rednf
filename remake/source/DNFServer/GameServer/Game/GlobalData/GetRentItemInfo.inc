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

    // [b28d-solo-GlobalData] P214①/P222：ORIG 0x0826f91e 先 call size() 落
    // -0x14，再 `movzbl (%eax); test %al,%al; js F1`、`movsbl; cmp -0x14(%ebp),%eax;
    // jl OK1`（失败块 F1 居中 `mov $0; jmp end`、成功体 jl 直落、无 setcc 汇聚）；
    // 第二段同构（0x0826f96e）。
    // [FL7GlobalData 2026-09-10 · P169 双向实测否定] 改两级正向 if 嵌套
    // （`if (sectionIdx >= 0) { if (sectionIdx < listSize) { … return true; } } return false;`）
    // 后单件 ndiff 仍 2（我方仍多一条 `jmp`）且 TU 四数 ae 1791→1790 回退 ⇒ 已回滚，
    // 维持早退两条件形（残余 2 条 jmp 记 N59/N82 族）。
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
