// advancealtar::DB_Load —— ORIG W 声明头（仅声明，无实现）。
// ORIG：C1/C2 W 0x0844ddea、D1/D2 W 0x081420a0、D0 W 0x081420d0。
// 方法集来自 advancealtar_DB.cpp 局部类定义（当前 cpp-only，供未来收敛 include）。
#ifndef DNF_GAME_DB_LOAD_H_
#define DNF_GAME_DB_LOAD_H_

class DBDispatcher;
class _CharacAdvanceAltarDbData;
class _Stage;
class _Slot;
class _BuyItem;
class _Reward;

namespace advancealtar
{

class DB_Load : public DBDispatcher
{
public:
    DB_Load();
    ~DB_Load();
    virtual bool dispatch(int uid, int uniqueId, Stream* stream);
    bool Select(_CharacAdvanceAltarDbData& data);
    bool SendResult(int uid, int uniqueId, _CharacAdvanceAltarDbData& data, bool ok);
    bool parseStage(char* str, _Stage* out, int count);
    bool parseSlot(char* str, _Slot* out, int count);
    bool parseBuyItem(char* str, _BuyItem* out, int count);
    bool parseReward(char* str, _Reward* out, int count);
};

}  // namespace advancealtar

#endif  // DNF_GAME_DB_LOAD_H_