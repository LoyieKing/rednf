#ifndef GAME_DB_SELECTLEVELUPREWARD_H_
#define GAME_DB_SELECTLEVELUPREWARD_H_

class Stream;

// df_game_r 还原 —— DB_SelectLevelUpReward（升级奖励领取确认 DB 请求，
// DBThread 派发）。参考 docs/class_func_reports/DB_SelectLevelUpReward/。
// 调用点：EventClassify::CEventScriptMng::process_level_up_reward
// （0x0810c03d，is_recv_check_item 路径）。
class DB_SelectLevelUpReward
{
public:
    // ORIG 0x0844271a（_ZN22DB_SelectLevelUpReward11makeRequestEiji，静态）：
    // 向 DB 队列（QUEUE_IDX 2=DB_Q）发一条请求，表头 0x28e + (uid, characNo, level)。
    static void makeRequest(int uid, unsigned int characNo, int level);
};

#endif  // GAME_DB_SELECTLEVELUPREWARD_H_
