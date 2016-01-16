#include <onidev.h>
#include <fstream>
#include "save.h"

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        printf("Usage:\nmapviewer savename\n");
        return 1;
    }
    
    std::string path(argv[0]);
    int p1 = path.find_last_of('\\');
    if(p1 == -1) p1 = path.find_last_of('/');
    if(p1 != -1)
        path = path.substr(0, p1 + 1);
    
    std::string file;
    for(int i=1; i<argc; i+=1) {
        file += std::string(argv[i]);
        if(i != argc - 1) file += " ";
    }
    
    std::vector< std::pair<std::string, int> > entries;
    // read entries
    {
        std::ifstream f(path + "entries.txt");
        
        if(f.is_open())
        {
            std::string line;
            while(std::getline(f, line))
            {
                std::istringstream iss(line);
                std::string str;
                int icon;
                iss >> str >> icon;
                entries.push_back( std::make_pair(str, icon) );
            }
        }
    }
    
    SaveEditor save;
    save.deserialize(file);
    
    Table * tiles_table = save.table().find("map.tiles");
    std::string tiles = base64_decode(tiles_table->value_string);
    std::cout << tiles.size() << '\n' << save.width() << 'x' << save.height() << '\n';
    std::cout << 2 * save.width() * save.height() << '\n';
    
    using namespace od;
    Window win(320, 240, "OniDev app");
    win.setSynchronization(true);
    
    TexturePacker packer(path + "sprites", true);
    metatextureAdd("icons", new MetaTexture(packer));
    metatextureFind("icons").texture().setFiltering(Texture::Linear, Texture::Linear);
    int spr_icons = spriteIndex("icons");
    
    int size = 4;
    Surface surf(save.width() * size, save.height() * size);
    surf.setTarget();
    glBegin(GL_QUADS);
    for(int i=0; i<save.width(); ++i) {
        for(int j=0; j<save.height(); ++j) {
            int p = (i + j * save.width())*2 + 9;
            unsigned char c = (unsigned char) tiles[p];
            
            if(c == 1) glColor3f(0, 0, 0);
            else if(c == 2) glColor3ub(120, 92, 61);
            else if(c == 3) glColor3ub(183, 151, 91);
            else if(c == 4) glColor3ub(252, 209, 131);
            else if(c == 5) glColor3ub(232, 171, 69);
            else if(c == 6) glColor3ub(201, 192, 77);
            else if(c == 7) glColor3ub(128, 134, 71);
            else if(c == 8) glColor3ub(136, 77, 95);
            else if(c == 10) glColor3ub(179, 140, 75); // color trop proche d'autres
            else if(c == 11) glColor3ub(187, 125, 77); // color trop proche d'autres
            else if(c == 12) glColor3ub(207, 166, 102); // color trop proche d'autres
            // cave
            else if(c == 13) glColor3ub(172, 150, 102);
            else if(c == 14) glColor3ub(95, 105, 102);
            else if(c == 15) glColor3ub(167, 160, 74);
            else if(c == 16) glColor3ub(130, 108, 75);
            else if(c == 17) glColor3ub(152, 101, 50);
            else if(c == 24) glColor3ub(171, 112, 77); /// @todo
            else if(c == 25) glColor3ub(113, 105, 62);
            
            else if(c == 30) glColor3ub(153, 66, 66);
            else if(c == 31) glColor3ub(242, 195, 105);
            // shipwrecked
            else if(c == 44) glColor3ub(203, 182, 23); // grassland
            else if(c == 45) glColor3ub(255, 200, 114); // plage
            else if(c == 46) glColor3ub(116, 117, 56); // foret tropicale
            else if(c == 52) glColor3ub(158, 164, 72); // mangrove (?)
            else if(c == 53) glColor3ub(110, 98, 87); // zone magmatique
            else if(c == 54) glColor3ub(156, 113, 60); // marais
            else if(c == 55) glColor3ub(121, 151, 92); // mer peu profonde
            else if(c == 56) glColor3ub(96, 126, 82); // mer
            else if(c == 57) glColor3ub(86, 100, 70); // mer profonde
            else if(c == 58) glColor3ub(188, 175, 103); // bordure iles
            else if(c == 59) glColor3ub(95, 195, 123); // recifs
            else if(c == 60) glColor3ub(175, 197, 118); // bordure ile avec eau recif?
            else if(c == 61) glColor3ub(206, 182, 93); // bordure ile eau mangrove
            else
            {
                glColor3f(0, 1, 0);
                printf("%d ", c);
                return 1;
            }
            
            float x=i*size, y=j*size;
            od::drawRectangleVertex(x, y, x+size, y+size);
        }
    }
    glEnd();
    
    glColor3f(1, 1, 1);
    auto drawEntities = [&](const std::string& name, int img) {
        auto entities = save.getEntities(name);
        float wid = save.width () * size;
        float hei = save.height() * size;
        
        for(auto& e: entities)
        {
            int x = (e.x * size/2.f + wid) / 2 - 8;
            int y = (e.y * size/2.f + hei) / 2 - 8;
            drawSprite(spr_icons, img, x, y);
        }
    };
    
    for(auto& e: entries)
    {
        drawEntities(e.first, e.second);
    }
    surf.resetTarget();
    
    Image im = surf.pixels();
    im.mirror();
    im.save(path + "world.png");
    
    return 0;
}

