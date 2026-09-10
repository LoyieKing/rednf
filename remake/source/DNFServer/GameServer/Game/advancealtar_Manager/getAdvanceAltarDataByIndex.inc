AdvanceAltarData* CharacAdvanceAltarManager::getAdvanceAltarDataByIndex(int idx)
{
    if (idx < 0 || idx > 0)
    {
        return 0;
    }
    // ORIG：mov 0xc(%ebp),%eax; imul $0xd0,%eax,%eax; add 0x8(%ebp),%eax; add $0x4,%eax
    //（getAdvanceAltarDataByIndex.md）。成员数组下标寻址（同 setCurrentAdvanceAltarData
    // 的 m_data[i] 自然寻址形）；char* 乘法+加 4 混排会触发 4.4 恒折 (i*0x34+1)<<2。
    return &m_data[idx];
