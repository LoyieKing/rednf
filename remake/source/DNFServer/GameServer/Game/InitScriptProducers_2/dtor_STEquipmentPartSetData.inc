// b28c-L20: AE — ORIG D1 0x08907348 orig=52 our=52 gap=0 ndiff=0：`m_script = 0` 在 if 之外无条件执行（P141：
// 保留无条件 delete，编译器自带判空守卫，ORIG 双 test/je 形）。
STEquipmentPartSetData::~STEquipmentPartSetData()
{
    if (m_script != 0)
        delete m_script;
    m_script = 0;
}