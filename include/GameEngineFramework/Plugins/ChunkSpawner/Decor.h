#ifndef _DECORATION_LAYER__
#define _DECORATION_LAYER__

#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>

#define DECORATION_GRASS_THIN   0
#define DECORATION_GRASS        1
#define DECORATION_GRASS_THICK  2
#define DECORATION_TREE         3
#define DECORATION_ACTOR        4

class ENGINE_API Decoration {
    
public:
    
    /// Decoration types
    /// 0  = Thin grass
    /// 1  = Grass
    /// 2  = Thick grass
    /// 3  = Tree
    /// 4  = Actor
    int type;
    
    /// Decoration subtype name.
    std::string name;
    
    /// Spawn rate factor.
    unsigned int density;
    
    /// Base generation color.
    glm::vec3 colorLow;
    
    /// Fade to top generation color.
    glm::vec3 colorHigh;
    
    /// Minimum spawn height.
    float spawnHeightMinimum;
    
    /// Maximum spawn height.
    float spawnHeightMaximum;
    
    /// Minimum plant generation stack height.
    unsigned int spawnStackHeightMin;
    
    /// Maximum plant generation stack height.
    unsigned int spawnStackHeightMax;
    
    
    Decoration() : 
        type(0),
        name(""),
        density(10),
        
        spawnHeightMinimum(0),
        spawnHeightMaximum(100),
        
        spawnStackHeightMin(1),
        spawnStackHeightMax(1)
        
    {
        
        //colorLow  = Colors.green * 0.3f;
        //colorHigh = Colors.green * 0.3f;
        
    }
    
};

#endif
