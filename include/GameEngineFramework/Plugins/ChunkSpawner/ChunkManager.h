#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/EngineSystems.h>

#include <GameEngineFramework/Plugins/ChunkSpawner/Chunk.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Perlin.h>
#include <GameEngineFramework/Plugins/ChunkSpawner/Decor.h>

class ENGINE_API WorldGeneration {
    
public:
    
    float snowCapHeight;
    
    Color waterColorLow;
    Color waterColorHigh;
    
    float waterLevel;
    
    float ambientLight;
    
    Color chunkColorLow;
    Color staticColorLow;
    Color actorColorLow;
    
    Color chunkColorHigh;
    Color staticColorHigh;
    Color actorColorHigh;
    
    std::vector<Decoration> mDecorations;
    
    /// Add a layer of decoration to the world.
    void AddWorldDecoration(Decoration& decoration);
    
    
    
    // Static plant generation
    
    unsigned int staticDensity;
    float staticHeightCutoff;
    
    // Tree generation
    
    unsigned int treeDensity;
    float treeHeightCutoff;
    float treeHeightLow;
    float treeHeightHigh;
    
    unsigned int numberOfLeaves;
    float leafSpreadArea;
    float leafSpreadHeight;
    float leafHeightOffset;
    
    // Actors
    unsigned int actorDensity;
    float actorHeightCutoff;
    
    
    WorldGeneration() :
        snowCapHeight(60.0f),
        
        waterColorLow(Colors.black),
        waterColorHigh(Colors.white),
        
        waterLevel(-21.0f),
        
        ambientLight(1.0f),
        
        chunkColorLow(Colors.black),
        staticColorLow(Colors.black),
        actorColorLow(Colors.black),
        
        chunkColorHigh(Colors.white),
        staticColorHigh(Colors.white),
        actorColorHigh(Colors.white),
        
        staticDensity(200),
        staticHeightCutoff(50),
        
        treeDensity(40),
        treeHeightCutoff(50),
        treeHeightLow(5),
        treeHeightHigh(8),
        
        numberOfLeaves(15),
        leafSpreadArea(3.0f),
        leafSpreadHeight(1.4f),
        leafHeightOffset(-0.4f),
        
        actorDensity(10),
        
        actorHeightCutoff(20.0f)
        
    {
    }
    
};


void WorldGeneration::AddWorldDecoration(Decoration& decoration) {
    
    mDecorations.push_back( decoration );
    
    return;
}



class ENGINE_API ChunkManager {
    
public:
    
    // World saves directory
    std::string worldsDirectory;
    
    // World
    std::string worldName;
    int worldSeed;
    
    // Generation parameters
    int chunkSize;
    
    float currentChunkX;
    float currentChunkZ;
    
    float renderDistance;
    float generationDistance;
    float renderDistanceStatic;
    
    unsigned int generateCounter;
    unsigned int purgeCounter;
    
    unsigned int updateChunkCounter;
    
    bool updateWorldChunks;
    bool generateWorldChunks;
    
    WorldGeneration world;
    
    
    
    /// Create a new chunk at the given position.
    Chunk* CreateChunk(float chunkX, float chunkZ);
    
    /// Destroy a chunk freeing any memory therein.
    bool DestroyChunk(Chunk* chunkPtr);
    
    /// Check if the chunk exists at a given position.
    Chunk* CheckChunk(glm::vec2 position);
    
    unsigned int GetNumberOfChunks(void);
    
    /// Prepare the chunk generator.
    void Initiate(void);
    
    /// Check all the chunks in the world.
    void Update(void);
    
    /// Generate new chunks around the player position.
    void GenerateChunks(glm::vec2 position);
    
    /// Decorate a chunk static mesh.
    void Decorate(Chunk* chunk, int chunkWorldX, int chunkWorldZ, Mesh* staticMesh);
    
    /// Destroy old chunks beyond the player render distance.
    void PurgeChunks(glm::vec2 position);
    
    /// Purge the world and clean up any allocated memory.
    void PurgeWorld(void);
    
    /// Add a perlin noise layer to the generator.
    void AddPerlinNoiseLayer(Perlin& layer);
    
    
    /// Save the world to a given directory
    bool SaveWorld(void);
    
    /// Load a world from a given directory
    bool LoadWorld(void);
    
    
    /// World generation meshes.
    SubMesh subMeshWallHorz;
    SubMesh subMeshWallVert;
    
    SubMesh subMeshGrassHorz;
    SubMesh subMeshGrassVert;
    
    SubMesh subMeshStemHorz;
    SubMesh subMeshStemVert;
    
