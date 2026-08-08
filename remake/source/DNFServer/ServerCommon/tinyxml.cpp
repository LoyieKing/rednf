// Minimal TinyXML reconstruction for df_auction_r (layout-faithful, 2026-08-08)
#include "tinyxml.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

static bool IsWhiteSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

FILE* TiXmlFOpen(const char* filename, const char* mode)
{
    return fopen(filename, mode);
}

int TiXmlBase::IsAlphaNum(unsigned char anyByte, TiXmlEncoding)
{
    if (anyByte < 0x7f)
    {
        return isalnum((int)anyByte);
    }
    return 1;
}

int TiXmlBase::IsAlpha(unsigned char anyByte, TiXmlEncoding)
{
    if (anyByte < 0x7f)
    {
        return isalpha((int)anyByte);
    }
    return 1;
}

bool TiXmlBase::StringEqual(const char* p, const char* tag, bool ignoreCase, TiXmlEncoding encoding)
{
    if (p == 0 || tag == 0)
    {
        return false;
    }
    if (!*p)
    {
        return false;
    }
    if (ignoreCase)
    {
        while (*tag && *p)
        {
            unsigned char c1 = (unsigned char)*p;
            unsigned char c2 = (unsigned char)*tag;
            if (c1 == c2)
            {
                ++p;
                ++tag;
                continue;
            }
            if (c1 >= 'A' && c1 <= 'Z')
            {
                c1 += 0x20;
            }
            if (c2 >= 'A' && c2 <= 'Z')
            {
                c2 += 0x20;
            }
            if (c1 != c2)
            {
                return false;
            }
            ++p;
            ++tag;
        }
        return *tag == 0;
    }
    return strcmp(p, tag) == 0;
}

