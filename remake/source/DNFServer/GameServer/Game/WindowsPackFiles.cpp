// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 WindowsPackFiles/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// WindowsPackFiles.cpp —— WindowsExe/DataFile/PackFile 校验族实现
//
// 语义来源：docs/class_func_reports/{CObfuscator,WindowsDataFile,
//   WindowsDataFileFamily,WindowsExeFile,WindowsExeFileFamily,
//   WindowsCodeProtectorServer}/ 各 md 的 ## 反编译 C；CCompressedBlob 与
//   CompareWindowsExeConstantSectionsForSort/LoadClientFile 由 objdump 直接转写。
// CBitManager 的实现不在本文件：ORIG 属独立编译单元，已迁至 Game/CBitManager.cpp
// （见 CObfuscator::CObfuscator 上方注释的取证与实测）。本 TU 经 WindowsPackFiles.h
// 只取其声明。
// C++98 / 32 位 / O0；字段均为具名成员，无偏移宏。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11CObfuscatorC2Ev` / `_GLOBAL__I__Z14LoadClientFilePmPPvPKc`
// （ORIG 0x8585f03 / 0x8575e3c，两者同属本 TU 的两个 ORIG CU 键名，取首强符号者）。
#include <iostream>
// [b28d-64 判词汇总 · 泳道64] 本 TU 10 件（明细 /tmp/b28d_lane_64_result.tsv）：
//   LoadClientFile 66/66 AE；IsDifferentExeVersionRoughly 66/66 AE；
//   ~WindowsDataFileFamily 77/77 AE；~WindowsExeFileFamily 94/94 AE；
//   ~WindowsExeFile 151/151 AE；GenerateRange 106/109 N封顶（P198 边界）；
//   WindowsDataFileFamily::GenerateChecksum 93/106 N82；
//   WindowsExeFileFamily::GenerateChecksum 97/111 N82；
//   LoadWindowsFile 163/169 N封顶（P167 CLEAR 21/21）；
//   CodeProtectorServer::GenerateChecksum 152/162 N封顶（P167 CLEAR 10/10）；
//   MakeCommonWindowsExeConstantSections 164/179 N91（P167 EXTRA Interval D1×4）；
//   CompleteLoadingWindowsExeFiles 279/281 N91（EXTRA Interval D1×2）；
//   ReadPeHeader 551/532 N82（P167 MISSING 迭代器遍历×2 + IntervalSet 段，我方索引形）。
//   注：本清单含 13 行（TSV 10 件 + 3 件 D58 已判同 TU 归因）。
// [b28d-solo-WindowsPackFiles 判词汇总 · 单文件泳道] 8 件
//   （明细 /tmp/b28d_solo_WindowsPackFiles_result.tsv）：
//   AE/IDENTICAL 5：WindowsExeFileFamily::GenerateRange（P211 档③ 成员 unsigned long
//     + P198① 两句式预清 + 4 形槽序探针 genD）、WindowsDataFileFamily::GenerateChecksum
//     （迭代器形 + 正向 if + P247 8 字节视图）、WindowsExeFileFamily::GenerateChecksum
//     （同型）、LoadWindowsFile（exe/data 域序镜像翻正 + 单 return false 共享尾 +
//     嵌套守卫拆 && 物化）、WindowsCodeProtectorServer::GenerateChecksum
//     （P198① 预清 + if(m_loaded) 单出口包裹 + 扁平 else-if + (int)size() setle）。
//   N 封顶 3：MakeCommonWindowsExeConstantSections（N95/N82，p167 CLEAR 20/20）、
//     CompleteLoadingWindowsExeFiles（N95/N89，p167 CLEAR 37/37）、ReadPeHeader
//     （P165 双份展开/N82/N89，p167 差集为等价改写形态）。
//   [b29-R28 复核] 三件 p167 差集 = **P93 同址别名伪差**（本仓 nm 实证）：
//     ORIG 同一地址多符号 —— IntervalSet::C1Ev=C2Ev=0x8575e9c、D1Ev=D2Ev=0x8575eb0；
//     Interval::C1Ev=C2Ev=0x8575e58、C1Ell=C2Ell=0x8575e5e；
//     vector<IntervalSet*>::D1Ev=D2Ev=0x8576832。objdump 在同址多符号时选名不同
//     （C1 vs C2、D1 vs D2）⇒ MISSING/EXTRA 成对抵消，非语义缺口。
//     残余 = N95 全帧槽号浅移（shift.py 三件均 MIXED：ORIG 96/256/156 vs
//     我方 104/272/156）+ 4.4 聚合优先低槽分区（N82附）。语义等价。
//   .h 改动 2（均已 R2 广播 + 消费点核对）：WindowsPackFiles.h m_curSectionIndex
//     int→unsigned long（P211 档③）；CodeHackChecksum.h CodeHackCheckRangeEx
//     匿名 union {m_start104/m_size108; CodeHackCheckRange m_range;}（P247）。
//   P0：本文件无 register asm 绑定（0 命中，无需清绑）。
#include "WindowsPackFiles.h"
#include "AdvanceAltarZip.h"
#include "MySQL.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

