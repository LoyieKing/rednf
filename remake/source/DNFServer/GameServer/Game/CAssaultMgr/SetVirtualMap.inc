bool CStaticDisposition::SetVirtualMap(std::vector<MapArea>* areas) // ORIG 0x82e6588
{
    Point2d probe;
    for (int i = 0; i <= 4; ++i)
    {
        if (_CheckMatrix(areas, probe))
            return true;   // ORIG mov $0x1,%eax
        _MoveMatrix(probe);
    }
    return false;          // ORIG mov $0x0,%eax
