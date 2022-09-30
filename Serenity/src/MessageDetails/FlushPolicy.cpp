#include <serenity/MessageDetails/FlushPolicy.h>

namespace serenity::experimental {
	Flush_Policy::Flush_Policy(FlushSetting primaryOpt): mainOpt(primaryOpt), subOpt(PeriodicOptions::undef), subSettings({}) { }

	Flush_Policy::Flush_Policy(const Flush_Policy& p) {
		*this = p;
	}

	Flush_Policy::Flush_Policy(FlushSetting primaryOpt, PeriodicOptions secondaryOpt, PeriodicSettings settings)
		: mainOpt(primaryOpt), subOpt(secondaryOpt), subSettings(settings) { }

	Flush_Policy::Flush_Policy(PeriodicOptions secondaryOpt, PeriodicSettings settings)
		: mainOpt(FlushSetting::periodically), subOpt(secondaryOpt), subSettings(settings) { }

	Flush_Policy::Flush_Policy(FlushSetting primaryOpt, PeriodicOptions secondaryOpt): mainOpt(primaryOpt), subOpt(secondaryOpt) { }

	Flush_Policy& Flush_Policy::operator=(const Flush_Policy& p) {
		mainOpt                = p.mainOpt;
		subOpt                 = p.subOpt;
		subSettings.flushEvery = std::chrono::duration_cast<std::chrono::milliseconds>(p.subSettings.flushEvery);
		subSettings.flushOn    = p.subSettings.flushOn;
		return *this;
	}

	void Flush_Policy::SetPrimaryMode(FlushSetting primary) {
		mainOpt = primary;
	}

	void Flush_Policy::SetSecondaryMode(PeriodicOptions secondary) {
		subOpt = secondary;
	}

	void Flush_Policy::SetSecondarySettings(PeriodicSettings settings) {
		subSettings = settings;
	}

	FlushSetting Flush_Policy::PrimarySetting() const {
		return mainOpt;
	}

	PeriodicOptions Flush_Policy::SubSetting() const {
		return subOpt;
	}

	PeriodicSettings Flush_Policy::SecondarySettings() const {
		return subSettings;
	}

	Flush_Policy Flush_Policy::Policy() const {
		return *this;
	}
}    // namespace serenity::experimental
