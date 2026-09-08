// ============================================================================
// STCharacterLayoutC3.cpp —— STCharacterScript 生命周期（C1/D1）真实定义。
//
// ORIG nm（df_game_r）证据：
//   _ZN17STCharacterScriptC1Ev / C2Ev  W 0x08368348
//   _ZN17STCharacterScriptD1Ev / D2Ev  W 0x08376362
//   _ZN17STCharacterScript5ClearEv     W 0x08367a98（定义在 ScriptDepsA.cpp）
//
// 无独立函数报告（docs/class_func_reports 下无 STCharacterScript 目录），
// 本文件按门禁在完整读取同类报告（CCharacter/CCharacter.md、~CCharacter.md、
// set_data.md）与调用方（ScriptDepsA.cpp、CtorProducers_1.cpp::CCharacList::init）
// 之后，以 objdump 全文转写并逐段核对，特此标记。
//
// C1 0x08368348 转写（构造顺序 = 成员声明顺序，随后调 Clear()）：
//   +0x004  m_growtypeName[6]        6×SsC1Ev（步长4）
//   +0x024  m_initialStatus+m_growStatus[6]
//                                    7×STRDCharacterStatusC1Ev（步长0x57；
//                                    该 C1 0x0836773e 即调用自身 Clear 清零）
//   +0x288  m_skillList[7]           7×vector<pair<int,int>>C1Ev（步长0xc）
//   +0x2dc  m_etcStats[7]            7×CharacterEtcStatsC1Ev（步长0x1c；
//                                    vector 构造后调 clear）
//   +0x3a0  m_awakeningName[6][2]    12×SsC1Ev
//   +0x3d0  m_secondGrow[7]          7×STSecondGrowTypeDataC1Ev（步长0x100）
//   +0xad4..+0xb34                    25×SsC1Ev（motion/throw 域）
//   +0xb38/+0xb44/+0xb50              3×vector<string>C1Ev
//   +0xb5c  m_actionSet              ActiveActionFileSetC1Ev（0x08367eee 隐式逐成员）
//   +0xb9c..+0xba8                    4×SsC1Ev（lastAction/procAction/jump/dash info）
//   +0xbac/+0xbb8                     2×vector<string>C1Ev
//   +0xbc4  m_weaponWav              vector<WeaponInfo>C1Ev
//   +0xbd0/+0xbdc                     SsC1Ev ×2（scriptPath/scriptName）
//   +0xbe0  m_combo                  _ComboInfoC1Ev（双 map + vector 构造）
//   尾部                              call STCharacterScript::Clear()
//   （异常路径为逆序析构已构造成员后 _Unwind_Resume；与隐式构造语义一致）
//
// D1 0x08376362 转写：自 +0xbe0 _ComboInfoD1Ev 起，按声明逆序逐成员析构
//   （strings/vectors/ActiveActionFileSetD1/STSecondGrowTypeDataD1/
//    CharacterEtcStatsD1/vector<pair>D1），无其他语句。
//
// 语义等价性说明：ORIG 子对象 C1 中对 POD 的清零（STRDCharacterStatus::Clear、
// CharacterEtcStats::clear）随后被 Clear() 全量覆盖（Clear 触达全部 7 状态块、
// 7 二转块、全部 motion 字符串/向量）；Clear 不触的 +0xb5c actionSet 与
// +0xb38 ghostMotion 在两侧均为「默认构造的空容器」。故
// 「默认构造成员 + Clear()」与 ORIG 可观测状态逐字节一致。
//
// sizeof(STCharacterScript) == 0xc1c 由头文件 static_assert 锁定。
// ============================================================================

#include "STCharacterScript.h"

// [b28d-51] 判词 N（ORIG 0x083678a0，N82/N91 族）：STSecondGrowTypeData::C1 为
// STCharacterScript.h L127 头内 inline（clear() 体），7× 数组构造链槽/帧差；
// 语义/构造集齐，byte 层封顶。
// [b28d-51] 判词 N（ORIG 0x08368348，N82/N91 族）：vector/string 成员构造 +
// EH cleanup 残段跨版本差；语义/构造集齐，byte 层封顶。
STCharacterScript::STCharacterScript()
{
    Clear();
}

// [b28d-51] 判词 N（ORIG 0x08376362，N82/N91 族）：成员析构链 + EH 尾块序
// （N104）跨版本差；语义/析构集齐，byte 层封顶。
STCharacterScript::~STCharacterScript()
{
}
