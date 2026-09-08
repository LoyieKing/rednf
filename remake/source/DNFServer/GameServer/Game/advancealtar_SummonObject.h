#ifndef DNF_GAME_ADVANCE_ALTAR_SUMMON_OBJECT_H_
#define DNF_GAME_ADVANCE_ALTAR_SUMMON_OBJECT_H_

// ============================================================================
// advancealtar::_SummonObject 叶子头（b28d-52 从 CDataManager.h 迁出）。
// 依赖面：CDataManager.h（_Action::m_pSummon / map<int,_SummonObject>）与
// advancealtar_ProcStage.h（_SummonObjectMs::m_summon 真实子对象）双向需要，
// 而后者先于 CDataManager.h 展开 ⇒ 放叶子头消除反向依赖。
// 仅含标量成员；ctor 定义在 advancealtar_StageTimeline.cpp（ORIG W 0x08134650）。
// ============================================================================

namespace advancealtar
{

// _SummonObject（ORIG C1 0x08134650 W：memset(this,0,0x24)，sizeof=0x24；
//   字段写入点见 importSummonObject 0x0889a21e 报告）。
struct _SummonObject
{
    _SummonObject();             // ORIG 0x08134650 W（memset 0x24）
    int m_id;                    // +0x00（亦作 m_summonMap 的 key）
    unsigned char m_isEnemy;     // +0x04（"enemy"=1 / "friend"=0）
    int m_type;                  // +0x08（"normal"=0 / "named"=1 / "boss"=2）
    int m_field0c;               // +0x0c
    unsigned short m_field10;    // +0x10（ScanInt 低 16 位存储）
    int m_field14;               // +0x14
    int m_field18;               // +0x18
    int m_field1c;               // +0x1c
    int m_field20;               // +0x20
};                               // sizeof 0x24

}  // namespace advancealtar

#endif  // DNF_GAME_ADVANCE_ALTAR_SUMMON_OBJECT_H_
