#include "Arad_Script.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

Arad_Script::Arad_Script()
{
}

Arad_Script::Arad_Script(const char* path)
{
    const char* full = make_file_path(path);
    pathC_ = full;
}

int Arad_Script::GetRowNum()
{
    return usedRows_;
}

Arad_Script::~Arad_Script()
{
    if (cells_ != 0)
    {
        if (cells_ != 0)
            delete[] cells_;
    }
}

bool Arad_Script::Create()
{
    if (cells_ == 0 && cells_ != 0)
        delete[] cells_;
    cols_ = 0x20;
    rows_ = 0x100;
    bufBytes_ = cols_ * rows_ * 0x80;
    cells_ = new char[bufBytes_];
    if (cells_ == 0)
        return 0;
    std::memset(cells_, 0, (size_t)bufBytes_);
    usedCols_ = 0;
    usedRows_ = 0;
    colIndex_.clear();
    rowIndex_.clear();
    return 1;
}

bool Arad_Script::ExpandCol()
{
    cols_ = cols_ + 0x20;
    bufBytes_ = cols_ * rows_ * 0x80;
    char* n = new char[bufBytes_];
    if (n == 0)
        return 0;
    std::memset(n, 0, (size_t)bufBytes_);
    if (cells_ != 0)
        delete[] cells_;
    cells_ = n;
    return 1;
}

bool Arad_Script::ExpandRow()
{
    rows_ = rows_ + rows_;
    bufBytes_ = cols_ * rows_ * 0x80;
    char* n = new char[bufBytes_];
    if (n == 0)
        return 0;
    int nsz = bufBytes_;
    std::memset(n + nsz, 0, (size_t)(bufBytes_ - nsz));
    std::memcpy(n, cells_, (size_t)nsz);
    if (cells_ != 0)
        delete[] cells_;
    cells_ = n;
    return 1;
}

const char* Arad_Script::make_file_path(const char* file)
{
    static char full_path[0x200];
    std::strcpy(full_path, file);
    return full_path;
}

int Arad_Script::GetIdx4Col(char* name)
{
    std::map<std::string, int>::iterator it = colIndex_.find(std::string(name));
    if (it != colIndex_.end())
        return it->second;
    return -1;
}

int Arad_Script::GetIdx4Row(char* name)
{
    std::map<std::string, int>::iterator it = rowIndex_.find(std::string(name));
    if (it != rowIndex_.end())
        return it->second;
    return -1;
}

int Arad_Script::GetInt(int row, int col)
{
    char* local_c;
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    local_c = cells_ + a;
    return std::atoi(local_c);
}

int Arad_Script::GetInt(char* row, char* col)
{
    return GetInt(GetIdx4Row(row), GetIdx4Col(col));
}

int Arad_Script::GetInt(char* row, int col)
{
    return GetInt(GetIdx4Row(row), col);
}

int Arad_Script::GetInt(int row, char* col)
{
    return GetInt(row, GetIdx4Col(col));
}

float Arad_Script::GetFloat(int row, int col)
{
    char* local_c;
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    local_c = cells_ + a;
    return (float)std::atof(local_c);
}

float Arad_Script::GetFloat(char* row, char* col)
{
    return GetFloat(GetIdx4Row(row), GetIdx4Col(col));
}

float Arad_Script::GetFloat(char* row, int col)
{
    return GetFloat(GetIdx4Row(row), col);
}

float Arad_Script::GetFloat(int row, char* col)
{
    return GetFloat(row, GetIdx4Col(col));
}

double Arad_Script::GetDouble(int row, int col)
{
    char* local_c;
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    local_c = cells_ + a;
    return std::atof(local_c);
}

double Arad_Script::GetDouble(char* row, char* col)
{
    return GetDouble(GetIdx4Row(row), GetIdx4Col(col));
}

double Arad_Script::GetDouble(char* row, int col)
{
    return GetDouble(GetIdx4Row(row), col);
}

double Arad_Script::GetDouble(int row, char* col)
{
    return GetDouble(row, GetIdx4Col(col));
}

char* Arad_Script::GetString(int row, int col)
{
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    return cells_ + a;
}

char* Arad_Script::GetString(char* row, char* col)
{
    return GetString(GetIdx4Row(row), GetIdx4Col(col));
}

char* Arad_Script::GetString(char* row, int col)
{
    return GetString(GetIdx4Row(row), col);
}

char* Arad_Script::GetString(int row, char* col)
{
    return GetString(row, GetIdx4Col(col));
}

void Arad_Script::SetInt(int row, int col, int value)
{
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    std::sprintf(cells_ + a, "%d", value);
}

void Arad_Script::SetInt(char* row, char* col, int value)
{
    SetInt(GetIdx4Row(row), GetIdx4Col(col), value);
}

void Arad_Script::SetInt(char* row, int col, int value)
{
    SetInt(GetIdx4Row(row), col, value);
}

void Arad_Script::SetInt(int row, char* col, int value)
{
    SetInt(row, GetIdx4Col(col), value);
}

