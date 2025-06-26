from flask import Flask, jsonify, request
import subprocess
import sys


app = Flask(__name__)

@app.route('/<formula>', methods=['GET'])
def solve(formula):
    try:
        # Call the C++ binary with the formula as argument
        result = subprocess.run(
            ['./west', formula],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        if result.returncode != 0:
            return jsonify({'west error': result.stderr.strip()}), 400
        with open('./output/subformulas.txt', 'r') as f:
            output = f.read()
        # the output is separated by empty lines, separate each chunk
        subformulas = {}
        formula = f"({formula})"
        for chunk in output.strip().split('\n\n'):
            lines = chunk.strip().split('\n')
            subformula = lines[0].strip()
            if len(lines) >= 2 and \
                (subformula in formula or f"({subformula})" in formula):
                subformulas[subformula] = lines[1:]
        return jsonify(subformulas), 200

    
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    # app.run(debug=True)
    app.run(debug=True, host='0.0.0.0', port=5000) # use this for docker image