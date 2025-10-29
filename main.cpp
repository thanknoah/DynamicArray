#include <type_traits>
#include <iostream>
#include <cstring>


template <typename T>
class DynamicArray {

private:
	struct temp {
		T* memory;
		size_t capacity;
		size_t memoryUsage;
		size_t size;
	};

	int elementShift;
	temp data;

	inline size_t performance_growth(size_t currentCapacity) {
			return currentCapacity *= 2;
	}

public:
	DynamicArray() {
		data.capacity = 1;
		data.memory = new T[1];
		elementShift = 0;
		data.size = 0;
		data.memoryUsage = 0;
	}
	

	void insert(T& t) {
		bool optimizationEnabled = false;
		size_t elementSize = sizeof(T);

		if (elementSize <= 12) optimizationEnabled = true;
		if (data.capacity < data.size + 1) {
		   size_t newSize = DynamicArray::performance_growth(data.capacity);
		   T* newMemoryBlock = new T[newSize];
		   
		   if constexpr(std::is_trivially_copyable_v<T>) {
		      std::memcpy(newMemoryBlock, data.memory, data.size * elementSize); // copies data byte by byte from old ptr to new ptr
		   } else {
		      std::move(data.move, data.memory + data.size, newMemoryBlock); // transfers each elements internal ptr to an index of new ptr 
		   }


		   if (optimizationEnabled) newMemoryBlock[data.size] = std::forward<T>(t);
		   if (!optimizationEnabled) std::copy(&t, &t + 1, newMemoryBlock);
		   delete[] data.memory;

		   data.capacity = newSize;
		   data.memoryUsage += elementSize;
		   data.memory = newMemoryBlock;
		   elementShift = 0;
		}
		else {
			if (optimizationEnabled) data.memory[data.size] = std::forward<T>(t);
			if (!optimizationEnabled) std::copy(&t, &t + 1, data.memory + data.size);

			data.memoryUsage += elementSize;
		}

		data.size++;
	}


	/*
	void shrink_to_fit() {
		if (data.size == 0)
			return

	    T * newMemoryBlock = new T[data.size];
		if (std::is_trivially_copyable_v<T>) {
			std::memcpy(newMemoryBlock, data.memory, data.memoryUsage);
		}
		else {
			std::copy(data.memory, data.memoryUsage)
		}
	
	}
	*/
	
	
	void reserve(int x) {
		if (x < 0 || x < data.memoryUsage)
			return;

		T* newMemoryBlock = new T[x];
		delete[] data.memory;

		data.memory = newMemoryBlock;
		data.capacity = x;
		elementShift = 0;
	}

	void remove(int x) {
		if (x > data.size || x < 0 || data.size == 0)
			return;

		data.memory[x].~T();
		//elementShift++;

		// STILL NEED TO optimize this
		for (size_t i = x; i < data.size - 1; ++i)
			data.memory[i] = std::move(data.memory[i + 1]);

		data.size--;
	}

	T get(int x) {
		if (x > data.size || x < 0 || data.size == 0)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return data.memory[x];
	}
	
	inline size_t size() { return data.size; }
	inline size_t capacity() { return data.capacity; }
	inline size_t mem_usage() { return data.memoryUsage; }
    
	~DynamicArray() {
		delete[] data.memory;
	}
};
