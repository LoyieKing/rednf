// ---- STPacketLog::Compare（ORIG 0x08481030：cmp a->m_count, b->m_count；
//      jle → return false（a<=b），否则 true）----
bool STPacketLog::Compare(const STPacketLog* a, const STPacketLog* b)
{
    if (a->m_count > b->m_count)
        return true;
    return false;
}