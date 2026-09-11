int EncryptTool::wrapEncrypt(char* src, int len, char* out)
{
    // [b28c-lane25 判词·N7] ORIG 0x0848da30：虚调用 `mov (%eax),%ecx; … call *%ecx`
    // vs 4.4 间接 call 目标恒 %eax（N7 已录，8 件实证）。助记符序列全等仅寄存器名差。
    int blockSize = m_rijndael->GetBlockSize(0);
    int padded = 0;
    if (len % blockSize != 0)
    {
        padded = (len / blockSize + 1) * blockSize;
    }
    else
    {
        padded = len;
    }
    m_rijndael->Pad(src, len, 0);
    m_rijndael->Encrypt(src, out, padded);
    out[padded] = '\0';
    return padded;
}