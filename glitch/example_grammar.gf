# 
# glitch - Grammar Lexer and Interactive Terminal Command sHell
#
# Copyright 2018, Brendan McLearie, all rights reserved.
#
#  This file: 
#      - Cut down working version of the Fodder Factory Grammar

# Code to be directly included in the out.h header file
%header-start

#define DEAFULT_DURATION 1  // seconds

%header-end

# Code to be directly included in the user_code_out.cpp file
%code-start

// Nothing of substance included in user code

%code-end

# This separates the includes from the grammar code - must have
%grammar-start

# Various settings to be optionally specifed - see documentation
%ignore-case
%term-escape-sequence ^^^
%ccc-escape-sequence %%%

%enum-terminating-member
%enum-plus-list-array
%enum-start-value 0
%enum-array-member-label text
%enum-array-type SimpleStringArray

#------------------------------------ ENUM LIST and ARRAY Exmaples--------------------
%enum-identifier LED_COMMAND
%enum-array-instance led_command_strings
%enum-start
CMD_ERROR         "ERROR COMMAND"
CMD_ON            "ON"
CMD_OFF           "OFF"
CMD_LAZY          "Lazy"
CMD_PANIC         "PANIC"
CMD_CRAZY         "crazy"
LAST_COMMAND
%enum-end

%enum-identifier SAY_HOW
%enum-array-instance say_how_strings
%enum-start
SAY_ERROR         "ERROR SAY HOW"
SAY_CAPS          "caps"
SAY_LOWER         "lower"
SAY_MIXED         "mixed"
LAST_SAY_HOW
%enum-end

# --------------------------Grammar Actions and Lookups--------------------------------

# Lookups call into your code to get a string (or string value) to be used in the parser
%lookup-list LED_NAME LookupLEDName

# Action defines are labels to be used in the grammar and the names
#   of the functions in out_user_code.cpp that will be generated 
#   with parameters being any non-keyword. These functions are called
#   by the parser in itch at run-time
 
%action-define SET_LED_STATE_BY_NUM SetLEDStateByNum
%action-define SET_LED_STATE_BY_NAME SetLEDStateByName
%action-define SAY Say
%action-define SAY_WITH SayWith

# ---------------- Some Grammar --------
%1 keyword LED
%2 param-integer
%3 enum-array LED_COMMAND
%action SET_LED_STATE_BY_NUM
%2 lookup LED_NAME
%3 enum-array LED_COMMAND
%action SET_LED_STATE_BY_NAME

%1 keyword SAY
%2 param-string
%action SAY
%2 keyword WITH
%3 enum-array SAY_HOW
%action SAY_WITH

# -----------------------------------------
# need this
%grammar-end