    SubMesh subMeshTree;
    
    /// Water mesh
    Mesh* watermesh;
    Material* watermaterial;
    
    
    ChunkManager() : 
        worldsDirectory("worlds/"),
        
        worldName(""),
        worldSeed(10),
        
        chunkSize(32),
        
        currentChunkX(0),
        currentChunkZ(0),
        
        renderDistance(16),
        generationDistance(24),
        renderDistanceStatic(13),
        
        generateCounter(0),
        purgeCounter(0),
        updateChunkCounter(0),
        
        updateWorldChunks(true),
        generateWorldChunks(true)
    {}
    
private:
    
    // Chunk allocator
    PoolAllocator<Chunk> mChunkList;
    
    std::vector<Chunk*> mActiveChunks;
    std::vector<Perlin> mPerlinLayers;
    
};

Chunk* ChunkManager::CreateChunk(float chunkX, float chunkZ) {
    
    Chunk* newChunk = mChunkList.Create();
    
    newChunk->gameObject = Engine.Create<GameObject>();
    
    newChunk->gameObject->renderDistance = 10000;//renderDistanceStatic * (chunkSize / 2) * 2.0f;
    
    newChunk->gameObject->AddComponent( Engine.CreateComponentMeshRenderer( Engine.Create<Mesh>(), Engine.Create<Material>() ) );
    
    MeshRenderer* baseRenderer = newChunk->gameObject->GetComponent<MeshRenderer>();
    
    baseRenderer->mesh->isShared = false;
    
    baseRenderer->material->isShared = false;
    baseRenderer->material->shader = Engine.shaders.color;
    
    baseRenderer->material->ambient = Colors.Lerp(world.chunkColorLow, world.chunkColorHigh, world.ambientLight);
    baseRenderer->material->diffuse = Colors.Lerp(world.chunkColorLow, world.chunkColorHigh, world.ambientLight);
    
    baseRenderer->EnableFrustumCulling();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( baseRenderer, RENDER_QUEUE_GEOMETRY );
    
    //
    // Static object container
    
    GameObject* staticObjectContainer = Engine.Create<GameObject>();
    staticObjectContainer->renderDistance = 10000;//renderDistanceStatic * (chunkSize / 2) * 2.0f;
    
    Transform* transform = staticObjectContainer->GetComponent<Transform>();
    
    transform->position.x = chunkX;
    transform->position.z = chunkZ;
    
    // Static mesh
    Mesh* staticMesh = Engine.Create<Mesh>();
    
    staticMesh->isShared = false;
    
    // Static material
    Material* staticMaterial = Engine.Create<Material>();
    staticMaterial->DisableCulling();
    
    staticMaterial->shader = Engine.shaders.color;
    
    staticMaterial->ambient = Colors.Lerp(world.staticColorLow, world.staticColorHigh, world.ambientLight );
    staticMaterial->diffuse = Colors.Lerp(world.staticColorLow, world.staticColorHigh, world.ambientLight);
    
    staticObjectContainer->AddComponent( Engine.CreateComponentMeshRenderer(staticMesh, staticMaterial) );
    
    MeshRenderer* staticRenderer = staticObjectContainer->GetComponent<MeshRenderer>();
    staticRenderer->EnableFrustumCulling();
    
    newChunk->staticObjects = staticObjectContainer;
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(staticRenderer, RENDER_QUEUE_GEOMETRY);
    
    
    
    //
    // Water table generation
    //
    
    newChunk->waterObject = Engine.Create<GameObject>();
    newChunk->waterObject->renderDistance = 10000;//(renderDistance * (chunkSize / 2)) * 2.0f;
    
    newChunk->waterObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    MeshRenderer* waterRenderer = newChunk->waterObject->GetComponent<MeshRenderer>();
    waterRenderer->EnableFrustumCulling();
    
    Transform* transformWater = newChunk->waterObject->GetComponent<Transform>();
    transformWater->position.y = world.waterLevel;
    
    transformWater->scale = glm::vec3(15.5, 1.0f, 15.5);
    
    float maxWaterLayers  = 16.0f;
    float maxWaterSpacing = (1.0f / 3.0f);
    
    float layerThickness = 0.3f;
    
    // Water layers
    
    waterRenderer->mesh = Engine.Create<Mesh>();
    
    for (float c=0.0f; c <= maxWaterLayers; c += maxWaterSpacing) {
        
        Color layerColor = Colors.MakeGrayScale( -(c * (layerThickness * 16.0f)) );
        
        waterRenderer->mesh->AddPlain(0, 
                                      (c - maxWaterLayers) + maxWaterSpacing, 
                                      0, 
                                      1, 1, 
                                      layerColor);
        
    }
    
    waterRenderer->mesh->Load();
    waterRenderer->mesh->isShared = false;
    
    // Water material
    waterRenderer->material = Engine.Create<Material>();
    waterRenderer->material->shader = Engine.shaders.water;
    waterRenderer->material->isShared = false;
    
    // Blending
    waterRenderer->material->EnableBlending();
    waterRenderer->material->DisableCulling();
    
    waterRenderer->material->diffuse = Colors.Lerp(world.waterColorLow, world.waterColorHigh, world.ambientLight);
    waterRenderer->material->ambient = Colors.gray;
    
    waterRenderer->material->diffuse.a = 0.2;
    waterRenderer->material->ambient.a = 0.2;
    
    Engine.sceneMain->AddMeshRendererToSceneRoot( waterRenderer, RENDER_QUEUE_POSTGEOMETRY);
    
    return newChunk;
}

