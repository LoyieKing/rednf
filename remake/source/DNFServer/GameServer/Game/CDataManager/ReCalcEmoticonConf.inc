// ReCalcEmoticonConf —— ORIG T 0x0835fc76 _ZN12CDataManager18ReCalcEmoticonConfER23CChattingEmoticonConfigRK5CUser
//   （docs/class_func_reports/CDataManager/ReCalcEmoticonConf.md）：
//   直通 m_emoticonList(+0x28) 的 CChattingEmoticonList::ReCalc（call 前置：
//   user 压 0x8(%esp)、config 压 0x4(%esp)、m_emoticonList 压 (%esp)）。
// ---------------------------------------------------------------------------
void CDataManager::ReCalcEmoticonConf(CChattingEmoticonConfig& config, const CUser& user)
{
    m_emoticonList->ReCalc(config, user);
}