// [b28d-solo-STMapScript] _Z20ScanChampionFeaturesiR15STAssignMonster：AE（离开 DIFF/NEAR）。
// 修法：补 ORIG 具名临时 feature@-0x14（P0 值先物化）并置于 tok 之前，复现槽序
// -0x18(tok)/-0x14(feature)/-0x10(i)/-0xc(k)。p167 残 1：ORIG push_back(int&&)（4.1 视同型
// cast 为 prvalue），4.4 对 (int)feature 多一条 mov ⇒ 保留 lvalue 形；int 拷贝/移动等价（N）。
void ScanChampionFeatures(int count, STAssignMonster& monster)
{
    int feature;                       // ORIG -0x14(%ebp)：值先物化再传址（P0）
    std::string tok;
    for (int i = 0; i < count; ++i)
    {
        ScanStr(&tok);
        for (int k = 0; k < 37; ++k)
        {
            if (tok == CHAMPION_FEATURE_NAME[CHAMPION_FEATURE_LIST[k] & 0xff])
            {
                feature = CHAMPION_FEATURE_LIST[k];   // ORIG 0x089e6d4d-0x089e6d6a
                // ORIG 调 push_back(int&&)（4.1 把同类型 cast 视作 prvalue）；
                // 4.4 对 (int)feature 产额外 mov ⇒ 保留 lvalue 形（拷贝/移动对 int 等价）。
                monster.m_vec28.push_back(feature);
                break;
            }
        }
    }
}