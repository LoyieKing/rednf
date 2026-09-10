CSchoolMatchEvent::~CSchoolMatchEvent()
{
    // ORIG 0x08273f1a：写 vptr 后 m_channels 析构 + CEventBase 析构（编译器生成）。
