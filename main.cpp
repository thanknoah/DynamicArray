#include <type_traits>
#include <iostream>

template <typename T>
class DynamicArray {

private:
	struct temp {
		T* memory;
		size_t sizeOfEachElement;
		size_t capacity;
		size_t memoryUsage;
		size_t size;
	};

	int elementShift;
	temp data;

	inline size_t performance_growth(size_t currentCapacity, size_t minimumSize) {
		while (currentCapacity < minimumSize) {
			currentCapacity *= 2;
		}

		return currentCapacity;
	}

public:
	DynamicArray() {
		data.sizeOfEachElement = sizeof(T);
		data.capacity = sizeof(T) * 16;
		data.memory = new T[16];
		data.memoryUsage = 0;
		elementShift = 0;
		data.size = 0;
	}
	

	void insert(T&& t) {
		bool optimizationEnabled = false;

		if (data.sizeOfEachElement <= 12) optimizationEnabled = false;
		if (data.capacity < data.memoryUsage + data.sizeOfEachElement) {
		   size_t newSize = DynamicArray::performance_growth(data.capacity, data.capacity + data.sizeOfEachElement);
		   T* newMemoryBlock = new T[newSize];

		   if (std::is_trivially_copyable_v<T>) {
			   std::memcpy(newMemoryBlock, data.memory, data.memoryUsage);

			   if (optimizationEnabled) newMemoryBlock[data.size] = std::move(t);
			   if (!optimizationEnabled) std::memcpy(newMemoryBlock + data.size, &t, data.sizeOfEachElement);
		   }
		   else {
			   if (optimizationEnabled) newMemoryBlock[data.size] = std::move(t);
			   if (!optimizationEnabled) std::copy(&t, &t + 1, newMemoryBlock);
		   }

		   delete[] data.memory;

		   data.capacity = newSize * data.sizeOfEachElement;
		   data.memoryUsage += data.sizeOfEachElement;
		   data.memory = newMemoryBlock;
		   elementShift = 0;
		}
		else {
			if (optimizationEnabled) data.memory[data.size] = std::move(t);
			if (!optimizationEnabled) std::copy(&t, &t + 1, data.memory + data.size);

			data.memoryUsage += data.sizeOfEachElement;
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
		elementShift++;
		data.size--;
	}

	T get(int x) {
		if (x > data.size || x < 0 || data.size == 0)
			std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		int finalIndex;

		if (x > x - elementShift)
			finalIndex -= x;
		else
			finalIndex = x;

		return data.memory[finalIndex];
	}

	inline size_t size() { return data.size; }
	inline size_t capacity() { return data.capacity; }
	inline size_t mem_usage() { return data.memoryUsage; }
    
	~DynamicArray() {
		delete[] data.memory;
	}
};

// Example code for now
// DynamicArray<int> myInts; myInts.insert(100); myInts.insert(200); myInts.get(0); myInts.remove(1); 
