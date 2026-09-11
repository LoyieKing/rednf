// ---- PcBangItemRentarData::getSectionData —— T 0x08915518（同名报告）-------
EventRewardSection* PcBangItemRentarData::getSectionData(int idx)
{
    // rbegin→rend：返回首个 m_field1（段上界字节）<= idx 的段。
    for (std::vector<EventRewardSection>::reverse_iterator it = m_vec0.rbegin();
         it != m_vec0.rend(); ++it)
    {
        if ((int)(unsigned)it->m_field1 <= idx)
        {
            return &(*it);
        }
    }
    return 0;
}