void CDataManager::rewriteUsableEmoticon(const CUser& user, char* text)
{
    ChattingEmoticonBase* emo;
    if (text == 0)
    {
        return;
    }
    const size_t len = strlen(text);
    for (int i = 0; (int)len - 2 > i; ++i)
    {
        if ((unsigned char)text[i] != 0x1e ||
            (unsigned char)text[i + 1] != ' ')
        {
            continue;
        }
        const int emoCode = (signed char)text[i + 2];  // asm movsbl
        emo = m_emoticonList->find_emoticon(emoCode);
        if (emo == 0)
        {
            text[i + 2] = (char)0xff;
        }
        else if (!emo->isUsable(user))
        {
            text[i + 2] = (char)0xff;
        }
    }
