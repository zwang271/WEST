use crate::MLTL;
use nom::{
    branch::alt,
    bytes::complete::{tag, tag_no_case, take_while1},
    character::complete::{char, multispace0, digit1},
    combinator::map,
    IResult,
};

impl MLTL<String> {
    /// Parse an MLTL formula from a string
    pub fn parse(input: &str) -> Result<Self, String> {
        match parse_formula(input) {
            Ok((remaining, formula)) => {
                // Check that all input was consumed (after stripping whitespace)
                let remaining = remaining.trim();
                if remaining.is_empty() {
                    Ok(formula)
                } else {
                    Err(format!("Parse error: unexpected trailing input: '{}'", remaining))
                }
            }
            Err(e) => Err(format!("Parse error: {}", e)),
        }
    }
}

// Lexer utilities
fn ws(input: &str) -> IResult<&str, &str> {
    multispace0(input)
}

fn lexeme<'a, F, O>(mut f: F) -> impl FnMut(&'a str) -> IResult<&'a str, O>
where
    F: FnMut(&'a str) -> IResult<&'a str, O>,
{
    move |input: &str| {
        let (input, o) = f(input)?;
        let (input, _) = ws(input)?;
        Ok((input, o))
    }
}

fn parse_identifier(input: &str) -> IResult<&str, String> {
    // Parse an identifier, but stop before U, R, G, F if followed by '['
    // This allows parsing "p0R[0,2]" as "p0" followed by "R[0,2]"
    let mut chars = input.char_indices().peekable();
    let mut end_idx = 0;
    
    while let Some((idx, c)) = chars.next() {
        if !(c.is_alphanumeric() || c == '_') {
            break;
        }
        
        // Check if this is a temporal operator (U, R, G, F) followed by '['
        if matches!(c, 'U' | 'R' | 'G' | 'F') {
            // Peek at the next character
            if let Some(&(_, next_c)) = chars.peek() {
                if next_c == '[' {
                    // Don't consume this operator - stop here
                    break;
                }
            }
        }
        
        end_idx = idx + c.len_utf8();
    }
    
    if end_idx == 0 {
        return Err(nom::Err::Error(nom::error::Error::new(input, nom::error::ErrorKind::TakeWhile1)));
    }
    
    Ok((&input[end_idx..], input[..end_idx].to_string()))
}

fn parse_number(input: &str) -> IResult<&str, usize> {
    map(digit1, |s: &str| s.parse::<usize>().unwrap())(input)
}

fn parse_bound(input: &str) -> IResult<&str, (usize, usize)> {
    let (input, _) = lexeme(char('['))(input)?;
    let (input, lb) = lexeme(parse_number)(input)?;
    let (input, _) = lexeme(char(','))(input)?;
    let (input, ub) = lexeme(parse_number)(input)?;
    let (input, _) = lexeme(char(']'))(input)?;
    Ok((input, (lb, ub)))
}

// Forward declarations for mutual recursion
fn parse_formula(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, _) = ws(input)?;
    parse_or(input)
}

fn parse_or(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, first) = parse_and(input)?;
    let (input, rest) = nom::multi::many0(|input| {
        let (input, _) = ws(input)?;
        let (input, _) = alt((
            tag_no_case("or"),
            tag("||"),
            tag("|"),
        ))(input)?;
        let (input, _) = ws(input)?;
        parse_and(input)
    })(input)?;

    let result = rest.into_iter().fold(first, |acc, right| {
        MLTL::Or(Box::new(acc), Box::new(right))
    });
    Ok((input, result))
}

fn parse_and(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, first) = parse_not(input)?;
    let (input, rest) = nom::multi::many0(|input| {
        let (input, _) = ws(input)?;
        let (input, _) = alt((
            tag_no_case("and"),
            tag("&&"),
            tag("&"),
        ))(input)?;
        let (input, _) = ws(input)?;
        parse_not(input)
    })(input)?;

    let result = rest.into_iter().fold(first, |acc, right| {
        MLTL::And(Box::new(acc), Box::new(right))
    });
    Ok((input, result))
}

