bool CDynamicDisposition::SetVirtualMap(std::vector<MapArea>* areas) // ORIG 0x82e624c
{
    m_matrix0.RemoveCandidatePoint(areas);
    m_matrix1.RemoveCandidatePoint(areas);
    m_matrix0.ArrangeCandidatePoint();
    m_matrix1.ArrangeCandidatePoint();
    return true;           // ORIG mov $0x1,%eax
}