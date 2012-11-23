/**
Inheriting from MemoryBinding allows an object to be attached to
a MemoryPool. The object will be deleted automatically when the
MemoryPool is deleted.

Essentially, the lifespan of a MemoryBinding is constrained to be
less than or equal to the lifespan of its attached MemoryPool.
*/

#ifndef MEMORY_BINDING_H
#define MEMORY_BINDING_H

#include "MemoryPool.h"

class MemoryBinding {
protected:
	MemoryPool * pool;
public:
	MemoryBinding (MemoryPool * _pool)
		: pool(_pool)
	{
		if (pool)
			pool->AddMemoryBinding (this);
	}
	
	virtual ~MemoryBinding () {
		if (pool)
			pool->RemoveMemoryBinding (this);
	}
};

#endif

