std::string DNFLex::getStreamName()
{
    stream_t* finish = stream_stack.end().base();
    if (finish == stream_stack.begin().base())
        return std::string("NONE");
    std::string result((finish - 1)->dir);
    result.append((finish - 1)->filename);
    return result;
