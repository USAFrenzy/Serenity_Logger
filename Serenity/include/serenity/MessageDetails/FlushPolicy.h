#pragma once

#include <serenity/Common.h>

#include <chrono>
#include <mutex>

namespace serenity::expiremental
{
	enum class Flush
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
		std::chrono::milliseconds flushEvery { std::chrono::milliseconds( 500 ) };
		LoggerLevel               flushOn { LoggerLevel::trace };
	};

	class Flush_Policy
	{
	  public:
		Flush_Policy( ) = delete;
		explicit Flush_Policy( Flush primaryOpt );
		explicit Flush_Policy( Flush primaryOpt, PeriodicOptions secondaryOpt, PeriodicSettings settings );
		explicit Flush_Policy( Flush primaryOpt, PeriodicOptions secondaryOpt );
		explicit Flush_Policy( PeriodicOptions secondaryOpt, PeriodicSettings settings );
		Flush_Policy( const Flush_Policy &p );
		Flush_Policy &operator=( const Flush_Policy &p );
		~Flush_Policy( )      = default;

		void                   SetPrimaryMode( Flush primary );
		void                   SetSecondaryMode( PeriodicOptions secondary );
		void                   SetSecondarySettings( PeriodicSettings subSettings );
		const Flush            PrimarySetting( );
		const PeriodicOptions  SubSetting( );
		const Flush_Policy     Policy( );
		const PeriodicSettings SecondarySettings( );

	  private:
		Flush            mainOpt;
		PeriodicOptions  subOpt;
		PeriodicSettings subSettings;
		std::mutex       policyMutex;
	};
}  // namespace serenity::expiremental
