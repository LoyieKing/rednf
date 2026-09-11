#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserStatA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"          // G_CDataManager/m_characList/find_quest/find_item/get_event_script_mng
#include "CCharacList.h"           // CCharacList::m_characs[11]（+0x000，步长 0x7dc）
#include "CCharacter.h"            // CCharacter 布局（0x7dc 步长）/get_growtype_stat/_Additioal_info
#include "Quest.h"                 // Quest::m_mailFlag/m_levelMin、stSelectQuestParam(CUser*)
#include "CInventory.h"            // GetInvenRef/GetInvenData/insertItemIntoInventory/update_item
#include "Inven_Item.h"            // m_addInfo/m_field1/set_add_info/ResetItemAttr
#include "RDARScriptStringManager.h"
#include "CMailBoxHelper.h"
#include "GlobalData.h"
#include "CGuildServerProxy.h"     // CEventBase/CEventManager::GetRepeatEvent
#include "CFatigueBatteryHandle.h"
#include "CSystemTime.h"
#include "CEventScriptMng.h"
#include <string.h>
#include <list>

class DB_InsertUnlimitSupportLog
{
public:
    // ORIG 0x0842ebd0：_ZN26DB_InsertUnlimitSupportLog11makeRequestEjjPKc（返回 void，
    // 见 DB_InsertUnlimitSupportLog/makeRequest.md 反编译）。类声明与 DB_U3.cpp/
    // CMailBoxHelper.cpp 的 TU 局部前置声明模式一致。
    static void makeRequest(unsigned int uid, unsigned int characNo,
                            const char* name);
};

class DB_InsertArchieveEventLog
{
public:
    // ORIG 0x0842e856：
    // _ZN25DB_InsertArchieveEventLog11makeRequestEjjN24SIG_INSERT_ACHIEVE_EVENT17ENUM_ACHIEVE_TYPEE
    // （返回 void，见 DB_InsertArchieveEventLog/makeRequest.md 反编译）。
    static void makeRequest(unsigned int accId, unsigned int characNo,
                            SIG_INSERT_ACHIEVE_EVENT::ENUM_ACHIEVE_TYPE type);
};

#include "CUserStatA/_update_charac_stat.inc"


#include "CUserStatA/_onLevelUp.inc"


#include "CUserStatA/UpdateTutorialSkipable.inc"

