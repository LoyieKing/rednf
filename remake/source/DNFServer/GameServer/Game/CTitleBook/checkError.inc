int CTitleBook::checkError(CUser* user, ENUM_TITLE_BOOK_CATEGORY category,
                           int slot)
{
    if (!user)
    {
        return 3;
    }
    int result = 0;
    // b28d-46/214：ORIG 0x08641fac 为 switch 决策树（case 0..3 顺序）+ result 局部，
    // 每 case 只写 result 不早返；越界条件合并为 `slot<0 || slot>上限`（短路两跳同落
    // result=3 块，ORIG 0x08641fe8/0x08641ff4 实证）。上限取 ORIG 立即数（0x45/0xc7/
    // 0x77/0x3b = 源码 <0x46/<=199/<0x78/<0x3c 的等价边界）。
    switch ((int)category)
    {
    case 0:
        if ((slot < 0) || (slot > 0x45)) result = 3;
        break;
    case 1:
        if ((slot < 0) || (slot > 199)) result = 3;
        break;
    case 2:
        if ((slot < 0) || (slot > 0x77)) result = 3;
        break;
    case 3:
        if ((slot < 0) || (slot > 0x3b)) result = 3;
        break;
    default:
        result = 3;
        break;
    }
    return result;
}