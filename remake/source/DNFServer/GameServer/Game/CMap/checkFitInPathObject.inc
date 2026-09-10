bool CMap::checkFitInPathObject(int x, int y) const
{
    // b28c-36 AE：ORIG 0x0834e9a6 `lea 0x4c(%eax),%edx`（m_pathArea2 退化为
    // int(*)[2] 直传）+ `add $0x2c,%eax`（m_pathArea1）——成员声明必须是二维数组
    // int[4][2]（CMap.h）；一维 [8]+cast 在 4.4 恒 add+mov 两段（探针
    // /tmp/b28c_l36_p2.cpp f1-f7 七形全劣，f8 二维直传逐条同形）。
    return isInPathArea(x, y, m_pathArea1, m_pathArea2);
