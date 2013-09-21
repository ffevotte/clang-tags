#ifndef HXX_GETOPT
#define HXX_GETOPT

#include <vector>
#include <map>
#include <sstream>
#include <getopt.h>

/** @defgroup getopt GetOpt++
 *  @brief C++ wrapper around the @c getopt library
 */

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
 *
 *  @ingroup getopt
 */
class Getopt
{
public:
  /** @brief List of values for a command-line option */
  typedef std::vector<std::string> OptionValues;

  /** @brief Constructor
   *    @param argc           number of command-line arguments
   *    @param argv           command-line arguments
   *    @param documentation  description of non-switch command-line arguments
   */
  Getopt (int argc, char *const * argv, const char *documentation = "Usage: %c [options]");


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
   *  If the switch was given multiple times, only return the last value.
   *
   *  @param opt  option name
   *  @return "" if the switch is unset
   */
  inline std::string operator[] (std::string opt) const;

  /** @brief Get the number of times a command-line switch was given
   *  @param opt  option name
   *  @return count
   */
  inline int getCount (std::string opt) const;

  /** @brief Get all provided values for a command-line option
   *
   *  @param opt option name
   *  @return a vector of values
   */
  inline const OptionValues & getAll (std::string opt) const;


  /** @brief Shift remaining positional arguments
   *  @return the first remaining positional argument, "" if there are no more positional args
   */
  inline std::string shift ();

  /** @brief Get number of remaining positional arguments
   */
  inline int argc () const;

  /** @brief Get remaining positional arguments
   */
  inline char const *const * argv () const;

  /** @brief Get remaining positional arguments
   */
  inline std::string argv (int i) const;

private:
  void addOpt   (const char *longOpt, char shortOpt, int argument,
                 const char *description, const char *argDescription);
  void addUsage (const char *longOpt, char shortOpt, int argument,
                 const char *description, const char *argDescription);


private:
  typedef std::map<std::string, OptionValues> OptionsMap;
  typedef struct option Option;
  int                 argc_;
  char *const *       argv_;
  std::string         shortOpts_;
  std::ostringstream  usage_;
  std::vector<Option> options_;
  std::map<char,int>  shortIndex_;
  OptionsMap          cl_;
  static const OptionValues None_;
};

inline std::string Getopt::usage ()
{
  return usage_.str();
}

inline std::string Getopt::operator[] (std::string opt) const
{
  OptionsMap::const_iterator it = cl_.find(opt);
  if (it == cl_.end()) {
    return "";
  }

  return it->second.back();
}

inline int Getopt::getCount (std::string opt) const
{
  OptionsMap::const_iterator it = cl_.find(opt);
  if (it == cl_.end()) {
    return 0;
  }

  return it->second.size();
}

inline const Getopt::OptionValues & Getopt::getAll (std::string opt) const
{
  OptionsMap::const_iterator it = cl_.find(opt);
  if (it == cl_.end()) {
    return None_;
  }

  return it->second;
}

inline std::string Getopt::shift ()
{
  if (optind == argc_) {
    return "";
  }

  std::string res = argv_[optind];
  optind++;
  return res;
}

inline int Getopt::argc () const
{
  return argc_-optind;
}

inline char const *const * Getopt::argv () const
{
  return argv_+optind;
}

inline std::string Getopt::argv (int i) const
{
  return argv_[optind+i];
}

template <class T>
bool fromString(T& t, const std::string& s)
{
  std::istringstream iss(s);
  return !(iss >> t).fail();
}

#endif // HXX_GETOPT
