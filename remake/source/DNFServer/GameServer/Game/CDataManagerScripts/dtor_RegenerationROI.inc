RegenerationROI::~RegenerationROI()
{
    // ORIG D1 0x85f8506：+0x00 非空则 delete，置 0，析构 +0x04 regenerator
    if (m_regeneratorPtr != 0)
    {
        ::operator delete(m_regeneratorPtr);
    }
    m_regeneratorPtr = 0;
}