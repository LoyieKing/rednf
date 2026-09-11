// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 HeroMissionLink4/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "HeroMissionEvent.h"
#include "CCompoundAvatar.h"
#include "CDataManager.h"
#include "CEventScriptMng.h"
#include "CUser.h"
#include "GameWorld.h"
#include "GameRand.h"       // get_rand_int
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "PacketGuard.h"
#include "PvPTypes.h"       // CMTRand 完整类型

#include <boost/tokenizer.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "HeroMissionLink4/getItemIndex.inc"

#include "HeroMissionLink4/clear.inc"


#include "HeroMissionLink4/reset.inc"


#include "HeroMissionLink4/sendMissionData.inc"



#include "HeroMissionLink4/test_process_change_day.inc"


#include "HeroMissionLink4/ProcTest.inc"

#include "HeroMissionLink4/getItemIndex_1.inc"


#include "HeroMissionLink4/setStringToken.inc"
{
    clear();

    typedef boost::token_iterator<
        boost::char_separator<char>,
        __gnu_cxx::__normal_iterator<const char*, std::string>,
        std::string> TokenIterator;

    boost::char_separator<char> separator(" ", 0,
                                          boost::drop_empty_tokens);
    boost::tokenizer<boost::char_separator<char>,
                     __gnu_cxx::__normal_iterator<const char*, std::string>,
                     std::string>
        tokens(token, separator);

    TokenIterator tit = tokens.begin();

    // P23：ORIG 每步以 ++ 返回值直传下一调用（`mov %eax,(%esp); call ptEv`），
    // 拆语句 `++tit; tit->...` 会重复 lea 迭代器地址（每处 +2 条）。
    m_mission = atoi(tit->c_str());                     // +0x00
    m_field4 = (unsigned short)atoi((++tit)->c_str());  // +0x04
    m_field6 = (unsigned short)atoi((++tit)->c_str());  // +0x06
    m_fieldA = (unsigned short)atoi((++tit)->c_str());  // +0x0a
    m_field8 = (unsigned short)atoi((++tit)->c_str());  // +0x08
}

#include "HeroMissionLink4/dtor_HeroMissionEvent.inc"

