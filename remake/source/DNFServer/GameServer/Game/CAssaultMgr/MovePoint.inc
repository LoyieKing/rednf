void CExtraMatrix::MovePoint(const Point2d& delta) // ORIG 0x82e597c
{
    for (std::vector<Point2d>::iterator it = m_points.begin();
         it != m_points.end(); ++it)
    {
        // ORIG 82e5999：operator* 结果先物化临时槽 -0xc（`mov %eax,-0xc`），
        // 再 `mov -0xc(%ebp),%eax` 装载 lhs 传 operator+=（4.1 物化形）。
        // 直写 `*it += delta` 被 4.4 折成 eax 直通，少 2 条 ⇒ 具名引用中转。
        Point2d& ref = *it;
        ref += delta;
    }
}