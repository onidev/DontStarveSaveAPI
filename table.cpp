#include "table.h"
#include <cstdio> // a virer a terme
#include <cstdlib> // a virer a terme

static std::string to_string(int x)
{
    char buff[30];
    sprintf(buff, "%d", x);
    return buff;
}

Table::Table(Table&& table):
    type(table.type),
    value_float(table.value_float)
{
    std::swap(value_string, table.value_string);
    std::swap(elements, table.elements);
}

Table& Table::operator=(Table&& table)
{
    type = table.type;
    value_float = table.value_float;
    std::swap(value_string, table.value_string);
    std::swap(elements, table.elements);
    return *this;
}

void Table::parse(Table& root, Lexer& lex)
{
    if(lex.last() == Token::LeftBrace)
    {
        Table new_root;
        
        while(lex.get() != Token::RightBrace)
        {
            parse(new_root, lex);
            if(lex.last() != Token::Comma) {
                if(lex.last() == Token::RightBrace)
                    break;
                else
                {
                    printf("error (%d) %d\n", (int)lex.last(), lex.cursor()); /// throw
                    exit(1);
                }
            }
        }
        
        if(root.type == Table::List)
        {
            root.elements.emplace_back( new TableElement{0, "", "", std::move(new_root)} );
        }
        else if(root.type == Table::Empty)
        {
            root = std::move(new_root);
            root.type = Table::List;
        }
    }
    else if(lex.last() == Token::Float)
    {
        if(root.type == Table::List)
        {
            root.elements.emplace_back( new TableElement{0, "", "", Table(static_cast<float>(lex.value()))} );
        }
        else if(root.type == Table::Empty)
        {
            root.value_float = lex.value();
            root.type = Table::Float;
        }
    }
    else if(lex.last() == Token::Integer)
    {
        if(root.type == Table::List)
        {
            root.elements.emplace_back( new TableElement{0, "", "", Table(static_cast<int>(lex.valueInt()))} );
        }
        else if(root.type == Table::Empty)
        {
            root.value_int = lex.valueInt();
            root.type = Table::Int;
        }
    }
    else if(lex.last() == Token::Boolean)
    {
        if(root.type == Table::List)
        {
            root.elements.emplace_back( new TableElement{0, "", "", Table(static_cast<bool>(lex.valueInt()))} );
        }
        else if(root.type == Table::Empty)
        {
            root.value_int = lex.valueInt();
            root.type = Table::Boolean;
        }
    }
    else if(lex.last() == Token::Expression)
    {
        if(root.type == Table::List)
        {
            Table table(lex.identifier());
            table.type = Table::Expression;
            root.elements.emplace_back( new TableElement{0, "", "", std::move(table)} );
        }
        else if(root.type == Table::Empty)
        {
            root.value_string = lex.identifier();
            root.type = Table::Expression;
        }
    }
    else if(lex.last() == Token::String)
    {
        if(root.type == Table::List)
        {
            root.elements.emplace_back( new TableElement{0, "", "", Table(lex.identifier())} );
        }
        else if(root.type == Table::Empty)
        {
            root.value_string = lex.identifier();
            root.type = Table::String;
        }
    }
    else if(lex.last() == Token::Identifier)
    {
        std::string key = lex.identifier();
        if(lex.get() != Token::Equal)
        {
            throw(std::runtime_error("at position " + to_string(lex.cursor()) + " '=' expected (2)"));
        }
        lex.get();
        
        if(lex.last() == Token::Float)
        {
            root.elements.emplace_back( new TableElement{0, "", key, Table(static_cast<float>(lex.value()))} );
        }
        else if(lex.last() == Token::Integer)
        {
            root.elements.emplace_back( new TableElement{0, "", key, Table(static_cast<int>(lex.valueInt()))} );
        }
        else if(lex.last() == Token::Boolean)
        {
            root.elements.emplace_back( new TableElement{0, "", key, Table(static_cast<bool>(lex.valueInt()))} );
        }
        else if(lex.last() == Token::Expression)
        {
            Table table(lex.identifier());
            table.type = Table::Expression;
            root.elements.emplace_back( new TableElement{0, "", key, std::move(table)} );
        }
        else
        {
            Table table(Empty);
            Table::parse(table, lex);
            root.elements.emplace_back( new TableElement{0, "", key, std::move(table)} );
            return;
        }
    }
    else if(lex.last() == Token::LeftBracket)
    {
        lex.get();
        
        int index = 0;
        std::string index_str;
        
        if(lex.last() == Token::Float)
            index = lex.value();
        else if(lex.last() == Token::Integer || lex.last() == Token::Boolean)
            index = lex.valueInt();
        else if(lex.last() == Token::String)
            index_str = lex.identifier();
        else
        {
            printf("(%d) ", (int)lex.last());
            throw(std::runtime_error("at position " + to_string(lex.cursor()) + " [index] expect a value"));
        }
        
        if(lex.get() != Token::RightBracket)
        {
            throw(std::runtime_error("at position " + to_string(lex.cursor()) + " ']' expected"));
        }
        if(lex.get() != Token::Equal)
        {
            throw(std::runtime_error("at position " + to_string(lex.cursor()) + " '=' expected"));
        }
        
        lex.get();
        
        if(lex.last() == Token::Float)
        {
            root.elements.emplace_back( new TableElement{index, index_str, "", Table(static_cast<float>(lex.value()))} );
        }
        else if(lex.last() == Token::Integer)
        {
            root.elements.emplace_back( new TableElement{index, index_str, "", Table(static_cast<int>(lex.valueInt()))} );
        }
        else if(lex.last() == Token::Boolean)
        {
            root.elements.emplace_back( new TableElement{index, index_str, "", Table(static_cast<bool>(lex.valueInt()))} );
        }
        else if(lex.last() == Token::Expression)
        {
            Table table(lex.identifier());
            table.type = Table::Expression;
            root.elements.emplace_back( new TableElement{index, index_str, "", std::move(table)} );
        }
        else
        {
            Table table(Empty);
            Table::parse(table, lex);
            root.elements.emplace_back( new TableElement{index, index_str, "", std::move(table)} );
            return;
        }
    }
    
    lex.get();
}

