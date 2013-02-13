#include "getopt.hxx"

#include <iomanip>

Getopt::Getopt (int argc, char **argv, const char *arguments)
  : argc_ (argc),
    argv_ (argv)
{
  usage_ << "Usage: " << argv_[0] << " [options] ";
  if (arguments[0])
      usage_ << arguments;
  usage_ << "\n\n";
  usage_ << "Options:\n";
}

void Getopt::add (const char *longOpt, char shortOpt, int argument, const char *description, const char *argDescription)
{
  addOpt   (longOpt, shortOpt, argument, description, argDescription);
  addUsage (longOpt, shortOpt, argument, description, argDescription);
}

void Getopt::addUsage (const char *longOpt, char shortOpt, int argument,
                       const char *description, const char *argDescription)
{
  std::ostringstream tmp;
  tmp << "  --" << longOpt;
  if (argument == 1)
    {
      tmp << " <" << argDescription << ">";
    }

  if (shortOpt)
    {
      tmp << ", -" << shortOpt;
      if (argument == 1)
        {
          tmp << " <" << argDescription << ">";
        }
    }
  usage_ << std::setw(30) << std::left << tmp.str();
  usage_ << " " << description << "\n";
}

void Getopt::addOpt (const char *longOpt, char shortOpt, int argument,
                     const char *description, const char *argDescription)
{
  if (shortOpt)
    {
      shortOpts_ += shortOpt;
      shortIndex_[shortOpt] = options_.size();
      if (argument == 1)
        {
          shortOpts_ += ":";
        }
      if (argument == 2)
        {
          shortOpts_ += "::";
        }
    }

  options_.resize (1+options_.size());
  options_.back().name    = longOpt;
  options_.back().has_arg = argument;
  options_.back().flag    = 0;
  options_.back().val     = shortOpt;
}

void Getopt::get ()
{
  // Add the mandatory zero-filled last element
  if (options_.back().name)
    {
      options_.resize (1+options_.size());
      options_.back().name    = 0;
      options_.back().has_arg = 0;
      options_.back().flag    = 0;
      options_.back().val     = 0;
    }

  while (1)
    {
      int index = 0;
      char c = getopt_long (argc_, argv_, shortOpts_.c_str(), &(options_[0]), &index);
      if (c == -1)
        break;

      if (c == '?')
        throw std::string("Unknown option");
      else
        {
          if (c != 0)
            {
              index = shortIndex_[c];
            }
          
          if (options_[index].has_arg == 0)
            {
              cl_[std::string (options_[index].name)] = std::string ("true");
            }
          else
            {
              cl_[std::string (options_[index].name)] = std::string (optarg);
            }
        }
    }
}
