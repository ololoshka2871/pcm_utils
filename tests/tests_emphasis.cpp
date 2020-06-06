#include "CppUnitTest.h"

#include <cassert>

#include "../emphasis/emphasis.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	TEST_CLASS(TestEmphasis)
	{
	public:
		TEST_METHOD(testemphasis)
		{
			float in[] = { 200 };
			float out[std::size(in)];

			float In_Old_OUT;
			float X1_OUT;
			const float sampleRate = 44100.0;

			deemphasis(0, 0, in, out, std::size(in), &In_Old_OUT, &X1_OUT, sampleRate);
		}
	};
}