void TiXmlBase::ConvertUTF32ToUTF8(unsigned long input, char* output, int* length)
{
    const unsigned long BYTE_MARK = 0x80;
    const unsigned long FIRST_BYTE_MARK[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
    if (input < 0x80)
    {
        *length = 1;
    }
    else if (input < 0x800)
    {
        *length = 2;
    }
    else if (input < 0x10000)
    {
        *length = 3;
    }
    else if (input < 0x200000)
    {
        *length = 4;
    }
    else
    {
        *length = 5;
    }
    output += *length;
    for (int i = *length; i > 1; --i)
    {
        unsigned long d = input & 0x3f;
        input >>= 6;
        *--output = (char)(d | BYTE_MARK);
    }
    *--output = (char)(input | FIRST_BYTE_MARK[*length]);
}

const char* TiXmlBase::SkipWhiteSpace(const char* p, TiXmlEncoding encoding)
{
    if (!p || !*p)
    {
        return 0;
    }
    while (p && *p)
    {
        if (encoding == TIXML_ENCODING_UTF8)
        {
            if (*p == 0xef && p[1] == 0xbb && p[2] == 0xbf)
            {
                p += 3;
                continue;
            }
            if (*p == 0xef && p[1] == 0xbf && p[2] == 0xbe)
            {
                return p;
            }
        }
        if (IsWhiteSpace(*p))
        {
            ++p;
        }
        else
        {
            break;
        }
    }
    return p;
}

void TiXmlBase::StreamWhiteSpace(std::istream* in, std::string* tag)
{
    char c = (char)in->peek();
    while (in->good() && IsWhiteSpace(c))
    {
        in->get(c);
        tag->append(1, c);
        c = (char)in->peek();
    }
}

void TiXmlBase::StreamTo(std::istream* in, int character, std::string* tag)
{
    while (in->good())
    {
        int c = in->peek();
        if (c == character)
        {
            return;
        }
        in->get();
        tag->append(1, (char)c);
    }
}

const char* TiXmlBase::ReadName(const char* p, std::string* name, TiXmlEncoding encoding)
{
    name->erase();
    if (p == 0)
    {
        return 0;
    }
    unsigned char c = (unsigned char)*p;
    if (c == 0 || (c < 0x7f && isalpha((int)c) == 0 && c != 0x5f))
    {
        return 0;
    }
    while (p && *p)
    {
        c = (unsigned char)*p;
        if (c > 0x7e || isalnum((int)c) || c == 0x5f || c == 0x2d || c == 0x2e || c == 0x3a)
        {
            name->append(1, (char)c);
            ++p;
        }
        else
        {
            break;
        }
    }
    return p;
}

const char* TiXmlBase::ReadText(const char* p, std::string* text, bool trimWhiteSpace,
                                const char* endTag, bool caseInsensitive, TiXmlEncoding encoding)
{
    text->erase();
    if (!trimWhiteSpace || !condenseWhiteSpace || !p || !*p)
    {
        const char* p2 = p;
        while (p2 && *p2)
        {
            if (StringEqual(p2, endTag, caseInsensitive, encoding))
            {
                break;
            }
            ++p2;
        }
        if (p2)
        {
            text->assign(p, (size_t)(p2 - p));
        }
        return p2;
    }
    p = SkipWhiteSpace(p, encoding);
    while (p && *p)
    {
        if (StringEqual(p, endTag, caseInsensitive, encoding))
        {
            break;
        }
        unsigned char c = (unsigned char)*p;
        if (c == 0xef)
        {
            if (p[1] == 0xbb && p[2] == 0xbf)
            {
                text->append(" ", 1);
                p += 3;
                continue;
            }
        }
        if (IsWhiteSpace(c))
        {
            char c2;
            if (text->length() && text->at(text->length() - 1) != ' ')
            {
                text->append(1, ' ');
            }
            p = SkipWhiteSpace(p, encoding);
        }
        else
        {
            text->append(1, (char)c);
            ++p;
        }
    }
    return p;
}

const char* TiXmlBase::GetEntity(const char* p, char* value, int* length, TiXmlEncoding encoding)
{
    *length = 0;
    if (p[1] == '#')
    {
        unsigned long ucs = 0;
        const char* end = p + 2;
        if (*end == 'x')
        {
            ++end;
            while (end && *end && isxdigit((int)(unsigned char)*end))
            {
                unsigned int digit = 0;
                char c = *end;
                if (c >= '0' && c <= '9')
                {
                    digit = (unsigned int)(c - '0');
                }
                else if (c >= 'a' && c <= 'f')
                {
                    digit = (unsigned int)(c - 'a' + 10);
                }
                else if (c >= 'A' && c <= 'F')
                {
                    digit = (unsigned int)(c - 'A' + 10);
                }
                ucs = ucs * 16 + digit;
                ++end;
            }
        }
        else
        {
            while (end && *end && isdigit((int)(unsigned char)*end))
            {
                ucs = ucs * 10 + (unsigned int)(*end - '0');
                ++end;
            }
        }
        if (end && *end == ';')
        {
            if (encoding == TIXML_ENCODING_UTF8)
            {
                char buf[8];
                int len = 0;
                ConvertUTF32ToUTF8(ucs, buf, &len);
                memcpy(value, buf, (size_t)len);
                *length = len;
            }
            else
            {
                *value = (char)ucs;
                *length = 1;
            }
            return end + 1;
        }
    }
    for (int i = 0; i < 5; ++i)
    {
        if (strncmp(entity[i].str, p, entity[i].strLength) == 0)
        {
            *value = entity[i].chr;
            *length = 1;
            return p + entity[i].strLength;
        }
    }
    return 0;
}

void TiXmlBase::EncodeString(const std::string& str, std::string* out)
{
    out->clear();
    for (size_t i = 0; i < str.length(); ++i)
    {
        unsigned char c = (unsigned char)str.at(i);
        switch (c)
        {
        case '<':
            out->append("&lt;", 4);
            break;
        case '>':
            out->append("&gt;", 4);
            break;
        case '&':
            out->append("&amp;", 5);
            break;
        case '\'':
            out->append("&apos;", 6);
            break;
        case '"':
            out->append("&quot;", 6);
            break;
        default:
            out->append(1, (char)c);
            break;
        }
    }
}

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

TiXmlNode* TiXmlNode::NextSibling()
{
    return next;
}

const TiXmlNode* TiXmlNode::NextSibling() const
{
    return next;
}

TiXmlDocument* TiXmlNode::GetDocument() const
{
    const TiXmlNode* node = this;
    while (node)
    {
        if (node->type == DOCUMENT)
        {
            return (TiXmlDocument*)node;
        }
        node = node->parent;
    }
    return 0;
}

TiXmlNode* TiXmlNode::InsertEndChild(const TiXmlNode& addThis)
{
    TiXmlNode* node = addThis.Clone();
    if (!node)
    {
        return 0;
    }
    if (!LinkEndChild(node))
    {
        delete node;
        return 0;
    }
    return node;
}

TiXmlNode* TiXmlNode::InsertBeforeChild(TiXmlNode* beforeThis, const TiXmlNode& addThis)
{
    if (beforeThis == 0 || beforeThis->parent != this)
    {
        return 0;
    }
    TiXmlNode* node = addThis.Clone();
    if (!node)
    {
        return 0;
    }
    node->parent = this;
    node->prev = beforeThis->prev;
    node->next = beforeThis;
    if (beforeThis->prev)
    {
        beforeThis->prev->next = node;
    }
    else
    {
        firstChild = node;
    }
    beforeThis->prev = node;
    return node;
}

TiXmlNode* TiXmlNode::InsertAfterChild(TiXmlNode* afterThis, const TiXmlNode& addThis)
{
    if (afterThis == 0 || afterThis->parent != this)
    {
        return 0;
    }
    TiXmlNode* node = addThis.Clone();
    if (!node)
    {
        return 0;
    }
    node->parent = this;
    node->prev = afterThis;
    node->next = afterThis->next;
    if (afterThis->next)
    {
        afterThis->next->prev = node;
    }
    else
    {
        lastChild = node;
    }
    afterThis->next = node;
    return node;
}

bool TiXmlNode::RemoveChild(TiXmlNode* removeThis)
{
    if (removeThis == 0 || removeThis->parent != this)
    {
        return false;
    }
    if (removeThis->prev)
    {
        removeThis->prev->next = removeThis->next;
    }
    else
    {
        firstChild = removeThis->next;
    }
    if (removeThis->next)
    {
        removeThis->next->prev = removeThis->prev;
    }
    else
    {
        lastChild = removeThis->prev;
    }
    delete removeThis;
    return true;
}

bool TiXmlNode::ReplaceChild(TiXmlNode* replaceThis, const TiXmlNode& withThis)
{
    if (replaceThis == 0 || replaceThis->parent != this)
    {
        return false;
    }
    TiXmlNode* node = withThis.Clone();
    if (!node)
    {
        return false;
    }
    node->parent = this;
    node->prev = replaceThis->prev;
    node->next = replaceThis->next;
    if (replaceThis->prev)
    {
        replaceThis->prev->next = node;
    }
    else
    {
        firstChild = node;
    }
    if (replaceThis->next)
    {
        replaceThis->next->prev = node;
    }
    else
    {
        lastChild = node;
    }
    delete replaceThis;
    return true;
}

void TiXmlNode::CopyTo(TiXmlNode* target) const
{
    target->SetValue(value.c_str());
}

const TiXmlNode* TiXmlNode::FirstChildElement() const
{
    const TiXmlNode* node = firstChild;
    while (node)
    {
        if (node->type == ELEMENT)
        {
            break;
        }
        node = node->next;
    }
    return node;
}

TiXmlNode* TiXmlNode::FirstChildElement()
{
    return const_cast<TiXmlNode*>(const_cast<const TiXmlNode*>(this)->FirstChildElement());
}

const TiXmlNode* TiXmlNode::FirstChildElement(const char* _value) const
{
    const TiXmlNode* node = firstChild;
    while (node)
    {
        if (node->type == ELEMENT && node->value == _value)
        {
            break;
        }
        node = node->next;
    }
    return node;
}

TiXmlNode* TiXmlNode::FirstChildElement(const char* _value)
{
    return const_cast<TiXmlNode*>(const_cast<const TiXmlNode*>(this)->FirstChildElement(_value));
}

const TiXmlNode* TiXmlNode::NextSiblingElement() const
{
    const TiXmlNode* node = next;
    while (node)
    {
        if (node->type == ELEMENT)
        {
            break;
        }
        node = node->next;
    }
    return node;
}

TiXmlNode* TiXmlNode::NextSiblingElement()
{
    return const_cast<TiXmlNode*>(const_cast<const TiXmlNode*>(this)->NextSiblingElement());
}

const TiXmlNode* TiXmlNode::NextSiblingElement(const char* _value) const
{
    const TiXmlNode* node = next;
    while (node)
    {
        if (node->type == ELEMENT && node->value == _value)
        {
            break;
        }
        node = node->next;
    }
    return node;
}

TiXmlNode* TiXmlNode::NextSiblingElement(const char* _value)
{
    return const_cast<TiXmlNode*>(const_cast<const TiXmlNode*>(this)->NextSiblingElement(_value));
}

const TiXmlNode* TiXmlNode::PreviousSibling(const char* _value) const
{
    const TiXmlNode* node = prev;
    while (node)
    {
        if (node->value == _value)
        {
            break;
        }
        node = node->prev;
    }
    return node;
}

const TiXmlNode* TiXmlNode::LastChild(const char* _value) const
{
    const TiXmlNode* node = lastChild;
    while (node)
    {
        if (node->value == _value)
        {
            break;
        }
        node = node->prev;
    }
    return node;
}

const TiXmlNode* TiXmlNode::IterateChildren(const TiXmlNode* previous) const
{
    if (!previous)
    {
        return FirstChild();
    }
    return previous->NextSibling();
}

const TiXmlNode* TiXmlNode::IterateChildren(const char* val, const TiXmlNode* previous) const
{
    if (!previous)
    {
        return FirstChild(val);
    }
    return previous->NextSibling(val);
}

TiXmlNode* TiXmlNode::Identify(const char* start, TiXmlEncoding encoding)
{
    if (!start || !*start)
    {
        return 0;
    }
    const char* p = SkipWhiteSpace(start, encoding);
    if (!p || !*p)
    {
        return 0;
    }
    if (*p == '<')
    {
        if (p[1] == '?')
        {
            return new TiXmlDeclaration();
        }
        if (p[1] == '!')
        {
            if (strncmp(p, "<!--", 4) == 0)
            {
                return new TiXmlComment();
            }
            return new TiXmlUnknown();
        }
        if (p[1] == '/' || IsAlpha((unsigned char)p[1], encoding))
        {
            return new TiXmlElement("");
        }
        return new TiXmlUnknown();
    }
    return new TiXmlText("");
}

const char* TiXmlNode::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return p;
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

const char* TiXmlDocument::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
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
        return 0;
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
    return s;
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

bool TiXmlDocument::LoadFile(const std::string& filename, TiXmlEncoding encoding)
{
    return LoadFile(filename.c_str(), encoding);
}

bool TiXmlDocument::LoadFile(TiXmlEncoding encoding)
{
    return LoadFile(value.c_str(), encoding);
}

bool TiXmlDocument::SaveFile() const
{
    return SaveFile(value.c_str());
}

bool TiXmlDocument::SaveFile(FILE* file) const
{
    Print(file, 0);
    return true;
}

void TiXmlDocument::CopyTo(TiXmlDocument* target) const
{
    TiXmlDocument* copy = target;
    TiXmlNode::CopyTo(copy);
    copy->errorId = errorId;
    copy->error = error;
    copy->errorDesc = errorDesc;
    copy->tabSize = tabSize;
    copy->errorRow = errorRow;
    copy->errorCol = errorCol;
    copy->encoded = encoded;
}

TiXmlDocument::TiXmlDocument(const TiXmlDocument& copy) : TiXmlNode(DOCUMENT)
{
    copy.CopyTo(this);
}

TiXmlDocument& TiXmlDocument::operator=(const TiXmlDocument& copy)
{
    Clear();
    copy.CopyTo(this);
    return *this;
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

void TiXmlDocument::StreamIn(std::istream* in, std::string* tag)
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

TiXmlAttribute* TiXmlAttribute::Previous() const
{
    return prev;
}

int TiXmlAttribute::IntValue() const
{
    return atoi(value.c_str());
}

double TiXmlAttribute::DoubleValue() const
{
    return atof(value.c_str());
}

int TiXmlAttribute::QueryIntValue(int* ival) const
{
    if (sscanf(value.c_str(), "%d", ival) == 1)
    {
        return 0;
    }
    return 0x80000001;
}

int TiXmlAttribute::QueryDoubleValue(double* dval) const
{
    if (sscanf(value.c_str(), "%lf", dval) == 1)
    {
        return 0;
    }
    return 0x80000001;
}

void TiXmlAttribute::Print(FILE* cfile, int depth) const
{
    (void)depth;
    fprintf(cfile, "%s=\"%s\"", name.c_str(), value.c_str());
}

void TiXmlAttribute::Print(FILE* cfile, int depth, std::string* str) const
{
    (void)depth;
    if (cfile)
    {
        Print(cfile, depth);
        return;
    }
    str->append(name);
    str->append("=\"");
    str->append(value);
    str->append("\"");
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

const TiXmlAttribute* TiXmlAttributeSet::Find(const std::string& name) const
{
    return Find(name.c_str());
}

TiXmlAttribute* TiXmlAttributeSet::FindOrCreate(const char* _name)
{
    TiXmlAttribute* ret = Find(_name);
    if (!ret)
    {
        ret = new TiXmlAttribute(_name, "");
        Add(ret);
    }
    return ret;
}

TiXmlAttribute* TiXmlAttributeSet::FindOrCreate(const std::string& _name)
{
    return FindOrCreate(_name.c_str());
}

void TiXmlAttributeSet::Remove(TiXmlAttribute* attribute)
{
    if (attribute == 0 || first == 0)
    {
        return;
    }
    if (attribute == first)
    {
        first = attribute->next;
    }
    else
    {
        attribute->prev->next = attribute->next;
    }
    if (attribute == last)
    {
        last = attribute->prev;
    }
    else
    {
        attribute->next->prev = attribute->prev;
    }
    delete attribute;
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

const char* TiXmlElement::Attribute(const char* name, double* value) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (!node)
    {
        return 0;
    }
    *value = node->DoubleValue();
    return node->Value();
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

void TiXmlElement::CopyTo(TiXmlElement* target) const
{
    TiXmlElement* copy = target;
    TiXmlNode::CopyTo(copy);
    for (const TiXmlAttribute* attribute = attributeSet.first; attribute;
         attribute = attribute->Next())
    {
        copy->SetAttribute(attribute->Name(), attribute->Value());
    }
}

TiXmlElement::TiXmlElement(const TiXmlElement& copy) : TiXmlNode(ELEMENT)
{
    copy.CopyTo(this);
}

TiXmlElement& TiXmlElement::operator=(const TiXmlElement& base)
{
    ClearThis();
    base.CopyTo(this);
    return *this;
}

void TiXmlElement::ClearThis()
{
    while (attributeSet.first)
    {
        TiXmlAttribute* node = attributeSet.first;
        attributeSet.first = node->next;
        delete node;
    }
    attributeSet.last = 0;
}

const char* TiXmlElement::Attribute(const std::string& name) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    return node ? node->Value() : 0;
}

bool TiXmlElement::Attribute(const std::string& name, int* value) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (!node)
    {
        return false;
    }
    *value = node->IntValue();
    return true;
}

bool TiXmlElement::Attribute(const std::string& name, double* value) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (!node)
    {
        return false;
    }
    *value = node->DoubleValue();
    return true;
}

int TiXmlElement::QueryIntAttribute(const char* name, int* value) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (!node)
    {
        return 0x80000001;
    }
    return node->QueryIntValue(value);
}

