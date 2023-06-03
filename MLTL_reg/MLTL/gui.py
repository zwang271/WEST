# Author: Zili Wang
# Last updated: 4/12/23
# A user interface for the WEST program written in PyQT5

import sys
import os
from PyQt5.QtWidgets import * 
from PyQt5.QtGui import * 
from PyQt5.QtCore import Qt
import re
from random import choice, randint
from time import sleep
import math
from math import log
from gui_utils import * 
from dd import autoref as _bdd
import parser
from lark import Lark, tree


class FormulaWindow(QWidget):
    def __init__(self, formula, regexp, west_regexp, n):
        super().__init__()
        
        # Process inputs
        self.formula = formula
        self.t = 1
        self.t = 1
        self.n = n
        self.regexp, self.west_regexp = regexp, west_regexp
        self.complement_list = None
        if len(regexp) > 0:
            self.t = max(regexp[i].count(",") + 1 for i in range(len(regexp)))
        self.var = list(set(re.findall("p[0-9]*", self.formula)))
        self.var.sort()
        # debug info
        # print(self.formula)
        # print(self.var)
        # print(self.west_regexp)
        # print()


        # Define main layout
        self.setWindowTitle("WEST")
        main_layout = QVBoxLayout() # overall interface


        # Display current formula
        self.formula_label = QLabel("MLTL Formula: " + formula)
        self.formula_label.setFont(FONT)
        main_layout.addWidget(self.formula_label)


        # Display current computation and option to generate random computation
        computation_layout = QHBoxLayout()
        self.computation_label = QLabel()
        self.computation_label.setFont(FONT)
        self.computation_label.setToolTip("Computation")
        computation_layout.addWidget(self.computation_label)
        self.computation_help_button = QPushButton("Help")
        self.computation_help_button.setFont(FONT)
        self.computation_help_button.setToolTip("Click for explanation about string representation of computations.")
        self.computation_help_button.clicked.connect(self.show_computation_help)
        computation_layout.addWidget(self.computation_help_button)
        main_layout.addLayout(computation_layout)


        # Button to generate random sat and unsat comp
        rand_button_layout = QHBoxLayout()
        self.rand_computation_button = QPushButton("Rand SAT")
        self.rand_computation_button.setToolTip("Randomly generates a satisfying computaton to the MLTL formula.")
        self.rand_computation_button.setFont(FONT)
        self.rand_computation_button.clicked.connect(lambda state, x=None: self.rand_comp(x))
        self.rand_unsat_button = QPushButton("Rand UNSAT")
        self.rand_unsat_button.setToolTip("Randomly generates a computation that does NOT satisfy the MLTL formula.")
        self.rand_unsat_button.clicked.connect(self.rand_unsat)
        self.rand_unsat_button.setFont(FONT)
        rand_button_layout.addWidget(self.rand_computation_button)
        rand_button_layout.addWidget(self.rand_unsat_button)
        main_layout.addLayout(rand_button_layout)


        # Configure variable toggling layout
        var_layout = QGridLayout()
        reset_button = QPushButton("reset")
        reset_button.setFont(FONT)
        reset_button.clicked.connect(self.reset_toggles)
        var_layout.addWidget(reset_button)
        # Display variable names
        for i in range(self.n):
            widget = QLabel(f'p{i}')
            widget.setFont(FONT)
            if f'p{i}' not in self.var:
                widget.setEnabled(False)
            var_layout.addWidget(widget, i+1, 0)
        # Display time steps
        for i in range(self.t):
            widget = QLabel(str(i))
            widget.setFont(FONT)
            var_layout.addWidget(widget, 0, i+1)
        # Display each variable at each timestep
        self.variable_toggle = {}
        for var in range(self.n):
            self.variable_toggle[var] = {}
            for time in range(self.t):
                widget = QCheckBox()
                widget.toggled.connect(self.update_computation)
                if f'p{var}' not in self.var:
                    widget.setEnabled(False)
                self.variable_toggle[var][time] = widget
                var_layout.addWidget(self.variable_toggle[var][time], var+1, time+1)
        main_layout.addLayout(var_layout)



        # Creating tabs for multiple display options
        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab2 = QWidget()
        self.tabs.addTab(self.tab1, "Regexp List")
        self.tabs.addTab(self.tab2, "Backbone Analysis")
        self.tabs.setStyleSheet('QTabBar { font-size: 15pt; font-family: Times; }')
        main_layout.addWidget(self.tabs)  

        # Building scrollable layout to display regexps
        regexp_layout = QGridLayout()
        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        regexp_layout.addWidget(scroll_area)
        scroll_content = QWidget()
        scroll_layout = QGridLayout(scroll_content)
        self.reg_labels = []
        for i, reg in enumerate(self.west_regexp):
            label = QPushButton(reg)
            label.setFont(FONT)
            label.setToolTip("Click to generate satisfying computation that matches this particular regular expression.")
            self.reg_labels.append(label)
            label.clicked.connect(lambda state, x=reg: self.rand_comp(x))
            scroll_layout.addWidget(label, i, 0)
        scroll_area.setWidget(scroll_content)
        self.tab1.setLayout(regexp_layout)


        # Building area for looking at backbone analysis
        self.compute_bdd()
        bb_layout = QVBoxLayout()
        # Creating display messages for sat and unsat backbones
        self.compute_backbones()
        sat_backbone, unsat_backbone = "Backbone for SAT Assignments:\n", "Backbone for UNSAT Assignments:\n"
        for time in range(self.t):
            sat_msg = f"t = {time}: "
            unsat_msg = f"t = {time}: "
            for var in range(self.n):
                i = time * self.n + var
                # check if sat backbone
                if self.var_status[i].value in ["0", "1"]:
                    lit = f"p{var}" if self.var_status[i].value == "1" else f"~p{var}"
                    sat_msg += lit + ", "
                # check if unsat backbone
                if self.complement_var_status[i].value in ["0", "1"]:
                    lit = f"p{var}" if self.complement_var_status[i].value == "1" else f"~p{var}"
                    unsat_msg += lit + ", "
            sat_backbone += sat_msg + "\n" if sat_msg[-2]!="," else sat_msg[:-2] + "\n"
            unsat_backbone += unsat_msg + "\n" if unsat_msg[-2]!="," else unsat_msg[:-2] + "\n"
        # Create QLabels to display unsat and sat backbones 
        self.bb_sat_label = QLabel(sat_backbone)
        self.bb_sat_label.setFont(FONT)
        bb_layout.addWidget(self.bb_sat_label)
        self.bb_unsat_label = QLabel(unsat_backbone)
        self.bb_unsat_label.setFont(FONT)
        bb_layout.addWidget(self.bb_unsat_label)
        self.tab2.setLayout(bb_layout)   


        # Update initial computation of all zeros
        self.update_computation()
        # Define main widget of the gui
        self.setLayout(main_layout)
    
    def update_computation(self): 
        comp = ""
        for time in range(self.t):
            for var in range(self.n):
                if self.variable_toggle[var][time].isChecked():
                    comp += "1"
                else:
                    comp += "0"
            comp += ","
        comp = comp[:-1]
        self.computation_label.setText("Computation: " + comp)

        found = False
        for i, reg in enumerate(self.regexp):
            if re.match(reg, comp):
                found = True
                self.formula_label.setStyleSheet("background-color: lightgreen")
                self.formula_label.setToolTip("SATISFIED")
                self.reg_labels[i].setStyleSheet("background-color: lightgreen")
            else:
                self.reg_labels[i].setStyleSheet("background-color: none")
                self.formula_label.setToolTip("NOT SATISFIED")
        if not found:
            self.formula_label.setStyleSheet("background-color: pink")
    

    def rand_comp(self, regexp = None):
        if regexp is None and len(self.west_regexp) > 0: 
            if self.west_regexp == []:
                return
            comp = gen_reg(choice(self.west_regexp))
        else:
            comp = gen_reg(regexp)

        var, time = 0, 0 # keep track of what variable and time
        for c in comp:
            if c == "0" and f'p{var}' in self.var:
                self.variable_toggle[var][time].setChecked(False)
            elif c == "1" and f'p{var}' in self.var: 
                # var = self.var[var_i]
                self.variable_toggle[var][time].setChecked(True)
            elif c == ",":
                time += 1
                var = 0
                continue
            var += 1
    

    def compute_bdd(self):
        # Create a BDD manager
        bdd = _bdd.BDD()

        # Create constants true and false
        const = {"1": "True", "0": "False"}

        # Create boolean vars x_{i} for i in range(self.t * self.n)
        x_names = [f"x{i}" for i in range(self.t * self.n)]
        [bdd.declare(x_i) for x_i in x_names]
        x = [bdd.var(x_i) for x_i in x_names]

        complement = bdd.add_expr("True")
        for w in self.west_regexp:
            expr = "("
            w = w.replace(",", "")
            for i, char in enumerate(w):
                if char != "s":
                    expr += f"( x{i} <-> ~ {const[char]} ) | "
            expr = expr[:-3] + ")" if expr != "(" else "False"
            clause = bdd.add_expr(expr)

            complement = clause if complement is None else (complement) & clause

        self.complement_list = list(bdd.pick_iter(complement))
        self.model_list = list(bdd.pick_iter(~complement))
    

    def compute_backbones(self):
        # dataclass to compute status of each variable when iterating over all models
        class Status():
            def __init__(self):
                self.seen = False
                self.value = None # 0, 1, or c (contradiction)
            # Return false if value updated to contradiction, else return True
            def update(self, value):
                v = "1" if value else "0"
                if not self.seen:
                    self.seen = True
                    self.value = v
                else:
                    if self.value != v: 
                        self.value = "c"
                        return False
                return True

        self.var_status = [Status() for i in range(self.t * self.n)]
        for i in range(self.t * self.n):
            for model in self.model_list:
                if f"x{i}" in model.keys():
                    self.var_status[i].update(model[f"x{i}"])
                    if self.var_status[i].value == "c":
                        break

        self.complement_var_status = [Status() for i in range(self.t * self.n)]
        for i in range(self.t * self.n):
            for model in self.complement_list:
                if f"x{i}" in model.keys():
                    self.complement_var_status[i].update(model[f"x{i}"])
                    if self.complement_var_status[i].value == "c":
                        break
        
        # [(print(i, s.value)) for i, s in enumerate(self.var_status)]
        # [(print(i, s.value)) for i, s in enumerate(self.complement_var_status)]


    def show_computation_help(self):
        self.comp_help = QWidget()
        self.comp_help.setWindowTitle("String Representation of Computations")
        label = QTextEdit(self.comp_help)
        label.setReadOnly(True)
        label.setHtml('''<html> <body>
<p><b>Computations</b></p>
<p>In a computation, "1" represents a true truth value, and "0" represents a false truth value.<br>
"S" represents an arbitrary truth value, i.e. true or false.<br>
Time steps in a computation are separated by commas. <br>
The bit-strings at each time step represent the truth values of each propositional variable, in ascending order.</p>

<p><b>Examples</b></p>
The computation of this formula with one propositional variable and 5 time steps is represented as:</p>
<p><tt>NNF Formula: G[0,4]p0<br>

1,1,1,1,1</tt></p>

<p>The computation of a formula with 5 propositional variables and one time step is represented as:</p>
<p><tt>NNF Formula: G[0,0](&[p0,p1,p2,p3,p4])<br>

11111</tt></p>
<p>The computations of a formula with two propositional variables that are both eventually true <br>
at a third time step are represented as:</p>
<p><tt>NNF Formula: F[0,2](p0&p1)<br>

11,ss,ss<br>
ss,11,ss<br>
ss,ss,11</tt></p>
<p>Where in each string separated by commas, the first digit represents the truth value of p0, <br>
and the second digit represents the truth value of p1.</p>
<br>
</html></body>
''')

        font = label.document().defaultFont()  # or another font if you change it
        fontMetrics = QFontMetrics(font)  # a QFontMetrics based on our font
        textSize = fontMetrics.size(0, label.toPlainText())

        textWidth = textSize.width() + 20  # constant may need to be tweaked
        textHeight = textSize.height() + 30  # constant may need to be tweaked

        label.resize(textWidth, textHeight)
        self.comp_help.setMaximumSize(textWidth, textHeight)
        self.comp_help.show()


    #G[0:2](p0 v p1)
    def rand_unsat(self):
        if self.complement_list == []:
            return
        complement = choice(self.complement_list)

        for i in range(self.t * self.n):
            var, time = i % self.n, i // self.n
            if f"x{i}" in complement.keys():
                b = complement[f"x{i}"]
            else:
                b = True if randint(0, 1) == 1 else False
            self.variable_toggle[var][time].setChecked(b)


    def reset_toggles(self):
        for time in range(self.t):
            for var in range(self.n):
                self.variable_toggle[var][time].setChecked(False)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("WEST MLTL Formula Validation Tool")
        main_layout = QVBoxLayout()
        self.resize(800, 400)

        # Create field for user input
        input_layout = QHBoxLayout()
        self.input_line = QLineEdit()
        self.input_line.setMaxLength(100)
        self.input_line.setPlaceholderText("Type in a MLTL formula and press enter.")
        self.input_line.setFont(FONT)
        self.input_line.returnPressed.connect(self.validate_formula)
        if len(sys.argv) == 2:
            self.input_line.setText(sys.argv[1])
        input_layout.addWidget(self.input_line)
        # Button to run input
        self.run_button = QPushButton("Run")
        self.run_button.setFont(FONT)
        self.run_button.pressed.connect(self.validate_formula)
        input_layout.addWidget(self.run_button)
        # Help button to view full grammar
        self.cfg_button = QPushButton("Grammar")
        self.cfg_button.setFont(FONT)
        self.cfg_button.setToolTip("Click to view full context free grammar of input.")
        self.cfg_button.clicked.connect(self.show_cfg)
        input_layout.addWidget(self.cfg_button)
        main_layout.addLayout(input_layout)

        # Create checkboxes for toggling simp and rest
        toggle_layout = QHBoxLayout()
        self.simp_box = QCheckBox("Simplify")
        self.simp_box.setChecked(True)
        self.rest_box = QCheckBox("Apply REST")
        self.simp_box.setFont(FONT)
        self.rest_box.setFont(FONT)
        toggle_layout.addWidget(self.simp_box)
        toggle_layout.addWidget(self.rest_box)
        main_layout.addLayout(toggle_layout)

        # Create field for output messages
        self.out_text = QLabel("")
        self.out_text.setFont(FONT)
        main_layout.addWidget(self.out_text)

        # Create a layout to display all subformulas 
        self.subformula_button = []
        self.subformula_layout = QVBoxLayout()
        main_layout.addLayout(self.subformula_layout)

        widget = QWidget()
        widget.setLayout(main_layout)
        self.setCentralWidget(widget)
        self.resize(self.sizeHint()) # resize window
        self.resize(self.sizeHint()) # resize window


    def validate_formula(self):
        self.input_line.adjustSize()
        self.out_text.adjustSize()
        self.resize(self.sizeHint()) # resize window

        formula = self.input_line.text()
        if formula == "":
            return

        is_wff, self.tree, e = parser.check_wff(formula)
        if (not is_wff):
            self.out_text.setText(f"\"{formula}\" is not a valid formula!\n{e}")
            self.input_line.adjustSize()
            self.out_text.adjustSize()
            self.resize(self.sizeHint()) # resize window
            return

        formula = parser.to_west(formula, self.tree)

        outfile = run("Wff_check", [formula])
        valid = bool(int(outfile.readline()))
        if (not valid): # should never get here if grammar conversion is done correctly
            self.out_text.setText(f"\"{formula}\" is not a valid formula! (uh oh something went wrong)")
            self.out_text.adjustSize()
            self.resize(self.sizeHint()) # resize window
            self.show()
            return
    

        # convert to nnf form
        nnf = run("Wff_to_Nnf", [formula]).readline()
        nnf_message = ""
        if nnf != formula.replace(" ", ""):
            nnf_message = f"Converted to Negation Normal Form: {parser.from_west(nnf)}\n"


        # get number of propositional variables
        self.n = int(run("get_n", [nnf]).readline())


        # get simp and rest preferences and display warnings if appropriate
        simp = "y" if self.simp_box.isChecked() else "n"
        rest = "y" if self.rest_box.isChecked() else "n"
        # print(f"simp:{simp} rest:{rest}")
        simp_warning = "WARNING: not simplifying can lead to extremely long outputs!\n"
        simp_message =  simp_warning if not self.simp_box.isChecked() else ""
        rest_warning = "WARNING: using REST can be very computationally expensive!\n"
        rest_message = rest_warning if self.rest_box.isChecked() else ""
        

        # Compute complexity of formula
        self.complexity = self.compute_complexity(nnf)

        # Display appropriate messages to user
        self.out_text.setText(simp_message + 
                              rest_message +
                              nnf_message + 
                              "\nPlease select a subformula to explore below:")
        

        # If complexity is above a certain bound, do NOT run reg
        bound = math.inf
        if self.complexity > bound:
            return


        # call reg on input formula 
        run("reg", [nnf, simp, rest])


        # process output files from reg
        self.formula, self.regexp, self.west_regexp = process_file()
        # print(self.formula)
        # print(self.regexp)
        # print(self.west_regexp)

        # first delete any previously displayed subformula buttons
        for i in reversed(range(self.subformula_layout.count())): 
            self.subformula_layout.itemAt(i).widget().setParent(None)


        # display buttons for subformulas
        self.formula = [parser.from_west(f) for f in self.formula]
        for i,f in enumerate(self.formula): 

            # skip all atomic propositions and their negations
            if re.match("~?p[0-9]*", f):
                continue

            # only display subformulas that are substrings of the main formula
            if (f.replace(" ", "") not in self.formula[-1].replace(" ", "")) and\
                (f.replace(" ", "")[1:-1] not in self.formula[-1].replace(" ", "")):
                # print(f.replace(" ", ""))
                continue
                
            formula_button = QPushButton(f.replace("&", "&&"))
            formula_button.setToolTip("Explore formula in a separate window")
            formula_button.setFont(FONT)
            formula_button.clicked.connect(
                lambda state, 
                formula = f, 
                regexp = self.regexp[i], 
                west_regexp = self.west_regexp[i],
                n = self.n:
                self.show_subformula(formula, regexp, west_regexp, n))
            self.subformula_button.append(formula_button)
            self.subformula_layout.addWidget(formula_button)
        return 

        return 



    def compute_complexity(self, formula):
        def dfs(node, depth=0):
            # print("  " * depth, "Node:", node.data)
            children_connectives = [dfs(child, depth + 1) for child in node.children]
            if node.data in ["unary_temp_conn", "binary_temp_conn"]:
                return 1 + sum(children_connectives)
            else:
                return sum(children_connectives)

        n = self.n
        d, delta, l = 0, 0, dfs(self.tree)

        a, b, d, delta = 0, 0, 0, 0
        for interval in re.findall("[0-9]*:[0-9]*", formula):
            sep = interval.index(":")
            a, b = int(interval[:sep]), int(interval[sep+1:])
            d = max(d, b)
            delta = max(delta, b-a)
        c_u = delta * ((n+1)*(d-1)+1)**delta * (n+1) * d
        if c_u == 0:
            c_u = 1

        # print("a:", a, "b:", b, "delta:", delta, "d:", d)

        log_complexity = (delta**l) * log(c_u) + (delta**(l+1)) * log(l+1)
        return log_complexity
    

    def show_subformula(self, formula, regexp, west_regexp, n):
        w = FormulaWindow(formula, regexp, west_regexp, n)
        w.show()


    def show_cfg(self):
        self.cfg_window = QWidget()
        self.cfg_window.setWindowTitle("Context Free Grammar for WEST")
        label = QTextEdit(self.cfg_window)
        label.setReadOnly(True)
        label.setHtml(grammar_html)
        font = label.document().defaultFont()  # or another font if you change it
        fontMetrics = QFontMetrics(font)  # a QFontMetrics based on our font
        textSize = fontMetrics.size(0, label.toPlainText())

        textWidth = 2*int(textSize.width() + 20)  # constant may need to be tweaked
        textHeight = 2*int(textSize.height()/3 + 30)  # constant may need to be tweaked

        label.setFont(FONT)
        label.resize(textWidth, textHeight)
        self.cfg_window.setMaximumSize(textWidth, textHeight)
        self.cfg_window.show()


app = QApplication(sys.argv)

window = MainWindow()
window.show()

app.exec()



# TODO: 
# timeline
# grouping by time variables
# refrain from display outputs that are too large: compute complexity and find a nice bound
# formula rewriting, checking trivial intersections/unions, temporal logic vacuity

# TO DISCUSS
# Alternate input grammar 
#   have to parse input anyways to compute complexity so might as well have a nicer grammar
# Backbone display 

