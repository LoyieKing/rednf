#ifndef DNF_GAME_CMYFILELOG_H_
#define DNF_GAME_CMYFILELOG_H_

// CMyFileLog —— 文件日志写入器。
// 权威定义/实现：ServerCommon/DNFFileLog.cpp（ORIG T 0810786c ctor /
// 08107898 operator()(const char*, const char*, ...)），含成员 name/level。
class CMyFileLog
{
public:
    CMyFileLog(const char* name, int level);
    void operator()(const char* filename, const char* format, ...);

private:
    const char* name;   // +0x00
    int level;          // +0x04
};

#endif  // DNF_GAME_CMYFILELOG_H_
