# Author: Zili Wang
# Last updated: 01/19/2024

from PyQt5.QtWidgets import \
    QMainWindow, QVBoxLayout, QHBoxLayout, \
    QLineEdit, QPushButton, QCheckBox, QLabel, QWidget, \
    QTextEdit, QTabWidget, QScrollArea, QGridLayout , QFrame
from PyQt5.QtGui import QFont, QFontMetrics
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import Qt
import sys
import subprocess
from WEST.gui_utils import *
import re
from random import choice, randint
from dd import _bdd

class MainWindow(QMainWindow):
    # Main window for WEST MLTL Formula Validation Tool
    def __init__(self, font, initial_text=""):
        super().__init__()
        self.font = font
        self.setWindowTitle("WEST MLTL Formula Validation Tool")
        self.resize(800, 400)
        # set background color
        self.init_ui(initial_text)

    # Initialize the user interface
    def init_ui(self, initial_text):
        layout = QHBoxLayout()
        self.formula_explorer_layout = QVBoxLayout()
        main_layout = QVBoxLayout()
        self.main_layout = main_layout

        # Create a vertical separator
        separator = QFrame()
        # separator.setStyleSheet("QFrame { background-color: #CBDCF7; }")  # Set the color of the separator
        separator.setFrameShape(QFrame.VLine)  # Set the frame shape to a vertical line
        separator.setFrameShadow(QFrame.Sunken)  # Give it a sunken effect to make it more visible
        separator.setLineWidth(0)  # Set line width (optional, depending on the desired style)
        separator.setFixedWidth(10)  # Set the fixed width of the separator

        # add all sublayouts to layout
        layout.addLayout(main_layout)
        layout.addWidget(separator)  # Add the separator as a widget
        layout.addLayout(self.formula_explorer_layout)

        # User input field
        self.input_line = self.create_input_line(initial_text)
        run_button = self.create_button("Run", self.parse_formula,
                                        "Click to run WEST on formula.")
        cfg_button = self.create_button("Grammar", self.show_cfg, 
                                        "Click to view full context free grammar of input.")
        input_layout = QHBoxLayout()
        input_layout.addWidget(self.input_line)
        input_layout.addWidget(run_button)
        input_layout.addWidget(cfg_button)
        main_layout.addLayout(input_layout)

        # Checkboxes
        self.optimize_box = self.create_checkbox("Optimize Bits", False,
                                             "WARNING: This recompiles binary files, and has a few seconds of overhead.")
        self.rest_box = self.create_checkbox("Apply REST", False,
                                             "Not recommended, computationally expensive.")
        toggle_layout = QHBoxLayout()
        toggle_layout.addWidget(self.optimize_box)
        toggle_layout.addWidget(self.rest_box)
        main_layout.addLayout(toggle_layout)

        # Output field
        self.out_field = QVBoxLayout()
        self.out_text = QLabel("")
        self.out_text.setFont(self.font)
        self.unexpected_formula = QLabel("")
        self.unexpected_formula.setFont(self.font)
        self.out_field.addWidget(self.out_text)
        self.out_field.addWidget(self.unexpected_formula)
        main_layout.addLayout(self.out_field)

        # Subformula layout is a scrollable layout
        self.scrollable_subformula_layout = QVBoxLayout()
        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        self.subformula_layout = QVBoxLayout()
        scroll_content = QWidget()
        scroll_layout = QVBoxLayout(scroll_content)
        scroll_layout.addLayout(self.subformula_layout)
        scroll_area.setWidget(scroll_content)
        self.scrollable_subformula_layout.addWidget(scroll_area)
        main_layout.addLayout(self.scrollable_subformula_layout)

        # Fill the rest of the layout with a stretch
        main_layout.addStretch()

        central_widget = QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)
        # resize window to fit contents
        self.adjustSize()

    # Helper functions to create UI elements
    def create_input_line(self, initial_text):
        input_line = QLineEdit()
        input_line.setMaxLength(100)
        input_line.setPlaceholderText("Type in a MLTL formula and press enter.")
        input_line.setFont(self.font)
        input_line.returnPressed.connect(self.parse_formula)
        input_line.setText(initial_text)
        return input_line

    # Helper functions to create a button
    def create_button(self, text, callback, tooltip=None):
        button = QPushButton(text)
        button.setFont(self.font)
        button.pressed.connect(callback)
        if tooltip:
            button.setToolTip(tooltip)
        return button

    # Helper functions to create a checkbox
    def create_checkbox(self, label, is_checked, tooltip=None):
        checkbox = QCheckBox(label)
        checkbox.setChecked(is_checked)
        checkbox.setFont(self.font)
        if tooltip:
            checkbox.setToolTip(tooltip)
        return checkbox

    # Parse the formula and run WEST
    def parse_formula(self):
        # clear subformula layout
        for i in reversed(range(self.subformula_layout.count())):
            self.subformula_layout.itemAt(i).widget().setParent(None)
        # clear formula_explore_layout
        for i in reversed(range(self.formula_explorer_layout.count())):
            self.formula_explorer_layout.itemAt(i).widget().setParent(None)
        # clear output text
        self.out_text.setText("")

        # Check if formula is valid
        self.formula = self.input_line.text()
        valid, error = check_wff(self.formula)
        if not valid:
            self.out_text.setText(error)
            self.formula = None
            return
        self.run_west()
    
    # Run WEST on the formula and update the output text
    def run_west(self):
        # check if windows
        if sys.platform == "win32":
            subprocess.run(f".\\west.exe \"{self.formula}\"", 
                           shell=True)
        else: # linux or mac
            subprocess.run(f"./west \'{self.formula}\'",
                            stdout=subprocess.DEVNULL, 
                            stderr=subprocess.DEVNULL, 
                            shell=True)
        # read nnf formula from first line of ./output/output.txt
        with open("./output/output.txt", 'r') as f:
            self.nnf_formula = f.readline().strip()
            self.nnf_formula = pretty_display(self.nnf_formula)
        self.set_output_text()
        self.update_subformula_layout()

    # Set the output text
    def set_output_text(self):
        # set output text
        optimize_warning = "Bit optimization turned on. \n"
        optimize_message =  optimize_warning if self.optimize_box.isChecked() else ""
        rest_warning = "WARNING: using REST can be very computationally expensive!\n"
        rest_message = rest_warning if self.rest_box.isChecked() else ""
        nnf_message = ""
        nnf_message = f"Formula: {self.nnf_formula}\n"
        self.out_text.setText(optimize_message + 
                              rest_message + 
                              nnf_message)
        # set unexpected formula text
        self.unexpected_formula.setText("Unexpected Formula?\n\n\nPlease select a subformula to explore:")
        self.unexpected_formula.setToolTip("WEST automatically converts input to negation normal form (NNF). \nIf your input is already in NNF and this is still not the formula you expect, \n Try adding more parenthesis to disambiguate the formula.\n")

        # resize window to fit contents
        self.adjustSize()

    # Read in subformulas and regexes from ./output/subformulas.txt
    def read_subformulas(self):
        # read in subformulas and regexes from ./output/subformulas.txt
        # store as key value pairs in self.subformulas
        self.subformulas = {}
        with open("./output/subformulas.txt", 'r') as f:
            lines = f.read()
            groups = [x for x in lines.split("\n\n") if x]
            for group in groups:
                lines = group.split("\n")
                formula = lines[0] 
                regexp = lines[1:]

                # check if formula is a substring of nnf_formula
                if formula.replace(" ", "") not in self.nnf_formula.replace(" ", ""):
                    continue
                if formula.replace(" ", "")[1:-1] not in self.nnf_formula.replace(" ", ""):
                    continue

                formula = pretty_display(formula)
                formula = formula.replace("&", "&&")
                self.subformulas[formula] = regexp
        # reverse sort subformulas by length
        self.subformulas = dict(sorted(self.subformulas.items(), key=lambda x: len(x[0]), reverse=True))

    # Update subformula layout based on subformulas in ./output/subformulas.txt
    def update_subformula_layout(self):
        # read in subformulas and regexs from ./output/subformulas.txt
        self.read_subformulas()

        # clear subformula layout
        for i in reversed(range(self.subformula_layout.count())): 
            self.subformula_layout.itemAt(i).widget().setParent(None)
        # add subformula buttons
        for formula, regexes in self.subformulas.items():
            button = self.create_button(formula, self.visualize_formula, 
                                        "Click to visualize the formula.")
            self.subformula_layout.addWidget(button)

    # Show subformula in subformula_layout, in the same window
    def visualize_formula(self):
        # display subformula window
        formula = self.sender().text()
        regexp = self.subformulas[formula]
        # clear formula_explore_layout
        for i in reversed(range(self.formula_explorer_layout.count())):
            self.formula_explorer_layout.itemAt(i).widget().setParent(None)
        window = visualizeFormulaWindow(formula, regexp, self.font)
        self.formula_explorer_layout.addWidget(window)
        self.adjustSize()

    # Show the context free grammar for WEST
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

        label.setFont(self.font)
        label.resize(textWidth, textHeight)
        self.cfg_window.setMaximumSize(textWidth, textHeight)
        self.cfg_window.show()

