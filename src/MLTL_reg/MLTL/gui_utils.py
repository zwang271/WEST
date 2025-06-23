import os
from PyQt5.QtWidgets import * 
from PyQt5.QtGui import * 
from random import choice
import platform

# Detect the operating system
os_name = platform.system()
CWD = os.getcwd()
FONT = QFont('Times', 15)


grammar_html = '''
        <html>
        <body>
        <p>The WEST program strips whitespaces from input.<br>
        Non-empty intervals are recommended for meaningful truth table generation. </p>

<p><b>Propositional Variables and Constants</b></p>
<p>True: <tt>true</tt><br>
False: <tt>false</tt><br>
First Variable: <tt>p0</tt><br>
Second Variable: <tt>p1</tt></p>
<p>And so on, where each consecutive variable is followed with the appropriate natural number.</p>

<p>Let K be a well-formed formula, propositional variable, or propositional constant. <br>
Formulas do not necessarily need to be in negation normal form, as the WEST program converts formulas into this form <br>
and generates the truth table for the formula's translated syntax. <br>
The user does not necessarily need to start their propositional variables at p0. <br>
That is, a user can input a formula that, for example, includes only the propositional variables p3, p4, and p7. <br>
For faster runtime and less memory usage, however, it is not recommended to skip natural numbers like this.</p>

<p><b>Unary Propositional Connectives</b><br>
The only unary propositional connective is negation.<br>
Negation does NOT use parentheses.<br>
Let K be a well-formed formula, propositional variable, or propositional constant.</p>
<p>Negation: <tt>~K</tt> </p>

<p><b>Unary Temporal Connectives</b><br>
All temporal operators must be followed by an interval.<br>
All intervals must be followed by a well-formed formula, propositional variable, or propositional constant. <br>
Unary temporal operators do NOT use parentheses.<br>
Let a be the inclusive lower bound of an interval, and let b be inclusive upper bound of an interval. <br>
Let "," separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.<br>
Let K be a well-formed formula, propositional variable, or propositional constant.</p>
<p>Finally: <tt>F[a,b] K</tt><br>
Globally: <tt>G[a,b] K</tt></p>


<p><b>Binary Propositional Connectives</b><br>
All binary connectives must be enclosed with parentheses.<br>
Let K, L be well-formed formulas, propositional variables, or propositional constants.</p>
<p>And: <tt>(K & L)</tt><br>
Or: <tt>(K | L)</tt><br>
Equivalence: <tt>(K = L)</tt><br>
Implies: <tt>(K -> L)</tt></p>


<p><b>Binary Temporal Connectives</b><br>
All binary connectives must be enclosed with parentheses.<br>
All temporal operators must be followed by an interval. <br>
All intervals must be followed by a well-formed formula, propositional variable, or propositional constant.<br>
Let a be the inclusive lower bound of an interval, and let b be inclusive upper bound of an interval. <br>
Let "," separate a and b, and "[" and "]" indicate the beginning and end of an interval, respectively.<br>
Let K, L be well-formed formulas, propositional variables, or propositional constants.</p>
<p>Until: <tt>(K U[a,b] L)</tt><br>
Release: <tt>(K R[a,b] L)</tt></p>


<p><b>Associative Propositional Connectives</b><br>
The entirety of the associative propositional connective formula string must be enclosed in parentheses.<br>
The list of elements must be preceded by the associative propositional connective.<br>
Let "," separate each element in the list, and let "[" and "]" indicate the beginning and end of the list, respectively.<br>
Let K, L, ..., M be an arbitrarily-sized list of well-formed formulas, propositional variables, or propositional constants.</p>
<p>And: <tt>(&[K, L, ..., M])</tt><br>
Or: <tt>(|[K, L, ..., M])</tt><br>
Equivalence: <tt>(=[K, L, ..., M])</tt><br>
Implies: <tt>(->[K, L, ..., M])</tt></p>

<p><b>A note on the associative equivalence operator:</b> for lists with 2 elements, the equivalence operator functions <br>
identically to the binary propositional connective equivalence operator. <br>
For formulas with 3 or more elements, the associative equivalence operator does not mean "each element in the list is equivalent".<br> 
Instead, it means that the equivalence of the first two elements in the list is equivalent to the next element in the list, <br>
and the truth value for this expression is equivalent to the next element, and so on. <br>
For example:</p>
<p><tt>(=[p0,p1,p2])</tt> is equivalent to <tt>((p0=p1)=p2)</tt><br>
<tt>(=[p0,p1,p2,p3...])</tt> is equivalent to <tt>(...(((p0=p1)=p2)=p3)...)</tt></p>
<p>But,</p>
<p><tt>(=[p0,p1,p2])</tt> is not equivalent to <tt>(p0=p1=p2)</tt></p>
<p>Note that <tt>(p0=p1=p2)</tt> is not a valid input. Therefore, if one wishes to generate the truth table for a formula<br>
that means "each element in the list is equivalent", then one could employ the transitivity of the equivalence operator<br>
with the <tt>and</tt> operator. <br>
For example:</p>
<p><tt>(p0=p1=p2=p3)</tt> can be inputted as <tt>(&[(p0=p1), (p1=p2), (p2=p3])</tt></p>
<br>
        </html>
        </body>'''


