// ORIG GetUserList 0x8565184：内联 rep movsl（18 dword）拷贝第一支队伍
// （m_teams[0]，0x48 字节）到出参。
// [b28 P83] 4.1 把常量 memcpy 内联成 rep movsl 而 4.4 恒 call memcpy（N84）；
// 改结构整体赋值 `*(TeamInfoThird*)out = m_teams[0]` 触发 4.4 发射同形
// rep movsl 三件套缓存（探针 /tmp/l2_p5.cpp z2：dst→edi/src→esi/count→ecx，
// 与 ORIG `mov $0x12,%eax; lea 0x1c(%eax),%ebx` 结构一致）。语义不变。
void CFairMatch::GetUserList(void* out)
{
    *reinterpret_cast<TeamInfoThird*>(out) = m_teams[0];
}