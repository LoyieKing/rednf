bool IsOverlappedInterval(long aStart, long aSize, long bStart, long bSize) // T @0x0857302c
{
    // ORIG: (aStart+aSize > bStart) && (bStart+bSize > aStart)
    if (aStart + aSize <= bStart)
        return false;
    if (bStart + bSize <= aStart)
        return false;
    return true;
