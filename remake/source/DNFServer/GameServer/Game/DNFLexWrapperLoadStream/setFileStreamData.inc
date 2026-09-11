// setFileStreamData（ORIG 0x8acede0）：以 in|binary 打开 path 的 ifstream。
//   - path/s 任一为空 → 直接 false；
//   - 先 stream_data_t::clear(s)（stream/size/buffer 清零）；
//   - new ifstream(path, in|binary)；未打开（open 失败置 failbit）或 badbit
//     置位 → delete 流并返回 false；
//   - seekg(0, end) → tellg 取 size → seekg(0, beg)；
//   - size==0 → delete 流并委托 setStringStreamData(s, 0, 0)（空串流）；
//   - 否则 s->stream = ifs; s->size = size; 返回 true。
bool DNFLexWrapper::setFileStreamData(DNFLex::stream_data_t* s, const char* path)
{
    std::ifstream* ifs;
    int size;
    const char* p = path;
    DNFLex::stream_data_t* sd = s;

    if (p == 0)
        return false;
    if (sd == 0)
        return false;

    sd->clear();
    ifs = new std::ifstream(p, std::ios::in | std::ios::binary);
    if (!ifs->is_open())
        goto fail_delete;
    if (ifs->rdstate() & std::ios::badbit)
        goto fail_delete;

    ifs->seekg(0, std::ios::end);
    size = (int)ifs->tellg();
    ifs->seekg(0, std::ios::beg);
    if (size == 0)
    {
        delete ifs;
        return setStringStreamData(sd, 0, 0);
    }
    sd->stream = ifs;
    sd->size = size;
    return true;

fail_delete:
    delete ifs;
    return false;
}