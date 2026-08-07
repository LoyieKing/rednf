// Minimal TinyXML reconstruction for df_auction_r (layout-faithful, 2026-08-08)
#include "tinyxml.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

bool TiXmlBase::condenseWhiteSpace = true;
TiXmlEntity TiXmlBase::entity[5] =
{
    { "&amp;", 5, '&' },
    { "&lt;", 4, '<' },
    { "&gt;", 4, '>' },
    { "&quot;", 6, '\"' },
    { "&apos;", 6, '\'' },
};
std::string TiXmlBase::errorString[16];

TiXmlBase::TiXmlBase()
{
    row = -1;
    col = -1;
}

TiXmlBase::~TiXmlBase()
{
}

TiXmlNode::TiXmlNode(NodeType _type)
{
    row = -1;
    col = -1;
    document = 0;
    parent = 0;
    type = _type;
    firstChild = 0;
    lastChild = 0;
    value = "";
    prev = 0;
    next = 0;
}

TiXmlNode::~TiXmlNode()
{
    TiXmlNode* node = firstChild;
    while (node != 0)
    {
        TiXmlNode* temp = node->next;
        delete node;
        node = temp;
    }
}

void TiXmlNode::Clear()
{
    TiXmlNode* node = firstChild;
    while (node != 0)
    {
        TiXmlNode* temp = node->next;
        delete node;
        node = temp;
    }
    firstChild = 0;
    lastChild = 0;
}

bool TiXmlNode::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return false;
}

const TiXmlNode* TiXmlNode::FirstChild() const
{
    return firstChild;
}

const TiXmlNode* TiXmlNode::FirstChild(const char* _value) const
{
    const TiXmlNode* node = firstChild;
    while (node != 0 && strcmp(node->value.c_str(), _value) != 0)
    {
        node = node->next;
    }
    return node;
}

TiXmlNode* TiXmlNode::NextSibling(const char* _value)
{
    TiXmlNode* node = next;
    while (node != 0 && strcmp(node->value.c_str(), _value) != 0)
    {
        node = node->next;
    }
    return node;
}

const TiXmlNode* TiXmlNode::NextSibling(const char* _value) const
{
    const TiXmlNode* node = next;
    while (node != 0 && strcmp(node->value.c_str(), _value) != 0)
    {
        node = node->next;
    }
    return node;
}

bool TiXmlNode::LinkEndChild(TiXmlNode* node)
{
    if (node == 0 || node->parent != 0)
    {
        return false;
    }
    node->parent = this;
    node->prev = lastChild;
    node->next = 0;
    if (lastChild != 0)
    {
        lastChild->next = node;
    }
    else
    {
        firstChild = node;
    }
    lastChild = node;
    node->document = document;
    return true;
}

TiXmlDocument::TiXmlDocument()
    : TiXmlNode(TiXmlNode::DOCUMENT),
      errorId(0),
      error(0),
      errorDesc("primary"),
      tabSize(4),
      errorRow(0),
      errorCol(0),
      encoded(false)
{
    document = this;
}

TiXmlDocument::TiXmlDocument(const char* documentName)
    : TiXmlNode(TiXmlNode::DOCUMENT),
      errorId(0),
      error(0),
      errorDesc("primary"),
      tabSize(4),
      errorRow(0),
      errorCol(0),
      encoded(false)
{
    document = this;
    value = documentName;
}

TiXmlDocument::TiXmlDocument(const std::string& documentName)
    : TiXmlNode(TiXmlNode::DOCUMENT),
      errorId(0),
      error(0),
      errorDesc("primary"),
      tabSize(4),
      errorRow(0),
      errorCol(0),
      encoded(false)
{
    document = this;
    value = documentName;
}

TiXmlDocument::~TiXmlDocument()
{
}

void TiXmlDocument::SetError(int err, const char* errorDesc_, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    error = err;
    errorId = err;
    (void)encoding;
    if (data != 0)
    {
        errorRow = data->row;
        errorCol = data->col;
    }
    else
    {
        errorRow = 0;
        errorCol = 0;
    }
    if (errorDesc_ != 0)
    {
        errorDesc = errorDesc_;
    }
}

static const char* skipWhitespace(const char* p)
{
    while (*p != 0 && isspace((unsigned char)*p))
    {
        p++;
    }
    return p;
}

