#include <serenity/Utilities/TargetHelper.h>

namespace serenity::targets::helpers {
	BaseTargetHelper::BaseTargetHelper(): multiThreadSupport(false), policy(std::make_unique<Flush_Policy>(FlushSetting::never)) { }

	BaseTargetHelper& serenity::targets::helpers::BaseTargetHelper::operator=(BaseTargetHelper& other) {
		multiThreadSupport = other.multiThreadSupport;
		policy             = std::make_unique<Flush_Policy>(*other.policy);
		return *this;
	}

	void BaseTargetHelper::EnableMultiThreadingSupport(bool enableMultiThreading) {
		multiThreadSupport = enableMultiThreading;
	}

	bool BaseTargetHelper::isMTSupportEnabled() {
		return multiThreadSupport;
	}

	void BaseTargetHelper::SetFlushPolicy(const Flush_Policy& fPolicy) {
		policy = std::make_unique<Flush_Policy>(fPolicy);
	}

	const std::unique_ptr<Flush_Policy>& BaseTargetHelper::Policy() const {
		return policy;
	}

}    // namespace serenity::targets::helpers
