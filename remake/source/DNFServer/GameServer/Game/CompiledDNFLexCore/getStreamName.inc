std::string CompiledDNFLex::getStreamName() const
{
    stream_t* p = streamStack_.end()._M_cur;
    bool none = true;
    if (p != streamStack_.begin()._M_cur)
    {
        stream_t** node = streamStack_.end()._M_node;
        if (p == streamStack_.end()._M_first)
            p = *(node - 1) + STREAM_NODE_ELEMS;
        if (p - 1 != 0)
            none = false;
    }
    std::string result = none ? std::string("NONE") : std::string(p[-1].dir);
    if (!none)
        result.append(p[-1].filename);
    return result;
