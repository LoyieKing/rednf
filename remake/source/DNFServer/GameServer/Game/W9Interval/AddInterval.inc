void IntervalSet::AddInterval(const Interval& interval) // T @0x0857337a
{
    m_intervals.push_back(interval);                   // ORIG: push_back(const&)
}