// ---- DNFLex::GetInputStream()（ORIG 0x8acd280）----
DNFLex::stream_t* DNFLex::GetInputStream()
{
    stream_t* finish = stream_stack.end().base();
    stream_t* start = stream_stack.begin().base();
    unsigned empty = (unsigned)(finish == start);
    unsigned mask = empty - 1u;
    return reinterpret_cast<stream_t*>((unsigned)(finish - 1) & mask);
}