extern "C" void* memset(void*, int, std::size_t);

// ---------------------------------------------------------------------------
#include "ShaSupport.h"          // StrCpyMaxPath
// LoadClientFile —— _Z14LoadClientFilePmPPvPKc T @0x08572f3a（0xf2）
// fopen("rb") → fseek END 取长 → new → fread(1) 成功即 1。
// ---------------------------------------------------------------------------
// [N 类标注] 0x08572f3a：66/65（差 1），P167 调用点 9/9 双向差集 0 ⇒ 纯指令形差：
//  ① ORIG fread 返回值为 `cmp $0x1,%eax; setne %al; test %al,%al; je`（比较结果
//     经 setne 物化后再分支，且结果不落槽），我方按具名 n 变量存槽后 `cmpl;je`
//     （范式库 setcc-fold 负例的镜像变体：4.4 恒把该形折回存槽+整数比较）；
//  ② ORIG new[] 返回值以 %edx 直载跨两句使用（无缓存槽），4.4 走 -0x10 槽存取；
//  ③ fopen==0 的失败出口 ORIG 为 `je 远端 +0xeb` 单跳，4.4 生成 `jne 跳过 +
//     mov $0 + jmp` 三句岛。三者皆编译器版本 RA/分支物化策略，源码侧不可控。
#include "WindowsPackFiles/LoadClientFile.inc"
}

// ---------------------------------------------------------------------------
// CompareWindowsExeConstantSectionsForSort —— t @0x08572eef（qsort 比较器）
// 比较 *(unsigned long*)*lhs 与 *(unsigned long*)*rhs。
// ---------------------------------------------------------------------------
static int CompareWindowsExeConstantSectionsForSort(const void* lhs, const void* rhs)
{
    WindowsExeConstantSection* const* a =
        static_cast<WindowsExeConstantSection* const*>(lhs);
    WindowsExeConstantSection* const* b =
        static_cast<WindowsExeConstantSection* const*>(rhs);
    // ORIG 0x08572eef 逐条：两次比较**各自重读** `(*p)->start`（+0x12..+0x20 与
    // +0x2b..+0x39 两组各 6 insns 的 mov/mov/mov/cmp），⇒ 无 sa/sb 缓存量；
    // 尾部 `cmp; jne +0x44; mov $0; jmp; +0x44: mov $1` 即三目常量岛形。
    if ((*a)->start < (*b)->start)
    {
        return -1;
    }
    // 三目形被 4.4 折成 `setne+movzbl`（−3 insns，范式库 setcc-fold 负例的
    // 比较结果变体）；改回显式 if + 常量返回即复现 ORIG `jne L; mov $0; jmp;
    // L: mov $1` 双常量岛。
    if ((*a)->start == (*b)->start)
    {
        return 0;
    }
    return 1;
}

// ===========================================================================
// CObfuscator —— 位重排混淆（7bit 有效位流水）
// ===========================================================================
#include "WindowsPackFiles/CObfuscator.inc"
}

#include "WindowsPackFiles/dtor_CObfuscator.inc"
}

#include "WindowsPackFiles/ObfuscatePacket.inc"
}

#include "WindowsPackFiles/DeobfuscatePacket.inc"
}

#include "WindowsPackFiles/Obfuscate1.inc"
}

#include "WindowsPackFiles/Deobfuscate1.inc"
}

#include "WindowsPackFiles/Obfuscate2.inc"
}

#include "WindowsPackFiles/Deobfuscate2.inc"
}



// ===========================================================================
// WindowsDataFile —— 0x08574e3e..
// ===========================================================================
#include "WindowsPackFiles/WindowsDataFile.inc"
}                                                            // 0x08574e3e

