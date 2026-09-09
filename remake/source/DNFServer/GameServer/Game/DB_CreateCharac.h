#ifndef DNF_GAME_DB_CREATECHARAC_H_
#define DNF_GAME_DB_CREATECHARAC_H_

// ============================================================================
// df_game_r 还原 —— DB_CreateCharac（DB 线程角色创建分发器）
// 报告：docs/class_func_reports/DB_CreateCharac/（26 符号，地址见各注释）。
// 布局：仅基类 DBDispatcher 子对象，sizeof = 4（vptr）。
//   vtable _ZTV15DB_CreateCharac @0x8c5fc70 [dispatch][~D1][~D0]，
//   typeinfo @0x8c63084 [nm]。
// 硬契约：W6_TailA.TailADBLoadEtc 将 include 本头构造临时对象并调 CreatePvP。
// ============================================================================

#include "DBDispatcher.h"
#include "Inven_Item.h"

class MySQL;
class Stream;
struct SIG_CREATE_CHARAC;

class DB_CreateCharac : public DBDispatcher
{
public:
    DB_CreateCharac();                                  // C1/C2 T @0x08400cc2
    ~DB_CreateCharac();                                 // D1 T @0x08400cde / D0 T @0x08400d0e
                                                        // （D2 由 GCC 随非内联析构发射）

    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // T @0x08400d2c

    // ---- 角色创建主链 ----
    bool CreateNewCharac(SIG_CREATE_CHARAC* sig);       // T @0x08401dcc
    bool GetCharacNo(SIG_CREATE_CHARAC* sig);           // T @0x0840275c
    bool _CreateNewCharacStat(SIG_CREATE_CHARAC* sig);  // T @0x0840215e
    bool CreateInventory(unsigned int characNo, Inven_Item* item);  // T @0x08402230
    bool CreateEquip(unsigned int characNo, Inven_Item* item);      // T @0x084023ac
    bool CreateSkill(SIG_CREATE_CHARAC* sig);           // T @0x084024e6
    bool CreatePvP(unsigned int characNo);              // T @0x084026ec
    bool SendResult(int userIdx, int slot, SIG_CREATE_CHARAC* sig);  // T @0x084027b0

    // ---- 名称/限制检查与登记 ----
    bool _checkCharacName(MySQL* db, char const* dbTable,
                          SIG_CREATE_CHARAC* sig);      // T @0x084011d4
    bool CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig);          // T @0x08401504
    bool CheckLimitCreateNewCharac(SIG_CREATE_CHARAC* sig, bool traceFlag);  // T @0x08401922
    bool InsertLimitCreateNewCharac(SIG_CREATE_CHARAC* sig);         // T @0x084016ae
    bool InsertLimitCreateNewCharac(SIG_CREATE_CHARAC* sig, bool traceFlag); // T @0x08401b9a
    static void makeRequestResetLimitCreateMid();       // T @0x08401864（ORIG 无 this 直调）

    // ---- 事件角色/附属数据 ----
    bool _checkCreateEventCharacter(SIG_CREATE_CHARAC* sig);   // T @0x084013b2
    bool _updateCreateEventCharacter(SIG_CREATE_CHARAC* sig);  // T @0x08401428
    bool _createCharacQuest(unsigned int characNo);            // T @0x08401094
    bool _createCharacDungeon(unsigned int accId);             // T @0x084010fc
    // ORIG 尾部遗留 MySQL::exec 返回值被调用方（dispatch +0x292）按 al 判定；
    // mangled 不含返回类型，bool 化忠实还原该 ABI 行为。
    bool _createCharacNPCRelationship(unsigned int characNo);  // T @0x0840103e

private:
    void MakeInsertCharacInfo(MySQL* db, SIG_CREATE_CHARAC* sig,
                              char const* escapedName);    // T @0x080ec2a0
};

#endif  // DNF_GAME_DB_CREATECHARAC_H_
