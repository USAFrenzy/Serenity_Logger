#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/FlushPolicy.h>

namespace serenity::targets::helpers {

	class BaseTargetHelper
	{
	      public:
		explicit BaseTargetHelper();
		BaseTargetHelper(BaseTargetHelper&) = delete;
		BaseTargetHelper& operator=(BaseTargetHelper&) = delete;
		~BaseTargetHelper()                            = default;

		void EnableMultiThreadingSupport(bool enableMultiThreading = true);
		bool isMTSupportEnabled();
		void WriteToBaseBuffer(bool fmtToBuf = true);
		const bool isWriteToBuf();
		std::string* Buffer();
		serenity::experimental::Flush_Policy& Policy();

	      private:
		bool toBuffer;
		std::string internalBuffer;
		bool multiThreadSupport;

	      protected:
		serenity::experimental::Flush_Policy policy;
	};
}    // namespace serenity::targets::helpers
