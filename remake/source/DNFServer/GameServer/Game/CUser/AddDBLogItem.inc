void CUser::AddDBLogItem(unsigned int itemIdx, unsigned int type,
                         ENUM_DBLOG_ITEM_TYPE value)
{
    // ORIG 0x08689360：循环步进用后置 it++（ppEi 符号），非 ++it。
    // 槽序实证（2026-09-05 CUserC8 修正前批方向）：ORIG info=-0x14（近，5 处
    // lea -0x14）/it=-0x20（远）——4.4 后声明占近槽（技巧 21 正用）：info 声明
    // 置于 for-init 之后。
    // 残差 N82/P112 族：4.1 it=-0x14/info=-0x20 vs 4.4 it=-0x20/info=-0x1c，
    // begin() sret 槽分配跨版本恒差；dead 前置/后置只整体平移不换相对序
    // （nl11_probe6 实证），源形不可复现。
    // b28d-solo-CUser 复测：info 声明前置到 for 之前（P212/P309 头部声明族）实测
    // 槽位逐字不变（it -0x20 / info -0x1c）⇒ 4.4.6 按**首次使用序**分配（N98），
    // 声明序非开关；维持 N82/P112 封顶。
    // b28d-solo-CUser 二试（P309 全头部声明 + 声明序按 ORIG 槽浅→深）：
    std::vector<_DBLogItemInfo>::iterator it = m_dbLogItems.begin();
    _DBLogItemInfo info;
    for (; it != m_dbLogItems.end(); it++)
    {
        if (it->m_itemIdx == itemIdx)
        {
            it->m_type += type;
            return;
        }
    }
    memset(&info, 0, 0xc);
    info.m_itemIdx = itemIdx;
    info.m_type = type;
    info.m_value = value;
    m_dbLogItems.push_back(info);
