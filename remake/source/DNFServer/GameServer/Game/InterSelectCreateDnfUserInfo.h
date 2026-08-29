#ifndef GAME_INTERSELECTCREATEDNFUSERINFO_H_
#define GAME_INTERSELECTCREATEDNFUSERINFO_H_

// HdrCreate2 批（/tmp/missing_h_chunk_ab）：ORIG 有导出符号但仓库无 .h 声明的类。
// 仅声明骨架（不入 CMake，无 cpp 实现）。参数类型按 ORIG demangled 原样；
// 返回类型与 static/成员属性未逐项核实者标【推断】，实现时须以 docs/class_func_reports 为准。
// ORIG 存在 vtable/typeinfo：类含虚函数，析构按 virtual【推断】声明。
// 实现见 TaiwanInternalDispatcher.cpp（OnlyOrigBatch4 补 makeRequest/dispatch_sig）。

#include "TaiwanInterfaceInterDispatcher.h"
class InterSelectCreateDnfUserInfo : public TaiwanInterfaceInterDispatcher
{
public:
    // ORIG makeRequest 0x08161218 写 +0x0(int m_id)/+0x4(bool received)。
    struct Msg
    {
        int m_id;          // +0x00
        bool m_received;   // +0x04
    };

    // ORIG 0x816208c _ZN28InterSelectCreateDnfUserInfoC1Ev (size 0x00000024)  报告: docs/class_func_reports/InterSelectCreateDnfUserInfo
    InterSelectCreateDnfUserInfo() throw();
    // ORIG 0x81620e0 _ZN28InterSelectCreateDnfUserInfoD0Ev (size 0x0000001e)  报告: docs/class_func_reports/InterSelectCreateDnfUserInfo
    virtual ~InterSelectCreateDnfUserInfo();
    // ORIG T 0x8161218 makeRequest（Ghidra C 实证返回 void）。
    static void makeRequest(int uid, unsigned int memberId, bool received);   // ORIG T 0x8161218（反汇编无 this 压参 → static）
    // ORIG T 0x8161012 dispatch_sig（返回 0/0x12f/0x139）。
    int dispatch_sig(CUser* user, char* buf, int len);
};

#endif  // GAME_INTERSELECTCREATEDNFUSERINFO_H_
