// IncrementDBLoadCount ORIG 0x823465a：+0 自增，归零(回绕)则 Overflow(+8) +1。
void CExchangeServerHitRate::IncrementDBLoadCount()
{
    ++m_dbLoadCount;
    // [b28d-solo-ExchangeServer_Infra] P0/N96 清绑：ORIG 0x823466a 段
    // `mov (%eax),%eax; test %eax,%eax; sete %al; test %al,%al; je` 且全帧无
    // 局部槽 ⇒ P225 while+break 四段物化形（原 `register bool z asm("al")` 已删）。
    while (m_dbLoadCount == 0)
    {
        ++m_dbLoadOverflowCount;
        break;
    }
}