// getInputStreamSize（ORIG 0x8ace930）：NORMAL 取 DNFLex stream_t +0x0c；
// COMPILED 取 CompiledDNFLex stream_t +0x08；其它返回 -1。无空指针护栏。
int DNFLexWrapper::getInputStreamSize()
{
    if (lexType_ == LEX_TYPE_NORMAL)
        return dnfLex_.GetInputStream()->s.size;
    int r = -1;
    if (lexType_ == LEX_TYPE_COMPILED)
        return (int)compiledDnfLex_.GetInputStream()->size;
    return r;
}