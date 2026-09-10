void DNFLex::stream_data_t::destroy()
{
    if (stream != 0)
    {
        void (**vtable)(void*) = *reinterpret_cast<void (***)(void*)>(stream);
        vtable[1](stream);
        stream = 0;
    }
    if (buffer != 0)
    {
        ::operator delete[](buffer);   // ORIG _ZdaPv（buffer 由 new[] 分配）
        buffer = 0;
    }
