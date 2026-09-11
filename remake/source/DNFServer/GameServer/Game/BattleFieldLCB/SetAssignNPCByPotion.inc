// ---------------------------------------------------------------------------
// 3. SetAssignNPCByPotion  @0x085d19d6（W 弱符号；无独立报告，objdump 直转）
//
// objdump 全量（0x85d19d6..0x85d19f9）：
//   push %ebp / mov %esp,%ebp / sub $0x28,%esp
//   movzbl 0xc(%ebp),%edx           ; 取 bool 实参低字节
//   call GetCurrentMapInfo (0822cbcc)，this=0xc(%ebp)?? —— 见下注
//   mov %dl,0xa4(%eax)              ; 写 MapInfo.m_assignNPCByPotion (+0xa4)
//   leave / ret
// 即：GetCurrentMapInfo()->m_assignNPCByPotion = flag。
// ---------------------------------------------------------------------------
void CBattle_Field::SetAssignNPCByPotion(bool flag)
{
    // ORIG：无 cur 局部（mapInfo eax 直通），bool 形参直读形参槽。
    GetCurrentMapInfo()->m_assignNPCByPotion = flag;
}