int TiXmlElement::QueryIntAttribute(const std::string& name, int* value) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (!node)
    {
        return 0x80000001;
    }
    return node->QueryIntValue(value);
}

int TiXmlElement::QueryDoubleAttribute(const char* name, double* value) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (!node)
    {
        return 0x80000001;
    }
    return node->QueryDoubleValue(value);
}

int TiXmlElement::QueryDoubleAttribute(const std::string& name, double* value) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (!node)
    {
        return 0x80000001;
    }
    return node->QueryDoubleValue(value);
}

int TiXmlElement::QueryBoolAttribute(const char* name, bool* value) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (!node)
    {
        return 0x80000001;
    }
    int ival = 0;
    int result = node->QueryIntValue(&ival);
    if (ival != 0)
    {
        *value = true;
    }
    else
    {
        *value = false;
    }
    return result;
}

int TiXmlElement::QueryUnsignedAttribute(const char* name, unsigned int* value) const
{
    const TiXmlAttribute* node = attributeSet.Find(name);
    if (!node)
    {
        return 0x80000001;
    }
    int ival = 0;
    int result = node->QueryIntValue(&ival);
    if (ival < 0)
    {
        ival = 0;
    }
    *value = (unsigned int)ival;
    return result;
}

const char* TiXmlElement::GetText() const
{
    const TiXmlNode* child = FirstChild();
    if (child == 0 || child->type != TEXT)
    {
        return 0;
    }
    return child->Value();
}