fn parse_not(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, _) = ws(input)?;
    alt((
        |input| {
            let (input, _) = alt((
                tag_no_case("not"),
                tag("!"),
            ))(input)?;
            let (input, _) = ws(input)?;
            let (input, formula) = parse_not(input)?;
            Ok((input, MLTL::Not(Box::new(formula))))
        },
        parse_temporal,
    ))(input)
}

fn parse_temporal(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, _) = ws(input)?;
    alt((
        parse_global,
        parse_future,
        parse_until,
        parse_release,
        parse_primary,
    ))(input)
}

fn parse_global(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, _) = ws(input)?;
    let (input, _op) = alt((tag_no_case("global"), tag("G")))(input)?;
    let (input, _) = ws(input)?;
    let (input, (lb, ub)) = parse_bound(input)?;
    let (input, _) = ws(input)?;
    // Allow nested unary operators (NOT, temporal) or primary expressions
    let (input, formula) = parse_not(input)?;
    Ok((input, MLTL::Global(lb, ub, Box::new(formula))))
}

fn parse_future(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, _) = ws(input)?;
    let (input, _op) = alt((tag_no_case("future"), tag("F")))(input)?;
    let (input, _) = ws(input)?;
    let (input, (lb, ub)) = parse_bound(input)?;
    let (input, _) = ws(input)?;
    // Allow nested unary operators (NOT, temporal) or primary expressions
    let (input, formula) = parse_not(input)?;
    Ok((input, MLTL::Future(lb, ub, Box::new(formula))))
}

fn parse_until(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, left) = parse_primary(input)?;
    let (input, _) = ws(input)?;
    let (input, _op) = alt((tag_no_case("until"), tag("U")))(input)?;
    let (input, _) = ws(input)?;
    let (input, (lb, ub)) = parse_bound(input)?;
    let (input, _) = ws(input)?;
    // Right operand can be a full temporal expression (including G, F, etc.)
    let (input, right) = parse_not(input)?;
    Ok((input, MLTL::Until(lb, ub, Box::new(left), Box::new(right))))
}

fn parse_release(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, left) = parse_primary(input)?;
    let (input, _) = ws(input)?;
    let (input, _op) = alt((tag_no_case("release"), tag("R")))(input)?;
    let (input, _) = ws(input)?;
    let (input, (lb, ub)) = parse_bound(input)?;
    let (input, _) = ws(input)?;
    // Right operand can be a full temporal expression (including G, F, etc.)
    let (input, right) = parse_not(input)?;
    Ok((input, MLTL::Release(lb, ub, Box::new(left), Box::new(right))))
}

fn parse_primary(input: &str) -> IResult<&str, MLTL<String>> {
    let (input, _) = ws(input)?;
    alt((
        |input| {
            let (input, _) = tag_no_case("true")(input)?;
            let (input, _) = ws(input)?;
            Ok((input, MLTL::True))
        },
        |input| {
            let (input, _) = tag_no_case("false")(input)?;
            let (input, _) = ws(input)?;
            Ok((input, MLTL::False))
        },
        |input| {
            let (input, ident) = parse_identifier(input)?;
            let (input, _) = ws(input)?;
            Ok((input, MLTL::Prop(ident)))
        },
        |input| {
            let (input, _) = char('(')(input)?;
            let (input, _) = ws(input)?;
            let (input, formula) = parse_formula(input)?;
            let (input, _) = ws(input)?;
            let (input, _) = char(')')(input)?;
            let (input, _) = ws(input)?;
            Ok((input, formula))
        },
    ))(input)
}



#[cfg(test)]
mod tests {
    use super::*;
    use std::io::Write;

