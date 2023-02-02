import sys
import os
from PyQt5.QtWidgets import * 
from PyQt5.QtGui import * 
import re


CWD = os.getcwd()
FONT = QFont('Times', 15)

def process_file():
    # read in formula from file
    with open("./west_output/formula.txt") as file:
        formula = [line.rstrip() for line in file]
    
    # read number of variables n and timesteps t from file
    formula_info = {}
    with open("./west_output/formula_info.txt") as file:
        for i, line in enumerate(file):
            if i == 0:
                formula_info["n"] = int(line.rstrip())
            if i == 1:
                formula_info["t"] = int(line.rstrip())
    
    # construct regular expression from file
    regexp = ""
    with open("./west_output/regexp.txt") as file:
        for line in file:
            temp = line.rstrip()
            temp = temp.replace("s", "[01]")
            regexp += temp + "|"
    regexp = regexp[:-1]
    print(regexp)
    
    return formula, formula_info, regexp

class MainWindow(QMainWindow):

    def __init__(self):
        super(MainWindow, self).__init__()
        
        # Process text files
        formula, formula_info, regexp = process_file()
        formula = formula[0] # TODO: ADD FUNCTIONALITY TO DISPLAY SUBFORMULAS
        self.formula = formula
        self.n = formula_info["n"]
        self.t = formula_info["t"]
        self.regexp = regexp


        # Displaying the GUI
        self.setWindowTitle("My App")
        layout1 = QVBoxLayout()
        layout2 = QGridLayout()


        # Configuring layout2
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

        self.computation_label = QLabel()
        self.update_computation()
        self.computation_label.setFont(FONT)

        layout1.addWidget(self.formula_label)
        layout1.addWidget(self.computation_label)
        layout1.addLayout(layout2)
        
        
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

        if re.match(self.regexp, comp):
            self.formula_label.setStyleSheet("background-color: lightgreen")
        else:
            self.formula_label.setStyleSheet("background-color: red")



app = QApplication(sys.argv)

window = MainWindow()
window.show()

app.exec()

