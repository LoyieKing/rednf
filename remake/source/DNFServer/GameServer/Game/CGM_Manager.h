#ifndef _CGM_MANAGER_H_
#define _CGM_MANAGER_H_

#include <map>
#include <pthread.h>

#include "GameTypes.h"

class MySQL;

// CGMRecord —— gm_manifest 表记录（+0 level，+4 附加标志）。
struct CGMRecord
{
    int m_mode;    // +0x00（TurnGmMode/GetCurrentGmMode 按首字节访问）
    int m_level;   // +0x04（Set/GetGmLevel，ORIG 写 +4）
};

class CGM_Manager
{
public:
    CGM_Manager();
    ~CGM_Manager();

    int GetGmLevel(unsigned int accId);   // ORIG T 0x082993d8
    bool IsGm(unsigned int accId);
    unsigned char TurnGmMode(unsigned int accId);   // ORIG 0x0829917c，返回切换后的模式
    char GetCurrentGmMode(unsigned int accId);
    bool LoadGmListFromDb();   // ORIG T 0x08299530

private:
    // ORIG 布局：map<unsigned int,CGMRecord> +0x00、MySQL* +0x18、rwLock +0x1c。
    std::map<unsigned int, CGMRecord> m_gmMap;  // +0x00
    char m_pad14[4];                            // +0x14
    MySQL* m_db;                                // +0x18
    rwLock m_lock;                              // +0x1c（0x20 字节）
};

#endif // _CGM_MANAGER_H_
