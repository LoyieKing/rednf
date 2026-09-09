// STDeadTowerDialog 实现。
// 证据: objdump 0x0891b12c..0x0891b18a（ctor/dtor/clear）与
//   0x08929428..0x089294d8（copy ctor/operator=）；类报告缺失, nm 定址转写。
#include "STDeadTowerDialog.h"

// 全局 C++ 权威签名（DNFLexWrapperHelpers.cpp 提供，匹配 ORIG mangled 符号）。
// 不 include DNFLexWrapper.h：该头经 DNFLex.h 引入 <iostream>，会给本 TU 带上
// std::ios_base::Init（__ioinit）动态初始化与 `_GLOBAL__I_` 键；ORIG 本 TU 无 GI
// （orig_tu_names.tsv 513 条无 STDeadTowerDialog.cpp）。签名与 DNFLexWrapper.h 一致。
bool loadRDARScriptFile(const char* directory, const char* path);
bool ScanType(std::string& token, bool consume);
int ScanInt(bool* ok);
bool ScanStr(std::string* value);

// ORIG T 0x08908d46 importDeadTowerDialogScript(path, STDeadTowerFullDialog&)：
// loadRDARScriptFile 失败返 0x18d9；按 token 分派三组对话框，ScanInt 失败返
// 0x18e6/0x18f6/0x1906，ScanStr 失败返 0x18ea/0x18fa/0x190a；成功返 0。
// 211/211 AE：bool ok 函数级单槽（-0x9，P320）、token 先于 loadRDARScriptFile
// 构造（-0x10）、三分支分别写 m_flag1/m_flag2/m_flag3（ORIG +0x8/+0x9/+0xa）、
// ScanType 失败改 break 共享尾部 return 0（P164 jne 尾块 +0x299）。
int importDeadTowerDialogScript(const char* path, STDeadTowerFullDialog& out)
{
    // ORIG 0x8908d4e：bool ok 为函数级单槽（-0x9），三组 dialog 共用（P320）。
    bool ok = false;
    // ORIG 0x8908d52：token 在 loadRDARScriptFile 之前构造（-0x10）。
    std::string token;
    // ORIG 0x8908d64：loadRDARScriptFile("", path) —— dir 空串，path 即文件路径。
    if (!loadRDARScriptFile("", path))
        return 0x18d9;
    for (;;)
    {
        if (!ScanType(token, true))
            break;   // ORIG 0x8908d9a jne 尾块（+0x299）：失败共享尾部 return 0（P164）。
        if (token == "[dialog on map start]")
        {
            STDeadTowerDialog dlg;
            dlg.m_index = ScanInt(&ok);
            if (!ok)
                return 0x18e6;
            ok = ScanStr(&dlg.m_str);
            if (!ok)
                return 0x18ea;
            // ORIG 0x8908e13：movb $0x1,-0x14 = dlg1(+0x8) ⇒ m_flag1。
            dlg.m_flag1 = 1;
            out.m_mapStartDialog.push_back(dlg);
        }
        else if (token == "[dialog on map complete]")
        {
            STDeadTowerDialog dlg;
            dlg.m_index = ScanInt(&ok);
            if (!ok)
                return 0x18f6;
            ok = ScanStr(&dlg.m_str);
            if (!ok)
                return 0x18fa;
            // ORIG 0x8908ed3：movb $0x1,-0x1f = dlg2(+0x9) ⇒ m_flag2。
            dlg.m_flag2 = 1;
            out.m_mapCompleteDialog.push_back(dlg);
        }
        else if (token == "[dialog on die]")
        {
            STDeadTowerDialog dlg;
            dlg.m_index = ScanInt(&ok);
            if (!ok)
                return 0x1906;
            ok = ScanStr(&dlg.m_str);
            if (!ok)
                return 0x190a;
            // ORIG 0x8908f96：movb $0x1,-0x2a = dlg3(+0xa) ⇒ m_flag3。
            dlg.m_flag3 = 1;
            out.m_dieDialog.push_back(dlg);
        }
    }
    return 0;
}

STDeadTowerDialog::STDeadTowerDialog()
{
    clear();
}

STDeadTowerDialog::~STDeadTowerDialog()
{
}

void STDeadTowerDialog::clear()
{
    m_index = 0;
    m_str.clear();
    m_flag1 = 0;
    m_flag2 = 0;
    m_flag3 = 0;
}

STDeadTowerDialog::STDeadTowerDialog(STDeadTowerDialog const& r)
    : m_index(r.m_index)
    , m_str(r.m_str)
    , m_flag1(r.m_flag1)
    , m_flag2(r.m_flag2)
    , m_flag3(r.m_flag3)
{
}

STDeadTowerDialog& STDeadTowerDialog::operator=(STDeadTowerDialog const& r)
{
    m_index = r.m_index;
    m_str = r.m_str;
    m_flag1 = r.m_flag1;
    m_flag2 = r.m_flag2;
    m_flag3 = r.m_flag3;
    return *this;
}
