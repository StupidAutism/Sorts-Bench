#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

using std::cout; using std::endl;

#ifdef __linux__
#define CLEAR_CONSOLE() system("clear")
#elif _WIN32
#define CLEAR_CONSOLE() system("CLS")
#endif

#include <random>
#include <fstream>
#include <charconv>

#include "Sorts.hpp"
#include "Timer.hpp"

typedef struct 
{
	int n;
	int min;
	int max;
} BenchSets, *PBenchSets;

constexpr const char* sortNames[] = { "Quick Sort", "Radix Sort" };
constexpr int szSortNames = sizeof(sortNames) / sizeof(const char*);

typedef struct  
{
	int Sort;
	char* sortName;
	float AvgTime;
} BenchRes, *PBenchRes;

class CBenchmark
{
	Timer::CTimer*	timer_;
	std::ifstream	inSettings_;
	std::ofstream	outResults_;

	BenchSets*		benchSets_;
	unsigned		szBenches_;
	bool*			isFinished;

	BenchRes Results_[szSortNames];

	enum SORTS
	{
		QUICK,
		RADIX,
		LAST_ELEMENT
	};

	[[maybe_unused]] inline bool CheckStdRes(const std::from_chars_result& res)
	{
		if (res.ec == std::errc::invalid_argument ||
			res.ec == std::errc::result_out_of_range)
			return false;
		return true;
	}

	inline bool ParseInputString(std::string& str, BenchSets* CurBench)
	{
		if (str.find('/') != std::wstring::npos)
			return false;
		if (str.size() <= 4)
			return false;

		std::size_t fPos;
		int index = 0;

		fPos = str.find(L' ');
		auto stdRes = std::from_chars(str.data(), str.data() + fPos, CurBench->n);
		// if something goes wrong, then check 'stdRes' var
		CheckStdRes(stdRes);
		str.erase(str.begin(), str.begin() + fPos + 1);

		fPos = str.find(L' ');
		stdRes = std::from_chars(str.data(), str.data() + fPos, CurBench->min);
		str.erase(str.begin(), str.begin() + fPos + 1);

		stdRes = std::from_chars(str.data(), str.data() + str.size(), CurBench->max);

		return true;
	}

	inline void AllocArray(int** arr, const int& szArr)
	{
		*arr = new int[szArr];
	}

	inline void InitArray(int* arr, const int& szArr, const int& min, const int& max)
	{
		static std::random_device rd;
		static std::mt19937 mt(rd());
		std::uniform_int_distribution<> dist(min, max);

		for (int i = 0; i < szArr; ++i)
		{
			arr[i] = dist(mt);
		}
	}

	inline void CopyArray(int* from, int* to, const int& szArr)
	{
		memcpy(to, from, szArr * sizeof(int));
	}

	inline void FreeArray(int* to_free)
	{
		delete[] to_free;
	}

	void BenchCharArrayEx()
	{
		int szStr = 100000000;
		char* QuickStr;
		char* RadixStr;

		QuickStr = new char[szStr];
		for (int i = 0; i < szStr; ++i)
			QuickStr[i] = i & 0xff;

		timer_->Start();
		Sorts::QuickSort(QuickStr, QuickStr + szStr);
		float QuickTime = timer_->StopAndGet() / CLOCKS_PER_SEC;

		delete[] QuickStr;

		RadixStr = new char[szStr];
		for (int i = 0; i < szStr; ++i)
			RadixStr[i] = i & 0xff;

		timer_->Start();
		Sorts::RadixSortLSD(QuickStr, QuickStr + szStr);
		float RadixTime = timer_->StopAndGet() / CLOCKS_PER_SEC;

		delete[] RadixStr;

		ExportToFileEx(QuickTime, RadixTime);
	}

