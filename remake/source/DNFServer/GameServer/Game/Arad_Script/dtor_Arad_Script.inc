Arad_Script::~Arad_Script()
{
    // 修正：ORIG D1（0x0817be03 range 内）delete[] 前带显式 null 守卫
    // （mov 0x14 reload + test + je 4 条），恢复 if 形与 ORIG 对位。
    if (cells_ != 0)
        delete[] cells_;
}