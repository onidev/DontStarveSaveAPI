#ifndef TABLE_H_INCLUDED
#define TABLE_H_INCLUDED

#include "lexer.h"

#include <sstream>
#include <vector>
#include <memory>

struct TableElement;

struct Table
{
    enum Type {
        Empty,
        List,
        Float,
        Int,
        Boolean,
        Expression,
        String
    };
    
    // Indique le type de la table
    Type type;
    
    // Si juste valeur
    double      value_float = 0.f;
    int         value_int = 0;
    std::string value_string;
    
    std::vector< std::unique_ptr<TableElement> > elements;
    
    Table():type(List){}
    Table(Type type):type(type){}
    Table(double x):type(Float),value_float(x){}
    Table(int x):type(Int),value_int(x){}
    Table(bool x):type(Boolean),value_int(x){}
    Table(const std::string& str):type(String),value_string(str){}
    
    Table(Table&& table);
    Table& operator=(Table&& table);
    
    static void parse(Table& root, Lexer& lex);
    static Table parseString(const std::string& str);
    std::string asString() const;
    std::string asStringFormatted(int indent) const;
    
    Table* find(const std::string& keys); //a.b.c
};

struct TableElement
{
    int         index;
    std::string index_str;
    std::string key;
    Table       value;
};

std::vector<std::string> split(const std::string &s, char delim);

#endif // TABLE_H_INCLUDED
