#include "VecUtil.h"

#include <algorithm>
#include <unordered_map>

std::vector<size_t> VecUtil::fixIndices(const std::vector<size_t> &fixme, const std::set<size_t>& insertions)
{
	size_t max_index = 0;
	for (size_t x : fixme) {
		max_index = std::max((size_t)x, max_index);
	}
	// build delta_array, [+0, +0, +1, +1, +2, +2, +2]
	std::vector<size_t> delta_array(max_index + 1, 0);
	size_t shift = 0;
	auto it = insertions.begin();
	size_t old_index = 0;
	size_t new_index = 0;
	while (old_index < delta_array.size()) {
		if (it == insertions.end() || new_index != *it) {
			delta_array[old_index] = shift;
			old_index++;
			new_index++;
		}
		else {
			shift++;
			it++;
			new_index++;
		}
	}
	//for (size_t i = 0; i < delta_array.size(); i++) {
	//	if (it != insertions.end() && i == *it) {
	//		shift++;
	//		it++;
	//	}
	//	delta_array[i] = shift;
	//}
	std::vector<size_t> result(fixme);
	for (size_t i = 0; i < result.size(); i++) {
		size_t old_index = result[i];
		result[i] = old_index + delta_array[old_index];
	}
	return result;
	// Set version, I was wondering which would be faster
	// Should be O(nlogn) vs O(n), apparently this is really slow even for small n
	// ex: insert [1, 4, 6]
	// -> (1: 0, 4: 1, 6: 2)
	// < 1 gets bumped +0
	// >= 1, < 4 gets bumped +1
	// >= 4, < 6 gets bumped +2
	//std::map<int, int> index_to_delta;
	//int delta = 0;
	//for (int index : insertions) {
	//	index_to_delta[index] = delta;
	//	delta++;
	//}
	//std::vector<int> result(fixme);
	//for (size_t i = 0; i < result.size(); i++) {
	//	int x = result[i];
	//	auto it = std::upper_bound(insertions.begin(), insertions.end(), x);
	//	int d;
	//	if (it == insertions.end()) {
	//		d = insertions.size();
	//	}
	//	else {
	//		d = index_to_delta[*it];
	//	}
	//	result[i] = x + d;
	//}
	//return result;
}

std::vector<size_t> VecUtil::fixIndicesRemove(const std::vector<size_t>& fixme, const std::set<size_t>& changes)
{
	if (fixme.size() == 0) return {};
	size_t max = *std::max_element(fixme.begin(), fixme.end());
	std::vector<int> delta_array(max + 1, 0);
	int shift = 0;
	for (size_t i = 0; i < delta_array.size(); i++) {
		if (changes.find(i) != changes.end()) {
			shift--;
		}
		delta_array[i] = shift;
	}

	std::vector<size_t> result(fixme);
	for (size_t i = 0; i < result.size(); i++) {
		size_t old_index = result[i];
		result[i] = old_index + delta_array[old_index];
	}

	return result;
}

std::vector<std::pair<size_t, size_t>> VecUtil::clumpify(const std::vector<size_t>& indices)
{
	if (indices.size() == 0) return {};
	size_t start = indices[0];
	size_t prev = indices[0];
	std::vector<std::pair<size_t, size_t>> output;

	for (size_t x : indices) {
		// continue case
		if (x == prev + 1) {
		}
		// make range
		else if (x > prev + 1) {
			output.push_back({ start, prev });
			start = x;
		}
		// first item, or a bug if unsorted
		else {
		}
		prev = x;
	}
	// always make range at end
	output.push_back({ start, prev });
	return output;
}

std::vector<std::pair<size_t, size_t>> VecUtil::clumpify3(const std::vector<size_t>& indices)
{
	// (begin, end) pairs
	std::vector<std::pair<size_t, size_t>> begin_end_pairs;
	begin_end_pairs = VecUtil::clumpify(indices);

	// (index, count) pairs
	std::vector<std::pair<size_t, size_t>> index_count_pairs;

	for (const auto &pair : begin_end_pairs) {
		size_t begin = pair.first;
		size_t end = pair.second;
		index_count_pairs.push_back({ begin, end - begin + 1 });
	}
	return index_count_pairs;
}


void VecUtil::removalsInsertionsMoves(const std::vector<size_t>& before, const std::vector<size_t>& after, std::vector<size_t>* removals, std::vector<size_t>* insertions, std::vector<std::pair<size_t, size_t>>* moves)
{
	// this is so silly

	// maps value to index
	std::unordered_map<size_t, size_t> old_rmap;
	for (size_t i = 0; i < before.size(); i++) {
		old_rmap[before[i]] = i;
	}

	std::unordered_map<size_t, size_t> new_rmap;
	for (size_t i = 0; i < before.size(); i++) {
		new_rmap[after[i]] = i;
	}

	// things in old but not in new are removals
	for (size_t i = 0; i < before.size(); i++) {
		size_t v = before[i];
		auto it = new_rmap.find(v);
		if (it == new_rmap.end()) {
			removals->push_back(i);
		}
		// things in both are moves
		else {
			size_t where = it->second;
			if (i != where) moves->push_back({ i, where });
		}
	}

	// things in new but not in old are insertions
	for (size_t i = 0; i < after.size(); i++) {
		size_t v = after[i];
		if (old_rmap.find(v) == old_rmap.end()) {
			insertions->push_back(i);
		}
	}
}