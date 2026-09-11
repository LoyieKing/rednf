float Arad_Script::GetFloat(int row, char* col)
{
    return GetFloat(row, GetIdx4Col(col));
}