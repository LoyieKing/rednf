// ORIG RegisterCreatureIdAndItemId @ 0x833d54a。
// [b28d-30] 复核 NEAR：map 迭代器槽/转换临时差（P50/P96/N86 族），ORIG 0x0833d54a。
bool CCreatureScriptMgr::RegisterCreatureIdAndItemId(int creatureId, int itemId)
{
    // P50/P96：ORIG find 返回 iterator 直存（const_iterator 会额外引入转换临时+
    // 拷贝构造，帧 +0x10/多两条 lea——实测 we-larger 10 条）。
    // [b28-BM7 判词] NEAR 残余 = N95/N82：类对象槽基跨版本差——4.1 把
    // find/end/insert 临时深排（-0x34/-0x30/-0x38），4.4 恒从 ebp-8 浅排
    // （-0x24/-0x28），指令/助记符/调用集全等，仅 disp 槽号成串平移。
    std::map<int, int>::iterator it = m_creatureIdToItemId.find(creatureId);
    if (it != m_creatureIdToItemId.end())
    {
        // P221：ORIG 槽序 it(-0x38) < end(-0x34) < trace(-0x30)；4.4 具名 trace
        // 恒深槽 ⇒ 全表达式临时链。
        cMyTrace(__PRETTY_FUNCTION__, 0xfff, 5)("[%s][%d]",
                                                __PRETTY_FUNCTION__,
                                                0xfff);
        return false;
    }
    m_creatureIdToItemId.insert(std::make_pair(creatureId, itemId));
    return true;
}