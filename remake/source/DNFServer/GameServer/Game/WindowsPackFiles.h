#ifndef DNF_GAME_WINDOWSPACKFILES_H_
#define DNF_GAME_WINDOWSPACKFILES_H_

// ============================================================================
// WindowsExe/DataFile/PackFile 校验族（Wave9 唯一声明点）
//
// ORIG df_game_r 证据（docs/class_func_reports/ 对应目录 + objdump）：
//   CBitManager                T 0x0831c09c..0x0831c2fb
//   CObfuscator                T 0x08585888..；D1 0x085858d6
//   WindowsExeConstantSection  new(0x10) 内联对象（ReadPeHeader/MakeCommon 写入）
//   CompareWindowsExeConstantSectionsForSort t @0x08572eef (0x4b)
//   LoadClientFile             T @0x08572f3a (0xf2) _Z14LoadClientFilePmPPvPKc
//   WindowsDataFile            T 0x08574e3e..0x08574ee6
//   WindowsExeFile             T ctor 0x085736be / dtor 0x0857371a /
//                              ReadPeHeader 0x085738f6 / Sort 0x0857415c /
//                              MakeCommon 0x085741c6 / Load 0x085740de /
//                              GenChecksum 0x085743d4
//   WindowsDataFileFamily      C1/C2 @0x08574f16 D1/D2 @0x08574f7c（其余见下）
//   WindowsExeFileFamily       T 0x0857446a..（其余见下）
//   WindowsCodeProtectorServer C1 @0x08575462 D1 @0x08575484（其余见下）
//   CCompressedBlob            W @0x0844de52..0x0844e0aa（objdump 直接转写）
//
// 依赖（外部唯一 owner，本 TU 不重复定义）：
//   IntervalSet / Interval / IsOverlappedInterval / IntersectIntervalSet
//     —— Wave9 Interval 域权威头 Interval.h

class MySQL;
//   CodeHackChecksum(Data/Ex)、CodeHackCheckRangeEx、ZeroMemoryChecksum/
//   CalculateMemoryChecksum —— CodeHackChecksum.h
//   compress_zip/uncompress_zip —— AdvanceAltarZip.h
//   MySQL —— MySQL.h
// ============================================================================
#include "Interval.h"
#include "CodeHackChecksum.h"

#include <vector>

class CBitManager
{
public:
    CBitManager();                                   // T @0x0831c09c（C1/C2 同体）
    ~CBitManager();                                  // T @0x0831c0c8（空析构）

    void SetAll(int value, unsigned char* buf);      // T @0x0831c0ce
    void ReadData(unsigned char* data, int size);    // T @0x0831c116
    unsigned char GetBitFromLSB(int bits);           // T @0x0831c160
    void ShiftRight(int bits, unsigned char* buf);   // T @0x0831c1d6
    void ShiftLeft(int bits, unsigned char* buf);    // T @0x0831c266
    unsigned char GetBit(int idx, int bitLo, int bitHi); // T @0x0831c2fc

public:
    unsigned char* m_data;        // +0x00 外部缓冲指针（ORIG 同 TU 直访 → 保持 public）
    unsigned char* m_buffer;      // +0x04 malloc 缓冲（Obfuscate 操作对象）
    int            m_size;        // +0x08 字节数
    int            m_field0c;     // +0x0c CBitManager 本域未用；CObfuscator 当余位用

private:
    CBitManager(const CBitManager&);            // ORIG nm 无拷贝符号——禁用
    CBitManager& operator=(const CBitManager&);
};

static_assert(sizeof(CBitManager) == 0x10, "CBitManager ABI drift");

class CObfuscator : public CBitManager           // sizeof=16（仅基类）
{
public:
    CObfuscator();                               // T @0x08585888
    ~CObfuscator();                              // T @0x085858d6（空链）

    int ObfuscatePacket(unsigned char* data, int len, int type);   // T @0x085858ea
    int DeobfuscatePacket(unsigned char* data, int len, int type); // T @0x08585938
    int Obfuscate1();                            // T @0x08585986
    int Deobfuscate1();                          // T @0x08585aaa
    int Obfuscate2();                            // T @0x08585be4
    int Deobfuscate2();                          // T @0x08585d2e
};

// ---------------------------------------------------------------------------
// WindowsExeConstantSection —— new(0x10) 裸 POD：
//   ReadPeHeader 分支：{start=intv.start@0, fileOffset=start+vaDelta@4,
//                       size=intv.size@8, characteristics=sec.chars@c}
//   MakeCommon 写入：{range.start@0, src.fileOffset + (range.start-src.start)@4,
//                     range.size@8, src.flag@c}
// ---------------------------------------------------------------------------
struct WindowsExeConstantSection
{
    unsigned long start;         // +0x00 映像 VA 起点
    unsigned long fileOffset;    // +0x04 文件偏移
    unsigned long size;          // +0x08 长度
    unsigned long flag;          // +0x0c 区段特征字或源 fileOffset 视图字段
};

