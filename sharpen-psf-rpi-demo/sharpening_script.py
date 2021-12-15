import os

def single_threaded_120k():
    for i in range(1,10):
        print("run:", i, "\n")
        os.system("./sharpen Cactus-120kpixel.ppm 120k_output.pmm")
    return False

def single_threaded_12M():
    for i in range(1,10):
        print("run:", i, "\n")
        os.system("./sharpen Cactus-12mpixel.ppm 120k_output.pmm")
    return False

def multi_threaded_120k():
    for i in range(1,10):
        print("run:", i, "\n")
        os.system("./sharpen_grid Cactus-120kpixel.ppm 120k_output.pmm")
    return False

def multi_threaded_12M():
    for i in range(1,10):
        print("run:", i, "\n")
        os.system("./sharpen_grid Cactus-12mpixel.ppm 120k_output.pmm")
    return False

def is_done():
    return True

def main():
    done = False
    while done == False:
        print("Select Option:")
        options_text = {"0":"single-threaded 120K pixel sharpening", "1":"single-threaded 12M pixel sharpening", "2":"multi-threaded 120K pixel sharpening", "3":"multi-threaded 12M pixel sharpening", "4":"done"}
        options = { "0" : single_threaded_120k,
                    "1" : single_threaded_12M,
                    "2" : multi_threaded_120k,
                    "3" : multi_threaded_12M,
                    "4" : is_done}

        for option in options_text.keys():
            print(option, options_text[option])
        selection = str(input())

        done = options[selection]()
    return
    

if __name__ == "__main__":
	main()