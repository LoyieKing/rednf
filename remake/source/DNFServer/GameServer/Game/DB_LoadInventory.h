#ifndef GAME_DB_LOADINVENTORY_H_
#define GAME_DB_LOADINVENTORY_H_

// ============================================================================
// df_game_r 还原 —— DB_LoadInventory（SIG_LOAD_INVENTORY DB 装载分发器，
// DB 线程侧；Inter_LoadInven 载入侧对称）。
// ORIG 全部符号见 docs/class_func_reports/DB_LoadInventory/（14 mangled）。
//   ctor 0x0840a604（C1/C2 同址：调 DBDispatcher::DBDispatcher 后设 vptr）
//   ~DB_LoadInventory 0x0840a620（D1/D2）/ 0x0840a650（D0 deleting）
//   dispatch 0x0840a66e（virtual bool，覆盖 DBDispatcher::dispatch）
// 对象尺寸 4（仅 vptr），基类 DBDispatcher 见同目录头。
// ============================================================================

#include "SigTypes.h"      // SIG_LOAD_INVENTORY / TAG_CHARAC_INVEN_DATA
#include "DBDispatcher.h"
#include "GamePODTypes.h"  // AvatarDetailInfo（_GetUserAvatar 输出条目）
#include "Inven_Item.h"    // Inven_Item（0x3d 字节，avatar/creature 槽位）
#include "CTitleBook.h"    // stItemLockInfo（GetItemLock 输出）
class Stream;

class DB_LoadInventory : public DBDispatcher
{
public:
    DB_LoadInventory();                          // C1/C2 0x0840a604
    virtual ~DB_LoadInventory();                 // D1/D2 0x0840a620（D0
                                                 // deleting 0x0840a650 编译器产出）

    virtual bool dispatch(int userIdx, int slot, Stream* stream);
                                                 // ORIG T 0x0840a66e

    bool GetUserInven(SIG_LOAD_INVENTORY* sig);              // 0x0840b0d8
    bool GetUserAvatar(SIG_LOAD_INVENTORY* sig);             // 0x0840bd3a
    bool GetUserCreature(SIG_LOAD_INVENTORY* sig);           // 0x0840ca74
    bool _GetEquipMaxItemLevel(SIG_LOAD_INVENTORY* sig);     // 0x0840aac8
    int  _getCharacInvenExpand(SIG_LOAD_INVENTORY* sig) const;   // 0x0840ad2a
                                                             // _ZNK...21_getCharacInvenExpand
                                                             // （返回 LineFunc 风格错误行号）
    bool SendResult(int userIdx, int slot, SIG_LOAD_INVENTORY* sig);          // 0x0840da6e
    bool SendResult_forchina(int userIdx, int slot,
                             SIG_LOAD_INVENTORY* sig);                        // 0x0840d98c

    // PRETTY_FUNCTION 实证：static（__PRETTY_FUNCTION__ 前缀 "static"）。
    static unsigned int GetItemLock(unsigned int characNo,
                                    stItemLockInfo* out);    // 0x0840d766
    static bool _GetTagCharacInven(unsigned int characNo,
                                   TAG_CHARAC_INVEN_DATA* out);  // 0x0840aba2
    static bool _GetUserAvatar(unsigned int characNo,
                               AvatarDetailInfo* out,
                               Inven_Item* inven, Inven_Item* mirror,
                               int& count);                    // 0x0840be8a
};

#endif  // GAME_DB_LOADINVENTORY_H_
