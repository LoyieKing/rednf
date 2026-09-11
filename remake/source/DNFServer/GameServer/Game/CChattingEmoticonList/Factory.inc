// ============================================================================
// Factory（ORIG 0x80e53a8）
// 依 script.m_type 分派构造对应派生类；SetIndex 在**每个 case 内**执行
// （ORIG +0x73/+0xc3/+0x11a 三处独立 call，非共享尾），default 仅保持
// result=0（ORIG cmp 链尾 `cmp $1; jne end` 直落出口，无额外写）。
// ============================================================================
ChattingEmoticonBase* CChattingEmoticonList::Factory(
    const EmoticonType::EmoticonScript& script)
{
    ChattingEmoticonBase* result = 0;
    switch (script.m_type)
    {
    case 1:
        result = new ChattingEmoticon_byEvent();
        result->SetIndex(script.m_index);
        break;
    case 2:
        result = new ChattingEmoticon_byPremium();
        result->SetIndex(script.m_index);
        break;
    case 4:
        result = new ChattingEmoticon_byLevel(script.m_param);
        result->SetIndex(script.m_index);
        break;
    default:
        break;
    }
    return result;
}