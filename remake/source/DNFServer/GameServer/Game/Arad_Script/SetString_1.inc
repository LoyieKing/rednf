void Arad_Script::SetString(char* row, char* col, char* value)
{
    SetString(GetIdx4Row(row), GetIdx4Col(col), value);
