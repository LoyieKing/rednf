Rectangle::Rectangle() : m_pos(0, 0), m_width(0), m_height(0) {}

// ORIG W 0x85cb492：两矩形相交四情形（任一命中返回 true）：
//   左上角点包含 / 右边界x带×上边y带 / 上边x带×下边y带 / 右边界x带×下边y带。
