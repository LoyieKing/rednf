// ORIG 0x081ae578（W）：wireMagic='U'; *(u16*)netLen=htons(bodyLength) 指针直存
// （dst 0x081ae585 add $5 单步 + 0x081ae588 mov %eax,%ebx 先载入，技巧27）。
inline bool NCashMessage::encode_header()
{
    m_wireMagic = 'U';
    *reinterpret_cast<unsigned short*>(m_netLenB) = htons(m_bodyLength);
    return true;
}