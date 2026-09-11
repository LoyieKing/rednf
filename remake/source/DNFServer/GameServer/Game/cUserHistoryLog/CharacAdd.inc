// ORIG _ZN15cUserHistoryLog9CharacAddEi @0x08682a92：
//   栈上 TimeLog_ 计时后读 m_charac[idx].m_field26(+0x26 growtype) 日志 "Charac+,%d"。
//   【推断】TimeLog_ 为纯计时局部对象，无可观察副作用，重建中省略。
void cUserHistoryLog::CharacAdd(int idx)
{
    // ORIG 栈上 TimeLog_ 局部对象调 setTime()（结果未用，保留以对齐字节）。
    TimeLog_ tl;
    tl.setTime();
    // CUser::m_characList @+0x796e8（ORIG 0x08682a92 operator[] 基址），
    // 表达式内联传递（P134——引用局部会挤占 -0xc 槽并使实参装载序漂移）。
    m_user->LogHistory("Charac+,%d",
        (int)m_user->m_characList[idx].m_field26);
}