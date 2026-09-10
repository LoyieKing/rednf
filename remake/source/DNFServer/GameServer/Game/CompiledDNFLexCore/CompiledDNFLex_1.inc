CompiledDNFLex::CompiledDNFLex(ScriptStringManager* pStringManager)
    : offset_(0),
      lastToken_(0),
      stringManagerDelete_(false),
      stringManager_(pStringManager),
      stringBuffer_(0)
{
    // ORIG 08ad073e/08ad0741（`mov %esi,(%esp)` + `call <_ZNSt5dequeI…stream_t…D1Ev>`）
    // 位于 08ad0746（tokenBuffer_ memset 准备）之前 ⇒ tmp 的来源作用域在 swap 后即结束
    // （析构提前到 memset/create 之前）。块作用域复现该提前析构位置。
    {
        std::deque<stream_t> tmp;
        streamStack_.swap(tmp);
    }
    std::memset(tokenBuffer_, 0, sizeof(tokenBuffer_));
    flag2040_ = 1;
    lexerError_ = 0;
    loadStream_ = 0;
    create(false);
