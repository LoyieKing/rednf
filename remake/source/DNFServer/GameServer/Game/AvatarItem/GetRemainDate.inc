int Avatar_Item::GetRemainDate(int tick) const
{
    // [b28c-L7] 判词：ORIG 0x82fa1f4 为 `jle 尾块`（fail 块落尾、主体 fall-through）
    // ⇒ 源形=正向包裹 if(expire>0){...} + 尾 return；否定早返形产 jg 反向布局。
    if (m_expireDate > 0)
    {
        int remain = m_expireDate - tick;
        if (remain == 0)
            return 1;
        return remain;
    }
    return m_expireDate;
}