/* crc32.h -- CRC-32（PNG 多项式）的 tencrypt_new 变体
   符号核对：_ZN6CCrc325Crc32EPhj、_ZN6CCrc329CalcCrc32EhRj。 */

#ifndef TENCRYPT_INC_CRC32_H_
#define TENCRYPT_INC_CRC32_H_

class CCrc32 {
public:
    CCrc32();
    ~CCrc32();
    unsigned int Crc32(unsigned char *buf, unsigned int nsize);
private:
    void CalcCrc32(unsigned char byte, unsigned int &dwCrc32);
};

#endif /* TENCRYPT_INC_CRC32_H_ */
