int scanValue()
{
    return ScanInt(static_cast<bool*>(0));     // ORIG movl $0 占位 ok 指针
