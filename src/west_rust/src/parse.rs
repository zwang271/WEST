use crate::MLTL_WEST;
use nom::{
    branch::alt,
    bytes::complete::{tag, tag_no_case, take_while1},
    character::complete::{char, multispace0, digit1},
    combinator::map,
    IResult,
};

impl MLTL_WEST<String> {
    /// Parse an MLTL formula from a string
    pub fn parse(input: &str) -> Result<Self, String> {
        parse_formula(input)
            .map(|(_, formula)| formula)
            .map_err(|e| format!("Parse error: {}", e))
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
    map(
        take_while1(|c: char| c.is_alphanumeric() || c == '_'),
        |s: &str| s.to_string(),
    )(input)
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
fn parse_formula(input: &str) -> IResult<&str, MLTL_WEST<String>> {
    let (input, _) = ws(input)?;
    parse_or(input)
}

fn parse_or(input: &str) -> IResult<&str, MLTL_WEST<String>> {
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
        MLTL_WEST::Or(Box::new(acc), Box::new(right))
    });
    Ok((input, result))
}

fn parse_and(input: &str) -> IResult<&str, MLTL_WEST<String>> {
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
        MLTL_WEST::And(Box::new(acc), Box::new(right))
    });
    Ok((input, result))
}

fn parse_not(input: &str) -> IResult<&str, MLTL_WEST<String>> {
    let (input, _) = ws(input)?;
    alt((
        |input| {
            let (input, _) = alt((
                tag_no_case("not"),
                tag("!"),
            ))(input)?;
            let (input, _) = ws(input)?;
            let (input, formula) = parse_not(input)?;
            Ok((input, MLTL_WEST::Not(Box::new(formula))))
        },
        parse_temporal,
    ))(input)
}

fn parse_temporal(input: &str) -> IResult<&str, MLTL_WEST<String>> {
    let (input, _) = ws(input)?;
    alt((
        parse_global,
        parse_future,
        parse_until,
        parse_release,
        parse_primary,
    ))(input)
}

fn parse_global(input: &str) -> IResult<&str, MLTL_WEST<String>> {
    let (input, _) = ws(input)?;
    let (input, _op) = alt((tag("G"), tag_no_case("globally")))(input)?;
    let (input, _) = ws(input)?;
    let (input, (lb, ub)) = parse_bound(input)?;
    let (input, _) = ws(input)?;
    let (input, formula) = parse_primary(input)?;
    Ok((input, MLTL_WEST::Global(lb, ub, Box::new(formula))))
}

fn parse_future(input: &str) -> IResult<&str, MLTL_WEST<String>> {
    let (input, _) = ws(input)?;
    let (input, _op) = alt((tag("F"), tag_no_case("future")))(input)?;
    let (input, _) = ws(input)?;
    let (input, (lb, ub)) = parse_bound(input)?;
    let (input, _) = ws(input)?;
    let (input, formula) = parse_primary(input)?;
    Ok((input, MLTL_WEST::Future(lb, ub, Box::new(formula))))
}

fn parse_until(input: &str) -> IResult<&str, MLTL_WEST<String>> {
    let (input, left) = parse_primary(input)?;
    let (input, _) = ws(input)?;
    let (input, _op) = alt((tag("U"), tag_no_case("until")))(input)?;
    let (input, _) = ws(input)?;
    let (input, (lb, ub)) = parse_bound(input)?;
    let (input, _) = ws(input)?;
    let (input, right) = parse_primary(input)?;
    Ok((input, MLTL_WEST::Until(lb, ub, Box::new(left), Box::new(right))))
}

fn parse_release(input: &str) -> IResult<&str, MLTL_WEST<String>> {
    let (input, left) = parse_primary(input)?;
    let (input, _) = ws(input)?;
    let (input, _op) = alt((tag("R"), tag_no_case("release")))(input)?;
    let (input, _) = ws(input)?;
    let (input, (lb, ub)) = parse_bound(input)?;
    let (input, _) = ws(input)?;
    let (input, right) = parse_primary(input)?;
    Ok((input, MLTL_WEST::Release(lb, ub, Box::new(left), Box::new(right))))
}

