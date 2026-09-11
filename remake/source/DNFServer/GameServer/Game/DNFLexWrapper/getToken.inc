int DNFLexWrapper::getToken()
{
    if (lexType_ == LEX_TYPE_NORMAL)
    {
        lastToken_ = dnfLex_.get_next_token();
        return lastToken_;
    }
    if (lexType_ == LEX_TYPE_COMPILED)
    {
        CompiledDNFLex* c = &compiledDnfLex_;
        int tok = c->getToken();
        lastToken_ = tok;
        if ((unsigned)tok <= 1)
            return tok;
        unsigned d = (unsigned)tok - 2;
        if (d <= 1 || tok == DNF_TOK_PAIR_INT)
        {
            lastIntValue_ = c->getInt(0);
            return lastToken_;
        }
        if (tok == DNF_TOK_FLOAT)
        {
            lastFloatValue_ = c->getFloat(0);
            return lastToken_;
        }
        lastStringValue_ = c->getStr((bool*)0);
    }
    return lastToken_;
}