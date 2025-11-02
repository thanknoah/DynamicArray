#include <type_traits>
#include <vector>
#include <cstring>

// Notes: optimizationEnabled is where data type is less than or equal to 12 bytes, so i use std::move or = instead of memcpy for trivally copyable types

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
			size_t newSize = data.capacity * 2;
			T* newMemoryBlock = new T[newSize];

			if constexpr (std::is_trivially_copyable_v<T>) {
				std::memcpy(newMemoryBlock, data.memory, data.size * elementSize); // copies data byte by byte from old ptr to new ptr
			}
			else {
				std::move(data.memory, data.memory + data.size, newMemoryBlock); // transfers each elements internal ptr to an index of new ptr 
			}

		    if (optimizationEnabled) newMemoryBlock[data.size] = t;
			if (!optimizationEnabled) std::copy(&t, &t + 1, newMemoryBlock);
			delete[] data.memory;

			data.capacity = newSize;
			data.memoryUsage += elementSize;
			data.memory = newMemoryBlock;
			elementShift = 0;
		}
		else {
			if (optimizationEnabled) data.memory[data.size] = t;
		    if (!optimizationEnabled) std::copy(&t, &t + 1, data.memory);

			data.memoryUsage += elementSize;
		}

		data.size++;
	}

	void insert(T&& t) {
		bool optimizationEnabled = false;
		size_t elementSize = sizeof(T);

		if (elementSize <= 12) optimizationEnabled = true;

		if (data.capacity < data.size + 1) {
			size_t newSize = data.capacity * 2;
			T* newMemoryBlock = new T[newSize];

			if constexpr (std::is_trivially_copyable_v<T>) {
				std::memcpy(newMemoryBlock, data.memory, data.size * elementSize); // copies data byte by byte from old ptr to new ptr
			}
			else {
				std::move(data.memory, data.memory + data.size, newMemoryBlock); // transfers each elements internal ptr to an index of new ptr 
			}

		    newMemoryBlock[data.size] = std::move(t);
			delete[] data.memory;

			data.capacity = newSize;
			data.memoryUsage += elementSize;
			data.memory = newMemoryBlock;
			elementShift = 0;
		}
		else {
			data.memory[data.size] = std::move(t);
			data.memoryUsage += elementSize;
		}

		data.size++;
	}

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
		size_t elementSize = sizeof(T);
		bool optimizationEnabled = false;
		if (elementSize <= 12) optimizationEnabled = true;

		if constexpr (!std::is_trivially_copyable_v<T>) {
			for (size_t i = x; i < data.size - 1; ++i)
				data.memory[i] = std::move(data.memory[i + 1]);
		}
		else { 
			if (optimizationEnabled) {
				for (size_t i = x; i < data.size - 1; ++i)
					data.memory[i] = data.memory[i + 1];
			}
			else {
                for (size_t i = x; i < data.size - 1; ++i)
				    std::memmove(&data.memory[i], &data.memory[i + 1], sizeof(T));
			}
		}

		data.size--;
	}

	inline T* begin() { return data.memory; }
	inline T* end() { return data.memory + data.size; }
	inline size_t size() { return data.size; }
	inline size_t capacity() { return data.capacity; }
	inline size_t mem_usage() { return data.memoryUsage; }

	inline T get(int x) {
		if (x > data.size || x < 0 || data.size == 0)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return data.memory[x];
	}


	inline T& operator[](size_t x) {
		if (x > data.size || x < 0 || data.size == 0)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return data.memory[x];
	}

	inline const T& operator[](size_t x) const {
		if (x > data.size || x < 0 || data.size == 0)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return data.memory[x];
	}

	~DynamicArray() {
		delete[] data.memory;
	}
};
