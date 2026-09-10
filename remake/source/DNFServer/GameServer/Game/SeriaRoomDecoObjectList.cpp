// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SeriaRoomDecoObjectList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "SeriaRoomDecoObjectList.h"

#include <utility>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanStr

// ---- seriaDecoObjectInfo ----
#include "SeriaRoomDecoObjectList/seriaDecoObjectInfo.inc"

#include "SeriaRoomDecoObjectList/seriaDecoObjectInfo_1.inc"
    : m_stayAnimation(other.m_stayAnimation),
      m_walkAnimation(other.m_walkAnimation),
      m_runAnimation(other.m_runAnimation),
      m_ramdomAnimations(other.m_ramdomAnimations)
{
}

#include "SeriaRoomDecoObjectList/dtor_seriaDecoObjectInfo.inc"

#include "SeriaRoomDecoObjectList/clear.inc"
{
    m_stayAnimation.clear();
    m_walkAnimation.clear();
    m_runAnimation.clear();
    m_ramdomAnimations.clear();
}

// ============================================================================
// importSeriaRoomDecoEventObjectList —— ORIG T 0x0895e79b
//   bool (seriaRoomDecoEventObjectListScript*, const char*)
// ============================================================================
// [b28d-L8] N82 | 槽平移 + ScanType 调用序（ORIG 0x08xxxxxx）
#include "SeriaRoomDecoObjectList/importSeriaRoomDecoEventObjectList.inc"
                                        const char* path)
{
    // [round5 收口实测] ndiff=195（263/262）且 shift.py=MIXED：主体为**整体 +8 平移**
    // （token -0x5c→-0x54、tmpStr -0x60→-0x58、info -0x78→-0x70、key -0x7c→-0x74；
    // 两侧帧同为 0x90）⇒ 4.4 outgoing 对齐区/首槽基址常数族（N115/N126）；
    // 余项为 STL 临时/分配器槽（-0x51 vs -0x75，P86/P89 族）。p167：32 调用点
    // （ScanType/ScanStr/ScanInt/insert pair/dtor 链）两侧 CLEAR ⇒ 无语义缺口，封 N。
    if (!loadRDARScriptFile("", path))
        return false;
    if (s == 0)
        return false;

    std::string token;
    std::string tmpStr;
    int unusedInt = 0;      // ORIG -0x10（movl $0 实证，未使用）
    bool unusedFlag = false;  // ORIG -0xa（movb $0 实证，未使用）

    while (ScanType(token, true))
    {
        if (token == "[event object list]")
        {
            while (ScanType(token, true))
            {
                seriaDecoObjectInfo info;
                int key = -1;

                if (token == "[object index]")
                {
                    key = -1;
                    info.clear();
                    ScanInt(&key);
                    bool inObject = true;
                    while (inObject)
                    {
                        if (!ScanType(token, true))
                            break;
                        if (token == "[stay animation]")
                        {
                            ScanStr(&info.m_stayAnimation);
                        }
                        else if (token == "[walk animation]")
                        {
                            ScanStr(&info.m_walkAnimation);
                        }
                        else if (token == "[run animation]")
                        {
                            ScanStr(&info.m_runAnimation);
                        }
                        else if (token == "[ramdom animation]")
                        {
                            while (ScanStr(&tmpStr))
                            {
                                std::allocator<char> alloc;
                                std::string tmp(tmpStr.c_str(), alloc);
                                info.m_ramdomAnimations.push_back(std::move(tmp));
                            }
                        }
                        else if (token == "[/object index]")
                        {
                            inObject = false;
                            if (key != -1)
                            {
                                s->m_map.insert(
                                    std::make_pair(key, info));
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}
