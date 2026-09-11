// [b28d-solo-PvP_Room] N104（EH 尾块相对序）：ORIG 0x085d61a6 正常路径析构链
// （m_pMatchingSystem delete → Mutex@+0x5e8 → PvpResultRecvFlag@+0x5bc/@+0x5b4 →
// PvpUserTable → PvP_GuildWar_Log）位于函数中段、unwind 块（_Unwind_Resume + 重复
// 析构链）居中；4.4 把同一段 epilogue 就地内联在正常路径、unwind 压最末 ⇒ 两尾块
// 互换，difflib 把 ~9 行 rotate 放大成 ndiff（81 vs 91）。块内容逐条全等，P167
// CLEAR 13/13（nm 实证 ORIG 同名 D1 均在、无发明符号）⇒ 语义完成，byte 层 N104 封顶。
// [SweepPvPRoom2 P141] ORIG 0x085d61a6 只有一次成员读 + `test %ebx,%ebx; je`
// （编译器为 delete 生成的隐式空指针守卫）；手写 `if (m_pMatchingSystem)` 会
// 重复判空（多 mov+mov+test+je 4 条）⇒ 改无条件 delete 复现 ORIG 守卫形。
// [L11 复核 2026-09-11 · 封顶] 残余 116/118 全在**尾块相对序**：正常路径析构链
//   (`add $0xc0,%esp;pop×3;ret`) 与 unwind 段互置 + 各块间 4.4 多插 2 条中继 jmp；
//   块内容逐条全等（ORIG 亦是同一析构链两遍）⇒ EH landing-pad 布局固有，N104/N121 封顶。
PvP_Room::~PvP_Room()
{
    delete m_pMatchingSystem;
}