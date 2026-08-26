#ifndef GAME_CSWITCHLOG_H_
#define GAME_CSWITCHLOG_H_

// ============================================================================
// df_game_r 还原 —— CSwitchLog 轻量权威声明
// 类本体由 GameServer/Game/CSwitchLog.cpp 提供（ORIG C1 0x0854fd3c /
// operator() 0x0854fd64 / 静态 SetLogWriteFlag 0x0854fd18）。
// 抽出原因：CFairMatchRooms 等 TU 需要 include 使用，原先仅在该 cpp 内
// 本地定义，无法跨 TU 复用。布局 +0x00 name/+0x04 line/+0x08 level/
// +0x0c flagIndex（ORIG 报告 docs/class_func_reports/CSwitchLog.md）。
// ============================================================================

class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int level, int flagIndex);
    void operator()(const char* fmt, ...);
    static void SetLogWriteFlag(int flag);

private:
    const char* m_name;    // +0x00
    int m_line;            // +0x04
    int m_level;           // +0x08
    int m_flagIndex;       // +0x0c

    static char log_write_[1];   // ORIG 0x0943df78，1 字节
};

#endif  // GAME_CSWITCHLOG_H_