void TiXmlElement::ReadValue(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    TiXmlDocument* document = GetDocument();
    if (!p || !*p)
    {
        return;
    }
    p = SkipWhiteSpace(p, encoding);
    if (!p || !*p)
    {
        return;
    }
    while (p && *p)
    {
        TiXmlNode* node = Identify(p, encoding);
        if (node)
        {
            p = node->Parse(p, data, encoding);
            LinkEndChild(node);
        }
        else
        {
            break;
        }
        if (node->ToElement() && node->ToElement()->GetText() == 0 && *p == 0)
        {
            break;
        }
    }
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

const char* TiXmlElement::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return p;
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

void TiXmlElement::StreamIn(std::istream* in, std::string* tag)
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

const char* TiXmlComment::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return p;
}

TiXmlNode* TiXmlComment::Clone() const
{
    return new TiXmlComment();
}

bool TiXmlComment::Accept(TiXmlVisitor* visitor) const
{
    return visitor->Visit(*this);
}

void TiXmlComment::StreamIn(std::istream* in, std::string* tag)
{
    (void)in;
    (void)tag;
}

void TiXmlComment::CopyTo(TiXmlComment* target) const
{
    TiXmlComment* copy = target;
    TiXmlNode::CopyTo(copy);
}

TiXmlComment::TiXmlComment(const TiXmlComment& copy) : TiXmlNode(COMMENT)
{
    copy.CopyTo(this);
}

