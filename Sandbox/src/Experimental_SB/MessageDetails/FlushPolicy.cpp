#include "FlushPolicy.h"

namespace serenity
{
	namespace expiremental
	{
		Flush_Policy::Flush_Policy( Flush primaryOpt )
		  : mainOpt( primaryOpt ), subOpt( PeriodicOptions::undef ), subSettings( { } ) { }

		Flush_Policy::Flush_Policy( const Flush_Policy &p )
		{
			*this = p;
		}

		Flush_Policy::Flush_Policy( Flush primaryOpt, PeriodicOptions secondaryOpt, PeriodicSettings settings )
		  : mainOpt( primaryOpt ), subOpt( secondaryOpt ), subSettings( std::move( settings ) )
		{
		}

		Flush_Policy::Flush_Policy( PeriodicOptions secondaryOpt, PeriodicSettings settings )
		  : mainOpt( Flush::periodically ), subOpt( secondaryOpt ), subSettings( std::move( settings ) )
		{
		}
		Flush_Policy::Flush_Policy( Flush primaryOpt, PeriodicOptions secondaryOpt )
		  : mainOpt( primaryOpt ), subOpt( secondaryOpt ) { }

		Flush_Policy &Flush_Policy::operator=( const Flush_Policy &p )
		{
			mainOpt     = p.mainOpt;
			subOpt      = p.subOpt;
			subSettings = p.subSettings;
			return *this;
		}

		void Flush_Policy::SetPrimaryMode( Flush primary )
		{
			mainOpt = primary;
		}

		void Flush_Policy::SetSecondaryMode( PeriodicOptions secondary )
		{
			subOpt = secondary;
		}

		void Flush_Policy::SetSecondarySettings( PeriodicSettings settings )
		{
			subSettings = std::move( settings );
		}

		const Flush Flush_Policy::PrimarySetting( )
		{
			return mainOpt;
		}

		const PeriodicOptions Flush_Policy::SubSetting( )
		{
			return subOpt;
		}

		const PeriodicSettings Flush_Policy::SecondarySettings( )
		{
			return subSettings;
		}

		const Flush_Policy Flush_Policy::Policy( )
		{
			return *this;
		}
	}  // namespace expiremental
}  // namespace serenity