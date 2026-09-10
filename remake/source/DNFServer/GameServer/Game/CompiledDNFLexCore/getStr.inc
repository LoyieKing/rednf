bool CompiledDNFLex::getStr(std::string& stringBuffer)
{
    int tok = lastToken_;
    unsigned d = (unsigned)tok - 2;
    if (d <= 1)
        return false;
    if (tok == DNF_TOK_PAIR_INT)
        return false;
    if (tok == DNF_TOK_FLOAT)
        return false;
    if (tok == DNF_TOK_EOF)
        return false;
    if (tok == DNF_TOK_ERROR)
        return false;
    int index = readIntFromBuffer(offset_);
    return stringManager_->getString(index, &stringBuffer);
