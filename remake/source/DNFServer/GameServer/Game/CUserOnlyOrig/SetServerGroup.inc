// ---------------------------------------------------------------------------
// 9. CUser::SetServerGroup @0x082300ac W(报告缺失,objdump 直转)
// ---------------------------------------------------------------------------
void CUser::SetServerGroup(ENUM_SERVER_GROUP group)
{
    // 2026-09-04 §4.3：this+0x79630 → PostChecksumUserInfoBlock 具名字段
    // ORIG 0x082300ac：enum=enum 同型赋值 this 先载（技巧5延伸·装载序定律）
    m_postChecksumData.m_serverGroup = group;  // +0x79630（块+0xc）
}