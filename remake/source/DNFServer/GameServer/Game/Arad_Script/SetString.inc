void Arad_Script::SetString(int row, int col, char* value)
{
    std::strcpy(cells_ + (cols_ * row) * 0x80 + col * 0x80, value);
}