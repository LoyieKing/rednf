#ifndef TINYXML_H_INCLUDED
#define TINYXML_H_INCLUDED

#include <stdio.h>
#include <string>
#include <istream>

typedef std::string TiXmlString;

enum TiXmlEncoding
{
    TIXML_ENCODING_UNKNOWN,
    TIXML_ENCODING_UTF8,
    TIXML_ENCODING_LEGACY
};

class TiXmlNode;
class TiXmlDocument;
class TiXmlElement;
class TiXmlComment;
class TiXmlUnknown;
class TiXmlText;
class TiXmlDeclaration;
class TiXmlAttribute;
class TiXmlAttributeSet;
class TiXmlVisitor;
struct TiXmlParsingData;

struct TiXmlEntity
{
    const char* str;
    unsigned int strLength;
    char chr;
};

class TiXmlBase
{
    friend class TiXmlNode;
    friend class TiXmlElement;
    friend class TiXmlDocument;

public:
    TiXmlBase();
    virtual ~TiXmlBase();

    static bool condenseWhiteSpace;
    static TiXmlEntity entity[5];
    static std::string errorString[16];

protected:
    int row;
    int col;
};

class TiXmlNode : public TiXmlBase
{
public:
    enum NodeType
    {
        DOCUMENT,
        ELEMENT,
        COMMENT,
        UNKNOWN,
        TEXT,
        DECLARATION,
        TYPECOUNT
    };

    TiXmlNode(NodeType type);
    virtual ~TiXmlNode();

    virtual void Print(FILE* cfile, int depth) const = 0;
    virtual bool Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual const TiXmlDocument* ToDocument() const { return 0; }
    virtual const TiXmlElement* ToElement() const { return 0; }
    virtual const TiXmlComment* ToComment() const { return 0; }
    virtual const TiXmlUnknown* ToUnknown() const { return 0; }
    virtual const TiXmlText* ToText() const { return 0; }
    virtual const TiXmlDeclaration* ToDeclaration() const { return 0; }
    virtual TiXmlDocument* ToDocument() { return 0; }
    virtual TiXmlElement* ToElement() { return 0; }
    virtual TiXmlComment* ToComment() { return 0; }
    virtual TiXmlUnknown* ToUnknown() { return 0; }
    virtual TiXmlText* ToText() { return 0; }
    virtual TiXmlDeclaration* ToDeclaration() { return 0; }
    virtual TiXmlNode* Clone() const = 0;
    virtual bool Accept(TiXmlVisitor* visitor) const = 0;
    virtual void StreamIn(std::istream& in, std::string* tag) = 0;

    const char* Value() const { return value.c_str(); }
    void SetValue(const char* _value) { value = _value; }

    const TiXmlNode* FirstChild(const char* _value) const;
    TiXmlNode* FirstChild(const char* _value)
    {
        return const_cast<TiXmlNode*>(const_cast<const TiXmlNode*>(this)->FirstChild(_value));
    }
    const TiXmlNode* FirstChild() const;
    TiXmlNode* FirstChild() { return const_cast<TiXmlNode*>(const_cast<const TiXmlNode*>(this)->FirstChild()); }

    TiXmlNode* NextSibling() { return next; }
    const TiXmlNode* NextSibling() const { return next; }
    TiXmlNode* NextSibling(const char* _value);
    const TiXmlNode* NextSibling(const char* _value) const;

    void Clear();
    bool LinkEndChild(TiXmlNode* node);

    TiXmlNode* FirstChildElement() { return FirstChild(); }

    TiXmlDocument* GetDocument() const { return document; }

public:
    TiXmlDocument* document;
    TiXmlNode* parent;
    NodeType type;
    TiXmlNode* firstChild;
    TiXmlNode* lastChild;
    std::string value;
    TiXmlNode* prev;
    TiXmlNode* next;
};

class TiXmlDocument : public TiXmlNode
{
public:
    TiXmlDocument();
    TiXmlDocument(const char* documentName);
    TiXmlDocument(const std::string& documentName);
    virtual ~TiXmlDocument();

    bool LoadFile(const char* filename, TiXmlEncoding encoding = TIXML_ENCODING_UNKNOWN);
    bool LoadFile(const std::string& filename, TiXmlEncoding encoding = TIXML_ENCODING_UNKNOWN)
    {
        return LoadFile(filename.c_str(), encoding);
    }
    bool LoadFile(FILE* file, TiXmlEncoding encoding);
    bool LoadFile();
    bool SaveFile(const char* filename) const;

