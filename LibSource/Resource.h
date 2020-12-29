#ifndef __RESOURCE_STORAGE_H__
#define __RESOURCE_STORAGE_H__

#include <cstddef>

class Resource {
public:
  /**
   * Check if data is available
   */
  bool hasData() const {
    return data != NULL;
  }

  /**
   * Get pointer to data. This may be NULL if no buffer is assigned yet.
   */
  void* getData() {
    return data;
  }

  /**
   * Get buffer size in bytes
   */
  size_t getSize() const {
    return size;
  }    

  /**
   * Get resource name
   */
  const char* getName() const {
    return name;
  }

  /**
   * Array conversion.
   *
   * @param offset: offset in bytes
   * @param max_size maximum size, actual size can be smaller depending on object size
   */
  template<typename Array, typename Element>
  Array asArray(size_t offset = 0, size_t max_size = 0xFFFFFFFF);

  /**
   * Get resource from storage. Creates a new Resource on the heap.
   * Shoule be garbage collected with Resource::destroy()
   * 
   * @param name resource name
   * 
   * @return NULL if resource does not exist or can't be read.
   * 
   */
  static Resource* open(const char* name);

  /**
   * Open resource and load data. Creates a new Resource on the heap and 
   * allocates extra memory if required.
   * Must be garbage collected with Resource::destroy()
   */
  static Resource* load(const char* name);

  /**
   * Destroy allocated memory used by a resource
   */
  static void destroy(Resource* resource);

  /**
   * Read data from resource into memory
   *
   * @param len maximum number of bytes to read
   * @param offset index of first byte to read from
   *
   * @return number of bytes actually read
   */
  size_t read(void* dest, size_t len, size_t offset=0); 

protected:
  Resource(const char* name, size_t size, void* data)
    : name(name), size(size), data(data) {}
  const char* name = NULL;
  size_t size = 0;
  void* data = NULL;
  bool allocated = false;
};
#endif
