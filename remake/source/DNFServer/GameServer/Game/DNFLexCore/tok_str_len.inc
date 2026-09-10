int DNFLex::tok_str_len()
{
    return static_cast<int>(m_str_tok_pos - strTokBuf);