bool ChunkManager::DestroyChunk(Chunk* chunkPtr) {
    
    MeshRenderer* chunkRenderer = chunkPtr->gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* waterRenderer = chunkPtr->waterObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunkPtr->staticObjects->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( waterRenderer, RENDER_QUEUE_POSTGEOMETRY );
    
    Engine.Destroy( chunkPtr->gameObject );
    Engine.Destroy( chunkPtr->staticObjects );
    Engine.Destroy( chunkPtr->waterObject );
    
    Physics.world->destroyRigidBody( chunkPtr->rigidBody );
    
    // Destroy physics collider
    Physics.DestroyHeightFieldMap( chunkPtr->collider );
    
    // Destroy actors
    
    unsigned int numberOfActors = chunkPtr->actorList.size();
    
    for (unsigned int i=0; i < numberOfActors; i++) {
        
        GameObject* actorObject = chunkPtr->actorList[i];
        
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        
        unsigned int numberOfRenderers = actorPtr->GetNumberOfMeshRenderers();
        
        for (unsigned int r=0; r < numberOfRenderers; r++) {
            
            MeshRenderer* actorRenderer = actorPtr->GetMeshRendererAtIndex(r);
            
            Engine.sceneMain->RemoveMeshRendererFromSceneRoot( actorRenderer, RENDER_QUEUE_GEOMETRY);
            
            Renderer.DestroyMeshRenderer(actorRenderer);
            
        }
        
        Engine.Destroy( actorPtr );
        
    }
    
    chunkPtr->actorList.clear();
    
    return mChunkList.Destroy(chunkPtr);
}

Chunk* ChunkManager::CheckChunk(glm::vec2 position) {
    
    unsigned int numberOfChunks = mChunkList.Size();
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        Chunk* chunk = mChunkList[i];
        
        if (position == chunk->position) 
            return chunk;
        
    }
    
    return nullptr;
}

unsigned int ChunkManager::GetNumberOfChunks(void) {
    
    return mChunkList.Size();
}


void ChunkManager::AddPerlinNoiseLayer(Perlin& layer) {
    
    mPerlinLayers.push_back(layer);
    
    return;
}


bool ChunkManager::SaveWorld(void) {
    
    if (worldName == "") 
        return false;
    
    std::string worldPath = worldsDirectory + worldName;
    
    Directory.Create(worldPath);
    
    // Save world data
    
    std::string worldSaveData = worldName+","+Int.ToString(worldSeed)+",";
    
    Directory.Create(worldPath + "/chunks");
    
    Serializer.Serialize(worldPath + "/world", (void*)worldSaveData.data(), worldSaveData.size());
    
    // Save world chunks
    unsigned int numberOfChunks = mChunkList.Size();
    
    if (numberOfChunks == 0) 
        return true;
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        std::string chunkData = "";
        
        // Save actor genetics
        
        unsigned int numberOfActors = mChunkList[i]->actorList.size();
        
        for (unsigned int a=0; a < numberOfActors; a++) {
            
            GameObject* actorObject = mChunkList[i]->actorList[a];
            
            glm::vec3 actorPosition = actorObject->GetPosition();
            
            Actor* actorPtr = actorObject->GetComponent<Actor>();
            
            chunkData += Float.ToString(actorPosition.x) + "~";
            chunkData += Float.ToString(actorPosition.y) + "~";
            chunkData += Float.ToString(actorPosition.z) + "~";
            chunkData += AI.genomes.ExtractGenome( actorPtr );
            
            
            chunkData += "\n";
            
        }
        
        std::string chunkName = Float.ToString(mChunkList[i]->position.x) + "_" + Float.ToString(mChunkList[i]->position.y);
        
        Serializer.Serialize(worldPath + "/chunks/" + chunkName, (void*)chunkData.data(), chunkData.size());
        
    }
    
    return true;
}


