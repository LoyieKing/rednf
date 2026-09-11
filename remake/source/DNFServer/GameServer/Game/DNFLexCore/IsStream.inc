bool DNFLex::IsStream()
{
    return stream_stack.end().base() != stream_stack.begin().base();
}