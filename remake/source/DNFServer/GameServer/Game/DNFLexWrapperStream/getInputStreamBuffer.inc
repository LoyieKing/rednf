// getInputStreamBuffer（ORIG 0x8ace970）：NORMAL 取 DNFLex stream_t +0x10，
// COMPILED 取 CompiledDNFLex stream_t +0x0c；其它返回 0。无空指针护栏。
char* DNFLexWrapper::getInputStreamBuffer()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.GetInputStream()->s.buffer;
    char* r = 0;
    if (lexType_ == LEX_TYPE_COMPILED)
        return compiledDnfLex_.GetInputStream()->buffer;
    return r;
}