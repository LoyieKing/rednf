// ORIG D1 0x0848089c：Reset() + map 成员析构（Reset 已 clear，析构为空操作）。
CPowerWarPacketLog::~CPowerWarPacketLog()
{
    Reset();
}