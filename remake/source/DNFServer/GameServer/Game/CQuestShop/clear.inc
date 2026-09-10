void stQuestShopStatus::clear()
{
    // [b28c-27] AE：+0x20 ORIG `mov $0x0,%edx; mov %edx,0x20(%eax)`（常量经 edx
    // 中转）⇒ 字段改 float + `= 0.0f`（P211 常量 store 方向开关的 float 变体，
    // C1ERKS_ 4 字节 mov 拷贝不变）；int/unsigned int 两形实测均直写。
    m_name.clear();
    m_name2.clear();
    m_explain.clear();
    m_icon.clear();
    m_field10 = -1;
    m_questPoint = 0;
    m_field18 = 0x4d;
    m_statusType = 2;
    m_field20 = 0.0f;            // ORIG mov $0x0,%edx 中转（gcc4.1 特性，本工具链直写）
    m_levelInfos.clear();
