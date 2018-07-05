#include "FormatTestUtils.h"
#include "clang/Format/Format.h"
#include "llvm/Support/Debug.h"
#include "gtest/gtest.h"

#define DEBUG_TYPE "format-test"

namespace clang {
namespace format {
class FormatTestXC : public ::testing::Test {
protected:
  static std::string format(llvm::StringRef Code, unsigned Offset,
                            unsigned Length, const FormatStyle &Style) {
    LLVM_DEBUG(llvm::errs() << "---\n");
    LLVM_DEBUG(llvm::errs() << Code << "\n\n");
    std::vector<tooling::Range> Ranges(1, tooling::Range(Offset, Length));
    FormattingAttemptStatus Status;
    tooling::Replacements Replaces =
        reformat(Style, Code, Ranges, "<stdin>", &Status);
    EXPECT_TRUE(Status.FormatComplete);
    auto Result = applyAllReplacements(Code, Replaces);
    EXPECT_TRUE(static_cast<bool>(Result));
    LLVM_DEBUG(llvm::errs() << "\n" << *Result << "\n\n");
    return *Result;
  }

  static std::string format(
      llvm::StringRef Code,
      const FormatStyle &Style = getGoogleStyle(FormatStyle::LK_XC)) {
    return format(Code, 0, Code.size(), Style);
  }

  static FormatStyle getGoogleJSStyleWithColumns(unsigned ColumnLimit) {
    FormatStyle Style = getGoogleStyle(FormatStyle::LK_XC);
    Style.ColumnLimit = ColumnLimit;
    return Style;
  }

  static void verifyFormat(
      llvm::StringRef Code,
      const FormatStyle &Style = getGoogleStyle(FormatStyle::LK_XC)) {
    EXPECT_EQ(Code.str(), format(Code, Style))
        << "Expected code is not stable";
    std::string Result = format(test::messUp(Code), Style);
    EXPECT_EQ(Code.str(), Result) << "Formatted:\n" << Result;
  }

  static void verifyFormat(
      llvm::StringRef Expected,
      llvm::StringRef Code,
      const FormatStyle &Style = getGoogleStyle(FormatStyle::LK_XC)) {
    EXPECT_EQ(Expected.str(), format(Expected, Style))
        << "Expected code is not stable";
    std::string Result = format(Code, Style);
    EXPECT_EQ(Expected.str(), Result) << "Formatted:\n" << Result;
  }
};

TEST_F(FormatTestXC, Write) {
  EXPECT_EQ("c <: 4;", format("c<:4;"));
}

TEST_F(FormatTestXC, Read) {
  EXPECT_EQ("c :> 4;", format("c:>4;"));
}

TEST_F(FormatTestXC, Par) {
  EXPECT_EQ(
    "par {\n"
    "  f();\n"
    "  g();\n"
    "}\n",
    format("par{f();g();}\n")
  );

  EXPECT_EQ(
    "par (int i = 0; i < 16; ++i) {\n"
    "  f(i);\n"
    "}\n",
    format("par (int i = 0; i < 16; ++i) {f(i);}\n")
  );
}
}
}
