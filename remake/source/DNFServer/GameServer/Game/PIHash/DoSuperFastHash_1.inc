int DoSuperFastHash(char* data, unsigned int len, unsigned int& hash);

// ---- MakeHash 前置（P210 callee 定义顺序定律：调用者定义须在 DoSuperFastHash
// 定义之前，否则 4.4 按 callee 帧精确继承 outgoing，0x38 保守大帧塌缩为 0x20）----
unsigned char MakeHash(unsigned char* data, unsigned int len, unsigned char seed)
{
    unsigned int hash;   // ORIG 无 init store（DoSuperFastHash 出参槽直传）
    DoSuperFastHash(data, len, hash);
    unsigned char* digest = reinterpret_cast<unsigned char*>(&hash);
    return static_cast<unsigned char>(digest[0] ^ digest[1] ^ digest[2] ^
                                      digest[3] ^ seed);
