CRijndael_fake::CRijndael_fake()
{   // ORIG 0x080b824c：salt=0xa321bcde ^ (rand()|rand()<<16)；new(0x268) 后 ptr^=salt
    m_salt = 0xa321bcdeu;
    unsigned int r1 = (unsigned int)std::rand();
    unsigned int r2 = (unsigned int)std::rand();
    m_salt ^= (r2 | (r1 << 16));
    // ORIG 0x080b824c：标量 `_Znwj`(0x268)，非 new[]（p167：我方曾发 `_Znaj`）
    m_ctxObf = ::operator new(0x268);          // symmetric_key(anubis) 空间
    m_ctxObf = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(m_ctxObf) ^ static_cast<std::size_t>(m_salt));
}