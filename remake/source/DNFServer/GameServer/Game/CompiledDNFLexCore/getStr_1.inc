bool CompiledDNFLex::getStr(char* buffer, int maxSize)
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
    const std::string* r_string = stringManager_->getString(index);
    const char* src = r_string->c_str();
    int string_length = (int)r_string->size();
    if (__builtin_expect(string_length > maxSize, 0))
        goto overflow;
    std::memcpy(buffer, src, (unsigned)string_length);
    buffer[string_length] = 0;
    return true;
overflow:
    std::memcpy(buffer, src, (unsigned)(maxSize - 1));
    buffer[string_length - 1] = 0;
    return false;
