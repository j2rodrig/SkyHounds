/**
Inheriting from MemoryPool allows an object to "host" MemoryBindings.
All MemoryBindings will be deleted automatically when the
MemoryPool is deleted. BindingType should inherit from MemoryBinding
or compatible type.

Essentially, the lifespan of a MemoryBinding is constrained to be
less than or equal to the lifespan of its MemoryPool.
*/

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

class MemoryBinding;  // forward declaration

class MemoryPool {
protected:
	std::set<MemoryBinding * > bindings;
public:
	// Events for when things are added or removed from this pool
	virtual void onBinding (MemoryBinding * thing) { }
	virtual void onDeleting (MemoryBinding * thing) { }
	
	void AddMemoryBinding (MemoryBinding * thing) {
		onBinding (thing);
		bindings.insert (thing);
	}
	
	void RemoveMemoryBinding (MemoryBinding * thing) {
		bindings.erase (thing);
		onDeleting (thing);
	}

	virtual ~MemoryPool ();
};

#endif

