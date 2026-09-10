bool isObjectType(int objectType, int code)
{
    // ORIG 0x080950e8：无帧无槽，and 后直接 cmp 参数（P0 单表达式）。
    return (objectType & code) == code;
