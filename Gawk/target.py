
import re

class Token:
    T_none = 0
    T_plus = 1
    T_minus = 2
    T_multiply = 3
    T_divide = 4
    T_modulo = 5
    T_l_parenthesis = 6
    T_r_parenthesis = 7
    T_integer = 8
    T_float = 9
    T_space = 10

    def __init__(self, token_type, token_value=None):
        self.type = token_type
        self.value = token_value

class TokenParser:
    def __init__(self, source_file):
        self.source = open(source_file, 'rb').read()
        self.pos = 0
        self.token = Token(Token.T_none)

    def next(self):
        # TODO: move to next token
        return self.token

class S_expression_1:
    s_term = None

class S_expression_2:
    s_left = None
    s_operator = None
    s_right = None

class S_term_1:
    s_item = None

class S_term_2:
    s_left = None
    s_operator = None
    s_right = None

class S_item_1:
    s_number = None

class S_item_2:
    s_expression = None

class S_number_1:
    s_integer = None

class S_number_2:
    s_float = None

class AstBuilder:
    def __init__(self, token_parser):
        self.token_parser = token_parser
