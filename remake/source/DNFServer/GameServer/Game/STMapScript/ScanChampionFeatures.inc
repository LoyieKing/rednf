// [b28d-solo-STMapScript + R6a20 ④ 修复] _Z20ScanChampionFeaturesiR15STAssignMonster
// 槽序 -0x18(tok)/-0x14(prvalue 临时)/-0x10(i)/-0xc(k)；ORIG 0x089e6d57 只一次 store 到
// -0x14（**绑定临时**，非具名局部），0x089e6d6a call 目标 = push_back(int&&)（EOi）。
// 旧译文用 lvalue `push_back(feature)` ⇒ 选中 ERKi（const int&）调用目标不同；
// 用具名局部 + static_cast<int&&> 会多一对 store/load（实测 AE→DIFF 劣化）。
// 正解 = prvalue 形 `(int)CHAMPION_FEATURE_LIST[k]`：物化单个 -0x14 临时并 lea 直传。
void ScanChampionFeatures(int count, STAssignMonster& monster)
{
    std::string tok;
    for (int i = 0; i < count; ++i)
    {
        ScanStr(&tok);
        for (int k = 0; k < 37; ++k)
        {
            if (tok == CHAMPION_FEATURE_NAME[CHAMPION_FEATURE_LIST[k] & 0xff])
            {
                // ORIG 0x089e6d4d-0x089e6d6a：prvalue 临时 + push_back(int&&)
                monster.m_vec28.push_back((int)CHAMPION_FEATURE_LIST[k]);
                break;
            }
        }
    }
}