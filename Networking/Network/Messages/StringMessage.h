#include "Message.h"

namespace Message
{
	class StringMessage : public IMessageType<std::string>
	{
	public:
		StringMessage();
		StringMessage(std::string message);
		StringMessage(std::vector<std::byte> bytes);

		int GetMessageHash() override;
		uint32_t GetMessageSize() override;

		void FromBytes(std::vector<std::byte> bytes) override;

		std::vector<std::byte> AsBytes() override;

		std::shared_ptr<void> AsObject() override;
		std::shared_ptr<std::string> AsType() override;

	private:
		std::shared_ptr<std::string> _message;
		std::vector<std::byte> _bytes;
	};
}