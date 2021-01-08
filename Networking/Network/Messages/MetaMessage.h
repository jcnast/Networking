#include "Message.h"
#include "WrappedMessage.h"

namespace Message
{
	class MetaMessage : public IMessageType<std::vector<std::shared_ptr<WrappedMessage>>>
	{
	public:
		MetaMessage();

		int GetMessageHash() override;
		uint32_t GetMessageSize() override;

		void FromBytes(std::vector<std::byte> bytes) override;

		std::vector<std::byte> AsBytes() override;

		std::shared_ptr<void> AsObject() override;
		std::shared_ptr<std::vector<std::shared_ptr<WrappedMessage>>> AsType() override;

		void AddWrappedMessage(std::shared_ptr<WrappedMessage> message);

	private:
		bool _recalculateBytes = true;

		uint32_t TotalMessageSize();
	};
}