class Popup(QWidget):
    def __init__(self, title = "", path = None):
        super().__init__()
        self.setWindowTitle(title)
        layout = QHBoxLayout()
        lb = QLabel()
        pixmap = QPixmap(path)
        lb.resize(pixmap.width(), pixmap.height())
        lb.setPixmap(pixmap)
        layout.addWidget(lb)
        self.setLayout(layout)


def gen_reg(w_reg):
    if w_reg is None:
        return ""
    if w_reg is None:
        return ""
    output = ""
    for c in w_reg:
        if c == "s":
            output += choice(["0", "1"])
        else:
            output += c
    return output


def rewrite(formula):
    # define rewrite mapping
    REWRITE = {
        ":": ", ",
        ">": " -> ",
        "v": " v ",
        "&": " & ",
        "=": " = "
    }
    f = ""
    for i, char in enumerate(formula):
        if i in [0, len(formula)-1]:
            # pass
            f += char
        elif char in REWRITE.keys():
            f += REWRITE[char]
        else:
            f += char
    return f; 


def process_file():
    # read in formula from file
    formula = []
    with open("./west_output/formula.txt") as file:
        for line in file:
            line = line.rstrip()
            formula.append(rewrite(line))
    
    # construct regular expression from file
    regexp = []
    west_regexp = []
    with open("./west_output/regexp.txt") as file:
        temp_regexp = []
        temp_west_regexp = []
        for line in file:
            if line == "\n":
                regexp.append(temp_regexp)
                west_regexp.append(temp_west_regexp)
                temp_regexp = []
                temp_west_regexp = []
            else:
                temp = line.rstrip()
                temp_west_regexp.append(temp)
                temp = temp.replace("s", "[01]")
                temp_regexp.append(temp) 
    
    return formula, regexp, west_regexp


def run(func, argList):
    # use os.path.join to define execute = ".\gui\west_lib.exe " + func + " "

    # run appropriate command based on system
    if os_name == "Windows":
        execute = os.path.join(".", "gui", "west_lib.exe") + " " + func + " "
        for arg in argList:
            execute += '\"' + arg + '\" '
        os.system(execute)
    else:
        execute = os.path.join(".", "gui", "west_lib") + " " + func + " "
        for arg in argList:
            execute += '\"' + arg + '\" '
        os.system(execute)

    # print(execute)

    try:
        f = os.path.join(".", "gui", f"{func}.txt")
        return open(f, 'r')
    except FileNotFoundError:
        return []


if __name__ == '__main__':
    outfile1 = run("Wff_check", ["~G[0:2](p0 v p1)"])
    outfile2 = run("Wff_to_Nnf", ["~G[0:2](p0 v p1)"])
    outfile3 = run("get_n", ["F[0:2](~p0v~p1)"])
    outfile4 = run("reg", ["F[0:2](~p0v~p1)", "y", "n"])

    files = [outfile1, outfile2, outfile3, outfile4]
    for outfile in files:
        for line in outfile: 
            print(line)