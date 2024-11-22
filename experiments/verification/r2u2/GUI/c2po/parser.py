#type: ignore
from logging import getLogger

from .sly import Lexer, Parser
from .ast import *
from .logger import *

logger = getLogger(STANDARD_LOGGER_NAME)

class C2POLexer(Lexer):

    tokens = { KW_STRUCT, KW_INPUT, KW_DEFINE, KW_ATOMIC, KW_FTSPEC, KW_PTSPEC,
               TL_GLOBAL, TL_FUTURE, TL_HIST, TL_ONCE, TL_UNTIL, TL_RELEASE, TL_SINCE,
               LOG_NEG, LOG_AND, LOG_OR, LOG_IMPL, LOG_IFF, #LOG_XOR,
               BW_NEG, BW_AND, BW_OR, BW_XOR, BW_SHIFT_LEFT, BW_SHIFT_RIGHT,
               REL_EQ, REL_NEQ, REL_GTE, REL_LTE, REL_GT, REL_LT,
               ARITH_ADD, ARITH_SUB, ARITH_MUL, ARITH_DIV, ARITH_MOD, #ARITH_POW, ARITH_SQRT, ARITH_PM,
               ASSIGN, CONTRACT_ASSIGN, SYMBOL, DECIMAL, NUMERAL, SEMI, COLON, DOT, COMMA, #QUEST,
               LBRACK, RBRACK, LBRACE, RBRACE, LPAREN, RPAREN }

    # String containing ignored characters between tokens
    ignore = ' \t'
    ignore_comment = r'--.*'
    ignore_newline = r'\n+'

    REL_NEQ = r'!=|≠' # longer tokens must come first
    DECIMAL   = r'-?\d*\.\d+'
    NUMERAL     = r'-?[1-9][0-9]*|0'

    # Propositional logic ops/literals
    LOG_NEG  = r'!|¬'
    LOG_AND  = r'&&|∧'
    LOG_OR   = r'\|\||∨'
    # LOG_XOR  = r'XOR|⊕'
    LOG_IMPL = r'->|→'
    LOG_IFF  = r'<->|↔'

    # Bitwise ops
    BW_NEG          = r'~'
    BW_AND          = r'&' 
    BW_OR           = r'\|' 
    BW_XOR          = r'\^' 
    BW_SHIFT_LEFT   = r'<<'
    BW_SHIFT_RIGHT  = r'>>'

    # Relational ops
    REL_EQ  = r'=='
    REL_GTE = r'>=|≥'
    REL_LTE = r'<=|≤' 
    REL_GT  = r'>'
    REL_LT  = r'<'

    # Arithmetic ops
    ARITH_ADD   = r'\+'
    ARITH_SUB   = r'-'
    ARITH_MUL   = r'\*|•|⋅'
    ARITH_DIV   = r'/|÷'
    ARITH_MOD   = r'%'
    # ARITH_POW   = r'\*\*'
    # ARITH_SQRT  = r'√'
    # ARITH_PM    = r'\+/-|±'

    # Others
    CONTRACT_ASSIGN = r'=>'
    ASSIGN  = r':='
    SYMBOL  = r'[a-zA-Z_][a-zA-Z0-9_]*'
    # QUEST   = r'\?'
    SEMI    = r';'
    COLON   = r':'
    DOT     = r'\.'
    COMMA   = r','
    LBRACK  = r'\['
    RBRACK  = r'\]'
    LBRACE  = r'\{'
    RBRACE  = r'\}'
    LPAREN  = r'\('
    RPAREN  = r'\)'

    # Keywords
    SYMBOL['STRUCT'] = KW_STRUCT
    SYMBOL['INPUT'] = KW_INPUT
    SYMBOL['DEFINE'] = KW_DEFINE
    SYMBOL['ATOMIC'] = KW_ATOMIC
    SYMBOL['FTSPEC'] = KW_FTSPEC
    SYMBOL['PTSPEC'] = KW_PTSPEC
    SYMBOL['G'] = TL_GLOBAL
    SYMBOL['F'] = TL_FUTURE
    SYMBOL['H'] = TL_HIST
    SYMBOL['O'] = TL_ONCE
    SYMBOL['U'] = TL_UNTIL
    SYMBOL['R'] = TL_RELEASE
    SYMBOL['S'] = TL_SINCE

    # Extra action for newlines
    def ignore_newline(self, t):
        self.lineno += t.value.count('\n')

    def error(self, t):
        logger.error(f'{self.lineno}: Illegal character \'%s\' {t.value[0]}')
        self.index += 1


