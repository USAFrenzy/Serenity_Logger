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

	const FlushSetting Flush_Policy::PrimarySetting() {
		return mainOpt;
	}

	const PeriodicOptions Flush_Policy::SubSetting() {
		return subOpt;
	}

	const PeriodicSettings Flush_Policy::SecondarySettings() {
		return subSettings;
	}

	const Flush_Policy Flush_Policy::Policy() {
		return *this;
	}
}    // namespace serenity::experimental
