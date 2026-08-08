#ifndef TINYXML_H_INCLUDED
#define TINYXML_H_INCLUDED

#include <stdio.h>
#include <string>
#include <istream>
#include <ostream>

typedef std::string TiXmlString;

enum TiXmlEncoding
{
    TIXML_ENCODING_UNKNOWN,
    TIXML_ENCODING_UTF8,
    TIXML_ENCODING_LEGACY
};

const TiXmlEncoding TIXML_DEFAULT_ENCODING = TIXML_ENCODING_UNKNOWN;

enum
{
    TIXML_NO_ERROR = 0,
    TIXML_ERROR,
    TIXML_ERROR_OPENING_FILE,
    TIXML_ERROR_PARSING_ELEMENT,
    TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME,
    TIXML_ERROR_READING_ELEMENT_VALUE,
    TIXML_ERROR_READING_ATTRIBUTES,
    TIXML_ERROR_PARSING_EMPTY,
    TIXML_ERROR_READING_END_TAG,
    TIXML_ERROR_PARSING_UNKNOWN,
    TIXML_ERROR_PARSING_COMMENT,
    TIXML_ERROR_PARSING_DECLARATION,
    TIXML_ERROR_DOCUMENT_EMPTY,
    TIXML_ERROR_EMBEDDED_NULL,
    TIXML_ERROR_PARSING_CDATA,
    TIXML_ERROR_DOCUMENT_TOP_ONLY,
    TIXML_ERROR_STRING_COUNT
};

#define TIXML_UTF_LEAD_0 0xefU
#define TIXML_UTF_LEAD_1 0xbbU
#define TIXML_UTF_LEAD_2 0xbfU

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
    friend class TiXmlAttribute;

public:
    TiXmlBase();
    virtual ~TiXmlBase();

    static bool condenseWhiteSpace;
    static TiXmlEntity entity[5];
    static const char* errorString[16];
    static const int utf8ByteTable[256];
    static int IsAlphaNum(unsigned char anyByte, TiXmlEncoding encoding);
    static int IsAlpha(unsigned char anyByte, TiXmlEncoding encoding);
    static bool StringEqual(const char* p, const char* tag, bool ignoreCase, TiXmlEncoding encoding);
    static void ConvertUTF32ToUTF8(unsigned long input, char* output, int* length);
    static const char* SkipWhiteSpace(const char* p, TiXmlEncoding encoding);
    static void StreamWhiteSpace(std::istream* in, std::string* tag);
    static bool StreamTo(std::istream* in, int character, std::string* tag);
    static const char* ReadName(const char* p, std::string* name, TiXmlEncoding encoding);
    static const char* ReadText(const char* p, std::string* text, bool trimWhiteSpace,
                                const char* endTag, bool caseInsensitive, TiXmlEncoding encoding);
    static const char* GetEntity(const char* p, char* value, int* length, TiXmlEncoding encoding);
    static void EncodeString(const std::string& str, std::string* out);

protected:
    int row;
    int col;
};

