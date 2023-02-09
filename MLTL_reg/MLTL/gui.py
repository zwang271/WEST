import sys
import os
from PyQt5.QtWidgets import * 
from PyQt5.QtGui import * 
import re
from random import choice
from time import sleep


CWD = os.getcwd()
FONT = QFont('Times', 15)


def gen_reg(w_reg):
    output = ""
    for c in w_reg:
        if c == "s":
            output += choice(["0", "1"])
        else:
            output += c
    return output


def process_file():
    # definte rewrite mapping
    REWRITE = {
        ":": ", ",
        ">": " -> ",
        "v": " v ",
        "&": " & ",
        "=": " = "
    }
    # read in formula from file
    formula = []
    with open("./west_output/formula.txt") as file:
        for line in file:
            line = line.rstrip()
            f = ""
            for i, char in enumerate(line):
                if i in [0, len(line)-1]:
                    # pass
                    f += char
                elif char in REWRITE.keys():
                    f += REWRITE[char]
                else:
                    f += char
            formula.append(f)
    

    # read number of variables n and timesteps t from file
    formula_info = {}
    with open("./west_output/formula_info.txt") as file:
        for i, line in enumerate(file):
            if i == 0:
                formula_info["n"] = int(line.rstrip())
            if i == 1:
                formula_info["t"] = int(line.rstrip())
    

    # construct regular expression from file
    regexp = []
    west_regexp = []
    with open("./west_output/regexp.txt") as file:
        for line in file:
            temp = line.rstrip()
            west_regexp.append(temp)
            temp = temp.replace("s", "[01]")
            regexp.append(temp) 
    
    return formula, formula_info, regexp, west_regexp

class MainWindow(QMainWindow):

    def __init__(self):
        super(MainWindow, self).__init__()
        
        # Process text files
        formula, formula_info, regexp, west_regexp = process_file()
        formula = formula[0] # TODO: ADD FUNCTIONALITY TO DISPLAY SUBFORMULAS
        self.formula = formula
        self.n = formula_info["n"]
        self.t = formula_info["t"]
        self.regexp = regexp
        self.west_regexp = west_regexp


        # Displaying the GUI
        self.setWindowTitle("My App")
        layout1 = QVBoxLayout() # overall interface
        layout2 = QGridLayout() # variable toggles
        layout3 = QHBoxLayout() # computation display
        layout4 = QGridLayout() # west regex display

        # Configuring layout2
        reset_button = QPushButton("reset")
        reset_button.clicked.connect(self.reset_toggles)
        layout2.addWidget(reset_button)

        for i in range(self.n):
            widget = QLabel("p" + str(i))
            widget.setFont(FONT)
            layout2.addWidget(widget, i+1, 0)

        for i in range(self.t):
            widget = QLabel(str(i))
            widget.setFont(FONT)
            layout2.addWidget(widget, 0, i+1)

        self.variable_toggle = {}
        for var in range(self.n):
            self.variable_toggle[var] = {}
            for time in range(self.t):
                widget = QCheckBox()
                widget.toggled.connect(self.update_computation)
                self.variable_toggle[var][time] = widget
                layout2.addWidget(self.variable_toggle[var][time], var+1, time+1)


        # Configuring layout1
        self.formula_label = QLabel("MLTL Formula: " + formula)
        self.formula_label.setFont(FONT)

        layout1.addWidget(self.formula_label)

        self.computation_label = QLabel()
        self.computation_label.setFont(FONT)
        self.rand_computation_button = QPushButton("Rand Comp")
        self.rand_computation_button.clicked.connect(lambda state, x=None: self.rand_comp(x))
        layout3.addWidget(self.computation_label)
        layout3.addWidget(self.rand_computation_button)

        layout1.addLayout(layout3) # add computation layout
        layout1.addLayout(layout2) # add variable toggle layout

        self.reg_labels = []
        for i, reg in enumerate(self.west_regexp):
            label = QLabel(reg)
            label.setFont(FONT)
            self.reg_labels.append(label)
            button = QPushButton("Rand Comp")
            button.clicked.connect(lambda state, x=reg: self.rand_comp(x))
            layout4.addWidget(label, i, 0)
            layout4.addWidget(button, i, 1)
        layout1.addLayout(layout4)
        
        self.update_computation()
        

        # Define main widget of the gui
        widget = QWidget()
        widget.setLayout(layout1)
        self.setCentralWidget(widget)
    
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
        if regexp is None: 
            comp = gen_reg(choice(self.west_regexp))
        else:
            comp = gen_reg(regexp)

        var, time = 0, 0 # keep track of what variable and time
        for c in comp:
            if c == "0":
                self.variable_toggle[var][time].setChecked(False)
            elif c == "1": 
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


app = QApplication(sys.argv)

window = MainWindow()
window.show()

app.exec()