bool ChunkManager::LoadWorld(void) {
    
    if (worldName == "") 
        return false;
    
    std::string worldPath = worldsDirectory + worldName;
    
    if (!Directory.CheckExists( worldPath )) {
        
        Engine.Print("World not found");
        
        return false;
    }
    
    // Load world chunks
    
    std::vector<std::string> worldChunks = Directory.List(worldPath + "/world");
    
    unsigned int numberOfChunks = worldChunks.size();
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        std::string chunkFilename = worldChunks[i];
        
        std::string chunkPath = worldPath + "/world/" + chunkFilename;
        
        std::string worldLoadData(chunkPath);
        
        int bufferSz = Serializer.GetFileSize(chunkPath);
        
        Serializer.Deserialize(worldLoadData, (void*)worldLoadData.data(), worldLoadData.size());
        
        
        
        
    }
    
    return true;
}






//
// Chunk update loop
//

glm::vec2 cameraPosition;



void ChunkManager::Update(void) {
    
    if (generateWorldChunks) 
        GenerateChunks(cameraPosition);
    
    PurgeChunks( cameraPosition );
    
    // Update world chunks
    
    unsigned int numberOfChunks = mActiveChunks.size();
    
    if (numberOfChunks == 0) 
        return;
    
    updateChunkCounter++;
    if (updateChunkCounter >= numberOfChunks) {
        updateChunkCounter=0;
        
        cameraPosition = glm::vec2(Engine.cameraController->GetPosition().x, Engine.cameraController->GetPosition().z);
    }
    
    Chunk* chunk = mActiveChunks[updateChunkCounter];
    
    MeshRenderer* chunkRenderer  = chunk->gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk->staticObjects->GetComponent<MeshRenderer>();
    MeshRenderer* waterRenderer  = chunk->waterObject->GetComponent<MeshRenderer>();
    
    // Update chunk lighting
    chunkRenderer->material->ambient = Colors.Lerp(world.chunkColorLow, world.chunkColorHigh, world.ambientLight);
    chunkRenderer->material->diffuse = Colors.Lerp(world.chunkColorLow, world.chunkColorHigh, world.ambientLight);
    
    // Update static object lighting
    staticRenderer->material->ambient = Colors.Lerp(world.staticColorLow, world.staticColorHigh, world.ambientLight);
    staticRenderer->material->diffuse = Colors.Lerp(world.staticColorLow, world.staticColorHigh, world.ambientLight);
    
    
    // Update chunk actors
    unsigned int actorCount = chunk->actorList.size();
    
    for (unsigned int a=0; a < actorCount; a++) {
        
        GameObject* actorObject = chunk->actorList[a];
        
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        
        //
        // Keep actor in its current chunk
        
        if (actorPtr->GetUserDataA() != actorPtr->GetUserDataB()) {
            
            Chunk* chunkPtr = (Chunk*)actorPtr->GetUserDataA();
            
            if (chunkPtr == nullptr) 
                continue;
            
            actorPtr->SetUserDataB( actorPtr->GetUserDataA() );
            
            for (unsigned int c=0; c < numberOfChunks; c++) {
                
                Chunk* worldChunk = mChunkList[c];
                
                unsigned int actorListSz = worldChunk->actorList.size();
                
                for (unsigned int i=0; i < actorListSz; i++) {
                    
                    if (actorObject == worldChunk->actorList[i]) {
                        
                        worldChunk->actorList.erase( worldChunk->actorList.begin() + i );
                        
                        break;
                    }
                    
                }
                
            }
            
            chunkPtr->actorList.push_back( actorObject );
            
        }
        
        
        //
        // Lighting
        
        unsigned int numberOfRenderers = actorPtr->GetNumberOfMeshRenderers();
        
        for (unsigned int z=0; z < numberOfRenderers; z++) {
            
            MeshRenderer* actorRenderer = actorPtr->GetMeshRendererAtIndex(z);
            
            //actorRenderer->material->ambient = Colors.Lerp(world.actorColorLow, world.actorColorHigh, world.ambientLight);
            //actorRenderer->material->diffuse = Colors.Lerp(world.actorColorLow, world.actorColorHigh, world.ambientLight);
            
        }
        
    }
    
    // Water lighting
    
    //waterRenderer->material->diffuse = Colors.Lerp(world.waterColorLow, world.waterColorHigh, (world.ambientLight * 0.01f) + 0.0004f);
    //waterRenderer->material->ambient = Colors.Lerp(world.waterColorLow, world.waterColorHigh, world.ambientLight * 0.001f);
    
    waterRenderer->material->diffuse.a = 0.01f;
    
    
    //
    // Update level of detail
    /*
    if ((chunk->lodHigh != nullptr) & (chunk->lodLow != nullptr)) {
        
        if (glm::distance(chunkPosition, playerPosition) < levelOfDetailDistance) {
            
            // High level of detail
            chunkRenderer->mesh = chunk.lodHigh;
            
            chunkRenderer->material->EnableCulling();
            
            transform->scale = glm::vec3(1, 1, 1);
            transform->position.y = 0.0f;
            
        } else {
            
            // Low level of detail
            chunkRenderer->mesh = chunk.lodLow;
            
            chunkRenderer->material->DisableCulling();
            
            transform->scale = glm::vec3(2.0f, 1.1f, 2.0f);
            transform->position.y = -5.0f;
            
        }
        
    }
    */
    
    return;
}







