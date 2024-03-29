#include <GameEngineFramework/Resources/FileSystem.h>

#include <dirent.h>

std::vector<std::string> FileSystemDir::GetList(std::string path) {
    
    DIR* dir;
    struct dirent* ent;
    
    std::vector<std::string> directoryList;
    
    if ((dir = opendir( path.c_str() )) == NULL) 
        return directoryList;
    
    int index=0;
    while ((ent = readdir(dir)) != NULL) {
        if (index > 1) 
            directoryList.push_back( ent->d_name );
        index++;
    }
    
    closedir(dir);
    
    return directoryList;
}

