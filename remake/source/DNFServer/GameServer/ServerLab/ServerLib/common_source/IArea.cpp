// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 IArea/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// nsl::IArea.cpp
#include "IArea.h"
#include "ISession.h"
#include "TraceLog.h"

namespace nsl {

#include "IArea/IArea.inc"
{
    mMapIndex = mapIndex;
}

#include "IArea/dtor_IArea.inc"
{
}

#include "IArea/getMemberNum.inc"
{
    return (int)mMapObj.size();
}

#include "IArea/regist.inc"
{
    std::pair<MAP_OBJECTS_ITER, bool> ret;
    ret = mMapObj.insert(std::make_pair(id, obj));
    if (ret.second != false)
    {
        return true;
    }
    G_TraceLog()->sysLog(7, "IArea Insert Fail-%d, %x\n", id, this);
    return false;
}

#include "IArea/unregist.inc"
{
    mMapObj.erase(id);
}

#include "IArea/notifyAllAreaMember.inc"
{
    MAP_OBJECTS_ITER mapIter;
    mapIter = mMapObj.begin();
    while (mapIter != mMapObj.end())
    {
        ++mapIter;
    }
}

#include "IArea/isIterEnd.inc"
{
    // ORIG：test+je RET0(al==0 → false); mov $1; jmp EPI; RET0: mov $0。
    // 注意 je 在 al==0 时跳，故语义为 equal→true（与直接 return iter==end 相同）。
    if (iter == mMapObj.end())
    {
        return true;
    }
    return false;
}

#include "IArea/getBeginIter.inc"
{
    // ORIG：先写成员 mMapIter 再返回（-O0 经 -0xc 临时槽拷贝，
    // 与 begin() 返回槽直传的形态不同）。
    mMapIter = mMapObj.begin();
    return mMapIter;
}

#include "IArea/getValueFromIter.inc"
{
    return iter->second;
}

#include "IArea/getMemberAt.inc"
{
    MAP_OBJECTS_ITER mapIter;
    mapIter = mMapObj.find(id);
    if (mapIter != mMapObj.end())
    {
        return mapIter->second;
    }
    return NULL;
}

#include "IArea/getMemberFirst.inc"
{
    MAP_OBJECTS_ITER mapIter;
    if (mMapObj.begin() == mMapObj.end())
    {
        return NULL;
    }
    return mMapObj.begin()->second;
}

} // namespace nsl
