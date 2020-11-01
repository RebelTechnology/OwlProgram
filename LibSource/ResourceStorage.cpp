#include <cstddef>
#include "ResourceStorage.h"

template<typename Array, typename Element>
Array Resource::asArray(){
    // Data is expected to be aligned
    return Array((Element*)getData(), size / sizeof(Element));
}

template FloatArray Resource::asArray<FloatArray, float>();


bool DynamicMemoryResource::init(uint32_t size){
    if (!size || buffer == NULL)
        // We want to allocate memory just once per object
        return false;
    else {
        buffer = new uint8_t[size];
        return buffer != NULL;
    }
}

DynamicMemoryResource::~DynamicMemoryResource(){
    if (buffer != NULL)
        delete [] buffer;
}

template<typename Array, typename Element>
Array DynamicMemoryResource::asArray(){
    return Array((Element*)getData(), size / sizeof(Element));
}

template FloatArray DynamicMemoryResource::asArray<FloatArray, float>();


/*const */
Resource*  ResourceStorage::getResource(const char* name) const {
    /*const */Resource* resource;
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

bool ResourceStorage::deleteResource(const char* name){
    void (*callback)(const char*) = NULL;
    void* args[] = {
        (void*)(SYSTEM_FUNCTION_RESOURCE_DELETE), &callback
    };
    int ret = getProgramVector()->serviceCall(OWL_SERVICE_REQUEST_CALLBACK, args, 2);
    if (ret == OWL_SERVICE_OK){
        callback(name);
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

bool ResourceStorage::loadResource(
        const char* name, DynamicMemoryResource& resource, uint32_t max_size, uint32_t offset) const {
    // Load resource header separately to know full resource size in advance
    uint32_t arg_length = sizeof(Resource);
    uint32_t arg_offset = 0;
    void* args[] = {
        (void*)(SYSEX_CONFIGURATION_RESOURCE_BY_NAME), (void*)&resource, (void*)name,
        (void*)&arg_length, (void*)&arg_offset
    };
    if (getProgramVector()->serviceCall(OWL_SERVICE_LOAD_RESOURCE, args, 5) != OWL_SERVICE_OK)
        return false;

    if (offset >= resource.size) // Offset specifies reading past resource end
        max_size = 0;
    else if (max_size > resource.size - offset) // Truncate read bytes to stop at resource end
        max_size = resource.size - offset;

    // Update size in case if we're fetching partial data.
    resource.size = max_size;

    // Load resource contents
    if (max_size > 0) {
        // Allocate buffer
        if (!resource.init(max_size))
            return false;

        // Load data after offset. We don't use sizeof(DynamicMemoryResource) below,
        // because we're actually interested in size of ResourceHeader object in storage block.
        // DynamicMemoryResource contains extra data - pointer to buffer.
        offset = offset + sizeof(Resource);
        args[1] = (void*)resource.getData();
        args[3] = (void*)&max_size;
        args[4] = (void*)&offset;
        if (getProgramVector()->serviceCall(OWL_SERVICE_LOAD_RESOURCE, args, 5) == OWL_SERVICE_OK){
            return true;
        }
    };
    return false;
}