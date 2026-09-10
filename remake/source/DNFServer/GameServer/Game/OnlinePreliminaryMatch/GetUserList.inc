void COnlinePreliminary::GetUserList(void* out)
{
    /* ORIG @08562107 一次性装载 out→%eax / this→%edx，随后 10 对字
     * mov disp(%edx),%ecx / mov %ecx,disp(%eax)（末对复用 %edx）——4.1 O0
     * 结构体整体赋值的 block-move 形；用 0x28 POD 视图结构赋值复现 */
    struct TeamView28 { unsigned int w[10]; };  // 10 dword = m_teams[0] 全貌
    /* pad+成员 形式使 0x1c 折进每条 disp（ORIG @+0x09 mov 0x1c(%edx),%ecx
     * 无独立 add） */
    struct TeamGap { char g[0x1c]; TeamView28 v; };
    *static_cast<TeamView28*>(out) =
        reinterpret_cast<const TeamGap*>(this)->v;
