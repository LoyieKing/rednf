// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TCPDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// nsl::TCPDispatcher.cpp
#include "TCPDispatcher.h"
#include "IHandlers.h"
#include "TCPUser.h"
#include "TCPSocket.h"
#include "Message.h"
#include "TraceLog.h"
#include "ServiceFactory.h"
#include "PacketDesign.h"

namespace nsl {

#include "TCPDispatcher/TCPDispatcher.inc"
{
    MaxCategory = 100000;
    NullHandler = NULL;
}

#include "TCPDispatcher/SetMaxCategory.inc"
{
    MaxCategory = Category;
}

#include "TCPDispatcher/SetNullSessionHandler.inc"
{
    NullHandler = handler;
}

#include "TCPDispatcher/dispatch.inc"
{
    CMsgCell* pMsg = msg->getCellFromMessage();
    PACKET_HEADER* pPCK = pMsg->GetPacket();
    int nProtoID = pPCK->getPacketID();
    int nCategory = pPCK->getCategory();
    int ret = 0;
    u->setLastAccessTime();
    if ((nCategory == 0xffffffff) && (nProtoID == 0))
    {
        return false;
    }
    else if (nCategory < 0 || nCategory >= MaxCategory)
    {
        return true;
    }
    else
    {
        INetWorkHandler* netWorkHandler = pApp->super_IHandlers.getNetWorkHandler(nCategory);
        if (netWorkHandler != NULL)
        {
            INetWorkHandler::networkFuncType handle = netWorkHandler->searchNetworkFunc(nProtoID);
            if (handle != NULL)
            {
                ret = (pApp->super_IHandlers.getNetWorkHandler(nCategory)->*handle)(pMsg, u);
                if (ret != 0)
                {
                    G_TraceLog()->sysLog(7, "TCP : '%d' \xc3\xb3\xb8\xae\xbf\xa1 \xbd\xc7\xc6\xd0 \xc7\xcf\xbf\xb4\xbd\xc0\xb4\xcf\xb4\xd9. Error: %d", nProtoID, ret);
                    return true;
                }
            }
            else
            {
                G_TraceLog()->sysLog(7, "TCP : could not find handler for Category :'%d' Protocol: '%d'.", nCategory, nProtoID);
                return true;
            }
        }
        else
        {
            G_TraceLog()->sysLog(7,
                "TCP : '%d' <- protocol category \xb0\xa1 \xb9\xfc\xc0\xa7\xb8\xa6 \xb9\xfe\xbe\xee\xb3\xb5\xbd\xc0\xb4\xcf\xb4\xd9, ip:%d.%d.%d.%d",
                nCategory, (int)u->pSock_->getPeerAdrs()[0],
                (int)u->pSock_->getPeerAdrs()[1], (int)u->pSock_->getPeerAdrs()[2],
                (int)u->pSock_->getPeerAdrs()[3]);
            return true;
        }
        return false;
    }
}

} // namespace nsl