static const char* parseAttributeString(const char* p, std::string* out)
{
    p = skipWhitespace(p);
    if (*p != '\"')
    {
        return p;
    }
    p++;
    std::string result;
    while (*p != 0 && *p != '\"')
    {
        if (*p == '&')
        {
            const char* cand = 0;
            char ch = 0;
            for (int i = 0; i < 5; i++)
            {
                if (strncmp(p, TiXmlBase::entity[i].str, TiXmlBase::entity[i].strLength) == 0)
                {
                    cand = TiXmlBase::entity[i].str;
                    ch = TiXmlBase::entity[i].chr;
                    break;
                }
            }
            if (cand != 0)
            {
                result += ch;
                p += strlen(cand);
                continue;
            }
        }
        result += *p;
        p++;
    }
    if (*p == '\"')
    {
        p++;
    }
    *out = result;
    return p;
}

static const char* parseName(const char* p, std::string* name)
{
    const char* start = p;
    while (*p != 0 && (isalnum((unsigned char)*p) || *p == '_' || *p == '-' || *p == ':'))
    {
        p++;
    }
    *name = std::string(start, p - start);
    return p;
}

static const char* parseElement(const char* p, TiXmlNode* parentNode, TiXmlDocument* doc)
{
    p = skipWhitespace(p);
    if (*p == 0)
    {
        return p;
    }
    if (*p == '<')
    {
        p++;
        if (strncmp(p, "!--", 3) == 0)
        {
            // comment
            const char* end = strstr(p + 3, "-->");
            if (end == 0)
            {
                return p - 1;
            }
            TiXmlComment* comment = new TiXmlComment();
            comment->value = std::string(p + 3, end - (p + 3));
            parentNode->LinkEndChild(comment);
            return end + 3;
        }
        if (strncmp(p, "?", 1) == 0)
        {
            // declaration
            const char* end = strstr(p, "?>");
            if (end == 0)
            {
                return p - 1;
            }
            TiXmlDeclaration* decl = new TiXmlDeclaration();
            decl->value = std::string(p, end - p);
            parentNode->LinkEndChild(decl);
            return end + 2;
        }
        if (*p == '/')
        {
            // closing tag
            const char* end = strchr(p, '>');
            if (end == 0)
            {
                return p - 1;
            }
            return end + 1;
        }
        std::string tagName;
        p = parseName(p, &tagName);
        TiXmlElement* element = new TiXmlElement(tagName);
        parentNode->LinkEndChild(element);
        // attributes
        bool selfClose = false;
        while (*p != 0 && *p != '>')
        {
            p = skipWhitespace(p);
            if (strncmp(p, "/>", 2) == 0)
            {
                selfClose = true;
                p += 2;
                break;
            }
            if (*p == '>')
            {
                p++;
                break;
            }
            std::string attrName;
            p = parseName(p, &attrName);
            p = skipWhitespace(p);
            if (*p == '=')
            {
                p++;
                std::string attrValue;
                p = parseAttributeString(p, &attrValue);
                element->SetAttribute(attrName, attrValue);
            }
            else
            {
                element->SetAttribute(attrName, "");
            }
        }
        if (!selfClose)
        {
            // parse children and text until matching close tag
            while (*p != 0)
            {
                const char* next = strchr(p, '<');
                if (next == 0)
                {
                    return p;
                }
                if (next > p)
                {
                    std::string text(p, next - p);
                    const char* t = text.c_str();
                    while (*t != 0 && isspace((unsigned char)*t))
                    {
                        t++;
                    }
                    if (*t != 0)
                    {
                        TiXmlText* textNode = new TiXmlText(text.c_str());
                        element->LinkEndChild(textNode);
                    }
                }
                if (strncmp(next, "</", 2) == 0)
                {
                    const char* close = strchr(next + 2, '>');
                    if (close == 0)
                    {
                        return next;
                    }
                    return close + 1;
                }
                p = parseElement(next, element, doc);
                if (p == next)
                {
                    return p;
                }
            }
        }
        return p;
    }
    return p + 1;
}

bool TiXmlDocument::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)encoding;
    Clear();
    errorId = 0;
    error = 0;
    errorDesc = "primary";
    errorRow = 0;
    errorCol = 0;
    if (p == 0 || *p == 0)
    {
        errorId = 1;
        error = 12;
        errorRow = -1;
        errorCol = -1;
        return false;
    }
    const char* s = p;
    if (data != 0)
    {
        errorRow = data->row;
        errorCol = data->col;
    }
    while (*s != 0)
    {
        const char* before = s;
        s = parseElement(s, this, this);
        if (s == before)
        {
            s++;
        }
    }
    return true;
}

bool TiXmlDocument::LoadFile(const char* filename, TiXmlEncoding encoding)
{
    std::string fname(filename);
    value = fname;
    FILE* fp = fopen(value.c_str(), "rb");
    if (fp == 0)
    {
        SetError(2, 0, 0, TIXML_ENCODING_UNKNOWN);
        return false;
    }
    bool result = LoadFile(fp, encoding);
    fclose(fp);
    return result;
}

