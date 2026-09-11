// ---- IMethod 抽象基类（G7 加密模式基类，算法正确即可）----
IMethod::IMethod()
    : m_bInit(false), m_blockSize(8), m_keyLength(0), m_mode(0), m_padding(0)
{
}