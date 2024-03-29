#include <GameEngineFramework/Resources/FileLoader.h>

#include <GameEngineFramework/Types/types.h>

extern StringType String;

FileLoader::FileLoader(std::string FileName) 
    : isFileLoaded(false) 
{
    
    std::fstream FileStream;
    
    this ->assetData.clear();
    this ->dataBlocks.clear();
    
    // Load the file data
    FileStream.open( FileName );
    if (!FileStream.is_open()) 
        return;
    
    std::string value;
    
    // Load the data from file
    while ( getline(FileStream, value) ) {
        
        if (value == "") continue;
        if (value.find("//") == 0) continue;
        
        rawData.push_back(value);
        
        // Split string by spaces
        std::vector<std::string> ArrayData = String.Explode(value, ' ');
        int MaxSz = ArrayData.size();
        if (MaxSz < 1) continue;
        
        // Check data block
        if (ArrayData[0] == "[begin]") {
            
            // Get block name
            std::string BlockName = ArrayData[1];
            std::string BlockString = "";
            
            for (int i=0; i < 512; i++) {
                
                getline(FileStream, value);
                if (value == "[end]") break;
                
                BlockString += value + "\n";
                
            }
            
            this ->dataBlocks.emplace(BlockName, BlockString);
        }
        
        std::vector<std::string> ValueList;
        
        for (int i=1; i < MaxSz; i++) {
            
            ValueList.push_back(ArrayData[i]);
            
        }
        
        this ->assetData.insert( std::pair<std::string, std::vector<std::string>>(ArrayData[0], ValueList) );
        
    }
    
    FileStream.close();
    isFileLoaded = true;
    
    return;
}

std::string FileLoader::GetValueByName(std::string Name, unsigned int Index) {
    
    for (std::map<std::string, std::vector<std::string>>::iterator it = assetData.begin(); it != assetData.end(); it++) {
        
        if (it ->first == Name) {
            
            std::vector<std::string> ValueList = it ->second;
            if (ValueList.size() == 0) return "";
            
            if (ValueList.size() < Index) return "";
            
            return ValueList[Index];
            
        }
        
    }
    
    return "";
}


std::string FileLoader::GetDataBlockByName(std::string Name) {
    
    for (std::map<std::string, std::string>::iterator it = dataBlocks.begin(); it != dataBlocks.end(); it++) {
        
        if (it ->first == Name) {
            
            return it ->second;
        }
        
    }
    
    return "";
}

