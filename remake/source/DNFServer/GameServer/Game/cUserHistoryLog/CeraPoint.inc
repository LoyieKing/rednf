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
