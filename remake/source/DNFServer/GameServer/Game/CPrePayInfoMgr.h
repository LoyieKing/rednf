#ifndef GAME_CPREPAYINFOMGR_H_
#define GAME_CPREPAYINFOMGR_H_

#include <map>

// df_game_r 还原 —— CPrePayInfoMgr / PrePayInfo / G_CPrePayInfoMgr（InterWave1 批）。
// PrePayInfo 按值 0x28（10×int，W3 批证据）；m_map 由 ctor 0x08231ede 反汇编确认
// （map<int,PrePayInfo> 构造 + clear）。

class CPrePayInfoMgr;

struct PrePayInfo
{
    char m_data[0x28];    // ORIG 按值 0x28 视图（字段化待属主批次）
};

class CPrePayInfoMgr
{
public:
    CPrePayInfoMgr();                                  // ORIG W 0x08231ede
    void InsertPrePayInfo(int characNo, PrePayInfo info);   // ORIG W 0x084ed0bc
    PrePayInfo* FindPrePayInfo(int idx);               // ORIG T 0x082151b8：find 命中返回 &second，否则 NULL
    void DeletePrePayInfo(int idx);                    // ORIG W 0x084ed10e

private:
    std::map<int, PrePayInfo> m_map;   // ORIG ctor 0x08231ede：map 构造 + clear
};

CPrePayInfoMgr* G_CPrePayInfoMgr();   // ORIG W 0x08231f20

#endif  // GAME_CPREPAYINFOMGR_H_