bool TiXmlDocument::LoadFile()
{
    return LoadFile("tinyxml.xml", TIXML_ENCODING_UNKNOWN);
}

bool TiXmlDocument::LoadFile(FILE* fp, TiXmlEncoding encoding)
{
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (size < 0)
    {
        return false;
    }
    char* buffer = new char[size + 1];
    size_t read = fread(buffer, 1, size, fp);
    buffer[read] = 0;
    bool result = Parse(buffer, 0, encoding);
    delete[] buffer;
    return result;
}

bool TiXmlDocument::SaveFile(const char* filename) const
{
    FILE* fp = fopen(filename, "w");
    if (fp == 0)
    {
        return false;
    }
    fclose(fp);
    return true;
}

void TiXmlDocument::Print(FILE* cfile, int depth) const
{
    (void)cfile;
    (void)depth;
}

TiXmlNode* TiXmlDocument::Clone() const
{
    return new TiXmlDocument();
}

bool TiXmlDocument::Accept(TiXmlVisitor* visitor) const
{
    if (!visitor->VisitEnter(*this))
    {
        return false;
    }
    for (const TiXmlNode* node = firstChild; node != 0; node = node->next)
    {
        if (!node->Accept(visitor))
        {
            return false;
        }
    }
    return visitor->VisitExit(*this);
}

void TiXmlDocument::StreamIn(std::istream& in, std::string* tag)
{
    (void)in;
    (void)tag;
}

TiXmlAttribute::TiXmlAttribute()
    : next(0),
      prev(0)
{
}

TiXmlAttribute::TiXmlAttribute(const char* _name, const char* _value)
    : name(_name),
      value(_value),
      next(0),
      prev(0)
{
}

TiXmlAttribute::~TiXmlAttribute()
{
}

void TiXmlAttribute::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
}

void TiXmlAttribute::SetIntValue(int _value)
{
    char buf[32];
    sprintf(buf, "%d", _value);
    value = buf;
}

void TiXmlAttribute::SetDoubleValue(double _value)
{
    char buf[64];
    sprintf(buf, "%f", _value);
    value = buf;
}

TiXmlAttributeSet::TiXmlAttributeSet()
    : first(0),
      last(0)
{
}

TiXmlAttributeSet::~TiXmlAttributeSet()
{
    while (first != 0)
    {
        TiXmlAttribute* node = first;
        first = first->Next();
        delete node;
    }
    last = 0;
}

void TiXmlAttributeSet::Add(TiXmlAttribute* attribute)
{
    if (attribute == 0)
    {
        return;
    }
    if (first == 0)
    {
        first = attribute;
        last = attribute;
        attribute->prev = 0;
        attribute->next = 0;
    }
    else
    {
        attribute->prev = last;
        attribute->next = 0;
        last->next = attribute;
        last = attribute;
    }
}

const TiXmlAttribute* TiXmlAttributeSet::Find(const char* name) const
{
    const TiXmlAttribute* node = first;
    while (node != 0)
    {
        if (strcmp(node->Name(), name) == 0)
        {
            break;
        }
        node = node->Next();
    }
    return node;
}

TiXmlAttribute* TiXmlAttributeSet::Find(const char* name)
{
    TiXmlAttribute* node = first;
    while (node != 0)
    {
        if (strcmp(node->Name(), name) == 0)
        {
            break;
        }
        node = node->Next();
    }
    return node;
}

TiXmlElement::TiXmlElement(const char* in_value)
    : TiXmlNode(TiXmlNode::ELEMENT)
{
    value = in_value;
}

TiXmlElement::TiXmlElement(const std::string& in_value)
    : TiXmlNode(TiXmlNode::ELEMENT)
{
    value = in_value;
}

TiXmlElement::~TiXmlElement()
{
}

const char* TiXmlElement::Attribute(const char* name) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (node != 0)
    {
        return node->Value();
    }
    return 0;
}

bool TiXmlElement::Attribute(const char* name, int* outValue) const
{
    const char* attribute = Attribute(name);
    if (attribute == 0)
    {
        return false;
    }
    *outValue = atoi(attribute);
    return true;
}

void TiXmlElement::SetAttribute(const char* name, const char* value_)
{
    TiXmlAttribute* node = attributeSet.Find(name);
    if (node != 0)
    {
        node->SetValue(value_);
        return;
    }
    TiXmlAttribute* attribute = new TiXmlAttribute(name, value_);
    attributeSet.Add(attribute);
}

void TiXmlElement::SetAttribute(const char* name, int value_)
{
    char buf[32];
    sprintf(buf, "%d", value_);
    SetAttribute(name, buf);
}

void TiXmlElement::SetAttribute(const std::string& name, const std::string& value_)
{
    SetAttribute(name.c_str(), value_.c_str());
}

