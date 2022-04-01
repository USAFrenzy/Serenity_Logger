#pragma once

#include <serenity/Common.h>

#include <chrono>
#include <mutex>

namespace serenity::experimental {
	enum class FlushSetting
	{
		always,
		periodically,
		never,
	};
	enum class PeriodicOptions
	{
		timeBased,
		logLevelBased,
		undef,
	};

	struct PeriodicSettings
	{
		std::chrono::milliseconds flushEvery { std::chrono::milliseconds(5000) };
		LoggerLevel flushOn { LoggerLevel::trace };
	};

	class Flush_Policy
	{
	      public:
		Flush_Policy() = delete;
		explicit Flush_Policy(FlushSetting primaryOpt);
		explicit Flush_Policy(FlushSetting primaryOpt, PeriodicOptions secondaryOpt, PeriodicSettings settings);
		explicit Flush_Policy(FlushSetting primaryOpt, PeriodicOptions secondaryOpt);
		explicit Flush_Policy(PeriodicOptions secondaryOpt, PeriodicSettings settings);
		Flush_Policy(const Flush_Policy& p);
		Flush_Policy& operator=(const Flush_Policy& p);
		~Flush_Policy() = default;

		void SetPrimaryMode(FlushSetting primary);
		void SetSecondaryMode(PeriodicOptions secondary);
		void SetSecondarySettings(PeriodicSettings subSettings);
		FlushSetting PrimarySetting() const;
		PeriodicOptions SubSetting() const;
		Flush_Policy Policy() const;
		PeriodicSettings SecondarySettings() const;

	      private:
		FlushSetting mainOpt;
		PeriodicOptions subOpt;
		PeriodicSettings subSettings;
	};
}    // namespace serenity::experimental
