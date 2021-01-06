#include <cstddef>
#include <memory>
#include <vector>

/*

Really basic serialization, to bytes only. In a more through implementation, this should take in a type of serialization/objects and deserialize it from there

*/
namespace Serialization
{
	namespace
	{
		// first try object's custom serialization method
		template <typename T>
		std::vector<std::byte> InternalSerialize(const T& object)
		{
			return object.Serialize();
		}

		// then try trivially copying it
		template <typename T>
		std::vector<std::byte> InternalSerialize(const T& object)
		{
			static_assert(std::is_trivially_copyable<T>::value, "Type is not trivially copyable!");

			std::vector<std::byte> bytes(sizeof(T));

			const std::byte* begin = reinterpret_cast<const std::byte*>(std::addressof(object));
			const std::byte* end = begin + sizeof(T);

			std::copy(begin, end, bytes);

			return bytes;
		}
		// any other types can't be serialized

		//*********************************************************************************

		// first try object's custom deserialization method
		template <typename T>
		T Deserialize(const std::vector<std::byte>& bytes)
		{
			T newObject;

			newObject.Deserialize(bytes);

			return newObject;
		}

		// then try trivially copying it
		template <typename T>
		T Deserialize(const std::vector<std::byte>& bytes)
		{
			static_assert(std::is_trivially_copyable<T>::value, "Type is not trivially copyable");

			T newObject;

			std::byte* begin_object = reinterpret_cast<std::byte*>(std::addressof(newObject));
			std::copy(bytes.begin(), bytes.end(), begin_object);

			return newObject;
		}
		// any other types can't be deserialized
	}

	template <typename T>
	std::vector<std::byte> Serialize(const T& object)
	{
		return InternalSerialize<T>(object);
	}

	template <typename T>
	T Deserialize(const std::vector<std::byte>& bytes)
	{
		return InternalDeserialize<T>(bytes);
	}
}