#include "WindowsPackFiles/dtor_WindowsDataFile.inc"
}

#include "WindowsPackFiles/LoadWindowsDataFile.inc"
}

#include "WindowsPackFiles/GenerateChecksum.inc"
}

// ===========================================================================
// WindowsExeFile —— 0x085736be..
// ===========================================================================
// [b28c-lane17] ORIG 0x085736be：先构造 m_sections(+8)/m_commonSections(+0x14)，
// 再体赋值 m_size=0/m_image=0（标量不进初始化列表）。
#include "WindowsPackFiles/WindowsExeFile.inc"
}

#include "WindowsPackFiles/dtor_WindowsExeFile.inc"
}

// ReadPeHeader —— T @0x085738f6（0x7de）：解析 PE 头产出常量区段表。
// ORIG 真形修正（本批 objdump+Ghidra C 双段核读）：
// ① 分支极性与直觉相反——`chars>=0 && 0x40000000` 门内，**(sh[9]&0x20000000)==0**
//    走 dir/import 解剖大分支，可执行分支只减入口头 10 字节（0x08573a1c else 体）；
// ② 两分支各持独立 IntervalSet+push 迭代器循环（dc/c4 双对象，dtor 各 1）；
// ③ hint name 长度奇偶用 (char)(s+1)+(char)nameOff，且 nm!=0 外层判断；
// ④ dll 名指针 = img+dllRva（**无** vaDelta），奇偶第二项 = thunkRva+vaDelta
//    （local_38）——前批两处误读已修正。
#include "WindowsPackFiles/ReadPeHeader.inc"
}

#include "WindowsPackFiles/Sort.inc"
}

#include "WindowsPackFiles/MakeCommonWindowsExeConstantSections.inc"
}

#include "WindowsPackFiles/LoadWindowsExeFile.inc"
}

#include "WindowsPackFiles/GenerateChecksum_1.inc"
}

// ===========================================================================
// WindowsDataFileFamily —— 0x08574f16..
// ===========================================================================
#include "WindowsPackFiles/WindowsDataFileFamily.inc"
}                                                            // 0x08574f16

#include "WindowsPackFiles/dtor_WindowsDataFileFamily.inc"
}

#include "WindowsPackFiles/IsDifferentFileData.inc"
}

#include "WindowsPackFiles/SetCheckingOption.inc"
}

#include "WindowsPackFiles/LoadWindowsDataFile_1.inc"
}

#include "WindowsPackFiles/GenerateRange.inc"
}

#include "WindowsPackFiles/GenerateChecksum_2.inc"
}

#include "WindowsPackFiles/IsScanningAddressReset.inc"
}

// ===========================================================================
// WindowsExeFileFamily —— 0x0857446a..
// ===========================================================================
#include "WindowsPackFiles/WindowsExeFileFamily.inc"
}

#include "WindowsPackFiles/dtor_WindowsExeFileFamily.inc"
}

#include "WindowsPackFiles/SetCheckingOption_1.inc"
}

#include "WindowsPackFiles/LoadWindowsExeFile_1.inc"
}

#include "WindowsPackFiles/IsDifferentFileData_1.inc"
}

// swap_value<IntervalSet*> —— ORIG W 0x0857688f 全局模板（OnlyOrigBatch12.cpp
// force 发射同定义；本 TU 隐式实例化再发同名 W，链接合并）。
template <class T>
#include "WindowsPackFiles/swap_value.inc"
}

#include "WindowsPackFiles/CompleteLoadingWindowsExeFiles.inc"
}

#include "WindowsPackFiles/GenerateRange_1.inc"
}

#include "WindowsPackFiles/GenerateChecksum_3.inc"
}

// ===========================================================================
// WindowsCodeProtectorServer —— 0x08575462..
// ===========================================================================
#include "WindowsPackFiles/WindowsCodeProtectorServer.inc"
}                                                            // （m_exeMin..m_dataMax 由 SetExeCheckingOption 设置，构造不初始化）

#include "WindowsPackFiles/dtor_WindowsCodeProtectorServer.inc"
}

#include "WindowsPackFiles/Initialize.inc"
}

#include "WindowsPackFiles/Destroy.inc"
}

#include "WindowsPackFiles/FindExeFamilyFile.inc"
}

WindowsDataFileFamily*
#include "WindowsPackFiles/FindDataFamilyFile.inc"
}

#include "WindowsPackFiles/SetExeCheckingOption.inc"
}

