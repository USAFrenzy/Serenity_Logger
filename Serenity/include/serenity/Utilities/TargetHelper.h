#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/FlushPolicy.h>

namespace serenity::targets::helpers {

	class BaseTargetHelper
	{
	  public:
		explicit BaseTargetHelper();
		BaseTargetHelper(BaseTargetHelper&) = delete;
		BaseTargetHelper& operator=(BaseTargetHelper&);
		~BaseTargetHelper() = default;

		void EnableMultiThreadingSupport(bool enableMultiThreading = true);
		bool isMTSupportEnabled();
		void SetFlushPolicy(const serenity::experimental::Flush_Policy& fPolicy);
		const std::unique_ptr<serenity::experimental::Flush_Policy>& Policy() const;

	  private:
		bool multiThreadSupport;

	  protected:
		std::unique_ptr<serenity::experimental::Flush_Policy> policy;
	};
}    // namespace serenity::targets::helpers
