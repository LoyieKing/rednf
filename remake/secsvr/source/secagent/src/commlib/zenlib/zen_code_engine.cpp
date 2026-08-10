// Restored from secagent binary (reverse-engineered, behavior matched).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag435/src/commlib/zenlib/zen_code_engine.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
//
// 自定义二进制编解码：整数/短整型大端序（字节交换）写入，字符串为
// 2 字节大端长度前缀 + 数据。所有函数失败返回 0，成功返回消耗的字节数。

#include "src/commlib/zenlib/zen_code_engine.h"
#include <cstring>

namespace AB_NS_CODEENGINE {

int EncodeChar(char **pstrEncode, unsigned char ucSrc) {
    if (pstrEncode == NULL || *pstrEncode == NULL) {
        return 0;
    }
    **pstrEncode = (char)ucSrc;
    ++(*pstrEncode);
    return 1;
}

int DecodeChar(char **pstrDecode, unsigned char *pucDest) {
    if (pstrDecode == NULL || *pstrDecode == NULL || pucDest == NULL) {
        return 0;
    }
    *pucDest = (unsigned char)**pstrDecode;
    ++(*pstrDecode);
    return 1;
}

int EncodeShort(char **pstrEncode, unsigned short usSrc) {
    if (pstrEncode == NULL || *pstrEncode == NULL) {
        return 0;
    }
    unsigned short usTemp = (unsigned short)((usSrc >> 8) | (usSrc << 8));
    *(unsigned short *)*pstrEncode = usTemp;
    *pstrEncode += 2;
    return 2;
}

int DecodeShort(char **pstrDecode, unsigned short *pusDest) {
    if (pstrDecode == NULL || *pstrDecode == NULL || pusDest == NULL) {
        return 0;
    }
    unsigned short usTemp = *(unsigned short *)*pstrDecode;
    *pstrDecode += 2;
    *pusDest = (unsigned short)((usTemp >> 8) | (usTemp << 8));
    return 2;
}

int EncodeInt(char **pstrEncode, unsigned int uiSrc) {
    if (pstrEncode == NULL || *pstrEncode == NULL) {
        return 0;
    }
    unsigned int uiTemp = (uiSrc >> 24)
                        | ((uiSrc >> 8) & 0x0000FF00)
                        | ((uiSrc << 8) & 0x00FF0000)
                        | (uiSrc << 24);
    *(unsigned int *)*pstrEncode = uiTemp;
    *pstrEncode += 4;
    return 4;
}

int DecodeInt(char **pstrDecode, unsigned int *puiDest) {
    if (pstrDecode == NULL || *pstrDecode == NULL || puiDest == NULL) {
        return 0;
    }
    unsigned int uiTemp = *(unsigned int *)*pstrDecode;
    *pstrDecode += 4;
    *puiDest = (uiTemp >> 24)
             | ((uiTemp >> 8) & 0x0000FF00)
             | ((uiTemp << 8) & 0x00FF0000)
             | (uiTemp << 24);
    return 4;
}

int EncodeString(char **pstrEncode, char *strSrc, short int sMaxStrLength) {
    if (pstrEncode == NULL || *pstrEncode == NULL || strSrc == NULL || sMaxStrLength <= 0) {
        return 0;
    }
    unsigned short usTempLength = (unsigned short)strlen(strSrc);
    unsigned short usRealLength = (usTempLength > (unsigned short)sMaxStrLength)
                                  ? (unsigned short)sMaxStrLength : usTempLength;
    unsigned short usTemp = (unsigned short)((usRealLength >> 8) | (usRealLength << 8));
    *(unsigned short *)*pstrEncode = usTemp;
    *pstrEncode += 2;
    memcpy(*pstrEncode, strSrc, usRealLength);
    *pstrEncode += usRealLength;
    return usRealLength + 2;
}

int DecodeString(char **pstrDecode, char *strDest, short int sMaxStrLength) {
    if (pstrDecode == NULL || *pstrDecode == NULL || strDest == NULL || sMaxStrLength <= 0) {
        return 0;
    }
    unsigned short usTemp = *(unsigned short *)*pstrDecode;
    *pstrDecode += 2;
    unsigned short usTempLength = (unsigned short)((usTemp >> 8) | (usTemp << 8));
    unsigned short usRealLength = (usTempLength > (unsigned short)sMaxStrLength)
                                  ? (unsigned short)sMaxStrLength : usTempLength;
    memcpy(strDest, *pstrDecode, usRealLength);
    *pstrDecode += usTempLength;
    strDest[usRealLength] = 0;
    return usTempLength + 2;
}

int EncodeMem(char **pstrEncode, char *pcSrc, short int sMemSize) {
    if (pstrEncode == NULL || *pstrEncode == NULL || pcSrc == NULL || sMemSize <= 0) {
        return 0;
    }
    memcpy(*pstrEncode, pcSrc, (size_t)sMemSize);
    *pstrEncode += sMemSize;
    return sMemSize;
}

int DecodeMem(char **pstrDecode, char *pcDest, short int sMemSize) {
    if (pstrDecode == NULL || *pstrDecode == NULL || pcDest == NULL || sMemSize <= 0) {
        return 0;
    }
    memcpy(pcDest, *pstrDecode, (size_t)sMemSize);
    *pstrDecode += sMemSize;
    return sMemSize;
}

int EncodeMemInt(char **pstrEncode, char *pcSrc, int sMemSize) {
    if (pstrEncode == NULL || *pstrEncode == NULL || pcSrc == NULL || sMemSize <= 0) {
        return 0;
    }
    memcpy(*pstrEncode, pcSrc, (size_t)sMemSize);
    *pstrEncode += sMemSize;
    return sMemSize;
}

int DecodeMemInt(char **pstrDecode, char *pcDest, int sMemSize) {
    if (pstrDecode == NULL || *pstrDecode == NULL || pcDest == NULL || sMemSize <= 0) {
        return 0;
    }
    memcpy(pcDest, *pstrDecode, (size_t)sMemSize);
    *pstrDecode += sMemSize;
    return sMemSize;
}

} // namespace AB_NS_CODEENGINE
