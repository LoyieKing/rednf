#ifndef GAME_DB_SAVEEVENTLEVELUPSUPPORT_H_
#define GAME_DB_SAVEEVENTLEVELUPSUPPORT_H_

// df_game_r Game/ DB_SaveEventLevelupSupport（DB 请求封装；独立叶子头）。
//
// ORIG（df_game_r, elf32-i386）：
//   _ZN26DB_SaveEventLevelupSupport11makeRequestEiiii  T 0x0844cc2e
//   静态成员。函数体经 StreamPool::Acquire(0x940bd6c, "DBThread.cpp", 0xc25a)
//   + CStreamGuard 构造，顺序写 int 0x34f、四个实参，最后
//   MsgQueueMgr::put((QUEUE_IDX)2, guard) 入 DB_Q。
class DB_SaveEventLevelupSupport
{
public:
    static void makeRequest(int accId, int level, int characNo, int state);
};

#endif  // GAME_DB_SAVEEVENTLEVELUPSUPPORT_H_
