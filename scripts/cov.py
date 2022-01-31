import csv
import sys
import os
import elftools as pyelf
from elftools.elf.elffile import ELFFile
from collections import defaultdict

import matplotlib.pyplot as plt

def decode_file_line(dwarfinfo, address):
    for CU in dwarfinfo.iter_CUs():
        lineprog = dwarfinfo.line_program_for_CU(CU)
        line_program = dwarfinfo.line_program_for_CU(CU)
        filename = line_entry_mapping(line_program)
        prevstate = None
        for entry in lineprog.get_entries():
            if entry.state is None:
                continue
            if prevstate and prevstate.address <= address < entry.state.address:
                line = prevstate.line
                return filename, line
            if entry.state.end_sequence:
                prevstate = None
            else:
                prevstate = entry.state
    return None, None

def line_entry_mapping(line_program):
    filename_map = defaultdict(int)
    lp_entries = line_program.get_entries()
    for lpe in lp_entries:
        if not lpe.state or lpe.state.file == 0:
            continue
        filename = lpe_filename(line_program, lpe.state.file)
    return filename
def lpe_filename(line_program, file_index):
    lp_header = line_program.header
    file_entries = lp_header["file_entry"]
    file_entry = file_entries[file_index - 1]
    dir_index = file_entry["dir_index"]
    if dir_index == 0:
        return file_entry.name.decode()
    directory = lp_header["include_directory"][dir_index - 1]
    return os.path.join(directory, file_entry.name).decode()

if __name__ == "__main__":
    pcDict = {}
    lineDict = {}
    with open("output.txt") as file:
        reader = csv.reader(file)
        for entry in reader:
            val = entry[0]
            if not val in pcDict.keys():
                pcDict[val] = 1
            else:
                pcDict[val] +=1
    with open("/scratch/alex/git/TGC-VP-Internal/fw/hello-world/hello", mode = "rb") as file:
        elffile = ELFFile(file)
        if not elffile.has_dwarf_info():
            print("No Debug information found, use -g as a g++ option")
            sys.exit(-1)
        for key in pcDict.keys():
            hexkey = (int("0x"+key, 0))
            decoding = decode_file_line(elffile.get_dwarf_info(),hexkey)
            if decoding != (None,None):
                lineDict[key] = decoding
    sortedDict = {} 
    for key in lineDict.keys():
        filename = lineDict[key][0]
        line = lineDict[key][1]
        count = pcDict[key]
        if filename not in sortedDict.keys():
            sortedDict[filename] = [(line,count)]
        else:
            if (line,count) not in  sortedDict[filename]:
                sortedDict[filename].append((line,count))
    
    with open ("/scratch/alex/git/TGC-VP-Internal/coverage.info", "w") as f:
        for key in sortedDict.keys():
            pathToFile = "/scratch/alex/git/TGC-VP-Internal/fw/hello-world" #THIS IS HARDCODED, TODO GET PATH
            f.write(f"SF:{os.path.normpath(os.path.join(pathToFile,key))}\n")
            for each in sortedDict[key]:
                f.write(f"DA:{each[0]},{each[1]}\n")
            f.write("end_of_record\n")
            
    
        