void Arad_Script::SetString(int row, int col, char* value)
{
    int a = cols_;
    a = a * row;
    a = a + col;
    a = a << 7;
    std::strcpy(cells_ + a, value);
}

void Arad_Script::SetString(char* row, char* col, char* value)
{
    SetString(GetIdx4Row(row), GetIdx4Col(col), value);
}

void Arad_Script::SetString(char* row, int col, char* value)
{
    SetString(GetIdx4Row(row), col, value);
}

void Arad_Script::SetString(int row, char* col, char* value)
{
    SetString(row, GetIdx4Col(col), value);
}

static bool parse_arad_from_getc(Arad_Script* self, int (*getc_fn)(void*), void* src,
                                 const char* section_name)
{
    char section[0x400];
    char token[0x200];
    int i;
    int sect_len;
    int match_pos;
    int ch;
    int tok_len;
    int col;

    if (!self->Create())
        return 0;

    std::memset(section, 0, sizeof(section));
    std::sprintf(section, "[%s]", section_name);
    match_pos = 0;
    sect_len = (int)std::strlen(section);

read_sect:
    ch = getc_fn(src);
    if (ch == -1)
        return 0;
    if (ch <= 0x20)
        goto read_sect;
    if (ch > 0x7e)
        goto read_sect;
    if ((int)(signed char)section[match_pos] == ch)
    {
        match_pos = match_pos + 1;
        if (match_pos == sect_len)
            goto found_sect;
        goto read_sect;
    }
    match_pos = 0;
    goto read_sect;

found_sect:
    std::memset(token, 0, 0x200);
    tok_len = 0;

read_hdr:
    ch = getc_fn(src);
    if (ch == -1)
        return 0;
    if (ch == 0x20 || ch == 9 || ch == 0xa || ch == 0xd)
    {
        if (tok_len <= 0)
            goto after_hdr_tok;
        token[tok_len] = 0;
        tok_len = 0;
        {
            char* cur = token;
            for (i = 0; token[i] != 0; ++i)
            {
                if (token[i] == '/')
                {
                    token[i] = 0;
                    self->colIndex_[std::string(cur)] = self->usedCols_;
                    cur = token + i + 1;
                    self->usedCols_ = self->usedCols_ + 1;
                    if (self->usedCols_ + 1 >= self->cols_)
                    {
                        if (!self->ExpandCol())
                            return 0;
                    }
                }
            }
            self->colIndex_[std::string(cur)] = self->usedCols_;
            self->usedCols_ = self->usedCols_ + 1;
            if (self->usedCols_ + 1 >= self->cols_)
            {
                if (!self->ExpandCol())
                    return 0;
            }
        }
    after_hdr_tok:
        if (ch == 0xa)
            goto rows;
        goto read_hdr;
    }
    token[tok_len] = (char)ch;
    tok_len = tok_len + 1;
    goto read_hdr;

rows:
    col = -1;
    tok_len = 0;
read_row:
    ch = getc_fn(src);
    if (ch == 0x20 || ch == 9 || ch == 0xa || ch == 0xd)
        goto end_tok;
    if (ch == -1)
        goto end_tok;
    token[tok_len] = (char)ch;
    tok_len = tok_len + 1;
    goto read_row;

end_tok:
    if (tok_len <= 0)
    {
        ch = -1;
        goto row_done;
    }
    token[tok_len] = 0;
    tok_len = 0;
    if (col == -1)
    {
        self->rowIndex_[std::string(token)] = self->usedRows_;
        goto next_col;
    }
    std::strcpy(self->cells_ + (self->cols_ * self->usedRows_ + col) * 0x80, token);
next_col:
    col = col + 1;
    if (col == self->usedCols_)
    {
        while (ch != -1 && ch != 0xa)
            ch = getc_fn(src);
    }
    if (ch == 0xa || ch == -1)
    {
        if (col != self->usedCols_)
            return 0;
        self->usedRows_ = self->usedRows_ + 1;
        if (self->usedRows_ + 1 >= self->rows_)
        {
            if (!self->ExpandRow())
                return 0;
        }
    }
    else
        goto read_row;

row_done:
    if (ch == 0xa || ch == -1)
        ;
    else
        goto read_row;
    if (ch == -1)
        return 1;
    goto rows;
}

static int getc_from_ptr(void* p)
{
    const char** s = (const char**)p;
    unsigned char c = (unsigned char)**s;
    *s = *s + 1;
    return (int)(signed char)c;
}

static int getc_from_file(void* p)
{
    return std::fgetc((std::FILE*)p);
}

bool Arad_Script::Load(const char* dir, const char* file)
{
    const char* p = dir;
    return parse_arad_from_getc(this, getc_from_ptr, &p, file);
}

bool Arad_Script::Load(const char* path)
{
    std::FILE* fp = std::fopen(pathC_.c_str(), "rt");
    bool ok;
    if (fp == 0)
        return 0;
    ok = parse_arad_from_getc(this, getc_from_file, fp, path);
    std::fclose(fp);
    return ok;
}
