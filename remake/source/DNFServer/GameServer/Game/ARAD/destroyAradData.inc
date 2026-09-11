void Arad_DataManager::destroyAradData()
{
    // ORIG 0x8188952：std::for_each(begin,end,FN_DeleteGameScript()) 逐项删除，clear。
    // FN_DeleteGameScript::operator() 定义归 Arad_Verify.cpp（ORIG 0x8188c56），
    // 类声明见 ARAD_FN_DeleteGameScript.h（唯一声明点）。
    std::for_each(m_scriptMap.begin(), m_scriptMap.end(), FN_DeleteGameScript());
    m_scriptMap.clear();
}