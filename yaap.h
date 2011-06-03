// Copyright 2011 Jérôme Velut. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
//
//    1. Redistributions of source code must retain the above copyright notice, this list of
//       conditions and the following disclaimer.
//
//    2. Redistributions in binary form must reproduce the above copyright notice, this list
//       of conditions and the following disclaimer in the documentation and/or other materials
//       provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY Jérôme Velut ``AS IS'' AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
// FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Jérôme Velut OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// The views and conclusions contained in the software and documentation are those of the
// authors and should not be interpreted as representing official policies, either expressed
// or implied, of Jérôme Velut.
//
// ORIGINAL HOME PAGE: https://github.com/jeromevelut/yaap

#ifndef __yaap_h__
#define __yaap_h__

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

//! \namespace yaap contains the classes for command line arguments parsing
namespace yaap {

//! \class Option
//! \brief Defines a boolean option
//!
//! An option is defined by its flag, a description and a state. The flag and
//! and description are given at construction, whereas the state is defined
//! after the effective parsing.
class Option {
public:
    //! constructor.
    Option( char flag, std::string description )
    {
        this->flag = flag;
        this->description = description;
        this->state = false;
        this->required = false;
        this->error = false;
    };

    //! Get the flag of this option.
    char Flag( ) {
        return( this->flag);
    };
    //! Set the state (exists or not in the command line)
    void Exists(bool state) {
        this->state = state;
    };
    //! Get the state. If true, the option exists in the command line
    bool Exists() {
        return(this->state);
    };

   //! Set the requirement level (required is true, optional is false)
   void SetRequired( bool r ){
         this->required = r;
  };

   //! Set the error flag to true
   void RaiseError( ){
      this->error =true;
   };
   
   //! Get the error state
   bool ErrorFlag(){
      return( this->error );
   };

   bool IsRequired( ){
         return( this->required );
   };

   //! Print how to use the option in the command line format
   virtual void CLUsage( )
   {
      std::cout << " [-" << this->Flag() << "]";
   };

   std::string GetDescription( ){
           return( this->description );
   };

protected:
    char flag; //!< Command line flag character
    std::string description;//!< Short description
    bool state;//!< true if present in the command line
    bool required;//!< if true, the absence of the option in the command line will raise an error in the parser.
    bool error; //!< if true, an error occured while parsing.
};

//! \class OptionArg
//! \brief Defines an option with possibly several arguments
//!
//! This is a templated class derived from Option. It adds a vector of
//! arguments which type and number is defined by the template statements T
//! and N.
template<class T, int N>
class OptionArg : public Option {

public:
    //! constructor
    OptionArg( char flag, std::string description ):Option(flag,description)
    {
        this->nbArgs = N;
    };

    //! Set the pos-th arg of type T.
    void SetArgument( T arg, int pos ) {
        this->argVector[pos] = arg;
    };
    //! Get the pos-th arg of type T
    T GetArgument( int pos ) {
        return( argVector[pos] );
    };

   //! Print how to use the option in the command line format
   virtual void CLUsage( )
   {
      std::cout << " [-" << this->Flag();
      for( int i = 0; i < nbArgs; i++ )
         std::cout << " x";
      std::cout << "]"; 
   };

protected:
    int nbArgs; //!< Number of arguments of this specific option
    T argVector[N]; //!< Vector of arguments of type T
};

//! \class Parser
//! \brief Manages a set of options
class Parser {
public:
    //! constructor. Initializes number of args and argument vector.
    Parser( int argc, char** argv, std::string description = "" )
    {
        this->nbArgs = argc;
        this->argv = argv;
        this->error = false;
        this->description = description;
    };

    //! destructor
    virtual ~Parser()
    {
        // todo: delete options
    };

