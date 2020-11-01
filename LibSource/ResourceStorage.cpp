#include <cstddef>
#include "ResourceStorage.h"

const Resource* ResourceStorage::getResource(uint8_t index) const {
    const Resource* resource;
    void* args[] = {
        (void*)(SYSEX_CONFIGURATION_RESOURCE_BY_ID), (void*)&resource, (void*)&index
    };
    int ret = getProgramVector()->serviceCall(OWL_SERVICE_GET_PARAMETERS, args, 3);
    if (ret == OWL_SERVICE_OK){
        return resource;
    }
    else {
        return NULL;
    }
}

const Resource*  ResourceStorage::getResource(const char* name) const {
    const Resource* resource;
    void* args[] = {
        (void*)(SYSEX_CONFIGURATION_RESOURCE_BY_NAME), (void*)&resource, (void*)name
    };
    int ret = getProgramVector()->serviceCall(OWL_SERVICE_GET_PARAMETERS, args, 3);
    if (ret == OWL_SERVICE_OK){
        return resource;
    }
    else {
        return NULL;
    }
}

bool ResourceStorage::deleteResource(uint8_t index){
    void (*callback)(uint8_t) = NULL;
    void* args[] = {
        (void*)(SYSTEM_FUNCTION_RESOURCE_DELETE), &callback
    };
    int ret = getProgramVector()->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, args, 2);
    if (ret == OWL_SERVICE_OK){
        callback(index);
        init();
        return true;
    }
    else {
        return false;
    }
}

void ResourceStorage::init(){
    void* args[] = {
        (void*)(SYSEX_CONFIGURATION_RESOURCE_COUNT), (void*)&num_resources
    };
    int ret = getProgramVector()->serviceCall(OWL_SERVICE_GET_PARAMETERS, args, 2);
    if (ret != OWL_SERVICE_OK)
        num_resources = 0;
}
