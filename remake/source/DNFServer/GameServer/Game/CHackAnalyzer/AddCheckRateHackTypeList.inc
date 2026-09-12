// b28d-solo-CHackAnalyzer: AE（299/299，无实质残差；ORIG 0x080f8550）。
// [P8S1 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 NEAR 系 objdump 无 -r 的假残差
// [R6b-Main · 2026-09-12] 重载目标对齐：ORIG 42 处 call 目标 = `_ZNSt6vectorIiSaIiEE9push_backEOi`
//   （**int&& 右值重载**）；旧译文用 42 个具名局部 `v1..v42` + `push_back(vN)`（左值）⇒ 选中
//   `push_backERKi`（const int&），调用目标与 ORIG 不同（NEAR 的主因）。
//   正解 = **prvalue 形**（字面量直接入参）：GCC 为每个调用点物化一个临时槽（`movl $K,-0xNN(%ebp)`
//   + `lea -0xNN(%ebp),%eax` + `call push_backEOi`），恰好复现 ORIG 的 42 个独立槽
//   -0xb0..-0xc 与前 18/后 24 的分组顺序。
//   注：构建标准为 `-std=gnu++0x`（flags.make CXX_FLAGS 实证）⇒ 右值重载**可复现**
//   （推翻旧「C++98 不可复现（N130）」结论；同 TU 先例 ScanChampionFeatures R6a20 同法）。
void CHackAnalyzer::AddCheckRateHackTypeList()
{
    // ORIG 0x080f8550：前 18 项压入 m_checkRateHackTypes(+0x7a24)，
    // 后 24 项压入 m_vec7a30(+0x7a30)；均为 prvalue 临时逐值 push_back(int&&)。
    m_checkRateHackTypes.push_back(0x84);
    m_checkRateHackTypes.push_back(0xdc);
    m_checkRateHackTypes.push_back(0x138);
    m_checkRateHackTypes.push_back(0x139);
    m_checkRateHackTypes.push_back(0x86);
    m_checkRateHackTypes.push_back(0xde);
    m_checkRateHackTypes.push_back(0x1a4);
    m_checkRateHackTypes.push_back(0x1a5);
    m_checkRateHackTypes.push_back(0x1a7);
    m_checkRateHackTypes.push_back(0xdf);
    m_checkRateHackTypes.push_back(0xe0);
    m_checkRateHackTypes.push_back(0xe1);
    m_checkRateHackTypes.push_back(0x1a4);
    m_checkRateHackTypes.push_back(0x1a5);
    m_checkRateHackTypes.push_back(0x1a7);
    m_checkRateHackTypes.push_back(0xdf);
    m_checkRateHackTypes.push_back(0xe0);
    m_checkRateHackTypes.push_back(0xe1);
    m_vec7a30.push_back(0x84);
    m_vec7a30.push_back(0xca);
    m_vec7a30.push_back(0xcd);
    m_vec7a30.push_back(0xce);
    m_vec7a30.push_back(0xd0);
    m_vec7a30.push_back(0xd1);
    m_vec7a30.push_back(0xd7);
    m_vec7a30.push_back(0xd8);
    m_vec7a30.push_back(0xd9);
    m_vec7a30.push_back(0xda);
    m_vec7a30.push_back(0xdb);
    m_vec7a30.push_back(0xdc);
    m_vec7a30.push_back(0x138);
    m_vec7a30.push_back(0x139);
    m_vec7a30.push_back(0x193);
    m_vec7a30.push_back(0x19e);
    m_vec7a30.push_back(0x86);
    m_vec7a30.push_back(0xde);
    m_vec7a30.push_back(0x1a4);
    m_vec7a30.push_back(0x1a5);
    m_vec7a30.push_back(0x1a7);
    m_vec7a30.push_back(0xdf);
    m_vec7a30.push_back(0xe0);
    m_vec7a30.push_back(0xe1);
}