void ChunkManager::GenerateChunks(glm::vec2 position) {
    
    float noiseWidth  = 0.01f;
    float noiseHeight = 0.01f;
    float heightMul   = 70;
    
    Color colorLow;
    Color colorHigh;
    
    colorLow  = Colors.dkgreen * 0.07f;
    colorHigh = Colors.brown   * 0.07f;
    
    float cameraX = 0;
    float cameraZ = 0;
    
    if (updateWorldChunks) {
        cameraX = Math.Round(position.x / chunkSize);
        cameraZ = Math.Round(position.y / chunkSize);
    }
    
    float heightField [ chunkSize * chunkSize ];
    glm::vec3 colorField  [ chunkSize * chunkSize ];
    
    // Repeat chunk generation
    for (unsigned int i=0; i < 8; i++) {
        
        if (currentChunkX >= generationDistance) {
            
            currentChunkX = 0;
            
            if (currentChunkZ >= generationDistance) {
                
                currentChunkZ = 0;
                
            } else {
                
                currentChunkZ++;
            }
            
        } else {
            
            currentChunkX++;
        }
        
        int x = currentChunkX;
        int z = currentChunkZ;
        
        int chunkAbsX = (x + cameraX) * chunkSize;
        int chunkAbsZ = (z + cameraZ) * chunkSize;
        
        float chunkX = chunkAbsX - (((generationDistance / 2) * chunkSize) + chunkSize / 2);
        float chunkZ = chunkAbsZ - (((generationDistance / 2) * chunkSize) + chunkSize / 2);
        
        int chunkWorldX = (chunkX / chunkSize) * (chunkSize - 1);
        int chunkWorldZ = (chunkZ / chunkSize) * (chunkSize - 1);
        
        Chunk* chunk = CheckChunk(glm::vec2(chunkAbsX, chunkAbsZ));
        
        if (chunk != nullptr) 
            continue;
        
        chunk = CreateChunk(chunkWorldX, chunkWorldZ);
        
        chunk->position = glm::vec2(chunkAbsX, chunkAbsZ);
        
        chunk->gameObject->SetPosition(chunkX, 0, chunkZ);
        
        chunk->gameObject->SetUserData( (void*)chunk );
        
        Engine.SetHeightFieldValues(heightField, chunkSize, chunkSize, 0);
        Engine.SetColorFieldValues(colorField, chunkSize, chunkSize, Colors.white);
        
        Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSize, chunkSize, colorLow, colorHigh, 0.024f);
        
        
        //
        // Perlin noise layers
        
        chunk->gameObject->SetPosition(chunkWorldX, 0, chunkWorldZ);
        
        MeshRenderer* chunkRenderer = chunk->gameObject->GetComponent<MeshRenderer>();
        
        // Chunk perlin
        unsigned int numberOfLayers = mPerlinLayers.size();
        for (unsigned int l=0; l < numberOfLayers; l++) {
            
            Perlin* perlinLayer = &mPerlinLayers[l];
            
            Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSize, chunkSize, 
                                                perlinLayer->noiseWidth, 
                                                perlinLayer->noiseHeight, 
                                                perlinLayer->heightMultuplier, 
                                                chunkWorldX, chunkWorldZ, worldSeed);
            
        }
        
        //
        // Biome test
        
        //Engine.SetColorFieldFromPerlinNoise(colorField, chunkSize, chunkSize, 0.01, 0.01, 0.4, Colors.blue, Colors.red, chunkX, chunkZ);
        
        Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSize, chunkSize, colorLow, colorHigh, 0.024f);
        
        //
        // Snow cap
        
        Engine.AddColorFieldSnowCap(colorField, heightField, chunkSize, chunkSize, Colors.white, world.snowCapHeight, 7.0f);
        
        //
        // Water table
        
        Engine.AddColorFieldWaterTable(colorField, heightField, chunkSize, chunkSize, world.waterColorHigh, world.waterLevel, 0.1f, world.waterLevel);
        
        Transform* transformWater = chunk->waterObject->GetComponent<Transform>();
        transformWater->position.x = chunkWorldX;
        transformWater->position.z = chunkWorldZ;
        
        
        
        
        //
        // Finalize chunk mesh
        
        Engine.AddHeightFieldToMesh(chunkRenderer->mesh, heightField, colorField, chunkSize, chunkSize, 0, 0, 1, 1);
        chunkRenderer->mesh->Load();
        
        
        
        
        // Generate rigid body
        
        RigidBody* chunkBody = Physics.world->createRigidBody( rp3d::Transform::identity() );
        chunk->rigidBody = chunkBody;
        
        chunkBody->setAngularLockAxisFactor( rp3d::Vector3(0, 0, 0) );
        chunkBody->setLinearLockAxisFactor( rp3d::Vector3(0, 0, 0) );
        chunkBody->setType(rp3d::BodyType::STATIC);
        
        rp3d::Transform bodyTransform = rp3d::Transform::identity();
        bodyTransform.setPosition( rp3d::Vector3(chunkWorldX, 0, chunkWorldZ) );
        chunkBody->setTransform(bodyTransform);
        
        // Generate a height field collider
        MeshCollider*  meshCollider = Physics.CreateHeightFieldMap(heightField, chunkSize, chunkSize);
        chunk->collider = meshCollider;
        
        rp3d::Collider* bodyCollider = chunkBody->addCollider( meshCollider->heightFieldShape, rp3d::Transform::identity() );
        bodyCollider->setUserData( (void*)chunk->gameObject );
        bodyCollider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
        bodyCollider->setCollideWithMaskBits((unsigned short)CollisionMask::Entity);
        
        chunk->bodyCollider = bodyCollider;
        
        
        
        //
        // Generate world decorations and actors
        //
        
        Mesh* staticMesh = chunk->staticObjects->GetComponent<MeshRenderer>()->mesh;
        
        Decorate(chunk, chunkWorldX, chunkWorldZ, staticMesh);
        
        staticMesh->Load();
        
        mActiveChunks.push_back(chunk);
        
    }
    
    return;
}



