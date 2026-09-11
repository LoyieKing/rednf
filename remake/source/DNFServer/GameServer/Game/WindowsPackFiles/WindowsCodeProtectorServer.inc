// ===========================================================================
// WindowsCodeProtectorServer —— 0x08575462..
// ===========================================================================
WindowsCodeProtectorServer::WindowsCodeProtectorServer()
    : m_dataFamilies()
{
    Initialize();                                            // ORIG 0x08575462：仅 vector 构造 + Initialize()
}