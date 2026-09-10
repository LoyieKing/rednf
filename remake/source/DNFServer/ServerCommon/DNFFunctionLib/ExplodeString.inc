int DNFFLib::ExplodeString(char* str, char* delims, char** out, int maxCount) {
    // ORIG DWARF 局部：iTokenCnt(158)/pToken(159)/pLast(160)/pTail(160)。
    int iTokenCnt = 0;
    if (maxCount < 1 || str == NULL) {
        out[0] = str;
        return 0;
    }
    // 原始：直接存尾指针 end = str + strlen(str)（mov [ebp-0x10],eax）
    // ORIG 声明序：pToken(159) 先于 pLast(160)，但体中先赋 pLast 再 pToken。
    char* pToken;
    char* pLast;
    pLast = str + strlen(str);
    pToken = strtok(str, delims);
    while (pToken != NULL) {
        out[iTokenCnt] = pToken;
        iTokenCnt++;
        if (iTokenCnt >= maxCount) {
            // 原始：token + strlen(token) 存为临时指针（mov [ebp-0xc],eax）
            char* pTail = pToken + strlen(pToken);
            if (pTail == pLast) {
                out[iTokenCnt] = NULL;
            } else {
                out[iTokenCnt] = pTail + 1;
            }
            break;
        }
        pToken = strtok(NULL, delims);
    }
    return iTokenCnt;
