# MLTL Formula Syntax

This document describes the syntax for Metric Linear Temporal Logic (MLTL) formulas supported by the WEST parser.

## Quick Start

Basic examples:
```
true                           # Boolean true
false                          # Boolean false
x                              # Proposition variable
x & y                          # Conjunction (AND)
x | y                          # Disjunction (OR)
!x                             # Negation (NOT)
G[0,5](x)                      # Globally in interval [0,5]
F[1,10](y)                     # Future in interval [1,10]
```

## Literals

| Type | Syntax | Example |
|------|--------|---------|
| Boolean True | `true` or `TRUE` or `True` | `true` |
| Boolean False | `false` or `FALSE` or `False` | `false` |
| Proposition | Any identifier | `x`, `var_1`, `property_p` |

Propositions are identifiers containing letters, digits, and underscores (`[a-zA-Z0-9_]+`).

## Boolean Operators

All boolean operators are **case-insensitive** and support multiple syntax variants:

### Conjunction (AND)
```
x & y          # Single ampersand
x && y         # Double ampersand
x AND y        # Keyword
x and y        # Lowercase keyword
x And y        # Mixed case
```

### Disjunction (OR)
```
x | y          # Single pipe
x || y         # Double pipe
x OR y         # Keyword
x or y         # Lowercase keyword
x Or y         # Mixed case
```

### Negation (NOT)
```
!x             # Exclamation mark
NOT x          # Keyword
not x          # Lowercase keyword
Not x          # Mixed case
```

### Operator Precedence
Operators are evaluated with the following precedence (highest to lowest):
1. `NOT` / `!`
2. `AND` / `&` / `&&`
3. `OR` / `|` / `||`

Parentheses can be used to override precedence:
```
(x & y) | z    # OR has higher precedence
```

## Temporal Operators

All temporal operators take an interval `[lb, ub]` where `lb ≤ ub` (nonnegative integers).

### Global (G)
Asserts that a property holds globally (throughout an interval).

```
G[0,5](x)           # x global in [0,5] time units
GLOBAL[0,5](x)      # Full keyword version
global[0,5](x)      # Case-insensitive keyword
```

Format: Single-letter form `G` MUST be uppercase. Keyword form is case-insensitive.

### Future (F)
Asserts that a property eventually holds (within an interval).

```
F[1,10](y)          # y eventually in [1,10] time units
FUTURE[1,10](y)     # Full keyword version
future[1,10](y)     # Case-insensitive keyword
```

Format: Single-letter form `F` MUST be uppercase. Keyword form is case-insensitive.

### Until (U)
Binary operator: left property holds until right property holds (within time interval).

```
x U[1,5] y           # x until y in [1,5]
x UNTIL[1,5] y       # Full keyword
x until[1,5] y       # Case-insensitive
```

Format: Single-letter form `U` MUST be uppercase. Keyword form is case-insensitive.

### Release (R)
Binary operator: right property holds until left property holds (within time interval).

```
x R[1,5] y           # x release y in [1,5]
x RELEASE[1,5] y     # Full keyword
x release[1,5] y     # Case-insensitive
```

Format: Single-letter form `R` MUST be uppercase. Keyword form is case-insensitive.

## Temporal Bounds

Bounds are specified in square brackets with format `[lower_bound, upper_bound]`:
```
[0, 5]         # All nonnegative integers
[1, 10]        # From 1 to 10
```

**Requirements:**
- Lower bound ≤ upper bound (enforced by `welldef()` check)
- Both bounds must be nonnegative
- Bounds are parsed as 64-bit unsigned integers

## Complex Examples

### Nested Operators
```
G[0,5](x & F[1,3](y))         # Globally (x AND eventually y)
```

### Mixed Syntax
```
NOT (X AND Y) OR Z            # All operators mixed case
g[0,5](x)                     # Lowercase 'g' parsed as proposition
G[0,5](x)                     # Uppercase 'G' parsed as Global operator
```

### With Temporal Operators
```
G[0,10]( (x | y) & z )        # Complex nesting
x U[1,5] (y & F[2,4](z))      # Until with nested Future
```

## Whitespace

Whitespace is flexible and ignored in most places:
```
G[0,5](x)              # No spaces
G [ 0 , 5 ] ( x )      # With spaces
G[0,5] (x)             # Space before parentheses
G[0,5]( x )            # Space inside parentheses
```

## Valid Formula Examples

```
true
false
x
!x
x & y
x | y
NOT(x AND y)
G[0,5](x)
F[1,10](y & z)
G[0,5](x | y)
(x & y) | z
G[0,5](x & y) | F[1,3](z)
x U[1,5] (y & F[2,4](z))
G[0,5](NOT(x AND y))
```

## Invalid Syntax

```
g[0,5](x)              # ERROR: lowercase 'g' is treated as proposition, not Global operator
G[5,1](x)              # PARSES BUT FAILS: welldef() returns false (5 > 1)
G[0,5] x               # ERROR: missing parentheses around subformula
G[0,5]x                # ERROR: missing parentheses around subformula
x AND & y              # ERROR: double operator
```

## Well-defined Formulas

A formula is considered "well-defined" if:
1. All temporal bounds satisfy `lower_bound ≤ upper_bound`
2. All subformulas are well-defined (recursively)

Use `formula.welldef()` to check:
```rust
let formula = MLTL_WEST::parse("G[0,5](x)")?;
assert!(formula.welldef());  // true

let invalid = MLTL_WEST::parse("G[5,1](x)")?;
assert!(!invalid.welldef()); // false - bounds are inverted
```
