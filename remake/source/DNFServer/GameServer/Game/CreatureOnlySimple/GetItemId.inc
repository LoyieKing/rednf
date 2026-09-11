// ORIG _ZN13user_creature13CCreatureItem9GetItemIdEv W 0x0833efa4（size 0x0b）：
// mov 0x4(%eax),%eax 直返。ORIG 为外联弱符号，CEgg::SaveCreatureItem 以 call 调
// （不内联）；头内仅声明、此处外联定义产出真实 call。定义**置于 SaveCreatureItem
// 之后**（P210）：调用点不见定义→保守 outgoing 帧 sub $0x18 与 ORIG 一致。
int CCreatureItem::GetItemId()
{
    return m_itemId;
}