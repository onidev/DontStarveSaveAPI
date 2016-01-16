#include "save.h"
#include <cstdlib>

bool SaveEditor::deserialize(const std::string& fname, bool skip_data)
{
    std::string encoded;
    {
        FILE * f = fopen(fname.c_str(), "rb");
        if(!f)
        {
            //throw(std::runtime_error("Unable to open the file " + fname));
            return false;
        }
        
        fseek(f, 0, SEEK_SET);
        int c;
        
        while( (c = fgetc(f)) != EOF )
        {
            encoded += static_cast<char>(c);
        }
        fclose(f);
    }
    
    encoded = encoded.substr(11);
    encoded = base64_decode(encoded);
    encoded = encoded.substr(16);
    encoded = decompress_string(encoded);
    encoded = encoded.substr(7); // "return "
    
//    {
//        FILE* f = fopen("out_raw", "wb");
//        if(!f)
//            return false;
//        
//        for(size_t i=0; i<encoded.size(); ++i)
//            fputc(encoded[i], f);
//        fclose(f);
//    }
//    return true;

    // skip base64 data
//    if(skip_data)
//    {
//        std::string converted;
//        for(size_t i=0; i<encoded.size(); ++i)
//        {
//            char& c = encoded[i];
//            if(c == '"')
//            {
//                ++i;
//                while(encoded[i] != '"') {
//                    ++i;
//                    if(i >= encoded.size())
//                        return 1;
//                }
//                converted += "\"\"";
//                continue;
//            }
//            
//            converted += c;
//        }
//        encoded.swap(converted);
//    }
    
    _table = Table::parseString(encoded);
    
    Table* t = _table.find("map.width");
    if(t) _world_wid = t->value_int;
    
    t = _table.find("map.height");
    if(t) _world_hei = t->value_int;
    
    return true;
}

bool SaveEditor::serialize(const std::string& fname) const
{
    static const unsigned char head[] = {
        1, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    std::string encoded = "return " + _table.asString();
    //printf("decompressed size %d\n", encoded.size());
    int decoded_size = encoded.size();
    encoded = std::string(head, head + 16) + compress_string(encoded);
    //printf("compressed size %d\n", encoded.size() - 16);
    int encoded_size = encoded.size() - 16;
    
    encoded[8 ] = (decoded_size >> 0 )&0xFF;
    encoded[9 ] = (decoded_size >> 8 )&0xFF;
    encoded[10] = (decoded_size >> 16)&0xFF;
    encoded[11] = (decoded_size >> 24)&0xFF;
    
    encoded[12] = (encoded_size >> 0 )&0xFF;
    encoded[13] = (encoded_size >> 8 )&0xFF;
    encoded[14] = (encoded_size >> 16)&0xFF;
    encoded[15] = (encoded_size >> 24)&0xFF;
    
    encoded = "KLEI     1D" + base64_encode(reinterpret_cast<const unsigned char*>(encoded.data()), encoded.length());
    
    {
        FILE* f = fopen(fname.c_str(), "wb");
        if(!f)
            return false;
        
        for(size_t i=0; i<encoded.size(); ++i)
            fputc(encoded[i], f);
        fclose(f);
    }
    
    return true;
}

bool SaveEditor::loadFromString(const std::string& fname)
{
    std::string encoded;
    
    {
        FILE * f = fopen(fname.c_str(), "rb");
        if(!f)
            return false;
        
        fseek(f, 0, SEEK_SET);
        int c;
        
        while( (c = fgetc(f)) != EOF )
        {
            encoded += static_cast<char>(c);
        }
        fclose(f);
    }
    
    _table = Table::parseString(encoded);
    
//    Table* hunger = _table.find("playerinfo.data.hunger.hunger");
//    
//    if(hunger)
//    {
//        printf("%s\n", hunger->value_string.c_str());
//        *hunger = Table(150.f);
//    }
    
    
    
    return true;
}

bool SaveEditor::saveAsString(const std::string& fname, bool formatted) const
{
    std::string encoded = formatted? _table.asStringFormatted(0) : _table.asString();
    
    {
        FILE* f = fopen(fname.c_str(), "wb");
        if(!f)
            return false;
        
        for(size_t i=0; i<encoded.size(); ++i)
            fputc(encoded[i], f);
        fclose(f);
    }
    
    return true;
}

std::vector<Entity> SaveEditor::getEntities(const std::string& name)
{
    std::vector<Entity> ret;
    
    Table* entities = _table.find("ents." + name);
    
    if(entities)
    {
        for(auto& e: entities->elements)
        {
            Table* x = e->value.find("x");
            Table* z = e->value.find("z");
            
            if(x && z)
            {
                float pos_x, pos_y;
                if(x->type == Table::Expression)
                    pos_x = atof( x->value_string.c_str() );
                else
                if(x->type == Table::Int)
                    pos_x = x->value_int;
                else
                    pos_x = x->value_float;
                
                if(z->type == Table::Expression)
                    pos_y = atof( z->value_string.c_str() );
                else
                if(z->type == Table::Int)
                    pos_y = z->value_int;
                else
                    pos_y = z->value_float;
                    
                //printf("%f %f\n", pos_x, pos_y);
                
                ret.push_back( Entity{pos_x, pos_y, &e->value} );
            }
        }
    }
    
    return ret;
}

