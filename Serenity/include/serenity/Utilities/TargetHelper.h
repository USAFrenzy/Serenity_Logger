#pragma once

#include <serenity/Common.h>
#include <serenity/MessageDetails/FlushPolicy.h>
#include <serenity/MessageDetails/Message_Info.h>

namespace serenity::targets {
	struct SeFmtArgRefs
	{
		explicit SeFmtArgRefs(const msg_details::Message_Info& i)
			: m_lvl(i.MsgLevel()), m_name(i.Name()), m_msg(i.Message()), m_src(i.SourceLocation()), m_thread(i.ThisThreadID()), m_time(i.TimeInfo()) { }
		SeFmtArgRefs()                                = delete;
		~SeFmtArgRefs()                               = default;
		SeFmtArgRefs(const SeFmtArgRefs&)             = delete;
		SeFmtArgRefs& operator=(const SeFmtArgRefs&)  = delete;
		SeFmtArgRefs(const SeFmtArgRefs&&)            = delete;
		SeFmtArgRefs& operator=(const SeFmtArgRefs&&) = delete;

		const LoggerLevel& m_lvl;
		const std::string& m_name;
		const std::string& m_msg;
		const std::source_location& m_src;
		const std::thread::id& m_thread;
		const std::tm& m_time;
	};
}    // namespace serenity::targets

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
		void SetFlushPolicy(const Flush_Policy& fPolicy);
		const std::unique_ptr<Flush_Policy>& Policy() const;

	  private:
		bool multiThreadSupport;

	  protected:
		std::unique_ptr<Flush_Policy> policy;
	};
}    // namespace serenity::targets::helpers
