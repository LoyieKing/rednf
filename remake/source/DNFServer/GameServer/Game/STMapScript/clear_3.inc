void BackAniScript::clear()                 // ORIG 0x089e884a
{
    m_filename = "";                        // ORIG Ss::operator=("")
    m_layer = 1;                            // +0x04
    m_orderUpper = false;                   // +0x08
    m_loopYAxis = false;                    // +0x09
    m_hasScreenPos = false;                 // +0x0a
    m_screenX = 0;                          // +0x0c
    m_screenY = 0;                          // +0x10
}