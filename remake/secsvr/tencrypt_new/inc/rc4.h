/* rc4.h -- RC4 流密码（由 DWARF 重建 + 实现修正）
   rc4_key 的柔性数组 state[] 修正为固定 256（sizeof 258 = 256+1+1 验证）。 */
#ifndef TENCRYPT_INC_RC4_H_
#define TENCRYPT_INC_RC4_H_

// sizeof = 258
struct rc4_key {
    unsigned char state[256];
    unsigned char x;
    unsigned char y;
};
typedef rc4_key rc4_key;

// sizeof = 12
struct CRc4 {
private:
    rc4_key *key;
    void *m_pSys;
    void *m_pLocal;
public:
    CRc4();
    ~CRc4();
    void Init(void *arg0, void *arg1);
    void UnInit();
    void Rc4(unsigned char *arg0, bool arg1, unsigned char *arg2, unsigned int arg3);
private:
    void crypt(unsigned char *arg0, unsigned int arg1);
    void set_key(unsigned char *arg0, unsigned int arg1);
};

#endif /* TENCRYPT_INC_RC4_H_ */