TiXmlComment& TiXmlComment::operator=(const TiXmlComment& base)
{
    Clear();
    base.CopyTo(this);
    return *this;
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

const char* TiXmlUnknown::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return p;
}

TiXmlNode* TiXmlUnknown::Clone() const
{
    return new TiXmlUnknown();
}

bool TiXmlUnknown::Accept(TiXmlVisitor* visitor) const
{
    return visitor->Visit(*this);
}

void TiXmlUnknown::StreamIn(std::istream* in, std::string* tag)
{
    (void)in;
    (void)tag;
}

void TiXmlUnknown::CopyTo(TiXmlUnknown* target) const
{
    TiXmlUnknown* copy = target;
    TiXmlNode::CopyTo(copy);
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

const char* TiXmlText::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return p;
}

TiXmlNode* TiXmlText::Clone() const
{
    return new TiXmlText(value.c_str());
}

bool TiXmlText::Accept(TiXmlVisitor* visitor) const
{
    return visitor->Visit(*this);
}

void TiXmlText::StreamIn(std::istream* in, std::string* tag)
{
    (void)in;
    (void)tag;
}

bool TiXmlText::Blank() const
{
    for (size_t i = 0; i < value.length(); ++i)
    {
        if (!IsWhiteSpace(value.at(i)))
        {
            return false;
        }
    }
    return true;
}

