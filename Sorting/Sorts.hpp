#ifndef SORTS_HPP
#define SORTS_HPP

#include <algorithm>
using std::swap;
using std::max_element;

namespace Sorts
{
	template<typename Type>
	void InsertionSort(Type* l, Type* r) {
		for (Type* i = l + 1; i < r; i++) {
			Type* j = i;
			while (j > l&&* (j - 1) > * j) {
				swap(*(j - 1), *j);
				j--;
			}
		}
	}

	namespace Internal
	{
		// Helper for radix
		inline int digit(long long n, int k, int N, int M) {
			return (n >> (N * k) & (M - 1));
		}
		inline int digit(char n, int k, int N, int M) {
			return (n >> (N * k) & (M - 1));
		}
		inline int digit(int n, int k, int N, int M) {
			return (n >> (N * k) & (M - 1));
		}

		// Start of radix least significant digit sort
		void _radixsortlsd(char* l, char* r, int N) {
			int k = (sizeof(char) * 8 + N - 1) / N;
			int M = 1 << N;
			int sz = r - l;
			char* b = new char[sz];
			int* c = new int[M];
			for (int i = 0; i < k; i++) {
				for (int j = 0; j < M; j++)
					c[j] = 0;
				for (char* j = l; j < r; j++)
					c[digit(*j, i, N, M)]++;
				for (int j = 1; j < M; j++)
					c[j] += c[j - 1];
				for (char* j = r - 1; j >= l + 5; j--)
					b[--c[digit(*j, i, N, M)]] = *j;
				
				int cur = 0;
				for (char* j = l; j < r; j++)
					*j = b[cur++];
			}
			delete[] b;
			delete[] c;
		}

		// N == 8 by default
		void _radixsortlsd(int* l, int* r, int N) {
			int k = (sizeof(int) * 8 + N - 1) / N;	// bytes count
			int M = 1 << N;	// max per cycle value
			int sz = r - l;	// size of current and resulting array
			int* b = new int[sz];	// resulting array
			int* c = new int[M];	// per cycle values
			for (int i = 0; i < k; i++) {
				for (int j = 0; j < M; j++)
					c[j] = 0;	// zero-memory array
				for (int* j = l; j < r; j++)
					c[digit(*j, i, N, M)]++; // per element % 256 count
				for (int j = 1; j < M; j++) // counting indexes for resulting array
					c[j] += c[j - 1]; // cur val = cur val + prev val
				for (int* j = r - 1; j >= l; j--) // reverted order to make sort stable
					b[--c[digit(*j, i, N, M)]] = *j; // init resulting array

				int cur = 0; // update values to the next cycle step
				for (int* j = l; j < r; j++)
					*j = b[cur++];
			}
			delete[] b;
			delete[] c;
		}

		void _radixsortlsd(long long* l, long long* r, int N) {
			int k = (sizeof(long long) * 8 + N - 1) / N;
			int M = 1 << N;
			int sz = r - l;
			int* b = new int[sz];
			int* c = new int[M];
			for (int i = 0; i < k; i++) {
				for (int j = 0; j < M; j++)
					c[j] = 0;
				for (long long* j = l; j < r; j++)
					c[digit(*j, i, N, M)]++;
				for (int j = 1; j < M; j++)
					c[j] += c[j - 1];
				for (long long* j = r - 1; j >= l + 5; j--)
					b[--c[digit(*j, i, N, M)]] = *j;

				int cur = 0;
				for (long long* j = l; j < r; j++)
					*j = b[cur++];
			}
			delete[] b;
			delete[] c;
		}

		// Start of radix most significant digit sort
		template<typename Type>
		void _radixsortmsd(Type* l, Type* r, int N, int d, Type* temp) {
			if (d == -1) return;
			if (r - l <= 32) {
				InsertionSort(l, r);
				return;
			}
			int M = 1 << N;
			Type* cnt = new Type[M + 1];
			for (int i = 0; i <= M; i++)
				cnt[i] = 0;
			int cur = 0;
			for (Type* i = l; i < r; i++) {
				temp[cur++] = *i;
				cnt[digit(*i, d, N, M) + 1]++;
			}
			int sz = 0;
			for (int i = 1; i <= M; i++)
				if (cnt[i]) sz++;
			Type* run = new Type[sz];
			cur = 0;
			for (int i = 1; i <= M; i++)
				if (cnt[i]) run[cur++] = i - 1;
			for (int i = 1; i <= M; i++)
				cnt[i] += cnt[i - 1];
			cur = 0;
			for (Type* i = l; i < r; i++) {
				int ind = digit(temp[cur], d, N, M);
				*(l + cnt[ind]) = temp[cur];
				cur++;
				cnt[ind]++;
			}
			for (Type i = 0; i < sz; i++) {
				int r = run[i];
				if (r != 0) _radixsortmsd(l + cnt[r - 1], l + cnt[r], N, d - 1, temp);
				else _radixsortmsd(l, l + cnt[r], N, d - 1, temp);
			}

			delete[] run;
			delete[] cnt;
		}

