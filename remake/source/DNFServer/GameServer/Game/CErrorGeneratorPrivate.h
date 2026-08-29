// private_store::CErrorGeneratorPrivate —— ORIG W 声明头。
// ORIG：C1/C2 W 0x085d0fa4（调 std::map<CUser*,int> C1）、D1/D2 W 0x085d0f7c（调 map D1），
// 唯一成员即该 map（sizeof 0x18，与 g_ErrorGenerator BSS size 0x18 实证一致）。
// 定义在 CPrivateStoreMgr.cpp（private_store 域独占）。
#ifndef DNF_GAME_CERRORGENERATORPRIVATE_H_
#define DNF_GAME_CERRORGENERATORPRIVATE_H_
#include <cstddef>
#include <map>

class CUser;

namespace private_store
{

class CErrorGeneratorPrivate
{
public:
    CErrorGeneratorPrivate();
    ~CErrorGeneratorPrivate();

private:
    std::map<CUser*, int> m_errors;   // +0x00（sizeof 0x18；ORIG C1 调 map C1 / D1 调 map D1 实证）
};

// ORIG B _ZN13private_store16g_ErrorGeneratorE @0x09472de8（BSS size 0x18）
extern CErrorGeneratorPrivate g_ErrorGenerator;

}  // namespace private_store

#endif  // DNF_GAME_CERRORGENERATORPRIVATE_H_
