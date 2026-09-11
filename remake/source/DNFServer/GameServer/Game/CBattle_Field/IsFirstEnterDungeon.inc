// [E05 R2] P321 T3 消费端判据：bool（ORIG 0x08307656 `movzbl; ret` + 调用点直判）。
bool CBattle_Field::IsFirstEnterDungeon()
{
    return m_firstEnterDungeon;
}