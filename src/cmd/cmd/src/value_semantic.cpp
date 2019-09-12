/*
 * Copyright (C) 2019
 */

#include "value_semantic.h"

static const std::string arg("arg");

// --------------------------- untyped_value -----------------------------------

std::string untyped_value::name() const { return arg; }