class C2POParser(Parser):
    tokens = C2POLexer.tokens

    # Using C operator precedence as a guide
    precedence = (
        ('left', LOG_IMPL, LOG_IFF),
        ('left', LOG_OR),
        ('left', LOG_AND),
        ('left', TL_UNTIL, TL_RELEASE, TL_SINCE),
        ('left', BW_OR),
        ('left', BW_XOR),
        ('left', BW_AND),
        ('left', REL_EQ, REL_NEQ),
        ('left', REL_GT, REL_LT, REL_GTE, REL_LTE),
        ('left', BW_SHIFT_LEFT, BW_SHIFT_RIGHT),
        ('left', ARITH_ADD, ARITH_SUB),
        ('left', ARITH_MUL, ARITH_DIV, ARITH_MOD),
        ('right', LOG_NEG, BW_NEG, UNARY_ARITH_SUB, TL_GLOBAL, TL_FUTURE, TL_HIST, TL_ONCE),
        ('right', LPAREN, DOT)
    )

    def __init__(self):
        super().__init__()
        self.structs: StructDict = {}
        self.signals: Dict[str,Type] = {}
        self.defs: Dict[str,Node] = {}
        self.contracts: Dict[str,int] = {}
        self.atomics: Dict[str,Node] = {}
        self.spec_num: int = 0
        self.has_ft = False
        self.has_pt = False
        self.status = True

    def error(self, token):
        self.status = False
        lineno = getattr(token, 'lineno', 0)
        logger.error(f"{lineno}: Syntax error, token='{token.value}'")

    @_('block ft_spec_block')
    def block(self, p):
        if self.has_ft:
            logger.warning(f"{p.lineno}: Multiple FTSPEC blocks, ignoring and using first instance.")
            return p[0]

        self.has_ft = True
        p[0][FormulaType.FT] = p[1]
        return p[0]

    @_('block pt_spec_block')
    def block(self, p):
        if self.has_pt:
            logger.warning(f"{p.lineno}: Multiple PTSPEC blocks, ignoring and using first instance.")
            return p[0]

        self.has_pt = True
        p[0][FormulaType.PT] = p[1]
        return p[0]

    @_('block struct_block',
       'block input_block',
       'block define_block',
       'block atomic_block')
    def block(self, p):
        return p[0]

    @_('')
    def block(self, p):
        return {}

    @_('KW_STRUCT struct struct_list')
    def struct_block(self, p):
        pass

    @_('struct_list struct', '')
    def struct_list(self, p):
        pass

    @_('SYMBOL COLON LBRACE variable_declaration_list RBRACE SEMI')
    def struct(self, p):
        self.structs[p[0]] = []
        for variables, type in p[3]:
            for v in variables:
                self.structs[p[0]].append((v,type))

    @_('KW_INPUT variable_declaration variable_declaration_list')
    def input_block(self, p):
        for variables, type in [p[1]]+p[2]:
            for v in variables:
                self.signals[v] = type

    @_('variable_declaration_list variable_declaration')
    def variable_declaration_list(self, p):
        p[0].append(p[1])
        return p[0]

    @_('')
    def variable_declaration_list(self, p):
        return []

    @_('SYMBOL symbol_list COLON type SEMI')
    def variable_declaration(self, p):
        return ([p[0]]+p[1], p[3])

    @_('symbol_list COMMA SYMBOL')
    def symbol_list(self, p):
        p[0].append(p[2])
        return p[0]

    @_('')
    def symbol_list(self, p):
        return []

    @_('SYMBOL', 'SYMBOL REL_LT type REL_GT')
    def type(self, p):
        symbol = p[0]

        if symbol == 'bool':
            return BOOL(False)
        elif symbol == 'int':
            return INT(False)
        elif symbol == 'float':
            return FLOAT(False)
        elif symbol == 'set':
            return SET(False, p[2])
        elif symbol in self.structs.keys():
            return STRUCT(False, symbol)

        logger.error(f'{p.lineno}: Type \'{p[0]}\' not recognized')
        self.status = False
        return NOTYPE()

    @_('KW_DEFINE definition definition_list')
    def define_block(self, p):
        pass

    @_('definition_list definition', '')
    def definition_list(self, p):
        pass

    @_('SYMBOL ASSIGN expr SEMI')
    def definition(self, p):
        ln = p.lineno
        variable = p[0]
        expr = p[2]

        if variable in self.signals.keys():
            logger.error(f'{ln}: Variable \'{variable}\' already declared.')
            self.status = False
        elif variable in self.defs.keys():
            logger.warning(f'{ln}: Variable \'{variable}\' defined twice, using last definition.')
            self.defs[variable] = expr
        else:
            self.defs[variable] = expr

    @_('KW_ATOMIC atomic atomic_list')
    def atomic_block(self, p):
        pass

    @_('atomic_list atomic', '')
    def atomic_list(self, p):
        pass

    @_('SYMBOL ASSIGN expr SEMI')
    def atomic(self, p):
        ln = p.lineno
        atomic = p[0]
        expr = p[2]

        if atomic in self.signals.keys():
            logger.error(f'{ln}: Atomic \'{atomic}\' name clashes with previously declared variable.')
            self.status = False
        elif atomic in self.atomics.keys():
            logger.warning(f'{ln}: Atomic \'{atomic}\' defined twice, using last definition.')
            self.atomics[atomic] = expr
        else:
            self.atomics[atomic] = expr

    # Future-time specification block
    @_('KW_FTSPEC spec_list')
    def ft_spec_block(self, p):
        ln = p.lineno
        return SpecificationSet(ln, FormulaType.FT, p[1])

    # Past-time specification block
    @_('KW_PTSPEC spec_list')
    def pt_spec_block(self, p):
        ln = p.lineno
        return SpecificationSet(ln, FormulaType.PT, p[1])

    @_('spec_list spec')
    def spec_list(self, p):
        p[0] += p[1]
        return p[0]

    @_('')
    def spec_list(self, p):
        return []

    # Basic specification
    @_('expr SEMI')
    def spec(self, p):
        ln = p.lineno
        expr = p[0]
        self.spec_num += 1
        return [Specification(ln, '', self.spec_num-1, expr)]

    # Labeled specification
    @_('SYMBOL COLON expr SEMI')
    def spec(self, p):
        ln = p.lineno
        label = p[0]
        expr = p[2]

        self.spec_num += 1
        spec = Specification(ln, label, self.spec_num-1, expr)

        if label in self.defs.keys():
            logger.warning(f'{ln}: Spec label identifier \'{label}\' previously declared, not storing')
            self.spec_num -= 1
        else:
            self.defs[label] = spec

        return [spec]

    # Contract
    @_('SYMBOL COLON expr CONTRACT_ASSIGN expr SEMI')
    def spec(self, p):
        ln = p.lineno
        label = p[0]

        self.spec_num += 3
        return [Contract(ln, label, self.spec_num-3, self.spec_num-2, self.spec_num-1, p[2], p[4])]

    @_('expr_list COMMA expr')
    def expr_list(self, p):
        p[0].append(p[2])
        return p[0]

    @_('')
    def expr_list(self, p):
        return []

    # Set expression
    @_('LBRACE expr expr_list RBRACE')
    def expr(self, p):
        return Set(p.lineno, [p[1]]+p[2])

    # Empty set expression
    @_('LBRACE RBRACE')
    def expr(self, p):
        return Set(ln, [])

    # Set aggregation expression with parameter
    @_('SYMBOL LPAREN SYMBOL bind_rule COLON expr COMMA expr RPAREN LPAREN expr RPAREN')
    def expr(self, p):
        ln = p.lineno
        operator = p[0]
        variable_name = p[2]
        mset = p[5]
        param = p[7]
        expr = p[10]

        boundvar = Variable(ln, variable_name)
        del self.defs[variable_name]

        if operator == 'forexactlyn':
            return ForExactlyN(ln, mset, param, boundvar, expr)
        elif operator == 'foratleastn':
            return ForAtLeastN(ln, mset, param, boundvar, expr)
        elif operator == 'foratmostn':
            return ForAtMostN(ln, mset, param, boundvar, expr)
        else:
            self.error(f'{ln}: Set aggregation operator \'{operator}\' not supported')
            self.status = False
            return Node(ln, [])

    # Set aggregation expression
    @_('SYMBOL LPAREN SYMBOL bind_rule COLON expr RPAREN LPAREN expr RPAREN')
    def expr(self, p):
        ln = p.lineno
        operator = p[0]
        variable_name = p[2]
        mset = p[5]
        expr = p[8]

        boundvar = Variable(ln, variable_name)
        del self.defs[variable_name]

        if operator == 'foreach':
            return ForEach(ln, mset, boundvar, expr)
        elif operator == 'forsome':
            return ForSome(ln, mset, boundvar, expr)
        else:
            self.error(f'{ln}: Set aggregation operator \'{operator}\' not supported')
            self.status = False
            return Node(ln, [])

    # Stub rule for binding a set agg variable
    @_('')
    def bind_rule(self, p):
        variable_name = p[-1]
        self.defs[variable_name] = Variable(0, variable_name)

        # TODO: p is empty, so p.lineno does not work
        # how to get boundvar the correct lineno?

    # Function/struct constructor expression nonempty arguments
    @_('SYMBOL LPAREN expr expr_list RPAREN')
    def expr(self, p):
        ln = p.lineno
        expr_list = [p[2]]+p[3]
        symbol = p[0]

        if symbol in self.structs:
            member_map: Dict[str,int] = {}
            if len(expr_list) == len(self.structs[symbol]):
                idx: int = 0
                for (m,t) in self.structs[symbol]:
                    member_map[m] = idx
                    idx += 1
                return Struct(ln, symbol, member_map, expr_list)
            else:
                logger.error(f'{ln}: Member mismatch for struct \'{symbol}\', number of members do not match')
                self.status = False
                return Node(ln, [])
        else:
            return Function(ln, symbol, expr_list)

    # Function/struct constructor expression empty arguments
    @_('SYMBOL LPAREN RPAREN')
    def expr(self, p):
        ln = p.lineno
        symbol = p[0]

        if symbol in self.structs.keys():
            if len(self.structs[symbol]) == 0:
                return Struct(ln,symbol,[])
            else:
                logger.error(f'{ln}: Member mismatch for struct \'{symbol}\', number of members do not match')
                self.status = False
                return Node(ln, [])
        else:
            return Function(ln, symbol, [])

    # Struct member access
    @_('expr DOT SYMBOL')
    def expr(self, p):
        return StructAccess(p.lineno, p[0], p[2])

    # Unary expressions
    @_('LOG_NEG expr',
       'BW_NEG expr',
       'ARITH_SUB expr %prec UNARY_ARITH_SUB')
    def expr(self, p):
        ln = p.lineno
        operator = p[0]

        if operator == '!':
            return LogicalNegate(ln, p[1])
        elif operator == '~':
            return BitwiseNegate(ln, p[1])
        elif operator == '-':
            return ArithmeticNegate(ln, p[1])
        else:
            logger.error(f'{ln}: Bad expression')
            self.status = False
            return Node(ln, [])

    # Binary expressions
    @_('expr LOG_IMPL expr',
       'expr LOG_IFF expr',
       'expr LOG_OR expr',
       'expr LOG_AND expr',
       'expr BW_OR expr',
       'expr BW_XOR expr',
       'expr BW_AND expr',
       'expr REL_EQ expr',
       'expr REL_NEQ expr',
       'expr REL_GT expr',
       'expr REL_LT expr',
       'expr REL_GTE expr',
       'expr REL_LTE expr',
       'expr BW_SHIFT_LEFT expr',
       'expr BW_SHIFT_RIGHT expr',
       'expr ARITH_ADD expr',
       'expr ARITH_SUB expr',
       'expr ARITH_MUL expr',
       'expr ARITH_DIV expr',
       'expr ARITH_MOD expr')
    def expr(self, p):
        ln = p.lineno
        operator = p[1]
        lhs = p[0]
        rhs = p[2]

        if operator == '->':
            return LogicalImplies(ln, lhs, rhs)
        elif operator == '<->':
            return LogicalIff(ln, lhs, rhs)
        elif operator == '||':
            return LogicalOr(ln, [lhs, rhs])
        elif operator == '&&':
            return LogicalAnd(ln, [lhs, rhs])
        elif operator == '|':
            return BitwiseOr(ln, lhs, rhs)
        elif operator == '^':
            return BitwiseXor(ln, lhs, rhs)
        elif operator == '&':
            return BitwiseAnd(ln, lhs, rhs)
        elif operator == '==':
            return Equal(ln, lhs, rhs)
        elif operator == '!=':
            return NotEqual(ln, lhs, rhs)
        elif operator == '>':
            return GreaterThan(ln, lhs, rhs)
        elif operator == '<':
            return LessThan(ln, lhs, rhs)
        elif operator == '>=':
            return GreaterThanOrEqual(ln, lhs, rhs)
        elif operator == '<=':
            return LessThanOrEqual(ln, lhs, rhs)
        elif operator == '<<':
            return BitwiseShiftLeft(ln, lhs, rhs)
        elif operator == '>>':
            return BitwiseShiftRight(ln, lhs, rhs)
        elif operator == '+':
            return ArithmeticAdd(ln, [lhs, rhs])
        elif operator == '-':
            return ArithmeticSubtract(ln, lhs, rhs)
        elif operator == '*':
            return ArithmeticMultiply(ln, lhs, rhs)
        elif operator == '/':
            return ArithmeticDivide(ln, lhs, rhs)
        elif operator == '%':
            return ArithmeticModulo(ln, lhs, rhs)
        else:
            logger.error(f'{ln}: Bad expression')
            self.status = False
            return Node(ln, [])

    # Unary temporal expressions
    @_('TL_GLOBAL interval expr',
       'TL_FUTURE interval expr',
       'TL_HIST interval expr',
       'TL_ONCE interval expr')
    def expr(self, p):
        ln = p.lineno
        operator = p[0]

        if operator == 'G':
            return Global(ln, p[2], p[1].lb, p[1].ub)
        elif operator == 'F':
            return Future(ln, p[2], p[1].lb, p[1].ub)
        elif operator == 'H':
            return Historical(ln, p[2], p[1].lb, p[1].ub)
        elif operator == 'O':
            return Once(ln, p[2], p[1].lb, p[1].ub)
        else:
            logger.error(f'{ln}: Bad expression')
            self.status = False
            return Node(ln, [])

    # Binary temporal expressions
    @_('expr TL_UNTIL interval expr',
       'expr TL_RELEASE interval expr',
       'expr TL_SINCE interval expr')
    def expr(self, p):
        ln = p.lineno
        operator = p[1]

        if operator == 'U':
            return Until(ln, p[0], p[3], p[2].lb, p[2].ub)
        elif operator == 'R':
            return Release(ln, p[0], p[3], p[2].lb, p[2].ub)
        elif operator == 'S':
            return Since(ln, p[0], p[3], p[2].lb, p[2].ub)
        else:
            logger.error(f'{ln}: Bad expression')
            self.status = False
            return Node(ln, [])

    # Parentheses
    @_('LPAREN expr RPAREN')
    def expr(self, p):
        return p[1]

    # Symbol
    @_('SYMBOL')
    def expr(self, p):
        ln = p.lineno
        symbol = p.SYMBOL

        if symbol == 'true':
            return Bool(ln, True)
        elif symbol == 'false':
            return Bool(ln, False)
        else:
            return Variable(ln, symbol)

    # Integer
    @_('NUMERAL')
    def expr(self, p):
        return Integer(p.lineno, int(p.NUMERAL))

    # Float
    @_('DECIMAL')
    def expr(self, p):
        return Float(p.lineno, float(p.DECIMAL))
        
    # Shorthand interval
    @_('LBRACK NUMERAL RBRACK')
    def interval(self, p):
        return Interval(0, int(p[1]))

    # Standard interval
    @_('LBRACK NUMERAL COMMA NUMERAL RBRACK')
    def interval(self, p):
        return Interval(int(p[1]), int(p[3]))
