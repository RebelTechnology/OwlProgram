#include <algorithm>
#include <cstring>
#include "Resource.h"
#include "OpenWareMidiControl.h"
#include "ServiceCall.h"
#include "ProgramVector.h"

template<typename Array, typename Element>
Array Resource::asArray(size_t offset, size_t max_size) {
    // Data is expected to be aligned
    if (max_size > size - offset)
        max_size = size - offset;
    return Array((Element*)(data + offset), max_size / sizeof(Element));
}

template FloatArray Resource::asArray<FloatArray, float>(size_t offset, size_t max_size);

void Resource::destroy(Resource* resource) {
  if(resource && resource->allocated)
    delete[] resource->data;
  delete resource;
}

Resource* Resource::open(const char* name){
  uint8_t* data = NULL;
  size_t offset = 0;
  size_t size = 0;
  void* args[] = {
		  (void*)name, (void*)&data, (void*)&offset, (void*)&size
  };
  if(getProgramVector()->serviceCall(OWL_SERVICE_LOAD_RESOURCE, args, 4) == OWL_SERVICE_OK)
    return new Resource(name, size, data);
  return NULL;
}

Resource* Resource::load(const char* name){
  Resource* resource = Resource::open(name);
  if(resource && !resource->hasData()){
    uint8_t* data = new uint8_t[resource->size];
    size_t offset = 0;
    void* args[] = {
		    (void*)name, (void*)&data, (void*)&offset, (void*)&resource->size
    };
    if (getProgramVector()->serviceCall(OWL_SERVICE_LOAD_RESOURCE, args, 4) == OWL_SERVICE_OK){
      resource->data = data;
      resource->allocated = true;
    }else{
      delete[] data;
    }
  }
  return resource;
}

size_t Resource::read(void* dest, size_t len, size_t offset){
  if(this->data == NULL){
    void* args[] = {
		    (void*)this->name, (void*)&dest, (void*)&offset, (void*)&len
    };
    if (getProgramVector()->serviceCall(OWL_SERVICE_LOAD_RESOURCE, args, 4) == OWL_SERVICE_OK){
      return len;
    }else{
      return 0;
    }
  }else{
    len = std::min(this->size-offset, len);
    memcpy(dest, (uint8_t*)this->data+offset, len);
    return len;
  }
}
