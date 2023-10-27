import sys

instructions = {
    'nop': (0x00, 0),
    'add': (0x01, 0),
    'sub': (0x02, 0),
    'mul': (0x03, 0),
    'l1i': (0x04, 1),
    'l2i': (0x05, 1),
    'mrx': (0x06, 1),
    'cmp': (0x07, 0),
    'mov': (0x08, 2),
    'jif': (0x09, 1),
    'jiz': (0x0A, 1),
    'jic': (0x0B, 1),
    'jio': (0x0C, 1),
    'mvm': (0x0D, 0),
    'ldm': (0x0E, 1),
    'ldr': (0x0F, 0),
}

def assemble(input_file, output_file):
    with open(input_file, 'r') as asm_file, open(output_file, 'wb') as hex_file:
        for line in asm_file:
            line = line.strip()
            if not line:
                continue

            tokens = line.split()
            opcode = instructions.get(tokens[0].lower())
            if opcode is None:
                raise ValueError(f"Invalid instruction: {tokens[0]}")

            hex_file.write(bytes([opcode[0]]))

            if opcode[1] == 1:
                argument = int(tokens[1])
                if argument > 0xFF:
                    raise ValueError("Argument out of range (0-255)")
                hex_file.write(bytes([argument]))
            elif opcode[1] == 2:
                arg1 = int(tokens[1])
                arg2 = int(tokens[2])
                if arg1 > 0xFF or arg2 > 0xFF:
                    raise ValueError("Arguments out of range (0-255)")
                hex_file.write(bytes([arg1, arg2]))

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("error input files name missing")
        exit()
    input_file = sys.argv[1]
    output_file = 'input.hex'
    assemble(input_file, output_file)
    print(f'Assembled {input_file} to {output_file}')
