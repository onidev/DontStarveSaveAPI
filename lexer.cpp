#include "lexer.h"
#include <cstdlib>
#include <cstdio>

Lexer::Lexer(const std::string & str):
    _str(str),
    
    _last_char(' '),
    _last_token(),
    _cursor(0),
    
    _identifier(),
    _value(0.f),
    
    _currentLine(0),
    _linePos(0)
{
}

char Lexer::getchar()
{
    if(_cursor < _str.length())
        return _str[_cursor++];
    return '\0';
}

Token Lexer::get()
{
    return _last_token = gettok();
}


Token Lexer::gettok()
{
    auto& last_char = _last_char;
    
    // Skip spaces
    while( isspace(last_char) || last_char == '\n' || last_char == '\t' )
    {
        last_char = getchar();
    }
    
    // Skip comments
    if(last_char == '/')
    {
        last_char = getchar();
        // One line comment
        if(last_char == '/')
        {
            while( last_char != '\n' && last_char != '\0' )
                last_char = getchar();
            
            if(last_char != '\0')
                return gettok();
            else
                return Token::Eof;
        }
        else
        // Multi lines comment
        if(last_char == '*')
        {
            do {
                while( (last_char = getchar()) != '*' )
                {
                    if(last_char == '\0')
                        return Token::Eof;
                }
            } while( (last_char = getchar()) != '/' );
            last_char = getchar();
            return gettok();
        }
        else
            _cursor--;
    }
    
    if(last_char == '{')
    {
        last_char = getchar();
        return Token::LeftBrace;
    }
    if(last_char == '}')
    {
        last_char = getchar();
        return Token::RightBrace;
    }
    if(last_char == '[')
    {
        last_char = getchar();
        return Token::LeftBracket;
    }
    if(last_char == ']')
    {
        last_char = getchar();
        return Token::RightBracket;
    }
    if(last_char == '=')
    {
        last_char = getchar();
        return Token::Equal;
    }
    if(last_char == ',')
    {
        last_char = getchar();
        return Token::Comma;
    }
    
    if(last_char == '"')
    {
        _identifier.clear();
            
        while((last_char = getchar()) != '"')
        {
            if(last_char == '\\')
            {
                last_char = getchar();
                if(last_char == '"')
                    _identifier += '"';
                else if(last_char == 'n')
                    _identifier += '\n';
                else
                {
                    _identifier += '\\';
                    _identifier += last_char;
                }
            }
            _identifier += last_char;
        }
        last_char = getchar();
        return Token::String;
    }
    
    // parse identifier
    // @todo ajouter boolean pour savoir s'il s'agit d'une fonction/expression lua...
    if(isalpha(last_char))
    {
        _identifier = last_char;
        while(isalnum(last_char = getchar()) || last_char == '_')
        {
            _identifier += last_char;
        }
        
        if(last_char == '(')
        {
            _identifier += last_char;
            while( (last_char = getchar()) != ')')
            {
                if(last_char == '"') {
                    _identifier += last_char;
                     while( (last_char = getchar()) != '"') {
                        _identifier += last_char;
                     }
                }
                _identifier += last_char;
            }
            _identifier += last_char;
            last_char = getchar();
            
            return Token::Expression;
        }
        
        if(_identifier == "true")
        {
            _value_int = 1;
            return Token::Boolean;
        }
        if(_identifier == "false")
        {
            _value_int = 0;
            return Token::Boolean;
        }
        return Token::Identifier;
    }
    
    if(isdigit(last_char) || last_char == '.' || last_char == '-') // + ?
    {
        bool is_float = last_char == '.';
        
        std::string n;
        n += last_char;
        while(isdigit(last_char = getchar()) || last_char == '.' || last_char == 'e' || last_char == '-') // mouai, ameliorer pour le e-007
        {
            n += last_char;
            if(last_char == '.' || last_char == 'e')
                is_float = true;
        }
        
        if(!is_float)
        {
            //printf("%s %d\n", n.c_str(), atoi(n.c_str()));
            _value_int = atoi(n.c_str());
            return Token::Integer;
        }
        
        _identifier = n;
        return Token::Expression;
    }
    return Token::Eof;
}
