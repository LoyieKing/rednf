void Inven_Item::setCopy(const Inven_Item& other)
{
    m_field0 = other.m_field0;
    m_field1 = other.m_field1;
    m_addInfo = other.m_addInfo;
    m_addInfo2 = other.m_addInfo2;
    m_fieldb = other.m_fieldb;
    m_attr = other.m_attr;
    m_fieldd = other.m_fieldd;
    // b28d-60：P247/P29——ORIG 0x814a688 起为「一次载入 this/other + 连续
    // disp 拷贝」形；原 (char*)&m + k 指针算术使 4.4 逐字段重算地址（add 链，
    // 60→92 insn）。改具名视图/整体赋值：m_random/m_reserved 整体结构赋值
    // （4.4 对 20B POD 发 5 连发 mov，与 ORIG 同）。
    // b28d-solo-Inven_Item：+0x11/+0x33 两处 ORIG 为 LHS-first
    // （`mov this,%eax; mov other,%edx; mov d(%edx),…; mov …,d(%eax)`），标量/
    // union 标量视角形 4.4 恒 RHS-first（N76）。改结构赋值形即复现：
    // +0x11 → stAmplifyHeadView 四字节结构（P247，CCreatureMgr setCopy 同法）；
    // +0x33 → UpgradeSeparateInfo 整体赋值（1 字节 POD 结构赋值，探针 D2 全等）。
    m_amp.m_amp0Head = other.m_amp.m_amp0Head;
    m_random = other.m_random;
    m_upgradeSep = other.m_upgradeSep;
    m_reserved = other.m_reserved;
}