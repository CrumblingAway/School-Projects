#ifndef CPP_EX3_HASHMAP_HPP
#define CPP_EX3_HASHMAP_HPP

#include <iostream>
#include <vector>
#include <cassert>
using std::vector;
using std::pair;

#define DEF_UPPER 0.75
#define DEF_LOWER 0.25
#define DEF_SIZE 0
#define DEF_CAP 16
#define UPPER_FACTOR 2.0
#define LOWER_FACTOR 0.5

/**
 * Exception to be thrown in case of invalid key for at() method
 */
class invalidKeyException: public std::exception
{
public:
	/**
	 * Constructor that prints a message
	 * @param message
	 */
	explicit invalidKeyException(char const* msg): _msg(msg) {}
	
	const char* what() const noexcept override
	{
		return _msg.c_str();
	}
	
private:
	std::string _msg;
};

/**
 * Exception to be thrown in case of vectors provided to constructor
 * having different sizes
 */
class differentVectorSizes: public std::exception
{
public:
	/**
	 * Constructor that prints a message
	 * @param message
	 */
	explicit differentVectorSizes(char const* msg): _msg(msg) {}
	
	const char* what() const noexcept override
	{
		return _msg.c_str();
	}
	
private:
	std::string _msg;
};

/**
 * Generic class for a HashMap, containing an upper and lower
 * bound for load factors and storing all keys and values in a
 * dynamic array of vectors of pairs (see field "storage")
 * @tparam KeyT type of key
 * @tparam ValueT type of value
 */
template <class KeyT, class ValueT>
class HashMap
{
public:

	/**
	 * Constructor that receives upper and lower load factors
	 * @param dUpper upper bound
	 * @param dLower lower bound
	 */
	HashMap<KeyT, ValueT>(double dLower, double dUpper):
			_dLower(dLower),
			_dUpper(dUpper),
			_iSize(DEF_SIZE),
			_iCapacity(DEF_CAP)
	{
		_storage = new vector<pair<KeyT, ValueT>>[_iCapacity];
	}

	/**
	 * Default constructor, empty HashMap with
	 * fUpper = 0.75 and fLower = 0.25
	 */
	HashMap<KeyT, ValueT>(): HashMap(DEF_LOWER, DEF_UPPER) {}

	/**
	 * Constructor that receives a key vector and a value vector,
	 * creates HashMap of keys and values with corresponding indices
	 * @param keyVec vector of keys
	 * @param valVec vector of values
	 */
	HashMap<KeyT, ValueT>(vector<KeyT> keyVec, vector<ValueT> valVec): HashMap()
	{
		if(keyVec.size() == valVec.size())
		{
			for(unsigned long i = 0; i < keyVec.size(); ++i)
			{
				insert(keyVec[i], valVec[i]);
			}
		}
		else
		{
			throw differentVectorSizes("Vectors are of different sizes");
		}
	}

	/**
	 * Copy constructor
	 * @param other
	 */
	HashMap<KeyT, ValueT>(const HashMap<KeyT, ValueT> &other):
	        _dUpper(other._dUpper),
	        _dLower(other._dLower),
	        _iSize(other._iSize),
	        _iCapacity(other._iCapacity)
	{
		_storage = new vector<pair<KeyT, ValueT>>[_iCapacity];

		for(int i = 0; i < _iCapacity; ++i)
		{
			for(unsigned long j = 0; j < other._storage[i].size(); ++j)
			{
				_storage[i].push_back(other._storage[i][j]);
			}
		}
	}

	/**
	 * Move constructor
	 * @param other
	 */
	HashMap<KeyT, ValueT>(const HashMap<KeyT, ValueT> && other) noexcept:
			_dUpper(other._dUpper),
			_dLower(other._dLower),
			_iSize(other._iSize),
			_iCapacity(other._iCapacity)
	{
		for(int i = 0; i < _iCapacity; ++i)
		{
			for(unsigned long j = 0; j < other._storage[i].size(); ++j)
			{
				_storage[i].push_back(other._storage[i][j]);
			}
		}
	}

	/**
	 * Destructor
	 */
	~HashMap<KeyT, ValueT>()
	{
		delete[] _storage;
	}

	/**
	 * Get amount of cells currently occupied
	 * @return
	 */
	int size() const
	{
		return _iSize;
	}

	/**
	 * Get the amount of elements the map is capable of storing
	 * @return
	 */
	int capacity() const
	{
		return _iCapacity;
	}

	/**
	 * Get current load factor (size / capacity)
	 * @return
	 */
	double getLoadFactor() const
	{
		return (double)_iSize / _iCapacity;
	}

	/**
	 * Check if map is empty
	 * @return true if empty, otherwise false
	 */
	bool empty() const
	{
		return _iSize == 0;
	}

	/**
	 * Check if map contains given key
	 * @param key key to look up
	 * @return true if key is present in map, otherwise false
	 */
	bool containsKey(KeyT key) const
	{
		std::hash<KeyT> keyHasher;
		size_t newIdx = keyHasher(key) & (_iCapacity - 1);
		for(unsigned long i = 0; i < _storage[newIdx].size(); ++i)
		{
			if(_storage[newIdx][i].first == key)
			{
				return true;
			}
		}

		return false;
	}

