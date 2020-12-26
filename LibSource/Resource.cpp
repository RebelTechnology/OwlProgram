#include <cstddef>
#include "Resource.h"

bool Resource::setData(void* src, uint32_t length, bool copy){
    if (hasData()){
        // Buffer is allocated, we don't allow doing it more than once
        return false;
    }
    else if (copy) {
        // Allocate a new buffer and copy there
        size = length;
        data = new uint8_t[size];
        memcpy((void*)data, src, length);
        allocated = true;
    }
    else {
        // Set pointer to source data, but don't allocate a new buffer
        data = (uint8_t*)src;
        size = length;
    }
    return true;
}

template<typename Array, typename Element>
const Array Resource::asArray(uint32_t offset, uint32_t max_size) const {
    // Data is expected to be aligned
    if (max_size > size - offset)
        max_size = size - offset;
    return Array((Element*)(getData() + offset), max_size / sizeof(Element));
}

template const FloatArray Resource::asArray<FloatArray, float>(uint32_t offset, uint32_t max_size) const;

template<typename Array, typename Element>
Array Resource::asArray(uint32_t offset, uint32_t max_size) {
    // Data is expected to be aligned
    if (max_size > size - offset)
        max_size = size - offset;
    return Array((Element*)(getData() + offset), max_size / sizeof(Element));
}

template FloatArray Resource::asArray<FloatArray, float>(uint32_t offset, uint32_t max_size);

void Resource::destroy(Resource resource) {
    if (resource.isAllocated()){
        delete [] resource.getData();
    }
}

const Resource* Resource::get(const char* name, uint32_t offset, uint32_t max_size) {
    uint8_t* buffer = NULL;
    // Load resource with a pointer to empty buffer
    void* args[] = {
        (void*)name, (void*)&buffer, (void*)&offset, (void*)&max_size
    };
    if (getProgramVector()->serviceCall(OWL_SERVICE_LOAD_RESOURCE, args, 4) == OWL_SERVICE_OK && buffer != NULL) {
        Resource* resource = new Resource(name);
        resource->setData(buffer, max_size, false);
        return resource;
    }
    else {
        return NULL;
    }
}

Resource Resource::load(const char* name, uint32_t offset, uint32_t max_size) {
    uint8_t* buffer = NULL;
    // Load resource header separately to know full resource size in advance
    void* args[] = {
        (void*)name, (void*)&buffer, (void*)&offset, (void*)&max_size
    };
    if (getProgramVector()->serviceCall(OWL_SERVICE_LOAD_RESOURCE, args, 4) == OWL_SERVICE_OK){
        // Reset data pointer to force allocation on memory mapped resources.
        Resource allocated_resource = Resource::create(max_size);
        args[1] = &allocated_resource.data;
        if (getProgramVector()->serviceCall(OWL_SERVICE_LOAD_RESOURCE, args, 4) == OWL_SERVICE_OK){
            return allocated_resource;
        }
        else {
            Resource::destroy(allocated_resource);
        }
    }
    // An empty resource is returned if we've failed to load one
    Resource resource(name);
    return resource;
}

Resource Resource::create(uint32_t size) {
    Resource resource;
    resource.setData(new uint8_t[size], size, false);
    resource.allocated = true;
    return resource;
}
