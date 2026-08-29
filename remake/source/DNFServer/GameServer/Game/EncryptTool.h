#ifndef GAME_ENCRYPTTOOL_H_
#define GAME_ENCRYPTTOOL_H_

// EncryptTool（ORIG 还原，2026-08-28）。
// 依据 docs/class_func_reports/EncryptTool/* 与 /tmp/df_game_r objdump：
//   - sizeof=0x208（GlobalInstance<EncryptTool>::create new $0x208 实证）；
//   - 布局：m_key[16][0x20]@0x000、m_keyIndex@0x200、m_rijndael@0x204；
//   - C1/C2(0x848e0ea)：memset(this,0,0x20)；m_rijndael=new CRijndael；
//     16×0x20 块各自 memset+strncpy 静态密钥表（表 @0x93ab8e0）；
//   - createEncKey(0x848d5b0)：m_keyIndex=time()&0xf；SeedRand 播种；
//     用 m_key[0..15] 各块特定字节 + m_keyIndex + 常量生成 m_key[m_keyIndex]，
//     再 CRijndael::Initialize（返回 void，Ghidra C 实证）；
//   - setCryptKey(0x848d94a)：Initialize(this+index*0x20,…)（返回 void）；
//   - getKeyIndex(0x848d9a2)：return m_keyIndex（int）；
//   - wrapEncrypt(0x848d9b0)：GetBlockSize→Pad→Encrypt 虚调用→尾 '\0'，
//     返回 padding 后长度（int）。

class CRijndael;

class EncryptTool
{
public:
    EncryptTool();
    int getKeyIndex();
    void setCryptKey(int index);
    int wrapEncrypt(char* src, int len, char* out);
    void createEncKey();

private:
    unsigned char m_key[16][0x20];  // +0x000 16 组密钥源（ctor 由静态表填充）
    int           m_keyIndex;       // +0x200
    CRijndael*    m_rijndael;       // +0x204
};

#endif  // GAME_ENCRYPTTOOL_H_
