// ORIG W 0x85f7be0（0x0f）：mov 0x8(%ebp),%eax; movzbl 0x144(%eax),%eax; ret
// [2026-09-10 E01 落 · Main 批准] 返回类型 uchar→bool（与 CItem.h 声明同批）：
// ORIG 消费端 0x085f77f4 `call; test %al,%al; je` 无 setne 归一（P248 档①）。
// 返回类型不入 mangled；体仍为 movzbl 直返（bool 返回不插 setne，P292 勘误）。
bool CItem::IsBanRedeemItem() const
{
    return m_char144;
}