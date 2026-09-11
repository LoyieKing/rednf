// ===========================================================================
// 8/9/9a) LevelupSupportEventManger —— GetStepIndex 0x08147362 /
//         InsertItem 0x081473cc / GetItemInfoVector 0x08147724
// ===========================================================================
int LevelupSupportEventManger::GetStepIndex(int requestLevel)
{
    // ORIG T 0x08147362：find(-0x10) 先、end(-0xc) 后、operator!= + test/je
    // 落 `mov $-1` 岛 ⇒ 正极性 `if (it != end) return it->second;`（P220）。
    std::map<int, int>::iterator it = m_stepIndexMap.find(requestLevel);
    if (it != m_stepIndexMap.end())
        return it->second;
    return -1;
}