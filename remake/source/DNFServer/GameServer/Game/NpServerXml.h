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
};

}  // namespace np_server_xml
extern np_server_xml::CServerXml g_ServerString_;  // ORIG C 名 g_ServerString_ B @0x093fa660

#endif  // GAME_NPSERVERXML_H_
