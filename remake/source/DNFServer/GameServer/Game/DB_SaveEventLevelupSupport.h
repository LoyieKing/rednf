#ifndef GAME_DB_SAVEEVENTLEVELUPSUPPORT_H_
#define GAME_DB_SAVEEVENTLEVELUPSUPPORT_H_

// df_game_r Game/ DB_SaveEventLevelupSupport（DB 请求封装；独立叶子头）。
//
// ORIG（df_game_r, elf32-i386）：
//   _ZN26DB_SaveEventLevelupSupport11makeRequestEiiii  T 0x0844cc2e
//   静态成员。函数体经 StreamPool::Acquire(0x940bd6c, "DBThread.cpp", 0xc25a)
//   + CStreamGuard 构造，顺序写 int 0x34f、四个实参，最后
//   MsgQueueMgr::put((QUEUE_IDX)2, guard) 入 DB_Q。
//   流序 = (accId, characNo, level, state)：W8LT_DBTask.cpp 的
//   DB_SaveEventLevelupSupport::dispatch（ORIG 0x0844cafe）读流
//   characNo→level→state，UpdateEventInfo 调用点（ORIG 0x081472f2）压栈
//   `$1, level, characNo, uid` 双向实证第 2/3 参语义（旧形参名 level/characNo 反）。
class DB_SaveEventLevelupSupport
{
public:
    static void makeRequest(int accId, int characNo, int level, int state);
};

#endif  // GAME_DB_SAVEEVENTLEVELUPSUPPORT_H_
