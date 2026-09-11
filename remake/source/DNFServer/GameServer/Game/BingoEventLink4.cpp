// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BingoEventLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "BingoEvent.h"
#include "CBingoTest.h"
#include "CEnvironment.h"
#include "CSystemTime.h"
#include "CGameManager.h"
#include "Action.h"

#include "GlobalData.h"
#include "CGuildServerProxy.h"
#include "CDataManager.h"
#include "CUser.h"
#include "Inven_Item.h"
#include "CItem.h"
#include "CStackableItem.h"   // m_vec3a0（+0x3a0 标记号 vector 具名起点）
#include "CMailBoxHelper.h"
#include "RDARScriptStringManager.h"
#include "LogManager.h"
#include "PacketGuard.h"

#include <bitset>
#include <boost/foreach.hpp>   // ORIG：StartAction/dailyresetData 用 BOOST_FOREACH 遍历 +0x22c map
#include <map>
#include <vector>
#include <time.h>
#include <string.h>

#include "BingoEventLink4/BingoEvent.inc"


#include "BingoEventLink4/isSetBit_Uint.inc"


#include "BingoEventLink4/setBit_Uint.inc"


// ---------------------------------------------------------------------------
// BingoEvent 方法（docs/class_func_reports/BingoEvent/ 13 报告全文已读）。
// ---------------------------------------------------------------------------

#include "BingoEventLink4/dtor_BingoEvent.inc"


#include "BingoEventLink4/getBingoSystem.inc"


#include "BingoEventLink4/StartEvent.inc"


#include "BingoEventLink4/StartEvent_1.inc"


#include "BingoEventLink4/StartAction.inc"
{
    // [b28d-D9] ORIG 0x080c9bb8 用 BOOST_FOREACH 展开（contain/begin/end/deref/next/
    // done 全序列），非手写迭代器循环；容器为 G_CGameManager()+0x22c map。
    typedef std::map<unsigned int, CUser*>::value_type UserPair;
    BOOST_FOREACH(UserPair& pr, G_CGameManager()->m_userByAccId)
    {
        CUser* user = pr.second;
        if (user != 0)
        {
            user->sendBingoData();
        }
    }
}

#include "BingoEventLink4/EndEvent.inc"


#include "BingoEventLink4/dailyresetData.inc"
{
    int hourLimit = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    long curSec = GlobalData::s_systemTime_.getCurSec();
    int day = m_bingo->getToday(curSec, hourLimit);
    if (day > 0 && day < 0x20)
    {
        m_bingo->resetBoard(day);
        m_bingo->resetQuiz(day);
        cMyTrace trace(__PRETTY_FUNCTION__, 0x50, 0);
        trace("BingoEvent Day(%d)", day);
    }

    // [b28d-D9] ORIG 0x080c9d82 同样为 BOOST_FOREACH 展开（deref 后取 pair.second
    // 落 -0xc 槽，判空后三步操作）。
    typedef std::map<unsigned int, CUser*>::value_type UserPair;
    BOOST_FOREACH(UserPair& pr, G_CGameManager()->m_userByAccId)
    {
        CUser* user = pr.second;
        if (user != 0)
        {
            static_cast<BingoData*>(user->getBingoData())->clear();
            user->sendBingoData();
            user->sendDecorationObjectCnt(0);
        }
    }
}

#include "BingoEventLink4/reqBingoMark.inc"


#include "BingoEventLink4/sendOneMatchLineReward.inc"


#include "BingoEventLink4/sendBingoData.inc"

