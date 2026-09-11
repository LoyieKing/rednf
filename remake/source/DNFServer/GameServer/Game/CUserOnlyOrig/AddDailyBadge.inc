// ---------------------------------------------------------------------------
// 19) CUser::AddDailyBadge(ENUM_BADGE_TYPE badge) 【报告缺失@0x08568e5e size 0x26】
//     objdump -d 0x8568e5e..0x8568e84:
//       lea 0x47040(%eax),%ecx        ; ecx = badge + 0x47040
//       movzwl 0x4(%edx,%ecx,2),%edx  ; u16 @ this + (badge+0x47040)*2 + 4
//                                     ;       = this + 0x8E084 + badge*2
//       lea 0x1(%edx),%ecx ; mov %cx,0x4(%edx,%eax,2)  ; 自增
//     ⇒ ushort 计数表基址 this+0x8E084，按 badge 下标自增。
// ---------------------------------------------------------------------------
void CUser::AddDailyBadge(ENUM_BADGE_TYPE badge)
{
    // ORIG 0x08568e5e：单装载 movzwl + lea 0x1(%edx) + 单 store —— 后缀 ++（技巧4）
    m_dailyBadge[(int)badge]++;
}