#include "WindowsPackFiles/BeginLoading.inc"
}

#include "WindowsPackFiles/LoadWindowsFile.inc"
}

#include "WindowsPackFiles/CompleteLoading.inc"
}

#include "WindowsPackFiles/IsDifferentExeVersionRoughly.inc"
}

#include "WindowsPackFiles/GenerateChecksum_4.inc"
}

// ===========================================================================
// CCompressedBlob —— objdump 0x0844de52..0x0844e0aa 逐指令转写
// [b28c-lane24] throw()（P216 static-local guard 边界）：ORIG put_compressed_blob_data
// @0x083fbd06 无 landing pad；W 体尺寸不变（-fno-enforce-eh-specs 无 unexpected guard）。
// [判据㉓ 符号绑定] ORIG 8 符号全 W（头内 inline 定义由使用点 TU 发射，DB_Login.cpp
//   get_compressed_blob_data / W8LT_DBTask.cpp put_compressed_blob_data 为使用点）；
//   我方唯一权威定义落本 TU ⇒ 定义处加 __attribute__((weak)) 对齐绑定（不改头，
//   mangled/尺寸/体零变化：nm 双侧 0xea/0x29/0x56/0xa5/0x18/0x30 逐值相等）。
// ===========================================================================
__attribute__((weak)) CCompressedBlob::CCompressedBlob() throw() : m_buf(0), m_size(0)
{
}                                                            // 0x0844de52

__attribute__((weak)) CCompressedBlob::~CCompressedBlob() throw()
{                                                            // 0x0844de6a
    if (m_buf != 0)
    {
        delete[] m_buf;
        m_buf = 0;
    }
}

__attribute__((weak)) void CCompressedBlob::resize(int needSize) // 0x0844e02a
{
    if (m_size < needSize)
    {
        if (m_buf != 0)
        {
            // ORIG 0x844e040 二次 test 系 4.1 jump-threading 产物（test1 失败直达
            // new 段），4.4 无线程化 + 尾 jmp/nop 岛，跨版本豁免。
            delete[] m_buf;
            m_buf = 0;
        }
        m_buf = new char[needSize];
        m_size = needSize;
    }
}

__attribute__((weak)) void CCompressedBlob::reset()          // 0x0844e080
{
    memset(m_buf, 0, m_size);
}

__attribute__((weak)) bool CCompressedBlob::compress(MySQL* db, int fieldIdx, char* src,
                                                     int srcLen,
                                                     char** outStr) // 0x0844de9a
{
    // ORIG 0x0844de9a：单一 unsigned int 槽（-0xc）既作 resize 入参又作
    // compress_zip 的 unsigned int& 出参（blob_to_str 读回同槽）——引入独立
    // outLen 变量会多占一槽并差 store/load。除法带符号展开 magic 0x66666667
    // 配 `sar $1` = **/5**（前批误读为 /10：/10 与 /5 共用 magic、以移位区分，
    // ORIG sar1 vs ours sar2 即常数不同之证；改 /5 后 4.4 展开与 ORIG 同形）。
    unsigned int needSize = srcLen + srcLen / 5 + 12;
    resize(needSize);
    reset();
    if (!compress_zip(m_buf, needSize, src, static_cast<unsigned int>(srcLen)))
    {
        return false;
    }
    *outStr = db->blob_to_str(fieldIdx, m_buf, static_cast<int>(needSize));
    return true;
}

__attribute__((weak)) bool CCompressedBlob::uncompress(MySQL* db, int fieldIdx, char* dst,
                                                       int dstLen,
                                                       int align) // 0x0844df40
{
    // ORIG 0x0844df40：needSize 单槽（-0x10）复用 uint& 出参；binaryLen 独立
    // 槽 -0xc；尾部 align 检查 `align!=0 && needSize%align!=0 && needSize!=0`。
    unsigned int needSize = dstLen + dstLen / 5 + 12;
    resize(needSize);
    reset();

    int binaryLen = db->get_binary_length(fieldIdx);
    if (!db->get_binary(fieldIdx, m_buf, needSize))
    {
        return false;
    }
    if (!uncompress_zip(dst, needSize, m_buf,
                        static_cast<unsigned int>(binaryLen)))
    {
        return false;
    }
    if (align != 0 && (needSize % static_cast<unsigned int>(align)) != 0 &&
        needSize != 0)
    {
        return false;
    }
    return true;
}
