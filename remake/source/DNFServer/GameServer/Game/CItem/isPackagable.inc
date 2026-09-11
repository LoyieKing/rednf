// ORIG 0x828b5b4：（movzbl 0x60(%eax); sete %al）即 this+0x60 字节 == 1
bool CItem::isPackagable() const
{
    return m_char60 == 0x1;
}