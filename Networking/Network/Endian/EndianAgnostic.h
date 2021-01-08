
namespace Endian
{
	/*
	 
	 We are going to proceed without caring about endian-ness for now, as it seems that most of the standard architecture runs on little-endian.
	 Networking standard is big endian, but we can choose our own standard for now...

	 Sources:
	 - https://softwareengineering.stackexchange.com/questions/316566/can-i-safely-ignore-byte-order-in-networking
	 - https://exceptionshub.com/how-to-write-endian-agnostic-cc-code.html
	 - https://en.wikipedia.org/wiki/Endianness#Current_architectures (current architectures)

	 Endianness breakdown: https://www.ibm.com/developerworks/library/l-ibm-xl-c-cpp-compiler/index.html

	 Code reference: https://www.realtime.bc.ca/articles/endian-safe.html

	 General notes:
		- How does endianness impact complex structs?
			struct Complex
			{
				uint32_t UINT1;
				uint32_t UINT2;
				uint8_t SHORT;
				bool BOOL;
			}
			- Is just 'Complex' switched, are all sub-fields also switched, or does it not matter since achieving one achieves the other?
		- For networking/serializing we should pay some attention to struct creation/layout to minimize space where possible
			- https://www.joshcaratelli.com/blog/struct-packing
		- Padding (and alignment?) is compiler dependent, so we will need to make sure that server and client code is compiled in the same way to ensure compatibility
			- Otherwise things could break even if we handle endianness correctly
		- Considerations: https://www.gamedev.net/reference/articles/article2242.asp
		- https://stackoverflow.com/questions/7895869/cross-platform-alignx-macro
		- https://stackoverflow.com/questions/3261227/how-to-serialize-an-object-to-send-over-network
	 
	*/
	namespace SystemEndianness
	{
		class Endianness
		{
			union
			{
				int i;
				char c[sizeof(int)];
			};

			constexpr Endianness() : i(1) {}
		};

		Endianesss _systemEndianness;

		constexpr bool IsBig()
		{
			return c[0] == 0;
		}

		constexpr bool IsLittle()
		{
			return c[0] != 0;
		}
	}

	template <typename T, bool BigFirst>
	class Endian
	{
	public:
		Endian() {}
		Endian(const T& value) : _value(value) {}
		operator T() const { return Swap(_value); }

	private:
		T _value;

		static T Swap(const T& arg)
		{
			if (SystemEndianness::IsBig() == BigFirst)
			{
				return arg;
			}
			else
			{
				T ret;

				char* dst = reinterpret_cast<char*>(&ret);
				const char* src = reinterpret_cast<const char*>(&arg + 1);

				for (size_t i = 0; i < sizeof(T); i++)
					*dst++ = *--src;

				return ret;
			}
		}
	};

	template <typename T>
	class BigEndian : public Endian<T, true>
	{
	public:
		BigEndian(const T& value) : Endian<T, true>(value) {}
	};

	template <typename T>
	class LittleEndian : public Endian<T, false>
	{
	public:
		LittleEndian(const T& value) : Endian<T, false>(value) {}
	};
}