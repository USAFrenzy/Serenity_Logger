#include <serenity/Utilities/TargetHelper.h>

namespace serenity::targets::helpers {
	BaseTargetHelper::BaseTargetHelper()
		: toBuffer(false), internalBuffer(std::string {}), multiThreadSupport(false),
		  policy(serenity::experimental::FlushSetting::never) { }

	BaseTargetHelper& serenity::targets::helpers::BaseTargetHelper::operator=(BaseTargetHelper& other) {
		internalBuffer     = other.internalBuffer;
		multiThreadSupport = other.multiThreadSupport;
		policy             = other.policy;
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

	const bool BaseTargetHelper::isWriteToBuf() {
		return toBuffer;
	}

	std::string* BaseTargetHelper::Buffer() {
		return &internalBuffer;
	}

	serenity::experimental::Flush_Policy& BaseTargetHelper::Policy() {
		return policy;
	}

}    // namespace serenity::targets::helpers