FILE* TiXmlFOpen(const char* filename, const char* mode);

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
    virtual const char* Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding) = 0;
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
    virtual void StreamIn(std::istream* in, std::string* tag) = 0;

    const char* Value() const { return value.c_str(); }
    void SetValue(const char* _value) { value = _value; }

    const TiXmlNode* FirstChild(const char* _value) const;
    TiXmlNode* FirstChild(const char* _value)
    {
        return const_cast<TiXmlNode*>(const_cast<const TiXmlNode*>(this)->FirstChild(_value));
    }
    const TiXmlNode* FirstChild() const;
    TiXmlNode* FirstChild() { return const_cast<TiXmlNode*>(const_cast<const TiXmlNode*>(this)->FirstChild()); }

    TiXmlNode* NextSibling();
    const TiXmlNode* NextSibling() const;
    TiXmlNode* NextSibling(const char* _value);
    const TiXmlNode* NextSibling(const char* _value) const;

    void Clear();
    bool LinkEndChild(TiXmlNode* node);
    TiXmlNode* InsertEndChild(const TiXmlNode& addThis);
    TiXmlNode* InsertBeforeChild(TiXmlNode* beforeThis, const TiXmlNode& addThis);
    TiXmlNode* InsertAfterChild(TiXmlNode* afterThis, const TiXmlNode& addThis);
    bool RemoveChild(TiXmlNode* removeThis);
    bool ReplaceChild(TiXmlNode* replaceThis, const TiXmlNode& withThis);
    TiXmlNode* Identify(const char* start, TiXmlEncoding encoding);

    const TiXmlNode* FirstChildElement() const;
    TiXmlNode* FirstChildElement();
    const TiXmlNode* FirstChildElement(const char* _value) const;
    TiXmlNode* FirstChildElement(const char* _value);
    const TiXmlNode* NextSiblingElement() const;
    TiXmlNode* NextSiblingElement();
    const TiXmlNode* NextSiblingElement(const char* _value) const;
    TiXmlNode* NextSiblingElement(const char* _value);
    const TiXmlNode* PreviousSibling(const char* _value) const;
    const TiXmlNode* LastChild(const char* _value) const;
    const TiXmlNode* IterateChildren(const TiXmlNode* previous) const;
    const TiXmlNode* IterateChildren(const char* val, const TiXmlNode* previous) const;
    virtual void CopyTo(TiXmlNode* target) const;

    TiXmlDocument* GetDocument() const;

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
    TiXmlDocument(const TiXmlDocument& copy);
    virtual ~TiXmlDocument();
    TiXmlDocument& operator=(const TiXmlDocument& copy);

    bool LoadFile(const char* filename, TiXmlEncoding encoding = TIXML_ENCODING_UNKNOWN);
    bool LoadFile(const std::string& filename, TiXmlEncoding encoding = TIXML_ENCODING_UNKNOWN);
    bool LoadFile(TiXmlEncoding encoding);
    bool LoadFile(FILE* file, TiXmlEncoding encoding);
    bool LoadFile();
    bool SaveFile(const char* filename) const;
    bool SaveFile() const;
    bool SaveFile(FILE* file) const;

    virtual void Print(FILE* cfile, int depth) const;
    virtual const char* Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream* in, std::string* tag);
    virtual const TiXmlDocument* ToDocument() const { return this; }
    virtual TiXmlDocument* ToDocument() { return this; }
    virtual void CopyTo(TiXmlDocument* target) const;

    void SetError(int err, const char* errorDesc, TiXmlParsingData* data, TiXmlEncoding encoding);
    bool Error() const { return error; }
    const char* ErrorDesc() const { return errorDesc.c_str(); }
    int ErrorId() const { return errorId; }

    void SetTabSize(int _tabsize) { tabSize = _tabsize; }
    int TabSize() const { return tabSize; }

protected:
    int error;
    int errorId;
    std::string errorDesc;
    int tabSize;
    int errorRow;
    int errorCol;
    bool encoded;
};

class TiXmlAttribute : public TiXmlBase
{
public:
    TiXmlAttribute();
    TiXmlAttribute(const char* name, const char* value);
    virtual ~TiXmlAttribute();

    const char* Name() const { return name.c_str(); }
    const char* Value() const { return value.c_str(); }
    void SetValue(const char* _value) { value = _value; }
    void SetDocument(TiXmlDocument* doc) { document = doc; }
    TiXmlDocument* GetDocument() const { return document; }

    TiXmlAttribute* Next() const { return next; }
    TiXmlAttribute* Previous() const;
    int IntValue() const;
    double DoubleValue() const;
    int QueryIntValue(int* ival) const;
    int QueryDoubleValue(double* dval) const;
    void Print(FILE* cfile, int depth) const;
    void Print(FILE* cfile, int depth, std::string* str) const;

