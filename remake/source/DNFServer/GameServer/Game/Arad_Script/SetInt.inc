void Arad_Script::SetInt(int row, int col, int value)
{
    std::sprintf(cells_ + (cols_ * row) * 0x80 + col * 0x80, "%d", value);
}