from tkinter import *
from tkinter import ttk

matrix_width = 24
matrix_height = 10

buttons_matrix = [[0 for j in range(matrix_width)] for i in range(matrix_height)]

def button_pressed(button, i,j):
    if button["bg"] == "black":
        button.config(bg="white")
        buttons_matrix[i][j] = 1
    else:
        button.config(bg="black")
        buttons_matrix[i][j] = 0

def create_matrix(matrix_width,matrix_height):
    string_matrix = ""

    for i in range(0, matrix_height):
        string_matrix += "{"
        for j in range(0, matrix_width):

            if buttons_matrix[i][j] == 1:
                string_matrix += "1"
            else:
                string_matrix += "0"

            if j != matrix_width-1:
                string_matrix += ","
        
        if i == matrix_height-1:
            string_matrix += "}"
        else:
            string_matrix += "},"



    return string_matrix
    

root = Tk()
frm = ttk.Frame(root, padding=5)

# visual LED matrix grid
frm.grid()
for i in range(0, matrix_height):
    for j in range(0, matrix_width):
        button = Button(frm, width=2, height=1, bg="black")
        button.config(command=lambda b=button, row=i, col=j: button_pressed(b, row, col))
        button.grid(column=j, row=i)


frm2 = ttk.Frame(root, padding=5)
frm2.grid()

def print_grid():
    text.delete("1.0", END)
    text.insert("1.0", f"bool frame[{matrix_height}][{matrix_width}] = {{\n" + create_matrix(matrix_width, matrix_height) + "\n};")


#generate the matrix from the grid
ttk.Button(frm2, text="Generate matrix", command=print_grid).grid(column=matrix_width + 5, row=0)

#matrix output to copy paste
text = Text(frm2, width=50, height=10)
text.grid(column=matrix_width + 5, row=1)
text.insert("1.0", "[press \"Generate matrix\" to see the equivalent configuration in matrix form]")



root.mainloop()