void DecodeGenome(Decoration& decor, Actor* actorPtr) {
    
    if (decor.name == "Sheep")  {AI.genomes.Sheep( actorPtr );}
    if (decor.name == "Bear")   {AI.genomes.Bear( actorPtr );}
    
}

void ChunkManager::Decorate(Chunk* chunk, int chunkWorldX, int chunkWorldZ, Mesh* staticMesh) {
    
    if (world.mDecorations.size() > 0) {
        
        for (int xx=0; xx < chunkSize-1; xx++) {
            
            for (int zz=0; zz < chunkSize-1; zz++) {
                
                int xp = xx - (chunkSize / 2);
                int zp = zz - (chunkSize / 2);
                
                int staticX = (chunkWorldX - xp);
                int staticZ = (chunkWorldZ - zp);
                
                glm::vec3 from(staticX, 0, staticZ);
                glm::vec3 direction(0, -1, 0);
                
                Hit hit;
                
                float distance = 2000;
                float height = 0;
                
                if (Physics.Raycast(from, direction, distance, hit, LayerMask::Ground)) 
                    height = hit.point.y;
                
                if (height > world.staticHeightCutoff) 
                    continue;
                
                if (height == 0) 
                    continue;
                
                unsigned int decorIndex = Random.Range(0, world.mDecorations.size());
                
                Decoration decor = world.mDecorations[ decorIndex ];
                
                // Spawn density
                if (Random.Range(0, 10000) > decor.density) 
                    continue;
                
                // World spawn height range
                if ((height > decor.spawnHeightMaximum) | 
                    (height < decor.spawnHeightMinimum)) 
                    continue;
                
                // Stack height
                unsigned int stackHeight = Random.Range((float)decor.spawnStackHeightMin, (float)decor.spawnStackHeightMax);
                
                switch (decor.type) {
                    
                    // Thin grass
                    case 0: {
                        
                        for (unsigned int c=0; c < stackHeight; c++) {
                            
                            staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshStemHorz, false);
                            staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshStemVert, false);
                            
                            unsigned int index = staticMesh->GetSubMeshCount() - 1;
                            
                            Color finalColor;
                            finalColor = Colors.green * 0.018f;
                            
                            finalColor += Colors.Make(Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f,
                                                    Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f,
                                                    Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f);
                            
                            staticMesh->ChangeSubMeshColor(index, finalColor);
                            staticMesh->ChangeSubMeshColor(index-1, finalColor);
                            
                        }
                        
                        continue;
                    }
                    
                    // Grass
                    case 1: {
                        
                        for (unsigned int c=0; c < stackHeight; c++) {
                            
                            staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshGrassHorz, false);
                            staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshGrassVert, false);
                            
                            unsigned int index = staticMesh->GetSubMeshCount() - 1;
                            
                            Color finalColor;
                            finalColor = Colors.green * 0.04f;
                            
                            staticMesh->ChangeSubMeshColor(index, finalColor);
                            staticMesh->ChangeSubMeshColor(index-1, finalColor);
                            
                        }
                        
                        continue;
                    }
                    
                    // Thicker grass
                    case 2: {
                        
                        staticMesh->AddSubMesh(-xp, height, -zp, subMeshWallHorz, false);
                        staticMesh->AddSubMesh(-xp, height, -zp, subMeshWallVert, false);
                        
                        unsigned int index = staticMesh->GetSubMeshCount() - 1;
                        
                        Color finalColor;
                        finalColor = Colors.green * 0.05f;
                        
                        if (Random.Range(0, 100) < 20) finalColor = Colors.yellow * 0.05f;
                        if (Random.Range(0, 100) < 20) finalColor = Colors.orange * 0.01f;
                        
                        staticMesh->ChangeSubMeshColor(index, finalColor);
                        staticMesh->ChangeSubMeshColor(index-1, finalColor);
                        
                        continue;
                    }
                    
                    // Trees
                    case 3: {
                        
                        // Tree logs
                        
                        float logHeight = Random.Range((float)decor.spawnStackHeightMin, (float)decor.spawnStackHeightMax);
                        
                        for (unsigned int s=0; s < logHeight; s++) {
                            
                            staticMesh->AddSubMesh(-xp, height + s, -zp, subMeshTree, false);
                            
                            unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
                            
                            unsigned int index = numberOfSubMeshes - 1;
                            
                            Color finalColor;
                            Color lowTrunk;
                            Color highTrunk;
                            
                            lowTrunk  = (Colors.brown * 0.1f) + (Colors.green * 0.01);
                            highTrunk = (Colors.brown * 0.8f) + (Colors.green * 0.01);
                            
                            finalColor = Colors.Lerp(lowTrunk, highTrunk, (s * 0.087f));
                            
                            staticMesh->ChangeSubMeshColor(index, finalColor);
                            
                            staticMesh->ChangeSubMeshPosition(index, -xp, height + s - 1.0f, -zp);
                            
                        }
                        
                        // Leaves
                        
                        float leafCount = Random.Range(((float)world.numberOfLeaves) / 2, (float)world.numberOfLeaves);
                        
                        unsigned int leafAccent = Random.Range(0, 100);
                        
                        for (unsigned int z=0; z < leafCount; z++) {
                            
                            float offset_xx = Random.Range(0.0f, world.leafSpreadArea)   - Random.Range(0.0f, world.leafSpreadArea);
                            float offset_yy = Random.Range(0.0f, world.leafSpreadHeight) - Random.Range(0.0f, world.leafSpreadHeight);
                            float offset_zz = Random.Range(0.0f, world.leafSpreadArea)   - Random.Range(0.0f, world.leafSpreadArea);
                            
                            staticMesh->AddSubMesh(0, 0, 0, subMeshWallHorz, false);
                            staticMesh->AddSubMesh(0, 0, 0, subMeshWallVert, false);
                            
                            unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
                            unsigned int index = numberOfSubMeshes - 1;
                            
                            Color finalColor;
                            Color lowLeaves;
                            Color highLeaves;
                            
                            lowLeaves  = Colors.green * 0.08f;
                            highLeaves = Colors.green * 0.01f;
                            
                            if ((leafAccent > 70) & (leafAccent < 80)) 
                                lowLeaves = Colors.orange * 0.1f;
                            
                            if ((leafAccent > 20) & (leafAccent < 40)) 
                                lowLeaves = Colors.yellow * 0.1f;
                            
                            finalColor = Colors.Lerp(lowLeaves, highLeaves, Random.Range(0, 100) * 0.01f);
                            
                            staticMesh->ChangeSubMeshColor(index, finalColor);
                            staticMesh->ChangeSubMeshColor(index-1, finalColor);
                            
                            staticMesh->ChangeSubMeshPosition(index,   -xp + offset_xx, world.leafHeightOffset + height + logHeight + offset_yy, -zp + offset_zz);
                            staticMesh->ChangeSubMeshPosition(index-1, -xp + offset_xx, world.leafHeightOffset + height + logHeight + offset_yy, -zp + offset_zz);
                            
                        }
                        
                        
                        continue;
                    }
                    
                    // Actor generation
                    case 4: {
                        
                        GameObject* actorObject = Engine.CreateAIActor( glm::vec3(from.x, 0, from.z) );
                        
                        chunk->actorList.push_back( actorObject );
                        
                        Actor* actor = actorObject->GetComponent<Actor>();
                        
                        actor->SetHeightPreferenceMin(world.waterLevel);
                        actor->SetHeightPreferenceMax(40.0f);
                        
                        DecodeGenome(decor, actor);
                        
                        actor->SetAge( 1000 );
                        
                        // Generate youth
                        if (Random.Range(0, 100) > 90) {
                            
                            unsigned int numberOfChildren = Random.Range(1, 3);
                            
                            for (unsigned int c=0; c < numberOfChildren; c++) {
                                
                                float offsetX = (Random.Range(0, 1) - Random.Range(0, 1));
                                float offsetZ = (Random.Range(0, 1) - Random.Range(0, 1));
                                
                                GameObject* youthActorObject = Engine.CreateAIActor( glm::vec3(from.x + offsetX, 
                                                                                               0, 
                                                                                               from.z + offsetZ) );
                                
                                chunk->actorList.push_back( youthActorObject );
                                
                                Actor* youthActor = youthActorObject->GetComponent<Actor>();
                                
                                youthActor->SetHeightPreferenceMin(world.waterLevel);
                                youthActor->SetHeightPreferenceMax(40.0f);
                                
                                DecodeGenome(decor, youthActor);
                                
                                youthActor->SetAge(0);
                            }
                            
                            continue;
                        }
                        
                        continue;
                    }
                    
                    
                    default:
                        break;
                }
                
            }
            
        }
        
    }
}