static_assert(sizeof(WindowsExeConstantSection) == 0x10,
              "WindowsExeConstantSection ABI drift");

// CompareWindowsExeConstantSectionsForSort —— ORIG t(static)@0x08572eef，
// 定义 WindowsPackFiles.cpp（文件内 static，链接名 _ZL40...，无外部调用者）。

bool LoadClientFile(unsigned long* outSize, void** outBuf, const char* path);
                                                 // T @0x08572f3a

// ---------------------------------------------------------------------------
// WindowsDataFile —— {ulong size@0; uchar* data@4} 8B
// ---------------------------------------------------------------------------
class WindowsDataFile
{
public:
    WindowsDataFile();                           // T @0x08574e3e
    ~WindowsDataFile();                          // T @0x08574e56

    bool LoadWindowsDataFile(const char* path);  // T @0x08574e80
    void GenerateChecksum(CodeHackChecksum* ck,
                          unsigned long offset, unsigned long len); // T @0x08574eba

    unsigned long   m_size;                      // +0x00（IsDifferent memcmp 长度）
    unsigned char*  m_data;                      // +0x04
};

static_assert(sizeof(WindowsDataFile) == 8, "WindowsDataFile ABI drift");

// ---------------------------------------------------------------------------
// WindowsExeFile —— {ulong size@0; uchar* image@4;
//                    vector<WCS*> sections@8; vector<WCS*> common@0x14} 0x20
// ---------------------------------------------------------------------------
class WindowsExeFile
{
public:
    WindowsExeFile();                            // T @0x085736be
    ~WindowsExeFile();                           // T @0x0857371a

    bool ReadPeHeader();                         // T @0x085738f6
    void Sort();                                 // T @0x0857415c
    bool MakeCommonWindowsExeConstantSections(const IntervalSet& intervals); // T @0x085741c6
    bool LoadWindowsExeFile(const char* path);   // T @0x085740de
    // [b28d-58] 返回类型 void→bool（ORIG 0x085743d4 成功路径尾 `mov $0x1,%eax`、
    // 失败路径 `mov $0x0,%eax`；返回类型不入 mangled，符号/ABI 不变；唯一调用点
    // WindowsPackFiles.cpp:994 忽略返回值）。
    bool GenerateChecksum(CodeHackChecksum* ck, long sectionIdx,
                          unsigned long offset, unsigned long len);  // T @0x085743d4

    unsigned long  m_size;                       // +0x00 文件字节数
    unsigned char* m_image;                      // +0x04 PE 全镜像
    std::vector<WindowsExeConstantSection*> m_sections;       // +0x08
    std::vector<WindowsExeConstantSection*> m_commonSections; // +0x14
};

static_assert(sizeof(WindowsExeFile) == 0x20, "WindowsExeFile ABI drift");

// ---------------------------------------------------------------------------
// WindowsDataFileFamily —— vector<WindowsDataFile*>@0(12B) + name[0x104]@0xc
//   + scanOffset@0x110 + maxSize@0x114 + randMin@0x118 + randMax@0x11c (0x120)
// ---------------------------------------------------------------------------
class WindowsDataFileFamily
{
public:
    WindowsDataFileFamily();                     // T @0x08574f16
    ~WindowsDataFileFamily();                    // T @0x08574f7c

    void SetCheckingOption(unsigned long minOff, unsigned long maxOff); // T @0x0857511e
    bool LoadWindowsDataFile(const char* path, const char* name);       // T @0x0857513c
    bool IsDifferentFileData(WindowsDataFile* f);                      // T @0x08575068（非 const）
    bool GenerateRange(unsigned long* outStart, unsigned long* outLen); // T @0x08575230
    bool GenerateChecksum(CodeHackChecksumEx* ckEx,
                          CodeHackCheckRangeEx* rangeEx);              // T @0x08575302
    bool IsScanningAddressReset();                              // T @0x08575444

    std::vector<WindowsDataFile*> m_files;       // +0x00..0xb
    char                  m_name[0x104];         // +0x0c（StrCpyMaxPath 目标）
    unsigned long         m_scanOffset;          // +0x110 当前扫描偏移
    unsigned long         m_maxSize;             // +0x114 加载到的最大文件长度
    unsigned long         m_randMin;             // +0x118
    unsigned long         m_randMax;             // +0x11c
};

static_assert(sizeof(WindowsDataFileFamily) == 0x120,
              "WindowsDataFileFamily ABI drift");

// ---------------------------------------------------------------------------
// WindowsExeFileFamily : public IntervalSet（基类 12B）
//   exeFiles@0xc、name[0x104]@0x18、curIdx@0x11c、curOff@0x120、
//   randMin@0x124、randMax@0x128 → sizeof 0x130
// ---------------------------------------------------------------------------
class WindowsExeFileFamily : public IntervalSet
{
public:
    WindowsExeFileFamily();                      // T @0x0857446a
    ~WindowsExeFileFamily();                     // T @0x08574500

