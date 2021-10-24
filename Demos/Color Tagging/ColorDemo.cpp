#include <iostream>
#include <serenity/Color/Color.h>

int main( )
{
	using namespace se_colors;

	std::cout << Tag::Blue( "\n\nSome " ) << Tag::Cyan( "Examples " ) << Tag::Green( "Of " )
		  << Tag::Magenta( "Specific " ) << Tag::Bright_Red_On_Bright_White( "Word" ) << " "
		  << Tag::Red( "Color " ) << Tag::Yellow( "Tagging" ) << Tag::Bright_Green( "!\n\n" );

	std::cout << Tag::Black_On_Black( "Black On Black" ) << Tag::Black_On_Red( "Black On Red" )
		  << Tag::Black_On_Green( "Black On Green" ) << Tag::Black_On_Yellow( "Black On Yellow" )
		  << Tag::Black_On_Blue( "Black On Blue" ) << Tag::Black_On_Magenta( "Black On Magenta" )
		  << Tag::Black_On_Cyan( "Black On Cyan" ) << Tag::Black_On_White( "Black On White\n" );

	std::cout << Tag::Red_On_Black( "Red On Black" ) << Tag::Red_On_Red( "Red On Red" )
		  << Tag::Red_On_Green( "Red On Green" ) << Tag::Red_On_Yellow( "Red On Yellow" )
		  << Tag::Red_On_Blue( "Red On Blue" ) << Tag::Red_On_Magenta( "Red On Magenta" )
		  << Tag::Red_On_Cyan( "Red On Cyan" ) << Tag::Red_On_White( "Red On White\n" );

	std::cout << Tag::Green_On_Black( "Green On Black" ) << Tag::Green_On_Red( "Green On Red" )
		  << Tag::Green_On_Green( "Green On Green" ) << Tag::Green_On_Yellow( "Green On Yellow" )
		  << Tag::Green_On_Blue( "Green On Blue" ) << Tag::Green_On_Magenta( "Green On Magenta" )
		  << Tag::Green_On_Cyan( "Green On Cyan" ) << Tag::Green_On_White( "Green On White\n" );

	std::cout << Tag::Yellow_On_Black( "Yellow On Black" ) << Tag::Yellow_On_Red( "Yellow On Red" )
		  << Tag::Yellow_On_Green( "Yellow On Green" ) << Tag::Yellow_On_Yellow( "Yellow On Yellow" )
		  << Tag::Yellow_On_Blue( "Yellow On Blue" ) << Tag::Yellow_On_Magenta( "Yellow On Magenta" )
		  << Tag::Yellow_On_Cyan( "Yellow On Cyan" ) << Tag::Yellow_On_White( "Yellow On White\n" );

	std::cout << Tag::Blue( "Blue" ) << Tag::Blue_On_Black( "Blue On Black" )
		  << Tag::Blue_On_Red( "Blue On Red" ) << Tag::Blue_On_Green( "Blue On Green" )
		  << Tag::Blue_On_Yellow( "Blue On Yellow" ) << Tag::Blue_On_Blue( "Blue On Blue" )
		  << Tag::Blue_On_Magenta( "Blue On Magenta" ) << Tag::Blue_On_Cyan( "Blue On Cyan" )
		  << Tag::Blue_On_White( "Blue On White\n" );

	std::cout << Tag::Magenta_On_Black( "Magenta On Black" ) << Tag::Magenta_On_Red( "Magenta On Red" )
		  << Tag::Magenta_On_Green( "Magenta On Green" ) << Tag::Magenta_On_Yellow( "Magenta On Yellow" )
		  << Tag::Magenta_On_Blue( "Magenta On Blue" ) << Tag::Magenta_On_Magenta( "Magenta On Magenta" )
		  << Tag::Magenta_On_Cyan( "Magenta On Cyan" ) << Tag::Magenta_On_White( "Magenta On White\n" );

	std::cout << Tag::Cyan_On_Black( "Cyan On Black" ) << Tag::Cyan_On_Red( "Cyan On Red" )
		  << Tag::Cyan_On_Green( "Cyan On Green" ) << Tag::Cyan_On_Yellow( "Cyan On Yellow" )
		  << Tag::Cyan_On_Blue( "Cyan On Blue" ) << Tag::Cyan_On_Magenta( "Cyan On Magenta" )
		  << Tag::Cyan_On_Cyan( "Cyan On Cyan" ) << Tag::Cyan_On_White( "Cyan On White\n" );

	std::cout << Tag::White_On_Black( "White On Black" ) << Tag::White_On_Red( "White On Red" )
		  << Tag::White_On_Green( "White On Green" ) << Tag::White_On_Yellow( "White On Yellow" )
		  << Tag::White_On_Blue( "White On Blue" ) << Tag::White_On_Magenta( "White On Magenta" )
		  << Tag::White_On_Cyan( "White On Cyan" ) << Tag::White_On_White( "White On White\n" );


	std::cout << Tag::Black_On_Grey( "Black On Grey" ) << Tag::Black_On_Bright_Red( "Black On Bright Red" )
		  << Tag::Black_On_Bright_Green( "Black On Bright Green" )
		  << Tag::Black_On_Bright_Yellow( "Black On Bright Yellow" )
		  << Tag::Black_On_Bright_Blue( "Black On Bright Blue" )
		  << Tag::Black_On_Bright_Magenta( "Black On Bright Magenta" )
		  << Tag::Black_On_Bright_Cyan( "Black On Bright Cyan" )
		  << Tag::Black_On_Bright_White( "Black On Bright White\n" );

	std::cout << Tag::Red_On_Grey( "Red On Grey" ) << Tag::Red_On_Bright_Red( "Red On Bright Red" )
		  << Tag::Red_On_Bright_Green( "Red On Bright Green" )
		  << Tag::Red_On_Bright_Yellow( "Red On Bright Yellow" )
		  << Tag::Red_On_Bright_Blue( "Red On Bright Blue" )
		  << Tag::Red_On_Bright_Magenta( "Red On Bright Magenta" )
		  << Tag::Red_On_Bright_Cyan( "Red On Bright Cyan" )
		  << Tag::Red_On_Bright_White( "Red On Bright White\n" );

	std::cout << Tag::Green_On_Grey( "Green On Grey" ) << Tag::Green_On_Bright_Red( "Green On Bright Red" )
		  << Tag::Green_On_Bright_Green( "Green On Bright Green" )
		  << Tag::Green_On_Bright_Yellow( "Green On Bright Yellow" )
		  << Tag::Green_On_Bright_Blue( "Green On Bright Blue" )
		  << Tag::Green_On_Bright_Magenta( "Green On Bright Magenta" )
		  << Tag::Green_On_Bright_Cyan( "Green On Bright Cyan" )
		  << Tag::Green_On_Bright_White( "Green On Bright White\n" );

	std::cout << Tag::Yellow_On_Grey( "Yellow On Grey" )
		  << Tag::Yellow_On_Bright_Red( "Yellow On Bright Red" )
		  << Tag::Yellow_On_Bright_Green( "Yellow On Bright Green" )
		  << Tag::Yellow_On_Bright_Yellow( "Yellow On Bright Yellow" )
		  << Tag::Yellow_On_Bright_Blue( "Yellow On Bright Blue" )
		  << Tag::Yellow_On_Bright_Magenta( "Yellow On Bright Magenta" )
		  << Tag::Yellow_On_Bright_Cyan( "Yellow On Bright Cyan" )
		  << Tag::Yellow_On_Bright_White( "Yellow On Bright White\n" );

	std::cout << Tag::Blue_On_Grey( "Blue On Grey" ) << Tag::Blue_On_Bright_Red( "Blue On Bright Red" )
		  << Tag::Blue_On_Bright_Green( "Blue On Bright Green" )
		  << Tag::Blue_On_Bright_Yellow( "Blue On Bright Yellow" )
		  << Tag::Blue_On_Bright_Blue( "Blue On Bright Blue" )
		  << Tag::Blue_On_Bright_Magenta( "Blue On Bright Magenta" )
		  << Tag::Blue_On_Bright_Cyan( "Blue On Bright Cyan" )
		  << Tag::Blue_On_Bright_White( "Blue On Bright White\n" );

	std::cout << Tag::Magenta_On_Grey( "Magenta On Grey" )
		  << Tag::Magenta_On_Bright_Red( "Magenta On Bright Red" )
		  << Tag::Magenta_On_Bright_Green( "Magenta On Bright Green" )
		  << Tag::Magenta_On_Bright_Yellow( "Magenta On Bright Yellow" )
		  << Tag::Magenta_On_Bright_Blue( "Magenta On Bright Blue" )
		  << Tag::Magenta_On_Bright_Magenta( "Magenta On Bright Magenta" )
		  << Tag::Magenta_On_Bright_Cyan( "Magenta On Bright Cyan" )
		  << Tag::Magenta_On_Bright_White( "Magenta On Bright White\n" );

	std::cout << Tag::Cyan_On_Grey( "Cyan On Grey" ) << Tag::Cyan_On_Bright_Red( "Cyan On Bright Red" )
		  << Tag::Cyan_On_Bright_Green( "Cyan On Bright Green" )
		  << Tag::Cyan_On_Bright_Yellow( "Cyan On Bright Yellow" )
		  << Tag::Cyan_On_Bright_Blue( "Cyan On Bright Blue" )
		  << Tag::Cyan_On_Bright_Magenta( "Cyan On Bright Magenta" )
		  << Tag::Cyan_On_Bright_Cyan( "Cyan On Bright Cyan" )
		  << Tag::Cyan_On_Bright_White( "Cyan On Bright White\n" );

	std::cout << Tag::White_On_Grey( "White On Grey" ) << Tag::White_On_Bright_Red( "White On Bright Red" )
		  << Tag::White_On_Bright_Green( "White On Bright Green" )
		  << Tag::White_On_Bright_Yellow( "White On Bright Yellow" )
		  << Tag::White_On_Bright_Blue( "White On Bright Blue" )
		  << Tag::White_On_Bright_Magenta( "White On Bright Magenta" )
		  << Tag::White_On_Bright_Cyan( "White On Bright Cyan" )
		  << Tag::White_On_Bright_White( "White On Bright White\n" );


	std::cout << Tag::Grey_On_Black( "Black On Black" ) << Tag::Grey_On_Red( "Grey On Red" )
		  << Tag::Grey_On_Green( "Black On Green" ) << Tag::Grey_On_Yellow( "Grey On Yellow" )
		  << Tag::Grey_On_Blue( "Black On Blue" ) << Tag::Grey_On_Magenta( "Grey On Magenta" )
		  << Tag::Grey_On_Cyan( "Black On Cyan" ) << Tag::Grey_On_White( "Grey On White\n" );

	std::cout << Tag::Bright_Red_On_Black( "Bright Red On Black" )
		  << Tag::Bright_Red_On_Red( "Bright Red On Red" )
		  << Tag::Bright_Red_On_Green( "Bright Red On Green" )
		  << Tag::Bright_Red_On_Yellow( "Bright Red On Yellow" )
		  << Tag::Bright_Red_On_Blue( "Bright Red On Blue" )
		  << Tag::Bright_Red_On_Magenta( "Bright Red On Magenta" )
		  << Tag::Bright_Red_On_Cyan( "Bright Red On Cyan" )
		  << Tag::Bright_Red_On_White( "Bright Red On White\n" );

	std::cout << Tag::Bright_Green_On_Black( "Bright Green On Black" )
		  << Tag::Bright_Green_On_Red( "Bright Green On Red" )
		  << Tag::Bright_Green_On_Green( "Bright Green On Green" )
		  << Tag::Bright_Green_On_Yellow( "Bright Green On Yellow" )
		  << Tag::Bright_Green_On_Blue( "Bright Green On Blue" )
		  << Tag::Bright_Green_On_Magenta( "Bright Green On Magenta" )
		  << Tag::Bright_Green_On_Cyan( "Bright Green On Cyan" )
		  << Tag::Bright_Green_On_White( "Bright Green On White\n" );

	std::cout << Tag::Bright_Yellow_On_Black( "Bright Yellow On Black" )
		  << Tag::Bright_Yellow_On_Red( "Bright Yellow On Red" )
		  << Tag::Bright_Yellow_On_Green( "Bright Yellow On Green" )
		  << Tag::Bright_Yellow_On_Yellow( "Bright Yellow On Yellow" )
		  << Tag::Bright_Yellow_On_Blue( "Bright Yellow On Blue" )
		  << Tag::Bright_Yellow_On_Magenta( "Bright Yellow On Magenta" )
		  << Tag::Bright_Yellow_On_Cyan( "Bright Yellow On Cyan" )
		  << Tag::Bright_Yellow_On_White( "Bright Yellow On White\n" );

	std::cout << Tag::Bright_Blue( "Bright Blue" ) << Tag::Bright_Blue_On_Black( "Bright Blue On Black" )
		  << Tag::Bright_Blue_On_Red( "Bright Blue On Red" )
		  << Tag::Bright_Blue_On_Green( "Bright Blue On Green" )
		  << Tag::Bright_Blue_On_Yellow( "Bright Blue On Yellow" )
		  << Tag::Bright_Blue_On_Blue( "Bright Blue On Blue" )
		  << Tag::Bright_Blue_On_Magenta( "Bright Blue On Magenta" )
		  << Tag::Bright_Blue_On_Cyan( "Bright Blue On Cyan" )
		  << Tag::Bright_Blue_On_White( "Bright Blue On White\n" );

	std::cout << Tag::Bright_Magenta_On_Black( "Bright Magenta On Black" )
		  << Tag::Bright_Magenta_On_Red( "Bright Magenta On Red" )
		  << Tag::Bright_Magenta_On_Green( "Bright Magenta On Green" )
		  << Tag::Bright_Magenta_On_Yellow( "Bright Magenta On Yellow" )
		  << Tag::Bright_Magenta_On_Blue( "Bright Magenta On Blue" )
		  << Tag::Bright_Magenta_On_Magenta( "Bright Magenta On Magenta" )
		  << Tag::Bright_Magenta_On_Cyan( "Bright Magenta On Cyan" )
		  << Tag::Bright_Magenta_On_White( "Bright Magenta On White\n" );

	std::cout << Tag::Bright_Cyan_On_Black( "Bright Cyan On Black" )
		  << Tag::Bright_Cyan_On_Red( "Bright Cyan On Red" )
		  << Tag::Bright_Cyan_On_Green( "Bright Cyan On Green" )
		  << Tag::Bright_Cyan_On_Yellow( "Bright Cyan On Yellow" )
		  << Tag::Bright_Cyan_On_Blue( "Bright Cyan On Blue" )
		  << Tag::Bright_Cyan_On_Magenta( "Bright Cyan On Magenta" )
		  << Tag::Bright_Cyan_On_Cyan( "Bright Cyan On Cyan" )
		  << Tag::Bright_Cyan_On_White( "Bright Cyan On White\n" );

	std::cout << Tag::Bright_White_On_Black( "Bright_White On Black" )
		  << Tag::Bright_White_On_Red( "Bright White On Red" )
		  << Tag::Bright_White_On_Green( "Bright White On Green" )
		  << Tag::Bright_White_On_Yellow( "Bright_White On Yellow" )
		  << Tag::Bright_White_On_Blue( "Bright White On Blue" )
		  << Tag::Bright_White_On_Magenta( "Bright White On Magenta" )
		  << Tag::Bright_White_On_Cyan( "Bright White On Cyan" )
		  << Tag::Bright_White_On_White( "Bright White On White\n" );


	std::cout << Tag::Black_On_Grey( "Black On Grey" ) << Tag::Black_On_Bright_Red( "Black On Bright Red" )
		  << Tag::Black_On_Bright_Green( "Black On Bright Green" )
		  << Tag::Black_On_Bright_Yellow( "Black On Bright Yellow" )
		  << Tag::Black_On_Bright_Blue( "Black On Bright Blue" )
		  << Tag::Black_On_Bright_Magenta( "Black On Bright Magenta" )
		  << Tag::Black_On_Bright_Cyan( "Black On Bright Cyan" )
		  << Tag::Black_On_Bright_White( "Black On Bright White\n" );

	std::cout << Tag::Bright_Red_On_Grey( "Bright Red On Grey" )
		  << Tag::Bright_Red_On_Bright_Red( "Bright Red On Bright Red" )
		  << Tag::Bright_Red_On_Bright_Green( "Bright Red On Bright Green" )
		  << Tag::Bright_Red_On_Bright_Yellow( "Bright Red On Bright Yellow" )
		  << Tag::Bright_Red_On_Bright_Blue( "Bright Red On Bright Blue" )
		  << Tag::Bright_Red_On_Bright_Magenta( "Bright Red On Bright Magenta" )
		  << Tag::Bright_Red_On_Bright_Cyan( "Bright Red On Bright Cyan" )
		  << Tag::Bright_Red_On_Bright_White( "Bright Red On Bright White\n" );

	std::cout << Tag::Bright_Green_On_Grey( "Bright Green On Grey" )
		  << Tag::Bright_Green_On_Bright_Red( "Bright Green On Bright Red" )
		  << Tag::Bright_Green_On_Bright_Green( "Bright Green On Bright Green" )
		  << Tag::Bright_Green_On_Bright_Yellow( "Bright Green On Bright Yellow" )
		  << Tag::Bright_Green_On_Bright_Blue( "Bright Green On Bright Blue" )
		  << Tag::Bright_Green_On_Bright_Magenta( "Bright Green On Bright Magenta" )
		  << Tag::Bright_Green_On_Bright_Cyan( "Bright Green On Bright Cyan" )
		  << Tag::Bright_Green_On_Bright_White( "Bright Green On Bright White\n" );

	std::cout << Tag::Bright_Yellow_On_Grey( "Bright Yellow On Grey" )
		  << Tag::Bright_Yellow_On_Bright_Red( "Bright Yellow On Bright Red" )
		  << Tag::Bright_Yellow_On_Bright_Green( "Bright Yellow On Bright Green" )
		  << Tag::Bright_Yellow_On_Bright_Yellow( "Bright Yellow On Bright Yellow" )
		  << Tag::Bright_Yellow_On_Bright_Blue( "Bright Yellow On Bright Blue" )
		  << Tag::Bright_Yellow_On_Bright_Magenta( "Bright Yellow On Bright Magenta" )
		  << Tag::Bright_Yellow_On_Bright_Cyan( "Bright Yellow On Bright Cyan" )
		  << Tag::Bright_Yellow_On_Bright_White( "Bright Yellow On Bright White\n" );

	std::cout << Tag::Bright_Blue_On_Grey( "Bright Blue On Grey" )
		  << Tag::Bright_Blue_On_Bright_Red( "Bright Blue On Bright Red" )
		  << Tag::Bright_Blue_On_Bright_Green( "Bright Blue On Bright Green" )
		  << Tag::Bright_Blue_On_Bright_Yellow( "Bright Blue On Bright Yellow" )
		  << Tag::Bright_Blue_On_Bright_Blue( "Bright Blue On Bright Blue" )
		  << Tag::Bright_Blue_On_Bright_Magenta( "Bright Blue On Bright Magenta" )
		  << Tag::Bright_Blue_On_Bright_Cyan( "Bright Blue On Bright Cyan" )
		  << Tag::Bright_Blue_On_Bright_White( "Bright Blue On Bright White\n" );

	std::cout << Tag::Bright_Magenta_On_Grey( "Bright Magenta On Grey" )
		  << Tag::Bright_Magenta_On_Bright_Red( "Bright Magenta On Bright Red" )
		  << Tag::Bright_Magenta_On_Bright_Green( "Bright Magenta On Bright Green" )
		  << Tag::Bright_Magenta_On_Bright_Yellow( "Bright Magenta On Bright Yellow" )
		  << Tag::Bright_Magenta_On_Bright_Blue( "Bright Magenta On Bright Blue" )
		  << Tag::Bright_Magenta_On_Bright_Magenta( "Bright Magenta On Bright Magenta" )
		  << Tag::Bright_Magenta_On_Bright_Cyan( "Bright Magenta On Bright Cyan" )
		  << Tag::Bright_Magenta_On_Bright_White( "Bright Magenta On Bright White\n" );

	std::cout << Tag::Bright_Cyan_On_Grey( "Bright Cyan On Grey" )
		  << Tag::Bright_Cyan_On_Bright_Red( "Bright Cyan On Bright Red" )
		  << Tag::Bright_Cyan_On_Bright_Green( "Bright Cyan On Bright Green" )
		  << Tag::Bright_Cyan_On_Bright_Yellow( "Bright Cyan On Bright Yellow" )
		  << Tag::Bright_Cyan_On_Bright_Blue( "Bright Cyan On Bright Blue" )
		  << Tag::Bright_Cyan_On_Bright_Magenta( "Bright Cyan On Bright Magenta" )
		  << Tag::Bright_Cyan_On_Bright_Cyan( "Bright Cyan On Bright Cyan" )
		  << Tag::Bright_Cyan_On_Bright_White( "Bright Cyan On Bright White\n" );

	std::cout << Tag::Bright_White_On_Grey( "Bright White On Grey" )
		  << Tag::Bright_White_On_Bright_Red( "Bright White On Bright Red" )
		  << Tag::Bright_White_On_Bright_Green( "Bright White On Bright Green" )
		  << Tag::Bright_White_On_Bright_Yellow( "Bright White On Bright Yellow" )
		  << Tag::Bright_White_On_Bright_Blue( "Bright White On Bright Blue" )
		  << Tag::Bright_White_On_Bright_Magenta( "Bright White On Bright Magenta" )
		  << Tag::Bright_White_On_Bright_Cyan( "Bright White On Bright Cyan" )
		  << Tag::Bright_White_On_Bright_White( "Bright White On Bright White\n" );
}