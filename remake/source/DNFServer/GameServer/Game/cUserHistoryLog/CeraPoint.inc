// ORIG _ZN15cUserHistoryLog9CeraPointE23ENUM_CERAPOINT_LOG_TYPEj @0x0868593c：
//   尾参为当前余额 CUser::GetCeraPoint()。
// [b28d-solo-cUserHistoryLog] IDENTICAL 19/19：第三实参**去 (int) cast** 直传 unsigned
// （GetCeraPoint 返回 unsigned int，vararg %d 位模式不变、语义等价）⇒ 4.4 复现 ORIG
// 「balance 留 %eax / type 先入 %ecx 跨 receiver 保活 / receiver 用 %edx」装载序
// （探针 /tmp/hlprobe/Game/probe3.cpp fB 与 ORIG 逐条同形；旧 fA 带 cast 多 mov %eax,%ecx）。
void cUserHistoryLog::CeraPoint(ENUM_CERAPOINT_LOG_TYPE type, unsigned int amount)
{
    // 证据（ORIG 0x0868594f..0x08685971）：call GetCeraPoint 后
    //   mov 0xc(%ebp),%ecx（type 先入 ecx）→ mov 0x8(%ebp),%edx; mov (%edx),%edx
    //   → mov %eax,0x10(%esp)（balance 留 eax）→ mov 0x10(%ebp),%eax（amount）
    //   → mov %eax,0xc(%esp) → mov %ecx,0x8(%esp) → call。
    // 带 (int) 的第三实参（unsigned→int 转换节点）会使 4.4 把 balance 提前转存 %ecx
    // （fA 20 条）；去掉转换节点即得 ORIG 19 条全等（P41 变体：冗余 cast 诱发寄存器改派）。
    m_user->LogHistory("CeraPoint,%d,%d,%d", (int)type, amount,
                       m_user->GetCeraPoint());
}