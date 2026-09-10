void Arad_Script::SetString(int row, char* col, char* value)
{
    SetString(row, GetIdx4Col(col), value);
