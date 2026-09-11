// ===========================================================================
// isEndModuleTag  ORIG T 0x08a97610：*tag == MODULE_NAME_TAG[idx*2+1]。
// （ORIG TU = RDARScriptSkill.cpp）
// ===========================================================================
bool isEndModuleTag(std::string* tag, int idx)
{
    return *tag == MODULE_NAME_TAG[idx][1];
}