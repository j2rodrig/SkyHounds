/**
MemoryPool implmentation.
*/

#include "libraries.h"

#include "MemoryPool.h"

#include "MemoryBinding.h"

/**
Deletes all MemoryBindings.
delete *it++ calls MemoryBinding destructor, which calls RemoveMemoryBinding,
so bindings set should be empty at the end of this function.
*/
MemoryPool::~MemoryPool () {
	std::set<MemoryBinding * >::iterator it = bindings.begin ();
	while (it != bindings.end())
		delete *it++;
}
	