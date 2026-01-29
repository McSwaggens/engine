#ifndef PAIR_H
#define PAIR_H

template<typename Key, typename Value>
struct Pair {
	Key key;
	Value value;

	Pair(Key key, Value value) : key(key), value(value) { }
	Pair() = default;

	bool operator ==(const Pair& other) const { return key == other.key && value == other.value; }
	bool operator !=(const Pair& other) const { return !(*this == other); }
};

#endif // PAIR_H