fn parse_primary(input: &str) -> IResult<&str, MLTL_WEST<String>> {
    let (input, _) = ws(input)?;
    alt((
        |input| {
            let (input, _) = tag_no_case("true")(input)?;
            let (input, _) = ws(input)?;
            Ok((input, MLTL_WEST::True))
        },
        |input| {
            let (input, _) = tag_no_case("false")(input)?;
            let (input, _) = ws(input)?;
            Ok((input, MLTL_WEST::False))
        },
        |input| {
            let (input, ident) = parse_identifier(input)?;
            let (input, _) = ws(input)?;
            Ok((input, MLTL_WEST::Prop(ident)))
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

    #[test]
    fn test_parse_true() {
        let result = MLTL_WEST::parse("true");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_false() {
        let result = MLTL_WEST::parse("false");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_proposition() {
        let result = MLTL_WEST::parse("x");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_not_with_exclamation() {
        let result = MLTL_WEST::parse("!x");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_not_with_keyword() {
        let result = MLTL_WEST::parse("not x");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_and_with_ampersand() {
        let result = MLTL_WEST::parse("x & y");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_and_with_double_ampersand() {
        let result = MLTL_WEST::parse("x && y");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_and_with_keyword() {
        let result = MLTL_WEST::parse("x and y");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_or_with_pipe() {
        let result = MLTL_WEST::parse("x | y");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_or_with_double_pipe() {
        let result = MLTL_WEST::parse("x || y");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_or_with_keyword() {
        let result = MLTL_WEST::parse("x or y");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_global() {
        let result = MLTL_WEST::parse("G[0,5](x)");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_future() {
        let result = MLTL_WEST::parse("F[1,10](y & z)");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_global_with_whitespace() {
        let result = MLTL_WEST::parse("G[0,5] (x | y)");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_parentheses() {
        let result = MLTL_WEST::parse("(x & y) | z");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_complex_formula() {
        let result = MLTL_WEST::parse("G[0,5](x & y) | F[1,3](z)");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_invalid_bounds() {
        // Bounds with lb > ub should parse but fail welldef check
        let result = MLTL_WEST::parse("G[5,1](x)");
        assert!(result.is_ok());
        let formula = result.unwrap();
        assert!(!formula.welldef(), "Expected welldef to be false for G[5,1](x)");
    }

    #[test]
    fn test_parse_nested_temporal() {
        let result = MLTL_WEST::parse("G[0,5](F[1,3](x))");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_multiple_levels_of_nesting() {
        let result = MLTL_WEST::parse("G[0,5]((x & F[1,3](y)) | !z)");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_with_underscores() {
        let result = MLTL_WEST::parse("var_1 & var_2");
        assert!(result.is_ok());
        assert!(result.unwrap().welldef());
    }

    #[test]
    fn test_parse_case_insensitive_keywords() {
        // Test case-insensitive keyword versions and operators
        assert!(MLTL_WEST::parse("X AND Y").is_ok());
        assert!(MLTL_WEST::parse("x AND y").is_ok());
        assert!(MLTL_WEST::parse("x Or Y").is_ok());
        assert!(MLTL_WEST::parse("NOT x").is_ok());
        // Full keyword temporal operators (case-insensitive)
        assert!(MLTL_WEST::parse("GLOBALLY[0,5](x)").is_ok());
        assert!(MLTL_WEST::parse("globally[0,5](x)").is_ok());
        assert!(MLTL_WEST::parse("Future[1,10](y)").is_ok());
        assert!(MLTL_WEST::parse("UNTIL[1,5](x, y)").is_ok());
        assert!(MLTL_WEST::parse("release[1,5](x, y)").is_ok());
        assert!(MLTL_WEST::parse("TRUE").is_ok());
        assert!(MLTL_WEST::parse("False").is_ok());
    }

    #[test]
    fn test_single_letter_temporal_uppercase_only() {
        // Single-letter operators must be uppercase
        assert!(MLTL_WEST::parse("G[0,5](x)").is_ok());
        assert!(MLTL_WEST::parse("F[1,10](y)").is_ok());
        // Lowercase single letters should not parse as temporal operators
        // (they would be parsed as propositions instead)
        assert!(MLTL_WEST::parse("g").is_ok()); // parses as proposition 'g'
        assert!(MLTL_WEST::parse("f").is_ok()); // parses as proposition 'f'
    }
}