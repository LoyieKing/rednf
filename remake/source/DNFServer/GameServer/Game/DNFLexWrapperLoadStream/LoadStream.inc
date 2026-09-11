// LoadStream（ORIG 0x8ace2b0）：若 SetHandler 注册了 LoadStream 回调
// （+0x610c 非空），以 (dir, filename, s) 尾调用之并返回其 bool；
// 否则返回 false。ORIG 汇编：mov 0x610c(%eax),%eax → test/je → jmp *%eax。
bool DNFLexWrapper::LoadStream(const char* dir, const char* filename,
                               DNFLex::stream_data_t* s)
{
    if (loadStream_ == 0)
        return false;
    return reinterpret_cast<bool (*)(const char*, const char*, DNFLex::stream_data_t*)>(
        loadStream_)(dir, filename, s);
}