#include "application.hxx"

#include <clang-c/Index.h>
#include <cassert>


namespace LibClang {
  class Chunk;

  class Completion {
  public:
    Completion (CXCompletionString completionString)
      : completionString_ (completionString)
    { }

    unsigned int size () {
      return clang_getNumCompletionChunks(raw());
    }
    Chunk chunk (unsigned int i);

    unsigned int priority () {
      return clang_getCompletionPriority(raw());
    }

    CXAvailabilityKind availability () {
      return clang_getCompletionAvailability(raw());
    }

    CXCompletionString & raw () {
      return completionString_;
    }

  private:
    CXCompletionString completionString_;
  };

  class Chunk {
  public:
    Chunk (Completion & completion, unsigned int index)
      : completion_ (completion),
        index_      (index)
    {}

    CXCompletionChunkKind kind () {
      return clang_getCompletionChunkKind(completion_.raw(), index_);
    }

    std::string kindStr () {
      switch (kind()) {
      case CXCompletionChunk_Optional:         return "Optional";
      case CXCompletionChunk_TypedText:        return "TypedText";
      case CXCompletionChunk_Text:             return "Text";
      case CXCompletionChunk_Placeholder:      return "Placeholder";
      case CXCompletionChunk_Informative:      return "Informative";
      case CXCompletionChunk_CurrentParameter: return "CurrentParameter";
      case CXCompletionChunk_LeftParen:        return "LeftParen";
      case CXCompletionChunk_RightParen:       return "RightParen";
      case CXCompletionChunk_LeftBracket:      return "LeftBracket";
      case CXCompletionChunk_RightBracket:     return "RightBracket";
      case CXCompletionChunk_LeftBrace:        return "LeftBrace";
      case CXCompletionChunk_RightBrace:       return "RightBrace";
      case CXCompletionChunk_LeftAngle:        return "LeftAngle";
      case CXCompletionChunk_RightAngle:       return "RightAngle";
      case CXCompletionChunk_Comma:            return "Comma";
      case CXCompletionChunk_ResultType:       return "ResultType";
      case CXCompletionChunk_Colon:            return "Colon";
      case CXCompletionChunk_SemiColon:        return "SemiColon";
      case CXCompletionChunk_Equal:            return "Equal";
      case CXCompletionChunk_HorizontalSpace:  return "HorizontalSpace";
      case CXCompletionChunk_VerticalSpace:    return "VerticalSpace";
      }

      return "Unknown";
    }

    std::string text () {
      CXString s = clang_getCompletionChunkText(completion_.raw(), index_);
      std::string ret = clang_getCString(s);
      clang_disposeString(s);
      return ret;
    }

    Completion completion () {
      if (kind() != CXCompletionChunk_Optional) {
        throw std::runtime_error ("LibClang::CodeCompletions::Chunk::completion()");
      }
      return clang_getCompletionChunkCompletionString(completion_.raw(), index_);
    }

  private:
    Completion & completion_;
    unsigned int index_;
  };

  class CompletionResult {
  public:
    CompletionResult (CXCompletionResult completionResult)
      : completionResult_ (completionResult)
    {}

    Completion get () {
      return completionResult_.CompletionString;
    }

    CXCursorKind kind () {
      return completionResult_.CursorKind;
    }

    std::string kindStr () {
      CXString kindSpelling = clang_getCursorKindSpelling (kind());
      std::string res = clang_getCString (kindSpelling);
      clang_disposeString (kindSpelling);
      return res;
    }

  private:
    CXCompletionResult completionResult_;
  };

  class CodeCompletions {
  public:
    CodeCompletions (CXCodeCompleteResults * results)
      : codeCompletions_ (new CodeCompletions_ (results))
    { }

    unsigned int size () {
      return codeCompletions_->results_->NumResults;
    }

    CompletionResult operator[] (unsigned int i) {
      return codeCompletions_->results_->Results[i];
    }

    void sort () {
      clang_sortCodeCompletionResults(codeCompletions_->results_->Results,
                                      size());
    }

  private:
    struct CodeCompletions_ {
      CXCodeCompleteResults * results_;
      CodeCompletions_ (CXCodeCompleteResults * results) : results_ (results) {}
      ~CodeCompletions_ () { clang_disposeCodeCompleteResults(results_); }
    };
    std::shared_ptr<CodeCompletions_> codeCompletions_;
  };


  Chunk Completion::chunk (unsigned int i) {
    return Chunk (*this, i);
  }
}

void printCompletionString(LibClang::Completion completion, std::ostream & stream) {
  int n = completion.size();
  for (int i = 0; i != n; ++i) {
    LibClang::Chunk chunk = completion.chunk(i);

    switch (chunk.kind()) {
    case CXCompletionChunk_ResultType:
      stream << "[#" << chunk.text() << "#]";
      break;
    case CXCompletionChunk_Placeholder:
      stream << "<#" << chunk.text() << "#>";
      break;
    case CXCompletionChunk_Optional:
      stream << "{optional: ";
      printCompletionString(chunk.completion(), stream);
      stream << "}";
      break;

    case CXCompletionChunk_VerticalSpace:
      break;

    default:
      stream << chunk.text();
    }
  }
}

void printCompletionResult(LibClang::CompletionResult completionResult,
                             std::ostream & stream) {
  LibClang::Completion completion = completionResult.get();

  stream << "COMPLETION";

  // switch (completion.availability()){
  // case CXAvailability_Available:
  //   break;
  // case CXAvailability_Deprecated:
  //   stream << "(deprecated)";
  //   break;
  // case CXAvailability_NotAvailable:
  //   stream << "(unavailable)";
  //   break;
  // case CXAvailability_NotAccessible:
  //   stream << "(inaccessible)";
  //   break;
  // }
  // stream << "(" << completion.priority() << ")";

  stream << ": ";

  int n = completion.size();
  for (int i = 0 ; i < n ; ++i) {
    LibClang::Chunk chunk = completion.chunk(i);
    if (chunk.kind() == CXCompletionChunk_TypedText) {
      stream << chunk.text() << " : ";
      break;
    }
  }

  printCompletionString(completion, stream);

  stream << std::endl;
}

void Application::complete (CompleteArgs & args) {
  LibClang::TranslationUnit & tu = translationUnit_ (args.fileName);

  CXCodeCompleteResults * results
    = clang_codeCompleteAt(tu.raw(),
                           args.fileName.c_str(), args.line, args.column,
                           0, 0,
                           clang_defaultCodeCompleteOptions());
  LibClang::CodeCompletions completions (results);
  completions.sort();

  int n = completions.size();
  for (int i = 0 ; i != n ; ++i)
    printCompletionResult (completions[i], std::cout);
}
