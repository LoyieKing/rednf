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
