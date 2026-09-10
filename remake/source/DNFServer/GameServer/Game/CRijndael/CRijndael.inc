CRijndael::CRijndael()
{
    // [B24D判词 N] 语义正确：基类 IMethod 构造 + m_bInit/m_blockSize/m_keyLength/
    //   m_mode/m_padding 初始化 + 多段 memset(m_key...m_temp3) + m_rounds=0。
    //   P167 差集 ORIG IMethodC1(完整构造) vs 我方 IMethodC2(基类构造)——C1/C2
    //   mangling 差异，基类构造逻辑相同，非语义缺口。diff=lowering。
    m_bInit = false;
    m_blockSize = 0x10;
    m_keyLength = 0;
    m_mode = 0;
    m_padding = 0;
    std::memset(m_key, 0, sizeof(m_key));
    std::memset(m_w, 0, sizeof(m_w));
    std::memset(m_wd, 0, sizeof(m_wd));
    std::memset(m_block, 0, sizeof(m_block));
    std::memset(m_chain, 0, sizeof(m_chain));
    std::memset(m_temp, 0, sizeof(m_temp));
    std::memset(m_temp2, 0, sizeof(m_temp2));
    std::memset(m_temp3, 0, sizeof(m_temp3));
    m_rounds = 0;
