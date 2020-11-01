#ifndef __RESOURCE_STORAGE_H__
#define __RESOURCE_STORAGE_H__

#include <stdint.h>
#include <cstring>
#include "OpenWareMidiControl.h"
#include "ServiceCall.h"
#include "ProgramVector.h"
#include "FloatArray.h"


/*
 * This class should not be instantiated directly, but it can be returned by untyped 
 * getResource() calls to storage.
 */

class Resource {
protected:
    uint32_t magic;
    Resource() = default;
    Resource(uint32_t size) : size(size) {};
public:
    uint32_t size;
    char name[24];
    Resource& operator=(const Resource&) = delete;
    Resource(const Resource&) = delete;
    /**
     * Get pointer to resource data
     */
    uint8_t* getData() const{
        return (uint8_t*)this + sizeof(Resource);
    }
    /**
     * Set new name for resource
     */
    void setName(const char* new_name) {        
        strncpy(name, new_name, 24);
    }

    template<typename Array, typename Element>
    Array asArray();
};


/*
 * StorageResource is template for accessing payload of specific type. It's OK to instantiate it
 * multiple types in one patch if necessary. It should be considered read-only, because
 * its data comes from flash.
 */
template<typename Payload>
class StorageResource : public Resource {
public:
    const Payload& getPayload() const { // Only reference to payload can be obtained and it's immutable
        return *(Payload*)getData();
    }
};


/*
 * This template contains payload that is stored in memory and filled from flash. It is mutable.
 */
template<typename Payload>
class MemoryResource : public Resource {
public:
    Payload payload;
    MemoryResource() {
        magic = 0xDADADEED;
        size = sizeof(Payload);
    }
    MemoryResource(Payload payload) : payload(payload) {
        MemoryResource();
    }
    MemoryResource& operator=(const MemoryResource&) = delete; // We want to move and not copy data in memory.
    MemoryResource& operator=(const StorageResource<Payload>& resource) { // Copy from storage to memory.
        std::memcpy((void*)&payload, (void*)&resource.getPayload(), sizeof(Payload));
        setName(resource.name);
        return *this;
    }
    MemoryResource(const MemoryResource&) = delete;
};


class DynamicMemoryResource : public Resource {
public:
    DynamicMemoryResource() = default;
    //DynamicMemoryResource(uint32_t size, uint8_t* buffer)
    //    : Resource(size)
    //    , buffer(buffer) {};
    ~DynamicMemoryResource();
    uint8_t* getData() const{
        return (uint8_t*)buffer;
    }

    bool init(uint32_t size);

    template<typename Array, typename Element>
    Array asArray();
protected:
    uint8_t* buffer;
};


class ResourceStorage {
public:
    ResourceStorage() {
        init();
    };
    // Disable copy and assignment
    ResourceStorage& operator=(const ResourceStorage&) = delete;
    ResourceStorage(const ResourceStorage&) = delete;

    // This is just an explicit call to return data retrieved by init()
    uint8_t getResourceCount() const {
        return num_resources;
    };
    bool deleteResource(const char* name);
    
    /**
     *  Untyped resource access
     */
    /*const */
    Resource* getResource(const char* name) const;

    /**
     *  Typed resource access
     */
    template<typename Payload>
    const StorageResource<Payload>* getResource(const char* name) const {
        return reinterpret_cast<const StorageResource<Payload>*>(getResource(name));
    }

    /**
     * Untyped resource load
     */
    bool loadResource(
        const char* name, DynamicMemoryResource& resource,
        uint32_t max_size = 0xffffffff, uint32_t offset = 0) const ;

    /**
     * Type resource load
     */
    template<typename Payload>
    bool loadResource(const char* name, MemoryResource<Payload>& resource, uint32_t offset=0) const {
        offset += sizeof(Resource);
        uint32_t length = sizeof(Payload);
        void* args[] = {
            (void*)(SYSEX_CONFIGURATION_RESOURCE_BY_NAME), (void*)&resource.payload, (void*)name,
            (void*)&length, (void*)&offset
        };
        return getProgramVector()->serviceCall(OWL_SERVICE_LOAD_RESOURCE, args, 5) == OWL_SERVICE_OK;
    }
    
private:
    uint8_t num_resources;
    // This could be made public if necessary. For now it's called automaticall on
    // patch start and after deleting resource. We might have a callback that would
    // call this if resource gets deleted outside of patch, i.e. by MIDI or in UI.
    void init();
};


#endif