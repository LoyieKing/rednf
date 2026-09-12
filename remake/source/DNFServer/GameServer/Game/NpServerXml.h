#ifndef GAME_NPSERVERXML_H_
#define GAME_NPSERVERXML_H_

// ============================================================================
// df_game_r 还原 —— np_server_xml::CServerXml / g_ServerString_（B @0x093fa660）。
// 定义在 CBoosterGage.cpp（LinkClosure 批迁移）；本头为跨 TU 唯一声明点
// （InterWave1 批，Inter_EmergencyPatchMessage/邮件文案消费）。
// ============================================================================

#include <string>

namespace np_server_xml
{

class CServerXml
{
public:
    std::string GetServerString(int nIndex, bool* pFlag) const;
    // [R3a18 2026-09-12] ORIG T（mangled `_ZN13np_server_xml10CServerXml10StrLoadingEv`）：
    // Init @0x82a1f4b 以全局 g_ServerString_ 为接收者调用（`movl $0x93fa660,(%esp)`）。
    void StrLoading();
};

}  // namespace np_server_xml
extern np_server_xml::CServerXml g_ServerString_;  // ORIG C 名 g_ServerString_ B @0x093fa660

#endif  // GAME_NPSERVERXML_H_
