int importRewards(std::vector<stTitleRewardBasis*>* out, std::string token)
{
    std::string typeText;
    const unsigned int appendage =
        static_cast<unsigned int>(loadAppendageType(token));

    // ORIG 0x08aa8ba4: cmpl $-1,槽; setne %al; test %al,%al; je <else>
    while (appendage != static_cast<unsigned int>(-1))
    {
        stRewardChangeStatus* reward = new stRewardChangeStatus();
        reward->m_appendage = appendage;                    // +0x0c
        ScanStr(&typeText);
        reward->m_valueType = loadValueType(typeText);      // +0x10
        reward->m_value = ScanFloat((bool*)0);              // +0x14（fstps）
        out->push_back(reward);
        goto rewardsTail;                                   // ORIG 0x08aa8c10 jmp 尾
    }
    if (token == "[give emotion]")
    {
        stRewardGiveEmotion* reward = new stRewardGiveEmotion();
        reward->m_emotionId = ScanInt((bool*)0);            // +0x0c
        out->push_back(reward);
    }
    else if (token == "[send postal]")
    {
        int deadPostal = 0;
        stRewardPostal* reward = new stRewardPostal();
        reward->m_itemIdx = ScanInt((bool*)0);              // +0x18
        reward->m_addInfo = ScanInt((bool*)0);              // +0x1c
        ScanStr(&reward->m_sender);                         // +0x0c
        ScanStr(&reward->m_subject);                        // +0x10
        ScanStr(&reward->m_message);                        // +0x14
        out->push_back(reward);
    }
rewardsTail:
    return 1;
