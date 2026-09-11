// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LogManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LogManager（G5 日志族）
// 参照 docs/class_func_reports/LogManager.md；ORIG 布局：
//   s_data   0x09501e60  static LogManagerData*（std::array<shared_ptr<LogWriter>,64>）
//   s_indent 0x09501e64  static int
// 全部 LogManager 方法为静态成员；prepareData 为文件内 static。
#include "LogManager.h"
#include <memory>
#define LOG_WRITER_SP std::shared_ptr

#include <array>
#include <cstdarg>
#include <cstdio>
#include <new>
#include <string>

struct LogManagerData
{
    std::array<LOG_WRITER_SP<LogWriter>, 64> data;
};

static LogManagerData* s_data = 0;
static int s_indent = 0;

static void prepareData()
{
    if (!s_data)
    {
        s_data = new LogManagerData();
    }
}

// ORIG 0x8ad40b8：_ZN23_LogManagerData_createrC1Ev / C2Ev（各 0xd 字节）
//   构造器仅调用 prepareData()；全局实例使 s_data 在静态初始化期完成分配
//   （ORIG 生命周期：启动即建，非懒初始化）。ORIG 无该类的 D1/D0 符号，
//   故无用户析构（trivial）。
class _LogManagerData_creater
{
public:
    _LogManagerData_creater() { prepareData(); }
};

static _LogManagerData_creater _LogManagerData_creater_instance;   // ORIG _ZL32_LogManagerData_creater_instance 0x09501e68

#include "LogManager/dtor_LogWriter.inc"


#include "LogManager/logFormat.inc"


#include "LogManager/logString.inc"


#include "LogManager/getLogWriter.inc"


#include "LogManager/setLogWriter.inc"


#include "LogManager/pushIndent.inc"


#include "LogManager/popIndent.inc"

