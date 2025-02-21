import re

# Token types
NUMBER    = 'NUMBER'
PLUS      = 'PLUS'
MINUS     = 'MINUS'
MULTIPLY  = 'MULTIPLY'
DIVIDE    = 'DIVIDE'
POWER     = 'POWER'
LPAREN    = 'LPAREN'
RPAREN    = 'RPAREN'
VARIABLE  = 'VARIABLE'
EOF       = 'EOF'

# Token specification (regex patterns)
token_specification = [
    (NUMBER,    r'\d+(\.\d+)?'),       # Integer or decimal number
    (PLUS,      r'\+'),                # Plus symbol
    (MINUS,     r'-'),                 # Minus symbol
    (MULTIPLY,  r'\*'),                # Multiplication symbol
    (DIVIDE,    r'/'),                 # Division symbol
    (POWER,     r'\^'),                # Exponentiation symbol
    (LPAREN,    r'\('),                # Left Parenthesis
    (RPAREN,    r'\)'),                # Right Parenthesis
    (VARIABLE,  r'[a-zA-Z_]\w*'),      # Variable names
    ('SKIP',    r'[ \t]+'),            # Skip over spaces and tabs
    ('MISMATCH',r'.'),                 # Any other character (error)
]

# Combine token regex patterns
token_regex = '|'.join(f'(?P<{pair[0]}>{pair[1]})' for pair in token_specification)

# Token class for storing token info
class Token:
    def __init__(self, type_, value, position):
        self.type = type_
        self.value = value
        self.position = position
    
    def __repr__(self):
        return f'Token({self.type}, {self.value}, pos={self.position})'

def tokenize(code):
    """
    Tokenize the input string 'code' into a list of tokens.
    """
    tokens = []
    for match in re.finditer(token_regex, code):
        token_type = match.lastgroup
        token_value = match.group(token_type)
        pos = match.start()
        
        if token_type == NUMBER:
            # Convert numeric token to int or float
            token_value = float(token_value) if '.' in token_value else int(token_value)
            tokens.append(Token(NUMBER, token_value, pos))
        elif token_type in (PLUS, MINUS, MULTIPLY, DIVIDE, POWER, LPAREN, RPAREN, VARIABLE):
            tokens.append(Token(token_type, token_value, pos))
        elif token_type == 'SKIP':
            continue  # Ignore whitespace
        elif token_type == 'MISMATCH':
            raise RuntimeError(f'Unexpected character {token_value!r} at position {pos}')
    # Append an EOF token at the end to signify the end of input
    tokens.append(Token(EOF, None, len(code)))
    return tokens

def shunting_yard(tokens):
    """
    Convert a list of tokens (from tokenize()) in infix order
    into a list of tokens in postfix (Reverse Polish) order.
    """
    output_queue = []
    operator_stack = []

    # Operator precedence and associativity
    precedence = {
        PLUS: 2,
        MINUS: 2,
        MULTIPLY: 3,
        DIVIDE: 3,
        POWER: 4
    }
    associativity = {
        PLUS: 'L',
        MINUS: 'L',
        MULTIPLY: 'L',
        DIVIDE: 'L',
        POWER: 'R'  # Exponentiation is right-associative
    }

    for token in tokens:
        if token.type in (NUMBER, VARIABLE):
            output_queue.append(token)
        elif token.type in (PLUS, MINUS, MULTIPLY, DIVIDE, POWER):
            while operator_stack:
                top = operator_stack[-1]
                if top.type in (PLUS, MINUS, MULTIPLY, DIVIDE, POWER):
                    if (associativity[token.type] == 'L' and precedence[token.type] <= precedence[top.type]) or \
                       (associativity[token.type] == 'R' and precedence[token.type] < precedence[top.type]):
                        output_queue.append(operator_stack.pop())
                        continue
                break
            operator_stack.append(token)
        elif token.type == LPAREN:
            operator_stack.append(token)
        elif token.type == RPAREN:
            # Pop operators until left parenthesis is encountered
            while operator_stack and operator_stack[-1].type != LPAREN:
                output_queue.append(operator_stack.pop())
            if operator_stack and operator_stack[-1].type == LPAREN:
                operator_stack.pop()  # Discard the left parenthesis
            else:
                raise RuntimeError("Mismatched parentheses")
        elif token.type == EOF:
            break

    # After processing all tokens, pop any remaining operators
    while operator_stack:
        top = operator_stack.pop()
        if top.type in (LPAREN, RPAREN):
            raise RuntimeError("Mismatched parentheses")
        output_queue.append(top)
    return output_queue

def evaluate_postfix(postfix_tokens, variables=None):
    """
    Evaluate a postfix expression (list of tokens).
    The 'variables' parameter should be a dictionary mapping variable names to numeric values.
    """
    if variables is None:
        variables = {}

    stack = []
    for token in postfix_tokens:
        if token.type == NUMBER:
            stack.append(token.value)
        elif token.type == VARIABLE:
            if token.value in variables:
                stack.append(variables[token.value])
            else:
                raise RuntimeError(f"Undefined variable: {token.value}")
        elif token.type in (PLUS, MINUS, MULTIPLY, DIVIDE, POWER):
            # Pop two operands (note the order: a operator b, where b is popped first)
            if len(stack) < 2:
                raise RuntimeError("Insufficient operands")
            b = stack.pop()
            a = stack.pop()
            if token.type == PLUS:
                result = a + b
            elif token.type == MINUS:
                result = a - b
            elif token.type == MULTIPLY:
                result = a * b
            elif token.type == DIVIDE:
                result = a / b
            elif token.type == POWER:
                result = a ** b
            stack.append(result)
        # We ignore EOF tokens or any other tokens
    if len(stack) != 1:
        raise RuntimeError("The user input has too many values for the number of operators.")
    return stack[0]

# Example usage
if __name__ == '__main__':
    expression = "3 + 4.5 * (2 - x)^2"
    print("Infix Expression:", expression)
    
    # Tokenize the expression
    tokens = tokenize(expression)
    print("\nTokens:")
    for token in tokens:
        print(token)
    
    # Convert tokens to postfix using the Shunting Yard algorithm
    postfix_tokens = shunting_yard(tokens)
    
    print("\nPostfix Tokens:")
    # Print postfix tokens in order; skipping the EOF token
    for token in postfix_tokens:
        if token.type != EOF:
            print(token.value, end=' ')
    print()  # Newline at the end
    
    # Define variable values for evaluation (e.g., x = 1)
    variables = {"x": 1}
    
    # Evaluate the postfix expression
    result = evaluate_postfix(postfix_tokens, variables)
    print("\nEvaluation Result:", result)
