#ifndef HXX_GETOPT
#define HXX_GETOPT

#include <vector>
#include <map>
#include <sstream>
#include <getopt.h>

/** @brief Command-line arguments handler
 *
 *  @code
 *    Getopt opt (argc, argv);
 *    opt.add ("help", 'h', 0, "Show this help");
 *
 *    if (opt["help"] != "") {
 *      std::cout << opt.usage();
 *    }
 *  @endcode
 */
class Getopt
{
public:
  /** @brief Constructor
   *    @param argc       number of command-line arguments
   *    @param argv       command-line arguments
   *    @param arguments  description of non-switch command-line arguments
   */
  Getopt (int argc, char **argv, const char *arguments = "");


  /** @brief Define an option
   *    @param longOpt         long option name
   *    @param shortOpt        short option switch
   *    @param argument        0 (none), 1 (required), 2 (optional)
   *    @param description     long description of the option
   *    @param argDescription  argument description if applicable
   */
  void add (const char *longOpt, char shortOpt, int argument,
            const char *description = "", const char *argDescription = "");


  /** @brief Process options from the command-line
   *
   *  This method must be called after all calls to 'add'.
   */
  void get ();


  /** @brief Get usage string
   */
  inline std::string usage ();


  /** @brief Get the value of a command-line option
   *  @return "" if the switch is unset
   */
  inline std::string operator[] (std::string opt);


private:
  void addOpt   (const char *longOpt, char shortOpt, int argument,
                 const char *description, const char *argDescription);
  void addUsage (const char *longOpt, char shortOpt, int argument,
                 const char *description, const char *argDescription);


private:
  typedef struct option Option;
  int                 argc_;
  char              **argv_;
  std::string         shortOpts_;
  std::ostringstream  usage_;
  std::vector<Option> options_;
  std::map<char,int>  shortIndex_;
  std::map<std::string, std::string> cl_;
};

inline std::string Getopt::usage ()
{
  return usage_.str();
}

inline std::string Getopt::operator[] (std::string opt)
{
  return cl_[opt];
}

template <class T>
bool fromString(T& t, const std::string& s)
{
  std::istringstream iss(s);
  return !(iss >> t).fail();
}

#endif // HXX_GETOPT
