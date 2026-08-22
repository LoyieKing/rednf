#ifndef DNF_GAME_CMYFILELOG_H_
#define DNF_GAME_CMYFILELOG_H_

// CMyFileLog —— 文件日志写入器。ORIG 的 CMyFileLog(const char*,int) 与
// operator()(const char*,const char*,...) 由 ServerCommon/DNFFileLog.cpp 提供。
// 工程另有 PvP_GuildWar_Log.cpp 调用单参形式 operator()(const char*, ...)
// （_ZN10CMyFileLogclEPKcz，项目自造桥接符号），由 CMyFileLog.cpp 提供。
class CMyFileLog
{
public:
    void operator()(const char*, ...);
};

#endif  // DNF_GAME_CMYFILELOG_H_