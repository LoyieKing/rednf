// ---- ChattingEmoticonBase 基类虚函数（ORIG 0x80e5f46/0x80e5dde）----
bool ChattingEmoticonBase::isCommonEmoticon(const CUser& user) const
{
    // ORIG 0x80e5f46：vtable+0x08（isUsable）转发。
    // [b28c-L12 代落·C28L19 判词·N7] 唯一残差 = 虚调用目标寄存器：ORIG
    // `mov (%eax),%edx; …; call *%edx` vs 4.4 恒 `call *%eax`（14/14 助记符
    // 全等，范式库 N7）；源形已最简，不可翻。
    return isUsable(user);
}