void ChunkManager::PurgeChunks(glm::vec2 position) {
    
    if (!updateWorldChunks) 
        return;
    
    unsigned int numberOfChunks = mActiveChunks.size();
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        for (std::vector<Chunk*>::iterator it = mActiveChunks.begin(); it != mActiveChunks.end(); ++it) {
            
            Chunk* chunkPtr = *it;
            
            if (glm::distance(position, chunkPtr->position) < ((generationDistance * 2.0f) * chunkSize)) 
                continue;
            
            DestroyChunk( chunkPtr );
            
            mActiveChunks.erase( it );
            
            break;
        }
        
    }
    
    return;
}

void ChunkManager::PurgeWorld(void) {
    
    unsigned int numberOfChunks = mChunkList.Size();
    
    for (unsigned int i=0; i < mChunkList.Size(); i++) {
        
        Chunk* chunkPtr = mChunkList[0];
        
        if (chunkPtr == nullptr) 
            break;
        
        DestroyChunk( chunkPtr );
        
    }
    
    mActiveChunks.clear();
    
    return;
}

void ChunkManager::Initiate(void) {
    
    // Source meshes for world construction
    
    glm::vec3 normalUp(0, 1, 0);
    
    Engine.meshes.wallHorizontal->SetNormals(normalUp);
    Engine.meshes.wallVertical->SetNormals(normalUp);
    
    Engine.meshes.grassHorz->SetNormals(normalUp);
    Engine.meshes.grassVert->SetNormals(normalUp);
    
    Engine.meshes.stemHorz->SetNormals(normalUp);
    Engine.meshes.stemVert->SetNormals(normalUp);
    
    Engine.meshes.log->GetSubMesh(2, subMeshTree);
    
    Engine.meshes.wallHorizontal->GetSubMesh(0, subMeshWallHorz);
    Engine.meshes.wallVertical  ->GetSubMesh(0, subMeshWallVert);
    
    Engine.meshes.grassHorz->GetSubMesh(0, subMeshGrassHorz);
    Engine.meshes.grassVert->GetSubMesh(0, subMeshGrassVert);
    
    Engine.meshes.stemHorz->GetSubMesh(0, subMeshStemHorz);
    Engine.meshes.stemVert->GetSubMesh(0, subMeshStemVert);
    
    watermesh     = Engine.Create<Mesh>();
    watermaterial = Engine.Create<Material>();
    
    return;
}

