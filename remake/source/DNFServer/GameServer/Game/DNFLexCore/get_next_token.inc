int DNFLex::get_next_token()
{
    int tok = 0;
    if (stream_stack.end().base() != stream_stack.begin().base())
    {
        tok = dnf_yylex(this);
        if (allow_inclusion_)
        {
            if (tok == DNF_TOK_INCLUDE)
            {
                stream_t* cur = GetInputStream();
                int next = dnf_yylex(this);
                if (next == DNF_TOK_STRING)
                {
                    if (PushInputStream("", toTString(strTokBuf), 0))
                        return dnf_yylex(this);
                    std::sprintf(errBuf,
                                 "%s - %s/%s, line %d, PushInputStream failed.",
                                 __FUNCTION__, toMbcs(cur->dir),
                                 toMbcs(cur->filename), yylineno);
                }
                else
                {
                    std::sprintf(errBuf,
                                 "%s - %s/%s, line %d, invalid inclusion.",
                                 __FUNCTION__, toMbcs(cur->dir),
                                 toMbcs(cur->filename), yylineno);
                }
                LexerError(errBuf);
                return DNF_TOK_ERROR;
            }
            if (tok == DNF_TOK_EOF)
            {
                if (PopInputStream(false))
                    return dnf_yylex(this);
            }
        }
    }
    return tok;
