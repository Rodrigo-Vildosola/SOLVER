#pragma once

#include "pch.h"
#include "token.h"
#include "function.h"
#include "exception.h"


EvalFunc compilePostfix(const std::vector<Token>& tokens, const std::unordered_map<std::string, Function>& functions);
