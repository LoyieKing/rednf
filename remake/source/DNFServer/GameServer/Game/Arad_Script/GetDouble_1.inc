double Arad_Script::GetDouble(char* row, char* col)
{
    return GetDouble(GetIdx4Row(row), GetIdx4Col(col));
