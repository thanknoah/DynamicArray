#include <type_traits>
#include <cstring>
#include <iostream>


template <typename T>
class DynamicArray {

private:
	struct temp {
		T* memory;
		size_t capacity;
		size_t size;
		size_t elementSize;
	};

	temp data;

public:
	DynamicArray() {
		data.capacity = 0;
		data.memory = nullptr;
		data.size = 0;
	}

	void insert(T& t) {
		T* memoryBlock = nullptr;
		size_t elementSize = sizeof(T);

		if (data.capacity < data.size + 1) {
			if (data.capacity == 0) data.capacity = 1;

			size_t newSize = data.capacity * 2;
			std::byte* tempPtr = new std::byte[newSize * elementSize];
			memoryBlock = reinterpret_cast<T*>(tempPtr);

			if constexpr (std::is_trivially_copyable_v<T>) {
				std::memcpy(memoryBlock, data.memory, data.size * elementSize);
			}
			else {
				for (int x = 0; x < data.size; ++x) {
					new (&memoryBlock[x]) T(std::move(data.memory[x]));
					data.memory[x].~T();
				}
			}

			delete[] reinterpret_cast<std::byte*>(data.memory);
			data.capacity = newSize;
			data.memory = memoryBlock;
		}
		else {
			memoryBlock = data.memory;
		}

		if constexpr (std::is_trivially_copyable_v<T>) 
		    std::memcpy(memoryBlock + data.size, &t, elementSize);
		else 
			new (&memoryBlock[data.size]) T(std::move(t));

		data.size++;
	}


	void reserve(int x) {
		if (x < 0)
			return;

		size_t elementSize = sizeof(T);
		T* memoryBlock = reinterpret_cast<T*>(new std::byte[x * elementSize]);

		if (data.memory != nullptr) {
			if constexpr (std::is_trivially_copyable_v<T>) {
				std::memcpy(memoryBlock, data.memory, data.size * elementSize);
			}
			else {
				for (int x = 0; x < data.size; ++x) {
					new (&memoryBlock[x]) T(std::move(data.memory[x]));
					data.memory[x].~T();
				}
			}
		}

		delete[] reinterpret_cast<std::byte*>(data.memory);
		data.capacity = x;
		data.memory = memoryBlock;
	}

	void remove(int x) {
		size_t elementSize = sizeof(T);
		if (x >= data.size || data.memory == nullptr)
			return;

		if constexpr (!std::is_trivially_destructible_v<T>) {
			data.memory[x].~T();
		}

		if constexpr (std::is_trivially_copyable_v<T>) {
			std::memmove(&data.memory[x], &data.memory[x + 1], (data.size - x - 1) * elementSize);
		}
		else {
			for (size_t j = x; j < data.size - 1; ++j) {
				new (&data.memory[j]) T(std::move(data.memory[j + 1]));
			    data.memory[j + 1].~T(); 
			}
		}

		
		data.size--;
	}

	inline T* begin() { return data.memory; }
	inline T* end() { return data.memory + data.size; }
	inline size_t size() { return data.size; }
	inline size_t capacity() { return data.capacity; }

	inline T get(size_t x) {
		if (x >= data.size || data.memory == nullptr)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return data.memory[x];
	}


	inline T& operator[](size_t x) {
		if (x >= data.size || data.memory == nullptr)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return data.memory[x];
	}

	inline const T& operator[](size_t x) const {
		if (x >= data.size || data.memory == nullptr)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return data.memory[x];
	}

	~DynamicArray() {
		if (data.memory == nullptr) return;
		if constexpr (!std::is_trivially_destructible_v<T>) {
			for (size_t i = 0; i < data.size; ++i)
				data.memory[i].~T();
		}

		delete[] reinterpret_cast<std::byte*>(data.memory);
	}
};
