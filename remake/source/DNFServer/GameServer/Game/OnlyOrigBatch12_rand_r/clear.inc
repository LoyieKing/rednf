// ===========================================================================
// tagRecommendNewbieOptionTable::clear  ORIG W 0x0891ad8c：
//   先 push_back 11 个空 vector，再逐项 clear + reserve(6)。
// （ORIG TU = RDARScriptEventCharacterParameter.cpp）
// ===========================================================================
void tagRecommendNewbieOptionTable::clear()
{
    for (int i = 0; i <= 0xa; ++i)
        push_back(std::vector<tagRecommendNewbieOption>());
    for (int i = 0; i <= 0xa; ++i)
    {
        operator[](i).clear();
        operator[](i).reserve(6);
    }
}