void TiXmlText::CopyTo(TiXmlText* target) const
{
    TiXmlText* copy = target;
    TiXmlNode::CopyTo(copy);
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

const char* TiXmlDeclaration::Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding)
{
    (void)p;
    (void)data;
    (void)encoding;
    return p;
}

TiXmlNode* TiXmlDeclaration::Clone() const
{
    return new TiXmlDeclaration();
}

bool TiXmlDeclaration::Accept(TiXmlVisitor* visitor) const
{
    return visitor->Visit(*this);
}

void TiXmlDeclaration::StreamIn(std::istream* in, std::string* tag)
{
    (void)in;
    (void)tag;
}

void TiXmlDeclaration::CopyTo(TiXmlDeclaration* target) const
{
    TiXmlDeclaration* copy = target;
    TiXmlNode::CopyTo(copy);
}

TiXmlDeclaration::TiXmlDeclaration(const char* _version, const char* _encoding,
                                   const char* _standalone)
    : TiXmlNode(DECLARATION)
{
    version = _version ? _version : "";
    encoding = _encoding ? _encoding : "";
    standalone = _standalone ? _standalone : "";
}

TiXmlDeclaration::TiXmlDeclaration(const std::string& _version, const std::string& _encoding,
                                   const std::string& _standalone)
    : TiXmlNode(DECLARATION)
{
    version = _version;
    encoding = _encoding;
    standalone = _standalone;
}

