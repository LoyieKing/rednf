// df_game_r 还原 —— CPrePayInfoMgr（LinkE1AB 批，E1-remaining B 组）。
// 符号：
//   CPrePayInfoMgr::CPrePayInfoMgr()        W 0x08231ede（map 构造 + clear）
//   CPrePayInfoMgr::InsertPrePayInfo        W 0x084ed0bc（m_map.insert(make_pair(...))）
//   G_CPrePayInfoMgr()                      W 0x08231f20（GlobalInstance<CPrePayInfoMgr>::inst_ptr）

#include "CPrePayInfoMgr.h"

#include "CEnvironment.h"   // GlobalInstance<T>（全局命名空间单例模板，CPrePayInfoMgr.h 同 TU 集）

CPrePayInfoMgr::CPrePayInfoMgr()
{
    m_map.clear();
}

void CPrePayInfoMgr::InsertPrePayInfo(int characNo, PrePayInfo info)
{
    m_map.insert(std::make_pair(characNo, info));
}

void CPrePayInfoMgr::DeletePrePayInfo(int idx)  // ORIG W 0x084ed10e：m_map.erase(idx)
{
    m_map.erase(idx);
}

PrePayInfo* CPrePayInfoMgr::FindPrePayInfo(int idx)  // ORIG T 0x082151b8：find 后命中返回 &it->second，否则 NULL
{
    // P28：ORIG 先默认构造迭代器（-0x10 槽）再 find 到临时（-0x1c）拷贝回来
    // （帧 0x38）；一行直构被 4.4 copy-elision 折叠（帧 0x28）。
    std::map<int, PrePayInfo>::iterator it;
    it = m_map.find(idx);
    if (it != m_map.end())
        return &it->second;
    return 0;
}

CPrePayInfoMgr* G_CPrePayInfoMgr()
{
    return GlobalInstance<CPrePayInfoMgr>::inst_ptr();
}
