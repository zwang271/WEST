import sys
import os
from PyQt5.QtWidgets import * 
from PyQt5.QtGui import * 
import re
from random import choice
from time import sleep
from gui_utils import * 



class FormulaWindow(QWidget):

    def __init__(self, formula, regexp, west_regexp, n):
        super().__init__()
        
        # Process inputs
        self.formula = formula
        self.t = 0
        self.n = n
        self.regexp, self.west_regexp = regexp, west_regexp
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
        self.setWindowTitle("My App")
        main_layout = QVBoxLayout() # overall interface


        # Display current formula
        self.formula_label = QLabel("MLTL Formula: " + formula)
        self.formula_label.setFont(FONT)
        main_layout.addWidget(self.formula_label)


        # Display current computation and option to generate random computation
        computation_layout = QHBoxLayout()
        self.computation_label = QLabel()
        self.computation_label.setFont(FONT)
        computation_layout.addWidget(self.computation_label)
        self.rand_computation_button = QPushButton("Rand Comp")
        self.rand_computation_button.clicked.connect(lambda state, x=None: self.rand_comp(x))
        computation_layout.addWidget(self.rand_computation_button)
        main_layout.addLayout(computation_layout)


        # Configure variable toggling layout
        var_layout = QGridLayout()
        reset_button = QPushButton("reset")
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


        # building scrollable layout to display regexps
        regexp_layout = QGridLayout()
        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        regexp_layout.addWidget(scroll_area)
        scroll_content = QWidget()
        scroll_layout = QGridLayout(scroll_content)

        self.reg_labels = []
        for i, reg in enumerate(self.west_regexp):
            # label = QLabel(reg)
            # label.setFont(FONT)
            # self.reg_labels.append(label)
            # button = QPushButton("Rand Comp")
            # button.clicked.connect(lambda state, x=reg: self.rand_comp(x))
            # layout4.addWidget(label, i, 0)
            # layout4.addWidget(button, i, 1)
            label = QPushButton(reg)
            label.setFont(FONT)
            self.reg_labels.append(label)
            label.clicked.connect(lambda state, x=reg: self.rand_comp(x))
            scroll_layout.addWidget(label, i, 0)
        scroll_area.setWidget(scroll_content)
        main_layout.addLayout(regexp_layout)     
        

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
                self.reg_labels[i].setStyleSheet("background-color: lightgreen")
            else:
                self.reg_labels[i].setStyleSheet("background-color: none")
        if not found:
            self.formula_label.setStyleSheet("background-color: red")
    

    def rand_comp(self, regexp = None):
        if regexp is None and len(self.west_regexp) > 0: 
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

    
    def reset_toggles(self):
        for time in range(self.t):
            for var in range(self.n):
                self.variable_toggle[var][time].setChecked(False)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        main_layout = QVBoxLayout()
        self.resize(800, 400)

        # Create field for user input
        self.input_line = QLineEdit()
        self.input_line.setMaxLength(100)
        self.input_line.setPlaceholderText("Type in a MLTL formula")
        self.input_line.setFont(FONT)
        self.input_line.returnPressed.connect(self.validate_formula)
        main_layout.addWidget(self.input_line)

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


    def validate_formula(self):
        formula = self.input_line.text()
        outfile = run("Wff_check", [formula])
        valid = bool(int(outfile.readline()))
    

        if (not valid):
            self.out_text.setText(f"\"{formula}\" is not a valid formula!")
            return
        

        # convert to nnf form
        nnf = run("Wff_to_Nnf", [formula]).readline()
        nnf_message = ""
        if nnf != formula.replace(" ", ""):
            nnf_message = f"Converted to Negation Normal Form: {nnf}\n"


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
        

        # Display appropriate messages to user
        self.out_text.setText(simp_message + 
                              rest_message +
                              nnf_message + 
                              "\nPlease select a subformula to explore below:")
        self.show()

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
        for i,f in enumerate(self.formula): 

            # skip all atomic propositions and their negations
            if re.match("~?p[0-9]*", f):
                continue

            # only display subformulas that are substrings of the main formula
            if f not in self.formula[-1]:
                continue


            formula_button = QPushButton(f)
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


    def show_subformula(self, formula, regexp, west_regexp, n):
        w = FormulaWindow(formula, regexp, west_regexp, n)
        w.show()


app = QApplication(sys.argv)

window = MainWindow()
window.show()

app.exec()



# TODO: 
# DONE input within gui 
# DONE scrolling
# DONE subformula support
# timeline
# grouping by time variables
# refrain from display outputs that are too large
# error handling

# Problematic examples:
# (G[0:2] (p0 v p1) v F[0:2] (p1 > p2))