std::vector<int> CDataManager::IsUsableEmoticons(const CUser& user,
                                                 const std::vector<int>& emoticons)
{
    std::vector<int> result;
    if (m_emoticonList != 0)
    {
        ChattingEmoticonBase* p;
        for (unsigned int i = 0; i < emoticons.size(); ++i)
        {
            p = m_emoticonList->find_emoticon(emoticons[i]);
            if (p == 0)
                continue;
            if (!p->isUsable(user))
                continue;
            result.push_back(emoticons[i]);
        }
    }
    return result;
