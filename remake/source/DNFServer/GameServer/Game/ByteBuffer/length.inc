// ORIG 0x081acf60：length = m_writePtr - m_start。
// 定义置于全部 <<x/>>x 消费者之后：ORIG 各 W body 内 call length 为宽松帧
// （sub $0x18 + 具名 value 槽 -0xc），同 TU 定义前置会收紧 caller 帧（技巧31）。
unsigned int ByteBuffer::length() const
{
    return static_cast<unsigned int>(m_writePtr - m_start);
}
