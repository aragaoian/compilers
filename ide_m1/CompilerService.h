#ifndef COMPILER_SERVICE_H
#define COMPILER_SERVICE_H

#include <string>

struct CompilationResult
{
    bool success;
    std::string message;
};

class CompilerService
{
public:
    CompilationResult compile(const std::string &sourceCode) const;

private:
    static std::string normalizeSource(const std::string &sourceCode);
    static std::string formatPosition(const std::string &sourceCode, int position);
};

#endif
