#include "CompilerService.h"

#include "../gals_code/LexicalError.h"
#include "../gals_code/Lexico.h"
#include "../gals_code/Semantico.h"
#include "../gals_code/Sintatico.h"
#include "../gals_code/SyntacticError.h"

#include <sstream>

CompilationResult CompilerService::compile(const std::string &sourceCode) const
{
    const std::string normalizedSource = normalizeSource(sourceCode);

    Lexico lexico;
    lexico.setInput(normalizedSource.c_str());

    Sintatico sintatico;
    Semantico semantico;
    semantico.reset();

    auto appendSemanticMessages = [](std::string base, const std::vector<std::string> &messages) {
        for (const auto &msg : messages) {
            base += "\n" + msg;
        }
        return base;
    };

    try
    {
        sintatico.parse(&lexico, &semantico);
        semantico.logDeclaredButNotUsed();
        return {true, appendSemanticMessages("[SUCESSO] Compilacao concluida sem erros lexicos/sintaticos.", semantico.getMessages()), semantico.getSymbolRows()};
    }
    catch (const LexicalError &error)
    {
        std::ostringstream stream;
        stream << "[ERRO LEXICO] " << error.getMessage() << " (" 
             << formatPosition(normalizedSource, error.getPosition()) << ")";
        return {false, appendSemanticMessages(stream.str(), semantico.getMessages()), semantico.getSymbolRows()};
    }
    catch (const SyntacticError &error)
    {
        std::ostringstream stream;
        stream << "[ERRO SINTATICO] " << error.getMessage() << " (" 
               << formatPosition(normalizedSource, error.getPosition()) << ")";
        return {false, appendSemanticMessages(stream.str(), semantico.getMessages()), semantico.getSymbolRows()};
    }
    catch (const SemanticError &error){
        std::ostringstream stream;
        stream << "[ERRO SEMANTICO] " << error.getMessage() << " (" 
               << formatPosition(normalizedSource, error.getPosition()) << ")";
        return {false, appendSemanticMessages(stream.str(), semantico.getMessages()), semantico.getSymbolRows()};
    }
}

std::string CompilerService::normalizeSource(const std::string &sourceCode)
{
    std::string normalized;
    normalized.reserve(sourceCode.size());

    for (std::size_t i = 0; i < sourceCode.size();)
    {
        // Strip UTF-8 BOM both at file start and after line breaks.
        if (i + 2 < sourceCode.size() &&
            static_cast<unsigned char>(sourceCode[i]) == 0xEF &&
            static_cast<unsigned char>(sourceCode[i + 1]) == 0xBB &&
            static_cast<unsigned char>(sourceCode[i + 2]) == 0xBF)
        {
            i += 3;
            continue;
        }

        // Normalize CRLF/CR to LF.
        if (sourceCode[i] == '\r')
        {
            normalized.push_back('\n');
            if (i + 1 < sourceCode.size() && sourceCode[i + 1] == '\n')
            {
                i += 2;
            }
            else
            {
                ++i;
            }
            continue;
        }

        // Replace common UTF-8 non-breaking space with regular space.
        if (i + 1 < sourceCode.size() &&
            static_cast<unsigned char>(sourceCode[i]) == 0xC2 &&
            static_cast<unsigned char>(sourceCode[i + 1]) == 0xA0)
        {
            normalized.push_back(' ');
            i += 2;
            continue;
        }

        // Remove common zero-width unicode chars (UTF-8).
        if (i + 2 < sourceCode.size() &&
            static_cast<unsigned char>(sourceCode[i]) == 0xE2 &&
            static_cast<unsigned char>(sourceCode[i + 1]) == 0x80 &&
            (static_cast<unsigned char>(sourceCode[i + 2]) == 0x8B ||
             static_cast<unsigned char>(sourceCode[i + 2]) == 0x8C ||
             static_cast<unsigned char>(sourceCode[i + 2]) == 0x8D))
        {
            i += 3;
            continue;
        }

        normalized.push_back(sourceCode[i]);
        ++i;
    }

    return normalized;
}

std::string CompilerService::formatPosition(const std::string &sourceCode, int position)
{
    if (position < 0)
    {
        return "posicao desconhecida";
    }

    int line = 1;
    int column = 1;

    for (int i = 0; i < position && i < static_cast<int>(sourceCode.size()); ++i)
    {
        if (sourceCode[static_cast<std::string::size_type>(i)] == '\n')
        {
            ++line;
            column = 1;
        }
        else
        {
            ++column;
        }
    }

    std::ostringstream stream;
    stream << "linha " << line << ", coluna " << column;
    return stream.str();
}
