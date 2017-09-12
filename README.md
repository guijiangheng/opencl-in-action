## reduction_complete
注意在使用CL_MEM_USE_HOST_PTR时，OpenCL可能会对主机的内存进行缓存，因此为了保证能够读取到数据，一定要进行同步，可以通过map操作实现同步：
```c++
auto mapped_memory = queue.enqueueMapBuffer(buffer, CL_TRUE, CL_MAP_READ, 0, sizeof(data));
queue.enqueueUnmapMemObject(buffer, mapped_memory);
```
