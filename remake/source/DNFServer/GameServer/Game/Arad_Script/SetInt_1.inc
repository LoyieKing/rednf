void Arad_Script::SetInt(char* row, char* col, int value)
{
    SetInt(GetIdx4Row(row), GetIdx4Col(col), value);
}