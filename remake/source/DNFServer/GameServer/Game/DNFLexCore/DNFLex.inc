DNFLex::DNFLex()
    : __dnf_script__FlexLexer(0, 0),
      m_str_tok_pos(0), is_dbcs_(0), allow_inclusion_(1),
      isDbcsLeadChar(0), lexerOutput(0), lexerError(0), loadStream(0)
{
    std::memset(errBuf, 0, sizeof(errBuf));
    std::memset(strTokBuf, 0, sizeof(strTokBuf));
}