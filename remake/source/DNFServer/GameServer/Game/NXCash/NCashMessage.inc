// ---------------------------------------------------------------------------
// NCashMessage 定义必须前置于 NCashImpl ctor 且保持 inline（P210 帧继承）
//
// NCashImpl ctor 调用 m_inMsg()/m_outMsg()（NCashMessage::C1）。拆 TU 前二者
// 同处 nexon_cash.cpp 且 NCashMessage 定义在前 ⇒ 4.4 精确继承 callee outgoing；
// 拆分后若本 TU 只见声明，4.4 转保守形，C1 prologue 由 ORIG 的
// `push %ebx; sub $0x24` 退化为 `push %esi; push %ebx; sub $0x20`（DIFF）。
// 实测三形（均定义前置）：
//   · 普通 out-of-line 定义 ⇒ C1 恢复，但 NCashMessageC2 成为 TU 首强符号，
//     GI 键名被顶成 NCashMessageC2（≠ want）；
//   · __attribute__((weak)) 定义 ⇒ C1 仍 DIFF（4.4 对 weak callee 不做帧继承）；
//   · inline 定义 ⇒ C1 恢复且符号为 W（不参与 GI 命名）⇒ 唯一可行形。
// 符号类型随之与 ORIG 一致（ORIG 该 8 符号全为 W）；代价为 4.4 额外发射
// local 别名 `_ZN5nexon4cash12NCashMessageC5Ev`（n 类型、地址 0、结构性噪音）。
// ---------------------------------------------------------------------------
inline NCashMessage::NCashMessage()
    : m_magic('U'), m_bodyLength(0)
{
}