    virtual void Print(FILE* cfile, int depth) const;
    virtual bool Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream& in, std::string* tag);

    void SetError(int err, const char* errorDesc, TiXmlParsingData* data, TiXmlEncoding encoding);
    bool Error() const { return error; }
    const char* ErrorDesc() const { return errorDesc.c_str(); }
    int ErrorId() const { return errorId; }

    void SetTabSize(int _tabsize) { tabSize = _tabsize; }
    int TabSize() const { return tabSize; }

protected:
    int errorId;
    int error;
    std::string errorDesc;
    int tabSize;
    int errorRow;
    int errorCol;
    bool encoded;
};

class TiXmlAttribute
{
public:
    TiXmlAttribute();
    TiXmlAttribute(const char* name, const char* value);
    virtual ~TiXmlAttribute();

    const char* Name() const { return name.c_str(); }
    const char* Value() const { return value.c_str(); }
    void SetValue(const char* _value) { value = _value; }

    TiXmlAttribute* Next() const { return next; }

    virtual void Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    void SetIntValue(int value);
    void SetDoubleValue(double value);

public:
    std::string name;
    std::string value;
    TiXmlAttribute* next;
    TiXmlAttribute* prev;
};

class TiXmlAttributeSet
{
    friend class TiXmlElement;

public:
    TiXmlAttributeSet();
    ~TiXmlAttributeSet();

    void Add(TiXmlAttribute* attribute);
    const TiXmlAttribute* Find(const char* name) const;
    TiXmlAttribute* Find(const char* name);

private:
    TiXmlAttribute* first;
    TiXmlAttribute* last;
};

class TiXmlElement : public TiXmlNode
{
public:
    TiXmlElement(const char* in_value);
    TiXmlElement(const std::string& in_value);
    virtual ~TiXmlElement();

    virtual const TiXmlElement* ToElement() const { return this; }
    virtual TiXmlElement* ToElement() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual bool Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream& in, std::string* tag);

    const char* Attribute(const char* name) const;
    bool Attribute(const char* name, int* value) const;
    void SetAttribute(const char* name, const char* value);
    void SetAttribute(const char* name, int value);
    void SetAttribute(const std::string& name, const std::string& value);
    void SetAttribute(const std::string& name, int value);
    void SetDoubleAttribute(const char* name, double value);
    void SetDoubleAttribute(const std::string& name, double value);
    void RemoveAttribute(const char* name);

private:
    TiXmlAttributeSet attributeSet;
};

class TiXmlComment : public TiXmlNode
{
public:
    TiXmlComment();
    virtual ~TiXmlComment();
    virtual const TiXmlComment* ToComment() const { return this; }
    virtual TiXmlComment* ToComment() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual bool Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream& in, std::string* tag);
};

class TiXmlUnknown : public TiXmlNode
{
public:
    TiXmlUnknown();
    virtual ~TiXmlUnknown();
    virtual const TiXmlUnknown* ToUnknown() const { return this; }
    virtual TiXmlUnknown* ToUnknown() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual bool Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream& in, std::string* tag);
};

class TiXmlText : public TiXmlNode
{
public:
    TiXmlText(const char* initValue);
    virtual ~TiXmlText();
    virtual const TiXmlText* ToText() const { return this; }
    virtual TiXmlText* ToText() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual bool Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream& in, std::string* tag);
};

class TiXmlDeclaration : public TiXmlNode
{
public:
    TiXmlDeclaration();
    virtual ~TiXmlDeclaration();
    virtual const TiXmlDeclaration* ToDeclaration() const { return this; }
    virtual TiXmlDeclaration* ToDeclaration() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual bool Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream& in, std::string* tag);
};

class TiXmlVisitor
{
public:
    virtual ~TiXmlVisitor() {}
    virtual bool VisitEnter(const TiXmlDocument&) { return true; }
    virtual bool VisitExit(const TiXmlDocument&) { return true; }
    virtual bool VisitEnter(const TiXmlElement&, const TiXmlAttribute*) { return true; }
    virtual bool VisitExit(const TiXmlElement&) { return true; }
    virtual bool Visit(const TiXmlDeclaration&) { return true; }
    virtual bool Visit(const TiXmlText&) { return true; }
    virtual bool Visit(const TiXmlComment&) { return true; }
    virtual bool Visit(const TiXmlUnknown&) { return true; }
};

struct TiXmlParsingData
{
    int row;
    int col;
    void Stamp(const char* now, TiXmlEncoding encoding);
};

#endif // TINYXML_H_INCLUDED
