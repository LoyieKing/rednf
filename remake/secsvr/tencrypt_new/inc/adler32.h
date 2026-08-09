/* adler32.h -- Adler-32（RFC 1950 / zlib）的 tencrypt_new 变体
   符号核对：_ZN8CAdler327adler32EjPKhj。 */

#ifndef TENCRYPT_INC_ADLER32_H_
#define TENCRYPT_INC_ADLER32_H_

class CAdler32 {
public:
    CAdler32();
    ~CAdler32();
    unsigned int adler32(unsigned int adler, const unsigned char *buf, unsigned int len);
};

#endif /* TENCRYPT_INC_ADLER32_H_ */
