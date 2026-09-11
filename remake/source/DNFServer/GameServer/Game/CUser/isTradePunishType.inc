// ============================================================================
// ORIG T 0x0868991c _ZN5CUser17isTradePunishTypeEi：
//   type==0 → m_tradePunishType(+0x79628)!=0；否则按位与命中。
// ============================================================================
bool CUser::isTradePunishType(int type)
{
    if (type == 0 && m_postChecksumData.m_tradePunishType != 0)
        return 1;
    // ORIG 0x8689949 je 尾岛（0x8689952 mov $0）：命中 fall `mov $1`（probe12 B 反向布局）
    if (type & m_postChecksumData.m_tradePunishType)
        return 1;
    return 0;
}