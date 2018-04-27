#ifndef VECUTIL_H
#define VECUTIL_H

#include <vector>
#include <set>

namespace VecUtil {

	// insertions occur at vector positions then fix index pointers in fixme
	// ex. [a, b, c, d], insertions: a[1] = e, a[3] = f
	//     [a, e, b, f, c, d]
	// fixme [0, 1, 2, 3] -> [0, 2, 4, 5]
	std::vector<size_t> fixIndicesAfterInsert(const std::vector<size_t> &fixme, const std::set<size_t> &changes);

	std::vector<size_t> fixIndicesAfterRemove(const std::vector<size_t> &fixme, const std::set<size_t> &changes);

	std::vector<size_t> fixIndicesAfterMove(const std::vector<size_t> &fixme,
		const std::vector<std::pair<size_t, size_t>> &moves);

	// inserts multiple items into a vector
	// expects the insertions to be sorted
	// O(n)
	template <typename T>
	void multiInsert(std::vector<T> *old,
		const std::vector<std::pair<size_t, T>> &insertions)
	{
		std::vector<T> new_items;
		size_t new_size = old->size() + insertions.size();
		new_items.reserve(new_size);

		// perform merge
		size_t old_i = 0;
		auto it = insertions.begin();
		while (new_items.size() < new_size) {
			bool add_new;
			if (it == insertions.end()) {
				add_new = false;
			}
			else if (old_i >= old->size()) {
				add_new = true;
			}
			else if (it->first == new_items.size()) {
				add_new = true;
			}
			else {
				add_new = false;
			}
			if (add_new) {
				new_items.push_back(it->second);
				it++;
			}
			else {
				new_items.push_back((*old)[old_i]);
				old_i++;
			}
		}
		*old = new_items;
	}

	// removes items from a vector
	// expects the indices to be sorted
	// O(n)
	template <typename T>
	void multiRemove(std::vector<T> *old, const std::vector<size_t> &indices)
	{
		std::vector<T> new_list;
		auto it = indices.begin();
		for (size_t i = 0; i < old->size(); i++) {
			// push old
			if (it == indices.end() || *it > i) {
				new_list.push_back((*old)[i]);
			}
			// removed - push nothing
			else if (*it == i) {
				// dont push anything
				it++;
			}
			// out of order?
			else {
			}
		}
		*old = new_list;
	}

	// moves items of a vector
	// O(n)
	template <typename T>
	void multiMove(std::vector<T> *vec, const std::vector<std::pair<size_t, size_t>> &mapping)
	{
		// build removals and insertions
		std::vector<size_t> removals;
		std::vector<std::pair<size_t, T>> insertions;
		for (auto &pair : mapping) {
			size_t src = pair.first;
			size_t dest = pair.second;
			T item = (*vec)[pair.first];
			removals.push_back(src);
			insertions.push_back({ dest, item });
		}
		// sort both
		std::sort(removals.begin(), removals.end());
		std::sort(insertions.begin(), insertions.end(),
			[](auto &left, auto &right) { return left.first < right.first; });
		multiRemove(vec, removals);
		multiInsert(vec, insertions);
	}

	// clumps a vector of indices into a vector of range pairs
	// expects a sorted vector
	// ex. {1,2,3,5,6,7,8,9} => {{1,3}, {5,9}}
	std::vector<std::pair<size_t, size_t>> clumpify(const std::vector<size_t> &indices);

	// returns {index, count} pairs instead of {begin, end}
	std::vector<std::pair<size_t, size_t>> clumpify3(const std::vector<size_t> &indices);

	// clumps a vector of index/T pairs into {index, vec<T>} pairs
	// expects a sorted input, output is sorted
	// ex. {1a, 2b, 4x, 5w} => {{1, ab}, {4, xw}}
	template <typename T>
	std::vector<std::pair<size_t, std::vector<T>>>
		clumpify2(const std::vector<std::pair<size_t, T>> &list)
	{
		if (list.size() == 0) return {};
		std::vector<std::pair<size_t, std::vector<T>>> output;

		size_t start = 0;
		size_t prev = 0;
		std::vector<T> clump;
		for (const auto &it : list) {
			// start case
			if (clump.size() == 0) {
				start = it.first;
				prev = start;
				clump.push_back(it.second);
			}
			// continue case
			else if (it.first == prev + 1) {
				clump.push_back(it.second);
				prev++;
			}
			// end clump
			else if (it.first > prev + 1) {
				output.push_back({ start, clump });
				clump.clear();

				start = it.first;
				prev = start;
				clump.push_back(it.second);
			}
			else {
				// invalid input
				return {};
			}
		}
		// always make range at end
		if (clump.size() > 0) {
			output.push_back({ start, clump });
		}
		return output;
	}

	void removalsInsertionsMoves(const std::vector<size_t> &before,
		const std::vector<size_t> &after,
		std::vector<size_t> *removals, // input indices removed, sorted
		std::vector<size_t> *insertions, // output indices inserted, sorted
		std::vector<std::pair<size_t, size_t>> *moves); // input to output pairs moved, sorted by first

}

#endif