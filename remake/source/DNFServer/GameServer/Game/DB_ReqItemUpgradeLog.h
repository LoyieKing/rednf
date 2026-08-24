#ifndef GAME_DB_REQITEMUPGRADELOG_H_
#define GAME_DB_REQITEMUPGRADELOG_H_

// df_game_r 还原 —— DB_ReqItemUpgradeLog（强化/增幅 DB 日志请求，LOGDB 派发）。
// 唯一声明点。
// 语义：向 LOGDB 队列发一条强化日志请求。参考
// docs/class_func_reports/DB_ReqItemUpgradeLog/makeRequest.md，ORIG 0x08425ece。
class DB_ReqItemUpgradeLog
{
public:
    // ORIG 0x08425ece：表头 0xc0，正文 (a,b,c) uint + d uint + e bool + str char*，
    // 均经 CStreamGuard::operator<< 写入。
    static void makeRequest(unsigned int a, unsigned int b, unsigned int c,
                            const char* str, unsigned int d, bool e);
};

#endif  // GAME_DB_REQITEMUPGRADELOG_H_