Table Table::parseString(const std::string& str)
{
    Table root(Table::Empty);    
    Lexer lex(str);
    lex.get();
    parse(root, lex);
    return root;
}

std::string Table::asString() const
{
    if(type == Float)
    {
        char buff[30];
        
        if(static_cast<int>(value_float) == value_float)
        {
            sprintf(buff, "%d", static_cast<int>(value_float));
            return buff;
        }
        else
        if(value_float < 0.1 && value_float > -0.1)
        {
            sprintf(buff, "%.13e", value_float);
            return buff;
        }
        else
        {
            if(value_float < 1.0)
            {
                sprintf(buff, "%.15f", value_float);
                std::string ret(buff);
                if(ret.size() > 16)
                    ret.resize(16);
                while(ret.back() == '0')
                    ret.pop_back();
                return ret;
            }
            else
            {
                sprintf(buff, "%.14f", value_float);
                std::string ret(buff);
                if(ret.size() > 15)
                    ret.resize(15);
                while(ret.back() == '0')
                    ret.pop_back();
                return ret;
            }
        }
    }
    else if(type == Int)
    {
        char buff[30];
        sprintf(buff, "%d", value_int);
        return buff;
    }
    else if(type == Boolean)
    {
        if(value_int == 0)
            return "false";
        return "true";
    }
    
    if(type == Expression) {
        return value_string;
    }
    if(type == String) {
        return "\"" + value_string + "\"";
    }
    
    if(type == List) {
        std::string ret;
        ret += '{';
        
        for(auto it=elements.begin(); it!=elements.end(); ++it)
        {
            auto& e = **it;
            if(!e.key.empty())
            {
                ret += e.key + '=' + e.value.asString();
            }
            else if(!e.index_str.empty())
            {
                ret += "[\"" + e.index_str + "\"]=" + e.value.asString();
            }
            else if(e.index != 0)
            {
                char buff[30];
                sprintf(buff, "[%d]=", e.index);
                ret += std::string(buff) + e.value.asString();
            }
            else
            {
                ret += e.value.asString();
            }
            
            auto it2 = it;
            if(++it2 != elements.end())
                ret += ',';
        }
        ret += '}';
        return ret;
    }
    
    return std::string();
}

std::string Table::asStringFormatted(int indent) const
{
    if(type == Float) {
        char buff[30];
        
        if(static_cast<int>(value_float) == value_float)
        {
            sprintf(buff, "%d", static_cast<int>(value_float));
            return buff;
        }
        else
        if(value_float < 0.1 && value_float > -0.1)
        {
            sprintf(buff, "%.13e", value_float);
            return buff;
        }
        else
        {
            if(value_float < 1.0)
            {
                sprintf(buff, "%.15f", value_float);
                std::string ret(buff);
                if(ret.size() > 16)
                    ret.resize(16);
                while(ret.back() == '0')
                    ret.pop_back();
                return ret;
            }
            else
            {
                sprintf(buff, "%.14f", value_float);
                std::string ret(buff);
                if(ret.size() > 15)
                    ret.resize(15);
                while(ret.back() == '0')
                    ret.pop_back();
                return ret;
            }
        }
    }
    else if(type == Int)
    {
        char buff[30];
        sprintf(buff, "%d", value_int);
        return buff;
    }
    else if(type == Boolean)
    {
        if(value_int == 0)
            return "false";
        return "true";
    }
    
    if(type == Expression) {
        return value_string;
    }
    if(type == String) {
        return "\"" + value_string + "\"";
    }
    
    if(type == List) {
        std::string ret("\n");
        
        for(int i=0; i<indent; ++i)
            ret += ' ';
        
        ret += "{\n";
        
        indent += 2;
        
        for(auto it=elements.begin(); it!=elements.end(); ++it)
        {
            for(int i=0; i<indent; ++i)
                ret += ' ';
            
            auto& e = **it;
            if(!e.key.empty())
            {
                ret += e.key + '=' + e.value.asStringFormatted(indent);
            }
            else if(!e.index_str.empty())
            {
                ret += "[\"" + e.index_str + "\"]=" + e.value.asString();
            }
            else if(e.index != 0)
            {
                char buff[30];
                sprintf(buff, "[%d]=", e.index);
                ret += std::string(buff) + e.value.asStringFormatted(indent);
            }
            else
            {
                ret += e.value.asStringFormatted(indent);
            }
            
            auto it2 = it;
            if(++it2 != elements.end())
                ret += ",\n";
            else
                ret += '\n';
        }
        
        for(int i=0; i<indent-2; ++i)
            ret += ' ';
        
        ret += "}";
        return ret;
    }
    
    return std::string();
}


Table* Table::find(const std::string& keys_)
{
    auto keys = split(keys_, '.');
    
    Table* ret = this;
    
    for(auto& key: keys)
    {
        bool find = false;
        for(auto& e: ret->elements)
        {
            if(e->key == key)
            {
                //printf("%s.", key.c_str());
                find = true;
                ret = &e->value;
                break;
            }
            //else printf("%s\n", e->key.c_str());
        }
        if(!find)
            return nullptr;
    }
    
    return ret;
}

std::vector<std::string> split(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


