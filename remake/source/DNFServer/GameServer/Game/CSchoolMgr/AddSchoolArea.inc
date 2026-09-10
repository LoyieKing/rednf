void CSchoolArea::AddSchoolArea(unsigned int schoolNo)
{
    int areaIdx = GetAreaIndex();
    if (areaIdx >= 0)
    {
        SetAreaMapping(schoolNo, areaIdx);
    }