    /// Helper that returns the parsed formula for explicit structure checking
    fn parse_and_check(formula_str: &str) -> MLTL<String> {
        println!("\n  Formula: {}", formula_str);
        let _ = std::io::stdout().flush();
        match MLTL::parse(formula_str) {
            Ok(formula) => {
                assert!(formula.welldef(), "Formula should be well-defined: {}", formula_str);
                formula.print_parsetree();
                let _ = std::io::stdout().flush();
                println!("  ✓ Well-defined");
                let _ = std::io::stdout().flush();
                formula
            }
            Err(e) => {
                panic!("Failed to parse '{}': {}", formula_str, e);
            }
        }
    }

    #[test]
    fn test_parse_true() {
        let formula = parse_and_check("true");
        assert_eq!(formula, MLTL::True);
        
        let formula = parse_and_check("TRUE");
        assert_eq!(formula, MLTL::True);
    }

    #[test]
    fn test_parse_false() {
        let formula = parse_and_check("false");
        assert_eq!(formula, MLTL::False);
        
        let formula = parse_and_check("FALSE");
        assert_eq!(formula, MLTL::False);
    }

    #[test]
    fn test_parse_proposition() {
        let formula = parse_and_check("x");
        assert_eq!(formula, MLTL::Prop("x".to_string()));
    }

    #[test]
    fn test_parse_not_with_exclamation() {
        let formula = parse_and_check("!x");
        assert_eq!(formula, MLTL::Not(Box::new(MLTL::Prop("x".to_string()))));
    }

    #[test]
    fn test_parse_not_with_keyword() {
        let formula = parse_and_check("not x");
        assert_eq!(formula, MLTL::Not(Box::new(MLTL::Prop("x".to_string()))));
    }

    #[test]
    fn test_parse_and_with_ampersand() {
        let formula = parse_and_check("x & y");
        assert_eq!(formula, MLTL::And(
            Box::new(MLTL::Prop("x".to_string())),
            Box::new(MLTL::Prop("y".to_string()))
        ));
    }

    #[test]
    fn test_parse_and_with_double_ampersand() {
        let formula = parse_and_check("x && y");
        assert_eq!(formula, MLTL::And(
            Box::new(MLTL::Prop("x".to_string())),
            Box::new(MLTL::Prop("y".to_string()))
        ));
    }

    #[test]
    fn test_parse_and_with_keyword() {
        let formula = parse_and_check("x and y");
        assert_eq!(formula, MLTL::And(
            Box::new(MLTL::Prop("x".to_string())),
            Box::new(MLTL::Prop("y".to_string()))
        ));
    }

    #[test]
    fn test_parse_or_with_pipe() {
        let formula = parse_and_check("x | y");
        assert_eq!(formula, MLTL::Or(
            Box::new(MLTL::Prop("x".to_string())),
            Box::new(MLTL::Prop("y".to_string()))
        ));
    }

    #[test]
    fn test_parse_or_with_double_pipe() {
        let formula = parse_and_check("x || y");
        assert_eq!(formula, MLTL::Or(
            Box::new(MLTL::Prop("x".to_string())),
            Box::new(MLTL::Prop("y".to_string()))
        ));
    }

    #[test]
    fn test_parse_or_with_keyword() {
        let formula = parse_and_check("x or y");
        assert_eq!(formula, MLTL::Or(
            Box::new(MLTL::Prop("x".to_string())),
            Box::new(MLTL::Prop("y".to_string()))
        ));
    }

    #[test]
    fn test_parse_global() {
        let formula = parse_and_check("G[0,5](x)");
        assert_eq!(formula, MLTL::Global(0, 5, Box::new(MLTL::Prop("x".to_string()))));
    }

    #[test]
    fn test_parse_future() {
        let formula = parse_and_check("F[1,10](y & z)");
        assert_eq!(formula, MLTL::Future(1, 10, Box::new(MLTL::And(
            Box::new(MLTL::Prop("y".to_string())),
            Box::new(MLTL::Prop("z".to_string()))
        ))));
    }

    #[test]
    fn test_parse_global_with_whitespace() {
        let formula = parse_and_check("G[0,5] (x | y)");
        assert_eq!(formula, MLTL::Global(0, 5, Box::new(MLTL::Or(
            Box::new(MLTL::Prop("x".to_string())),
            Box::new(MLTL::Prop("y".to_string()))
        ))));
    }

