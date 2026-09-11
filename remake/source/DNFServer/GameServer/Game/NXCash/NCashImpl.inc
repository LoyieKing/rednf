// ORIG 0x081acfdc：NCashImpl ctor 本体（TU 首符号 ⇒ GI 键名 C2 形）。
NCashImpl::NCashImpl(const char* serverAdrs, short port, unsigned char paramB,
                     unsigned short fieldC, short fieldD)
    : m_socket(), m_inMsg(), m_outMsg()
{
    // ORIG 0x081acfdc：子对象构造（m_socket@+0x20、m_inMsg@+0x40、m_outMsg@+0x248）
    // 后依次写 m_requestID=0、m_paramB、m_fieldC、m_fieldD、m_connected=0、
    // strcpy(m_serverAdrs, serverAdrs)（无 NULL 检查/预清零）、m_port。
    m_requestID = 0;
    m_paramB = paramB;
    m_fieldC = fieldC;
    m_fieldD = fieldD;
    m_connected = false;
    strcpy(m_serverAdrs, serverAdrs);
    m_port = port;
}