    virtual const char* Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    void SetIntValue(int value);
    void SetDoubleValue(double value);

public:
    std::string name;
    std::string value;
    TiXmlAttribute* next;
    TiXmlAttribute* prev;
    TiXmlDocument* document;
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
    const TiXmlAttribute* Find(const std::string& name) const;
    TiXmlAttribute* FindOrCreate(const char* _name);
    TiXmlAttribute* FindOrCreate(const std::string& _name);
    void Remove(TiXmlAttribute* attribute);

private:
    TiXmlAttribute* first;
    TiXmlAttribute* last;
};

class TiXmlElement : public TiXmlNode
{
public:
    TiXmlElement(const char* in_value);
    TiXmlElement(const std::string& in_value);
    TiXmlElement(const TiXmlElement& copy);
    virtual ~TiXmlElement();
    TiXmlElement& operator=(const TiXmlElement& base);

    virtual const TiXmlElement* ToElement() const { return this; }
    virtual TiXmlElement* ToElement() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual const char* Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream* in, std::string* tag);

    const char* Attribute(const char* name) const;
    const char* Attribute(const char* name, double* value) const;
    bool Attribute(const char* name, int* value) const;
    const char* Attribute(const std::string& name) const;
    bool Attribute(const std::string& name, int* value) const;
    bool Attribute(const std::string& name, double* value) const;
    int QueryIntAttribute(const char* name, int* value) const;
    int QueryIntAttribute(const std::string& name, int* value) const;
    int QueryDoubleAttribute(const char* name, double* value) const;
    int QueryDoubleAttribute(const std::string& name, double* value) const;
    int QueryBoolAttribute(const char* name, bool* value) const;
    int QueryUnsignedAttribute(const char* name, unsigned int* value) const;
    const char* GetText() const;
    void ClearThis();
    const char* ReadValue(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual void CopyTo(TiXmlElement* target) const;
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
    TiXmlComment(const TiXmlComment& copy);
    virtual ~TiXmlComment();
    TiXmlComment& operator=(const TiXmlComment& base);
    virtual const TiXmlComment* ToComment() const { return this; }
    virtual TiXmlComment* ToComment() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual const char* Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream* in, std::string* tag);
    virtual void CopyTo(TiXmlComment* target) const;
};

class TiXmlUnknown : public TiXmlNode
{
public:
    TiXmlUnknown();
    virtual ~TiXmlUnknown();
    virtual const TiXmlUnknown* ToUnknown() const { return this; }
    virtual TiXmlUnknown* ToUnknown() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual const char* Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream* in, std::string* tag);
    virtual void CopyTo(TiXmlUnknown* target) const;
};

class TiXmlText : public TiXmlNode
{
public:
    TiXmlText(const char* initValue);
    TiXmlText(const std::string& initValue);
    virtual ~TiXmlText();
    virtual const TiXmlText* ToText() const { return this; }
    virtual TiXmlText* ToText() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual const char* Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream* in, std::string* tag);
    bool Blank() const;
    virtual void CopyTo(TiXmlText* target) const;
    void SetCDATA(bool _cdata) { cdata = _cdata; }
    bool CDATA() const { return cdata; }

private:
    bool cdata;
};

class TiXmlDeclaration : public TiXmlNode
{
public:
    TiXmlDeclaration();
    TiXmlDeclaration(const char* version, const char* encoding, const char* standalone);
    TiXmlDeclaration(const std::string& version, const std::string& encoding,
                     const std::string& standalone);
    TiXmlDeclaration(const TiXmlDeclaration& copy);
    virtual ~TiXmlDeclaration();
    TiXmlDeclaration& operator=(const TiXmlDeclaration& copy);
    virtual const TiXmlDeclaration* ToDeclaration() const { return this; }
    virtual TiXmlDeclaration* ToDeclaration() { return this; }
    virtual void Print(FILE* cfile, int depth) const;
    virtual const char* Parse(const char* p, TiXmlParsingData* data, TiXmlEncoding encoding);
    virtual TiXmlNode* Clone() const;
    virtual bool Accept(TiXmlVisitor* visitor) const;
    virtual void StreamIn(std::istream* in, std::string* tag);
    void Print(FILE* cfile, int depth, std::string* str) const;
    virtual void CopyTo(TiXmlDeclaration* target) const;
    const char* Version() const { return version.c_str(); }
    const char* Encoding() const { return encoding.c_str(); }
    const char* Standalone() const { return standalone.c_str(); }

private:
    std::string version;      // +0x2c
    std::string encoding;     // +0x30
    std::string standalone;   // +0x34
};

