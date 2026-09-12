// [R19a05 ④ 2026-09-13] 返回类型 uchar→bool（声明同改 CBattle_Field.h）：
//   ORIG 两调用点均 bool（0x085a842e xor$1 形 / 0x085a3e28 直判形，见头注释）；
//   字段 m_visited 为 bool ⇒ 体仍单条 `movzbl`（ORIG 0x085bf2e8 同形）。
bool CBattle_Field::IsVisitedCurrentMap()
{
    return GetCurrentMapInfo()->m_visited;
}