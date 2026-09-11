// ============================================================================
// getRandomChampionElement —— ORIG T 0x088b9d54（STAICharacterCommonScript 版；
// STMonsterScript 版在 STMonsterScript.cpp）。报告
// _lt_global_gt_/getRandomChampionElement_088b9d54.md 完整转写。
// m_championElements(+0xf20) 非空 → rand()%size 取元素；空 → rand()%4+1。
// ============================================================================
int getRandomChampionElement(const STAICharacterCommonScript* script, int seed)
