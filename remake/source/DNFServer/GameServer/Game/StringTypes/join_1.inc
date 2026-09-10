WideString WideString::join(const std::vector<WideString>& stringVector,
                            const WideString& separator)
{
    // [R22 2026-09-10] DIFF(gap=-30) → NEAR 200=200：同 CharString::join 的 runOnce
    // 守卫复现（ORIG 0x08adc34e/0x08adc42e）。残余为 1 字节局部槽位分配序差
    // （P112/N82 族，指令数与助记符全等）。
    if (stringVector.empty())
    {
        return WideString();
    }
    const wchar_t* separatorStrSrc = separator.c_str();
    int separatorStrLen = separator.length();
    int totalLength = (static_cast<int>(stringVector.size()) - 1) * separatorStrLen;
    // [R22] 同 CharString::join（ORIG 0x08adc34e/0x08adc42e 两遍 runOnce 守卫）。
    bool token_runOnce = false;
    if (token_runOnce)
    {
    }
    else
    {
        std::vector<WideString>::const_iterator it = stringVector.begin();
        std::vector<WideString>::const_iterator itEnd = stringVector.end();
        while (it != itEnd && (token_runOnce = !token_runOnce))
        {
            const WideString& token = *it;
            while (token_runOnce)
            {
                totalLength += token.length();
                token_runOnce = false;
            }
            it++;
        }
    }
    if (totalLength == 0)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::createTerminated(totalLength);
    wchar_t* newDataBuf = newData->getBuffer();
    bool token_runOnce2 = false;
    if (token_runOnce2)
    {
    }
    else
    {
        std::vector<WideString>::const_iterator it2 = stringVector.begin();
        std::vector<WideString>::const_iterator itEnd2 = stringVector.end();
        while (it2 != itEnd2 && (token_runOnce2 = !token_runOnce2))
        {
            const WideString& token = *it2;
            while (token_runOnce2)
            {
                if (separatorStrLen >= 1 && it2 != stringVector.begin())
                {
                    memcpy(newDataBuf, separatorStrSrc, separatorStrLen * sizeof(wchar_t));
                    newDataBuf += separatorStrLen;
                }
                int tokenLength = token.length();
                memcpy(newDataBuf, token.c_str(), tokenLength * sizeof(wchar_t));
                newDataBuf += tokenLength;
                token_runOnce2 = false;
            }
            it2++;
        }
    }
    return WideString(newData);
