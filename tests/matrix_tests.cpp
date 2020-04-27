#include "stdafx.h"

#include "CppUnitTest.h"

#include <cassert>

#include "../pcm_utils/DataBlock.h"
#include "../pcm_utils/Q_calc.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	static const DataBlock testblock{
		{1,0,1,1,1,1,0,0,0,0,1,1,1,1},
		{1,0,1,1,1,1,0,0,0,0,1,1,1,1},
		{1,0,1,1,1,1,0,0,0,0,1,1,1,1},
		{1,0,1,1,1,1,0,0,0,0,1,1,1,1},
		{1,0,1,1,1,1,0,0,0,0,1,1,1,1},
		{1,0,1,1,1,1,0,0,0,0,1,1,1,1},
		{1,0,1,1,1,1,0,0,0,0,1,1,1,1},
		{1,0,1,1,1,1,0,0,0,0,1,1,1,1}
		/*
		{1,1,1,1,0,0,0,0,1,1,1,1,0,1},
		{1,1,1,1,0,0,0,0,1,1,1,1,0,1},
		{1,1,1,1,0,0,0,0,1,1,1,1,0,1},
		{1,1,1,1,0,0,0,0,1,1,1,1,0,1},
		{1,1,1,1,0,0,0,0,1,1,1,1,0,1},
		{1,1,1,1,0,0,0,0,1,1,1,1,0,1},
		{1,1,1,1,0,0,0,0,1,1,1,1,0,1},
		{1,1,1,1,0,0,0,0,1,1,1,1,0,1}
		*/
	};

	TEST_CLASS(TestQ)
	{
	public:
		TEST_METHOD(CorrectP)
		{
			assert(testblock.Correct_P() == Sample{});
		}

		TEST_METHOD(CorrectQ)
		{
			assert((testblock.Correct_Q() == 
				Sample{ 0,1,1,1,1,1,1,0,0,1,0,1,0,0 }
				//Sample{ 0,0,1,0,1,0,0,1,1,1,1,1,1,0 }
			));
		}

		TEST_METHOD(Restore) 
		{
			static const DataBlock correctBlock{
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
			};

			static const DataBlock errorBlock{
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0},
				{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
			};

			//auto correctBlock_be = correctBlock.Reversed();
			//auto errorBlock_be = errorBlock.Reversed();
			DataBlock correctBlock_le{ correctBlock };
			DataBlock errorBlock_le{ errorBlock };

			auto err1_pos = DataBlock::ElementNames::R1;
			auto err2_pos = DataBlock::ElementNames::R2;

			Sample restored1;
			Sample restored2;

			RestoreByQ(errorBlock_le.ptr(), static_cast<int>(err1_pos), static_cast<int>(err2_pos),
				restored1.ptr(), restored2.ptr());

			errorBlock_le[err1_pos] = restored1;
			errorBlock_le[err2_pos] = restored2;

			assert((correctBlock_le == errorBlock_le));
		}

		TEST_METHOD(RestoreP)
		{
			static const DataBlock correctBlock{
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
			};

			static const DataBlock errorBlock{
				{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
				{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
			};

			DataBlock correctBlock_le{ correctBlock };
			DataBlock errorBlock_le{ errorBlock };

			auto err1_pos = DataBlock::ElementNames::L0;
			auto err2_pos = DataBlock::ElementNames::P;

			Sample restored1;
			Sample restored2;

			RestoreByQ(errorBlock_le.ptr(), static_cast<int>(err1_pos), static_cast<int>(err2_pos),
				restored1.ptr(), restored2.ptr());

			errorBlock_le[err1_pos] = restored1;
			errorBlock_le[err2_pos] = restored2;

			assert((correctBlock_le == errorBlock_le));
		}
	};
}