TiXmlDeclaration::TiXmlDeclaration(const TiXmlDeclaration& copy) : TiXmlNode(DECLARATION)
{
    copy.CopyTo(this);
}

TiXmlDeclaration& TiXmlDeclaration::operator=(const TiXmlDeclaration& copy)
{
    Clear();
    copy.CopyTo(this);
    return *this;
}

void TiXmlDeclaration::Print(FILE* cfile, int depth, std::string* str) const
{
    (void)depth;
    if (cfile)
    {
        fwrite("<?xml ", 1, 6, cfile);
    }
    if (str)
    {
        str->append("<?xml ", 6);
    }
    if (!version.empty())
    {
        if (cfile)
        {
            fprintf(cfile, "version=\"%s\" ", version.c_str());
        }
        if (str)
        {
            str->append("version=\"", 9);
            str->append(version);
            str->append("\" ", 2);
        }
    }
    if (!encoding.empty())
    {
        if (cfile)
        {
            fprintf(cfile, "encoding=\"%s\" ", encoding.c_str());
        }
        if (str)
        {
            str->append("encoding=\"", 10);
            str->append(encoding);
            str->append("\" ", 2);
        }
    }
    if (!standalone.empty())
    {
        if (cfile)
        {
            fprintf(cfile, "standalone=\"%s\" ", standalone.c_str());
        }
        if (str)
        {
            str->append("standalone=\"", 12);
            str->append(standalone);
            str->append("\" ", 2);
        }
    }
    if (cfile)
    {
        fwrite("?>", 1, 2, cfile);
    }
    if (str)
    {
        str->append("?>", 2);
    }
}

void TiXmlParsingData::Stamp(const char* now, TiXmlEncoding encoding)
{
    (void)now;
    (void)encoding;
}

// ---- TiXmlVisitor ----
TiXmlVisitor::~TiXmlVisitor()
{
}

bool TiXmlVisitor::VisitEnter(const TiXmlDocument&)
{
    return true;
}

bool TiXmlVisitor::VisitExit(const TiXmlDocument&)
{
    return true;
}

bool TiXmlVisitor::VisitEnter(const TiXmlElement&, const TiXmlAttribute*)
{
    return true;
}

bool TiXmlVisitor::VisitExit(const TiXmlElement&)
{
    return true;
}

bool TiXmlVisitor::Visit(const TiXmlDeclaration&)
{
    return true;
}

bool TiXmlVisitor::Visit(const TiXmlText&)
{
    return true;
}

bool TiXmlVisitor::Visit(const TiXmlComment&)
{
    return true;
}

bool TiXmlVisitor::Visit(const TiXmlUnknown&)
{
    return true;
}

// ---- TiXmlPrinter ----
bool TiXmlPrinter::VisitEnter(const TiXmlDocument&)
{
    return true;
}

bool TiXmlPrinter::VisitExit(const TiXmlDocument&)
{
    return true;
}

bool TiXmlPrinter::VisitEnter(const TiXmlElement& element, const TiXmlAttribute* firstAttribute)
{
    buffer.append(indent);
    buffer.append("<");
    buffer.append(element.Value());
    const TiXmlAttribute* attrib = firstAttribute;
    while (attrib)
    {
        buffer.append(" ");
        attrib->Print(0, 0, &buffer);
        attrib = attrib->Next();
    }
    if (element.FirstChild())
    {
        buffer.append(">");
        ++depth;
    }
    else
    {
        buffer.append(" />");
    }
    buffer.append(lineBreak);
    return true;
}

bool TiXmlPrinter::VisitExit(const TiXmlElement& element)
{
    if (element.FirstChild())
    {
        --depth;
        buffer.append(indent);
        buffer.append("</");
        buffer.append(element.Value());
        buffer.append(">");
        buffer.append(lineBreak);
    }
    return true;
}

