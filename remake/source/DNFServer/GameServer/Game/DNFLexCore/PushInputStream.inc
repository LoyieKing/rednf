// ---- DNFLex::PushInputStream(...)（ORIG 0x8acd7d0）----
// [b28d-19] AE（真实构建口径 = flags.make + build.make per-source `-O2 -std=gnu++98`；该口径下 178=178 逐条对齐。check_tu_routed.sh 曾漏 per-source 追加 flags 按 -O0 假报 DIFF，Main 已修）；ORIG 0x08acd7d0。
bool DNFLex::PushInputStream(const char* dir, const char* filename, stream_data_t* s)
// [P8S2 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
{
    stream_t stream;
    const char* d = "";
    std::size_t dlen = 0;
    if (dir != 0)
    {
        dlen = std::strlen(dir);
        d = dir;
    }
    stream.dir.assign(d, dlen);

    const char* f = "";
    std::size_t flen = 0;
    if (filename != 0)
    {
        flen = std::strlen(filename);
        f = filename;
    }
    stream.filename.assign(f, flen);

    if (s == 0)
    {
        if (!LoadStream(dir, filename, &stream.s))
        {
            std::sprintf(errBuf, "%s - DNFLex::ReadStream failed.", __FUNCTION__);
            LexerError(errBuf);
            return false;
        }
    }
    else
    {
        stream.s.stream = s->stream;
        stream.s.size = s->size;
        stream.s.buffer = s->buffer;
    }

    if (stream_stack.end().base() != stream_stack.begin().base())
        stream_stack.end().base()[-1].line_number = this->yylineno;

    stream_stack.push_back(stream);

    yy_buffer_state* new_buffer = this->yy_create_buffer(
        reinterpret_cast<std::istream*>(stream.s.stream), stream.s.size);
    this->yypush_buffer_state(new_buffer);
    this->yy_start = 1;
    this->yylineno = 1;
    return true;
}