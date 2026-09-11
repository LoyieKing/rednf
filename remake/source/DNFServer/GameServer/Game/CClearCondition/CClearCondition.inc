CClearCondition::CClearCondition()
{
    // ORIG 0x82a4342：m_vec(std::vector<int> @+8) 默认构造后 call Clear()（0x82a438a，
    // 出线）清 +0/+4/+0x14 并 vec.clear。此处不得内联 Clear 体——ORIG ctor 是
    // `vector<int>::C1` + `call CClearCondition::Clear`，内联会多出逐字段 mov。
    Clear();
}