    #[test]
    fn test_parse_parentheses() {
        let formula = parse_and_check("(x & y) | z");
        assert_eq!(formula, MLTL::Or(
            Box::new(MLTL::And(
                Box::new(MLTL::Prop("x".to_string())),
                Box::new(MLTL::Prop("y".to_string()))
            )),
            Box::new(MLTL::Prop("z".to_string()))
        ));
    }

    #[test]
    fn test_parse_complex_formula() {
        let formula = parse_and_check("G[0,5](x & y) | F[1,3](z)");
        assert_eq!(formula, MLTL::Or(
            Box::new(MLTL::Global(0, 5, Box::new(MLTL::And(
                Box::new(MLTL::Prop("x".to_string())),
                Box::new(MLTL::Prop("y".to_string()))
            )))),
            Box::new(MLTL::Future(1, 3, Box::new(MLTL::Prop("z".to_string()))))
        ));
    }

    #[test]
    fn test_parse_invalid_bounds() {
        println!("\n  Formula: G[5,1](x)");
        // Bounds with lb > ub should parse but fail welldef check
        let result = MLTL::parse("G[5,1](x)");
        assert!(result.is_ok());
        let formula = result.unwrap();
        formula.print_parsetree();
        assert!(!formula.welldef(), "Expected welldef to be false for G[5,1](x)");
        println!("  ✓ Correctly identified as not well-defined");
    }

    #[test]
    fn test_parse_nested_temporal() {
        let formula = parse_and_check("G[0,5](F[1,3](x))");
        assert_eq!(formula, MLTL::Global(0, 5, 
            Box::new(MLTL::Future(1, 3, 
                Box::new(MLTL::Prop("x".to_string()))))));
    }

    #[test]
    fn test_parse_multiple_levels_of_nesting() {
        let formula = parse_and_check("G[0,5]((x & F[1,3](y)) | !z)");
        assert_eq!(formula, MLTL::Global(0, 5, Box::new(MLTL::Or(
            Box::new(MLTL::And(
                Box::new(MLTL::Prop("x".to_string())),
                Box::new(MLTL::Future(1, 3, Box::new(MLTL::Prop("y".to_string()))))
            )),
            Box::new(MLTL::Not(
                Box::new(MLTL::Prop("z".to_string()))
            ))
        ))));
    }

    #[test]
    fn test_parse_with_underscores() {
        let formula = parse_and_check("p0 & p1");
        assert_eq!(formula, MLTL::And(
            Box::new(MLTL::Prop("p0".to_string())),
            Box::new(MLTL::Prop("p1".to_string()))
        ));
    }

    #[test]
    fn test_case_insensitive_keywords() {
        // Test uppercase keyword variant
        let formula = parse_and_check("GLOBAL[0,5](x)");
        assert_eq!(formula, MLTL::Global(0, 5, Box::new(MLTL::Prop("x".to_string()))));
        
        // Test mixed case keyword
        let formula = parse_and_check("Future[1,10](y)");
        assert_eq!(formula, MLTL::Future(1, 10, Box::new(MLTL::Prop("y".to_string()))));
        
        // Test uppercase boolean operators
        let formula = parse_and_check("X AND Y");
        assert_eq!(formula, MLTL::And(
            Box::new(MLTL::Prop("X".to_string())),
            Box::new(MLTL::Prop("Y".to_string()))
        ));
    }

    #[test]
    fn test_lowercase_single_letters_as_propositions() {
        // Lowercase 'g' should parse as proposition, not temporal operator
        let formula = parse_and_check("g");
        assert_eq!(formula, MLTL::Prop("g".to_string()));
        
        // Lowercase 'f' should parse as proposition, not temporal operator
        let formula = parse_and_check("f");
        assert_eq!(formula, MLTL::Prop("f".to_string()));
    }
}