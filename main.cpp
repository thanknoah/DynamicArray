#include <type_traits>
#include <utility>
#include <cstring>
#include <iostream>

template <typename T>
class DynamicArray {

private:
	T* memory = nullptr;
	size_t capacity = 0;
	size_t size = 0;

public:
	DynamicArray() = default;
	DynamicArray(size_t x) {
		capacity = x;
		memory = reinterpret_cast<T*>(new std::byte[x * sizeof(T)]); // only if user calls DynamicArray<T>(x); then it allocates sizeof(T) * X amount of bytes of uninitalised memory
		size = 0;
	}

	DynamicArray(const DynamicArray& other) {
		reserve(other.capacity, other.size, other.memory); // reserve has addictional functionality, so it can copy from dynamic array to dynamic array
	}

	DynamicArray(DynamicArray&& other) noexcept {
		memory = other.memory;
	    size = other.size;
		capacity = other.capacity;

		other.capacity = 0;
		other.size = 0;
		other.memory = nullptr;
	}

	DynamicArray& operator=(DynamicArray&& other) noexcept {
		if (this != &other) {
			delete[] reinterpret_cast<std::byte*>(memory);

			memory = other.memory;
			size = other.size;
			capacity = other.capacity;

			other.capacity = 0;
			other.size = 0;
			other.memory = nullptr;
		}
		return *this;
	}

	DynamicArray& operator=(const DynamicArray& other) {
		if (this != &other) {
			reserve(other.capacity, other.size, other.memory);
		}
		return *this;
	}

	template <typename U>
	void insert(U&& t) {
		size_t elementSize = sizeof(T);
		if (capacity < size + 1) {
			if (capacity == 0) capacity = 1;
			size_t newCapacity = capacity * 2;
			reserve(newCapacity);
		}

		if constexpr (std::is_trivially_copyable_v<T>) {
			std::copy_n(&t, 1, memory + size);
		}
		else {
			new (&memory[size]) T(std::forward<U>(t));
		}

		size++;
	}

	void reserve(size_t x, size_t newSize = 1, T* outsidePtr = nullptr) {
		if (x < 0 || size > x)
			return;

		size_t elementSize = sizeof(T);
		size_t iterationAmount = (newSize == 1) ? size : newSize;
		bool usingOutsidePtr = (newSize == 1) ? false : true;

		T* memoryBlock = reinterpret_cast<T*>(new std::byte[x * elementSize]);

		if constexpr (std::is_trivially_copyable_v<T>) {
			std::copy_n(memory, size, memoryBlock);
		}
		else {
			for (size_t y = 0; y < iterationAmount; ++y) {
				if (!usingOutsidePtr) new (&memoryBlock[y]) T(std::move(memory[y]));
				if (!usingOutsidePtr) memory[y].~T();
				if (usingOutsidePtr) new (&memoryBlock[y]) T(outsidePtr[y]);
			}
		}

		delete[] reinterpret_cast<std::byte*>(memory);
		if (usingOutsidePtr) size = iterationAmount;

		capacity = x;
		memory = memoryBlock;
	}

	void clear() {
		for (size_t x = 0; x < size; x++) {
			memory[x].~T();
		}

		size = 0;
	}

	// Doesnt allow capacity to be shrinked for now, maybe later on
	void resize(size_t x) {
		if (x < 0 || size > x)
			return;

		size_t elementSize = sizeof(T);
		T* memoryBlock = reinterpret_cast<T*>(new std::byte[x * elementSize]);

		if constexpr (std::is_trivially_copyable_v<T>) {
			std::copy_n(memory, size, memoryBlock);
		}
		else {
			for (size_t y = 0; y < x; ++y) {
				if (size > y) {
					new (&memoryBlock[y]) T(std::move(memory[y])); // intializing construct and moving memory[y] into it
					memory[y].~T();
				}
				else {
					new (&memoryBlock[y]) T(); // intializing empty construct so can be modified later
				}
			}
		}

		delete[] reinterpret_cast<std::byte*>(memory);
		capacity = x;
		size = x;
		memory = memoryBlock;
	}

	void remove(size_t x) {
		size_t elementSize = sizeof(T);
		if (x >= size || memory == nullptr)
			return;

		if constexpr (std::is_trivially_copyable_v<T>) {
			std::memmove(&memory[x], &memory[x + 1], (size - x - 1) * elementSize);
		}
		else {
			for (size_t j = x; j < size - 1; ++j) {
				new (&memory[j]) T(std::move(memory[j + 1]));
			    memory[j + 1].~T(); 
			}
		}

		if constexpr (!std::is_trivially_destructible_v<T>) {
			memory[size - 1].~T();
		}

		size--;
	}

	void shrink_capacity_to_size() { reserve(size); }
	inline T* begin() { return memory; }
	inline T* end() { return memory + size; }
	inline size_t size_get() { return size; }
	inline size_t capacity_get() { return capacity; }

	inline T& get(size_t x) {
		if (x >= size || memory == nullptr)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return memory[x];
	}

	inline const T& get(size_t x) const {
		if (x >= size || memory == nullptr)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return memory[x];
	}


	inline T& operator[](size_t x) {
		if (x >= size || memory == nullptr)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return memory[x];
	}

	inline const T& operator[](size_t x) const {
		if (x >= size || memory == nullptr)
			throw std::runtime_error("DynamicArray error: tried to access out of bounds index.");

		return memory[x];
	}

	~DynamicArray() {
		if constexpr (!std::is_trivially_destructible_v<T>) {
			if (memory != nullptr) for (size_t i = 0; i < size; ++i) memory[i].~T();
		}

		delete[] reinterpret_cast<std::byte*>(memory);
	}
};
