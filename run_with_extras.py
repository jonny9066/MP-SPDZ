
import subprocess
import pyaes
import re


# hex_key = '0x2b7e151628aed2a6abf7158809cf4f3c'
# hex_plain = '0x6bc1bee22e409f96e93d7e117393172a'
# hex_res = '0x3ad77bb40d7a3660a89ecaf32466ef97'
hex_key = '0x00000000000000000000000000000000'
hex_plain = '0x00000000000000000000000000000000'

num_of_bits = 128
hex_base = 16
bin_key = bin(int(hex_key, hex_base))[2:].zfill(num_of_bits)
bin_plain = bin(int(hex_plain, hex_base))[2:].zfill(num_of_bits)

print(bin_key)
print(bin_plain)

cmd1 = "echo "+ "\"1 "+ " ".join(bin_key) + "\" > Player-Data/Input-P0-0"
cmd2 = "echo "+ " ".join(bin_plain) + " > Player-Data/Input-P1-0"
subprocess.run(cmd1, shell = True)
subprocess.run(cmd2, shell = True)


commands = ["make -f maketz spdz0".split(), 
            "make -f maketz spdz1".split()]
p1 = subprocess.Popen(commands[0],stdout=subprocess.PIPE, stderr=subprocess.PIPE)
p2 = subprocess.Popen(commands[1],stdout=subprocess.PIPE, stderr=subprocess.PIPE)
p1.wait()
p2.wait()

pat = re.compile(r'[10]{128}')

outs, errs = p1.communicate()
print(hex(int(pat.search(outs.decode('ascii'))[0][::-1], 2)))

b_key = int(hex_key, 16).to_bytes(16, byteorder='big')
b_plain = int(hex_plain, 16).to_bytes(16, byteorder='big')

aes = pyaes.AESModeOfOperationECB(b_key)    
ciphertext = aes.encrypt(b_plain)

# show the encrypted data
print (hex(int.from_bytes(ciphertext, byteorder='big')))

