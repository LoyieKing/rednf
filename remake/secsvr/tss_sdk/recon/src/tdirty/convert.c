/* Reconstructed from libtss_sdk.so.1.0.394 (tdirty/api/convert.c).
   ORIG is iconv based: GB2312ToUTF8 -> iconv_open("UTF-8","GB18030"),
   UTF8ToGB2312 -> iconv_open("GB18030","UTF-8").  Return value is the number
   of bytes produced, -1 on iconv failure (errno preserved). */
#include <errno.h>
#include <iconv.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int GB2312ToUTF8(char *pszOut, int iOutLen, const char *pszIn, int iInLen) {
    iconv_t cd;
    char *pIn;
    char *pOut;
    size_t iInLeft;
    size_t iOutLeft;

    cd = iconv_open("UTF-8", "GB18030");
    if (cd == (iconv_t)-1)
        return -1;

    memset(pszOut, 0, iOutLen);
    iconv(cd, 0, 0, 0, 0);

    pIn = (char *)pszIn;
    pOut = pszOut;
    iInLeft = iInLen;
    iOutLeft = iOutLen;

    if (iconv(cd, &pIn, &iInLeft, &pOut, &iOutLeft) == (size_t)-1) {
        int iErr = errno;
        iconv_close(cd);
        errno = iErr;
        return -1;
    }

    iconv_close(cd);
    return (int)(pOut - pszOut);
}

int UTF8ToGB2312(char *pszOut, int iOutLen, const char *pszIn, int iInLen) {
    iconv_t cd;
    char *pIn;
    char *pOut;
    size_t iInLeft;
    size_t iOutLeft;

    cd = iconv_open("GB18030", "UTF-8");
    if (cd == (iconv_t)-1)
        return -1;

    memset(pszOut, 0, iOutLen);
    iconv(cd, 0, 0, 0, 0);

    pIn = (char *)pszIn;
    pOut = pszOut;
    iInLeft = iInLen;
    iOutLeft = iOutLen;

    if (iconv(cd, &pIn, &iInLeft, &pOut, &iOutLeft) == (size_t)-1) {
        int iErr = errno;
        iconv_close(cd);
        errno = iErr;
        return -1;
    }

    iconv_close(cd);
    return (int)(pOut - pszOut);
}

#ifdef __cplusplus
}
#endif
