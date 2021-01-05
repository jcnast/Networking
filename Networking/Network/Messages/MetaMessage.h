#include "Message.h"
#include "WrappedMessage.h"

namespace Message
{
	class MetaMessage : public IMessageType<std::vector<WrappedMessage>>
	{

	};
}