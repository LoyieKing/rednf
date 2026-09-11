// closeInputStream（ORIG 0x8acea70）：依次调用 DNFLex::CloseInputStream 与
// CompiledDNFLex::CloseInputStream。
void DNFLexWrapper::closeInputStream()
{
    dnfLex_.CloseInputStream();
    compiledDnfLex_.CloseInputStream();
}