class visualizeFormulaWindow(QWidget):
    def __init__(self, formula, regexp, font):
        super().__init__()
        self.formula = formula.replace("&&", "&")
        self.regexp = regexp
        self.regexp_re = [re.compile(r.replace("s", "[01]")) for r in regexp]
        if len(regexp) > 0:
            self.t = max(regexp[i].count(",") + 1 for i in range(len(regexp)))
        else:
            self.t = 0
        if self.t == 0:
            self.n = 0
        elif self.t == 1:
            self.n = len(regexp[0])
        else:
            self.n = len((regexp[0].split(","))[0])
        self.var = list(set(re.findall("p[0-9]*", self.formula)))
        self.var.sort()
        self.font = font
        self.init_ui()

    def init_ui(self):
        self.layout = QVBoxLayout()
        self.setLayout(self.layout)

        # Display current formula
        self.formula_label = QLabel("MLTL Formula: " + self.formula)
        self.formula_label.setFont(self.font)
        self.layout.addWidget(self.formula_label)

        # Display current trace and option to generate random trace
        trace_layout = QHBoxLayout()
        self.trace_label = QLabel()
        self.trace_label.setFont(self.font)
        self.trace_label.setToolTip("trace")
        trace_layout.addWidget(self.trace_label)
        self.trace_help_button = QPushButton("Help")
        self.trace_help_button.setFont(self.font)
        self.trace_help_button.setToolTip("Click for explanation about string representation of traces.")
        self.trace_help_button.clicked.connect(self.show_trace_help)
        trace_layout.addWidget(self.trace_help_button) 
        self.layout.addLayout(trace_layout)

        # Options to import and export traces to and from file
        trace_io_layout = QVBoxLayout()
        import_layout = QHBoxLayout()
        self.import_button = QPushButton("Import trace")
        self.import_button.setFont(self.font)
        self.import_button.setToolTip("Click to import trace from csv file.")
        self.import_button.clicked.connect(self.import_trace)
        self.import_path = QLineEdit()
        self.import_path.setPlaceholderText("trace string or path to csv file")
        self.import_path.setFont(self.font)
        self.import_path.returnPressed.connect(self.import_trace)
        import_layout.addWidget(self.import_button)
        import_layout.addWidget(self.import_path)
        trace_io_layout.addLayout(import_layout)
        export_layout = QHBoxLayout()
        self.export_button = QPushButton("Export trace")
        self.export_button.setFont(self.font)
        self.export_button.setToolTip("Click to export trace to csv file.")
        self.export_button.clicked.connect(self.export_trace)
        self.export_path = QLineEdit()
        self.export_path.setText("trace.csv")
        self.export_path.setFont(self.font)
        self.export_path.returnPressed.connect(self.export_trace)
        export_layout.addWidget(self.export_button)
        export_layout.addWidget(self.export_path)
        trace_io_layout.addLayout(export_layout)
        self.io_announce = QLabel("")
        self.io_announce.setFont(self.font)
        trace_io_layout.addWidget(self.io_announce)
        self.layout.addLayout(trace_io_layout)


        # Buttons to generate random sat and unsat traces
        rand_button_layout = QHBoxLayout()
        self.rand_trace_button = QPushButton("Rand SAT")
        self.rand_trace_button.setToolTip("Randomly generates a satisfying computaton to the MLTL formula.")
        self.rand_trace_button.setFont(self.font)
        self.rand_trace_button.clicked.connect(lambda state, x=None: self.rand_comp(x))
        self.rand_unsat_button = QPushButton("Rand UNSAT")
        self.rand_unsat_button.setToolTip("Randomly generates a trace that does NOT satisfy the MLTL formula.")
        self.rand_unsat_button.clicked.connect(self.rand_unsat)
        self.rand_unsat_button.setFont(self.font)
        rand_button_layout.addWidget(self.rand_trace_button)
        rand_button_layout.addWidget(self.rand_unsat_button)
        self.layout.addLayout(rand_button_layout)

        # Configure variable toggling layout
        var_layout = QGridLayout()
        reset_button = QPushButton("reset")
        reset_button.setFont(self.font)
        reset_button.clicked.connect(self.reset_toggles)
        var_layout.addWidget(reset_button)
        # Display variable names
        for i in range(self.n):
            widget = QLabel(f'p{i}')
            widget.setFont(self.font)
            if f'p{i}' not in self.var:
                widget.setEnabled(False)
            var_layout.addWidget(widget, i+1, 0)
        # Display time steps
        for i in range(self.t):
            widget = QLabel(str(i))
            widget.setFont(self.font)
            var_layout.addWidget(widget, 0, i+1)
        # Display each variable at each timestep
        self.variable_toggle = {}
        for var in range(self.n):
            self.variable_toggle[var] = {}
            for time in range(self.t):
                widget = QCheckBox()
                widget.toggled.connect(self.update_trace)
                if f'p{var}' not in self.var:
                    widget.setEnabled(False)
                self.variable_toggle[var][time] = widget
                var_layout.addWidget(self.variable_toggle[var][time], var+1, time+1)
        self.layout.addLayout(var_layout)

        # Creating tabs for multiple display options
        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab2 = QWidget()
        self.tabs.addTab(self.tab1, "Regexp List")
        self.tabs.addTab(self.tab2, "Backbone Analysis")
        self.tabs.setStyleSheet('QTabBar { font-size: 15pt; font-family: Times; }')
        self.layout.addWidget(self.tabs)

        # Building scrollable layout to display regexps
        regexp_layout = QGridLayout()
        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        regexp_layout.addWidget(scroll_area)
        scroll_content = QWidget()
        scroll_layout = QGridLayout(scroll_content)
        self.reg_labels = []
        for i, reg in enumerate(self.regexp):
            label = QPushButton(reg)
            label.setFont(self.font)
            label.setToolTip("Click to generate satisfying trace that matches this particular regular expression.")
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
        self.bb_sat_label.setFont(self.font)
        bb_layout.addWidget(self.bb_sat_label)
        self.bb_unsat_label = QLabel(unsat_backbone)
        self.bb_unsat_label.setFont(self.font)
        bb_layout.addWidget(self.bb_unsat_label)
        self.tab2.setLayout(bb_layout)  

        # Update initial trace of all zeros
        self.update_trace()

    def import_trace(self):
        path = self.import_path.text()
        if path.endswith(".csv"):
            path = os.path.join(os.path.dirname(__file__), path)
            if not os.path.exists(path):
                self.io_announce.setText("File does not exist.\n")
                return
            with open(path, 'r') as f:
                trace = read_trace(f)
        else:
            trace = path.replace(" ", "").strip()
        if trace == "":
            return
        if not check_valid_trace(trace, self.n, self.t):
            self.io_announce.setText("Invalid import trace.\n")
            return
        var, time = 0, 0 # keep track of what variable and time
        for c in trace:
            if c == "0" and f'p{var}' in self.var:
                self.variable_toggle[var][time].setChecked(False)
            elif c == "1" and f'p{var}' in self.var: 
                self.variable_toggle[var][time].setChecked(True)
            elif c == ",":
                time += 1
                var = 0
                continue
            var += 1

    def export_trace(self):
        path = self.export_path.text()
        if path == "":
            path = "trace.csv"
        if not path.endswith(".csv"):
            path += ".csv"
            if os.path.exists(path):
                self.io_announce.setText("Warning: overwriting existing file.\n")
            return
        self.io_announce.setText("Trace exported to " + path + "\n")
        path = os.path.join(os.path.dirname(__file__), path)
        with open(path, 'w') as f:
            trace = self.trace_label.text().replace("trace: ", "")
            trace = trace.split(",")
            for x in trace:
                x = list(x)
                x = ",".join(x)
                f.write(x + "\n")

    def update_trace(self):
        comp = ""
        for time in range(self.t):
            for var in range(self.n):
                if self.variable_toggle[var][time].isChecked():
                    comp += "1"
                else:
                    comp += "0"
            comp += ","
        comp = comp[:-1]
        self.trace_label.setText("trace: " + comp)

        found = False
        for i, reg in enumerate(self.regexp_re):
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
        if regexp is None and len(self.regexp) > 0: 
            if self.regexp == []:
                return
            comp = gen_reg(choice(self.regexp))
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
        for w in self.regexp:
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

    def show_trace_help(self):
        self.comp_help = QWidget()
        self.comp_help.setWindowTitle("String Representation of traces")
        label = QTextEdit(self.comp_help)
        label.setReadOnly(True)
        # read html from ./src/trace_help.html
        with open("./src/trace_help.html", 'r') as f:
            label.setHtml(f.read())
        font = label.document().defaultFont()  # or another font if you change it
        fontMetrics = QFontMetrics(font)  # a QFontMetrics based on our font
        textSize = fontMetrics.size(0, label.toPlainText())

        textWidth = textSize.width() + 20  # constant may need to be tweaked
        textHeight = textSize.height() + 30  # constant may need to be tweaked

        label.resize(textWidth, textHeight)
        self.comp_help.setMaximumSize(textWidth, textHeight)
        self.comp_help.show()
   
if __name__ == '__main__':
    app = QApplication(sys.argv)  # Create QApplication instance
    app_font = QFont("Times", 16)
    initial_text = sys.argv[1] if len(sys.argv) == 2 else ""
    window = MainWindow(app_font, initial_text)
    window.show()
    sys.exit(app.exec_())
