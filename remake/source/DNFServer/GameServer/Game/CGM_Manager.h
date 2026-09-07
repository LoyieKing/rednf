#ifndef _CGM_MANAGER_H_
#define _CGM_MANAGER_H_

#include <map>
#include <pthread.h>

#include "GameTypes.h"

// ORIG 各 TU 一份 _ZL6GM_LV{0,1,2,3,C}（r 0x4）：GM 等级常量（文件级 static）。
static const int GM_LV0 = 0;
static const int GM_LV1 = 1;
static const int GM_LV2 = 2;
static const int GM_LV3 = 3;
static const int GM_LVC = 0;

class MySQL;

// CGMRecord —— gm_manifest 表记录（+0 模式标志，+4 GM 等级）。
// ORIG C1 0x082990b4：movb $0,(+0)；movl $0,(+4)。D1/D2 0x082990ca 空析构。
// +0 是 bool：TurnGmMode/GetCurrentGmMode 以 movzbl 读、movb 写（P9/P11 判据）。
struct CGMRecord
{
    bool m_mode;            // +0x00（TurnGmMode/GetCurrentGmMode 按字节访问，movzbl）
    int m_level;            // +0x04（Set/GetGmLevel，ORIG 写 +4）

    CGMRecord();
    ~CGMRecord();
};

class CGM_Manager
{
public:
    CGM_Manager();
    ~CGM_Manager();

    int GetGmLevel(unsigned int accId);   // ORIG T 0x082993d8
    bool IsGm(unsigned int accId);
    unsigned char TurnGmMode(unsigned int accId);   // ORIG 0x0829917c，返回切换后的模式
    char GetCurrentGmMode(unsigned int accId);      // ORIG T 0x08299276（Batch8 实现，补声明）
    char LoadGmListFromDb();   // ORIG T 0x08299530（Ghidra char __thiscall，返回 ok 原文）
    void SetGmLevel(unsigned int accId, int level);   // ORIG T 0x0829932a
    bool LoadGmDataFromDb(unsigned int accId);        // ORIG T 0x08299740

private:
    // ORIG 布局：map<unsigned int,CGMRecord> +0x00（0x18 字节）、MySQL* +0x18、rwLock +0x1c。
    std::map<unsigned int, CGMRecord> m_gmMap;  // +0x00
    MySQL* m_db;                                // +0x18
    rwLock m_lock;                              // +0x1c（0x20 字节）
};

#endif // _CGM_MANAGER_H_
