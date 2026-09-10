CharString CharString::join(const std::vector<CharString>& stringVector,
                            const CharString& separator)
{
    // [R22 2026-09-10] DIFF(gap=-30) → NEAR 196=196（判据 P292/P168a + 探针
    // /tmp/p_r22{,b,c,d}.cpp 定形）：ORIG 0x08ad9268/0x08ad9348 两遍扫描各带一个
    // runOnce 守卫——`movb $0,-0x19` → 头部 `cmpb $0,-0x19; jne <跳过整个循环块>`
    // → `&&` 右侧 `xorb $1,-0x19; cmpb $0,-0x19; je` → 体内 `while(runOnce){…;=false;}`。
    // 4.4 复现三要素：① 变量必须是 **bool**（uchar 时 while 恒 setne 物化，bool 才
    // `cmpb;jne` 直跳）；② 头部守卫写「`if (x) {} else { 整个循环块 }`」（反置空 then
    // ⇒ `cmpb;jne` 直跳；写 `if (!x)` 触发 4.4 bool 取反 xor 物化 N122）；③ 条件写
    // `(x = !x)`（产 ORIG 的 xorb 就地异或）。残余仅 1 字节局部/指针局部的槽位分配
    // 序差（OURS -0x21/-0x15 vs ORIG -0x19/-0x11）⇒ P112/N82 族，指令数与助记符全等。
    if (stringVector.empty())
    {
        return CharString();
    }
    const char* separatorStrSrc = separator.c_str();
    int separatorStrLen = separator.length();
    int totalLength = (static_cast<int>(stringVector.size()) - 1) * separatorStrLen;
    // [R22] ORIG 0x08ad9268/0x08ad9348 两遍扫描各带一个 runOnce bool 守卫
    // （movb $0,-0x19 / -0x11 → 循环头 cmpb+jne 跳过 → 条件段 xorb $1 取反），
    // 内层 while(runOnce){...; runOnce=false;} 只跑一次。照抄该形状。
    bool token_runOnce = false;
    if (token_runOnce)
    {
    }
    else
    {
        std::vector<CharString>::const_iterator it = stringVector.begin();
        std::vector<CharString>::const_iterator itEnd = stringVector.end();
        while (it != itEnd && (token_runOnce = !token_runOnce))
        {
            const CharString& token = *it;
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
        return CharString();
    }
    CharStringData* newData = CharStringData::createTerminated(totalLength);
    char* newDataBuf = newData->getBuffer();
    bool token_runOnce2 = false;
    if (token_runOnce2)
    {
    }
    else
    {
        std::vector<CharString>::const_iterator it2 = stringVector.begin();
        std::vector<CharString>::const_iterator itEnd2 = stringVector.end();
        while (it2 != itEnd2 && (token_runOnce2 = !token_runOnce2))
        {
            const CharString& token = *it2;
            while (token_runOnce2)
            {
                if (separatorStrLen >= 1 && it2 != stringVector.begin())
                {
                    memcpy(newDataBuf, separatorStrSrc, separatorStrLen);
                    newDataBuf += separatorStrLen;
                }
                int tokenLength = token.length();
                memcpy(newDataBuf, token.c_str(), tokenLength);
                newDataBuf += tokenLength;
                token_runOnce2 = false;
            }
            it2++;
        }
    }
    return CharString(newData);
