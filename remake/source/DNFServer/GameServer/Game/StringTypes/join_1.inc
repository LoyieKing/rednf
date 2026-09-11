// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
WideString WideString::join(const std::vector<WideString>& stringVector,
                            const WideString& separator)
{
    // [R22 2026-09-10] DIFF(gap=-30) → NEAR 200=200：同 CharString::join 的 runOnce
    // 守卫复现（ORIG 0x08adc34e/0x08adc42e）。残余为 1 字节局部槽位分配序差
    // （P112/N82 族）已由下方「槽序归零组合旋钮」归零（compare_tu 口径 near→ae）。
    if (stringVector.empty())
    {
        return WideString();
    }
    const wchar_t* separatorStrSrc = separator.c_str();
    int separatorStrLen = separator.length();
    int totalLength = (static_cast<int>(stringVector.size()) - 1) * separatorStrLen;
    // [L4e 2026-09-11] 同 CharString::join：newData/newDataBuf 顶层声明提前 + 两个
    // runOnce bool 移入各自块 ⇒ 槽序复现 ORIG（newData -0x24 / newDataBuf -0x20 /
    // bool1 -0x19 / token1 -0x18）。
    WideStringData* newData;
    wchar_t* newDataBuf;
    {
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
    }
    if (totalLength == 0)
    {
        return WideString();
    }
    newData = WideStringData::createTerminated(totalLength);
    newDataBuf = newData->getBuffer();
    {
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
    }
    return WideString(newData);
}