void TiXmlElement::SetAttribute(const std::string& name, int value_)
{
    SetAttribute(name.c_str(), value_);
}

void TiXmlElement::SetDoubleAttribute(const char* name, double value_)
{
    char buf[64];
    sprintf(buf, "%f", value_);
    SetAttribute(name, buf);
}

void TiXmlElement::SetDoubleAttribute(const std::string& name, double value_)
{
    SetDoubleAttribute(name.c_str(), value_);
}

void TiXmlElement::RemoveAttribute(const char* name)
{
    TiXmlAttribute* node = attributeSet.Find(name);
    if (node == 0)
    {
        return;
    }
    if (node->prev != 0)
    {
        node->prev->next = node->next;
    }
    else
    {
        attributeSet.first = node->next;
    }
    if (node->next != 0)
    {
        node->next->prev = node->prev;
    }
    else
    {
        attributeSet.last = node->prev;
    }
    delete node;
}

void TiXmlElement::Print(FILE* cfile, int depth) const
{
    (void)cfile;
    (void)depth;
}

bool TiXmlElement::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return false;
}

TiXmlNode* TiXmlElement::Clone() const
{
    return new TiXmlElement(value.c_str());
}

bool TiXmlElement::Accept(TiXmlVisitor* visitor) const
{
    if (!visitor->VisitEnter(*this, 0))
    {
        return false;
    }
    for (const TiXmlNode* node = firstChild; node != 0; node = node->next)
    {
        if (!node->Accept(visitor))
        {
            return false;
        }
    }
    return visitor->VisitExit(*this);
}

void TiXmlElement::StreamIn(std::istream& in, std::string* tag)
{
    (void)in;
    (void)tag;
}

TiXmlComment::TiXmlComment()
    : TiXmlNode(TiXmlNode::COMMENT)
{
}

TiXmlComment::~TiXmlComment()
{
}

void TiXmlComment::Print(FILE* cfile, int depth) const
{
    (void)cfile;
    (void)depth;
}

bool TiXmlComment::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return false;
}

TiXmlNode* TiXmlComment::Clone() const
{
    return new TiXmlComment();
}

bool TiXmlComment::Accept(TiXmlVisitor* visitor) const
{
    return visitor->Visit(*this);
}

void TiXmlComment::StreamIn(std::istream& in, std::string* tag)
{
    (void)in;
    (void)tag;
}

TiXmlUnknown::TiXmlUnknown()
    : TiXmlNode(TiXmlNode::UNKNOWN)
{
}

TiXmlUnknown::~TiXmlUnknown()
{
}

void TiXmlUnknown::Print(FILE* cfile, int depth) const
{
    (void)cfile;
    (void)depth;
}

bool TiXmlUnknown::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return false;
}

TiXmlNode* TiXmlUnknown::Clone() const
{
    return new TiXmlUnknown();
}

bool TiXmlUnknown::Accept(TiXmlVisitor* visitor) const
{
    return visitor->Visit(*this);
}

void TiXmlUnknown::StreamIn(std::istream& in, std::string* tag)
{
    (void)in;
    (void)tag;
}

TiXmlText::TiXmlText(const char* initValue)
    : TiXmlNode(TiXmlNode::TEXT)
{
    value = initValue;
}

TiXmlText::~TiXmlText()
{
}

void TiXmlText::Print(FILE* cfile, int depth) const
{
    (void)cfile;
    (void)depth;
}

bool TiXmlText::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return false;
}

TiXmlNode* TiXmlText::Clone() const
{
    return new TiXmlText(value.c_str());
}

bool TiXmlText::Accept(TiXmlVisitor* visitor) const
{
    return visitor->Visit(*this);
}

void TiXmlText::StreamIn(std::istream& in, std::string* tag)
{
    (void)in;
    (void)tag;
}

TiXmlDeclaration::TiXmlDeclaration()
    : TiXmlNode(TiXmlNode::DECLARATION)
{
}

TiXmlDeclaration::~TiXmlDeclaration()
{
}

void TiXmlDeclaration::Print(FILE* cfile, int depth) const
{
    (void)cfile;
    (void)depth;
}

bool TiXmlDeclaration::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return false;
}

TiXmlNode* TiXmlDeclaration::Clone() const
{
    return new TiXmlDeclaration();
}

bool TiXmlDeclaration::Accept(TiXmlVisitor* visitor) const
{
    return visitor->Visit(*this);
}

void TiXmlDeclaration::StreamIn(std::istream& in, std::string* tag)
{
    (void)in;
    (void)tag;
}

void TiXmlParsingData::Stamp(const char* now, TiXmlEncoding encoding)
{
    (void)now;
    (void)encoding;
}