	/**
	 * Resize storage array according to upper and lower bounds
	 * of load factor
	 * @param factor factor by which to resize
	 */
	void resize(double factor)
	{
		int newSize = (int) (_iCapacity * factor);
		auto *newArr = new vector<pair<KeyT, ValueT>>[newSize];

		/* Copy over all items */
		for(int i = 0; i < _iCapacity; ++i)
		{
			for(unsigned long j = 0; j < _storage[i].size(); ++j)
			{
				std::hash<KeyT> keyHasher;
				int newIdx = keyHasher(_storage[i][j].first) & (newSize - 1);

				newArr[newIdx].push_back(_storage[i][j]);
			}
		}

		_iCapacity = newSize;
		delete[] _storage;
		_storage = newArr;
	}

	/**
	 * Insert given item to map
	 * @param item item to be inserted
	 * @return true if successfully inserted, otherwise false
	 */
	bool insert(KeyT key, ValueT value)
	{
		if(containsKey(key))
		{
			return false;
		}

		double ratio = (double) _iSize / _iCapacity;
		if(ratio >= _dUpper)
		{
			resize(UPPER_FACTOR);
		}

		std::hash<KeyT> keyHasher;
		//TODO: fix the hashing with unsigned
		int newIdx = keyHasher(key) & (_iCapacity - 1);

		pair<KeyT, ValueT> item(key, value);
		_storage[newIdx].push_back(item);
		_iSize++;

		return true;
	}

	/**
	 * Get value at key in map. Throws exception if key not present
	 * @param key key to look up
	 * @return value bound to key, or exception if key not found
	 */
	ValueT at(KeyT key) const
	{
		if(containsKey(key))
		{
			std::hash<KeyT> keyHasher;
			size_t newIdx = keyHasher(key) & (_iCapacity - 1);
			for(unsigned long idx = 0; idx < _storage[newIdx].size(); ++idx)
			{
				if(_storage[newIdx][idx].first == key)
				{
					return _storage[newIdx][idx].second;
				}
			}
		}
		else
		{
			throw invalidKeyException("Key not present in map");
		}

		//TODO: handle exception
	}
	
	/**
	 * Erase value bound to key
	 * @param key key to look up
	 * @return true if value successfully erased, otherwise false
	 */
	bool erase(KeyT key)
	{
		std::hash<KeyT> keyHash;
		size_t index = keyHash(key) & (_iCapacity - 1);

		for(unsigned long i = 0; i < _storage[index].size(); ++i)
		{
			if(_storage[index][i].first == key)
			{
				_storage[index].erase(_storage[index].begin() + i);
				_iSize--;

				double ratio = (double) _iSize / _iCapacity;
				if(ratio < _dLower)
				{
					resize(LOWER_FACTOR);
				}

				return true;
			}
		}

		return false;
	}

	/**
	 * Get size of vector that contains key
	 * @param key key to look up
	 * @return
	 */
	int bucketSize(KeyT key)
	{
		std::hash<KeyT> keyHash;
		size_t index = keyHash(key) & (_iCapacity - 1);

		return _storage[index].size();
	}

	/**
	 * Erase all pairs from map
	 */
	void clear()
	{
		for(int vIdx = 0; vIdx < _iCapacity; ++vIdx)
		{
			_storage[vIdx].clear();
		}

		_iSize = DEF_SIZE;
	}
	
	/**
	 * Overload for = operator
	 * @param other HashMap to copy
	 * @return reference to HashMap
	 */
	HashMap<KeyT, ValueT>& operator=(const HashMap<KeyT, ValueT> &other)
	{
		_dLower = other._dLower;
		_dUpper = other._dUpper;
		_iSize = other._iSize;
		_iCapacity = other._iCapacity;
		_storage = other._storage;
		return *this;
	}

	/**
	 * Overload for [] operator, returns value corresponding to given key
	 * @param key key to look up
	 * @return value attached to key
	 */
	const ValueT& operator[](const KeyT &key) const
	{
		std::hash<KeyT> keyHasher;
		size_t newIdx = keyHasher(key) & (_iCapacity - 1);
		for(unsigned long idx = 0; idx < _storage[newIdx].size(); ++idx)
		{
			if(_storage[newIdx][idx].first == key)
			{
				return _storage[newIdx][idx].second;
			}
		}
	}

	/**
	 * Overload for [] operator, creates new pair
	 * @param key key to assign
	 * @return reference to value
	 */
	ValueT& operator[](const KeyT &key)
	{
		/* Instantiate a pair with the given key */
		insert(key, 0);
		int vecIdx = 0;

		/* Look up the pair and return its second value */
		std::hash<KeyT> keyHasher;
		size_t newIdx = keyHasher(key) & (_iCapacity - 1);
		for(unsigned long idx = 0; idx < _storage[newIdx].size(); ++idx)
		{
			if(_storage[newIdx][idx].first == key)
			{
				vecIdx = (int) idx;
			}
		}
		
		return _storage[newIdx][vecIdx].second;
	}

