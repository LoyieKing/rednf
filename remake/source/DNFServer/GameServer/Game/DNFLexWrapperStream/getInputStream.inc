// getInputStream（ORIG 0x8ace900）：仅 NORMAL 分流到 DNFLex::GetInputStream，
// 取其返回流对象偏移 +0x8 的 istream*；其它 lexType 返回 0。
void* DNFLexWrapper::getInputStream()
{
    void* r = 0;
    if (lexType_ != LEX_TYPE_NORMAL)
        return r;
    return dnfLex_.GetInputStream()->s.stream;
}