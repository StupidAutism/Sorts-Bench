# Sorts Bench
Comparing time complexity of Quick and Radix sorts  
## Usage: 
```cpp
#include "Benchmark.hpp"

int main()
{
	CBenchmark* bench = new CBenchmark();
    if (!bench->LoadSettingsFile("BenchProps.txt"))
	{
		wprintf(L"error\n");
		return -1;
	}
    bench->Start();
    delete bench;
    // Results of bench in 'BenchRes.txt' file
}
```
Also u can use sorts themselves:  
```cpp
#include "Sorts.hpp"

int main()
{
	int Arr[] = {1, 5, 3192, 12, 0, -55, -100};
    int szArr = sizeof(Arr) / sizeof(int);
    
    // Sorts with Quick with negatives
    Sorts::QuickSort(Arr, Arr + szArr);
   	
    // Sorts with Radix with negatives
    Sorts::RadixSortLSD(Arr, Arr + szArr);
    Sorts::SortNegativeRadix(Arr, Arr + szArr);
    
    // Sorts with Radix without sorting negatives values
    Sorts::RadixSortLSD(Arr, Arr + szArr);
}
```