	/**
	 * Overload for == operator, checks that all fields are equal
	 * @param other HashMap to compare to
	 * @return true if all fields are equal, otherwise false
	 */
	bool operator==(const HashMap<KeyT, ValueT> &other) const
	{
		if(_iSize != other._iSize ||
		   _iCapacity != other._iCapacity ||
		   _dUpper != other._dUpper ||
		   _dLower != other._dLower)
		{
			return false;
		}

		for(auto item: other)
		{
			if(!this -> containsKey(item.first))
			{
				return false;
			}

			if(item.second != this -> at(item.first))
			{
				return false;
			}
		}

		return true;
	}

	/**
	 * Overload for != operator
	 * @param other HashMap to compare to
	 * @return opposite of ==
	 */
	bool operator!=(const HashMap<KeyT, ValueT> &other) const
	{
		return  !(operator==(other));
	}

	/**
	 * Nested Iterator class
	 */
	class const_iterator
	{
	public:
		/**
		 * Constructor that accepts the actual storage of the map over which it iterates
		 * @param map pointer to storage
		 * @param mapCap capacity of given map
		 * @param curVec
		 */
		explicit const_iterator(vector<pair<KeyT, ValueT>> *map, int mapCap, int curVec = 0)
		{
			_map = map;
			_curVec = curVec;
			_mapCap = mapCap;

			if(_map != nullptr)
			{
				_mapIt = _map[_curVec].begin();

				/* Find first item in storage, guaranteed to be first item in the vector */
				while(_mapIt == _map[_curVec].end() && _curVec < _mapCap)
				{
					++_curVec;
					_mapIt = _map[_curVec].begin();
				}

				/* Reached the end without finding an element */
				if(_curVec == _mapCap)
				{
					_map = nullptr;
				}
			}
			else
			{
				_curVec = _mapCap;
			}
		}

		/**
		 * Postfix ncrement operator that holds the logic of iterating over the elements in the map
		 * @return next element in iteration
		 */
		const_iterator& operator++()
		{
			/* IF we've reached the last element then the next will be end() */
			if(_curVec + 1 == _mapCap)
			{
				_curVec++;
				_map = nullptr;
				return *this;
			}
			
			/* Use iterator of vector to increment */
			++_mapIt;

			/* If the increment above reached the end of the vecto */
			if(_mapIt == _map[_curVec].end())
			{
				/* Go from vector to vector until you find a pair */
				while(_mapIt == _map[_curVec].end() && (_curVec + 1 != _mapCap))
				{
					++_curVec;
					_mapIt = _map[_curVec].begin();
				}
			}

			return *this;
		}

		/**
		 * Prefix ncrement operator that holds the logic of iterating over the elements in the map
		 * @return next element in iteration
		 */
		const const_iterator operator++(int)
		{
			const_iterator temp = *this;
			operator++();
			return temp;
		}

		/**
		 * Overload for dereference * operator
		 * @return the item in the map to which the iterator points
		 */
		pair<KeyT, ValueT> operator*() const
		{
			return *_mapIt;
		}

		/**
		 * Overload for -> opoerator.
		 * @return
		 */
		pair<KeyT, ValueT>* operator->() const
		{
			return *_mapIt;
		}

		/**
		 * Overload for == operator
		 * @param other iterator to compare to
		 * @return true if operators point to the same location, otherwise false
		 */
		bool operator==(const const_iterator &other) const
		{
			/* For comparison with end() */
			if(this -> _curVec == this -> _mapCap)
			{
				return true;
			}
			return _mapIt == other._mapIt;
		}

		/**
		 * Overload for != operator
		 * @param other iterator to compare to
		 * @return opposite of ==
		 */
		bool operator!=(const const_iterator &other) const
		{
			return !(*this == other);
		}

	private:
		typename vector<pair<KeyT, ValueT>>::iterator _mapIt;
		vector<pair<KeyT, ValueT>> *_map;
		int _curVec;
		int _mapCap;
	};

	/**
	 * Returns first element in iteration
	 * @return
	 */
	const_iterator begin() const
	{
		return const_iterator(this -> _storage, this -> capacity());
	}

	/**
	 * Returns the element "one after the last"
	 * @return
	 */
	const_iterator end() const
	{
		return const_iterator(nullptr, this -> capacity(), this -> capacity());
	}
	
	/**
	 * Returns first element in iteration
	 * @return
	 */
	const_iterator cbegin()
	{
		return const_iterator(this -> _storage, this -> capacity());
	}
	
	/**
	 * Returns the element "one after the last"
	 * @return
	 */
	const_iterator cend()
	{
		return const_iterator(nullptr, this -> capacity(), this -> capacity());
	}

private:
	double _dLower;
	double _dUpper;
	int _iSize;
	int _iCapacity;
	vector<pair<KeyT, ValueT>> *_storage;
};

#endif //CPP_EX3_HASHMAP_HPP