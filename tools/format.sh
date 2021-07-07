#!/bin/bash
# Use this script to re-format C++ files to
clang-tidy $1 -fix -checks="readability-braces-around-statements"
clang-format -i $1
