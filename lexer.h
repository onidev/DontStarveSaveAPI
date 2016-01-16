#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include <string>

struct Token
{
    enum Kind
    {
        Identifier,  // str
        String,      // "str"
        Float,       // 123 | 12.3
        Integer,
        Boolean,     // true | false
        Expression,
        LeftBrace,   // {
        RightBrace,  // }
        LeftBracket, // [
        RightBracket,// ]
        Equal,       // =
        Comma,       // ,
        Undefined,
        Eof
    };
    Kind value;
    size_t line;
    size_t pos;
    
    Token():value(Undefined) {}
    Token(Kind value):value(value) {}
    
    operator Kind() const { return value; }
};

class Lexer
{
    const std::string & _str;
    
    char   _last_char;
    Token  _last_token;
    size_t _cursor;
    
    std::string _identifier;
    double      _value;
    int         _value_int;
    
    size_t _currentLine;
    size_t _linePos;
    
    char  getchar();
    Token gettok();
    
public:
    Lexer(const std::string & str);
    
    Token get();
    
    const Token last() const { return _last_token; }
    const std::string identifier() const { return _identifier; }
    double value() const { return _value; }
    int valueInt() const { return _value_int; }
    
    size_t cursor() const { return _cursor; }
};


#endif // LEXER_H_INCLUDED
