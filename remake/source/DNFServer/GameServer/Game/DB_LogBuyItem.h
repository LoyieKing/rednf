// ============================================================================
// df_game_r 还原 —— DB_LogBuyItem 权威声明（P104 跨 TU 统一）
// ORIG：C1 0x0844f66e（DBDispatcher 派生空构造，注册 slot 593）、
//   T 0x0812859a _ZN13DB_LogBuyItem8dispatchEiiP6Stream（虚覆盖，定义
//     W8LT_DBTask.cpp）
//   T 0x0812887e _ZN13DB_LogBuyItem11makeRequestEjjjj20ENUM_DBLOG_ITEM_TYPE
//     （static，定义 CUser.cpp，调用点 CUser.cpp:899）
// 消费 TU：W8LT_DBTask.cpp（RegistDispatcher new + dispatch）、CUser.cpp
// （makeRequest 调用）。两 TU 曾各自定义同名异形局部类 → ODR 冲突，统一至此头。
// ============================================================================
#ifndef DNF_GAME_DB_LOGBUYITEM_H_
#define DNF_GAME_DB_LOGBUYITEM_H_

#include "DBDispatcher.h"
// ENUM_DBLOG_ITEM_TYPE —— 与 CInventory.h/CUser.h 同款 guard 双定义（P105
// 类内 typedef 之外的老式防重模式），避免为单个枚举引入 CInventory.h 及其
// NumberToString(u64) 重载（会与 W8LT_DBTask.cpp 的 u32 局部声明歧义）。
#ifndef DNF_ENUM_DBLOG_ITEM_TYPE_DEFINED
#define DNF_ENUM_DBLOG_ITEM_TYPE_DEFINED
enum ENUM_DBLOG_ITEM_TYPE
{
    ENUM_DBLOG_ITEM_TYPE_0 = 0
};
#endif

class DB_LogBuyItem : public DBDispatcher
{
public:
    DB_LogBuyItem();   // ORIG C1 0x0844f66e（定义 W8LT_DBTask.cpp）
    virtual bool dispatch(int userIdx, int slot, Stream* stream);  // ORIG 0x0812859a
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            unsigned int itemIdx, unsigned int type,
                            ENUM_DBLOG_ITEM_TYPE value);  // ORIG 0x0812887e
};

#endif  // DNF_GAME_DB_LOGBUYITEM_H_