    void SetCheckingOption(unsigned long minOff, unsigned long maxOff); // T @0x08574620
    bool LoadWindowsExeFile(const char* path, const char* name);        // T @0x0857463e
    bool IsDifferentFileData(WindowsExeFile* f);                  // T @0x08574ace
    bool CompleteLoadingWindowsExeFiles();                              // T @0x08574736
    bool GenerateRange(long* outSecIdx, unsigned long* outStart,
                       unsigned long* outLen);                          // T @0x08574b8e
    bool GenerateChecksum(CodeHackChecksumEx* ckEx,
                          CodeHackCheckRangeEx* rangeEx);             // T @0x08574cf0

    std::vector<WindowsExeFile*>  m_exeFiles;    // +0x0c..0x17
    char                 m_name[0x104];          // +0x18..0x11b
    int                  m_curSectionIndex;      // +0x11c 常量区段轮转游标
    unsigned long        m_curOffset;            // +0x120 当前扫描起始
    unsigned long        m_randMin;              // +0x124
    unsigned long        m_randMax;              // +0x128
};

// ---------------------------------------------------------------------------
// WindowsCodeProtectorServer —— exeFamily*@0 + dataFamilies vec@4(12B) +
//   cfgLong@0x10 retryCount@0x14 curDataFamily@0x18 loaded(byte)@0x1c +
//   exeMin@0x20 exeMax@0x24 dataMin@0x28 dataMax@0x2c → sizeof 0x30
// ---------------------------------------------------------------------------
class WindowsCodeProtectorServer
{
public:
    WindowsCodeProtectorServer();                // T @0x08575462
    ~WindowsCodeProtectorServer();               // T @0x08575484

    void Initialize();                           // T @0x085754cc
    void Destroy();                              // T @0x08575500
    bool FindExeFamilyFile(const char* name);    // T @0x085755f6
    WindowsDataFileFamily* FindDataFamilyFile(const char* name); // T @0x0857562e
    void SetExeCheckingOption(long cfg, long e1, long e2, long d1, long d2); // T @0x085756ce
    bool BeginLoading();                         // T @0x08575700
    bool LoadWindowsFile(const char* path, const char* name);   // T @0x08575712
    bool CompleteLoading();                      // T @0x0857592c
    bool IsDifferentExeVersionRoughly();         // T @0x08575970
    bool GenerateChecksum(CodeHackChecksumEx* ckEx, CodeHackCheckRangeEx* rangeEx); // T @0x08575a3c

    WindowsExeFileFamily*               m_exeFamily;    // +0x00
    std::vector<WindowsDataFileFamily*> m_dataFamilies; // +0x04..0xf
    long                    m_cfgLong;       // +0x10 SetExeCheckingOption p1
    long                    m_retryCount;    // +0x14 data→exe 轮换计数器
    int                     m_curDataFamily; // +0x18 data 家族轮转游标
    unsigned char           m_loaded;        // +0x1c（ORIG this[0x1c] byte）
    unsigned long           m_exeMin;        // +0x20（与形参 long 异型化：SetExeCheckingOption
    unsigned long           m_exeMax;        // +0x24  ORIG store 序 RHS 先 0x085756ce，
    unsigned long           m_dataMin;       // +0x28  探针 p8 W2 实证；调用点按 unsigned 消费）
    unsigned long           m_dataMax;       // +0x2c

private:
    WindowsCodeProtectorServer(const WindowsCodeProtectorServer&);
    WindowsCodeProtectorServer& operator=(const WindowsCodeProtectorServer&);
};

static_assert(sizeof(WindowsCodeProtectorServer) == 0x30,
              "WindowsCodeProtectorServer ABI drift");

// ---------------------------------------------------------------------------
// CCompressedBlob —— {char* buf@0; int size@4} 8B（objdump 0x0844de52 区）
// ---------------------------------------------------------------------------
class CCompressedBlob
{
public:
    // [b28c-lane24] P216：ORIG put_compressed_blob_data @0x083fbd06 对函数级 static
    // 局部 guard 构造无任何 EH landing/D1 cleanup 路径 ⇒ 原码 ctor/dtor 不抛。
    CCompressedBlob() throw();                   // W @0x0844de52
    ~CCompressedBlob() throw();                  // W @0x0844de6a

    bool compress(MySQL* db, int fieldIdx, char* src, int srcLen,
                  char** outStr);                // W @0x0844de9a
    bool uncompress(MySQL* db, int fieldIdx, char* dst, int dstLen,
                    int align);                  // W @0x0844df40
    void resize(int needSize);                   // W @0x0844e02a
    void reset();                                // W @0x0844e080

    char* m_buf;                                 // +0x00
    int   m_size;                                // +0x04
};

static_assert(sizeof(CCompressedBlob) == 8, "CCompressedBlob ABI drift");

#endif  // DNF_GAME_WINDOWSPACKFILES_H_
