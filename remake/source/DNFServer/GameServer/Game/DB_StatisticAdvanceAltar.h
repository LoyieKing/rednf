// advancealtar::DB_StatisticAdvanceAltar —— ORIG W 声明头（仅声明，无实现）。
// ORIG：C1/C2 W 0x0844fc3a；dispatch(int,int,Stream*)（见 advancealtar__DB_StatisticAdvanceAltar 报告）。
#ifndef DNF_GAME_DB_STATISTICADVANCEALTAR_H_
#define DNF_GAME_DB_STATISTICADVANCEALTAR_H_

class DBDispatcher;

namespace advancealtar
{

class DB_StatisticAdvanceAltar : public DBDispatcher
{
public:
    DB_StatisticAdvanceAltar();
    ~DB_StatisticAdvanceAltar();
    virtual bool dispatch(int uid, int uniqueId, Stream* stream);
};

}  // namespace advancealtar

#endif  // DNF_GAME_DB_STATISTICADVANCEALTAR_H_