// ORIG T 0x08599712（报告 Sanicova__CPad/checkAnswer.md）：
// 清零 36 字节缓冲 ×2 → strncpy(answer, 8) → encryptAES_ → 与 m_pad 0x24 字节
// strncmp（ORIG call 0x0807e8c0 = strncmp@plt；密文二进制可能含 NUL，
// strncmp 遇 NUL 提前停止——ORIG 语义如此，不得改为 memcmp）。
bool Sanicova::CPad::checkAnswer(char* answer) const
{
    // b28d-36（修法，P214③/P253）：ORIG 0x08599712 对两缓冲各发内联
    // `mov $0x9,%edx; rep stos`（9 dword=36B）；源码 memset 使 4.4 恒发
    // call memset（N78 触发条件）⇒ 改 `= {0}` 显式零初始化即逐条复现。
    char plain[36] = {0};
    char cipher[36] = {0};

    strncpy(plain, answer, 8);
    encryptAES_(GlobalData::s_pcryptRijndael_MousePassword_, plain, cipher);
    return strncmp(m_pad, cipher, 0x24) == 0;
}