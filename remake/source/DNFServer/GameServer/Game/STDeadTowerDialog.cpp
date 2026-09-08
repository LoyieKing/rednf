// STDeadTowerDialog 实现。
// 证据: objdump 0x0891b12c..0x0891b18a（ctor/dtor/clear）与
//   0x08929428..0x089294d8（copy ctor/operator=）；类报告缺失, nm 定址转写。
#include "STDeadTowerDialog.h"
#include "DNFLexWrapper.h"   // ScanType / ScanInt / ScanStr
#include "NpcScript.h"       // loadRDARScriptFile

// ORIG T 0x08908d46 importDeadTowerDialogScript(path, STDeadTowerFullDialog&)：
// loadRDARScriptFile 失败返 0x18d9；按 token 分派三组对话框，ScanInt 失败返
// 0x18e6/0x18f6/0x1906，ScanStr 失败返 0x18ea/0x18fa/0x190a；成功返 0。
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x08908d46 orig/ours=209/212 ndiff=197。
// 残差为帧/槽号平移（局部 token/字符串缓冲）与 RDARScript 解析器内联差；
// P167 调用点已核，语义按报告逐条对齐。
int importDeadTowerDialogScript(const char* path, STDeadTowerFullDialog& out)
{
    // ORIG 0x8908d64：loadRDARScriptFile("", path) —— dir 空串，path 即文件路径。
    if (!loadRDARScriptFile("", path))
        return 0x18d9;
    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
            return 0;
        if (token == "[dialog on map start]")
        {
            bool ok = false;
            STDeadTowerDialog dlg;
            dlg.m_index = ScanInt(&ok);
            if (!ok)
                return 0x18e6;
            ok = ScanStr(&dlg.m_str);
            if (!ok)
                return 0x18ea;
            dlg.m_flag1 = 1;
            out.m_mapStartDialog.push_back(dlg);
        }
        else if (token == "[dialog on map complete]")
        {
            bool ok = false;
            STDeadTowerDialog dlg;
            dlg.m_index = ScanInt(&ok);
            if (!ok)
                return 0x18f6;
            ok = ScanStr(&dlg.m_str);
            if (!ok)
                return 0x18fa;
            dlg.m_flag1 = 1;
            out.m_mapCompleteDialog.push_back(dlg);
        }
        else if (token == "[dialog on die]")
        {
            bool ok = false;
            STDeadTowerDialog dlg;
            dlg.m_index = ScanInt(&ok);
            if (!ok)
                return 0x1906;
            ok = ScanStr(&dlg.m_str);
            if (!ok)
                return 0x190a;
            dlg.m_flag1 = 1;
            out.m_dieDialog.push_back(dlg);
        }
    }
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