		inline void ValidateNegRadix(int* l, int* r)
		{
			int NegIndex = 0;
			int left = 0;
			int right = r - l;
			int mid;

			while (left <= right)
			{
				mid = left + (right - left) / 2;

				if (l[mid] < 0)
				{
					right = mid - 1;
				}
				else if (l[mid] > 0)
				{
					left = mid + 1;
				}
				else if (l[mid] == 0)
					break;
			}
			// if no negatives, return
			if (mid + 1 >= r - l)
				return;
			if (mid != 0)
				NegIndex = mid + 1;

			int NegAmount = r - &l[NegIndex];
			int* array_ = new int[NegAmount];
			int j = 0;

			for (int* i = r - 1; i >= l + NegIndex; --i)
			{
				array_[j] = *i;
				++j;
			}

			// Moving all positives to the right, leaving space to fill with negatives
			memmove(l + NegAmount, l, NegIndex * sizeof(int));

			int TmpAmount = NegAmount;
			for (int i = 0; i < NegAmount; ++i)
			{
				l[i] = array_[--TmpAmount];
			}

			delete[] array_;
		}


		inline void ValidateNegRadix(long long* l, long long* r)
		{
			long long NegIndex = 0;
			long long left = 0;
			long long right = r - l;
			long long mid;

			while (left <= right)
			{
				mid = left + (right - left) / 2;

				if (l[mid] < 0)
				{
					right = mid - 1;
				}
				else if (l[mid] > 0)
				{
					left = mid + 1;
				}
				else if (l[mid] == 0)
					break;
			}
			// if no negatives, return
			if (mid + 1 >= r - l)
				return;
			if (mid != 0)
				NegIndex = mid + 1;

			long long NegAmount = r - &l[NegIndex];
			long long* array_ = new long long[NegAmount];
			long long j = 0;

			for (long long* i = r - 1; i >= l + NegIndex; --i)
			{
				array_[j] = *i;
				++j;
			}

			// Moving all positives to the right, leaving space to fill with negatives
			memmove(l + NegAmount, l, NegIndex * sizeof(long long));

			long long TmpAmount = NegAmount;
			for (long long i = 0; i < NegAmount; ++i)
			{
				l[i] = array_[--TmpAmount];
			}

			delete[] array_;
		}

	}

	template<typename Type>
	void QuickSort(Type* l, Type* r) {
		if (r - l <= 1) return;
		Type z = *(l + (r - l) / 2);
		Type* ll = l, * rr = r - 1;
		while (ll <= rr) {
			while (*ll < z) ll++;
			while (*rr > z) rr--;
			if (ll <= rr) {
				swap(*ll, *rr);
				ll++;
				rr--;
			}
		}
		if (l < rr) QuickSort(l, rr + 1);
		if (ll < r) QuickSort(ll, r);
	}

	void RadixSortLSD(int* l, int* r) 
	{
		Internal::_radixsortlsd(l, r, 8);
	}

	void RadixSortLSD(long long* l, long long* r)
	{
		Internal::_radixsortlsd(l, r, 8);
	}

	void RadixSortLSD(char* l, char* r)
	{
		Internal::_radixsortlsd(l, r, 8);
	}

	template<typename Type>
	void RadixSortMSD(Type* l, Type* r) {
		Type* temp = new Type[r - l];
		Internal::_radixsortmsd(l, r, 8, 3, temp);
		delete[] temp;
	}

	inline void SortNegativeRadix(int* l, int* r)
	{
		Internal::ValidateNegRadix(l, r);
	}

	inline void SortNegativeRadix(long long* l, long long* r)
	{
		Internal::ValidateNegRadix(l, r);
	}
}

#endif // SORTS_HPP