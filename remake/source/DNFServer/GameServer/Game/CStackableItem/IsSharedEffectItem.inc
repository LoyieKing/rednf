// ORIG _ZN14CStackableItem18IsSharedEffectItemEv W 0x0822ca18：movzbl 0x2ec(%eax),%eax —— 返回 m_char2ec 字节（共享特效物品标记）
char CStackableItem::IsSharedEffectItem()
{
    return m_char2ec;
}