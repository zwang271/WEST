use std::cmp::max;

#[allow(non_camel_case_types)]
pub enum MLTL_WEST<T> {
    True,
    False,
    Prop(T),
    Not(Box<MLTL_WEST<T>>),
    And(Box<MLTL_WEST<T>>, Box<MLTL_WEST<T>>),
    Or(Box<MLTL_WEST<T>>, Box<MLTL_WEST<T>>),
    Global(usize, usize, Box<MLTL_WEST<T>>),
    Future(usize, usize, Box<MLTL_WEST<T>>),
    Until(usize, usize, Box<MLTL_WEST<T>>, Box<MLTL_WEST<T>>),
    Release(usize, usize, Box<MLTL_WEST<T>>, Box<MLTL_WEST<T>>),
}

impl<T: Ord> MLTL_WEST<T> {
    pub fn welldef(&self) -> bool {
        match self {
            MLTL_WEST::True | MLTL_WEST::False | MLTL_WEST::Prop(_) => true,
            MLTL_WEST::Not(sub) => sub.welldef(),
            MLTL_WEST::And(left, right) 
            | MLTL_WEST::Or(left, right) => {
                left.welldef() && right.welldef()
            }
            MLTL_WEST::Global(_lb, ub, sub) 
            | MLTL_WEST::Future(_lb, ub, sub) => {
                _lb <= ub && sub.welldef()
            }
            MLTL_WEST::Until(_lb, ub, left, right)
            | MLTL_WEST::Release(_lb, ub, left, right) => {
                _lb <= ub && left.welldef() && right.welldef()
            }
        }
    }

    pub fn complen(&self) -> usize {
        match self {
            MLTL_WEST::True | MLTL_WEST::False | MLTL_WEST::Prop(_) => 1,
            MLTL_WEST::Not(sub) => sub.complen(),
            MLTL_WEST::And(left, right) 
            | MLTL_WEST::Or(left, right) => {
                max(left.complen(), right.complen())
            }
            MLTL_WEST::Global(_lb, ub, sub) 
            | MLTL_WEST::Future(_lb, ub, sub) => {
                ub + sub.complen()
            }
            MLTL_WEST::Until(_lb, ub, left, right)
            | MLTL_WEST::Release(_lb, ub, left, right) => {
                ub + max(left.complen()-1, right.complen())
            }
        }
    }
}

impl<T: Ord + std::fmt::Display> MLTL_WEST<T> {
    /// Print the parse tree in a nicely formatted hierarchical structure
    pub fn print_parsetree(&self) {
        self.print_parsetree_indent(0);
    }

    fn print_parsetree_indent(&self, indent: usize) {
        let prefix = "  ".repeat(indent);
        match self {
            MLTL_WEST::True => println!("{}├─ True", prefix),
            MLTL_WEST::False => println!("{}├─ False", prefix),
            MLTL_WEST::Prop(p) => println!("{}├─ Prop({})", prefix, p),
            MLTL_WEST::Not(sub) => {
                println!("{}├─ Not", prefix);
                sub.print_parsetree_indent(indent + 1);
            }
            MLTL_WEST::And(left, right) => {
                println!("{}├─ And", prefix);
                println!("{}│  Left:", prefix);
                left.print_parsetree_indent(indent + 2);
                println!("{}│  Right:", prefix);
                right.print_parsetree_indent(indent + 2);
            }
            MLTL_WEST::Or(left, right) => {
                println!("{}├─ Or", prefix);
                println!("{}│  Left:", prefix);
                left.print_parsetree_indent(indent + 2);
                println!("{}│  Right:", prefix);
                right.print_parsetree_indent(indent + 2);
            }
            MLTL_WEST::Global(lb, ub, sub) => {
                println!("{}├─ Global[{}, {}]", prefix, lb, ub);
                sub.print_parsetree_indent(indent + 1);
            }
            MLTL_WEST::Future(lb, ub, sub) => {
                println!("{}├─ Future[{}, {}]", prefix, lb, ub);
                sub.print_parsetree_indent(indent + 1);
            }
            MLTL_WEST::Until(lb, ub, left, right) => {
                println!("{}├─ Until[{}, {}]", prefix, lb, ub);
                println!("{}│  Left:", prefix);
                left.print_parsetree_indent(indent + 2);
                println!("{}│  Right:", prefix);
                right.print_parsetree_indent(indent + 2);
            }
            MLTL_WEST::Release(lb, ub, left, right) => {
                println!("{}├─ Release[{}, {}]", prefix, lb, ub);
                println!("{}│  Left:", prefix);
                left.print_parsetree_indent(indent + 2);
                println!("{}│  Right:", prefix);
                right.print_parsetree_indent(indent + 2);
            }
        }
    }
}
