// ---------------------------------------------------------------------------
// 10. CUser::count_specific_items(vector<int>&, vector<pair<int,int>>&) T 0x0866514a
void CUser::count_specific_items(std::vector<int>& ids, std::vector<std::pair<int,int> >& out)
{
    CInventory* inv = const_cast<CInventory*>(getCurCharacInvenR());
    CCargo* cargo = const_cast<CCargo*>(getCurCharacCargoR());
    int total = 0;
    Inven_Item probe;
    // Inven_Item::Inven_Item 0x80cb854
    unsigned int i = 0;
    while (i < ids.size()) {
        total = 0;
        if (inv->GetInvenData(ids[i], probe) != -1)
            total += probe.m_addInfo2;                       // probe+7 int（fields 建议）
        if (cargo->GetCargoData(ids[i], probe) != -1)
            total += probe.m_addInfo2;
        if (total > 0) {
            out.push_back(std::make_pair(ids[i], total));       // make_pair<int&,int&> 实参序实证 first=ids[i]
        }
        ++i;
    }
}