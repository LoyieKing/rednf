// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InterDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// nsl::InterDispatcher.cpp
#include "InterDispatcher.h"
#include "IHandlers.h"
#include "Message.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "ServiceError.h"
#include "InternalMsgDesign.h"
#include "MsgCell.h"

namespace nsl {

#include "InterDispatcher/InterDispatcher.inc"
{
}

#include "InterDispatcher/dispatch.inc"
{
    CMsgCell* pMsg = msg->getCellFromMessage();
    INTERNALMSG_HEADER* pInternalMsgHeader = pMsg->GetInternalMsg();
    int nProtoId = pInternalMsgHeader->getInternalMsgID();
    int nCategory = pInternalMsgHeader->getCategory();
    int ret = 0;
    IInterHandler* pInterHandler = pApp->super_IHandlers.getInterHandler(nCategory);
    if (pInterHandler != NULL)
    {
        IInterHandler::interFuncType handle = pInterHandler->searchInterFunc(nProtoId);
        if (handle != NULL)
        {
            ret = (pApp->super_IHandlers.getInterHandler(nCategory)->*handle)(pMsg);
            if (ret != 0)
            {
                G_TraceLog()->sysLog(7, "InterDispatcher : protocol: %d , Error: %s", nProtoId, GetErrorStr(ret));
                return false;
            }
        }
        else
        {
            G_TraceLog()->sysLog(7, "InterDispatcher : can't find handler for category: %d, protocol : %d.", nCategory, nProtoId);
            return false;
        }
    }
    else
    {
        G_TraceLog()->sysLog(7, "InterDispatcher : category : %d", nCategory);
        return false;
    }
    return true;
}

} // namespace nsl
