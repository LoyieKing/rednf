// 【objdump】ORIG _ZN13user_creature14COverSkillTime7GetTimeEv W 0x0833ef04
// （size 0x08）：push %ebp; mov %esp,%ebp; mov 0x8(%ebp),%eax; pop %ebp; ret
//   —— 字面返回 this 本身、不解引用。调用方（CCreatureMgr::SetOverSkillTime
//   0x0833c523/0x0833c637/0x0833c656）在 call 之后立即 `mov (%eax)` 取值，
//   即返回 &m_time。返回类型不参与 mangled；按 ORIG 指令忠实转写为 int*。
int* COverSkillTime::GetTime()
{
    return &m_time;
}