	void DefualtBench()
	{
		int PrevNval = 0;
		for (int i = 0; i < szBenches_; ++i)
		{
			BenchSets BSet = benchSets_[i];
			if (BSet.n == 0)
				continue;

			if (BSet.n != PrevNval)
			{
				PrevNval = BSet.n;
				for (auto&& element : Results_)
					element.AvgTime = 0.f;
			}

			int* MainArray;
			int* TempArray;
			AllocArray(&MainArray, BSet.n);
			AllocArray(&TempArray, BSet.n);
			InitArray(MainArray, BSet.n, BSet.min, BSet.max);

			for (int j = SORTS::QUICK; j < SORTS::LAST_ELEMENT; ++j)
			{
				// make every bench 5 times
				for (int k = 0; k < 5; ++k)
				{
					CopyArray(MainArray, TempArray, BSet.n);
					switch (j)
					{
					case SORTS::QUICK:
						timer_->Start();
						Sorts::QuickSort(TempArray, TempArray + BSet.n);
						Results_[j].AvgTime += timer_->StopAndGet();

						break;
					case SORTS::RADIX:
						timer_->Start();
						Sorts::RadixSortLSD(TempArray, TempArray + BSet.n);
						if (int isNeg1 = BSet.min & 0x80000000, isNeg2 = BSet.max & 0x80000000; isNeg1 || isNeg2)
							Sorts::SortNegativeRadix(TempArray, TempArray + BSet.n);

						Results_[j].AvgTime += timer_->StopAndGet();

						break;
					default:
						break;
					}
				}
				Results_[j].AvgTime /= 5.f;
				Results_[j].AvgTime /= CLOCKS_PER_SEC;
			}
			this->ExportToFile(BSet.n, BSet.min, BSet.max);

			FreeArray(MainArray);
			FreeArray(TempArray);

		}
	}

	void ExportToFile(const int& n, const int& min, const int& max)
	{
		for (auto&& element : Results_)
		{
			static const char* separator = "";
			outResults_ << separator << "---------------------------------------------";
			separator = "\n";
			outResults_ << separator << n << " elements from " << min << " to " << max;
			outResults_ << separator << element.sortName << " exec time: " << element.AvgTime << " seconds";
			outResults_ << separator << "---------------------------------------------\n";
		}
	}

	void ExportToFileEx(const float& quick, const float& radix)
	{
		const char* separator = "\n";

		outResults_ << separator << "---------------------------------------------";
		outResults_ << separator << "100.000.000 length string sorted by quick in " << quick << " seconds";
		outResults_ << separator << "100.000.000 length string sorted by radix in " << radix << " seconds";
	}

public:
	CBenchmark() : benchSets_(nullptr), szBenches_(10), Results_{}
	{
		this->timer_ = new Timer::CTimer();
		this->benchSets_ = (PBenchSets)malloc(szBenches_ * sizeof(BenchSets));
		if (this->benchSets_ == nullptr)
		{
			wprintf(L"bad malloc! exiting...");
			exit(-0x100);
		}
		memset(this->benchSets_, 0, szBenches_ * sizeof(BenchSets));
		this->isFinished = new bool(true);

		for (int i = 0; i < szSortNames; ++i)
		{
			Results_[i].sortName	= (char*)sortNames[i];
			Results_[i].Sort		= i;
			Results_[i].AvgTime		= 0.0f;
		}

		outResults_.open("BenchRes.txt");
	}
	~CBenchmark()
	{
		delete this->timer_;
		if (this->benchSets_ != nullptr)
			free(this->benchSets_);
		delete this->isFinished;

		outResults_.close();
	}

	bool LoadSettingsFile(const char* filename)
	{
		inSettings_.open(filename);
		if (!inSettings_.is_open())
			return false;

		std::string tmpBuffer;
		int CurElement = 0;
		while (!inSettings_.eof())
		{
			if (CurElement >= szBenches_ - 1)
			{
				szBenches_ += 10;
				auto TmpPBuff = (PBenchSets)realloc(benchSets_, szBenches_ * sizeof(BenchSets));
				if (TmpPBuff == nullptr)
				{
					wprintf(L"bad realloc! exiting...");
					exit(-0x101);
				}
				benchSets_ = TmpPBuff;
				memset(benchSets_ + szBenches_ - 10, 0, szBenches_ - 10);
			}

			std::getline(inSettings_, tmpBuffer);
			if (!ParseInputString(tmpBuffer, &benchSets_[CurElement]))
				continue;
			++CurElement;
		}

		inSettings_.close();
		return true;
	}

	void Start()
	{
		*this->isFinished = false;

		this->DefualtBench();
		this->BenchCharArrayEx();
		*this->isFinished = true;
	}

	bool* IsFinished() { return this->isFinished; }
};

#endif // BENCHMARK_HPP 