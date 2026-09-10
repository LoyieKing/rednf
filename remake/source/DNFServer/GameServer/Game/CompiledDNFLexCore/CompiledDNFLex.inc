CompiledDNFLex::CompiledDNFLex()
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(0),
      stringBuffer_(0)
{
    std::deque<stream_t> tmp;
    streamStack_.swap(tmp);
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    flag2040_ = 1;
    lexerError_ = 0;
    loadStream_ = 0;
    create(true);
