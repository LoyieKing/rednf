void InGameAdvertisementScript::clear()
{
    // P68 正向：ORIG 0x837330a 实参序「this 先物化→0x97→0x1」＝ 带 cast 形。
    memset((char*)this, 1, 0x97);