bool TiXmlPrinter::Visit(const TiXmlDeclaration& declaration)
{
    declaration.Print(0, 0, &buffer);
    buffer.append(lineBreak);
    return true;
}

bool TiXmlPrinter::Visit(const TiXmlText& text)
{
    buffer.append(indent);
    if (simpleTextPrint)
    {
        buffer.append(text.Value());
    }
    else
    {
        std::string encoded;
        TiXmlBase::EncodeString(text.Value(), &encoded);
        buffer.append(encoded);
    }
    buffer.append(lineBreak);
    return true;
}

bool TiXmlPrinter::Visit(const TiXmlComment& comment)
{
    buffer.append(indent);
    buffer.append("<!--");
    buffer.append(comment.Value());
    buffer.append("-->");
    buffer.append(lineBreak);
    return true;
}

bool TiXmlPrinter::Visit(const TiXmlUnknown& unknown)
{
    buffer.append(indent);
    buffer.append(unknown.Value());
    buffer.append(lineBreak);
    return true;
}

// ---- TiXmlHandle ----
TiXmlHandle TiXmlHandle::FirstChild() const
{
    if (node)
    {
        return TiXmlHandle(node->FirstChild());
    }
    return TiXmlHandle((TiXmlNode*)0);
}

TiXmlHandle TiXmlHandle::FirstChild(const char* value) const
{
    if (node)
    {
        return TiXmlHandle(node->FirstChild(value));
    }
    return TiXmlHandle((TiXmlNode*)0);
}

TiXmlHandle TiXmlHandle::FirstChildElement() const
{
    if (node)
    {
        return TiXmlHandle(node->FirstChildElement());
    }
    return TiXmlHandle((TiXmlNode*)0);
}

TiXmlHandle TiXmlHandle::FirstChildElement(const char* value) const
{
    if (node)
    {
        return TiXmlHandle(node->FirstChildElement(value));
    }
    return TiXmlHandle((TiXmlNode*)0);
}

TiXmlHandle TiXmlHandle::Child(int count) const
{
    if (node)
    {
        TiXmlNode* child = node->FirstChild();
        for (int i = 0; child && i < count; ++i)
        {
            child = child->NextSibling();
        }
        return TiXmlHandle(child);
    }
    return TiXmlHandle((TiXmlNode*)0);
}

TiXmlHandle TiXmlHandle::Child(const char* value, int count) const
{
    if (node)
    {
        TiXmlNode* child = node->FirstChild(value);
        for (int i = 0; child && i < count; ++i)
        {
            child = child->NextSibling(value);
        }
        return TiXmlHandle(child);
    }
    return TiXmlHandle((TiXmlNode*)0);
}

TiXmlHandle TiXmlHandle::ChildElement(int count) const
{
    if (node)
    {
        TiXmlNode* child = node->FirstChildElement();
        for (int i = 0; child && i < count; ++i)
        {
            child = child->NextSiblingElement();
        }
        return TiXmlHandle(child);
    }
    return TiXmlHandle((TiXmlNode*)0);
}

TiXmlHandle TiXmlHandle::ChildElement(const char* value, int count) const
{
    if (node)
    {
        TiXmlNode* child = node->FirstChildElement(value);
        for (int i = 0; child && i < count; ++i)
        {
            child = child->NextSiblingElement(value);
        }
        return TiXmlHandle(child);
    }
    return TiXmlHandle((TiXmlNode*)0);
}

// ---- 自由运算符 ----
std::ostream& operator<<(std::ostream& out, const TiXmlNode& base)
{
    TiXmlPrinter printer;
    printer.SetIndent("    ");
    base.Accept(&printer);
    out << printer.CStr();
    return out;
}

std::string& operator<<(std::string& out, const TiXmlNode& base)
{
    TiXmlPrinter printer;
    printer.SetIndent("    ");
    base.Accept(&printer);
    out.append(printer.CStr());
    return out;
}

std::istream& operator>>(std::istream& in, TiXmlNode& base)
{
    std::string tag;
    while (in.good())
    {
        int c = in.peek();
        if (c == EOF)
        {
            break;
        }
        base.StreamIn(&in, &tag);
    }
    return in;
}
