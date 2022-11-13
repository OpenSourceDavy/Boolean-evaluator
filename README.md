# Boolean-evaluator
a robust evaluator program that gives result based on input fomula and value assignment

## example:

input: 1 * (-VAr + ((0+VAr) * -b2)) ; VAr: 1, b2: 0, c:1, c:0
output: Error: contradicting assignment

input: 1 * (VAr + -((0 + ---c) * b2)) ; VAr: 1, b2: 0, c:1
output: 1
