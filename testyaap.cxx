//! \file testyaap.cxx
//! \brief Illustration of the Argument Parser yaap namespace
//!
//! try with for instance:
//!   [shell]$ testyaap -i inputfile.txt -vV -o outputfile.raw -s .558 .558 0.89

// The only thing to do to define the yaap namespace
#include "yaap.h"


int main( int argc, char** argv )
{

////////////////// OPTION LAYOUT DEFINITION /////////////////////
    // Initialize a parser following the command line argument
    yaap::Parser parser( argc, argv,"Test the Argument Parser \'yaap\'. It simply displays the following option as\nentered in the command line." );

    // Add an option of type "-i filename"
    yaap::OptionArg<std::string>* inputOpt = parser.AddOptionArg<std::string>( 'i', "Input file (.vti)",1,true);
    // Add an option of type "-e 0 127 0 127 0 127"
    yaap::OptionArg<int>* extentOpt = parser.AddOptionArg<int>( 'e', "Extent (dimension): xmin xmax ymin ymax zmin zmax (integer)",6);
    // Add an option of type "-s 0.588 0.588 0.877"
    yaap::OptionArg<double>* spacingOpt = parser.AddOptionArg<double>( 's', "Spacing (size of pixel): x y z (double)",3,true);
    // Add an option of type "-o filename"
    yaap::OptionArg<std::string>* outputOpt = parser.AddOptionArg<std::string>( 'o', "Output file (.vti)",1,true);
    // Add an option "-t tag" where tag is a uint, possibly hexa.
    yaap::OptionArg<unsigned int>* tagOpt = parser.AddOptionArg<unsigned int>( 't', "UINT Tag. Can be hexa (prefix with 0x)",1,true);
    // Add a simple option for ie. debugging information
    yaap::Option* verboseOpt = parser.AddOption('v', "Verbose output");
    // Add a simple option for ie. version display
    yaap::Option* versionOpt = parser.AddOption('V', "Display version");
    // Add a simple option for usage display
    yaap::Option* helpOpt = parser.AddOption('h', "Display a brief help");


//////////////// NOW THE OPTIONS ARE DEFINED. JUST USE THEM //////

    if( !parser.IsCommandLineValid( ) || helpOpt->Exists() || argc ==  1)
    {
        parser.Usage( );
        return( 0 );
    }

    // Check verbose option
    std::cout << "Verbose? ";
    (verboseOpt->Exists())? std::cout << "Yes": std::cout << "No";
    std::cout << std::endl;

    // Check version option
    std::cout << "Version? ";
    (versionOpt->Exists())? std::cout << "Yes": std::cout << "No";
    std::cout << std::endl;

    // Check input filename option
    std::cout << "Input filename: ";
    (inputOpt->Exists())? std::cout << inputOpt->GetValue(): std::cout << "n/a";
    std::cout << std::endl;

    // Check output filename option
    std::cout << "Output filename: ";
    (outputOpt->Exists())? std::cout << outputOpt->GetValue(): std::cout << "n/a";
    std::cout << std::endl;

    // Check tag option
    std::cout << "Tag: ";
    (tagOpt->Exists())? std::cout << tagOpt->GetValue(): std::cout << "n/a";
    std::cout << std::endl;

    // Check extent option
    std::cout << "Extent: ";
    if(extentOpt->Exists())
    {
        for( int i = 0; i < 5; i++ )
            std::cout << extentOpt->GetArgument(i) << " ; ";
        std::cout << extentOpt->GetArgument(5) << std::endl;
    }
    else
        std::cout << "n/a" << std::endl;

    // Check spacing option
    std::cout << "Spacing: ";
    if(spacingOpt->Exists())
    {
        for( int i = 0; i < 2; i++ )
            std::cout << spacingOpt->GetArgument(i) << " ; ";
        std::cout << spacingOpt->GetArgument(2) << std::endl;
    }
    else
        std::cout << "n/a" << std::endl;

    return ( 1 );
}
