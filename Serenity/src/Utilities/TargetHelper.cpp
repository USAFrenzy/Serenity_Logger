#include <serenity/Utilities/TargetHelper.h>

namespace serenity::targets::helpers {
	BaseTargetHelper::BaseTargetHelper()
		: toBuffer(false), internalBuffer(std::string {}), multiThreadSupport(false),
		  policy(std::make_unique<serenity::experimental::Flush_Policy>(serenity::experimental::FlushSetting::never)) { }

	BaseTargetHelper& serenity::targets::helpers::BaseTargetHelper::operator=(BaseTargetHelper& other) {
		internalBuffer     = other.internalBuffer;
		multiThreadSupport = other.multiThreadSupport;
		policy             = std::move(other.policy);
		toBuffer           = other.toBuffer;
		return *this;
	}

	void BaseTargetHelper::EnableMultiThreadingSupport(bool enableMultiThreading) {
		multiThreadSupport = enableMultiThreading;
	}

	bool BaseTargetHelper::isMTSupportEnabled() {
		return multiThreadSupport;
	}

	void BaseTargetHelper::WriteToBaseBuffer(bool fmtToBuf) {
		toBuffer = fmtToBuf;
	}

	bool BaseTargetHelper::isWriteToBuf() const {
		return toBuffer;
	}

	void BaseTargetHelper::SetFlushPolicy(const serenity::experimental::Flush_Policy& fPolicy) {
		policy = std::make_unique<serenity::experimental::Flush_Policy>(fPolicy);
	}

	std::string* BaseTargetHelper::Buffer() {
		return &internalBuffer;
	}

	const std::unique_ptr<serenity::experimental::Flush_Policy>& BaseTargetHelper::Policy() const {
		return policy;
	}

}    // namespace serenity::targets::helpers