    //! Add a simple option with given flag and description to the options
    //! vector and check its existence.
    //! \return the instanciated Option
    Option* AddOption( char flag, std::string description, bool required = false )
    {
        Option* option = new Option( flag, description );
        option->SetRequired( required );
        // For each argument in the command, check the underlying string
        for( int i = 1; i < this->nbArgs; i++ )
        {
            if( this->argv[i][0] == '-' ) // This is an option(s) statement
            {
                // The option flag can be concatenated after a unique '-'
                int c = 0;
                while(argv[i][c] != '\0' )
                {
                    if( argv[i][c] == option->Flag() ) // It is the processed option
                        option->Exists(true); // toggle the state in the Option object
                    c++;
                }
            }
        }
        // Put the Option in the options' array.
        this->optionVector.push_back( option );
        // if required but not found, raise an error
        if( !option->Exists() && required )
        {
           option->RaiseError( );
           this->error = true;
        }
        // Return the created Option for the user to use it in the main program
        return( option );
    };

    //! Add an option with arguments with given flag and description to the
    //! options vector, check its existence and perform the sub-arguments
    //! assignment. There are N sub-arguments of type T.
    //! \return the instanciated OptionArg
    template<class T, int N>
    OptionArg<T,N>* AddOptionArg( char flag, std::string description, bool required = false )
    {
        // option allocation
        OptionArg<T,N>* option = new OptionArg<T,N>( flag, description );
        option->SetRequired( required);
  
        // parse the argument list
        for( int i = 1; i < this->nbArgs; i++ )
        {
            if( this->argv[i][0] == '-' ) // This is an option(s) statement
            {
                if( argv[i][1] == option->Flag() ) // this is the processed option
                {
                    // toggle the state of the option to true
                    option->Exists(true);
                    if( i + N >= this->nbArgs )
                    {
                       option->RaiseError();
                       this->error = true;
                    }
                    else
                       for( int argIdx = 1; argIdx <= N ; argIdx++)
                       {
                           // For each sub-argument, memorize the command line value in
                           // the OptionArg object
                           std::istringstream argStream( argv[i+argIdx] );
                           T arg;
                           argStream >> arg;
                           if( argStream.fail() )
                           {
                              option->RaiseError();
                              this->error = true;
                           }
                           option->SetArgument( arg, argIdx-1 );
                       }
                }
            }
        }
        // Put the Option in the options' array.
        this->optionVector.push_back( option );
        // if required but not found, raise an error
        if( !option->Exists() && required )
        {
           this->error = true;
           option->RaiseError();
        }
        // Return the created Option for the user to use it in the main program
        return( option );
    };

   void Usage( )
   {
      std::cout <<std::endl<< "Utility " << this->argv[0] << " :" << std::endl;
      std::cout << std::endl<<this->description << std::endl;
      std::cout << std::endl<<"Usage: \n [shell]$ " << this->argv[0];
      for( int i = 0; i < optionVector.size(); i++ )
         optionVector[i]->CLUsage();
      std::cout << std::endl;

      for( int i = 0; i < optionVector.size(); i++ )
      {
         std::string requirement;
         if( optionVector[i]->IsRequired( ))
            requirement = "Required";
         else
            requirement = "Optional";

         if( optionVector[i]->ErrorFlag() )
            std::cout << "     *\t";
         else
            std::cout << "\t";
         std::cout << "-" << optionVector[i]->Flag() 
                   << " : " << optionVector[i]->GetDescription() 
                   << " ("<<requirement<<")."<<std::endl;
      }
      std::cout << "* indicate(s) wrong argument(s)."<< std::endl;
   };

   bool IsCommandLineValid( )
   { return( !this->error );};

   void SetDescription( std::string desc )
   { this->description = desc; };

private:
    int nbArgs; //!< Number of arguments (argc)
    char** argv; //!< Arguments' vector
    std::vector<Option*> optionVector; //!< vector of options
    bool error; //!< raised to 1 when one of the arguments in the command line is not valid
    std::string description; //!< give a general description of the command.
};


}; //end namespace yaap

#endif //yaap
