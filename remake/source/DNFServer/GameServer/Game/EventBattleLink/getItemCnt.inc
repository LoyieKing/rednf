// getItemCnt @ 0x080ebd44：item->m_cnt（+0x08 unsigned short）。
int CraneMinigameManager::getItemCnt(int slot)
{
    CraneItemInfo* item = reinterpret_cast<CraneMinigameScript*>(this)->getItem(static_cast<short>(slot));
    // ORIG：je 到尾部 mov $0 落叶（P47 三目布局）；movzwl+cwtl 符号扩展
    // （(int)(short)m_cnt，0xffff 无效标记返回 -1）。
    return item != 0 ? static_cast<int>(static_cast<short>(item->m_cnt)) : 0;
}