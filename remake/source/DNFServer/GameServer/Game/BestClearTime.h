#ifndef GAME_BESTCLEARTIME_H_
#define GAME_BESTCLEARTIME_H_

// df_game_r BestClearTime（角色扩展数据 18 号槽，2026-08-19）。
// 布局依据 ORIG objdump（BestClearTime.cpp TU）：
//   ctor 0x80c851c：map 构造于 this+0x08，newRecord 清零于 this+0x20；
//   setNewRecord 0x80c8b44：写 this+0x20；checkNewRecord 0x80c8b5c：读 this+0x20；
//   setBestClearTime 0x80c8928：find/insert 均基于 this+0x08 的 map。
// SIG_BEST_CLEAR_TIME_DIFFICULTY：20 字节（5 x int，难度 0..4），ctor 全零
//   （ORIG 0x80c8c72 memset(this, 0, 0x14)）。
// 依赖：loadData(0x80c8776)/_saveData(0x80c8616)/getData(0x80c883c) 属后续批次，
//   loadData/_saveData 暂留纯虚；getData 暂用基类默认（不覆盖）。

#include <cstring>
#include <map>

#include "CBoosterGage.h"  // charac_expand::CData（pack(1) 基类）

class CUser;

struct SIG_BEST_CLEAR_TIME_DIFFICULTY
{
    int m_time[5];  // 难度 0..4 的最佳通关时间

    SIG_BEST_CLEAR_TIME_DIFFICULTY()
    {
        memset(this, 0, sizeof(*this));
    }
};

class BestClearTime : public charac_expand::CData
{
public:
    BestClearTime();
    virtual ~BestClearTime();

    // loadData/_saveData：ORIG 0x80c8776 / 0x80c8616，后续批次实现。
    virtual bool loadData(CUser* pUser, char* pData) = 0;
    virtual bool _saveData(CUser* pUser) = 0;
    virtual void _reset();

    // ORIG 0x80c8928：记录某副本某难度的最佳通关时间。
    void setBestClearTime(int dungeonIdx, int difficulty, int time);
    // ORIG 0x80c8ac0：查询；无记录或难度越界返回 0。
    int getBestClearTime(int dungeonIdx, int difficulty);
    // ORIG 0x80c8b44 / 0x80c8b5c
    void setNewRecord(bool bNew);
    bool checkNewRecord();

private:
    std::map<int, SIG_BEST_CLEAR_TIME_DIFFICULTY> m_map;  // +0x08（5 字节基类尾对齐到 4）
    bool m_bNewRecord;                                    // map 之后（ORIG +0x20）
};

#endif  // GAME_BESTCLEARTIME_H_