class TiXmlVisitor
{
public:
    virtual ~TiXmlVisitor();
    virtual bool VisitEnter(const TiXmlDocument&);
    virtual bool VisitExit(const TiXmlDocument&);
    virtual bool VisitEnter(const TiXmlElement&, const TiXmlAttribute*);
    virtual bool VisitExit(const TiXmlElement&);
    virtual bool Visit(const TiXmlDeclaration&);
    virtual bool Visit(const TiXmlText&);
    virtual bool Visit(const TiXmlComment&);
    virtual bool Visit(const TiXmlUnknown&);
};

class TiXmlPrinter : public TiXmlVisitor
{
public:
    TiXmlPrinter() : depth(0), simpleTextPrint(false), buffer(), indent("    "), lineBreak("\n") {}
    virtual bool VisitEnter(const TiXmlDocument&);
    virtual bool VisitExit(const TiXmlDocument&);
    virtual bool VisitEnter(const TiXmlElement&, const TiXmlAttribute*);
    virtual bool VisitExit(const TiXmlElement&);
    virtual bool Visit(const TiXmlDeclaration&);
    virtual bool Visit(const TiXmlText&);
    virtual bool Visit(const TiXmlComment&);
    virtual bool Visit(const TiXmlUnknown&);
    void SetIndent(const char* _indent) { indent = _indent; }
    const char* Indent() { return indent.c_str(); }
    void SetLineBreak(const char* _lineBreak) { lineBreak = _lineBreak; }
    const char* LineBreak() { return lineBreak.c_str(); }
    void SetSimpleTextPrint(bool set) { simpleTextPrint = set; }
    const char* CStr() { return buffer.c_str(); }
    int Depth() { return depth; }
private:
    int depth;
    bool simpleTextPrint;
    std::string buffer;
    std::string indent;
    std::string lineBreak;
};

class TiXmlHandle
{
public:
    TiXmlHandle(TiXmlNode* node) { this->node = node; }
    TiXmlHandle(const TiXmlNode* node) { this->node = const_cast<TiXmlNode*>(node); }
    TiXmlHandle(const TiXmlHandle& ref) { this->node = ref.node; }
    TiXmlHandle operator=(const TiXmlHandle& ref)
    {
        this->node = ref.node;
        return *this;
    }
    TiXmlNode* ToNode() const { return node; }
    TiXmlHandle FirstChild() const;
    TiXmlHandle FirstChild(const char* value) const;
    TiXmlHandle FirstChildElement() const;
    TiXmlHandle FirstChildElement(const char* value) const;
    TiXmlHandle Child(int count) const;
    TiXmlHandle Child(const char* value, int count) const;
    TiXmlHandle ChildElement(int count) const;
    TiXmlHandle ChildElement(const char* value, int count) const;
private:
    TiXmlNode* node;
};

struct TiXmlParsingData
{
    int row;
    int col;
    const char* stamp;
    int tabsize;
    void Stamp(const char* now, TiXmlEncoding encoding);
};

std::ostream& operator<<(std::ostream& out, const TiXmlNode& base);
std::string& operator<<(std::string& out, const TiXmlNode& base);
std::istream& operator>>(std::istream& in, TiXmlNode& base);

#endif // TINYXML_H_INCLUDED
