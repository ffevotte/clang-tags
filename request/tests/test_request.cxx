/** @example test_request.cxx
 *
 * This example demonstrates the use of the @ref request module to build an
 * application handling requests.
 */
#include "request/request.hxx"

//! [CommandParser]
using Request::CommandParser;
using Request::key;

/** @brief Example custom command parser */
class Repeat : public CommandParser {
public:
  /** @brief Constructor
   *  @param name  command name
   */
  Repeat (std::string name)
    : CommandParser (name, "repeat provided input")
  {
    // The constructor should set default values before keys are defined
    defaults ();

    add (key ("times", times_)
         ->metavar ("N")
         ->description ("number of times to repeat"));

    add (key ("input", input_)
         ->metavar ("STRING")
         ->description ("message to repeat")
         ->required());
  }

  void defaults () {
    // Set default values
    times_ = 2;
  }

  void run (std::ostream & cout) {
    // do something with key values
    for (int i=0 ; i<times_ ; ++i) {
      cout << input_ << std::endl;
    }
  }

private:
  // Variables where key values will be stored
  int         times_;
  std::string input_;
};
//! [CommandParser]


int main () {
  //![Parser]
  using Request::Parser;
  std::stringstream requests;


  // Create the command parser
  Parser p ("Example application");
  p .add (new Repeat ("repeat"))
    .prompt ("Command> ")
    .echo ();


  // Print a usage message
  p.help(std::cout);


  // Request detailed help about command "echo"
  requests << "help repeat" << std::endl << std::endl;


  // Call command "echo"
  requests << "repeat" << std::endl
           << "times 5" << std::endl
           << "input foo" << std::endl << std::endl;


  // Handle requests
  p.parse (requests, std::cout);
  //![Parser]


  return 0;
}
