void STMonsterTypeSpawnProb::clear()
{
    // P68 正向：ORIG 0x88f2c6a 实参序「this 先物化→0x10→0x0」＝ 带 cast 形。
    memset((char*)this, 0, sizeof(STMonsterTypeSpawnProb));
