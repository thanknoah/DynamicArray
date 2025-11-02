#include <type_traits>
#include <vector>
#include <cstring>
#include <iostream>
#include <chrono>

// Notes: optimizationEnabled is where data type is less than or equal to 12 bytes, so i use (=) / std::move
// if optimizationEnabled is disabled then i use std::copy which will either copy the value or use memcpy depending on if its trivally copyale

template <typename T>
class DynamicArray {

private:
	struct temp {
		T* memory;
		size_t capacity;
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
	}

	void insert(const T& t) {
		bool optimizationEnabled = (sizeof(T) <= 12);
		size_t elementSize = sizeof(T);
		T* memoryBlock = nullptr;

		if (data.capacity < data.size + 1) {
			size_t newSize = data.capacity * 2;
			memoryBlock = new T[newSize];

			if constexpr (std::is_trivially_copyable_v<T>) {
				std::memcpy(memoryBlock, data.memory, data.size * elementSize);
			}
			else {
				std::move(data.memory, data.memory + data.size, memoryBlock);
			}

			delete[] data.memory;

			data.capacity = newSize;
			data.memory = memoryBlock;
		}

		if (memoryBlock == nullptr) memoryBlock = data.memory;
		if constexpr (std::is_trivially_copyable_v<T>) {
			if (optimizationEnabled) {
				memoryBlock[data.size] = t;
			}
			else {
				std::memcpy(memoryBlock + data.size, &t, elementSize);
			}
		}
		else {
			memoryBlock[data.size] = std::move(t);
		}

		data.size++;
	}


	void reserve(int x) {
		if (x < 0 || x < data.size)
			return;

		T* memoryBlock = new T[x];
		delete[] data.memory;

		data.memory = memoryBlock;
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

			data.memory[data.size - 1].~T();
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

	inline T get(size_t x) {
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
