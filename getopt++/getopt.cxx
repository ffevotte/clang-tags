#include "getopt.hxx"

#include <iomanip>

const Getopt::OptionValues Getopt::None_;

Getopt::Getopt (int argc, char **argv, const char *documentation)
  : argc_ (argc),
    argv_ (argv)
{
  std::string doc = documentation;
  size_t pos = 0;
  do {
    pos = doc.find ("%c", pos);
    if (pos != std::string::npos)
      doc.replace (pos, 2, argv_[0]);
  } while (pos != std::string::npos);

  usage_ << doc << "\n\n"
         << "Options:\n";
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

          std::string optionName = options_[index].name;
          if (options_[index].has_arg == 0)
            {
              cl_[optionName].push_back("true");
            }
          else
            {
              cl_[optionName].push_back(optarg);
            }
        }
    }
}
