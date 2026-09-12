// StrIosInst.cpp —— std::basic_string<char> 显式实例化 TU
// （libstdc++ char string 族归档成员的 -march 差异收敛；SemB04 / slice s2_04 实证）
//
// 背景：ORIG 侧 char string 成员来自 Red Hat 32 位默认旗标（-march=i486 -mtune=generic）
//   构建的 libstdc++ 归档成员（`jbe`/`sub $N,%esp`，无 CMOV）；我们侧链接器从
//   c6root libstdc++.a (4.4.4/32) 拉入的归档成员是 -march=i686 版（`cmovbe`、
//   `lea -N(%esp),%esp`、寄存器序不同）⇒ 链接口径多件 stl DIFF。
//   本 TU 显式实例化同符号集，链接器优先采用本 TU 定义（同 WStringInst.cpp 先例）。
//
// 编译路由（CMake SRC_OPTS，须与 SOURCES 同时挂 —— P230③）：
//   -O2 -std=gnu++0x -march=i486 -mtune=generic
//   （-O2：libstdc++ 归档成员为 -O2 构建；-march=i486：复现 ORIG 无 CMOV 形）
//
// ── 覆盖域（三方比较实测：/tmp/lane_diff/SemB04/overlap2.py，261/164 符号）─────────
//   仅 basic_string<char>：A 收益(ORIG!=NEW 且 TU==ORIG) **124**；B 危险(ORIG==NEW 但
//     TU!=ORIG) **0**；C 劣化(TU 与 ORIG 差异 > NEW 与 ORIG) **0** ⇒ **净收益零风险**。
//   若同时加 ostream/istream/iostream（见文末注释的三行）：A 收益升至 192，但
//     **12 个符号劣化**（`_ZNSo5tellpEv` diff 16→64、`_ZNSoC1Ev` 2→14、
//     `_ZNSo5writeEPKci` 31→46、`_ZNSo5seekpExSt12_Ios_Seekdir` 35→52、
//     `_ZNSdC1Ev` 16→32、`_ZNSi5tellgEv` 16→64、`_ZNSiC1Ev` 2→14、
//     `_ZNSi5seekgExSt12_Ios_Seekdir` 35→52、`_ZNSoC1EPSt15basic_streambuf…` 4→14、
//     `_ZNSo5seekpESt4fposI11__mbstate_tE` 34→57）**外加 2 个 thunk
//     `_ZThn8_NSdD1Ev`/`_ZThn8_NSdD0Ev` 由 identical(0) 变差(5)** —— 违反「劣化即回滚」，
//     故本 TU **默认不实例化 ostream/istream/iostream**。若 Main 愿以 2 个 thunk 回归
//     换取 +66 收益，取消文末三行注释即可（数据见上）。
//
// ── 未覆盖（源码层无解，仍需归档） ────────────────────────────────────────────
//   `_ZNSo9_M_insertI{PKv,l,m,y,x,e}EERSoT_` 族：basic_ostream 的**私有成员模板**
//   实例，4.4 头下 `template class basic_ostream<char>` 与使用点（`os << p`）均不发射
//   （444/446/447 三后端实测 0 条）。
//   `_GLOBAL__I_wlocale_inst.cc`：locale 归档成员的 TU 初始化器（序言 guard-check 调度差）。
#include <string>

template class std::basic_string<char, std::char_traits<char>, std::allocator<char> >;

// 方案 A（收益 +192 / 12 项劣化）：默认关闭
// #include <iostream>
// #include <istream>
// #include <ostream>
// template class std::basic_ostream<char, std::char_traits<char> >;
// template class std::basic_istream<char, std::char_traits<char> >;
// template class std::basic_iostream<char, std::char_traits<char> >;
