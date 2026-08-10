// Restored from secagent binary (reverse-engineered, behavior matched).
// Original path: src/commlib/zenlib/zen_code_engine.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CODE_ENGINE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CODE_ENGINE_H_H_

namespace AB_NS_CODEENGINE {

int EncodeChar(char **pstrEncode, unsigned char ucSrc);
int DecodeChar(char **pstrDecode, unsigned char *pucDest);
int EncodeShort(char **pstrEncode, unsigned short usSrc);
int DecodeShort(char **pstrDecode, unsigned short *pusDest);
int EncodeInt(char **pstrEncode, unsigned int uiSrc);
int DecodeInt(char **pstrDecode, unsigned int *puiDest);
int EncodeString(char **pstrEncode, char *strSrc, short int sMaxStrLength);
int DecodeString(char **pstrDecode, char *strDest, short int sMaxStrLength);
int EncodeMem(char **pstrEncode, char *pcSrc, short int sMemSize);
int DecodeMem(char **pstrDecode, char *pcDest, short int sMemSize);
int EncodeMemInt(char **pstrEncode, char *pcSrc, int sMemSize);
int DecodeMemInt(char **pstrDecode, char *pcDest, int sMemSize);

} // namespace AB_NS_CODEENGINE

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CODE_ENGINE_H_H_
