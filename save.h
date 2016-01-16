#ifndef SAVE_H_INCLUDED
#define SAVE_H_INCLUDED

#include <iostream>
#include <cstdio>
#include "base64.h"
#include "deflate.h"
#include "table.h"

struct Entity
{
    float x;
    float y;
    
    Table* table;
};

class SaveEditor
{
    Table _table;
    
    float _world_wid;
    float _world_hei;
    
public:
    bool deserialize(const std::string& fname, bool skip_data = false);
    bool serialize(const std::string& fname) const;
    
    bool loadFromString(const std::string& fname);
    bool saveAsString(const std::string& fname, bool formatted = false) const;
    
    float width () const { return _world_wid; }
    float height() const { return _world_hei; }
    
    const Table& table() const { return _table; }
    Table& table() { return _table; }
    
    std::vector<Entity> getEntities(const std::string& name);
};

#endif // SAVE_H_INCLUDED
