# CircularQueue

MIMO Circular Queue implementation

The circular queue has a performance advantage over a traditional queue when the elements contained with in the queue have the same memory footprint.  The memory allocation involves significant overhead and can be avoided by keeping existing objects in the queue and copying the information from the input into the queued data structure.  Media files are a typical example where circular queues can improve performance, particularly in the case where the frame data is especially memory intensive such as a 4K video.

The queue is designed to be thread safe and will accommodate multiple inputs and multiple outputs (MIMO).  Input threads will be required to wait when the queue is full before writing new data and outputs likewise will wait during an empty queue condition.  Upon the completion of all inputs, the queue must be flushed so that the outputs will drain the last data from the queue.  Once all operations are complete, the queue can be closed to prevent further attempts to access the queue data.

The queue will accomodate objects of differing memory configurations, however the performance advantage will not exist in this case.  It will behave as a traditional queue when elements do not have an identical memory footprint.

Care must be taken when implementing class objects that are to be used in the queue.  The element class must properly implement the copy constructor and assignment operator for correct operation in the queue.  Please refer to the Picture class example included.  

A typical element class will contain macro data such as a header identifying the character of the data structure and a pointer to memory allocated on the heap which is the actual data.  Some combination of malloc and memcpy will typically be used to allocate and copy the data to the heap memory, and the usual precautions used around these types of operations are required.  The element class destructor should handle freeing the heap memory.

