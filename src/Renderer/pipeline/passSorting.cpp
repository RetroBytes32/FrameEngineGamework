#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


bool RenderSystem::SortingPass(glm::vec3& eye, std::vector<MeshRenderer*>* renderQueueGroup, unsigned int queueGroupIndex) {
    
    std::vector<std::pair<float, MeshRenderer*>> sortList;
    
    std::vector<MeshRenderer*> renderQueue = *renderQueueGroup;
    
    for (unsigned int i=0; i < renderQueue.size(); i++) {
        
        std::pair<float, MeshRenderer*> renderPair;
        
        renderPair.first = glm::distance( eye, renderQueue[i]->transform.position );
        renderPair.second = renderQueue[i];
        
        sortList.push_back( renderPair );
        
    }
    
    std::sort(sortList.begin(), sortList.end(), [](std::pair<float, MeshRenderer*> a, std::pair<float, MeshRenderer*> b) {
        return a.first < b.first;
    });
    
    for (unsigned int i=0; i < renderQueue.size(); i++) {
        renderQueue[i] = sortList[i].second;
    }
    
    /*
    
    // Synchronize the sorting list
    if (mRenderQueueSorter[queueGroupIndex].size() != renderQueueGroup->size()) {
        
        mRenderQueueSorter[queueGroupIndex].clear();
        
        for (unsigned int i=0; i < renderQueueGroup->size(); i++) {
            
            MeshRenderer* meshRenderer = *(renderQueueGroup->data() + i);
            
            std::pair<float, MeshRenderer*> distancePair;
            
            distancePair.first  = 0;
            distancePair.second = meshRenderer;
            
            mRenderQueueSorter[queueGroupIndex].push_back( distancePair );
            
            continue;
        }
        
    }
    
    // Sort them out
    for (unsigned int i=0; i < renderQueueGroup->size(); i++) 
        mRenderQueueSorter[queueGroupIndex][i].first = glm::distance( eye, mRenderQueueSorter[queueGroupIndex][i].second->transform.position );
     
    std::sort(mRenderQueueSorter[queueGroupIndex].begin(), 
              mRenderQueueSorter[queueGroupIndex].end(), 
              [](std::pair<float, MeshRenderer*> a, std::pair<float, MeshRenderer*> b) {
        return a.first > b.first;
    });
    
    // Restore the sorted renderers
    renderQueueGroup->clear();
    
    for (unsigned int i=0; i < mRenderQueueSorter[queueGroupIndex].size(); i++) {
        
        renderQueueGroup->push_back( mRenderQueueSorter[queueGroupIndex][i].second );
        
        //*(renderQueueGroup->data() + i) = mRenderQueueSorter[queueGroupIndex][i].second;
        
    }
    
    */
    
    return true;
}

